// cycle_manager.h
#ifndef CYCLE_MANAGER_H
#define CYCLE_MANAGER_H

#include <Arduino.h>
#include <SD.h>

// #define __HARDCODE_DATA__ true

class cycle_manager
{
public:
    struct IntervalData
    {
        int interval_id;
        float ph;
        float oxygen;
        float temperature;
        float light;
    } intervalData;
    cycle_manager();
    bool begin();
    bool readNextInterval(IntervalData &intervalData);

    bool readInterval(cycle_manager::IntervalData &intervalData);

private:
    // Private variables
    // Private Structs
    enum CycleStatus
    {
        NO_CYCLE_IN_SD,
        CYCLE_NEW,
        CYCLE_RUNNING,
        CYCLE_PAUSED,
        CYCLE_COMPLETED,
        CYCLE_ERROR
    } cycleStatus;

    enum CheckNextInterval
    {
        INTERVAL_ERROR,
        INTERVAL_AVAILABLE,
        NO_MORE_INTERVALS
    };

    struct CycleData
    {
        String cycle_name;
        String cycle_id;
        String state;
        int interval_time;
        int interval_total;
        int interval_current;
    } cycleData;

    // Private functions
    void parseHeader(const String &data);
    bool checkInputHeader(String &header);
    CheckNextInterval readAndWriteCurrentIntervalFromCSV();
    void logIntervalDataforDebug(const IntervalData &intervalData);
};

#endif // CYCLE_MANAGER_H