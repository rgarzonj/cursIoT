/*
  Interruptions

  Vamos a reproducir el ejemplo “Blink” para ahora usando interrupciones. 
  Se trata de usar un PIN digital de salida, el que queramos, para que oscile 
  periódicamente entre 0 y 1 (BAJO y ALTO). Luego usaremos los estados de este 
  PIN como fuente de una interrupción, haciendo que nuestro LED (el que tenemos 
  soldado en la placa del MK1000) se encienda cada vez que el PIN de salida esté en ON.
*/

const int outputOscilatingPin = 1;
const int interruptPin = 4;
byte state = LOW;
volatile long counter = 0;
const int TICKS_PER_SECOND = 732; //PWM frequency is 732Hz
// setup se ejecuta una única vez al inicio
void setup()
{
  pinMode(outputOscilatingPin, OUTPUT);                // configurar el PIN digital de salida.
  pinMode(LED_BUILTIN, OUTPUT);                        // configurar el PIe N que tiene el LED como salida.
  pinMode(interruptPin, INPUT_PULLUP);                 //configurar el PIN que será la entrada de interrupción
  attachInterrupt(digitalPinToInterrupt(interruptPin), countPWMTicks, RISING); //Definir ISR y el tipo de evento en la interrupción
}

// ISR lo más breve posible, evitar uso de digitalWrite en el ISR
void countPWMTicks()
{
  counter++;
}

// loop se ejecuta indefinidamente
void loop()
{
    analogWrite(outputOscilatingPin, 127);
    digitalWrite(LED_BUILTIN, state);
    if (counter>TICKS_PER_SECOND)
    {
      state = !state;
      counter = 0;
    }
}
