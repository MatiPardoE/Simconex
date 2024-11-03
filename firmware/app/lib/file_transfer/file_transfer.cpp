#include "file_transfer.h"

FileTransfer::FileTransfer(HardwareSerial &serial, int chipSelectPin)
    : _serial(serial), _chipSelectPin(chipSelectPin) {}

FileTransfer::TransferStatus FileTransfer::transferCycle(const char *destPathHeader, const char *pathDataOut, unsigned long timeout)
{
    int retryCount = 0;
    SyncCycleStatus syncCycleStatus = SEND_TIMESTAMP;
    dataBuffer.reserve(BLOCK_SIZE);
    startTime = millis();

    while (true)
    {
        String response;

        if (millis() - startTime > TIMEOUT)
        {
            return FILE_TRANSFER_TIMEOUT;
        }
        switch(syncCycleStatus) {
            case SEND_TIMESTAMP:
                response = _serial.readStringUntil('\n');
                if (response == "#OK!") {
                    response = getCycleID(destPathHeader);
                    _serial.println(response);
                    syncCycleStatus = SEND_ID_1;
                    startTime = millis(); // reset timeout
                }
                break;
            case SEND_ID_1:
                response = _serial.readStringUntil('\n');
                if (response == "#OK!") {
                    _serial.println("#ID1!");
                    syncCycleStatus = SEND_DATAOUT_0;
                    startTime = millis(); // reset timeout
                }
                break;
            case SEND_DATAOUT_0:
                response = _serial.readStringUntil('\n');
                if (response == "#OK!") {
                    _serial.println("#DATAOUT0!");
                    syncCycleStatus = SEND_DATAOUT;
                    startTime = millis(); // reset timeout
                }
                break;
            case SEND_DATAOUT:
                response = _serial.readStringUntil('\n');
                if (response == "#OK!") {
                    if(!sendDataOutput(pathDataOut)) {
                        return FILE_TRANSFER_ERROR;
                    }
                    _serial.println("#DATAOUT1!");
                    syncCycleStatus = WAIT_OK_DATAOUT_1;
                    startTime = millis(); // reset timeout
                }
                break;
            case WAIT_OK_DATAOUT_1:
                response = _serial.readStringUntil('\n');
                if (response == "#OK!") {
                    response = _serial.readStringUntil('\n');
                    if (response == "#SYNC1!") { // termino el proceso de sincronizacion
                        _serial.println("#OK!");
                        startTime = millis(); // reset timeout
                        return FILE_TRANSFER_DONE;
                    }
                }
                break;
            
        }   
    }
}


FileTransfer::TransferStatus FileTransfer::transferFiles(const char *destPathHeader, const char *destPathData, unsigned long timeout)
{
    int retryCount = 0;
    dataBuffer.reserve(BLOCK_SIZE);
    startTime = millis();

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
        if (millis() - startTime > TIMEOUT)
        {
            return FILE_TRANSFER_TIMEOUT;
        }

        if (_serial.available())
        {
            String command;

            startTime = millis(); // reset timeout

            if (dataOpen)
            {
                command = _serial.readStringUntil('\n');
                if (command == "#DATA1!")
                {
                    // End of data transfer - write remaining buffer
                    if (!writeBufferToFile())
                    {
                        return FILE_TRANSFER_ERROR;
                    }
                    _serial.println("#OK!");
                    destFileData.close();
                    dataOpen = false;
                }
                else
                {
                    if (validateLine(command))
                    {
                        dataBuffer.push_back(command);
                        message_block++;
                    }
                    else
                    {
                        dataBuffer.clear();
                        message_block = 0;
                        if (retryCount == MAX_RETRIES)
                        {
                            _serial.println("#FAIL!");
                            return FILE_TRANSFER_ERROR;
                        }
                        retryCount++;
                        delay(300); // Agrego este delay para que siga tirando todo los datos basura y despues limpio el puerto y mando fail
                        flush_serial();
                        _serial.println("#FAIL!");
                    }
                    if (message_block == BLOCK_SIZE)
                    {
                        if (!writeBufferToFile())
                        {
                            // Este caso es si hay error de escritura en SD
                            _serial.println("#ERROR!");
                            return FILE_TRANSFER_ERROR;
                        }
                        _serial.println("#OK!");
                        message_block = 0;
                    }
                }
            }
            else
            {
                command = _serial.readStringUntil('\n');
            }

            // Handle commands
            if (command == "#HEADER0!")
            {
                _serial.println("#OK!");
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
                _serial.println("#OK!");
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
            else if (command == "#TRANSFER1!")
            {
                _serial.println("#OK!");
                return FILE_TRANSFER_DONE;
            }
            else if (headerOpen)
            {
                destFileHeader.println(command);
            }
        }
    }
}

bool FileTransfer::sendDataOutput(const char* filename) {
    bool ret = true;
    String response;
    String blockContent;
    File file = SD.open(filename);
    int lineCount = 0;

    while (file.available()) {
        String line = file.readStringUntil('\n');
        blockContent += line + "\n";
        lineCount++;

        if (lineCount == BLOCK_SIZE) {
            lineCount = 0;
            if (!sendBlockAndWaitForResponse(blockContent)) {
                ret = false;
                break;
            }
            lineCount = 0;
            blockContent = "";
        }
    }

    if (lineCount > 0) {
        if (!sendBlockAndWaitForResponse(blockContent)) {
            ret = false;
        }
    }

    file.close();  
    return ret;
}

bool FileTransfer::sendBlockAndWaitForResponse(String blockContent) {
    int retryCount = 0;
    bool success = false;

    while (retryCount < MAX_RETRIES) {
        Serial.println(blockContent);

        unsigned long startTime = millis();
        bool responseReceived = false;
        
        while (millis() - startTime < TIMEOUT) { 
            if (Serial.available() > 0) {
                String response = Serial.readStringUntil('\n');
                response.trim();

                if (response == "#OK!") {
                    success = true;
                    responseReceived = true;
                    retryCount = 0;
                    break;
                } else if (response == "#FAIL!") {
                    retryCount++;
                    responseReceived = true;
                    break;
                }
            }
        }
        if (responseReceived && success) {
            break;
        } else if (!responseReceived) {
            retryCount++;
        }
    }
    return success;
}

String FileTransfer::getCycleID(const char* filename) {
    File file = SD.open(filename);
    String cycle_id = "";
    while (file.available()) {
        String line = file.readStringUntil('\n');
        if (line.startsWith("cycle_id,")) {
            cycle_id = line.substring(line.indexOf(',') + 1);
            cycle_id.trim();
            break;
        }
    }
    file.close();
    return cycle_id;
}

bool FileTransfer::writeBufferToFile()
{
    if (dataBuffer.empty())
        return true;

    for (const String &line : dataBuffer)
    {
        if (!destFileData.println(line))
        {
            return false;
        }
    }
    dataBuffer.clear();
    return true;
}

bool FileTransfer::loadBufferFromFile()
{
    if (dataBuffer.empty())
        return true;

    for (const String &line : dataBuffer)
    {
        if (!destFileData.println(line))
        {
            return false;
        }
    }
    dataBuffer.clear();
    return true;
}

bool FileTransfer::validateLine(const String &line)
{
    // Check if line ends with newline and has correct length
    if (line.length() != LINE_LENGTH)
    {
        Log.errorln("Error in lenght: %s ", line.c_str());
        return false;
    }

    // Validate format: 8 digits, comma, 5 chars, comma, 6 chars, comma, 5 chars, comma, 2-3 chars
    // Example: 00000000,07.00,080.00,20.00,20

    if (!isDigit(line[0]))
    {
        Log.errorln("Error en primer caracter: %s ", line.c_str());
        return false; // First char must be digit
    }

    // Basic structure check esto esta mal solo chequea en 
    if (line[8] != ',' || line[14] != ',' || line[21] != ',' || line[27] != ','){
        Log.errorln("Error en estructura: %s ", line.c_str());
        return false;
    }
    return true;
}

void FileTransfer::flush_serial()
{
    while (_serial.available())
        _serial.read();
}