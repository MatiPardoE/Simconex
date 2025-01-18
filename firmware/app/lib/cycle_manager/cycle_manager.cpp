#include <SD.h> // Ensure the SD library is installed and included correctly
#include <SPI.h>
#include <ArduinoLog.h>
#include <cycle_manager.h>
#include "esp_log.h"

static const char *TAG = "cm";

const char *hardcodedHEADER = R"(
cycle_name,csv base
cycle_id,csv_base_20241013_182343
state,new_cycle
interval_time,10
interval_total,3
interval_current,0
)";

const char *hardcodedDATA = R"(
0,0,0,0,0,__________________________
1,4.31,104.45,21.35,10,_____________
2,2,,,,_____________________________
3,2.43,45,15,9,_____________________
)";

volatile bool cycle_manager::alarmFlag = false;

cycle_manager::cycle_manager(uint8_t SPI_CLK, uint8_t SPI_MISO, uint8_t SPI_MOSI, uint8_t SPI_SS)
{
    intervalData = {0, 0, 0, 0, 0};
    SPI.begin(SPI_CLK, SPI_MISO, SPI_MOSI, SPI_SS);
    SPI.setDataMode(SPI_MODE0);
}

/**
 * @brief Initializes the cycle manager.
 *
 * This function initializes the cycle manager by either using hardcoded data or reading data from an SD card.
 * It parses the header data and sets the cycle status based on the parsed state.
 *
 * @return true if initialization is successful, false otherwise.
 *
 * @note If using SD data, ensure that the SD card is properly initialized and the header data is valid.
 *
 * @details
 * - If `__HARDCODE_DATA__` is defined as true, the function uses hardcoded data.
 * - If `__HARDCODE_DATA__` is not defined or false, the function attempts to initialize the SD card and read the header data from it.
 * - The function logs the initialization process and the parsed header data.
 * - The cycle status is determined based on the first character of the `cycleData.state` string.
 *
 * @todo Implement SPI and SD initialization when using SD data.
 * @todo Implement header data coherence checks.
 * @todo Refactor `parseHeader` to eliminate the `cycleData.state` variable.
 */
bool cycle_manager::begin(uint8_t SD_CS_PIN)
{
    String header = "";
    if (!SD.begin(SD_CS_PIN))
    {
        ESP_LOGE(TAG,"SD Initialization failed!");
        cycleData.status = NO_CYCLE_IN_SD;
        return false;
    }
    else
    {
        ESP_LOGD(TAG,"SD Initialization done.");
    }

    if (analyzeHeaderandEvalAlarm())
    {
        ESP_LOGI(TAG,"Cycle Manager initialized successfully, header data OK, alarm status evaluated");
        return true;
    }
    else
    {
        ESP_LOGE(TAG,"Failed to initialize Cycle Manager");
        return false;
    }
    return true;
}

bool cycle_manager::analyzeHeaderandEvalAlarm()
{
    if (analyzeHeader() == HEADER_ERROR) // Parse the data TODO Handle the return value
    {
        ESP_LOGE(TAG,"Failed to parse header data");
        return false;
    }
    if (evaluateAlarmStatus())
    {
        ESP_LOGI(TAG,"Alarm status evaluated successfully");
    }
    else
    {
        ESP_LOGE(TAG,"Failed to evaluate alarm status");
        return false;
    }
    return true;
}

bool cycle_manager::writeMeasuresToSD(MeasuresAndOutputs measuresAndOutputs, uint32_t interval_id_measure)
{
    File file;

    // Check if the directory exists, if not create it
    if (!SD.exists("/output"))
    {
        if (!SD.mkdir("/output"))
        {
            ESP_LOGE(TAG,"Failed to create directory /output");
            return false;
        }
    }

    // Open the file for writing
    file = SD.open(dataOutPath, FILE_APPEND);
    if (!file)
    {
        ESP_LOGE(TAG,"Failed to open data output file for writing");
        return false;
    }
    // Write the measures to the file
    file.printf("%08d,%05.2f,%06.2f,%05.2f,%02d,%d,%d,%d,%d",
                interval_id_measure, measuresAndOutputs.ph, measuresAndOutputs.oxygen, measuresAndOutputs.temperature,
                measuresAndOutputs.light, measuresAndOutputs.EV_co2, measuresAndOutputs.EV_oxygen, measuresAndOutputs.EV_nitrogen, measuresAndOutputs.EV_air);
    file.println();
    file.close();

    sendDataToUI(measuresAndOutputs, interval_id_measure);
    return true;
}

bool cycle_manager::sendDataToUI(MeasuresAndOutputs measuresAndOutputs, uint32_t interval_id_measure)
{
    Serial.printf("%08d,%05.2f,%06.2f,%05.2f,%02d,%d,%d,%d,%d",
                  interval_id_measure, measuresAndOutputs.ph, measuresAndOutputs.oxygen, measuresAndOutputs.temperature,
                  measuresAndOutputs.light, measuresAndOutputs.EV_co2, measuresAndOutputs.EV_oxygen, measuresAndOutputs.EV_nitrogen, measuresAndOutputs.EV_air);
    Serial.println();
    return true;
}

// Helper method to parse CSV header data
cycle_manager::analyzeHeaderState cycle_manager::analyzeHeader()
{
    File file = SD.open(headerPath);
    if (!file)
    {
        ESP_LOGE(TAG,"Failed to open header  or no header file found");
        cycleData.status = NO_CYCLE_IN_SD;
        return HEADER_ERROR;
    }

    String header = "";
    while (file.available())
    {
        header += file.readStringUntil('\n') + "\n";
    }
    file.close();

    // Use parseHeader to update cycleData

    int start = 0;
    int end = header.indexOf('\n');
    String temp_status = "";
    while (end != -1)
    {
        String line = header.substring(start, end);
        int firstComma = line.indexOf(',');
        int secondComma = line.indexOf(',', firstComma + 1);

        if (line.startsWith("cycle_name"))
        {
            cycleData.cycle_name = line.substring(firstComma + 1, secondComma);
            cycleData.cycle_name.trim();
        }
        else if (line.startsWith("cycle_id"))
        {
            cycleData.cycle_id = line.substring(firstComma + 1, secondComma);
            cycleData.cycle_id.trim();
        }
        else if (line.startsWith("state"))
        {
            temp_status = line.substring(firstComma + 1, secondComma);
            temp_status.trim();
        }
        else if (line.startsWith("interval_time"))
        {
            cycleData.interval_time = line.substring(firstComma + 1, secondComma).toInt();
        }
        else if (line.startsWith("interval_total"))
        {
            cycleData.interval_total = line.substring(firstComma + 1, secondComma).toInt();
        }
        else if (line.startsWith("interval_current"))
        {
            cycleData.interval_current = line.substring(firstComma + 1, secondComma).toInt();
        }

        start = end + 1;
        end = header.indexOf('\n', start);
    }

    switch (temp_status[0]) // no admite string en switch
    {
    case 'n': // 'new_cycle'
        cycleData.status = CYCLE_RUNNING;
        break;
    case 'r': // 'running'
        cycleData.status = CYCLE_RUNNING;
        break;
    case 'p': // 'paused'
        cycleData.status = CYCLE_PAUSED;
        break;
    case 'c': // 'completed'
        cycleData.status = CYCLE_COMPLETED;
        break;
    default:
        // Handle unknown state
        ESP_LOGE(TAG,"Unknown state of cycle: %s", temp_status.c_str());
        cycleData.status = CYCLE_ERROR;
        return HEADER_ERROR;
        break;
    }

    return HEADER_AVAILABLE;
}

cycle_manager::CheckNextInterval cycle_manager::readAndWriteCurrentIntervalFromCSV()
{
    analyzeHeader(); // Actualizo valores del header

    if (cycleData.interval_current < 0)
    {
        return INTERVAL_ERROR;
    }
    if (cycleData.interval_current >= (cycleData.interval_total))
    {
        ESP_LOGI(TAG,"Detecto que no hay mas intervalos disponibles a traves del header");
        return NO_MORE_INTERVALS;
    }

    if (!writeHeaderToSD())
    {
        ESP_LOGE(TAG,"Failed to open header file for writing 1");
        return INTERVAL_ERROR;
    }

    return INTERVAL_AVAILABLE;
}

bool cycle_manager::writeHeaderToSD()
{
    File file = SD.open(headerPath, FILE_WRITE);
    if (!file)
    {
        ESP_LOGE(TAG,"Failed to open header file for writing 3");
        return false;
    }

    // WARN: Un apagon aca nos mata todo porque nos crashea el archivo header.csv
    file.print("cycle_name,");
    file.println(cycleData.cycle_name);
    file.print("cycle_id,");
    file.println(cycleData.cycle_id);
    file.print("state,");
    file.println(cycleStatusToString(cycleData.status));
    file.print("interval_time,");
    file.println(cycleData.interval_time);
    file.print("interval_total,");
    file.println(cycleData.interval_total);
    file.print("interval_current,");
    file.println(cycleData.interval_current + 1);
    file.close();

    return true;
}

/**
 * @brief Retrieves the first interval data when the cycle is running.
 *
 * This function checks if the cycle status is CYCLE_RUNNING. If it is,
 * it reads the interval data and sets the command to FIRST_INTERVAL_RUNNING.
 * Otherwise, it sets the command to NO_COMMAND.
 *
 * This function is important to handle the transfer of the first interval
 * to control.
 *
 * @return A CycleBundle containing the command and interval data.
 */
cycle_manager::CycleBundle cycle_manager::firstIntervalAtRunning()
{
    CycleBundle bundle;
    if (cycleData.status == CYCLE_RUNNING)
    {
        // readInterval();
        bundle.command = CommandBundle::FIRST_INTERVAL_RUNNING;
        bundle.intervalData = intervalData;
    }
    else
    {
        bundle.command = CommandBundle::NO_COMMAND;
    }
    return bundle;
}

cycle_manager::CycleBundle cycle_manager::run()
{
    CycleBundle bundle;
    if (alarmFlag) // la alarma solo se va a activar si el ciclo esta corriendo
    {
        alarmFlag = false;
        ESP_LOGE(TAG, "SE ACTIVO LA ALARMA");
        if (readNextInterval())
        {
            // ESP_LOGI(TAG,"New interval available");
            bundle.command = CommandBundle::NEW_INTERVAL;
            bundle.intervalData = intervalData;
        }
        else
        {
            // Termino el ciclo
            finishCycle();
            bundle.command = CommandBundle::FINISH_CYCLE;
            bundle.intervalData.interval_id = cycleData.interval_current;
        }
    }
    else
    {
        bundle.command = CommandBundle::NO_COMMAND;
    }
    return bundle;
}

/**
 * @brief Reads the next interval from a CSV file and updates the cycle status accordingly.
 *
 * This function attempts to read the next interval from a CSV file and updates the cycle status
 * based on the result. It logs appropriate messages for different outcomes and returns a boolean
 * indicating whether there are more intervals to process.
 *
 * @return true if there are more intervals to process, false otherwise.
 *
 * Possible outcomes:
 * - NO_MORE_INTERVALS: No more intervals are available, the cycle is marked as completed.
 * - INTERVAL_ERROR: An error occurred while reading the next interval.
 * - INTERVAL_AVAILABLE: The next interval is available and processed.
 */
bool cycle_manager::readNextInterval()
{
    int result = readAndWriteCurrentIntervalFromCSV();
    switch (result)
    {
    case NO_MORE_INTERVALS:
        ESP_LOGI(TAG,"No more intervals available, cycle finished");
        cycleData.status = CYCLE_COMPLETED;
        // TODO: Implementar que se cierre el ciclo
        return false;

    case INTERVAL_ERROR:
        ESP_LOGE(TAG,"[cycle_manager::readNextInterval()]Failed to read next interval");
        return false;

    case INTERVAL_AVAILABLE:
        // ESP_LOGI(TAG,"Interval available: %d", cycleData.interval_current);
        return readInterval();

    default:
        return false;
    }
    return false;
}

/**
 * @brief Reads the next interval data from the SD card.
 *
 * This function reads the next interval data from a CSV file located on the SD card.
 * It updates the provided IntervalData structure with the read values.
 *
 * @param intervalData Reference to an IntervalData structure where the read values will be stored.
 * @return true if the interval data was successfully read, false otherwise.
 *
 * @note The CSV file is expected to have the following columns in each row:
 *       interval_id, ph, oxygen, temperature, light
 *
 * @details The function performs the following steps:
 *          1. Opens the CSV file from the SD card.
 *          2. Reads the first line to calculate the row width.
 *          3. Calculates the position of the current interval row.
 *          4. Seeks to the calculated position in the file.
 *          5. Reads the next line and parses the interval data.
 *          6. Updates the intervalData structure with the parsed values.
 *          7. Logs the interval data for debugging purposes.
 *
 * @warning If the file cannot be opened or the seek operation fails, the function logs an error and returns false.
 *
 * @see cycle_manager::IntervalData
 */
bool cycle_manager::readInterval()
{
    File file = SD.open(dataPath);
    if (!file)
    {
        cycleData.status = NO_CYCLE_IN_SD;
        ESP_LOGE(TAG,"Failed to open input data file when reading next interval");
        return false;
    }

    // Leer la primera fila para calcular el ancho
    String firstLine = file.readStringUntil('\n');
    int rowWidth = firstLine.length() + 1; // +1 para incluir el salto de línea

    // Calcular la posición de la fila actual

    int targetLine = (cycleData.interval_current - 1)+1;
    // -1 porque mi valor ya lo actualice en el header (salvo cuando es la primera del ciclo)
    //+1 para moverse al siguiente intervalo
    int position = targetLine * rowWidth;

    // Mover el puntero a la posición calculada
    if (!file.seek(position))
    {
        ESP_LOGE(TAG,"Failed to seek to the target position");
        file.close();
        return false;
    }

    // Leer la siguiente línea
    if (file.available())
    {
        String line = file.readStringUntil('\n');
        int start = 0;
        int end = line.indexOf(',');

        // Parse interval_id
        intervalData.interval_id = line.substring(start, end).toInt();
        start = end + 1;
        end = line.indexOf(',', start);

        // Parse pH
        intervalData.ph = line.substring(start, end).toFloat();
        start = end + 1;
        end = line.indexOf(',', start);

        // Parse oxygen
        intervalData.oxygen = line.substring(start, end).toFloat();
        start = end + 1;
        end = line.indexOf(',', start);

        // Parse temperature
        intervalData.temperature = line.substring(start, end).toFloat();
        start = end + 1;
        end = line.indexOf(',', start);

        // Parse light
        String light_str = line.substring(start);
        intervalData.light = light_str.toInt();

        file.close();

        logIntervalDataforDebug(intervalData);
        return true;
    }
    else
    {
        ESP_LOGE(TAG,"[cycle_manager::readInterval()]Failed to read the next interval");
        file.close();
        return false;
    }
}

bool cycle_manager::finishCycle()
{
    ESP_LOGI(TAG,"Cycle finished.id: %s, %d intervals processed.", cycleData.cycle_id.c_str(), cycleData.interval_current);
    cycleData.status = CYCLE_COMPLETED;
    if (!evaluateAlarmStatus())
    {
        ESP_LOGE(TAG,"Failed to evaluate alarm status");
        return false;
    }
    if (!writeHeaderToSD())
    {
        ESP_LOGE(TAG,"Failed to write header to SD");
        return false;
    }
    return true;
}

void cycle_manager::logIntervalDataforDebug(const IntervalData &intervalData)
{
    ESP_LOGI(TAG,"ID: %d pH: %F Oxygen: %F Temp: %F Light: %d",
                 intervalData.interval_id,
                 intervalData.ph,
                 intervalData.oxygen,
                 intervalData.temperature,
                 intervalData.light);
}

void cycle_manager::onAlarm(void *arg)
{
    alarmFlag = true;
}

bool cycle_manager::evaluateAlarmStatus()
{
    // Check the alarm status
    switch (cycleData.status)
    {
    case NO_CYCLE_IN_SD:
        // Do nothing
        break;
    case CYCLE_RUNNING:
        ESP_LOGI(TAG, "Cycle running, setting alarm");
        cycleAlarm.setAlarm(cycleData.interval_time, cycle_manager::onAlarm);
        alarmFlag = true;
        break;
    case CYCLE_PAUSED:
        // Handle cycle paused
        ESP_LOGI(TAG, "Cycle paused, pausing alarm");
        cycleAlarm.pauseAlarm();
        break;
    case CYCLE_COMPLETED:
        ESP_LOGI(TAG, "Cycle completed, stop alarm");
        // Handle cycle completed
        cycleAlarm.stopAndDeleteAlarm();
        break;
    case CYCLE_ERROR:
        ESP_LOGI(TAG, "Cycle in error, stop alarm");
        // Handle cycle error
        cycleAlarm.stopAndDeleteAlarm();
        break;
    default:
        // Handle unknown status
        ESP_LOGE(TAG,"Unknown cycle status: %d", cycleData.status);
        return false;
        break;
    }
    return true;
}

String cycle_manager::cycleStatusToString(CycleStatus status)
{
    switch (status)
    {
    case NO_CYCLE_IN_SD:
        return "no_cycle_in_sd";
    case CYCLE_RUNNING:
        return "running_cycle";
    case CYCLE_PAUSED:
        return "paused_cycle";
    case CYCLE_COMPLETED:
        return "completed_cycle";
    case CYCLE_ERROR:
        return "error_cycle";
    default:
        return "Unknown status";
    }
    return String();
}

bool cycle_manager::resetHeaderForDebug(uint8_t SD_CS_PIN)
{
    if (!SD.begin(SD_CS_PIN))
    {
        ESP_LOGE(TAG,"SD Initialization failed!");
        return false;
    }
    else
    {
        ESP_LOGI(TAG,"SD Initialization done.");
    }

    File file = SD.open("/input/header.csv", FILE_WRITE);
    if (!file)
    {
        ESP_LOGE(TAG,"Failed to open header file for writing 2");
        return INTERVAL_ERROR;
    }
    file.print("cycle_name,");
    file.println("csv base");
    file.print("cycle_id,");
    file.println("csv_base_20241013_182343");
    file.print("state,");
    file.println("new_cycle");
    file.print("interval_time,");
    file.println("15");
    file.print("interval_total,");
    file.println("4");
    file.print("interval_current,");
    file.println("0");
    file.close();

    return true;
}