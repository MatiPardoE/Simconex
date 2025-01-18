#include <Arduino.h>
#include <ArduinoLog.h>
#include <cycle_manager.h>
#include <commUI.h>
#include <file_transfer.h>
#include <controlAPI.h>

#define SPI_MISO 34
#define SPI_MOSI 33
#define SPI_SS 26
#define SPI_CLK 25
#define SD_CS_PIN SPI_SS

cycle_manager cm(SPI_CLK, SPI_MISO, SPI_MOSI, SPI_SS); // Pide los pines SPI de la SD
CommUI commUI;
FileTransfer fileTransfer(Serial, SD_CS_PIN);
ControlAPI sensorControl;

pH pH_Device = pH(20, "EZO pH probe");

void setup()
{
    commUI.begin(230400); // Solo define el puerto y velocidad de comunicación
    Log.begin(LOG_LEVEL_VERBOSE, &Serial, true);
    Log.notice("Starting...\n");
    sensorControl.init();
    delay(350);
    if (!cm.begin(SD_CS_PIN)) // Inicializa la SD y lee el header
    {
        Log.error("Cycle manager failed to initialize\n");
    }
}

void loop()
{
    static CommUI::CommandFromUI commandUI;
    static cycle_manager::CycleBundle cycleBundle;
    static cycle_manager::MeasuresAndOutputs new_measure_outputs;
    cycle_manager::CycleBundle firstCycleBundle;
    // .run
    commandUI = commUI.run();
    cycleBundle = cm.run();
    sensorControl.run();
    switch (commandUI)
    {
    case CommUI::TRANSFER_FILE_START:
        // ------- BLOQUEANTE ---------
        // Log.notice("Transfer file start\n");
        Serial.println("#OK!"); // TODO Create function to send commands to UI
        fileTransfer.transferFiles(cm.headerPath.c_str(), cm.dataPath.c_str(),  cm.dataOutPath.c_str(), 10000);
        if (cm.begin(SD_CS_PIN)) // Inicializa la SD y lee el header
        {
            firstCycleBundle = cm.firstIntervalAtRunning();
            if (firstCycleBundle.command == cycle_manager::CommandBundle::FIRST_INTERVAL_RUNNING)
            {
                Log.notice("First Intervl set at beginning\n");
                sensorControl.set_control_var(firstCycleBundle.intervalData);
            }
        }
        else
        {
            Log.error("Cycle manager failed to initialize\n");
        }
        break;
    case CommUI::UNKNOWN_COMMAND:
        // Do nothing
        break;
    case CommUI::INIT_COMM_UI:
        Serial.println("#ESP!"); // Comando para inicializar la comunicación e identificar el puerto
        break;
    case CommUI::SYNC_CYCLE_START:
        // STA0 = NOT_CYCLE - STA1 = CYCLE_RUNNING - STA2 = CYCLE_FINISHED
        if (cm.cycleData.status == cycle_manager::NO_CYCLE_IN_SD)
        {
            Serial.println("#STA0!");
        }
        else if (cm.cycleData.status == cycle_manager::CYCLE_RUNNING)
        {
            Serial.println("#STA1!");
            fileTransfer.transferCycle(cm.headerPath.c_str(), cm.dataOutPath.c_str(), 10000);
        }
        else if (cm.cycleData.status == cycle_manager::CYCLE_COMPLETED)
        {
            Serial.println("#STA2!");
            fileTransfer.transferCycle(cm.headerPath.c_str(), cm.dataOutPath.c_str(), 10000);
        }
        break;
    default:
        Log.warning("Unknown command\n");
        break;
    }

    switch (cycleBundle.command)
    {
    case cycle_manager::NO_COMMAND:
        // Do nothing
        break;
    case cycle_manager::FINISH_CYCLE:
        //Termino el ciclo por lo que debo enviar el ultimo intervalo y finalizar el ciclo
        new_measure_outputs = sensorControl.takeMeasuresAndOutputs();                          // No deberia tardar mucho
        cm.writeMeasuresToSD(new_measure_outputs, (cycleBundle.intervalData.interval_id)); // Envio el ID-1 porque el ID es el siguiente intervalo
        Log.noticeln("Cycle FINISIHED");
        break;
    case cycle_manager::NEW_INTERVAL:
        // Log.noticeln("New interval available");
        new_measure_outputs = sensorControl.takeMeasuresAndOutputs();                          // No deberia tardar mucho
        cm.writeMeasuresToSD(new_measure_outputs, (cycleBundle.intervalData.interval_id - 1)); // Envio el ID-1 porque el ID es el siguiente intervalo
        sensorControl.set_control_var(cycleBundle.intervalData);
        break;
    default:
        break;
    }
}