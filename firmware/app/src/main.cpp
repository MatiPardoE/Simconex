#include <Arduino.h>
#include <ArduinoLog.h>
#include <cycle_manager.h>
#include <commUI.h>
#include <file_transfer.h>
#include <controlAPI.h>
#include "esp_log.h"
#include <manualMode.h>
#include "ezo_ph.h"

// cosas RDO
#include <rdoApiCode.h>
// MODBUS
#include <ModbusMessage.h>
#include <esp32ModbusRTU.h>
#include <algorithm> // for std::reverse

#define SPI_MISO 34
#define SPI_MOSI 33
#define SPI_SS 26
#define SPI_CLK 25
#define SD_CS_PIN SPI_SS

static const char *TAG = "simconex";
cycle_manager cm(SPI_CLK, SPI_MISO, SPI_MOSI, SPI_SS); // Pide los pines SPI de la SD
CommUI commUI;
FileTransfer fileTransfer(Serial, SD_CS_PIN);
ControlAPI sensorControl;
ManualMode manualMode;

pH pH_Device = pH(20, "EZO pH probe");
bool first_interval = false;

/* ----------------------------------------------------------------------------
  -- RDO
  ---------------------------------------------------------------------------- */
esp32ModbusRTU modbus(&Serial1, RDO_DE_RE_GPIO); // use Serial1 and GPIO11/27 as RTS
uint32_t lastMillisRDO = 0;                      // para que arranque de una
volatile rdo_t rdo;
bool initRDO = true;
uint8_t calModeOn[] = {0xE0, 0x00};
uint8_t saturationBarometricPressure100[] = {0x44, 0x7D, 0x50, 0x00};
uint8_t saturationSalinity100[] = {0x00, 0x00, 0x00, 0x00};
uint8_t defaultBarometricPressure[] = {0x44, 0x7D, 0x50, 0x00};

uint8_t calUpdate[] = {0xE0, 0x01};
uint8_t calModeOff[] = {0xE0, 0x02};

void setup()
{
    commUI.begin(230400); // Solo define el puerto y velocidad de comunicación
    Log.begin(LOG_LEVEL_VERBOSE, &Serial, true);
    ESP_LOGI(TAG, "Starting...\n");
    sensorControl.init();
    delay(350);
    if (!cm.begin(SD_CS_PIN)) // Inicializa la SD y lee el header
    {
        ESP_LOGE(TAG, "Cycle manager failed to initialize\n");
    }
}

void loop()
{
    static CommUI::CommandFromUI commandUI;
    static cycle_manager::CycleBundle cycleBundle;
    static cycle_manager::MeasuresAndOutputs new_measure_outputs;
    static ManualMode::ManualBundle manualBundle;
    cycle_manager::CycleBundle firstCycleBundle;
    String outputSerial;
    // .run
    commandUI = commUI.run(outputSerial);
    cycleBundle = cm.run();
    manualBundle = manualMode.run();
    sensorControl.run(cm.cycleData.status);
    switch (commandUI)
    {
    case CommUI::TRANSFER_FILE_START:
        // ------- BLOQUEANTE ---------
        // ESP_LOGI(TAG,"Transfer file start\n");
        manualMode.deactivate();
        Serial.println("#OK!"); // TODO Create function to send commands to UI
        fileTransfer.transferFiles(cm.headerPath.c_str(), cm.dataPath.c_str(), cm.dataOutPath.c_str(), 10000);
        delay(40);               // Por seguridad para los HS
        if (cm.begin(SD_CS_PIN)) // Inicializa la SD y lee el header
        {
            first_interval = true;
        }
        else
        {
            ESP_LOGE(TAG, "Cycle manager failed to initialize\n");
        }
        break;
    case CommUI::UNKNOWN_COMMAND:
        // Do nothing
        break;
    case CommUI::INIT_COMM_UI:
        manualMode.deactivate();
        Serial.println("#ESP!"); // Comando para inicializar la comunicación e identificar el puerto
        break;
    case CommUI::SYNC_CYCLE_START:
        manualMode.deactivate();
        // STA0 = NOT_CYCLE - STA1 = CYCLE_RUNNING - STA2 = CYCLE_FINISHED
        if (cm.cycleData.status == cycle_manager::NO_CYCLE_IN_SD)
        {
            Serial.println("#STA0!");
        }
        else if (cm.cycleData.status == cycle_manager::CYCLE_RUNNING)
        {
            Serial.println("#STA1!");
            fileTransfer.transferCycle(cm.headerPath.c_str(), cm.dataOutPath.c_str(), 10000);
        }
        else if (cm.cycleData.status == cycle_manager::CYCLE_COMPLETED)
        {
            Serial.println("#STA2!");
            fileTransfer.transferCycle(cm.headerPath.c_str(), cm.dataOutPath.c_str(), 10000);
        }
        break;
    case CommUI::PAUSE_CYCLE:
        if (cm.cycleData.status == cycle_manager::NO_CYCLE_IN_SD)
        {
            Serial.println("#FAIL!");
            ESP_LOGE(TAG, "No cycle in SD, imposible pausar");
        }
        else if (cm.cycleData.status == cycle_manager::CYCLE_RUNNING)
        {
            Serial.println("#OK!");
            cm.pauseCycle();
            sensorControl.turnOffOutputs();
        }
        else if (cm.cycleData.status == cycle_manager::CYCLE_COMPLETED)
        {
            Serial.println("#FAIL!");
            ESP_LOGE(TAG, "Ciclo completado, imposible pausar");
        }
        else if (cm.cycleData.status == cycle_manager::CYCLE_PAUSED)
        {
            Serial.println("#FAIL!");
            ESP_LOGE(TAG, "Ciclo pausado, imposible pausar");
        }
        break;
    case CommUI::RESUME_CYCLE:
        if (cm.cycleData.status == cycle_manager::NO_CYCLE_IN_SD)
        {
            Serial.println("#FAIL!");
            ESP_LOGE(TAG, "No cycle in SD, imposible play");
        }
        else if (cm.cycleData.status == cycle_manager::CYCLE_RUNNING)
        {
            Serial.println("#FAIL!");
            ESP_LOGE(TAG, "Ciclo corriendo, imposible play");
        }
        else if (cm.cycleData.status == cycle_manager::CYCLE_COMPLETED)
        {
            Serial.println("#FAIL!");
            ESP_LOGE(TAG, "Ciclo completado, imposible play");
        }
        else if (cm.cycleData.status == cycle_manager::CYCLE_PAUSED)
        {
            Serial.println("#OK!");
            cm.resumeCycle();
            manualMode.deactivate();
        }
        break;
    case CommUI::MANUAL_MODE:
        if (cm.cycleData.status == cycle_manager::CYCLE_RUNNING)
        {
            Serial.println("#FAIL!");
            ESP_LOGE(TAG, "Ciclo corriendo, imposible modo manual");
            break;
        }
        sensorControl.modeManualsetOutputs(outputSerial);
        if (manualMode.isActive())
        {
            // Ya esta activado
            break;
        }
        manualMode.activate();
        break;
    case CommUI::START_CALIB_OD_SAT:
        ESP_LOGI(TAG, "Start calibration OD saturation\n");
        if (cm.cycleData.status == cycle_manager::CYCLE_RUNNING)
        {
            ESP_LOGI(TAG, "Ciclo corriendo, pauso ciclo para iniciar la calibracion de OD");
            cm.pauseCycle();
            sensorControl.turnOffOutputs();
        }
        triggerPercentSaturationCalibration(&rdo);
        break;
    case CommUI::FINISH_CALIB_OD_SAT:
        ESP_LOGI(TAG, "Finish calibration OD saturation\n");
        if (cm.cycleData.status == cycle_manager::CYCLE_PAUSED)
        {
            ESP_LOGI(TAG, "reanudo ciclo luego de calibracion de OD");
            cm.resumeCycle();
        }
        // finishPercentSaturationcalibration(&rdo);
        break;
    case CommUI::START_CALIB_PH:
        ESP_LOGI(TAG, "Start calibration pH\n");
        pH_Device.start_calibration();
        if (cm.cycleData.status == cycle_manager::CYCLE_RUNNING)
        {
            ESP_LOGI(TAG, "Ciclo corriendo, pauso ciclo para iniciar la calibracion de pH");
            cm.pauseCycle();
            sensorControl.turnOffOutputs();
        }
        break;
    case CommUI::CLEAR_CALIB_PH:
        if (!pH_Device.is_on_calibration())
        {
            ESP_LOGI(TAG, "Not in calibration mode");
            break;
        }
        ESP_LOGI(TAG, "Clear calibration pH\n");
        pH_Device.state_calib = CLEAR_CALIB;
        break;
    case CommUI::SET_MID_POINT_PH:
        if (!pH_Device.is_on_calibration())
        {
            ESP_LOGI(TAG, "Not in calibration mode");
            break;
        }
        ESP_LOGI(TAG, "Set mid point pH\n");
        pH_Device.state_calib = MID_POINT;
        break;
    case CommUI::FINISH_CALIB_PH:
        ESP_LOGI(TAG, "Finish calibration pH\n");
        if (cm.cycleData.status == cycle_manager::CYCLE_PAUSED)
        {
            ESP_LOGI(TAG, "reanudo ciclo luego de calibracion de PH");
            cm.resumeCycle();
        }
        // finishPHcalibration(&rdo);
        break;
    default:
        ESP_LOGE(TAG, "Unknown command\n");
        break;
    }

    switch (cycleBundle.command)
    {
    case cycle_manager::NO_COMMAND:
        // Do nothing
        break;
    case cycle_manager::FINISH_CYCLE:
        // Termino el ciclo por lo que debo enviar el ultimo intervalo y finalizar el ciclo
        new_measure_outputs = sensorControl.takeMeasuresAndOutputs();                          // No deberia tardar mucho
        cm.writeMeasuresToSD(new_measure_outputs, (cycleBundle.intervalData.interval_id - 1)); // Envio el ID-1 porque el ID es el siguiente intervalo
        cm.sendDataToUI(new_measure_outputs, (cycleBundle.intervalData.interval_id - 1));
        ESP_LOGI(TAG, "Cycle FINISIHED");
        break;
    case cycle_manager::NEW_INTERVAL:
        sensorControl.set_control_var(cycleBundle.intervalData);
        if (first_interval)
        {
            first_interval = false;
            break;
        }
        new_measure_outputs = sensorControl.takeMeasuresAndOutputs();                          // No deberia tardar mucho
        cm.writeMeasuresToSD(new_measure_outputs, (cycleBundle.intervalData.interval_id - 1)); // Envio el ID-1 porque el ID es el siguiente intervalo
        cm.sendDataToUI(new_measure_outputs, (cycleBundle.intervalData.interval_id - 1));
        break;
    default:
        break;
    }

    switch (manualBundle)
    {
    case ManualMode::NO_COMMAND:
        break;
    case ManualMode::SEND_MEASURES:
        ESP_LOGI(TAG, "Envio mediciones desde modo manual");
        new_measure_outputs = sensorControl.takeMeasuresAndOutputs();
        cm.sendDataToUI(new_measure_outputs, 0);
        break;
    default:
        break;
    }

    switch (pH_Device.state_calib)
    {
    case NO_CALIB:
        break;
    case INIT_CALIB:
        break;
    case CLEAR_CALIB:
        if (calib_clear_seq.run() == calib_check_seq.FINISHED)
        {
            pH_Device.state_calib = ESTABLISH_MID_POINT;
            ESP_LOGI(TAG, "Calibracion de pH reseteada\n");
        }
        break;
    case ESTABLISH_MID_POINT:
        if (millis() - pH_Device.last_read_time >= 2000)
        {
            if (read_seq.run() == read_seq.FINISHED)
            {
                pH_Device.last_read_time = millis();
                if (pH_Device.get_error() == pH::SUCCESS)
                {
                    Serial.printf("Valor de pH: %.2f\n", pH_Device.get_last_received_reading());
                }
                else
                {
                    ESP_LOGE(TAG, "ph read error in Calib state");
                }
            }
        }
        break;
    case MID_POINT:
        if (calib_mid_seq.run() == calib_mid_seq.FINISHED)
        {
            state_calib = WAIT_ENTER_LOW;
        }
        break;
    case WAIT_ENTER_LOW:
        if (millis() - last_read_time >= 2000)
        {
            if (read_seq.run() == read_seq.FINISHED)
            {
                last_read_time = millis();
                if (pH_Device.get_error() == pH::SUCCESS)
                {
                    Serial.printf("Valor de pH: %.2f\n", pH_Device.get_last_received_reading());
                }
                else
                {
                    Log.error("ph read error in Calib state\n");
                }
            }
        }
        if (Serial.available() > 0)
        {
            char c = Serial.read();
            if (c == '\n')
            {
                state_calib = LOW_POINT;
            }
        }
        break;

    case LOW_POINT:
        if (calib_low_seq.run() == calib_low_seq.FINISHED)
        {
            state_calib = WAIT_ENTER_HIGH;
            ESP_LOGI(TAG, "Calibracion en ph 4.00 finalizada\n");
            ESP_LOGI(TAG, "Coloque la sonda en la solucion de calibracion de 10.00\n");
            ESP_LOGI(TAG, "Espere que se estabilice la medicion y presione enter para continuar...\n");
        }
        break;

    case WAIT_ENTER_HIGH:
        if (millis() - last_read_time >= 2000)
        {
            if (read_seq.run() == read_seq.FINISHED)
            {
                last_read_time = millis();
                if (pH_Device.get_error() == pH::SUCCESS)
                {
                    Serial.printf("Valor de pH: %.2f\n", pH_Device.get_last_received_reading());
                }
                else
                {
                    Log.error("ph read error in Calib state\n");
                }
            }
        }
        break;

    case HIGH_POINT:
        if (calib_high_seq.run() == calib_high_seq.FINISHED)
        {
            ESP_LOGI(TAG, "Calibracion en ph 10.00 finalizada\n");
            ESP_LOGI(TAG, "Vamos a chequear la calibracion\n");
        }
        break;

    case CHECK_CALIB:
        if (calib_check_seq.run() == calib_check_seq.FINISHED)
        {
            ESP_LOGI(TAG, "Calibracion chequeada!\n");
        }
        else
        {
            ESP_LOGI(TAG, "La calibracion no se chequeo bien!\n");
        }
        break;

    default: // error
        ESP_LOGE(TAG, "La calibracion no se chequeo bien!\n");
        break;
    }
}