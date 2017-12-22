// Convertidor de Analogo a Digital
#include <18F2550.h>

#fuses xt, nowdt, noprotect, put, CPUDIV1
// Numero de bits a utilizar sobre el convertidor analogo
#device adc = 10
#use delay(internal = 8000000)
#use rs232(baud = 9600, bits = 8, parity = N, xmit = PIN_C6, RCV = PIN_C7, stop=1, ERRORS)

#use fast_io(b)
#use fast_io(a)
#use fast_io(c)

// ****************  Variables Globales  *******************

// calculo de la resulucion en la salida
float resolucion = (float) 5 / (float) 1023;
char dato_recibido = '0';
// se usa en los bucles 
int i;

// Se define el puerto ADC a utilizar y sus pines.
void define_puertos() {

    // Define el pin como entrada y el resto del puerto como salidas
    //bin: 1000 0000
    set_tris_b(0x80);

    // define la entrada 0 del puerto a como entrada
    //bin: 0000 0001
    set_tris_a(0x01);
    output_high(PIN_A5);
    output_low(PIN_A3);

    // se define el puerto c como salida
    set_tris_c(0x00);
    output_c(0x00);

    // Define como entradas analoga el puerto AN0
    SETUP_ADC_PORTS(AN0);

    // Define el relog interno para el uso del ADC
    SETUP_ADC(ADC_CLOCK_INTERNAL);
}

//interrumcion de llegada de datos
#int_rda
void rb_isr() {
    if (kbhit()) {
        dato_recibido = getc();
        delay_ms(4);
    }
}

float leerADC(){
    return resolucion * (float) (READ_ADC());
}


void main() {
    output_high(PIN_B2);
    //habilita las interrupciones por serial
    enable_interrupts(int_rda);
    enable_interrupts(global);

    // configura los puertos
    define_puertos();

    // se configura el canal del adc
    SET_ADC_CHANNEL(0);
    
    //configuracion del pwm
    //setup_timer_2(T2_DIV_BY_4 ,125 ,1);
    //setup_ccp1(CCP_PWM);
    

    while (true) {
        
        // esta en este metodo hasta que pese algo
        while(true){
            delay_ms(200);
            if(leerADC() > 1.0f){
                break;
            }
        }
        
        // se detiene la banda
        output_low(PIN_B2);

        // espera 1s mientras cuadra la caja
        delay_ms(1000);

        // se calcula el promedio del peso
        float promedio = 0.0f;

        for(i = 0; i < 5; i++){
            delay_ms(300);
            promedio += leerADC();
        }
        promedio /= 5;
        
        // se envia el peso por r232
        printf("%1.4f", promedio);

        //**************** servo 1 (pasa a la otra banda) ***************
        delay_ms(1000);
      
      // brazo 90 grados
        for(i = 0; i < 15; i++){
            output_high(PIN_C2);
            delay_us(1500);
            output_low(PIN_C2);
            delay_us(18500);
        }
       
       
       for(i = 0; i < 15; i++){
            output_high(PIN_C2);
            delay_us(500);
            output_low(PIN_C2);
            delay_us(19500);
        }
        
        
        // corre las 3 bandas del rele
        output_low(PIN_A5);
        delay_ms(1000);
        output_high(PIN_A5);
        
        /*
        ENABLE RA3
        datos:
              A1
              A2
        */
        
      output_high(PIN_A1);
      output_low(PIN_A2);
        
        
        for(i = 0; i < 200; i++){
            output_high(PIN_A3);
            delay_ms(6);
            output_low(PIN_A3);
            delay_ms(14);
        }
        
      output_low(PIN_A1);
      output_high(PIN_A2);
      
         for(i = 0; i < 150; i++){
            output_high(PIN_A3);
            delay_ms(5);
            output_low(PIN_A3);
            delay_ms(15);
        }
        
       
    // bazo cajas pesadas
        for(i = 0; i < 60; i++){
            output_high(PIN_C1);
            delay_us(500);
            output_low(PIN_C1);
            delay_us(19500);
        }
        
        for(i = 0; i < 60; i++){
            output_high(PIN_C1);
            delay_us(2500);
            output_low(PIN_C1);
            delay_us(17500);
        }
        
        // bazo cajas livianas
        for(i = 0; i < 60; i++){
            output_high(PIN_C0);
            delay_us(500);
            output_low(PIN_C0);
            delay_us(19500);
        }
        
        for(i = 0; i < 60; i++){
            output_high(PIN_C0);
            delay_us(2500);
            output_low(PIN_C0);
            delay_us(17500);
        }
        
        output_high(PIN_B2);
        
    }
}