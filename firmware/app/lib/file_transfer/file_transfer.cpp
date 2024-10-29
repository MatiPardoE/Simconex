#include "file_transfer.h"

FileTransfer::FileTransfer(HardwareSerial &serial, int chipSelectPin)
    : _serial(serial), _chipSelectPin(chipSelectPin) {}

FileTransfer::TransferStatus FileTransfer::transferFiles(const char *destPathHeader, const char *destPathData, unsigned long timeout)
{
    // TODO: Implementar creacion de directorio si no existe y ademas borrar el archivo header y data si ya existe.
    unsigned long startTime = millis(); // i will restart the timeout if there is comm
    File destFileHeader;
    File destFileData;
    bool headerOpen = false;
    bool dataOpen = false;
    uint8_t message_block = 0;
    uint8_t message_block_ok = 0;
    uint16_t timeout_transfer_block = 250;

    if (SD.exists(destPathHeader))
    {
        SD.remove(destPathHeader);
        // Log.infoln("Removed existing header file");
    }

    if (SD.exists(destPathData))
    {
        SD.remove(destPathData);
        // Log.infoln("Removed existing header file");
    }

    while (true)
    {
        if (millis() - startTime > timeout)
        {
            return FILE_TRANSFER_TIMEOUT;
        }

        if (_serial.available())
        {
            String command = _serial.readStringUntil('\n');
            startTime = millis(); // reset the timeout
            if (command == "#HEADER0!")
            {
                Serial.println("#OK!"); // TODO Create function to send commands to UI
                destFileHeader = SD.open(destPathHeader, FILE_WRITE);
                if (!destFileHeader)
                {
                    Log.errorln("Failed to open header file for writing");
                    return FILE_TRANSFER_ERROR;
                }
                headerOpen = true;
            }
            else if (command == "#HEADER1!" && headerOpen)
            {
                Serial.println("#OK!");
                destFileHeader.close();
                headerOpen = false;
            }
            else if (command == "#DATA0!")
            {
                Serial.println("#OK!");
                destFileData = SD.open(destPathData, FILE_WRITE);
                if (!destFileData)
                {
                    Log.errorln("Failed to open data file for writing");
                    return FILE_TRANSFER_ERROR;
                }
                dataOpen = true;
            }
            else if (command == "#DATA1!" && dataOpen)
            {
                Serial.println("#OK!");
                destFileData.close();
                dataOpen = false;
            }
            else if (command == "#TRANSFER1!")
            {
                Serial.println("#OK!");
                Log.infoln("Transfer done");
                return FILE_TRANSFER_DONE;
            }
            else
            {
                if (headerOpen)
                {
                    destFileHeader.println(command);
                }
                else if (dataOpen)
                {
                    if (command.length() == 30)
                    {
                        destFileData.println(command);
                        message_block_ok++;
                    }
                    else
                    {
                        Log.errorln("Mensaje de transferencia con error");
                    }
                    message_block++;
                    if (message_block == BLOCK_SIZE_ESP_UI)
                    {
                        if (message_block_ok == message_block)
                        {
                            Serial.println("#OK!");
                            message_block = 0;
                            message_block_ok = 0;
                        }
                        else
                        {
                            Serial.println("#FAIL!");
                            // TODO Logica de recupero de bloque, capaz es necesaria capaz no (depende de la frecuencias de error)
                        }
                    }
                }
            }
        }
    }
}

void FileTransfer::writeFile(const char *filename, const char *data)
{
    File file = SD.open(filename, FILE_WRITE);
    if (!file)
    {
        _serial.println("Failed to open file for writing");
        return;
    }

    file.print(data);
    file.close();
}