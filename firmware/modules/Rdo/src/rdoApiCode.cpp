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
#include <algorithm>  // for std::reverse

#include <rdoApiGlobalVariables.h>

#define __DEBUG__
//#define __PREV_REPLY__
#define __PIN_DEBUG__

/***********************************************
 * @brief			: 	Function project prototype
 **********************************************/

/***********************************************************************************
* @function 	clearRDO
* @brief  	
* @param
* @retval
**********************************************************************************/
void clearRDO (void){

  //rdo.status = SENSOR_ID;
  rdo.status = GET_DO;

  rdo.requests  = 0;
  rdo.replies   = 0;
  rdo.errors    = 0;

  rdo.lastError         = SENSOR_ID;
  // para probar
  rdo.doConcentration.measuredValue = 0;
  rdo.temperature.measuredValue     = 0;
}

/***********************************************************************************
* @function 	requestRDO
* @brief  	
* @param
* @retval
**********************************************************************************/
void requestRDO ( volatile rdo_t * rdo ){

  switch (rdo->status)
  {

    case SENSOR_ID:
      modbus.readHoldingRegisters(RDO_SLAVE_ID,_SENSOR_ID_,_SENSOR_ID_SIZE_);
      rdo->requests++;
      break;

    case SERIAL_NUMBER:
      modbus.readHoldingRegisters(RDO_SLAVE_ID,_SERIAL_NUMBER_,_SERIAL_NUMBER_SIZE_);
      rdo->requests++;
      break;

    case GET_DO:
      modbus.readHoldingRegisters(RDO_SLAVE_ID,_MEASURED_VALUE_DO_,_MEASURED_VALUE_DO_SIZE_);
      rdo->requests++;
      break;

    case GET_TEMP:
      modbus.readHoldingRegisters(RDO_SLAVE_ID,_MEASURED_VALUE_TEMP_,_MEASURED_VALUE_TEMP_SIZE_);
      rdo->requests++;
      break;

    default:
      break;
  }

#ifdef __DEBUG__

  Serial.print("\n\t<--- TX --->\n");
  Serial.printf("STATUS = %d \n",rdo->status);
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
  Serial.printf("ENVIOS:\t %d \n",rdo->requests);
  Serial.printf("REPLIES:\t %d \n",rdo->replies);
  Serial.printf("ERRORES:\t %d \n",rdo->errors);
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
void rxRDO (uint8_t serverAddress, esp32Modbus::FunctionCode fc, uint8_t* data, size_t length ) {

  rdoStatus_t statusRx = rdo.status;

#ifdef __DEBUG__
  Serial.print("\n\t<--- RX --->\n");
  Serial.printf("id 0x%02x fc 0x%02x len %u: 0x", serverAddress, fc, length);
  //for (size_t i = 0; i < length; ++i) {
  for (size_t i = 0; i < 4; ++i) {
    Serial.printf("%02x", data[i]);
  }
  Serial.print("\n\t\t<---  --->\n");
#endif


  switch (rdo.status)
  {
  case SENSOR_ID:
    //entiendo que es un solo byte
    rdo.headers.sensorId = *reinterpret_cast<uint8_t*>(data);
    rdo.status = SERIAL_NUMBER;
    rdo.replies++;
    break;

  case SERIAL_NUMBER:
    std::reverse(data, data + 2);
    rdo.headers.serialNumber = *reinterpret_cast<uint16_t*>(data);
    rdo.status = GET_DO;
    rdo.replies++;
    break;
  //

  case GET_DO:
    std::reverse(data, data + 4);
#ifndef __PREV_REPLY__ 
    rdo.doConcentration.measuredValue = *reinterpret_cast<float*>(data);
#endif
#ifdef __PREV_REPLY__
    rdo.temperature.measuredValue = *reinterpret_cast<float*>(data);
#endif
    rdo.replies++;
    //rdo.status = GET_TEMP;
    rdo.status = GET_DO;
    break;

  case GET_TEMP:
    std::reverse(data, data + 4);
#ifndef __PREV_REPLY__ 
    rdo.temperature.measuredValue = *reinterpret_cast<float*>(data);
#endif
#ifdef __PREV_REPLY__
    rdo.doConcentration.measuredValue = *reinterpret_cast<float*>(data);
#endif
    rdo.replies++;
    rdo.status = GET_DO;
    break;

  default:
    break;
  }

#ifdef __DEBUG__
  switch (statusRx)
  {
  case SENSOR_ID:
    Serial.printf("SENSOR ID %d\n", rdo.headers.sensorId);
    break;

  case SERIAL_NUMBER:
    Serial.printf("SERIAL NUMBER %d\n", rdo.headers.serialNumber);
    break;
  //

  case GET_DO:
#ifndef __PREV_REPLY__ 
    Serial.printf("OD %.2f\n", rdo.doConcentration.measuredValue);
#endif
#ifdef __PREV_REPLY__
    Serial.printf("TEMP %.2f\n", rdo.temperature.measuredValue);
#endif
    break;

  case GET_TEMP:
  #ifndef __PREV_REPLY__ 
    Serial.printf("TEMP %.2f\n", rdo.temperature.measuredValue);
  #endif
  #ifdef __PREV_REPLY__
    Serial.printf("OD %.2f\n", rdo.doConcentration.measuredValue);
  #endif
    break;

  default:
    break;

  }
  Serial.printf("ENVIOS:\t %d \n",rdo.requests);
  Serial.printf("REPLIES:\t %d \n",rdo.replies);
  Serial.printf("ERRORES:\t %d \n",rdo.errors);
  Serial.print("\t<--- RX --->\n");
#endif

}

/***********************************************************************************
* @function 	rxErrorRDO
* @brief  	
* @param
* @retval
**********************************************************************************/
void rxErrorRDO (esp32Modbus::Error error) {
  rdo.lastError = rdo.status;
  rdo.errors++;
  //_updateTimeout_;
  //lastMillisRDO -= _TIMEOUT_RDO_REQUEST_;


  //uint8_t ledActual = digitalRead(RDO_LED1);
  //digitalWrite(RDO_LED1,!ledActual);
  
#ifdef __DEBUG__
  Serial.print("\n\t<--- ERROR --->\n");
  Serial.printf("0x%02x\n\n", static_cast<uint8_t>(error));
  Serial.printf("ENVIOS:\t %d \n",rdo.requests);
  Serial.printf("REPLIES:\t %d \n",rdo.replies);
  Serial.printf("ERRORES:\t %d \n",rdo.errors);
  Serial.printf("STATUS:\t %d \n",rdo.lastError);
#endif


}



  