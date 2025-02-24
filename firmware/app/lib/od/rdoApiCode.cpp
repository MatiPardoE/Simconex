/**
 ******************************************************************************
 * @file      : rdoApiCOde.c
 * @author    : Tobias Bavasso Piizzi
 * @brief	  :
 * @date	  : 2024/08/13
 * @version	  : 1.00
 ******************************************************************************
 * @attention		: This file it's dependent from
 * 					  uc's family used
 * 					  Este archivo contiene las funciones del proyecto de nivel alto.
 * 					  Para ello hay dos tipos de funciones
 * 					  - INICIALIZAION (dependientes del uc)
 * 					  - PROYECTO (independiente del uc)
 *
 ******************************************************************************
 */

/***********************************************
 * @brief			: Header's inclusion
 **********************************************/
#include <Arduino.h>
#include <rdoApi.h>

#include <ModbusMessage.h>
#include <esp32ModbusRTU.h>
#include <algorithm> // for std::reverse

#include <rdoApiGlobalVariables.h>

// #define __DEBUG__
// #define __AUTOCAL__

/***********************************************
 * @brief			: 	Function project prototype
 **********************************************/

/***********************************************************************************
 * @function 	clearRDO
 * @brief
 * @param
 * @retval
 **********************************************************************************/
void clearRDO(void)
{

    rdo.status = GET_DO;

    rdo.requests = 0;
    rdo.replies = 0;
    rdo.errors = 0;

    rdo.lastError = SENSOR_ID;
    // para probar
    rdo.doConcentration.measuredValue = 0;
    rdo.temperature.measuredValue = 0;
    rdo.doSaturation.measuredValue = 0;

    rdo.onCalibration   = false;
    rdo.points          = CALIB_SAT_1P; 
}

/***********************************************************************************
 * @function 	requestRDO
 * @brief
 * @param
 * @retval
 **********************************************************************************/
void requestRDO(volatile rdo_t *rdo)
{

    uint8_t tmpData[4];

    switch (rdo->status)
    {

    case GET_DO:
        modbus.readHoldingRegisters(RDO_SLAVE_ID, _MEASURED_VALUE_DO_, _MEASURED_VALUE_DO_SIZE_);
        rdo->requests++;
        // ESP_LOGI("OD","Haciendo request de DO");
        break;

    case GET_TEMP:
        modbus.readHoldingRegisters(RDO_SLAVE_ID, _MEASURED_VALUE_TEMP_, _MEASURED_VALUE_TEMP_SIZE_);
        rdo->requests++;
        // ESP_LOGI("OD","Haciendo request de TEMP");
        break;

    case GET_DO_SAT:
        modbus.readHoldingRegisters(RDO_SLAVE_ID, _MEASURED_VALUE_DO_SAT_, _MEASURED_VALUE_DO_SAT_SIZE_);
        rdo->requests++;
        // ESP_LOGI("OD","Haciendo request de DO SAT");
        break;

    case WRITE_CALIBRATION_COMMAND:
#ifdef __DEBUG__
        Serial.print("\n\t<--- ESP -> RDO: ESTADO: WRITE_CALIBRATION_COMMAND --->\n");
#endif
        modbus.writeMultHoldingRegisters(RDO_SLAVE_ID, SENSOR_CMD_REG, CAL_NUM_REGS, calModeOn);
        rdo->requests++;
        break;

    case SET_LIVE_BAR_PRESSURE:
#ifdef __DEBUG__
        Serial.print("\n\t<--- ESP -> RDO: ESTADO: SET_LIVE_BAR_PRESSURE --->\n");
#endif
        rdo->calibration.liveBarometricPressure = *reinterpret_cast<float *>(defaultBarometricPressure);
        modbus.writeMultHoldingRegisters(RDO_SLAVE_ID, _LIVE_BAROMETRIC_PRESS_, _LIVE_BAROMETRIC_PRESS_SIZE_, defaultBarometricPressure);
        rdo->requests++;
        break;

    case SET_100_SAT_BAR_PRESSURE:
#ifdef __DEBUG__
        Serial.print("\n\t<--- ESP -> RDO: ESTADO: SET_100_SAT_BAR_PRESSURE --->\n");
#endif
        rdo->calibration._100saturationBarometric = *reinterpret_cast<float *>(saturationBarometricPressure100);
        modbus.writeMultHoldingRegisters(RDO_SLAVE_ID, _SATURATION_BAR_PRESS_100_, _SATURATION_BAR_PRESS_100_SIZE_, saturationBarometricPressure100);
        rdo->requests++;
        break;

    case SET_100_SAT_SALINITY:
#ifdef __DEBUG__
        Serial.print("\n\t<--- ESP -> RDO: ESTADO: SET_100_SAT_SALINITY --->\n");
#endif
        rdo->calibration._100saturationSalinity = *reinterpret_cast<float *>(saturationSalinity100);
        modbus.writeMultHoldingRegisters(RDO_SLAVE_ID, _SATURATION_SALINITY_100_, _SATURATION_SALINITY_100_SIZE_, saturationSalinity100);
        rdo->requests++;
        break;

    case SET_100_SAT_CONCENTRATION:
#ifdef __DEBUG__
        Serial.print("\n\t<--- ESP -> RDO: ESTADO: SET_100_SAT_CONCENTRATION --->\n");
#endif
        rdo->calibration._100saturationConcentration = rdo->doConcentration.measuredValue;
#ifdef __DEBUG__
        Serial.printf("\n\t<--- CALIBRE CON %f\n", rdo->doConcentration.measuredValue);
#endif
        memcpy(&tmpData, (void *)&(rdo->doConcentration.measuredValue), sizeof(float));
        std::reverse(tmpData, tmpData + 4);
        modbus.writeMultHoldingRegisters(RDO_SLAVE_ID, _SATURATION_CONC_100_, _SATURATION_CONC_100_SIZE_, tmpData);
        rdo->requests++;
        break;

    case SET_100_SAT_TEMPERATURE:
#ifdef __DEBUG__
        Serial.print("\n\t<--- ESP -> RDO: ESTADO: SET_100_SAT_TEMPERATURE --->\n");
#endif
        rdo->calibration._100saturationTemperature = rdo->temperature.measuredValue;
#ifdef __DEBUG__
        Serial.printf("\n\t<--- CALIBRE CON %f\n", rdo->temperature.measuredValue);
#endif
        memcpy(&tmpData, (void *)&(rdo->temperature.measuredValue), sizeof(float));
        std::reverse(tmpData, tmpData + 4);
        modbus.writeMultHoldingRegisters(RDO_SLAVE_ID, _SATURATION_TEMP_100_, _SATURATION_TEMP_100_SIZE_, tmpData);
        rdo->requests++;
        break;

    case SET_0_SAT_CONCENTRATION:
#ifdef __DEBUG__
        Serial.print("\n\t<--- ESP -> RDO: ESTADO: SET_0_SAT_CONCENTRATION --->\n");
#endif
        rdo->calibration._0saturationConcentration = rdo->doConcentration.measuredValue;
#ifdef __DEBUG__
        Serial.printf("\n\t<--- CALIBRE 0% CON %f\n", rdo->doConcentration.measuredValue);
#endif
        memcpy(&tmpData, (void *)&(rdo->doConcentration.measuredValue), sizeof(float));
        std::reverse(tmpData, tmpData + 4);
        modbus.writeMultHoldingRegisters(RDO_SLAVE_ID, _SATURATION_CONC_0_, _SATURATION_CONC_0_SIZE_, tmpData);
        rdo->requests++;
        break;

    case SET_0_SAT_TEMPERATURE:
#ifdef __DEBUG__
        Serial.print("\n\t<--- ESP -> RDO: ESTADO: SET_0_SAT_TEMPERATURE --->\n");
#endif
        rdo->calibration._0saturationTemperature = rdo->temperature.measuredValue;
#ifdef __DEBUG__
        Serial.printf("\n\t<--- CALIBRE 0% CON %f\n", rdo->temperature.measuredValue);
#endif
        memcpy(&tmpData, (void *)&(rdo->temperature.measuredValue), sizeof(float));
        std::reverse(tmpData, tmpData + 4);
        modbus.writeMultHoldingRegisters(RDO_SLAVE_ID, _SATURATION_TEMP_0_, _SATURATION_TEMP_0_SIZE_, tmpData);
        rdo->requests++;
        break;

    case UPDATE_CALIBRATION_COMMAND:
#ifdef __DEBUG__
        Serial.print("\n\t<--- ESP -> RDO: ESTADO: UPDATE_CALIBRATION_COMMAND --->\n");
#endif
        modbus.writeMultHoldingRegisters(RDO_SLAVE_ID, SENSOR_CMD_REG, CAL_NUM_REGS, calUpdate);
        rdo->requests++;
        break;

    case WRITE_CALIBRATION_COMMAND_OFF:
#ifdef __DEBUG__
        Serial.print("\n\t<--- ESP -> RDO: ESTADO: WRITE_CALIBRATION_COMMAND_OFF --->\n");
#endif
        modbus.writeMultHoldingRegisters(RDO_SLAVE_ID, SENSOR_CMD_REG, CAL_NUM_REGS, calModeOff);
        rdo->requests++;
        break;

    default:
        break;
    }

#ifdef __DEBUG__

    Serial.print("\n\t<--- TX --->\n");
    Serial.printf("STATUS = %d \n", rdo->status);
    switch (rdo->status)
    {
    case SENSOR_ID:
        Serial.printf("REQUEST SENSOR ID \n");
        break;

    case SERIAL_NUMBER:
        Serial.printf("REQUEST SERIAL NUMBER \n");
        break;

    case GET_DO:
        Serial.printf("REQUEST DO \n");
        break;

    case GET_TEMP:
        Serial.printf("REQUEST TEMP \n");
        break;

    default:
        break;
    }
    Serial.printf("ENVIOS:\t %d \n", rdo->requests);
    Serial.printf("REPLIES:\t %d \n", rdo->replies);
    Serial.printf("ERRORES:\t %d \n", rdo->errors);
    Serial.print("\t<--- TX --->\n");
#endif
}

/***********************************************
 * @brief			: 	Function initialization prototype
 **********************************************/

/***********************************************************************************
 * @function 	rxRDO
 * @brief  	cosas que hago en la rx de datos
 * @param
 * @retval
 **********************************************************************************/
void rxRDO(uint8_t serverAddress, esp32Modbus::FunctionCode fc, uint8_t *data, size_t length)
{

    rdoStatus_t statusRx = rdo.status;
    // rdoParameterId_t id;
    uint8_t id;
    uint16_t dataAddress;
    float tmpMeasure;
    rdo.errors = 0;

    switch (fc)
    {

    case esp32Modbus::FunctionCode::READ_HOLD_REGISTER:

        id = data[DATA_BYTE_ID];

        switch (id)
        {
        case DO_CONCENTRATION_ID:
            std::reverse(data, data + 4);
            tmpMeasure = *reinterpret_cast<float *>(data);

#ifdef __AUTOCAL__
            if (evaluateEquilibrium(&rdo, id, tmpMeasure) == true)
            {
                // rdo.onCalibration = false;
                rdo.doConcentration.measuredValue = tmpMeasure;
                rdo.replies++;
                rdo.status = SET_LIVE_BAR_PRESSURE;
                return;
            }
#endif

            rdo.doConcentration.measuredValue = tmpMeasure;
            rdo.replies++;
            if (_NOT_CALIBRATING_CMD_)
            {
                rdo.status = GET_TEMP;
            }
            break;

        case TEMPERATURE_ID:
            std::reverse(data, data + 4);
            tmpMeasure = *reinterpret_cast<float *>(data);

#ifdef __AUTOCAL__
            if (evaluateEquilibrium(&rdo, id, tmpMeasure) == true)
            {
                // rdo.onCalibration = false;
                rdo.temperature.measuredValue = tmpMeasure;
                rdo.replies++;
                rdo.status = SET_LIVE_BAR_PRESSURE;
                return;
            }
#endif

            rdo.temperature.measuredValue = tmpMeasure;
            rdo.replies++;
            if (_NOT_CALIBRATING_CMD_)
            {
                rdo.status = GET_DO_SAT;
            }
            break;

        case DO_SATURATION_ID:
            std::reverse(data, data + 4);
            tmpMeasure = *reinterpret_cast<float *>(data);

#ifdef __AUTOCAL__
            if (evaluateEquilibrium(&rdo, id, tmpMeasure) == true)
            {
                // rdo.onCalibration = false;
                rdo.doSaturation.measuredValue = tmpMeasure;
                rdo.replies++;
                rdo.status = SET_LIVE_BAR_PRESSURE;
                return;
            }
#endif

            rdo.doSaturation.measuredValue = tmpMeasure;
            rdo.replies++;
            if (_NOT_CALIBRATING_CMD_)
            {
                rdo.status = GET_DO;
            }
            break;

        default:
            break;
        }

#ifdef __DEBUG__
        Serial.printf("\n\t\t<--- %d --->\n", id);
        Serial.printf("DO CONC [mg/L]: %.2f\t", rdo.doConcentration.measuredValue);
        Serial.printf("TEMP [°c]: %.2f\t", rdo.temperature.measuredValue);
        Serial.printf("DO SAT [%]: %.2f\t", rdo.doSaturation.measuredValue);
#endif

        break;

    case esp32Modbus::FunctionCode::WRITE_MULT_REGISTERS:

        // switch(dataAddress){
        switch (rdo.status)
        {

        // case SENSOR_CMD_REG:
        case WRITE_CALIBRATION_COMMAND:
#ifdef __DEBUG__
            Serial.print("RDO -> ESP :WRITE_CALIBRATION_COMMAND");
#endif
            rdo.replies++;
            rdo.status = GET_DO;
            break;

        case SET_LIVE_BAR_PRESSURE:
#ifdef __DEBUG__
            Serial.print("RDO -> ESP :SET_LIVE_BAR_PRESSURE");
#endif
            rdo.replies++;
            rdo.status = SET_100_SAT_BAR_PRESSURE;
            break;

        case SET_100_SAT_BAR_PRESSURE:
#ifdef __DEBUG__
            Serial.print("RDO -> ESP :SET_100_SAT_BAR_PRESSURE");
#endif
            rdo.replies++;
            rdo.status = SET_100_SAT_SALINITY;
            break;

        case SET_100_SAT_SALINITY:
#ifdef __DEBUG__
            Serial.print("RDO -> ESP :SET_100_SAT_SALINITY");
#endif
            rdo.replies++;
            rdo.status = SET_100_SAT_CONCENTRATION;
            break;

        case SET_100_SAT_CONCENTRATION:
#ifdef __DEBUG__
            Serial.print("RDO -> ESP :SET_100_SAT_CONCENTRATION");
#endif
            rdo.replies++;
            rdo.status = SET_100_SAT_TEMPERATURE;
            break;

        case SET_100_SAT_TEMPERATURE:
#ifdef __DEBUG__
            Serial.print("RDO -> ESP :SET_100_SAT_TEMPERATURE");
#endif
            rdo.replies++;
            if(rdo.points == CALIB_SAT_1P)
                rdo.status = UPDATE_CALIBRATION_COMMAND;
            else if(rdo.points == CALIB_SAT_2P)
                rdo.status = GET_DO;
            break;

//
            case SET_0_SAT_CONCENTRATION:
#ifdef __DEBUG__
            Serial.print("RDO -> ESP :SET_0_SAT_CONCENTRATION");
#endif
            rdo.replies++;
            rdo.status = SET_0_SAT_TEMPERATURE;
            break;

        case SET_0_SAT_TEMPERATURE:
#ifdef __DEBUG__
            Serial.print("RDO -> ESP :SET_0_SAT_TEMPERATURE");
#endif
            rdo.replies++;
            rdo.status = UPDATE_CALIBRATION_COMMAND;
            break;
//

        case UPDATE_CALIBRATION_COMMAND:
#ifdef __DEBUG__
            Serial.print("RDO -> ESP :UPDATE_CALIBRATION_COMMAND");
#endif
            rdo.replies++;
            rdo.status = WRITE_CALIBRATION_COMMAND_OFF;
            break;

        case WRITE_CALIBRATION_COMMAND_OFF:
#ifdef __DEBUG__
            Serial.print("RDO -> ESP :WRITE_CALIBRATION_COMMAND_OFF");
#endif
            rdo.replies++;
            rdo.status = GET_DO;
            rdo.onCalibration = false;
            break;

        default:
            break;
        }

        break;
    }
}

/***********************************************************************************
 * @function 	rxErrorRDO
 * @brief
 * @param
 * @retval
 **********************************************************************************/
void rxErrorRDO(esp32Modbus::Error error)
{
    rdo.lastError = rdo.status;
    rdo.errors++;

    if( _HAS_MORE_ERRORS_THAN_(MAX_ERRORS) ){
        rdo.doSaturation.measuredValue      = 0;
        rdo.doConcentration.measuredValue   = 0;
        rdo.temperature.measuredValue       = 0;
    }

    //_updateTimeout_;
    // lastMillisRDO -= _TIMEOUT_RDO_REQUEST_;

#ifdef __DEBUG__
    Serial.print("\n\t<--- ERROR --->\n");
    Serial.printf("0x%02x\n\n", static_cast<uint8_t>(error));
    Serial.printf("ENVIOS:\t %d \n", rdo.requests);
    Serial.printf("REPLIES:\t %d \n", rdo.replies);
    Serial.printf("ERRORES:\t %d \n", rdo.errors);
    Serial.printf("STATUS:\t %d \n", rdo.lastError);
#endif
}

/***********************************************************************************
 * @function 	triggerPercentSaturationCalibration100
 * @brief
 * @param
 * @retval
 **********************************************************************************/
void triggerPercentSaturationCalibration100(volatile rdo_t *rdo)
{
    rdo->status = WRITE_CALIBRATION_COMMAND;
    rdo->onCalibration = true;
}

/***********************************************************************************
 * @function 	isAnyCalibrationDone
 * @brief
 * @param
 * @retval       true si hay alguna calibracion hecha
 **********************************************************************************/
bool isAnyCalibrationDone(volatile rdo_t *rdo)
{
    return (!rdo->onCalibration);
}

/***********************************************************************************
 * @function 	setRdoCalibrationPoints
 * @brief
 * @param
 * @retval       true si hay alguna calibracion hecha
 **********************************************************************************/
rdoCalibSat_t setRdoCalibrationPoints(volatile rdo_t *rdo , rdoCalibSat_t points)
{
    rdo->points = points;
    return (rdo->points);
}



/***********************************************************************************
 * @function 	finishPercentSaturationCalibration100
 * @brief
 * @param
 * @retval
 **********************************************************************************/
void finishPercentSaturationCalibration100(volatile rdo_t *rdo)
{
    ESP_LOGI("OD", "En Calibracion 100? %d", rdo->onCalibration);
    if (rdo->onCalibration == true)
    {
        rdo->status = SET_LIVE_BAR_PRESSURE; // cambio de estado para finalizar
    }
}

/***********************************************************************************
 * @function 	finishPercentSaturationCalibration100
 * @brief
 * @param
 * @retval
 **********************************************************************************/
void finishPercentSaturationCalibration0(volatile rdo_t *rdo)
{
    ESP_LOGI("OD", "En Calibracion 0? %d", rdo->onCalibration);
    if (rdo->onCalibration == true)
    {
        rdo->status = SET_0_SAT_CONCENTRATION; // cambio de estado para finalizar
    }
}


