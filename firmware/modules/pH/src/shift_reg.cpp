#include "shift_reg.h"

extern byte shift_outputs;

void init_shift_reg() {
    pinMode(SHIFT_LATCH, OUTPUT);
    pinMode(SHIFT_CLOCK, OUTPUT);  
    pinMode(SHIFT_DATA,  OUTPUT);
}

void shift_reg_handle() {
    digitalWrite(SHIFT_LATCH, LOW);
    shiftOut(SHIFT_DATA, SHIFT_CLOCK, LSBFIRST, shift_outputs);
    digitalWrite(SHIFT_LATCH, HIGH);    
}