#include <Arduino.h>
#include <ArduinoLog.h>
#include "LedStrip.h"

LedStrip ledStrip;

void setup() {
    Serial.begin(115200);

    // Configurar el logger para usar Serial
    Log.begin(LOG_LEVEL_VERBOSE, &Serial);

    ledStrip.begin(2, 0, 5000, 8); // Configura el pin 2 Led on Board, canal 0, frecuencia de 5000 Hz, resolución de 8 bits
}

void loop() {
    ledStrip.setDuty(1); // Setear duty a 100%
    delay(3000); // Esperar 3 segundos
    ledStrip.setDuty(0); // Setear duty a 0%
    delay(3000); // Esperar 3 segundos
}
