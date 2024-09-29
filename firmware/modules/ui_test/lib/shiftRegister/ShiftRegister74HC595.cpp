#include "ShiftRegister74HC595.h"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

  
// Constructor
ShiftRegister74HC595::ShiftRegister74HC595(int data, int latch, int clock) {
    dataPin = data;
    latchPin = latch;
    clockPin = clock;
    outputState = 0;  // Inicializa todas las salidas en 0

    // Configura los pines como salidas
    pinMode(dataPin, OUTPUT);
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);

    // // Log de inicialización
    // Log.info("Shift register initialized: DATA pin %d, LATCH pin %d, CLOCK pin %d\n", dataPin, latchPin, clockPin);
}

// Actualiza el estado de una salida específica
void ShiftRegister74HC595::setOutput(int pin, bool state) {
    if (pin < 0 || pin > 7) {
        // Log.error("Invalid pin %d. Pin must be between 0 and 7.\n", pin);
        return;
    }

    if (state) {
        outputState |= (1 << pin);  // Establece el bit en 1
        // Log.info("Set pin %d to HIGH.\n", pin);
    } else {
        outputState &= ~(1 << pin);  // Establece el bit en 0
        // Log.info("Set pin %d to LOW.\n", pin);
    }

    updateShiftRegister();  // Actualiza el registro de desplazamiento
}

// Actualiza el registro de desplazamiento con el estado actual
void ShiftRegister74HC595::updateShiftRegister() {
    digitalWrite(latchPin, LOW);  // Baja LATCH para comenzar la transmisión
    // Log.info("Updating shift register with state: 0b" BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(outputState));

    shiftOut(dataPin, clockPin, MSBFIRST, outputState);  // Envía los datos

    digitalWrite(latchPin, HIGH);  // Sube LATCH para actualizar las salidas
    // Log.info("Shift register updated.\n");
}

// Obtener el estado actual de las salidas
byte ShiftRegister74HC595::getOutputState() {
    // Log.info("Current shift register state: 0b" BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(outputState));
    return outputState;
}
