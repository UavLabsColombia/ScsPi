/* 
    Created on : 01/12/2017
    Author     :
                  script-cat@outlook.com
                  uavlabs.org
    
    Programa para un micro controlador 18F2550 el cual se encarga del control de una
    planta clasificadora de paquetes.
*/

#include <18F4550.h>
#fuses xt, nowdt, noprotect, put, CPUDIV1

// Numero de bits a utilizar en el convertidor analogo a digital
#device adc = 10
// se define el uso del reloj interno a 8Mhz
#use delay(internal = 8000000)
//incluye laa libreria modificada para majear la lcd en por el puerto D
#include "Mi_LCD.c"

// se define la comunicacion serial a 9600 baudios, sin paridad y un bit de stop
#use rs232(baud = 9600, bits = 8, parity = N, xmit = PIN_C6, RCV = PIN_C7, stop=1, ERRORS)

#use fast_io(d)// lcd
#use fast_io(b)
#use fast_io(a)
#use fast_io(c)


// ****************  Variables Globales  *******************

// calculo de la resulucion para una salida maxima de 5
float resolucion = (float) 5 / (float) 1023;

// se guarda el ultimo caracter que se recibe por serial
char dato_recibido = '0';

// se usa en los bucles
int i;

// se calcula el promedio del peso
float promedio = 0.0f;

// bandera que avisa si la caja paso la primera banda
boolean primera_etapa = false;

//conteo de cajas
int cajasLivianas = 0;
int cajasPesadas = 0;
    
    
// se define una simulacion de un puerto pwm de 50 hz
void duty_50(int puerto, int16 time_on, int ciclos){
    for (i = 0; i < ciclos; i++) {
        output_high(puerto);
        delay_us(time_on);
        output_low(puerto);
        delay_us(20000 - time_on);
    }
}

void define_puertos() {
    //lcd
    set_tris_d(0x00);
    
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
    
    // se configura el canal del adc
    SET_ADC_CHANNEL(0);
}

void correr_bandas_finales(){
    // corre las 3 bandas del rele por 3.5seg
    output_low(PIN_A5);
    delay_ms(3500);
    output_high(PIN_A5);
}

void parar_planta(){
    // se enciende el led indicador de paro
    output_high(PIN_B1);
    
    // se detiene la banda 1
    output_low(PIN_B2);
    // se detiene las bandas finales
    output_high(PIN_A5);
    
    // se mantiene en el bucle hasta que se presione el boton de start
    while(!input(PIN_B5)){
        delay_ms(100);
    }
    
    lcd_putc("\f"); //limpia la lcd
    printf(lcd_putc,"Start");
    
    if(primera_etapa){
        // se reanuda la primera banda
        output_high(PIN_B2);
    }else{
        // se encomprueba los finales de carrera del elevador
        if(input(PIN_B0)){
            // esta en las bandas finles, activamos el rele de las badas
            correr_bandas_finales();
        }
    }
    
    // apagado del led indicador de paro
    output_low(PIN_B1);
}

// ************************* interupciones ************************

// iterupcion de cambio en el puerto b
#int_rb
void int_rb_pulsadores(){
    //boton de rest b7
    if(input(PIN_B7)){
    
        lcd_putc("\f"); //limpia la lcd
        printf(lcd_putc,"Reset");
        printf("7.0000");
        cajasLivianas = 0;
        cajasPesadas = 0;
    }
    // stop b6
    if(input(PIN_B6)){
        lcd_putc("\f"); //limpia la lcd
        printf(lcd_putc,"Stop");
        parar_planta();
    }
}

//interrupcion de llegada de datos
#int_rda
void rb_isr() {
    // comprueba si hay llegada de datos
    if (kbhit()) {
        dato_recibido = getc();
        delay_ms(4);
        
        if(dato_recibido == '1'){
            parar_planta();
        }
        
        dato_recibido = '0';
    }
}

// retorna un valor de 0 a 5 con respecto al valor medido en el pin A0
float leerADC() {
    return resolucion * (float) (READ_ADC());
}

void subir_ascensor() {
    //ENABLE RA3,  datos: A1, A2
    output_high(PIN_A1);
    output_low(PIN_A2);
    
    //inicio de la subida con fuerza
    for (i = 0; i < 40; i++) {
        output_high(PIN_A3);
        delay_ms(9);
        output_low(PIN_A3);
        delay_ms(11);
    }
    
    if (promedio >= 2.2) {
        //cajas pesadas
        while (!input(PIN_B0)) {
                output_high(PIN_A3);
                delay_ms(9);
                output_low(PIN_A3);
                delay_ms(11);
            }
    } else {
        //cajas livianas
        while (!input(PIN_B0)) {
            output_high(PIN_A3);
            delay_ms(8);
            output_low(PIN_A3);
            delay_ms(12);
        }
    }
}

void bajar_ascensor() {
    //ENABLE RA3,  datos: A1, A2
    
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
    //configuracion inicial del la lcd
    lcd_init();
    
          lcd_putc("\f"); //limpia la lcd
    printf(lcd_putc,"Wellcome ;)");
    
    //habilita las interrupciones por serial
    enable_interrupts(int_rda);
    //habilita las interrupciones puerto b
    enable_interrupts(int_rb);
    //habilita las interrupciones glovales
    enable_interrupts(global);
    
    //intrerrupcion por flanco de subida
    ext_int_edge(L_TO_H);
    
    // configura los puertos
    define_puertos();
    
    
    while (true) {
        lcd_putc("\f"); //limpia la lcd
        printf(lcd_putc,"pesadas: %d", cajasPesadas);
        delay_ms(1000);
        
        lcd_putc("\f"); //limpia la lcd
        printf(lcd_putc,"livianas: %d", cajasLivianas);
        delay_ms(1000);
        
        lcd_putc("\f"); //limpia la lcd
        printf(lcd_putc,"total: %d", (cajasPesadas+cajasLivianas));
        delay_ms(1000);
        
        lcd_putc("\f"); //limpia la lcd
        printf(lcd_putc,"banda 1");
        
        primera_etapa = true;
        
        // se apagan los led indicadores de caja
        output_low(PIN_B3);
        output_low(PIN_B4);
        
        // inicia la banda 1
        output_high(PIN_B2);
    
        // esta en este metodo hasta que pese algo
        while (true) {
            delay_ms(200);
            if (leerADC() > 1.0f) {
                break;
            }
        }
        lcd_putc("\f"); //limpia la lcd
        printf(lcd_putc,"sensando...");
    
        // se detiene la banda 1
        output_low(PIN_B2);
    
        // espera 3s mientras cuadra la caja
        delay_ms(3000);
    
        // ******************* peso de la caja ************   
        for (i = 0; i < 7; i++) {
            delay_ms(300);
            promedio += leerADC();
        }
        promedio /= 7;
 
        // se envia el peso por r232
        printf("%1.4f", promedio);
       
        lcd_putc("\f"); //limpia la lcd
        
        // enciende los led indicadores 
        if (promedio >= 2.2) {
            printf(lcd_putc,"caja pesada");
            output_high(PIN_B3);
            cajasPesadas+=1;
        }else{
            
            printf(lcd_putc,"caja liviana");
            output_high(PIN_B4);
            cajasLivianas+=1;
        }
        
        delay_ms(500);
        
        //**************** servo 1 (pasa a la otra banda) ***************
        // brazo 90 grados
        duty_50(PIN_C2, 1500, 15);
        duty_50(PIN_C2, 500, 15);

        // ************** Finaliza la primera etapa  *******************
        primera_etapa = false;

        correr_bandas_finales();
   
        subir_ascensor();
        delay_ms(500);
    
        // condicional para pesar las cajas 
        if (promedio >= 2.2) {
            // bazo cajas pesadas
            duty_50(PIN_C1, 500, 60);
            duty_50(PIN_C1, 2500, 60);
        } else {
            // bazo cajas livianas
            duty_50(PIN_C0, 500, 60);
            duty_50(PIN_C0, 2500, 60);
        }
        correr_bandas_finales();
        bajar_ascensor();
    }
}
