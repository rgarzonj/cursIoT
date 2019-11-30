/*
 Crear un “flow” en NodeRED que genere una página web a través de la cual mandamos
órdenes a los relés del MKR RELAY y además monitorizamos su estado.
○ Desarrollar el firmware para el MKR1000 que hemos conectado al MKR RELAY que se suscriba a un “topic” de MQTT para recibir comandos. Además el código
informará a un “topic” de su estado.
○ Enviar los datos leídos al servidor MQTT cumpliendo la convención “Homie”
referenciada en el PLA 5.
*/

//#include <ESP8266WiFi.h>
#include <WiFi101.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

#include "arduino_secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

const char *mqtt_server = "192.168.26.135";

const int relay1 = 1;
const int relay2 = 2;

WiFiClient wifi_Client;
PubSubClient client(wifi_Client);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi()
{
  //Configure relays
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(SECRET_SSID, SECRET_PASS);

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
void processRelayAction(int whichRelay, const char* value,const char* relayName)
{
  // whatever you want for this topic
  if (strcmp(value, "true") == 0)
  {
    Serial.println(String("Turning ON "  + String(relayName)));
    digitalWrite(whichRelay, HIGH);
    Serial.println("Sending MQTT message with state of relay.");
    client.publish(String("homie/mkrrelay/" + String(relayName) + "/relay_on").c_str(), "ON",true);
  }
  if (strcmp(value, "false") == 0)
  {
    Serial.println(String("Turning OFF "  + String(relayName)));
    digitalWrite(whichRelay, LOW);
    Serial.println("Sending MQTT message with state of relay.");
    client.publish(String("homie/mkrrelay/" + String(relayName) + "/relay_on").c_str(), "OFF",true);
  }
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String value = "";
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
    value += (char)payload[i];
  }
  Serial.println();
  if (strcmp(topic, "homie/mkrrelay/relay1/relay_on/set") == 0)
  {
    Serial.println("Detected message for relay1");
    processRelayAction(relay1, value.c_str(),"relay1");
  }
  if (strcmp(topic, "homie/mkrrelay/relay2/relay_on/set") == 0)
  {
    Serial.println("Detected message for relay2");
    processRelayAction(relay2, value.c_str(),"relay2");
  }

  Serial.println();
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "MKR1000Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("homie/mkrrelay/$state", "ready");
      // ... and resubscribe
      client.subscribe("homie/mkrrelay/relay1/relay_on/set");
      client.subscribe("homie/mkrrelay/relay2/relay_on/set");
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

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT); // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 8883);
  client.setCallback(callback);
}

void loop()
{

  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 20000)
  {
    lastMsg = now;
    ++value;
    Serial.print("Arduino waiting for MQTT message: ");
    //snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
    //Serial.print("Publish message: ");
    //Serial.println(msg);
    //client.publish("outTopic", msg);
  }
}