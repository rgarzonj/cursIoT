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
volatile byte state = LOW;
long previousMillis = 0;
int outputOscilatorState = LOW;
long interval = 1000;

// the setup function runs once when you press reset or power the board
void setup()
{
  pinMode(outputOscilatingPin, OUTPUT);                                         // configurar el PIN digital de salida.
  pinMode(LED_BUILTIN, OUTPUT);                                                 // configurar el PIN que tiene el LED como salida.
  pinMode(interruptPin, INPUT_PULLUP);                                          //configurar el PIN que será la entrada de interrupción
  attachInterrupt(digitalPinToInterrupt(interruptPin), changeLedState, CHANGE); //Definir ISR y el tipo de evento en la interrupción
}

// ISR lo más corto posible
void changeLedState()
{
  state = digitalRead(outputOscilatingPin);
}

// the loop function runs over and over again forever
void loop()
{
  digitalWrite(LED_BUILTIN, state);
  if (millis() - previousMillis > interval)
  {
    // save the last time you blinked the LED previousMillis = millis();
    previousMillis = millis();
    // if the LED is off turn it on and vice versa: if (ledState == LOW)
    outputOscilatorState = !outputOscilatorState;
    digitalWrite(outputOscilatingPin, outputOscilatorState);
  }
}
