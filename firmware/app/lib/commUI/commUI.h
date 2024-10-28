#ifndef COMM_UI_H
#define COMM_UI_H

#include <Arduino.h>
#include <ArduinoLog.h>
class CommUI
{
public:
    enum CommandFromUI
    {
        UNKNOWN_COMMAND,
        TRANSFER_FILE_START,
        INIT_COMM_UI
        // Add other commands as needed
    };
    CommUI();
    ~CommUI();

    void begin(unsigned long baudRate);
    bool isConnected();
    CommandFromUI run();

private:
    bool connectionStatus;
    String serialBuffer;

    void checkConnection();
    CommandFromUI parseCommand(const String &input);
};

#endif // COMM_UI_H