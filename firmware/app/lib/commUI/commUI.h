#ifndef COMM_UI_H
#define COMM_UI_H

#include <Arduino.h>
#include <ArduinoLog.h>
#include <esp_log.h>
class CommUI
{
public:
    enum CommandFromUI
    {
        UNKNOWN_COMMAND         ,
        TRANSFER_FILE_START     ,
        INIT_COMM_UI            ,
        SYNC_CYCLE_START        ,
        PAUSE_CYCLE             ,
        RESUME_CYCLE            ,
        MANUAL_MODE             ,
        START_CALIB_OD_SAT      ,
        FINISH_CALIB_OD_SAT_1P  ,
        FINISH_CALIB_OD_SAT_2P  ,
        START_CALIB_PH          ,
        FINISH_CALIB_PH         
    };
    CommUI();
    ~CommUI();

    void begin(unsigned long baudRate);
    bool isConnected();
    CommandFromUI run(String &input);

private:
    bool connectionStatus;
    String serialBuffer;

    void checkConnection();
    CommandFromUI parseCommand(const String &input);
    CommandFromUI parseCommandManual(const String &input);
};

#endif // COMM_UI_H