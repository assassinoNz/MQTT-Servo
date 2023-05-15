#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

const char *ssid = "ssid";
const char *password = "psk";

char *mqttServer = "broker.hivemq.com";
int mqttPort = 1883;

void reconnect()
{
    Serial.println("Connecting to MQTT Broker...");
    while (!mqttClient.connected())
    {
        Serial.println("Reconnecting to MQTT Broker..");
        String clientId = "dilina";

        if (mqttClient.connect(clientId.c_str()))
        {
            Serial.println("Connected.");
            // subscribe to topic
            mqttClient.subscribe("lednodeesp32");
        }
    }
}
void callback(char *topic, byte *payload, unsigned int length)
{
    // Serial.print("Callback - ");
    // Serial.print("Message:");
    String message;
    for (int i = 0; i < length; i++)
    {
        message += (char)payload[i];
        // Serial.print((char)payload[i]);
    }
    Serial.println("");
    if (message == "on")
    {
        digitalWrite(LED_BUILTIN, HIGH);
    }
    else if (message == "off")
    {
        digitalWrite(LED_BUILTIN, LOW);
    }
}

void setupMQTT()
{
    mqttClient.setServer(mqttServer, mqttPort);
    mqttClient.setCallback(callback);
}

void setup()
{
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("Connected to Wi-Fi");

    setupMQTT();
}

void loop()
{
    if (!mqttClient.connected())
        reconnect();
    mqttClient.loop();
    long now = millis();
    long previous_time = 0;

    if (now - previous_time > 1000)
    {
        previous_time = now;
    }
}