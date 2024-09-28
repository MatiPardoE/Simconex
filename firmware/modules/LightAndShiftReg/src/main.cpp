#include <Arduino.h>
#include <ArduinoLog.h>
#include "LedStrip.h"
#include "ShiftRegister74HC595.h"

/////////// PINES DE NUESTRO MICROCONTROLADOR ///////////
/////////// LED STRIPS //////
#define PIN_LED_STRIP_1 5
#define PIN_LED_STRIP_2 18
#define PIN_LED_STRIP_3 19
#define PIN_LED_STRIP_4 21
#define PIN_LED_STRIP_5 22
/////////// SHIFT REGISTER //////
#define SR_DATA_PIN 15
#define SR_LATCH_PIN 2
#define SR_CLOCK_PIN 0

LedStrip ledStrip1, ledStrip2, ledStrip3, ledStrip4, ledStrip5;
ShiftRegister74HC595 shiftRegister(SR_DATA_PIN, SR_LATCH_PIN, SR_CLOCK_PIN); // Conexiones: DATA = 15, LATCH = 2, CLOCK = 0

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
    ledStrip1.setDuty(1);   // Setear duty a 50%
    ledStrip2.setDuty(5);   // Setear duty a 30%
    ledStrip3.setDuty(15);  // Setear duty a 70%
    ledStrip4.setDuty(40);  // Setear duty a 90%
    ledStrip5.setDuty(100); // Setear duty a 10%
    shiftRegister.setOutput(0, HIGH); // Encender el primer bit del shift register
    shiftRegister.setOutput(1, HIGH); // Encender el segundo bit del shift register
    shiftRegister.setOutput(2, HIGH); // Encender el tercer bit del shift register
    shiftRegister.setOutput(3, LOW); // Encender el cuarto bit del shift register
    shiftRegister.setOutput(4, HIGH); // Encender el quinto bit del shift register
    shiftRegister.setOutput(5, HIGH); // Encender el sexto bit del shift register
    shiftRegister.setOutput(6, LOW); // Encender el séptimo bit del shift register
    shiftRegister.setOutput(7, HIGH); // Encender el octavo bit del shift register
    delay(3000); // Esperar 3 segundos
    shiftRegister.setOutput(0, LOW); // Apagar el primer bit del shift register
    shiftRegister.setOutput(1, LOW); // Apagar el segundo bit del shift register
    shiftRegister.setOutput(2, LOW); // Apagar el tercer bit del shift register
    shiftRegister.setOutput(3, HIGH); // Apagar el cuarto bit del shift register
    shiftRegister.setOutput(4, LOW); // Apagar el quinto bit del shift register
    shiftRegister.setOutput(5, LOW); // Apagar el sexto bit del shift register
    shiftRegister.setOutput(6, HIGH); // Apagar el séptimo bit del shift register
    shiftRegister.setOutput(7, LOW); // Apagar el octavo bit del shift register
    ledStrip1.setDuty(0); // Setear duty a 0%
    ledStrip2.setDuty(0); // Setear duty a 0%
    ledStrip3.setDuty(0); // Setear duty a 0%
    ledStrip4.setDuty(0); // Setear duty a 0%
    ledStrip5.setDuty(0); // Setear duty a 0%
    delay(3000);          // Esperar 3 segundos
}
