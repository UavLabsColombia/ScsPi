// Convertidor de Analogo a Digital
#include <18F2550.h>

#fuses xt, nowdt, noprotect, put, CPUDIV1
// Numero de bits a utilizar sobre el convertidor analogo
#device adc = 10
#use delay(internal = 8000000)
#use rs232(baud = 9600, bits = 8, parity = N, xmit = PIN_C6, RCV = PIN_C7, stop=1, ERRORS)

#use fast_io(b)
#use fast_io(a)

// Variables Globales
// calculo de la resulucion en la salida
float resolucion = (float)5 / (float)1023;
char dato_recibido = '0';

// Se define el puerto ADC a utilizar y sus pines.
void define_puertos() {
 
  // Define el pin b7 como entrada y el resto del puerto como salidas
  //bin: 1000 0000
  set_tris_b(0x80);
  
  // define la entrada 0 del puerto a como entrada
  //bin: 0000 0001
  set_tris_a(0x01);

  // Define como entradas analoga el puerto AN0
  SETUP_ADC_PORTS(AN0);

  // Define el relog interno para el uso del ADC
  SETUP_ADC(ADC_CLOCK_INTERNAL);
}

//interrumcion de llegada de datos
#int_rda
void rb_isr()
{
   if(kbhit())
   {
      dato_recibido = getc();
      delay_ms(4);
      //printf("ultimo dato recibido %c \n" getc());
   }
}



void main() {
   //habilita las interrupciones
   enable_interrupts(int_rda);
   enable_interrupts(global);
   
  // configura los puertos
  define_puertos();

  int16 Valor = 0;
  float ValorADC = 0;
  SET_ADC_CHANNEL(0);

   int16 tempo = 0;
  while (true) {
    delay_ms(5);
    // Lee el puerto Analogo
    Valor = READ_ADC();
    // output_toggle(PIN_B1);
    ValorADC = resolucion * (float)Valor;


//********  borrar start
    if (ValorADC > 1.0f) {
      output_high(PIN_B1);
      //delay_ms(500);
    } else {
      output_low(PIN_B1);
      //delay_ms(500);
    }
//********  borrar end
    

    if(dato_recibido == '1'){
      //start
      output_high(PIN_B2);
    }else if(dato_recibido == '0'){
      //stopt
      output_low(PIN_B2);
    }else if(dato_recibido == '2'){
     //reset
     //codigo para reset 
     
    }
    
    if(input(PIN_B7)){
      printf("777777");
    }

    // 5ms * 200 = 1s
    if(tempo == 200){
      tempo = 0;
      //printf("%c.1111", dato_recibido);
      printf("%1.4f", ValorADC);
      output_toggle(PIN_B1);
    }
    tempo ++;
    
  }
}
