#include <Arduino.h>
#include <ArduinoLog.h>
#include <cycle_manager.h>

cycle_manager cm;

void setup()
{
    Serial.begin(460800);
    Log.begin(LOG_LEVEL_NOTICE, &Serial, true);
    Log.notice("Starting...\n");
    delay(1000);
    cm.resetHeaderForDebug();
    cm.begin();
}

void loop()
{
    static unsigned long lastTime = 0;
    unsigned long currentTime = millis();

    if (currentTime - lastTime >= 5000) // Test every second
    {
        lastTime = currentTime;
        if (cm.readNextInterval())
        {
            Log.infoln("Interval Read Correct");
        }
        else
        {
            Log.error("Failed to read next interval\n");
        }
    }
}
