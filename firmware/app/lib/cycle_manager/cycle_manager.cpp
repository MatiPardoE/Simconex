#include <SD.h> // Ensure the SD library is installed and included correctly
#include <SPI.h>
#include <ArduinoLog.h>
#include <cycle_manager.h>

#define SD_CS_PIN 5 // Cambia según tu configuración

const char *hardcodedCSV = R"(
cycle_name,csv base,,,
cycle_id,csv_base_20241012_131635,,,
state,new,,,
interval_time,15,,,
interval_total,2,,,
interval_current,0,,,
header_finish,,,,
interval_id,ph,oxygen,temperature,light
0,0,0,0,0
1,4.31,104.45,21.35,10
2,2,,,
)";

cycle_manager::cycle_manager()
{
    // Initialize SD card
    if (!SD.begin(SD_CS_PIN))
    {
        Log.errorln("SD Initialization failed!");
        return;
    }
    Log.infoln("SD Initialization done.");
}

// Inicializa la tarjeta SD
bool cycle_manager::begin(bool useHardcodedData)
{
    String header = "";
    if (useHardcodedData) {
        Log.infoln("Using hardcoded data");
        header = String(hardcodedCSV);
    } else {
        if (!checkInputCSV(header)) {
            return false;
        }
    }

    // Parse the data
    parseHeader(header);

    Log.infoln("Header Read OK with this data:");
    Log.infoln("Cycle Name: %s", cycleData.cycle_name.c_str());
    Log.infoln("Cycle ID: %s", cycleData.cycle_id.c_str());
    Log.infoln("State: %s", cycleData.state.c_str());
    Log.infoln("Interval Time: %d", cycleData.interval_time);
    Log.infoln("Interval Total: %d", cycleData.interval_total);
    Log.infoln("Interval Current: %d", cycleData.interval_current);

    return true;
}

// Helper method to parse CSV header data
void cycle_manager::parseHeader(const String &data)
{
    int start = 0;
    int end = data.indexOf('\n');

    while (end != -1)
    {
        String line = data.substring(start, end);
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
            cycleData.state = line.substring(firstComma + 1, secondComma);
            cycleData.state.trim();
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
        end = data.indexOf('\n', start);
    }
}
bool cycle_manager::checkInputCSV(String &header) {
    File root = SD.open("/input");
    File file;
    int fileCount = 0;
    String fileName;

    while (true) {
        File entry = root.openNextFile();
        if (!entry) {
            break; // no more files
        }
        if (!entry.isDirectory()) {
            fileCount++;
            fileName = String(entry.name());
        }
        entry.close();
    }

    if (fileCount != 1) {
        Log.errorln("There should be exactly one file in the directory, found %d", fileCount);
        return false;
    }

    if (!fileName.endsWith(".csv")) {
        Log.errorln("The file found is not a .csv file: %s", fileName.c_str());
        return false;
    }

    file = SD.open(fileName.c_str());
    if (!file) {
        Log.errorln("Failed to open input file");
        return false;
    }

    while (file.available()) {
        String line = file.readStringUntil('\n');
        header += line + "\n";
        if (line.startsWith("header_finish")) {
            break;
        }
    }

    file.close();
    return true;
}