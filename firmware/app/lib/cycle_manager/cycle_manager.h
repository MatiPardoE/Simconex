// cycle_manager.h
#ifndef CYCLE_MANAGER_H
#define CYCLE_MANAGER_H

#include <Arduino.h>
#include <SD.h>

class cycle_manager
{
public:
    cycle_manager();

    bool begin(bool useHardcodedData = false);

private:
    struct CycleData
    {
        String cycle_name;
        String cycle_id;
        String state;
        int interval_time;
        int interval_total;
        int interval_current;
    } cycleData;

    struct IntervalData
    {
        int interval_id;
        float ph;
        float oxygen;
        float temperature;
        float light;
        bool synchronization;
        uint8_t checksum;
    }intervalData;

    void parseHeader(const String &data);
    bool checkInputCSV(String &header);
};

#endif // CYCLE_MANAGER_H