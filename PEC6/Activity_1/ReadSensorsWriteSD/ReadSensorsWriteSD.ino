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
#include <SD.h>

unsigned long currentFilePosition;
const int MAX_FILE_POSITIONS = 10000;
const int chipSelect = 4;
char data;
unsigned long time;

void initializeSDCard()
{
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect))
  {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1)
      ;
  }
  Serial.println("card initialized.");
}

/* Deletes the old data in the file by using a temporary file*/
void deleteOldData()
{
  Serial.println("Removing oldest data in the file.");
  //Create a new temp file
  int firstPositionToRead = round(MAX_FILE_POSITIONS / 2);
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  File tempFile = SD.open("templog.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile)
  {
    if (tempFile)
    {
      dataFile.seek(firstPositionToRead);
      while (dataFile.available())
      {
        data = dataFile.read();
        tempFile.print(data);
      }
      dataFile.close();
      tempFile.close();
    }
    else
    {
      Serial.println("ERROR: Could not read existing file.");
    }
  }
  else
  {
    Serial.println("ERROR: Could not write temporary file.");
  }
  //
  SD.remove("datalog.txt");
  dataFile = SD.open("datalog.txt", FILE_WRITE);
  tempFile = SD.open("templog.txt", FILE_WRITE);
  while (dataFile.available())
  {
    data = tempFile.read();
    dataFile.print(data);
  }
  dataFile.close();
  tempFile.close();
  SD.remove("templog.txt");
  Serial.println("Oldest data was removed from the datalog file.");
}

/*Write the passed string to the SD Card*/

void writeToSDCard(String dataString)
{
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile)
  {
    Serial.println();
    Serial.println("Writing data to SD Card");
    Serial.println();
    dataFile.print(dataString);
    if (dataFile.position() > MAX_FILE_POSITIONS)
    {
      dataFile.close();
      deleteOldData();
    }
    else
    {
      dataFile.close();
    }
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else
  {
    Serial.println("ERROR opening datalog.txt");
  }
}

void setup()
{
  delay(3000);
  Serial.begin(9600);
  while (!Serial)
    ;

  if (!ENV.begin())
  {
    Serial.println("Failed to initialize MKR ENV shield!");
    while (1)
      ;
  }
  initializeSDCard();
}

void loop()
{
  // read all the sensor values
  float temperature = ENV.readTemperature();
  float humidity = ENV.readHumidity();
  float pressure = ENV.readPressure();
  float illuminance = ENV.readIlluminance();
  float uva = ENV.readUVA();
  float uvb = ENV.readUVB();
  float uvIndex = ENV.readUVIndex();
  String dataString = "";

  Serial.print("Time: ");
  time = millis();
  Serial.println(String(time));
  Serial.println();

  dataString += String("Time: " + String(time) + "\n");

  // print each of the sensor values
  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" °C");

  dataString += String("Temperature = " + String(temperature) + " °C\n");

  Serial.print("Humidity    = ");
  Serial.print(humidity);
  Serial.println(" %");

  dataString += String("Humidity = " + String(humidity) + " %\n");

  Serial.print("Pressure    = ");
  Serial.print(pressure);
  Serial.println(" hPa");

  dataString += String("Pressure = " + String(pressure) + " hPa\n");

  Serial.print("Illuminance = ");
  Serial.print(illuminance);
  Serial.println(" lx");

  dataString += String("Illuminance = " + String(illuminance) + " lx\n");

  Serial.print("UVA         = ");
  Serial.println(uva);

  dataString += String("UVA = " + String(uva) + " \n");

  Serial.print("UVB         = ");
  Serial.println(uvb);

  dataString += String("UVB = " + String(uvb) + " \n");

  Serial.print("UV Index    = ");
  Serial.println(uvIndex);

  dataString += String("UV Index = " + String(uvIndex) + " \n");

  writeToSDCard(dataString);

  // print an empty line
  Serial.println();

  // wait 2 seconds to print again
  delay(1000);
}

