#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Arduino.h>

// Local includes
#include "Radio.h"

/************************* PUBLIC *****************************/
Radio::Radio(uint16_t ce, uint16_t csn) {
    Serial.println("We in Radio Constructor");
    _ce = ce;
    _csn = csn;
    _readingPipe = 1;

    _listening = true;
    _level = RF24_PA_MIN;

    _antenna = RF24(_ce, _csn);
    Serial.println("We out of Radio Constructor");
}

/**
 * @brief
 * Initializes the object. Should be called before doing anything else!
 * 
 */
void Radio::init() {
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
void Radio::setPALevel(String level) {
    if (level == "MIN") {
        _level = RF24_PA_MIN;
    } else if (level == "LOW") {
        _level = RF24_PA_LOW;
    } else if (level == "HIGH") {
        _level = RF24_PA_HIGH;
    } else if (level == "MAX") {
        _level = RF24_PA_MAX;
    } else {
        _level = RF24_PA_MIN;
    }
}

/**
 * @brief 
 * Switches from listening to sending and the other way around
 * 
 */
void Radio::switchState() {
    byte _adress[6] = "00001";
    // Invert listening
    _listening = !_listening;

    // If listening: Prepare antenna for listening. Otherwise: Prepare antenna for sending
    if (_listening) {
        _antenna.openReadingPipe(_readingPipe, _adress);
        _antenna.startListening();
    } else {
        _antenna.stopListening();
        _antenna.openWritingPipe(_adress);
    }

    _antenna.setPALevel(_level);
}

bool Radio::available(void) {
    return _antenna.available();
}

/**
 * @brief 
 * Checks, whether there are devices available nearby
 * 
 * @return true If there are devices available nearby 
 * @return false otherwise
 */
bool Radio::checkNearbyDevices(void) {
    char buffer[256];
    String msg;

    // Switch from listening to sending
    if (_listening) switchState();
    
    // Convert _acknowledge message to char array
    convertStringToCharArray(_acknowledge, buffer);

    // Send acknowledge message 3 times
    for (int i = 0; i < 3; i++) {
        _antenna.write(buffer, sizeof(buffer));
        delay(10);
    }

    // Switch back to listening
    switchState();

    // Wait for payload being available
    int counter = 0;
    while (!_antenna.available()) {
        counter++;
        if (counter > 9) return false;
        delay(10);
        continue;
    }

    // Read payload and convert it back to String
    _antenna.read(buffer, sizeof(buffer));
    msg = convertCharArrayToString(buffer);

    // If payload was the acknowlege String, that means there are devices nearby.
    if (msg == _acknowledge) return true;

    return false;
}

/**
 * @brief 
 * Receives message from radio frequency
 * 
 * @return String received message or \0, if nothing was gotten
 */
String Radio::receiveMessage(void) {
    char buffer[256];
    String msg;

    // Switch from sending to listening
    if (!_listening) switchState();

    // If no message is available, return universal break character
    if (!_antenna.available()) return "\0";
    Serial.println("Receiving: Antenna available");

    // Read available payload
    _antenna.read(buffer, sizeof(buffer));

    // Convert received message to String
    msg = convertCharArrayToString(buffer);

    // THIS IS DEPRECATED AND USELESS 
    if (msg == String(_acknowledge)) {
        return "\0";
    }

    // THIS MOST PROBABLY IS DEPRECATED AND USELESS AS WELL
    if (msg == String(_jam)) {
        acknowledge();
        return String(_jam);
    }

    return msg;
}

/**
 * @brief 
 * Sends acknowledgement message
 * 
 */
void Radio::acknowledge(void) {
    char buffer[256];

    // Switch from listening state to sending state
    if (_listening) switchState();

    // convert the acknowledge payload to a char array
    convertStringToCharArray(_acknowledge, buffer);

    // Send acknowledgement 3 times, to make sure partner gets the message
    for (int i = 0; i < 3; i++) {
        _antenna.write(buffer, sizeof(buffer));
        delay(10);
    }
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
bool Radio::sendMessage(String msg) {
    char buffer[256];
    
    Serial.println("Sending message: " + msg);
    Serial.println("Listening State: " + String((_listening ? "listening" : "writing")));

    if (_listening) switchState();
    
    Serial.println("Listening State: " + String((_listening ? "listening" : "writing")));

    convertStringToCharArray(msg, buffer);
    Serial.println(String(_antenna.write(buffer, sizeof(buffer)) ? "Successful" : "Unsuccessful"));

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
void Radio::convertStringToCharArray(String s, char a[256]) {
    int counter = 0;

    // Iterate through entire String and add character to char array
    for (int i = 0; i < s.length(); i++) {
        a[i] = s.charAt(i);
        counter++;
    }

    // Fill up char array with empty values
    for (int i = counter; i < 256; i++) {
        a[i] = '\0';
    }
}

/**
 * @brief 
 * Converts a given char array to a String and returns it
 * 
 * @param a Char array to be converted to a String
 * @return String 
 */
String Radio::convertCharArrayToString(char a[256]) {
    String s = "";

    // Iterate through the whole array and add every character to empty string
    for (int i = 0; i < 256; i++) {
        if (a[i] == '\0') continue;
        s += String(a[i]);
    }

    return s;
}