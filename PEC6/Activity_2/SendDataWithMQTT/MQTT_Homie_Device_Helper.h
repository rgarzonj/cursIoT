#ifndef MQTT_HOMIE_HELPER_H
#define MQTT_HOMIE_HELPER_H

class MQTT_Homie_Device_Helper
{

public:
    String deviceIdentifier;
    String brokerUser;
    String brokerPassword;
    PubSubClient client;

    MQTT_Homie_Device_Helper(String devID,String user,String password);
    void setClient(PubSubClient PSclient);

    void sendProperty(String node_prefix, String propertyName, String datatype, String unit, String payload);
    void registerNode(String device_prefix, String nodeName, String type, String properties);
    void registerDevice(String device_name, String friendlyDeviceName, String nodesList);
};

#endif