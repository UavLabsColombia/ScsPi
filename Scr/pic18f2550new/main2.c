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
// se calcula el promedio del peso
float promedio = 0.0f;

boolean primera_etapa = false;
boolean segunda_etapa = false;


// Se define el puerto ADC a utilizar y sus pines.
void define_puertos() {

    /* Define el pin 7 y 0 como entrada y el resto del puerto como salidas
    3 y 4 leds indicadores de caja
    0  entrada finales de carrera
    2 banda 1
    bin: 1110 0001
    */
    
    set_tris_b(0xE1);
    output_low(PIN_B1);

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
void parar_planta(){
       output_high(PIN_B1);

       // *************  detiene lo que esta funcionando 
      // if(primera_etapa){
          // se detiene la banda 1
           output_low(PIN_B2);
      // }else{
          // se detiene las bandas finales
          output_high(PIN_A5);
     //  }

      while(!input(PIN_B5)){
         delay_ms(100);
      }

         if(primera_etapa){
          // se reanuda la primera banda
           output_high(PIN_B2);
       }else{
          // se reanuda las bandas finales
              if(input(PIN_B0)){
                 // esta en las bandas
            output_low(PIN_A5);
            delay_ms(3500);
            output_high(PIN_A5);
              }
       }

         output_low(PIN_B1);
}

// ************************* interupciones ************************
// iterupcion de cambio en el puerto b
#int_rb
void int_rb_pulsadores(){
   //boton de rest
   if(input(PIN_B7)){
      printf("7.0000");
   }
   // stop 6
   if(input(PIN_B6)){
         parar_planta();
   }
}

//interrupcion de llegada de datos
#int_rda
void rb_isr() {
    if (kbhit()) {
        dato_recibido = getc();
        delay_ms(4);
        if(dato_recibido == '1'){
           parar_planta();
        }
        dato_recibido = '0';
    }
}

float leerADC() {
    return resolucion * (float)(READ_ADC());
}

void subir_ascensor() {
    //subir ascensor
    output_high(PIN_A1);
    output_low(PIN_A2);

    //inicio de la subida con fuerza
    for (i = 0; i < 40; i++) {
        output_high(PIN_A3);
        delay_ms(9);
        output_low(PIN_A3);
        delay_ms(11);
    }

    if (promedio >= 3.5) {
        //cajas pesadas
        while (!input(PIN_B0)) {
            output_high(PIN_A3);
            delay_ms(8);
            output_low(PIN_A3);
            delay_ms(12);
        }
    } else {
        //cajas livianas
        while (!input(PIN_B0)) {
            output_high(PIN_A3);
            delay_ms(6);
            output_low(PIN_A3);
            delay_ms(14);
        }
    }
}

void bajar_ascensor() {

    // bajar ascensor
    output_low(PIN_A1);
    output_high(PIN_A2);

    for (i = 0; i < 40; i++) {
        output_high(PIN_A3);
        delay_ms(12);
        output_low(PIN_A3);
        delay_ms(8);
    }

    for (i = 0; i < 80; i++) {
        output_high(PIN_A3);
        delay_ms(5);
        output_low(PIN_A3);
        delay_ms(15);
    }

    while (!input(PIN_B0)) {
        output_high(PIN_A3);
        delay_ms(3);
        output_low(PIN_A3);
        delay_ms(17);
    }
}

void main() {
    //habilita las interrupciones por serial
    enable_interrupts(int_rda);
    //habilita las interrupciones puerto b
    enable_interrupts(int_rb);
    
    
    enable_interrupts(global);
    
    //flanco de subida
    ext_int_edge(L_TO_H);
    
    // configura los puertos
    define_puertos();

    // se configura el canal del adc
    SET_ADC_CHANNEL(0);

    while (true) {

       primera_etapa = true;

        // inicia la banda 1
        output_high(PIN_B2);
        
             
       
       // se apagan los led indicadores de caja
        output_low(PIN_B3);
        output_low(PIN_B4);

        // esta en este metodo hasta que pese algo
        while (true) {
            delay_ms(200);
            if (leerADC() > 1.0f) {
                break;
            }
        }

        // se detiene la banda 1
        output_low(PIN_B2);

        // espera 3s mientras cuadra la caja
        delay_ms(3000);

        // ******************* peso de la caja

        for (i = 0; i < 7; i++) {
            delay_ms(300);
            promedio += leerADC();
        }
        promedio /= 7;


        // se envia el peso por r232
        printf("%1.4f", promedio);

        // enciende los led indicadores 
        if (promedio >= 3.5) {
            output_high(PIN_B3);
        }else{
            output_high(PIN_B4);
        }
        

        //**************** servo 1 (pasa a la otra banda) ***************
        delay_ms(500);

        // brazo 90 grados
        for (i = 0; i < 15; i++) {
            output_high(PIN_C2);
            delay_us(1500);
            output_low(PIN_C2);
            delay_us(18500);
        }

        for (i = 0; i < 15; i++) {
            output_high(PIN_C2);
            delay_us(500);
            output_low(PIN_C2);
            delay_us(19500);
        }
        // ***************  fin del servo 1 *****************++

        // cambia los estados de las etapas
        primera_etapa = false;
        segunda_etapa = true;


        // corre las 3 bandas del rele
        output_low(PIN_A5);
        delay_ms(3500);
        output_high(PIN_A5);

        /*
        ENABLE RA3
        datos:
              A1
              A2
        */
        subir_ascensor();

        delay_ms(500);

        // condicional para pesar las cajas 
        if (promedio >= 3.5) {
            // bazo cajas pesadas
            for (i = 0; i < 60; i++) {
                output_high(PIN_C1);
                delay_us(500);
                output_low(PIN_C1);
                delay_us(19500);
            }

            for (i = 0; i < 60; i++) {
                output_high(PIN_C1);
                delay_us(2500);
                output_low(PIN_C1);
                delay_us(17500);
            }

        } else {
            // bazo cajas livianas
            for (i = 0; i < 60; i++) {
                output_high(PIN_C0);
                delay_us(500);
                output_low(PIN_C0);
                delay_us(19500);
            }

            for (i = 0; i < 60; i++) {
                output_high(PIN_C0);
                delay_us(2500);
                output_low(PIN_C0);
                delay_us(17500);
            }
        }

        // corre las 3 bandas del rele
        output_low(PIN_A5);
        delay_ms(3500);
        output_high(PIN_A5);

        bajar_ascensor();

        segunda_etapa = false;
    }
}
