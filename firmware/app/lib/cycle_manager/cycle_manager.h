// cycle_manager.h
#ifndef CYCLE_MANAGER_H
#define CYCLE_MANAGER_H

#include <Arduino.h>
#include <SD.h>
#include <alarm.h>

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
        int light;
    };
    enum CommandBundle
    {
        NO_COMMAND,
        START_CYCLE,
        PAUSE_CYCLE,
        RESUME_CYCLE,
        STOP_CYCLE,
        NEW_INTERVAL,
    };
    struct CycleBundle
    {
        IntervalData intervalData;
        CommandBundle command;
    };
    enum CycleStatus
    {
        NO_CYCLE_IN_SD,
        CYCLE_NEW,
        CYCLE_RUNNING,
        CYCLE_PAUSED,
        CYCLE_COMPLETED,
        CYCLE_ERROR
    };
    struct CycleData
    {
        String cycle_name;
        String cycle_id;
        CycleStatus status;
        uint64_t interval_time;
        int interval_total;
        int interval_current;
    } cycleData;
    cycle_manager(uint8_t SPI_CLK, uint8_t SPI_MISO, uint8_t SPI_MOSI, uint8_t SPI_SS);
    bool begin(u_int8_t SD_CS_PIN);
    CycleBundle run();
    bool readNextInterval();
    bool readInterval();
    bool resetHeaderForDebug(u_int8_t SD_CS_PIN);
    // Getter for intervalData
    const IntervalData &getIntervalData() const;

private:
    // Private variables
    IntervalData intervalData;
    Alarm cycleAlarm;
    static volatile bool alarmFlag;
    // Constants
    const String headerPath = "/input/header.csv";
    const String dataPath = "/input/data.csv";
    // Private Structs
    enum CheckNextInterval
    {
        INTERVAL_ERROR,
        INTERVAL_AVAILABLE,
        NO_MORE_INTERVALS
    };

    enum analyzeHeaderState
    {
        HEADER_ERROR,
        HEADER_AVAILABLE,
        HEADER_NOT_AVAILABLE
    };

    // Private functions
    analyzeHeaderState analyzeHeader();
    CheckNextInterval readAndWriteCurrentIntervalFromCSV();
    void logIntervalDataforDebug(const IntervalData &intervalData);
    static void onAlarm(void *arg); // Make onAlarm static
    bool evaluateAlarmStatus();
    String cycleStatusToString(CycleStatus status);
};

#endif // CYCLE_MANAGER_H