#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

class MqttCommunicator {
    public:
        MqttCommunicator();

        String  receiveMessage(void);
    
    private:
        SoftwareSerial _arduinoLine;
        WifiClient _wifiClient;
        PubSubClient _mqttClient;
        
        int _maxMessages = 10;
        String[_maxMessages] _messages;

        String _lastReceivedMessage = "";

        int _rxPin = 1, _txPin = 2, _serialBaud = 9600, _randomSeed = 1, _msgBufferSize = 180;
        
        char* _ssid = "WLANrouterRPPT", _pw = "bbsbrinkstrasse", _mqttServer = "192.168.1.2", _topic = "DREHKOPP/send";

        void        reconnect(void);
        void        callback(char* topic, byte* payload, unsigned int length);
        bool        sendCallback(String messageToSend);
        String      receiveCallback(char* topic, byte* payload, unsigned int length);
};