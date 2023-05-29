
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>

#define MSG_BUFFER_SIZE (150)

const char* ssid = "Routy McRouteface";
const char* pw = "09574281992860585641";
const char* broker = "192.168.178.90";
const char* topic = "DREHKOPP/Message";
String thisClient = "";


WiFiClient wClient;
PubSubClient mClient(wClient);
SoftwareSerial arduinoLine(5, 4);

void setup() 
{
    Serial.begin(9600);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pw);

    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println(WiFi.localIP());
    mClient.setServer(broker, 1883);
    mClient.setCallback(callback);
    
    pinMode(5, INPUT);
    pinMode(4, OUTPUT);
    arduinoLine.begin(9600);
}

void callback(char* topic, byte* payload, unsigned int length)
{
    String gottenTopic = String(topic);
    Serial.println("Topic: " + gottenTopic + ", Client: " + thisClient);
    if (gottenTopic.indexOf(thisClient) > 0) return;
    
    String msg = "";

    for (int i = 0; i < length; i++)
    {
        msg += (char) payload[i];
    }

    mClient.publish("DEBUG", msg.c_str());
    if (arduinoLine.available() > 0)
    {
        sendMessage();
    }

    Serial.println("Sending message " + msg);
    int bytesSent = arduinoLine.write(msg.c_str());
    Serial.println("Bytes sent: " + String(bytesSent));
    if (bytesSent == 0)
    {
        Serial.print("Could not send to serial portt");
        return;
    }

    return;
}

void reconnect()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, pw);
        while (WiFi.status() != WL_CONNECTED) 
        {
            delay(500);
            Serial.print(".");
        }
        Serial.println(WiFi.localIP());
    }
    
    while (!mClient.connected())
    {
        String clientId = "MessengerClient";
        clientId += String(random(0xffff), HEX);
        thisClient = clientId;

        if (!mClient.connect(clientId.c_str())) 
        {
            Serial.println("Connecting again in 5s");
            delay(100);
        }
    }

    String myTopic = String(topic);
    myTopic += "/#";
    mClient.subscribe(myTopic.c_str());
}

void sendMessage()
{
    int bytesAvailable = arduinoLine.available();
    if (bytesAvailable < 1) return;

    Serial.println("Message available");
    String msg = arduinoLine.readString();
    
    if (msg.length() < bytesAvailable) 
    {
        for (int i = msg.length(); i < bytesAvailable; i++) 
        {
            msg += "\0";
        }
    }
    Serial.println("Message: " + msg);
    if (msg.charAt(0) == '\0') 
    {
        return;
    }

    String thisTopic = String(topic);
    thisTopic += "/";
    thisTopic += thisClient;
    Serial.println("Topic: " + thisTopic);

    mClient.publish(thisTopic.c_str(), msg.c_str());
}

void loop() 
{
    if (!mClient.connected())
    {
        reconnect();
    }
    sendMessage();
    
    mClient.loop();
}
