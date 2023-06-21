#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Arduino.h>
#include <SoftwareSerial.h>

// Local includes
#include "Radio.h"

/************************* PUBLIC *****************************/
Radio::Radio(uint16_t ce, uint16_t csn, SoftwareSerial *espLine)
{
    Serial.println("We in Radio Constructor");
    _ce = ce;
    _csn = csn;
    _readingPipe = 1;

    _listening = true;
    _level = RF24_PA_MIN;

    _antenna = RF24(_ce, _csn);

    _espLine = espLine;
    Serial1.begin(9600);
    // Serial1.listen();

    Serial.println("We out of Radio Constructor");
}

/**
 * @brief
 * Initializes the object. Should be called before doing anything else!
 *
 */
void Radio::init()
{
    _antenna.begin();
    byte _adress[6] = "00001";
    _antenna.openReadingPipe(_readingPipe, _adress);
    _antenna.setPALevel(_level);
    _antenna.startListening();
}

/**
 * @brief
 * Sets PA-Level / Reach of antenna
 *
 * @param level MIN, LOW, HIGH or MAX, depending on the level that is supposed to be used
 */
void Radio::setPALevel(String level)
{
    if (level == "MIN")
    {
        _level = RF24_PA_MIN;
    }
    else if (level == "LOW")
    {
        _level = RF24_PA_LOW;
    }
    else if (level == "HIGH")
    {
        _level = RF24_PA_HIGH;
    }
    else if (level == "MAX")
    {
        _level = RF24_PA_MAX;
    }
    else
    {
        _level = RF24_PA_MIN;
    }

    _antenna.setPALevel(_level);
    Serial.println("Internal PA-Level: " + String(getPALevel()));
}

/**
 * @brief
 * Switches from listening to sending and the other way around
 *
 */
void Radio::switchState()
{
    byte _adress[6] = "00001";
    // Invert listening
    _listening = !_listening;

    // If listening: Prepare antenna for listening. Otherwise: Prepare antenna for sending
    if (_listening)
    {
        _antenna.openReadingPipe(_readingPipe, _adress);
        _antenna.startListening();
    }
    else
    {
        _antenna.stopListening();
        _antenna.openWritingPipe(_adress);
    }

    _antenna.setPALevel(_level);
}

bool Radio::available(void)
{
    return _antenna.available();
}

/**
 * @brief
 * Checks, whether there are devices available nearby
 *
 * @return true If there are devices available nearby
 * @return false otherwise
 */
bool Radio::checkNearbyDevices(void)
{
    return checkNearbyMqtt();
    _ackHappened = false;
    bool msg[5] = {false, false, false, false, false};

    for (int i = 0; i < 5; i++)
    {
        sendMessage(_test, "mqtt");
        receiveMessage("mqtt");

        msg[i] = wasAcknowledged();
        Serial.println("Msg " + String(i) + ": " + String((msg[i] ? "True" : "False")));
        delay(1000);
    }

    // If payload was the acknowlege String, that means there are devices nearby.
    for (int i = 0; i < 5; i++)
    {
        if (msg[i])
            return true;
    }

    Serial.println("_ack: " + String((_ackHappened ? "True" : "False")));

    _ackHappened = false;
    return false;
}

bool Radio::checkNearbyMqtt()
{
    _ackHappened = false;
    bool msg = false;
    sendMessage(_test, "mqtt");
    delay(5000);
    receiveMessage("mqtt");
    msg = wasAcknowledged();
    Serial.println("Msg: " + String((msg ? "True" : "False")));
    delay(1000);

    // If payload was the acknowlege String, that means there are devices nearby.
    if (msg)
        return true;

    Serial.println("_ack: " + String((_ackHappened ? "True" : "False")));

    _ackHappened = false;
    return false;
}

/**
 * @brief
 * Receives message from radio frequency
 *
 * @return String received message or \0, if nothing was gotten
 */
String Radio::receiveMessage(void)
{
    char buffer[128] = "";
    String msg = "";

    // Switch from sending to listening
    if (!_listening)
        switchState();

    // If no message is available, return universal break character
    if (!_antenna.available())
    {
        // Serial.println("Receiving: No Antenna available");
        return "\0";
    }
    Serial.println("Receiving: Antenna available");

    // Read available payload
    _antenna.read(&buffer, sizeof(buffer));

    for (int i = 0; i < 128; i++)
    {
        msg += String(buffer[i]);
        if (buffer[i] == '\0')
            break;
    }

    Serial.println("Receiving: Received char Array: " + msg);

    // Convert received message to String
    // msg = convertCharArrayToString(buffer);

    // If the test string was received, acknowledge it and return \0
    if (msg == String(_test))
    {
        Serial.println("Receiving: Test string, now acknowledging.");
        acknowledge();
        return "\0";
    }

    // THIS MOST PROBABLY IS DEPRECATED AND USELESS AS WELL
    if (msg == String(_jam))
    {
        acknowledge();
        return String(_jam);
    }

    // If acknowledge string was received, change marker in class fields to true so it can be checked by other methods and return \0
    if (msg == String(_acknowledge))
    {
        Serial.println("Receiving: Acknowledge String");
        _ackHappened = true;
        return "\0";
    }

    return msg;
}

String Radio::receiveMessage(String type)
{
    if (type == "mqtt")
    {
        return receiveMqttMessage();
    }
    else if (type == "radio")
    {
        return receiveMessage();
    }

    return String("\0");
}

String Radio::receiveMqttMessage(void)
{
    // int bytesAvailable = Serial1.available();
    if (Serial1.available() < 1)
        return "\0";

    Serial.println("Message available");
    String received = Serial1.readString();

    // If the test string was received, acknowledge it and return \0
    if (received == String(_test))
    {
        Serial.println("Receiving: Test string, now acknowledging.");
        acknowledge();
        return "\0";
    }

    // THIS MOST PROBABLY IS DEPRECATED AND USELESS AS WELL
    if (received == String(_jam))
    {
        acknowledge();
        return String(_jam);
    }

    // If acknowledge string was received, change marker in class fields to true so it can be checked by other methods and return \0
    if (received == String(_acknowledge))
    {
        Serial.println("Receiving: Acknowledge String");
        _ackHappened = true;
        return "\0";
    }

    return received;
}

/**
 * @brief Callback to sendMessage for specific message type (mqtt or rf)
 *
 * @param msg The message to be send
 * @param type type of the message (mqtt or radio)
 * @return true If the message was sent successfully
 * @return false If an invalid type has been given to method
 */
bool Radio::sendMessage(String msg, String type)
{
    if (type == "mqtt")
    {
        return sendMqttMessage(msg);
    }
    else if (type == "radio")
    {
        return sendMessage(msg);
    }

    return false;
}

bool Radio::sendMqttMessage(String msg)
{
    // if (Serial1.isListening()) Serial1.stopListening();
    Serial.println("Sending message " + msg);
    int bytesSent = Serial1.write(msg.c_str());
    Serial.println("Bytes sent: " + String(bytesSent));
    // Serial1.listen();
    if (bytesSent == 0)
    {
        Serial.print("Could not send to serial porat");
        return false;
    }
    if (bytesSent < msg.length() * 0.8)
        return false;

    return true;
}

/**
 * @brief
 * Sends acknowledgement message
 *
 */
void Radio::acknowledge(void)
{
    sendMessage(_acknowledge, "mqtt");
}

/**
 * @brief
 * Sends message given in param
 *
 * @param msg Message to be sent
 * @return true If sending was successful
 * @return false
 * TODO return false somewhere, why should it be bool otherwise
 */
bool Radio::sendMessage(String msg)
{
    String sendTest = "";
    bool tx_ok, tx_fail, rx_ready, test = true;
    char buffer[128];
    msg.toCharArray(buffer, msg.length() + 1);

    Serial.println("Sending message: " + msg);
    Serial.println("Listening State: " + String((_listening ? "listening" : "writing")));

    if (_listening)
        switchState();

    Serial.println("Listening State: " + String((_listening ? "listening" : "writing")));

    // convertStringToCharArray(msg, buffer);
    for (int i = 0; i < msg.length(); i++)
    {
        sendTest += String(buffer[i]);
    }
    Serial.println("Sending: Char Arr after conversion: " + sendTest);

    test = _antenna.write(&buffer, sizeof(buffer));

    if (!test)
    {
        Serial.println("Sending unseccessful");
        _antenna.whatHappened(tx_ok, tx_fail, rx_ready);
        Serial.println("OK: " + String((tx_ok ? "Yes" : "No")) + ", FAIL: " + String((tx_fail ? "Yes" : "No")) + ", READY: " + String((rx_ready ? "Yes" : "No")));
    }

    Serial.println("Returning from sending");
    switchState();
    return true;
}

/**
 * @brief
 * Converts a given String s to a char array and saves it to given pointer a
 *
 * @param s Strung to be converted to char array
 * @param a Pointer to char array
 */
void Radio::convertStringToCharArray(String s, char a[128])
{
    int counter = 0;

    // Iterate through entire String and add character to char array
    for (int i = 0; i < s.length(); i++)
    {
        a[i] = s.charAt(i);
        counter++;
    }

    // Fill up char array with empty values
    for (int i = counter; i < 128; i++)
    {
        a[i] = '\0';
    }

    Serial.println("Char Arr after conversion: " + String(a));
}

/**
 * @brief
 * Converts a given char array to a String and returns it
 *
 * @param a Char array to be converted to a String
 * @return String
 */
String Radio::convertCharArrayToString(char a[128])
{
    String s = "";

    // Iterate through the whole array and add every character to empty string
    for (int i = 0; i < 128; i++)
    {
        if (a[i] == '\0')
            continue;
        s += String(a[i]);
    }

    Serial.println("String after conversion: " + s);

    return s;
}

/**
 * @brief
 * Returns the class internal field that marks acknowledgement happened and switches it back to false
 *
 * @return true If an acknowledgement message has been received earlier
 * @return false otherwise
 */
bool Radio::wasAcknowledged()
{
    bool ack = _ackHappened;
    _ackHappened = false;

    return ack;
}