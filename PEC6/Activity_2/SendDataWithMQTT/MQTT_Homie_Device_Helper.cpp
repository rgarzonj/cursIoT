#include <PubSubClient.h>
#include "MQTT_Homie_Device_Helper.h"

const char *HOMIE_VERSION = "3.1.1";

MQTT_Homie_Device_Helper::MQTT_Homie_Device_Helper(String devID, String user, String password)

{
    deviceIdentifier = devID;
    brokerUser = user;
    brokerPassword = password;
    deviceRegistered = false;
}

void MQTT_Homie_Device_Helper::setClient(PubSubClient PSclient)
{
    client = PSclient;
}

void MQTT_Homie_Device_Helper::sendProperty(String node_prefix, String propertyName, String datatype, String unit, String payload)
{
    if (deviceRegistered == false)
    {
        Serial.println("Please register the device first using the C command.");
        return;
    }
    // Attempt to connect
    // Create a random client ID
    String clientId = deviceIdentifier;
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), brokerUser.c_str(), brokerPassword.c_str()))
    {
        Serial.println();
        Serial.println(String("Sending Property " + propertyName + " with payload " + payload));
        Serial.println();
        client.publish(String(node_prefix + "$name").c_str(), propertyName.c_str(), true);
        client.publish(String(node_prefix + "$datatype").c_str(), datatype.c_str(), true);
        client.publish(String(node_prefix + "$settable").c_str(), "false", true);
        client.publish(String(node_prefix + "$unit").c_str(), unit.c_str(), true);
        client.publish(String(node_prefix + "" + propertyName.c_str()).c_str(), payload.c_str(), true);
    }
    else
    {
        Serial.print("failed to connect to MQTT broker, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
    }
}

void MQTT_Homie_Device_Helper::registerNode(String device_prefix, String nodeName, String type, String properties)
{
    // Attempt to connect
    // Create a random client ID
    String clientId = deviceIdentifier;
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str()), brokerUser.c_str(), brokerPassword.c_str())
    {
        Serial.println("Registering Node");
        Serial.println(String(device_prefix + nodeName));
        client.publish(String(device_prefix + "/" + nodeName + "/$name").c_str(), nodeName.c_str(), true);
        client.publish(String(device_prefix + "/" + nodeName + "/$type").c_str(), type.c_str(), true);
        client.publish(String(device_prefix + "/" + nodeName + "/$properties").c_str(), properties.c_str(), true);
    }
    else
    {
        Serial.print("failed to connect to MQTT broker, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
    }
}

void MQTT_Homie_Device_Helper::registerDevice(String device_name, String friendlyDeviceName, String nodesList)
{
    // Attempt to connect
    // Create a random client ID
    String clientId = deviceIdentifier;
    clientId += String(random(0xffff), HEX);
    Serial.println("Connecting with client ID");
    Serial.println(clientId);
    if (client.connect(clientId.c_str()), brokerUser.c_str(), brokerPassword.c_str())
    {
        Serial.println("Connected to MQTT broker");
        // Register the device
        String homie_device_prefix = "homie/" + device_name;
        // Homie version
        client.publish(String(homie_device_prefix + "/$homie").c_str(), HOMIE_VERSION, true);
        client.publish(String(homie_device_prefix + "/$name").c_str(), device_name.c_str(), true);
        client.publish(String(homie_device_prefix + "/$state").c_str(), "ready", true);
        client.publish(String(homie_device_prefix + "/$nodes").c_str(), nodesList.c_str(), true);
        client.publish(String(homie_device_prefix + "/$extensions").c_str(), "", true);
        deviceRegistered = true;
    }
    else
    {
        Serial.print("failed to connect to MQTT broker, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
    }
}
void MQTT_Homie_Device_Helper::disconnectDevice(String device_name)
{
    // Attempt to connect
    // Create a random client ID
    String clientId = deviceIdentifier;
    clientId += String(random(0xffff), HEX);
    Serial.println("Connecting with client ID");
    Serial.println(clientId);
    if (client.connect(clientId.c_str()), brokerUser.c_str(), brokerPassword.c_str())
    {
        Serial.println("Connected to MQTT broker");
        // Register the device
        String homie_device_prefix = "homie/" + device_name;
        // Sending last will message
        Serial.println("Sending last will message");
        client.publish(String(homie_device_prefix + "/$state").c_str(), "disconnected", true);
        deviceRegistered = false;
    }
    else
    {
        Serial.print("failed to connect to MQTT broker, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
    }
}

void MQTT_Homie_Device_Helper::sendLastWillMessage(String device_name)
{
    // Attempt to connect
    // Create a random client ID
    String clientId = deviceIdentifier;
    clientId += String(random(0xffff), HEX);
    Serial.println("Connecting with client ID");
    Serial.println(clientId);
    if (client.connect(clientId.c_str()), brokerUser.c_str(), brokerPassword.c_str())
    {
        Serial.println("Connected to MQTT broker");
        // Register the device
        String homie_device_prefix = "homie/" + device_name;
        // Sending last will message
        Serial.println("Sending last will message");
        client.publish(String(homie_device_prefix + "/$state").c_str(), "lost", true);
        deviceRegistered = false;
    }
    else
    {
        Serial.print("failed to connect to MQTT broker, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
    }
}
