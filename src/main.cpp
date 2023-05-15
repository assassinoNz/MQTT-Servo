#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <AsyncMqttClient.h>
#include <ArduinoJson.h>

namespace IO {
    const unsigned char PIN_RESET = D4;
}

namespace WIFI {
    const char *SSID = "ssid";
    const char *PSK = "password";

    WiFiClient client;
}

namespace MQTT {
    const char *HOST = "192.168.1.4";
    const short HOST_PORT = 1883;
    const char *HOST_USERNAME = "username";
    const char *HOST_PASSWORD = "password";
    const char *CLIENT_ID = "client_id";
    const char *SERVO_TOPIC = "servo"; 

    AsyncMqttClient client;

    void onMqttConnect(bool sessionPresent) {
        Serial.println("\n[MQTT]: Established connection HOST: " + String(MQTT::HOST) + ":" + String(MQTT::HOST_PORT));

        client.subscribe(MQTT::SERVO_TOPIC, 0);
    }

    void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
        Serial.print("\n[MQTT]: Disconnected BECAUSE: " + String((unsigned char) reason));
    }

    void onMqttPublish(uint16_t packetId) {
        Serial.println("\n[MQTT]: Published PACKET_ID: " + String(packetId));
    }

    void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
        Serial.println("\n[MQTT]: Subscribed PACKET_ID: " + String(packetId) + " QOS: " + qos);
    }

    void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
        Serial.println("\n[MQTT]: Recieved TOPIC: " + String(topic) + " PAYLOAD: " + String(payload));

        if (strcmp(topic, MQTT::SERVO_TOPIC) == 0) {
            DynamicJsonDocument message(1024);
            deserializeJson(message, payload);

            const bool state = message["state"];
            pinMode(LED_BUILTIN, OUTPUT);
            digitalWrite(LED_BUILTIN, !state); //WARNING: LED_BUILTIN seems to be active low
        }
    }
}

void setup() {
    //Serial communication
    Serial.begin(115200);
    Serial.println("\n==================================================================");
    Serial.println("[SETUP]: Configured Serial communication AT: 115200");

    //WiFi
    Serial.println("[SETUP]: Connecting to WiFi SSID: " + String(WIFI::SSID));
    WiFi.begin(WIFI::SSID, WIFI::PSK);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("\n[WIFI]: Connected to WiFi SSID: " + String(WIFI::SSID));

    //MQTT
    Serial.println("[SETUP]: Configuring MQTT");
    MQTT::client.setServer(MQTT::HOST, MQTT::HOST_PORT);
    MQTT::client.setClientId(MQTT::CLIENT_ID);
    MQTT::client.setCredentials(MQTT::HOST_USERNAME, MQTT::HOST_PASSWORD);
    MQTT::client.onConnect(MQTT::onMqttConnect);
    MQTT::client.onDisconnect(MQTT::onMqttDisconnect);
    MQTT::client.onPublish(MQTT::onMqttPublish);
    MQTT::client.onSubscribe(MQTT::onMqttSubscribe);
    MQTT::client.onMessage(MQTT::onMqttMessage);
    MQTT::client.connect();
}

void loop() {
    // if (WiFi.status() == WL_CONNECTED) {
    //     webSocketClient.loop();
    // }
}