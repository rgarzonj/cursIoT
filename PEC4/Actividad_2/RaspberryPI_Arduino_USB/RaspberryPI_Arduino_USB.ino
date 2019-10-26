/*
  Actividad 2
Se define el siguiente escenario: se conecta el Arduino a la Raspberry PI usando un cable USB. Desde la Raspberry PI ahora podemos ver un nuevo dispositivo USB-serie conectado.
●  comando: lsusb
 Abrimos puerto serie para comunicarnos con Arduino desde la Raspberry PI:		
●  comando: screen /dev/PUERTO_SERIE_DEL_ARDUINO 9600
 Desde la consola serie debemos mandar los siguientes comandos a Arduino y este debe
responder como procede:				
●  comando: ON - respuesta: LED ON
●  comando: OFF - respuesta: LED OFF							
El alcance de la actividad es desarrollar el código para Arduino para que cuando este se conecte a la Raspberry PI se comporte de la forma descrita anteriormente.
Se deberá subir el código de la actividad a GITHUB. 
*/

char data;
const char *ON_COMMAND = "ON";
const char *OFF_COMMAND = "OFF";
const int MAXCHARS = 100; //We'll create a buffer of chars as we receive them 
                          //and check the last 2 or 3 chars in the buffer
String command = "XX"; //This variable will store our character buffer

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  if (Serial.available() > 0)
  {
    data = Serial.read();
    //Enqueue the received character in the buffer
    command += data;
    //Check the last 2 characters to see if it's ON command
    const char *last_two = &command[command.length() - 2];
    if (!strcmp(last_two, ON_COMMAND))
    {
      digitalWrite(LED_BUILTIN, HIGH);
      //Initialize the buffer with at least 2 chars
      command = "XX";
    }
    else
    {
      //Check the last 3 characters to see if it's OFF command
      const char *last_three = &command[command.length() - 3];
      if (!strcmp(last_three, OFF_COMMAND))
        {
        digitalWrite(LED_BUILTIN, LOW);
        //Initialize the buffer with at least 2 chars
        command = "XX";
        }
    }
    //Reset the buffer when we reach the max chars in the buffer   
    if (command.length() > MAXCHARS)
    {
      char tmp[4] = "XXX";
      //We need to keep the last characters just in case it was partially a command
      strcpy(tmp, &command[command.length() - 3]);
      strcpy(&command[0],tmp);
    }
  }
}