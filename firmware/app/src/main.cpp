#include <Arduino.h>
#include <ArduinoLog.h>
#include <cycle_manager.h>
#include <commUI.h>
#include <file_transfer.h>
#include <controlAPI.h>
#include "esp_log.h"
#include <manualMode.h>
#include "ezo_ph.h"
#include "neo_pixel.h"

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
NeoPixel neoPixel(4, 4); // Pin 4, 4 pixel

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
    Serial1.begin(RDO_BAUD_RATE, SERIAL_8E1, RDO_RX_GPIO, RDO_TX_GPIO, false);
    ESP_LOGI(TAG, "Starting...\n");
    sensorControl.init();
    modbus.onData(rxRDO);
    modbus.onError(rxErrorRDO);
    modbus.begin();
    clearRDO();
    neoPixel.begin();
    neoPixel.initFun();

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
    static cycle_manager::MeasuresAndOutputs new_measure_calib;
    static ManualMode::ManualBundle manualBundle;
    cycle_manager::CycleBundle firstCycleBundle;
    String outputSerial;
    float ph_value_in_calib = -1;
    // .run
    commandUI = commUI.run(outputSerial);
    cycleBundle = cm.run();
    manualBundle = manualMode.run();
    sensorControl.run(cm.cycleData.status);

    unsigned long millis_init;

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
            neoPixel.setState(NeoPixel::CYCLE_PAUSED);
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
            manualMode.deactivate();
            cm.resumeCycle();
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
        }
        sensorControl.turnOffOutputs();
        triggerPercentSaturationCalibration100(&rdo);
        break;

    //
    case CommUI::FINISH_CALIB_OD_SAT_1P:
        ESP_LOGI(TAG, "Finish calibration OD saturation 100%\n");
        finishPercentSaturationCalibration100(&rdo);
        millis_init = millis();
        while (millis_init + 40000 > millis())
        {
            // TODO Hacer maquina de estado o no bloqueante y que haga el request en el .run
            if (isAnyCalibrationDone(&rdo))
            {
                Serial.println("#OKCALIBODSAT!");
                ESP_LOGI(TAG, "Checked calibration OD saturation 100%\n");
                break;
            }
            if (_TIMEOUT_TO_RDO_REQUEST_)
            {
                requestRDO(&rdo);
                _updateTimeout_;
            }
        }
        ESP_LOGE(TAG, "Error checking calibration OD saturation 100%\n");
        break;


    case CommUI::GOTO_CALIB_OD_SAT_2P:
        ESP_LOGI(TAG, "Continue calibration OD saturation 0%\n");
        setRdoCalibrationPoints(&rdo,CALIB_SAT_2P);
        //dispara la escritura de los registros de 100%
        //pero no hara el update command
        //porque sabe que le falta un punto de calib
        //y volvera a medir
        finishPercentSaturationCalibration100(&rdo); 
        break;

    //
    case CommUI::FINISH_CALIB_OD_SAT_2P:
        ESP_LOGI(TAG, "Finish calibration OD saturation 0%\n");
        finishPercentSaturationCalibration0(&rdo);
        millis_init = millis();
        while (millis_init + 40000 > millis())
        {
            if (isAnyCalibrationDone(&rdo))
            {
                ESP_LOGI(TAG, "Checked calibration OD saturation 0%\n");
                break;
            }
            if (_TIMEOUT_TO_RDO_REQUEST_)
            {
                requestRDO(&rdo);
                _updateTimeout_;
            }
        }

        if( millis_init + 40000  < millis()){
            Serial.println("#FAILCALIBODSAT!");
            ESP_LOGE(TAG, "Error checking calibration OD saturation\n");
        }


        break;
    case CommUI::START_CALIB_PH:
        ESP_LOGI(TAG, "Start calibration pH\n");
        if (cm.cycleData.status == cycle_manager::CYCLE_RUNNING)
        {
            ESP_LOGI(TAG, "Ciclo corriendo, pauso ciclo para iniciar la calibracion de pH");
            cm.pauseCycle();
            sensorControl.turnOffOutputs();
        }
        pH_Device.start_calibration();
        pH_Device.state_calib = pH::INIT_CALIB;
        break;
    case CommUI::CLEAR_CALIB_PH:
        if (!pH_Device.is_on_calibration())
        {
            ESP_LOGI(TAG, "Not in calibration mode");
            break;
        }
        ESP_LOGI(TAG, "Clear calibration pH\n");
        pH_Device.state_calib = pH::CLEAR_CALIB;
        break;
    case CommUI::SET_MID_POINT_PH:
        if (!pH_Device.is_on_calibration())
        {
            ESP_LOGI(TAG, "Not in calibration mode");
            break;
        }
        ESP_LOGI(TAG, "Set mid point pH\n");
        pH_Device.next_calib_step = true;
        break;
    case CommUI::SET_LOW_POINT_PH:
        if (!pH_Device.is_on_calibration())
        {
            ESP_LOGI(TAG, "Not in calibration mode");
            break;
        }
        ESP_LOGI(TAG, "Set low point pH\n");
        pH_Device.next_calib_step = true;
        break;
    case CommUI::SET_HIGH_POINT_PH:
        if (!pH_Device.is_on_calibration())
        {
            ESP_LOGI(TAG, "Not in calibration mode");
            break;
        }
        ESP_LOGI(TAG, "Set high point pH\n");
        pH_Device.next_calib_step = true;
        break;
    case CommUI::FINISH_CALIB_PH:
        ESP_LOGI(TAG, "Finish calibration pH\n");
        pH_Device.finish_calib = true;
        // if (cm.cycleData.status == cycle_manager::CYCLE_PAUSED)
        // {
        //     ESP_LOGI(TAG, "reanudo ciclo luego de calibracion de PH");
        //     cm.resumeCycle();
        // }
        break;
    default:
        ESP_LOGE(TAG, "Unknown command: %s \n", outputSerial.c_str());
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
        sensorControl.turnOffOutputs();
        ESP_LOGI(TAG, "Cycle FINISIHED");
        break;
    case cycle_manager::NEW_INTERVAL:
        neoPixel.setState(NeoPixel::NEW_INTERVAL);
        sensorControl.set_control_var(cycleBundle.intervalData);
        if (first_interval)
        {
            first_interval = false;
            break;
        }
        new_measure_outputs = sensorControl.takeMeasuresAndOutputs();                          // No deberia tardar mucho
        cm.writeMeasuresToSD(new_measure_outputs, (cycleBundle.intervalData.interval_id - 1)); // Envio el ID-1 porque el ID es el siguiente intervalo
        cm.sendDataToUI(new_measure_outputs, (cycleBundle.intervalData.interval_id - 1));
        neoPixel.setState(NeoPixel::CYCLE_RUNNING);
        break;
    default:
        break;
    }

    switch (manualBundle)
    {
    case ManualMode::NO_COMMAND:
        break;
    case ManualMode::SEND_MEASURES:
        // ESP_LOGI(TAG, "Envio mediciones desde modo manual");
        new_measure_outputs = sensorControl.takeMeasuresAndOutputs();
        cm.sendDataToUI(new_measure_outputs, 0);
        break;
    default:
        break;
    }

    switch (pH_Device.state_calib)
    {
    case pH::NO_CALIB:
        break;
    case pH::INIT_CALIB:
        if (comp_temp_calib_ph_sequencer())
        {
            if (pH_Device.get_error() == pH::SUCCESS)
            {
                pH_Device.state_calib = pH::WAIT_CLEAR;
                ESP_LOGI(TAG, "Temperature compensation for pH probe done");
            }
            else
            {
                pH_Device.state_calib = pH::NO_CALIB;
                ESP_LOGE(TAG, "Error en compensacion de temperatura de pH");
            }
        }
        break;
    case pH::WAIT_CLEAR:
        // Tengo que bloquear la medicion del ph en este estado
        break;
    case pH::CLEAR_CALIB:
        // Tengo que bloquear la medicion del ph en este estado
        if (clear_calib_ph_secuencer())
        {
            if (pH_Device.get_error() == pH::SUCCESS)
            {
                ESP_LOGI(TAG, "Calibracion de pH reseteada");
                pH_Device.state_calib = pH::ESTABLISH_MID_POINT;
            }
            else
            {
                pH_Device.state_calib = pH::NO_CALIB;
                ESP_LOGE(TAG, "Error en clear calib de pH");
            }
        }
        break;
    case pH::ESTABLISH_MID_POINT:
        // Estoy midiendo e imprimiendo en pantalla el valor de pH hasta que se establezca
        // Me voy a MID_POINT cunado llegue el comando por UI
        ph_value_in_calib = get_ph();
        if (ph_value_in_calib != -1)
        {
            // ESP_LOGI(TAG, "Valor de pH: %.2f", ph_value_in_calib);
            new_measure_calib.ph = ph_value_in_calib;
            cm.sendDataToUI(new_measure_calib, 0);
            if (pH_Device.next_calib_step)
            {
                ESP_LOGI(TAG, "Estableciendo mid point de pH");
                pH_Device.next_calib_step = false;
                pH_Device.state_calib = pH::MID_POINT;
            }
        }
        break;
    case pH::MID_POINT:
        if (calib_mid_ph_sequencer())
        {
            if (pH_Device.get_error() == pH::SUCCESS)
            {
                ESP_LOGI(TAG, "Calibracion de pH en 7.00 finalizada");
                pH_Device.state_calib = pH::ESTABLISH_LOW_POINT;
            }
            else
            {
                pH_Device.state_calib = pH::NO_CALIB;
                ESP_LOGE(TAG, "Error en estableciendo mid point de pH");
            }
        }
        break;
    case pH::ESTABLISH_LOW_POINT:
        ph_value_in_calib = get_ph();
        if (ph_value_in_calib != -1)
        {
            new_measure_calib.ph = ph_value_in_calib;
            cm.sendDataToUI(new_measure_calib, 0);
            if (pH_Device.next_calib_step)
            {
                ESP_LOGI(TAG, "Estableciendo low point de pH");
                pH_Device.next_calib_step = false;
                pH_Device.state_calib = pH::LOW_POINT;
            }
            if (pH_Device.finish_calib)
            {
                pH_Device.finish_calib = false;
                ESP_LOGI(TAG, "Calibracion de pH finalizada en 1 punto");
                pH_Device.state_calib = pH::CHECK_CALIB;
            }
        }
        break;

    case pH::LOW_POINT:
        if (calib_low_ph_sequencer())
        {
            if (pH_Device.get_error() == pH::SUCCESS)
            {
                ESP_LOGI(TAG, "Calibracion de pH en 4.00 finalizada");
                pH_Device.state_calib = pH::ESTABLISH_HIGH_POINT;
            }
            else
            {
                pH_Device.state_calib = pH::NO_CALIB;
                ESP_LOGE(TAG, "Error en estableciendo low point de pH");
            }
        }
        break;
    case pH::ESTABLISH_HIGH_POINT:
        ph_value_in_calib = get_ph();
        if (ph_value_in_calib != -1)
        {
            new_measure_calib.ph = ph_value_in_calib;
            cm.sendDataToUI(new_measure_calib, 0);
            if (pH_Device.next_calib_step)
            {
                ESP_LOGI(TAG, "Estableciendo low point de pH");
                pH_Device.next_calib_step = false;
                pH_Device.state_calib = pH::HIGH_POINT;
            }
            if (pH_Device.finish_calib)
            {
                pH_Device.finish_calib = false;
                ESP_LOGI(TAG, "Calibracion de pH finalizada en 2 puntos");
                pH_Device.state_calib = pH::CHECK_CALIB;
            }
        }
        break;

    case pH::HIGH_POINT:
        if (calib_high_ph_sequencer())
        {
            if (pH_Device.get_error() == pH::SUCCESS)
            {
                ESP_LOGI(TAG, "Calibracion de pH en 10.00 finalizada");
                pH_Device.state_calib = pH::CHECK_CALIB;
            }
            else
            {
                pH_Device.state_calib = pH::NO_CALIB;
                ESP_LOGE(TAG, "Error en estableciendo high point de pH");
            }
        }
        break;
    case pH::CHECK_CALIB:
        if (calib_check_ph_sequencer())
        {
            if (pH_Device.get_error() == pH::SUCCESS)
            {
                Serial.println("#OKCALIBPH!");
                ESP_LOGI(TAG, "Calibracion de pH finalizada");
            }
            else
            {
                Serial.println("#FAILCALIBPH!");
                ESP_LOGE(TAG, "Error chequeando la calibracion de pH");
            }
            pH_Device.finish_calibration();
        }
        break;
    default: // error
        ESP_LOGE(TAG, "La calibracion no se chequeo bien!");
        break;
    }
}