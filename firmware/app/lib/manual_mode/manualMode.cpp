#include "manualMode.h"

volatile bool ManualMode::manualAlarmFlag = false;

ManualMode::ManualMode()
{
    // Constructor implementation
}

void ManualMode::activate()
{
    if (active)
    {
        return;
    }
    ESP_LOGI("ManualMode", "Activating manual mode");
    active = true;
    manualAlarm.setAlarm(30, onManualAlarm);
    neoPixel.setState(NeoPixel::MANUAL_MODE);
}

bool ManualMode::isActive()
{
    return active;
}

ManualMode::ManualBundle ManualMode::run()
{
    ManualBundle bundle;
    if (manualAlarmFlag)
    {
        //ESP_LOGI("ManualMode", "Alarma de modo manual mode");
        manualAlarmFlag = false;
        bundle = ManualBundle::SEND_MEASURES;
    }
    else
    {
        bundle = ManualBundle::NO_COMMAND;
    }
    return bundle;
}

void ManualMode::onManualAlarm(void *arg)
{
    manualAlarmFlag = true;
}

void ManualMode::deactivate()
{
    if (!active)
    {
        return;
    }
    active = false;
    manualAlarmFlag = false;
    manualAlarm.stopAndDeleteAlarm();
    ESP_LOGI("ManualMode", "Desactivando manual mode");
    neoPixel.setState(NeoPixel::OFF);
}
