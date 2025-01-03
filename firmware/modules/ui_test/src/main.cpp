#include <Arduino.h>
#include "spi_sd.h"
#include "rtc.h"
#include "utilities.h"
#include "ezo_ph.h"
#include "LedStrip.h"
#include "ShiftRegister74HC595.h"
#include <rdoApiCode.h>
#include <ModbusMessage.h>
#include <esp32ModbusRTU.h>
#include <algorithm>

#define PIN_LED_STRIP_1 5
#define PIN_LED_STRIP_2 18
#define PIN_LED_STRIP_3 19
#define PIN_LED_STRIP_4 21
#define PIN_LED_STRIP_5 22

#define SR_DATA_PIN 15
#define SR_LATCH_PIN 2
#define SR_CLOCK_PIN 0

#define RELE_OUTPUT 23

#define PERIOD 5000

enum state_general_t {
    DESCONECTADO,
    CONECTADO,
    CONECTADO_FAKE
};

enum state_msg_t {
    ESPERO_INICIO,
    ESPERO_CLAVE,
    ESPERO_CIERRE
};

enum my_state_ph_t {
    ESPERANDO,
    LEYENDO,
    LEIDO,
    LEYENDO_RDO
};

DateTime date = DateTime(F(__DATE__), F(__TIME__)); 
pH pH_Device = pH(20, "EZO pH probe"); 
LedStrip ledStrip1, ledStrip2, ledStrip3, ledStrip4, ledStrip5;
ShiftRegister74HC595 shiftRegister(SR_DATA_PIN, SR_LATCH_PIN, SR_CLOCK_PIN); 

int clave = 0;
int valor = 0;
bool msg_valido = false;
unsigned long time_now = 0;

esp32ModbusRTU modbus(&Serial1, RDO_DE_RE_GPIO);  
uint32_t lastMillisRDO = -_TIMEOUT_RDO_REQUEST_; 
volatile rdo_t rdo;

int validar_clave(int byte){
    switch (byte) {
        case 'L':
        case 'P':
        case 'D':
        case 'T':
        case 'C':
        case 'O':
        case 'A':
        case 'W':
        case 'Z':
            return byte;
        
        default:
            return -1;
    }
}

void handler_ui(){
    static state_msg_t state_msg = ESPERO_INICIO;
    static my_state_ph_t state_ph = ESPERANDO;
    int byte;

    if(Serial.available() > 0){
        byte = Serial.read();
        switch(state_msg){
            case ESPERO_INICIO:
                if(byte == '#'){
                    state_msg = ESPERO_CLAVE;
                    valor = 0;
                }
                break;
            case ESPERO_CLAVE:
                clave = validar_clave(byte);
                if(clave != -1){
                    state_msg = ESPERO_CIERRE;
                } else {
                    state_msg = ESPERO_INICIO;
                }
                break;
            case ESPERO_CIERRE:
                if(byte >= '0' && byte <= '9'){
                    valor = (valor*10)+(byte-'0');
                } else if(byte == '!'){
                    state_msg = ESPERO_INICIO;
                    msg_valido = true;
                } else {
                    state_msg = ESPERO_INICIO;
                }
                break;
            default:
                state_msg = ESPERO_INICIO;
                msg_valido = false;
                break;
        }
    } else {
        switch(state_ph){
            case ESPERANDO:
                if(millis() >= time_now + PERIOD){
                    time_now += PERIOD;
                    state_ph = LEYENDO;
                }
                break;
            case LEYENDO:
                if(me_ph() == ME_FINISHED) {
                    state_ph = LEIDO;
                }
                break;
            case LEIDO:
                Serial.printf("#P%d!\n", (int)(pH_Device.get_last_received_reading()*100.0));
                state_ph = ESPERANDO;
                break;
            /*case LEYENDO_RDO:
                if ( _TIMEOUT_TO_RDO_REQUEST_ ) {
                    _updateTimeout_;
                    //Serial.print("sending Modbus request...\n");
                    //modbus.readHoldingRegisters(0x01,0x00,0x01);
                    requestRDO( &rdo );
                }
                state_ph = ESPERANDO;
                break;*/
        }
    }
}

void setup() {
    Serial.begin(115200); 
    Serial.setTimeout(5000);
    Wire.begin(I2C_SDA, I2C_SCL); 
    init_pH_probe();
    ledStrip5.begin(PIN_LED_STRIP_5, 4, 5000, 8);
    ledStrip5.setDuty(0);
    shiftRegister.setOutput(0, valor);
    shiftRegister.setOutput(1, valor);
    shiftRegister.setOutput(2, valor);
    shiftRegister.setOutput(3, valor);
    shiftRegister.setOutput(4, valor);
    shiftRegister.setOutput(5, valor);
    shiftRegister.setOutput(6, valor);
    pinMode(RELE_OUTPUT, OUTPUT);
    Serial.println();   

    Serial1.begin(RDO_BAUD_RATE, SERIAL_8E1, RDO_RX_GPIO, RDO_TX_GPIO, true); 
    modbus.onData(rxRDO);  
    modbus.onError(rxErrorRDO);
    modbus.begin();
    clearRDO(); 
}

void loop() {
    
    static state_general_t state_general = DESCONECTADO;

    switch(state_general){
        case DESCONECTADO:
            if (Serial.available() > 0) {
                if (Serial.readString().equals("INIT")) {
                    Serial.printf("ESP\n");
                    state_general = CONECTADO_FAKE;
                    //state_general = CONECTADO;
                    Serial.printf("#L00!\n");
                    Serial.printf("#C0!\n");
                    Serial.printf("#O0!\n");
                    Serial.printf("#A0!\n");
                    Serial.printf("#W0!\n");
                }
            }
            break;
        case CONECTADO:
            handler_ui();            
            if(msg_valido){
                msg_valido = false;
                switch (clave) {
                    case 'L':
                        Serial.printf("#L%02d!\n", valor);
                        ledStrip5.setDuty(valor);
                        break;
                    case 'P':
                        //Serial.printf("pH: %d\n", valor);
                        break;
                    case 'D':
                        //Serial.printf("Oxigeno disuelto: %d\n", valor);
                        break;
                    case 'T':
                        //Serial.printf("Temperatura: %d\n", valor);
                        break;
                    case 'C':
                        Serial.printf("#C%d!\n", valor);
                        shiftRegister.setOutput(1, valor); 
                        break;
                    case 'O':
                        Serial.printf("#O%d!\n", valor);
                        shiftRegister.setOutput(2, valor);
                        break;
                    case 'A':
                        Serial.printf("#A%d!\n", valor);
                        shiftRegister.setOutput(3, valor);
                        break;
                    case 'W':
                        Serial.printf("#W%d!\n", valor);
                        digitalWrite(RELE_OUTPUT, valor);
                        //shiftRegister.setOutput(4, valor);
                        break;
                    case 'Z':
                        Serial.printf("#Z1!\n");
                        state_general = DESCONECTADO;
                        break;
                }
            }
            break;
        case CONECTADO_FAKE:          
            if (Serial.available() > 0) {
                if (Serial.readString().equals("#Z1!")) {
                    Serial.printf("#Z1!\n");
                    state_general = DESCONECTADO;
                }
            }
            if(millis() >= time_now + PERIOD){
                time_now += PERIOD;
                Serial.printf("#P%d!\n", random(650, 750));
                Serial.printf("#D%d!\n", random(50, 150));
                Serial.printf("#T%d!\n", random(15, 25));
            }

    }     
    /*if ( _TIMEOUT_TO_RDO_REQUEST_ ) {
        _updateTimeout_;
        //Serial.print("sending Modbus request...\n");
        //modbus.readHoldingRegisters(0x01,0x00,0x01);
        requestRDO( &rdo );
    }*/
}
