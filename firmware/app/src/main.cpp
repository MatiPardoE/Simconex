#include <Arduino.h>
#include <ArduinoLog.h>
#include <cycle_manager.h>

cycle_manager cm;

void setup()
{
    Serial.begin(460800);
    Log.begin(LOG_LEVEL_VERBOSE, &Serial, true);
    Log.notice("Starting...\n");
    cm.begin();
}

void loop()
{
}
