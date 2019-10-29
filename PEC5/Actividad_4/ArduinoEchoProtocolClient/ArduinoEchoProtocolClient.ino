#include <SPI.h>
#include <WiFi101.h>
#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID; // your network SSID (name)
char pass[] = SECRET_PASS;

IPAddress ECHO_SERVER_ADDRESS(192, 168, 4, 20); // Echo Protocol Server
#define ECHO_SERVER_PORT 8888
char data;
int status = WL_IDLE_STATUS;

// Initialize the client library
WiFiClient client;

void setup()
{
  Serial.begin(9600);
  connectToWifi();
}

void loop()
{
  if (Serial.available() > 0)
  {
    data = Serial.read();
    /*Valid commands from the serial will be:
    W: Show Wifi status;
    C: Connect to Echo Server;
    R: Send request to Echo Server 
    Q: Quit*/
    executeCommandFromSerial(data);
    Serial.println();
  }

  readDataFromEchoServer();
}

void connectToWifi()
{
  // check if the WiFi module works
  if (WiFi.status() == WL_NO_SHIELD)
  {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true)
      ;
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
    digitalWrite(LED_BUILTIN, LOW);
  }

  // you're connected now, so print out the status:
  printWiFiStatus();
}

void connectToEchoProtocolServer()
{
  if (client.connect(ECHO_SERVER_ADDRESS, ECHO_SERVER_PORT))
  {
    Serial.println("Connected to Echo Protocol server.");
  }
  else
  {
    Serial.println("Could not connect to Echo Protocol server.");
  }
}

void stopEchoServerConnection()
{
  if (client.connected())
  {
    client.stop();
  }
}
void sendEchoRequest()
{
  if (client.connected())
  {
    Serial.println("Sending message HELLO ARDUINO HERE ");
    client.print("HELLO ARDUINO HERE");
  }
  else
  {
    Serial.println("ERROR: Cannot send Echo request.");
  }
}

void readDataFromEchoServer()
{
  if (client.available())
  {
    char c = client.read();
    Serial.print("Received character from server: ");
    Serial.println(c);
  }
}

void executeCommandFromSerial(char data)
{
  switch (data)
  {
  case 'C':
    //C: Connect to Echo Server;
    Serial.println("Received command C=Connect to Echo protocol server.");
    connectToEchoProtocolServer();
    break;
  case 'R':
    //R: Send request to Echo server;
    Serial.println("Received command R=Create new Echo request.");
    sendEchoRequest();
    break;
  case 'W':
    //W: Show Wifi status;
    Serial.println("Received command W=Print WIFI status.");
    printWiFiStatus();
    break;
  case 'Q':
    //Q: Quit*/
    Serial.println("Received command Q=Quit.");
    // do nothing forevermore:
    stopEchoServerConnection();
    for (;;)
      ;
    break;
  default:
  {
  }
  }
}

void printWiFiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
