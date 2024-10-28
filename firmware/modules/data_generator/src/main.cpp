#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char trama[50];
int estado=0;
int jj=0;

String mensajes[] = {
    "00000000,07.00,080.00,20.00,20\n",
    "00000001,07.50,085.00,21.00,30\n",
    "00000002,07.00,080.00,22.00,40\n",
    "00000003,07.50,075.00,21.00,50\n",
    "00000004,07.00,080.00,20.00,60\n",
    "00000005,07.50,085.00,19.00,70\n",
    "00000006,07.00,080.00,18.00,80\n",
    "00000007,07.50,075.00,19.00,70\n",
    "00000008,07.00,080.00,20.00,60\n",
    "00000009,07.50,085.00,21.00,50\n",
    "00000010,07.00,080.00,22.00,40\n",
    "00000011,07.50,075.00,21.00,30\n",
    "00000012,07.00,080.00,20.00,20\n",
    "00000000,07.00,080.00,20.00,20\n",
    "00000001,07.50,085.00,21.00,30\n",
    "00000002,07.00,080.00,22.00,40\n",
    "00000003,07.50,075.00,21.00,50\n",
    "00000004,07.00,080.00,20.00,60\n",
    "00000005,07.50,085.00,19.00,70\n",
    "00000006,07.00,080.00,18.00,80\n",
    "00000007,07.50,075.00,19.00,70\n",
    "00000008,07.00,080.00,20.00,60\n",
    "00000009,07.50,085.00,21.00,50\n",
    "00000010,07.00,080.00,22.00,40\n",
    "00000011,07.50,075.00,21.00,30\n",
    "00000012,07.00,080.00,20.00,20\n",
    "00000000,07.00,080.00,20.00,20\n",
    "00000001,07.50,085.00,21.00,30\n",
    "00000002,07.00,080.00,22.00,40\n",
    "00000003,07.50,075.00,21.00,50\n",
    "00000004,07.00,080.00,20.00,60\n",
    "00000005,07.50,085.00,19.00,70\n",
    "00000006,07.00,080.00,18.00,80\n",
    "00000007,07.50,075.00,19.00,70\n",
    "00000008,07.00,080.00,20.00,60\n",
    "00000009,07.50,085.00,21.00,50\n",
    "00000010,07.00,080.00,22.00,40\n",
    "00000011,07.50,075.00,21.00,30\n",
    "00000012,07.00,080.00,20.00,20\n",
    "00000000,07.00,080.00,20.00,20\n",
    "00000001,07.50,085.00,21.00,30\n",
    "00000002,07.00,080.00,22.00,40\n",
    "00000003,07.50,075.00,21.00,50\n",
    "00000004,07.00,080.00,20.00,60\n",
    "00000005,07.50,085.00,19.00,70\n",
    "00000006,07.00,080.00,18.00,80\n",
    "00000007,07.50,075.00,19.00,70\n",
    "00000008,07.00,080.00,20.00,60\n",
    "00000009,07.50,085.00,21.00,50\n",
    "00000010,07.00,080.00,22.00,40\n",
    "00000011,07.50,075.00,21.00,30\n",
    "00000012,07.00,080.00,20.00,20\n",
    "00000000,07.00,080.00,20.00,20\n",
    "00000001,07.50,085.00,21.00,30\n",
    "00000002,07.00,080.00,22.00,40\n",
    "00000003,07.50,075.00,21.00,50\n",
    "00000004,07.00,080.00,20.00,60\n",
    "00000005,07.50,085.00,19.00,70\n",
    "00000006,07.00,080.00,18.00,80\n",
    "00000007,07.50,075.00,19.00,70\n",
    "00000008,07.00,080.00,20.00,60\n",
    "00000009,07.50,085.00,21.00,50\n",
    "00000010,07.00,080.00,22.00,40\n",
    "00000011,07.50,075.00,21.00,30\n",
    "00000012,07.00,080.00,20.00,20\n",
    "00000000,07.00,080.00,20.00,20\n",
    "00000001,07.50,085.00,21.00,30\n",
    "00000002,07.00,080.00,22.00,40\n",
    "00000003,07.50,075.00,21.00,50\n",
    "00000004,07.00,080.00,20.00,60\n",
    "00000005,07.50,085.00,19.00,70\n",
    "00000006,07.00,080.00,18.00,80\n",
    "00000007,07.50,075.00,19.00,70\n",
    "00000008,07.00,080.00,20.00,60\n",
    "00000009,07.50,085.00,21.00,50\n",
    "00000010,07.00,080.00,22.00,40\n",
    "00000011,07.50,075.00,21.00,30\n",
    "00000012,07.00,080.00,20.00,20\n",
    "00000000,07.00,080.00,20.00,20\n",
    "00000001,07.50,085.00,21.00,30\n",
    "00000002,07.00,080.00,22.00,40\n",
    "00000003,07.50,075.00,21.00,50\n",
    "00000004,07.00,080.00,20.00,60\n",
    "00000005,07.50,085.00,19.00,70\n",
    "00000006,07.00,080.00,18.00,80\n",
    "00000007,07.50,075.00,19.00,70\n",
    "00000008,07.00,080.00,20.00,60\n",
    "00000009,07.50,085.00,21.00,50\n",
    "00000010,07.00,080.00,22.00,40\n",
    "00000011,07.50,075.00,21.00,30\n",
    "00000012,07.00,080.00,20.00,20\n",
    "00000000,07.00,080.00,20.00,20\n",
    "00000001,07.50,085.00,21.00,30\n",
    "00000002,07.00,080.00,22.00,40\n",
    "00000003,07.50,075.00,21.00,50\n",
    "00000004,07.00,080.00,20.00,60\n",
    "00000005,07.50,085.00,19.00,70\n",
    "00000006,07.00,080.00,18.00,80\n",
    "00000007,07.50,075.00,19.00,70\n",
    "00000008,07.00,080.00,20.00,60\n",
    "00000009,07.50,085.00,21.00,50\n",
    "00000010,07.00,080.00,22.00,40\n",
    "00000011,07.50,075.00,21.00,30\n",
    "00000012,07.00,080.00,20.00,20\n",
    "00000000,07.00,080.00,20.00,20\n",
    "00000001,07.50,085.00,21.00,30\n",
    "00000002,07.00,080.00,22.00,40\n",
    "00000003,07.50,075.00,21.00,50\n",
    "00000004,07.00,080.00,20.00,60\n",
    "00000005,07.50,085.00,19.00,70\n",
    "00000006,07.00,080.00,18.00,80\n",
    "00000007,07.50,075.00,19.00,70\n",
    "00000008,07.00,080.00,20.00,60\n",
    "00000009,07.50,085.00,21.00,50\n",
    "00000010,07.00,080.00,22.00,40\n",
    "00000011,07.50,075.00,21.00,30\n",
    "00000012,07.00,080.00,20.00,20\n",
    "00000000,07.00,080.00,20.00,20\n",
    "00000001,07.50,085.00,21.00,30\n",
    "00000002,07.00,080.00,22.00,40\n",
    "00000003,07.50,075.00,21.00,50\n",
    "00000004,07.00,080.00,20.00,60\n",
    "00000005,07.50,085.00,19.00,70\n",
    "00000006,07.00,080.00,18.00,80\n",
    "00000007,07.50,075.00,19.00,70\n",
    "00000008,07.00,080.00,20.00,60\n",
    "00000009,07.50,085.00,21.00,50\n",
    "00000010,07.00,080.00,22.00,40\n",
    "00000011,07.50,075.00,21.00,30\n",
    "00000012,07.00,080.00,20.00,20\n",
    "00000000,07.00,080.00,20.00,20\n",
    "00000001,07.50,085.00,21.00,30\n",
    "00000002,07.00,080.00,22.00,40\n",
    "00000003,07.50,075.00,21.00,50\n",
    "00000004,07.00,080.00,20.00,60\n",
    "00000005,07.50,085.00,19.00,70\n",
    "00000006,07.00,080.00,18.00,80\n",
    "00000007,07.50,075.00,19.00,70\n",
    "00000008,07.00,080.00,20.00,60\n",
    "00000009,07.50,085.00,21.00,50\n",
    "00000010,07.00,080.00,22.00,40\n",
    "00000011,07.50,075.00,21.00,30\n",
    "00000012,07.00,080.00,20.00,20\n",
    "00000000,07.00,080.00,20.00,20\n",
    "00000001,07.50,085.00,21.00,30\n",
    "00000002,07.00,080.00,22.00,40\n",
    "00000003,07.50,075.00,21.00,50\n",
    "00000004,07.00,080.00,20.00,60\n",
    "00000005,07.50,085.00,19.00,70\n",
    "00000006,07.00,080.00,18.00,80\n",
    "00000007,07.50,075.00,19.00,70\n",
    "00000008,07.00,080.00,20.00,60\n",
    "00000009,07.50,085.00,21.00,50\n",
    "00000010,07.00,080.00,22.00,40\n",
    "00000011,07.50,075.00,21.00,30\n",
    "00000012,07.00,080.00,20.00,20\n",
    "00000000,07.00,080.00,20.00,20\n",
    "00000001,07.50,085.00,21.00,30\n",
    "00000002,07.00,080.00,22.00,40\n",
    "00000003,07.50,075.00,21.00,50\n",
    "00000004,07.00,080.00,20.00,60\n",
    "00000005,07.50,085.00,19.00,70\n",
    "00000006,07.00,080.00,18.00,80\n",
    "00000007,07.50,075.00,19.00,70\n",
    "00000008,07.00,080.00,20.00,60\n",
    "00000009,07.50,085.00,21.00,50\n",
    "00000010,07.00,080.00,22.00,40\n",
    "00000011,07.50,075.00,21.00,30\n",
    "00000012,07.00,080.00,20.00,20\n",
    "00000000,07.00,080.00,20.00,20\n",
    "00000001,07.50,085.00,21.00,30\n",
    "00000002,07.00,080.00,22.00,40\n",
    "00000003,07.50,075.00,21.00,50\n",
    "00000004,07.00,080.00,20.00,60\n",
    "00000005,07.50,085.00,19.00,70\n",
    "00000006,07.00,080.00,18.00,80\n",
    "00000007,07.50,075.00,19.00,70\n",
    "00000008,07.00,080.00,20.00,60\n",
    "00000009,07.50,085.00,21.00,50\n",
    "00000010,07.00,080.00,22.00,40\n",
    "00000011,07.50,075.00,21.00,30\n",
    "00000012,07.00,080.00,20.00,20\n",
    "00000000,07.00,080.00,20.00,20\n",
    "00000001,07.50,085.00,21.00,30\n",
    "00000002,07.00,080.00,22.00,40\n",
    "00000003,07.50,075.00,21.00,50\n",
    "00000004,07.00,080.00,20.00,60\n",
    "00000005,07.50,085.00,19.00,70\n",
    "00000006,07.00,080.00,18.00,80\n",
    "00000007,07.50,075.00,19.00,70\n",
    "00000008,07.00,080.00,20.00,60\n",
    "00000009,07.50,085.00,21.00,50\n",
    "00000010,07.00,080.00,22.00,40\n",
    "00000011,07.50,075.00,21.00,30\n",
    "00000012,07.00,080.00,20.00,20\n",
    "00000000,07.00,080.00,20.00,20\n",
    "00000001,07.50,085.00,21.00,30\n",
    "00000002,07.00,080.00,22.00,40\n",
    "00000003,07.50,075.00,21.00,50\n",
    "00000004,07.00,080.00,20.00,60\n",
    "00000005,07.50,085.00,19.00,70\n",
    "00000006,07.00,080.00,18.00,80\n",
    "00000007,07.50,075.00,19.00,70\n",
    "00000008,07.00,080.00,20.00,60\n",
    "00000009,07.50,085.00,21.00,50\n",
    "00000010,07.00,080.00,22.00,40\n",
    "00000011,07.50,075.00,21.00,30\n",
    "00000012,07.00,080.00,20.00,20\n",
    "00000000,07.00,080.00,20.00,20\n",
    "00000001,07.50,085.00,21.00,30\n",
    "00000002,07.00,080.00,22.00,40\n",
    "00000003,07.50,075.00,21.00,50\n",
    "00000004,07.00,080.00,20.00,60\n",
    "00000005,07.50,085.00,19.00,70\n",
    "00000006,07.00,080.00,18.00,80\n",
    "00000007,07.50,075.00,19.00,70\n",
    "00000008,07.00,080.00,20.00,60\n",
    "00000009,07.50,085.00,21.00,50\n",
    "00000010,07.00,080.00,22.00,40\n",
    "00000011,07.50,075.00,21.00,30\n",
    "00000012,07.00,080.00,20.00,20\n",
    "00000000,07.00,080.00,20.00,20\n",
    "00000001,07.50,085.00,21.00,30\n",
    "00000002,07.00,080.00,22.00,40\n",
    "00000003,07.50,075.00,21.00,50\n",
    "00000004,07.00,080.00,20.00,60\n",
    "00000005,07.50,085.00,19.00,70\n",
    "00000006,07.00,080.00,18.00,80\n",
    "00000007,07.50,075.00,19.00,70\n",
    "00000008,07.00,080.00,20.00,60\n",
    "00000009,07.50,085.00,21.00,50\n",
    "00000010,07.00,080.00,22.00,40\n",
    "00000011,07.50,075.00,21.00,30\n",
    "00000012,07.00,080.00,20.00,20\n",
    "00000000,07.00,080.00,20.00,20\n",
    "00000001,07.50,085.00,21.00,30\n",
    "00000002,07.00,080.00,22.00,40\n",
    "00000003,07.50,075.00,21.00,50\n",
    "00000004,07.00,080.00,20.00,60\n",
    "00000005,07.50,085.00,19.00,70\n",
    "00000006,07.00,080.00,18.00,80\n",
    "00000007,07.50,075.00,19.00,70\n",
    "00000008,07.00,080.00,20.00,60\n",
    "00000009,07.50,085.00,21.00,50\n",
    "00000010,07.00,080.00,22.00,40\n",
    "00000011,07.50,075.00,21.00,30\n",
    "00000012,07.00,080.00,20.00,20\n"
};

String mensajes_short[] = {
    "00000000,07.00,080.00,20.00,20\n",
    "00000001,07.50,075.00,30.00,30\n",
    "00000002,07.00,070.00,40.00,40\n",
    "00000003,07.50,065.00,50.00,50\n",
    "00000004,07.00,060.00,60.00,60\n",
    "00000005,07.50,080.00,20.00,70\n",
    "00000006,07.00,075.00,30.00,80\n",
    "00000007,07.50,070.00,40.00,90\n",
    "00000008,07.00,065.00,50.00,10\n",
    "00000009,07.50,060.00,60.00,10\n",
    "00000010,07.00,080.00,20.00,20\n",
    "00000011,07.50,075.00,30.00,30\n",
    "00000012,07.00,070.00,40.00,40\n",
    "00000013,07.50,065.00,50.00,50\n",
    "00000014,07.00,060.00,60.00,60\n",
    "00000015,07.50,080.00,20.00,70\n",
    "00000016,07.00,075.00,30.00,80\n",
    "00000017,07.50,070.00,40.00,90\n",
    "00000018,07.00,065.00,50.00,00\n",
    "00000019,07.50,060.00,60.00,10\n",
    "00000020,07.00,080.00,20.00,20\n",
    "00000021,07.50,075.00,30.00,30\n",
    "00000022,07.00,070.00,40.00,40\n",
    "00000023,07.50,065.00,50.00,50\n",
    "00000024,07.00,060.00,60.00,60\n",
    "00000025,07.50,080.00,20.00,70\n",
    "00000026,07.00,075.00,30.00,80\n",
    "00000027,07.50,070.00,40.00,90\n",
    "00000028,07.00,065.00,50.00,00\n",
    "00000029,07.50,060.00,60.00,10\n",
    "00000030,07.00,080.00,20.00,20\n",
    "00000031,07.50,075.00,30.00,30\n",
    "00000032,07.00,070.00,40.00,40\n",
    "00000033,07.50,065.00,50.00,50\n",
    "00000034,07.00,060.00,60.00,60\n",
    "00000035,07.50,080.00,20.00,70\n",
    "00000036,07.00,075.00,30.00,80\n",
    "00000037,07.50,070.00,40.00,90\n",
    "00000038,07.00,065.00,50.00,00\n",
    "00000039,07.50,060.00,60.00,10\n",
    "00000040,07.00,080.00,20.00,20\n",
    "00000041,07.50,075.00,30.00,30\n",
    "00000042,07.00,070.00,40.00,40\n",
    "00000043,07.50,065.00,50.00,50\n",
    "00000044,07.00,060.00,60.00,60\n",
    "00000045,07.50,080.00,20.00,70\n",
    "00000046,07.00,075.00,30.00,80\n",
    "00000047,07.50,070.00,40.00,90\n",
    "00000048,07.00,065.00,50.00,00\n",
    "00000049,07.50,060.00,60.00,10\n",
    "00000050,07.00,080.00,20.00,20\n",
    "00000051,07.50,075.00,30.00,30\n",
    "00000052,07.00,070.00,40.00,40\n",
    "00000053,07.50,065.00,50.00,50\n",
    "00000054,07.00,060.00,60.00,60\n",
    "00000055,07.50,080.00,20.00,70\n",
    "00000056,07.00,075.00,30.00,80\n",
    "00000057,07.50,070.00,40.00,90\n",
    "00000058,07.00,065.00,50.00,00\n",
    "00000059,07.50,060.00,60.00,10\n",
    "00000060,07.00,080.00,20.00,20\n",
    "00000061,07.50,075.00,30.00,30\n",
    "00000062,07.00,070.00,40.00,40\n",
    "00000063,07.50,065.00,50.00,50\n",
    "00000064,07.00,060.00,60.00,60\n",
    "00000065,07.50,080.00,20.00,70\n"
};

unsigned long previousMillis = 0;  
const unsigned long interval = 15000; 
unsigned long currentMillis = 0;
unsigned long ii = 0;

int totalMensajes = sizeof(mensajes) / sizeof(mensajes[0]);
int totalMensajes_short = sizeof(mensajes_short) / sizeof(mensajes_short[0]);

void generar_trama(char *trama) {
    // Generar valores aleatorios para cada campo
    int val0 = jj*15;                 
    int val1 = rand() % 100;                 // 00 a 99
    float val2 = (rand() % 10000) / 100.0;   // 0.00 a 99.99
    float val3 = (rand() % 1000000) / 100.0; // 0.00 a 9999.99
    float val4 = (rand() % 10000) / 100.0;   // 0.00 a 99.99
    int val5 = rand() % 2;                  // 0 a 9
    int val6 = rand() % 2;                  // 0 a 9
    int val7 = rand() % 2;                  // 0 a 9
    int val8 = rand() % 2;                  // 0 a 9
    jj++;

    // Formatear la trama en el buffer proporcionado
    sprintf(trama, "#F%06d%02d%04d%06d%04d%d%d%d%d!", 
            val0, val1, 
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
                if (Serial.readString().equals("#INIT!")) {
                    Serial.printf("#ESP!\n");

                    // Serial.printf("#STA0!\n");
                    // Serial.printf("00000000,07.00,080.00,20.00,20\n");
                    // estado = 2;

                    Serial.printf("#STA1!\n");
                    estado = 1;

                    //Serial.printf("#STA2!\n");
                    //estado = 1;
                    
                } else {
                    Serial.printf("noup\n");
                }
            }
            break;
        case 1:
            Serial.printf("#SYN0!\n");
            for (int i = 0; i < totalMensajes_short; i++) {
                Serial.println(mensajes_short[i]); // Envía el string y agrega un salto de línea (\n)
                //delay(2000); // Espera 10 segundos (10000 milisegundos) antes de enviar el siguiente
            }
            Serial.printf("#SYN1!\n");
            estado = 2;
            break;
        case 2:
            if (Serial.available() > 0) {
                if (Serial.readString().equals("#Z1!")) {
                    Serial.printf("#Z1!\n");
                    estado = 0;
                }
            }
            currentMillis = millis();
            if (currentMillis - previousMillis >= interval) {
                previousMillis = currentMillis;  
                Serial.println(mensajes_short[ii]);
                ii++;
            }
            break;
    }
    
}

