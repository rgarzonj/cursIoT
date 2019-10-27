#include <SPI.h>
#include <WiFi.h>

char ssid[] = "JAZZTEL_exdV";          //  your network SSID (name)
char pass[] = "5eyphrzqthr2";   // your network password

int status = WL_IDLE_STATUS;
//IPAddress server(74,125,115,105);  // Google
IPAddress server(192,168,26,136);  // Echo Protocol Server

// Initialize the client library
WiFiClient client;

void setup() {
  Serial.begin(9600);
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
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }




  Serial.println("Attempting to connect to WPA network...");
  Serial.print("SSID: ");
  Serial.println(ssid);

  status = WiFi.begin(ssid, pass);
  if ( status != WL_CONNECTED) {
    Serial.println("Couldn't get a wifi connection");
    // don't do anything else:
    while(true);
  }
  else {
    Serial.println("Connected to wifi");
    Serial.println("\nStarting connection...");
    // if you get a connection, report back via serial:
    if (client.connect(server, 8888)) {
      Serial.println("connected");
      // Make a HTTP request:
      client.println("Hello World");
      client.println();
    }
  }
}

void loop() {
    if (client.available()) {
    char c = client.read();
    Serial.println('Received character from server');
    Serial.print(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    for(;;)
      ;
  }
  delay (3000);
    if (client.available()) {
  client.println("Hello World");
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
