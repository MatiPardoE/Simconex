#include <Arduino.h>
#include <ArduinoLog.h>
#include <cycle_manager.h>
#include <alarm.h>

#define SPI_MISO 34
#define SPI_MOSI 33
#define SPI_SS 26
#define SPI_CLK 25
#define SD_CS_PIN SPI_SS

cycle_manager cm(SPI_CLK, SPI_MISO, SPI_MOSI, SPI_SS); // Pide los pines SPI de la SD

void setup()
{
    Serial.begin(460800);
    Log.begin(LOG_LEVEL_VERBOSE, &Serial, true);
    Log.notice("Starting...\n");
    delay(200);
    cm.resetHeaderForDebug(SD_CS_PIN);
    cm.begin(SD_CS_PIN);
}

void loop()
{
    cm.run();
    delay(10);
}

// TODO: Implementar la alarma por software y que ya la clase cycle_manager la maneje cuando lee el header.
