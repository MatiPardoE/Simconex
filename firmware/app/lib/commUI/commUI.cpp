#include "commUI.h"

CommUI::CommUI() : connectionStatus(false) {}

CommUI::~CommUI() {}

void CommUI::begin(unsigned long baudRate)
{
    Serial.begin(baudRate);
    checkConnection();
}

bool CommUI::isConnected()
{
    checkConnection();
    return connectionStatus;
}

void CommUI::checkConnection()
{
    connectionStatus = Serial; // Check if the serial port is open
}

CommUI::CommandFromUI CommUI::run(String &outputSerial)
{
    if (Serial.available() > 0)
    {
        char incomingByte = Serial.read();
        if (incomingByte == '\n')
        {
            // Log.notice("Received: %s\n", serialBuffer.c_str()); // No puedo logear nada en el pairing
            CommandFromUI command = parseCommand(serialBuffer);
            serialBuffer = ""; // Clear the buffer
            return command;
        }
        else if (incomingByte == '$')
        {
            CommandFromUI command = parseCommandManual(serialBuffer);
            if(command == MANUAL_MODE)
            {
                outputSerial = serialBuffer;
            }
            serialBuffer = ""; // Clear the buffer
            return command;
        }
        else
        {
            serialBuffer += incomingByte;
        }
    }
    return UNKNOWN_COMMAND;
}

CommUI::CommandFromUI CommUI::parseCommandManual(const String &input)
{
    const String commands[] = {"#C0", "#C1", "#O0", "#O1", "#N0", "#N1", "#A0", "#A1", "#WCOLD0", "#WCOLD1", "#WHOT0", "#WHOT1"};
    for (const String &command : commands)
    {
        if (input == command)
        {
            return MANUAL_MODE;
        }
    }
    if (input.startsWith("#L") && input.length() == 5 && isDigit(input[2]) && isDigit(input[3]) && isDigit(input[4]))
    {
        return MANUAL_MODE;
    }
    return UNKNOWN_COMMAND;
}

CommUI::CommandFromUI CommUI::parseCommand(const String &input)
{
    // Check for the specific command
    if (input == "#TRANSFER0!")
    {
        return TRANSFER_FILE_START;
    }
    else if (input == "#INIT!")
    {
        return INIT_COMM_UI;
    }
    else if (input == "#SYNC0!")
    {
        return SYNC_CYCLE_START;
    }
    else if (input == "#PAUSE!")
    {
        return PAUSE_CYCLE;
    }
    else if (input == "#PLAY!")
    {
        return RESUME_CYCLE;
    }
    else if (input == "#STARTCALODSAT!")
    {
        return START_CALIB_OD_SAT;
    }
    

    //
    else if (input == "#FINISHCALODSAT1P!")
    {
        return FINISH_CALIB_OD_SAT_1P;
    }
    else if (input == "#STARTCALODSAT2P!")
    {
        return GOTO_CALIB_OD_SAT_2P;
    }
    else if (input == "#FINISHCALODSAT2P!")
    {
        return FINISH_CALIB_OD_SAT_2P;
    }
    //


    else if (input == "#STARTCALPH!")
    {
        return START_CALIB_PH;
    }
    else if (input == "#CLEARCALPH!")
    {
        return CLEAR_CALIB_PH;
    }
    else if (input == "#SETMIDCALPH!")
    {
        return SET_MID_POINT_PH;
    }
    else if (input == "#SETLOWCALPH!")
    {
        return SET_LOW_POINT_PH;
    }
    else if (input == "#SETHIGHCALPH!")
    {
        return SET_HIGH_POINT_PH;
    }
    else if (input == "#FINISHCALPH!")
    {
        return FINISH_CALIB_PH;
    }
    // Add more commands as needed
    return UNKNOWN_COMMAND;
}
