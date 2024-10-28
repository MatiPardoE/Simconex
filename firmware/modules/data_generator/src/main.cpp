#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char trama[30];
int estado=0;

void generar_trama(char *trama) {
    // Generar valores aleatorios para cada campo
    int val1 = rand() % 100;                 // 00 a 99
    float val2 = (rand() % 10000) / 100.0;   // 0.00 a 99.99
    float val3 = (rand() % 1000000) / 100.0; // 0.00 a 9999.99
    float val4 = (rand() % 10000) / 100.0;   // 0.00 a 99.99
    int val5 = rand() % 2;                  // 0 a 9
    int val6 = rand() % 2;                  // 0 a 9
    int val7 = rand() % 2;                  // 0 a 9
    int val8 = rand() % 2;                  // 0 a 9

    // Formatear la trama en el buffer proporcionado
    sprintf(trama, "#F%02d%04d%06d%04d%d%d%d%d!", 
            val1, 
            (int)(val2 * 100), 
            (int)(val3 * 100), 
            (int)(val4 * 100), 
            val5, val6, val7, val8);
}

void setup() {
    srand(time(NULL));
    Serial.begin(115200); 
}

void loop() {
    switch(estado){
        case 0:
            if (Serial.available() > 0) {
                if (Serial.readString().equals("INIT")) {
                    Serial.printf("ESP\n");
                    estado = 1;
                    Serial.printf("#L00!\n");
                    Serial.printf("#C0!\n");
                    Serial.printf("#O0!\n");
                    Serial.printf("#A0!\n");
                    Serial.printf("#W0!\n");
                }
            }
            break;
        case 1:
            generar_trama(trama);
            Serial.printf("%s\n", trama);
            delay(30000);
            break;
    }
    
}

