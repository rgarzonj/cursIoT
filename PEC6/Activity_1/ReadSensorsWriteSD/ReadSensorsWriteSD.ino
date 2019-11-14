/*
Programar Arduino para datos de los sensores de la “shield” MKR ENV.
Concretamente debemos leer y devolver por serie los siguientes datos:			

Temperatura
Humedad
Presión
UVA
UVB
Índice UV
		
(​Opcional​) si dispones de una memoria microSD sería una buena idea añadir al código
 								
anterior la capacidad de registrar las lecturas de un día en la flash e ir borrando los datos según el criterio del algoritmo “round robin” (​https://en.wikipedia.org/wiki/Round-robin_scheduling​)
○ Se recomienda usar la librería “SD.h” para interactuar con la microSD. 
*/

#include <Arduino_MKRENV.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!ENV.begin()) {
    Serial.println("Failed to initialize MKR ENV shield!");
    while (1);
  }
//  initializeSDCard();
}

void loop() {
  // read all the sensor values
  float temperature = ENV.readTemperature();
  float humidity    = ENV.readHumidity();
  float pressure    = ENV.readPressure();
  float illuminance = ENV.readIlluminance();
  float uva         = ENV.readUVA();
  float uvb         = ENV.readUVB();
  float uvIndex     = ENV.readUVIndex();

  // print each of the sensor values
  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity    = ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Pressure    = ");
  Serial.print(pressure);
  Serial.println(" hPa");

  Serial.print("Illuminance = ");
  Serial.print(illuminance);
  Serial.println(" lx");

  Serial.print("UVA         = ");
  Serial.println(uva);

  Serial.print("UVB         = ");
  Serial.println(uvb);

  Serial.print("UV Index    = ");
  Serial.println(uvIndex);

  // print an empty line
  Serial.println();

 if (Serial.available() > 0)
  {
    data = Serial.read();   
    if (data=="Q"){
      //
    }
  }
  // wait 1 second to print again
  delay(1000);
}
/*
void initializeSDCard()
{
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
}


}
/*
void writeTextToSDCard (String dataString)
{
// open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }

}*/