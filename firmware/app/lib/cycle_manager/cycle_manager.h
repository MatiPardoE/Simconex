// cycle_manager.h
#ifndef CYCLE_MANAGER_H
#define CYCLE_MANAGER_H

#include <Arduino.h>
#include <SD.h>

#define __HARDCODE_DATA__ true

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
        bool synchronization;
        uint8_t checksum;
    } intervalData;
    cycle_manager();
    bool begin();
    bool readNextInterval(IntervalData &intervalData);

private:

    //Private variables
    //Private Structs
    enum CycleStatus
    {
        NO_CYCLE_IN_SD,
        CYCLE_NEW,
        CYCLE_RUNNING,
        CYCLE_PAUSED,
        CYCLE_COMPLETED,
        CYCLE_ERROR
    } cycleStatus;
    struct CycleData
    {
        String cycle_name;
        String cycle_id;
        String state;
        int interval_time;
        int interval_total;
        int interval_current;
    } cycleData;
    
    //Private functions
    void parseHeader(const String &data);
    bool checkInputHeader(String &header);
};

#endif // CYCLE_MANAGER_H