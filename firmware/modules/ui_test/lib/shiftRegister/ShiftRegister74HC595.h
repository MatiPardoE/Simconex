#ifndef SHIFTREGISTER74HC595_H
#define SHIFTREGISTER74HC595_H

#include <Arduino.h>
//#include <ArduinoLog.h>

class ShiftRegister74HC595 {
  private:
    int dataPin;   // Pin de DATA
    int latchPin;  // Pin de LATCH
    int clockPin;  // Pin de CLOCK
    byte outputState;  // Estado actual de las salidas del shift register

  public:
    // Constructor
    ShiftRegister74HC595(int data, int latch, int clock);

    // Actualiza el estado de una salida específica
    void setOutput(int pin, bool state);

    // Actualiza el registro de desplazamiento con el estado actual
    void updateShiftRegister();

    // Obtener el estado actual de las salidas
    byte getOutputState();
};




#endif
