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
    FileTransfer(HardwareSerial &serial, int chipSelectPin);
    bool checkCommand();
    TransferStatus transferFiles(const char *destPathHeader, const char *destPathData, unsigned long timeout);

private:
    HardwareSerial &_serial;
    int _chipSelectPin;
    void writeFile(const char *filename, const char *data);
    void clearBuffer();
    void flush_serial();
    bool writeBufferToFile();
    bool validateLine(const String &line);

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