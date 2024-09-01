#include <Arduino.h>

#define SHIFT_LATCH 2     
#define SHIFT_CLOCK 0      
#define SHIFT_DATA  15 

#define agua_fria_on()          shift_outputs |= 0b00000001
#define agua_fria_off()         shift_outputs &= 0b11111110

#define electrovalvula_3_on()   shift_outputs |= 0b00000010
#define electrovalvula_3_off()  shift_outputs &= 0b11111101

#define electrovalvula_2_on()   shift_outputs |= 0b00000100
#define electrovalvula_2_off()  shift_outputs &= 0b11111011

#define electrovalvula_1_on()   shift_outputs |= 0b00001000
#define electrovalvula_1_off()  shift_outputs &= 0b11110111

#define agua_sucia_on()         shift_outputs |= 0b00010000
#define agua_sucia_off()        shift_outputs &= 0b11101111

#define agua_limpia_on()        shift_outputs |= 0b00100000
#define agua_limpia_off()       shift_outputs &= 0b11011111

#define agua_caliente_on()      shift_outputs |= 0b01000000
#define agua_caliente_off()     shift_outputs &= 0b10111111

void init_shift_reg();
void shift_reg_handle();