#include <Arduino.h>
#include <ArduinoLog.h>
#include <cycle_manager.h>
#include <commUI.h>
#include <file_transfer.h>

#define SPI_MISO 34
#define SPI_MOSI 33
#define SPI_SS 26
#define SPI_CLK 25
#define SD_CS_PIN SPI_SS

cycle_manager cm(SPI_CLK, SPI_MISO, SPI_MOSI, SPI_SS); // Pide los pines SPI de la SD
CommUI commUI;
FileTransfer fileTransfer(Serial, SD_CS_PIN);

void setup()
{
    commUI.begin(230400); // Solo define el puerto y velocidad de comunicación
    Log.begin(LOG_LEVEL_VERBOSE, &Serial, true);
    Log.notice("Starting...\n");
    delay(350);
    if (cm.begin(SD_CS_PIN)) // Inicializa la SD y lee el header
    {
        Log.notice("Cycle manager initialized\n");
    }
    else
    {
        Log.error("Cycle manager failed to initialize\n");
    }
}

void loop()
{
    static CommUI::CommandFromUI commandUI;

    commandUI = commUI.run();

    switch (commandUI)
    {
    case CommUI::TRANSFER_FILE_START:
        // Log.notice("Transfer file start\n");
        Serial.println("#OK!"); // TODO Create function to send commands to UI
        fileTransfer.transferFiles("/input_test/header.csv", "/input_test/data.csv", 10000);
        break;
    case CommUI::UNKNOWN_COMMAND:
        // Do nothing
        break;
    case CommUI::INIT_COMM_UI:
        Serial.println("#ESP!"); // TODO: el estado del ciclo lo tengo que obtener desde el header
        //Serial.printf("#STA0!\n"); // no hay un ciclo en curso
        Serial.printf("#STA1!\n"); // hay un ciclo en curso
        Serial.printf("00000000,07.00,080.00,20.00,20\n"); // valores instantaneos
        break;
    case CommUI::SYNC_CYCLE_START:
        Serial.println("#OK!");
        Serial.println("#ID0!");
        fileTransfer.transferCycle("/input_test/header.csv", "/input_output/cycle_out.csv", 10000);
        break;
    default:
        Log.warning("Unknown command\n");
        break;
    }
}

// TODO: Implementar la alarma por software y que ya la clase cycle_manager la maneje cuando lee el header.
