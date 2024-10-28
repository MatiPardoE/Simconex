#include "commUI.h"

CommUI::CommUI() : connectionStatus(false) {}

CommUI::~CommUI() {}

void CommUI::begin(unsigned long baudRate) {
    Serial.begin(baudRate);
    checkConnection();
}

bool CommUI::isConnected() {
    checkConnection();
    return connectionStatus;
}

void CommUI::checkConnection() {
    connectionStatus = Serial; // Check if the serial port is open
}

CommUI::CommandFromUI CommUI::run() {
    if (Serial.available() > 0) {
        char incomingByte = Serial.read();
        if (incomingByte == '\n') {
            //Log.notice("Received: %s\n", serialBuffer.c_str()); No puedo logear nada en el pairing
            CommandFromUI command = parseCommand(serialBuffer);
            serialBuffer = ""; // Clear the buffer
            return command;
        } else {
            serialBuffer += incomingByte;
        }
    }
    return UNKNOWN_COMMAND;
}

CommUI::CommandFromUI CommUI::parseCommand(const String& input) {
    // Check for the specific command
    if (input == "#TRANSFER0!") {
        return TRANSFER_FILE_START;
    }else if (input == "#INIT!") {
        return INIT_COMM_UI;
    }
    // Add more commands as needed
    return UNKNOWN_COMMAND;
}