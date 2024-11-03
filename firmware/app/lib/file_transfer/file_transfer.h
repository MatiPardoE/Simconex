#ifndef FILE_TRANSFER_H
#define FILE_TRANSFER_H

#include <Arduino.h>
#include <SD.h>
#include <ArduinoLog.h>
#include <vector>

class FileTransfer
{
public:
    enum TransferStatus
    {
        FILE_TRANSFER_DONE,
        FILE_TRANSFER_TIMEOUT,
        FILE_TRANSFER_ERROR
    };

    enum SyncCycleStatus
    {
        SEND_ID_0,
        WAIT_OK_ID_0,
        SEND_TIMESTAMP,
        WAIT_OK_TIMESTAMP,
        SEND_ID_1,
        WAIT_OK_ID_1,
        SEND_DATAOUT_0,
        WAIT_OK_DATAOUT_0,
        SEND_DATAOUT,
        WAIT_OK_DATAOUT,
        SEND_DATAOUT_1,
        WAIT_OK_DATAOUT_1
    };

    FileTransfer(HardwareSerial &serial, int chipSelectPin);
    bool checkCommand();
    TransferStatus transferFiles(const char *destPathHeader, const char *destPathData, unsigned long timeout);
    TransferStatus transferCycle(const char *destPathHeader, const char *pathDataOut, unsigned long timeout);

private:
    HardwareSerial &_serial;
    int _chipSelectPin;
    void writeFile(const char *filename, const char *data);
    void clearBuffer();
    void flush_serial();
    bool writeBufferToFile();
    bool loadBufferFromFile();
    bool validateLine(const String &line);
    String getCycleID(const char* filename);
    bool sendDataOutput(const char* filename);
    bool sendBlockAndWaitForResponse(String blockContent);

    File destFileData;
    File destFileHeader;
    std::vector<String> dataBuffer;
    unsigned long startTime;
    bool headerOpen = false;
    bool dataOpen = false;
    int message_block = 0;

    static const int BLOCK_SIZE = 160;          // Number of lines to buffer
    static const int LINE_LENGTH = 30;         // Expected length of each line
    static const unsigned long TIMEOUT = 5000; // 5 seconds timeout
    static const int MAX_RETRIES = 5;
};

#endif // FILE_TRANSFER_H