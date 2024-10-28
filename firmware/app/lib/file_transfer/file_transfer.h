#ifndef FILE_TRANSFER_H
#define FILE_TRANSFER_H

#include <Arduino.h>
#include <SD.h>
#include <ArduinoLog.h>

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
};

#endif // FILE_TRANSFER_H