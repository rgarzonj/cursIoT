/*
 Basic ESP8266 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/

/*
Temperature = 26.07 °C
Humidity    = 43.70 %
Pressure    = 96.84 kPa
Illuminance = 30.65 lx
UVA         = 0.00
UVB         = 0.00
UV Index    = 0.00
*/
#include <Arduino_MKRENV.h>
#include <WiFi101.h>
#include <PubSubClient.h>
#include "MQTT_Homie_Device_Helper.h"

// Update these with values suitable for your network.

#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID; // your network SSID (name)
char pass[] = SECRET_PASS; // your network password (use for WPA, or use as key for WEP)

const char *mqtt_server = "192.168.26.135";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
const char *DEVICE_IDENTIFIER = "mkrenv";

MQTT_Homie_Device_Helper mqtt_helper(DEVICE_IDENTIFIER);

void setup_wifi()
{

  delay(5000);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1')
  {
    digitalWrite(LED_BUILTIN, LOW); // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  }
  else
  {
    digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off by making the voltage HIGH
  }
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void registerDeviceAndNodes()
{
  mqtt_helper.registerDevice(DEVICE_IDENTIFIER, "MKRENV Ruben Garzon", "pressure,temperature,humidity,ambiancelight,ultraviolet");
  mqtt_helper.registerNode(String("homie/" + String(DEVICE_IDENTIFIER) + "/"), "pressure", "Pressure sensor", "pressure");
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT); // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  // Setup MKRENV Shield
  if (!ENV.begin())
  {
    Serial.println("Failed to initialize MKR ENV shield!");
    while (1)
      ;
  }
  // Connect to WIFI
  setup_wifi();

  // Setup MQTT helper
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  mqtt_helper.setClient(client);

  // Register device and node with MQTT Broker
  registerDeviceAndNodes();
}

void readSensors()
{
  // read all the sensor values
  float temperature = ENV.readTemperature();
  float humidity = ENV.readHumidity();
  float pressure = ENV.readPressure();
  float illuminance = ENV.readIlluminance();
  float uva = ENV.readUVA();
  float uvb = ENV.readUVB();
  float uvIndex = ENV.readUVIndex();

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
  mqtt_helper.sendProperty(String("homie/" + String(DEVICE_IDENTIFIER) + "/pressure/"), "pressure", "float", "kPa", String(pressure));

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
}
void loop()
{
  delay(2000);
  //  if (!client.connected()) {
  //    reconnect();
  //}
  readSensors();
  client.loop();
  /* 
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 50, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic", msg);
  }*/
}