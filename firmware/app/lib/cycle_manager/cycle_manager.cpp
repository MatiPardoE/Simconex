#include <SD.h> // Ensure the SD library is installed and included correctly
#include <SPI.h>
#include <ArduinoLog.h>
#include <cycle_manager.h>

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

#if __HARDCODE_DATA__ == true
    Log.infoln("Using hardcoded data");
    header = String(hardcodedHEADER);

#else
    Log.infoln("Using SD data");
    if (!SD.begin(SD_CS_PIN))
    {
        Log.errorln("SD Initialization failed!");
        cycleData.status = NO_CYCLE_IN_SD;
        return false;
    }
    else
    {
        Log.infoln("SD Initialization done.");
    }
#endif

    analyzeHeader();       // Parse the data TODO Handle the return value
    evaluateAlarmStatus(); // Evaluate the alarm status
    Log.infoln("Cycle Manager initialized successfully, header data OK");
    return true;
}

// Helper method to parse CSV header data
cycle_manager::analyzeHeaderState cycle_manager::analyzeHeader() // TODO: Cambiar esta funcion a updateVarsFromHeader y que haga la comm con la SD y todo
{
    File file = SD.open(headerPath);
    if (!file)
    {
        Log.errorln("Failed to open header file");
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
    case 'n': // 'new'
        cycleData.status = CYCLE_NEW;
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
        Log.errorln("Unknown state of cycle: %s", temp_status.c_str());
        cycleData.status = CYCLE_ERROR;
        return HEADER_ERROR;
        break;
    }

    Log.verboseln("Header Read OK, Cycle Name: %s, Cycle ID: %s, State: %s, Interval Time: %d, Interval Total: %d, Interval Current: %d",
                  cycleData.cycle_name.c_str(),
                  cycleData.cycle_id.c_str(),
                  cycleStatusToString(cycleData.status).c_str(),
                  cycleData.interval_time,
                  cycleData.interval_total,
                  cycleData.interval_current);

    return HEADER_AVAILABLE;
}

cycle_manager::CheckNextInterval cycle_manager::readAndWriteCurrentIntervalFromCSV()
{
    analyzeHeader(); // Actualizo valores del header

    if (cycleData.interval_current < 0)
    {
        return INTERVAL_ERROR;
    }

    if (cycleData.interval_current >= cycleData.interval_total)
    {
        return NO_MORE_INTERVALS;
    }

    // Increment interval_current and write back to the file
    cycleData.interval_current++;
    File file = SD.open(headerPath, FILE_WRITE);
    if (!file)
    {
        Log.errorln("Failed to open header file for writing");
        return INTERVAL_ERROR;
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
    file.println(cycleData.interval_current);
    file.close();

    return INTERVAL_AVAILABLE;
}

cycle_manager::CycleBundle cycle_manager::run()
{
    CycleBundle bundle;
    if (alarmFlag)
    {
        alarmFlag = false;
        if (readNextInterval())
        {
            Log.infoln("New interval available");
            bundle.command = CommandBundle::NEW_INTERVAL;
            bundle.intervalData = intervalData;
        }
        else
        {
            bundle.command = CommandBundle::STOP_CYCLE;
        }
    }
    else
    {
        bundle.command = CommandBundle::NO_COMMAND;
    }
    return bundle;
}

/**
 * @brief Reads the next interval data.
 *
 * This function attempts to read the next interval data from a CSV file. If the
 * __HARDCODE_DATA__ macro is defined, it will log a message indicating that hardcoded
 * data is being used and return false. Otherwise, it will read the current interval
 * from the CSV file and handle the result accordingly.
 *
 * @param intervalData A reference to an IntervalData object where the interval data
 *                     will be stored if available.
 * @return true if the next interval data is successfully read and available, false otherwise.
 */
bool cycle_manager::readNextInterval()
{
#ifdef __HARDCODE_DATA__
    Log.infoln("Using hardcoded data, cant check next interval. Cause it uses function from File library");
    return false;
#else
    int result = readAndWriteCurrentIntervalFromCSV();
    switch (result)
    {
    case NO_MORE_INTERVALS:
        Log.infoln("No more intervals available, cycle finished");
        cycleData.status = CYCLE_COMPLETED;
        // TODO: Implementar que se cierre el ciclo
        return false;

    case INTERVAL_ERROR:
        Log.errorln("Failed to read next interval");
        return false;

    case INTERVAL_AVAILABLE:
        Log.infoln("Interval available: %d", cycleData.interval_current);
        return readInterval();

    default:
        return false;
    }
#endif
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
        Log.errorln("Failed to open input data file when reading next interval");
        return false;
    }

    // Leer la primera fila para calcular el ancho
    String firstLine = file.readStringUntil('\n');
    int rowWidth = firstLine.length() + 1; // +1 para incluir el salto de línea

    // Calcular la posición de la fila actual

    int targetLine = (cycleData.interval_current - 1) + 1;
    // -1 porque mi valor ya lo actualice en el header
    //+1 para moverse al siguiente intervalo
    int position = targetLine * rowWidth;

    // Mover el puntero a la posición calculada
    if (!file.seek(position))
    {
        Log.errorln("Failed to seek to the target position");
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
        Log.errorln("Failed to read the next interval");
        file.close();
        return false;
    }
}

void cycle_manager::logIntervalDataforDebug(const IntervalData &intervalData)
{
    Log.noticeln("ID: %d pH: %F Oxygen: %F Temp: %F Light: %d",
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
    case CYCLE_NEW:
        // Do nothing
        cycleAlarm.setAlarm(cycleData.interval_time, cycle_manager::onAlarm);
        break;
    case CYCLE_RUNNING:
        cycleAlarm.setAlarm(cycleData.interval_time, cycle_manager::onAlarm);
        break;
    case CYCLE_PAUSED:
        // Handle cycle paused
        cycleAlarm.pauseAlarm();
        break;
    case CYCLE_COMPLETED:
        // Handle cycle completed
        cycleAlarm.stopAndDeleteAlarm();
        break;
    case CYCLE_ERROR:
        // Handle cycle error
        cycleAlarm.stopAndDeleteAlarm();
        break;
    default:
        // Handle unknown status
        Log.errorln("Unknown cycle status: %d", cycleData.status);
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
    case CYCLE_NEW:
        return "new_cycle";
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

const cycle_manager::IntervalData &cycle_manager::getIntervalData() const
{
    return intervalData;
}

bool cycle_manager::resetHeaderForDebug(uint8_t SD_CS_PIN)
{
    if (!SD.begin(SD_CS_PIN))
    {
        Log.errorln("SD Initialization failed!");
        return false;
    }
    else
    {
        Log.infoln("SD Initialization done.");
    }

    File file = SD.open("/input/header.csv", FILE_WRITE);
    if (!file)
    {
        Log.errorln("Failed to open header file for writing");
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