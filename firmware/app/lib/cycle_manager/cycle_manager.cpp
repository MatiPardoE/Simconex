#include <SD.h> // Ensure the SD library is installed and included correctly
#include <SPI.h>
#include <ArduinoLog.h>
#include <cycle_manager.h>

#define SPI_MISO 34
#define SPI_MOSI 33
#define SPI_SS 26
#define SPI_CLK 25
#define SD_CS_PIN SPI_SS // Cambia según tu configuración

const char *hardcodedHEADER = R"(
cycle_name,csv base
cycle_id,csv_base_20241013_182343
state,new_cycle
interval_time,15
interval_total,3
interval_current,0
)";

const char *hardcodedDATA = R"(
0,0,0,0,0,__________________________
1,4.31,104.45,21.35,10,_____________
2,2,,,,_____________________________
3,2.43,45,15,9,_____________________
)";

cycle_manager::cycle_manager()
{
}

// Inicializa la tarjeta SD
bool cycle_manager::begin()
{
    String header = "";

#if __HARDCODE_DATA__ == true
    Log.infoln("Using hardcoded data");
    header = String(hardcodedHEADER);

#else
    Log.infoln("Using SD data");
    // TODO: a chequear cuando se implemente el SPI y la SD
    // SPI.begin(SPI_CLK, SPI_MISO, SPI_MOSI, SPI_SS);
    // SPI.setDataMode(SPI_MODE0);
    //  Initialize SD card
    if (!SD.begin(SD_CS_PIN))
    {
        Log.errorln("SD Initialization failed!");
        return false;
    }
    else
    {
        Log.infoln("SD Initialization done.");
    }

    if (!checkInputHeader(header))
    {
        return false;
    }
#endif

    // Parse the data
    parseHeader(header);

    Log.infoln("Header Read OK with this data:");
    Log.infoln("Cycle Name: %s", cycleData.cycle_name.c_str());
    Log.infoln("Cycle ID: %s", cycleData.cycle_id.c_str());
    Log.infoln("State: %s", cycleData.state.c_str());
    Log.infoln("Interval Time: %d", cycleData.interval_time);
    Log.infoln("Interval Total: %d", cycleData.interval_total);
    Log.infoln("Interval Current: %d", cycleData.interval_current);

    // TODO: Implementar esto en el parseHeader y eliminar la variable cycleData.state xq
    switch (cycleData.state[0]) // no admite string en switch
    {
    case 'n': // 'new'
        cycleStatus = CYCLE_NEW;
        break;
    case 'r': // 'running'
        cycleStatus = CYCLE_RUNNING;
        break;
    case 'p': // 'paused'
        cycleStatus = CYCLE_PAUSED;
        break;
    case 'c': // 'completed'
        cycleStatus = CYCLE_COMPLETED;
        break;
    default:
        // Handle unknown state
        Log.errorln("Unknown state: %s", cycleData.state.c_str());
        cycleStatus = CYCLE_ERROR;
        return false;
    }
    // TODO: Check datos de header sean coherentes

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
bool cycle_manager::checkInputHeader(String &header)
{
    File file = SD.open("/input/header.csv");
    if (!file)
    {
        cycleStatus = NO_CYCLE_IN_SD;
        Log.errorln("Failed to open input header file");
        return false;
    }

    while (file.available())
    {
        String line = file.readStringUntil('\n');
        header += line + "\n";
    }

    file.close();
    return true;
}

bool cycle_manager::readNextInterval(IntervalData &intervalData)
{
#ifdef __HARDCODE_DATA__ == true
    Log.infoln("Using hardcoded data");
    
#else
    File file = SD.open("/input/data.csv");
    if (!file)
    {
        cycleStatus = NO_CYCLE_IN_SD;
        Log.errorln("Failed to open input data file");
        return false;
    }

    // Leer la primera fila para calcular el ancho
    String firstLine = file.readStringUntil('\n');
    int rowWidth = firstLine.length() + 1; // +1 para incluir el salto de línea

    // Calcular la posición de la fila actual
    int targetLine = cycleData.interval_current + 1; // +1 para moverse al siguiente intervalo
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

        intervalData.interval_id = line.substring(start, end).toInt();
        start = end + 1;
        end = line.indexOf(',', start);
        intervalData.ph = line.substring(start, end).toFloat();
        start = end + 1;
        end = line.indexOf(',', start);
        intervalData.oxygen = line.substring(start, end).toFloat();
        start = end + 1;
        end = line.indexOf(',', start);
        intervalData.temperature = line.substring(start, end).toFloat();
        start = end + 1;
        end = line.indexOf(',', start);
        intervalData.light = line.substring(start, end).toInt();

        file.close();
        return true;
    }
    else
    {
        Log.errorln("No more intervals available");
        file.close();
        return false;
    }

#endif
}