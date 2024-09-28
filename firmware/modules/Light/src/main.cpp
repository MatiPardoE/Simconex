#include <Arduino.h>
#include <ArduinoLog.h>
#include "LedStrip.h"

/////////// PINES DE NUESTRO MICROCONTROLADOR ///////////
#define PIN_LED_STRIP_1 5
#define PIN_LED_STRIP_2 18
#define PIN_LED_STRIP_3 19
#define PIN_LED_STRIP_4 21
#define PIN_LED_STRIP_5 22

LedStrip ledStrip1, ledStrip2, ledStrip3, ledStrip4, ledStrip5;

void setup()
{
    Serial.begin(460800);

    // Configurar el logger para usar Serial
    Log.begin(LOG_LEVEL_VERBOSE, &Serial);

    ledStrip1.begin(PIN_LED_STRIP_1, 0, 5000, 8); // Configura el pin 5, canal 0, frecuencia de 5000 Hz, resolución de 8 bits
    ledStrip2.begin(PIN_LED_STRIP_2, 1, 5000, 8); // Configura el pin 18, canal 1, frecuencia de 5000 Hz, resolución de 8 bits
    ledStrip3.begin(PIN_LED_STRIP_3, 2, 5000, 8); // Configura el pin 19, canal 2, frecuencia de 5000 Hz, resolución de 8 bits
    ledStrip4.begin(PIN_LED_STRIP_4, 3, 5000, 8); // Configura el pin 21, canal 3, frecuencia de 5000 Hz, resolución de 8 bits
    ledStrip5.begin(PIN_LED_STRIP_5, 4, 5000, 8); // Configura el pin 22, canal 4, frecuencia de 5000 Hz, resolución de 8 bits
}

void loop()
{
    ledStrip1.setDuty(1); // Setear duty a 50%
    ledStrip2.setDuty(5); // Setear duty a 30%
    ledStrip3.setDuty(15); // Setear duty a 70%
    ledStrip4.setDuty(40); // Setear duty a 90%
    ledStrip5.setDuty(100); // Setear duty a 10%
    delay(3000);           // Esperar 3 segundos
    ledStrip1.setDuty(0);  // Setear duty a 0%
    ledStrip2.setDuty(0); // Setear duty a 0%
    ledStrip3.setDuty(0); // Setear duty a 0%
    ledStrip4.setDuty(0); // Setear duty a 0%
    ledStrip5.setDuty(0); // Setear duty a 0%
    delay(3000);          // Esperar 3 segundos
}
