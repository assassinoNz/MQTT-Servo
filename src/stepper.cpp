#include <WiFi.h>
#include <Arduino.h>
#include <PubSubClient.h>
#include <Stepper.h>

// Replace with your network credentials
const char *ssid = "your_SSID";
const char *password = "your_PASSWORD";

// HiveMQ MQTT broker details
const char *mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;

// Replace with your MQTT topic
const char *mqttTopic = "your_MQTT_topic";

// Stepper motor configuration
const int stepsPerRevolution = 2048; // Change this according to your motor
const int stepperPin1 = 13;
const int stepperPin2 = 12;
const int stepperPin3 = 14;
const int stepperPin4 = 27;
Stepper stepperMotor(stepsPerRevolution, stepperPin1, stepperPin3, stepperPin2, stepperPin4);

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup()
{
    Serial.begin(115200);
    delay(100);

    setupWiFi();
    setupMQTT();

    // Set the speed of the stepper motor
    stepperMotor.setSpeed(10); // Adjust the speed as required
}

void loop()
{
    if (!mqttClient.connected())
    {
        reconnectMQTT();
    }
    mqttClient.loop();
}

void setupWiFi()
{
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void setupMQTT()
{
    mqttClient.setServer(mqttServer, mqttPort);
    mqttClient.setCallback(callback);

    reconnectMQTT();
}

void reconnectMQTT()
{
    while (!mqttClient.connected())
    {
        Serial.print("Attempting MQTT connection...");

        if (mqttClient.connect("ESP32Client"))
        {
            Serial.println("connected");
            mqttClient.subscribe(mqttTopic);
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" retrying in 5 seconds");
            delay(5000);
        }
    }
}

void callback(char *topic, byte *payload, unsigned int length)
{
    Serial.println("Message received: ");

    String message;
    for (int i = 0; i < length; i++)
    {
        message += (char)payload[i];
    }

    Serial.println(message);

    // Convert the received message to an integer
    int steps = message.toInt();

    // Rotate the stepper motor
    stepperMotor.step(steps);

    Serial.print("Stepper motor rotated ");
    Serial.print(steps);
    Serial.println(" steps");
}
