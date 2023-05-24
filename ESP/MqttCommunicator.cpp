#include "MqttCommunicator.h"
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>


MqttCommunicator::MqttCommunicator() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(_ssid, _pw);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
    }

    Serial.print("\nWifi Connected");

    _mqttClient = PubSubClient(_wifiClient);
    _mqttClient.setServer(_mqttServer, 1883);
}

String MqttCommunicator::receiveMessage(void) {
    if (!_mqttClient.connected()) {
        reconnect();
    }

    _mqttClient.loop();
}

void MqttCommunicator::reconnect(void) {
    while (!_mqttClient.connected()) {
        String clientId = "esp_drehkopp_" + String(_randomSeed);
        if (_mqttClient.connect(clientId.c_str())) { //client ID festlegen
            Serial.println("connected");
        } else {
            delay(1000);
        }
    }
}

void MqttCommunicator::callback(char* topic, byte* payload, unsigned int length) {

    else receiveCallback(topic, payload, length);
    return;
}

bool MqttCommunicator::sendCallback(String messageToSend) {
    char msg[mesageToSend.length() + 1];
    snprintf(msg, messageToSend.length() + 1, messageToSend);
    return (bool) _mqttClient.publish(msg, _topic);
}

String MqttCommunicator::receiveCallback(char* topic, byte* payload, unsigned int length) {
    String msg = "";
    for (int i = 0; i < payload.length; i++) {
        msg += (char) payload[i];
    }
    
}

