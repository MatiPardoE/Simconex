#ifndef MANUALMODE_H
#define MANUALMODE_H

#include <Arduino.h>
#include "esp_log.h"
#include <alarm.h>

class ManualMode
{
public:
    enum ManualBundle
    {
        NO_COMMAND,
        SEND_MEASURES
    };
    ManualMode();

    void activate();
    void deactivate();
    bool isActive();
    ManualBundle run();

private:
    bool active;
    Alarm manualAlarm;
    static volatile bool manualAlarmFlag;

    static void onManualAlarm(void *arg);
};

#endif // MANUALMODE_H