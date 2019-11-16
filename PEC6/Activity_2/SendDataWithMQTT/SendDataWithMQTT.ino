/*
PLA 6
Actividad 2

Enviar los datos leídos al servidor MQTT cumpliendo la convención “Homie” referenciada en
el PLA 5.
○ Explica que “topics” has usado y por qué.
○ Muestra con el comando “mosquitto_sub” cómo te has suscrito a los datos que
estabas mandado. Muestra cómo usas los comodines para suscripción y explica por qué usas esos comodines.
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

char mqtt_broker_user[] = MQTT_BROKER_USER;
char mqtt_broker_password[] = MQTT_BROKER_PASSWORD;

const char *mqtt_server = "192.168.26.135";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
const char *DEVICE_IDENTIFIER = "mkrenv1";
char data;

MQTT_Homie_Device_Helper mqtt_helper(DEVICE_IDENTIFIER, mqtt_broker_user, mqtt_broker_password);

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
  mqtt_helper.registerDevice(DEVICE_IDENTIFIER, "MKRENV Ruben Garzon", "pressure,termosthat,humidity,uv-sensor");

  mqtt_helper.registerNode(String("homie/" + String(DEVICE_IDENTIFIER) + "/"), "pressure", "Pressure sensor", "pressure");
  mqtt_helper.registerNode(String("homie/" + String(DEVICE_IDENTIFIER) + "/"), "termosthat", "Temperature sensor", "temperature");
  mqtt_helper.registerNode(String("homie/" + String(DEVICE_IDENTIFIER) + "/"), "humidity", "Humidity sensor", "humidity");
  mqtt_helper.registerNode(String("homie/" + String(DEVICE_IDENTIFIER) + "/"), "uv-sensor", "UV sensor", "uv-index,ultraviolet-a,ultraviolet-b");
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
  //  float illuminance = ENV.readIlluminance();
  float uva = ENV.readUVA();
  float uvb = ENV.readUVB();
  float uvIndex = ENV.readUVIndex();

  // print each of the sensor values
  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" °C");
  mqtt_helper.sendProperty(String("homie/" + String(DEVICE_IDENTIFIER) + "/thermostat/"), "humidity", "float", "°C", String(temperature));

  Serial.print("Humidity    = ");
  Serial.print(humidity);
  Serial.println(" %");
  mqtt_helper.sendProperty(String("homie/" + String(DEVICE_IDENTIFIER) + "/humidity/"), "humidity", "float", "%", String(humidity));

  Serial.print("Pressure    = ");
  Serial.print(pressure);
  Serial.println(" hPa");
  mqtt_helper.sendProperty(String("homie/" + String(DEVICE_IDENTIFIER) + "/pressure/"), "pressure", "float", "kPa", String(pressure));

  Serial.print("UVA         = ");
  Serial.print(uva);
  Serial.println(" μW/cm2");
  mqtt_helper.sendProperty(String("homie/" + String(DEVICE_IDENTIFIER) + "/uv-sensor/"), "ultraviolet-a", "float", "μW/cm2", String(uva));

  Serial.print("UVB         = ");
  Serial.print(uvb);
  Serial.println(" μW/cm2");

  mqtt_helper.sendProperty(String("homie/" + String(DEVICE_IDENTIFIER) + "/uv-sensor/"), "ultraviolet-b", "float", "μW/cm2", String(uvb));

  Serial.print("UV Index    = ");
  Serial.print(uvIndex);
  Serial.println(" (abs index 0 to 11)");
  mqtt_helper.sendProperty(String("homie/" + String(DEVICE_IDENTIFIER) + "/uv-sensor/"), "uv-index", "float", "#", String(uvIndex));

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

  if (Serial.available() > 0)
  {
    data = Serial.read();
    if (data == 'D') //Disconnect
    {
      mqtt_helper.disconnectDevice(DEVICE_IDENTIFIER);
    }

    if (data == 'C') //Connect, register device and nodes
    {
      registerDeviceAndNodes();
    }

    if (data == 'B') //Badly disconnected device
    {
      mqtt_helper.sendLastWillMessage(DEVICE_IDENTIFIER);
    }
  }
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