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

  rdo.consecutiveErrors = 0;
  rdo.lastError         = SENSOR_ID;

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
#ifdef __DEBUG__
      Serial.printf("REQUEST SENSOR ID \n");
#endif
      modbus.readHoldingRegisters(RDO_SLAVE_ID,_SENSOR_ID_,_SENSOR_ID_SIZE_);
#ifdef __DEBUG__
      Serial.printf("STATUS = %d \n",rdo->status);
#endif
      rdo->requests++;
      break;

    case SERIAL_NUMBER:
#ifdef __DEBUG__
        Serial.printf("REQUEST SERIAL NUMBER \n");
#endif
      modbus.readHoldingRegisters(RDO_SLAVE_ID,_SERIAL_NUMBER_,_SERIAL_NUMBER_SIZE_);
#ifdef __DEBUG__
      Serial.printf("STATUS = %d \n",rdo->status);
#endif
      rdo->requests++;
      break;

    case GET_DO:
#ifdef __DEBUG__
        Serial.printf("REQUEST DO \n");
#endif
      modbus.readHoldingRegisters(RDO_SLAVE_ID,_MEASURED_VALUE_DO_,_MEASURED_VALUE_DO_SIZE_);
#ifdef __DEBUG__
      Serial.printf("STATUS = %d \n",rdo->status);
#endif
      rdo->requests++;
      break;

    case GET_TEMP:
#ifdef __DEBUG__
        Serial.printf("REQUEST TEMP \n");
#endif
      modbus.readHoldingRegisters(RDO_SLAVE_ID,_MEASURED_VALUE_TEMP_,_MEASURED_VALUE_TEMP_SIZE_);
#ifdef __DEBUG__
      Serial.printf("STATUS = %d \n",rdo->status);
#endif
      rdo->requests++;
      break;

    
    default:
      break;
  }
  

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

/*
  Serial.printf("id 0x%02x fc 0x%02x len %u: 0x", serverAddress, fc, length);
  for (size_t i = 0; i < length; ++i) {
    Serial.printf("%02x", data[i]);
  }
  std::reverse(data, data + 4);  // fix endianness
  Serial.printf("\nval: %.2f", *reinterpret_cast<float*>(data));
  Serial.print("\n\n");
*/
  for (size_t i = 0; i < length; ++i) {
    Serial.printf("%02x", data[i]);
  }
  Serial.printf("%d\n", rdo.status);

  if( rdo.consecutiveErrors > 0){ 
    rdo.consecutiveErrors = 0;
#ifdef __DEBUG__
    Serial.printf("VENGO DE UN ERROR \n");
#endif
    return;
  }
  else{
    rdo.consecutiveReplies++;
  }

  if(rdo.consecutiveReplies < 1 ) return;


  switch (rdo.status)
  {
  case SENSOR_ID:
    std::reverse(data, data + 4);  // Invertir los primeros 2 bytes
    rdo.headers.sensorId = *reinterpret_cast<uint16_t*>(data);
#ifdef __DEBUG__
    //Serial.printf("id 0x%02x fc 0x%02x len %u: 0x", serverAddress, fc, length);
    Serial.printf("SENSOR ID %d\n", rdo.headers.sensorId);
#endif
    rdo.status = SERIAL_NUMBER;
    rdo.replies++;
    break;

  case SERIAL_NUMBER:
    std::reverse(data, data + 4);  // Invertir los primeros 4 bytes
    rdo.headers.serialNumber = *reinterpret_cast<uint16_t*>(data);
#ifdef __DEBUG__
    //Serial.printf("id 0x%02x fc 0x%02x len %u: 0x", serverAddress, fc, length);
    Serial.printf("SERIAL NUMBER %d\n", rdo.headers.serialNumber);
#endif
    rdo.status = GET_DO;
    rdo.replies++;
    break;
  //

  case GET_DO:
    std::reverse(data, data + 4);  // Invertir los primeros 4 bytes
    rdo.doConcentration.measuredValue = *reinterpret_cast<float*>(data);
#ifdef __DEBUG__
    //Serial.printf("id 0x%02x fc 0x%02x len %u: 0x", serverAddress, fc, length);
    Serial.printf("OD %.2f\n", rdo.doConcentration.measuredValue);
#endif
    rdo.replies++;
    rdo.status = GET_TEMP;
    return;
    break;

  case GET_TEMP:
    std::reverse(data, data + 4);  // Invertir los primeros 4 bytes
    rdo.temperature.measuredValue = *reinterpret_cast<float*>(data);
#ifdef __DEBUG__
    //Serial.printf("id 0x%02x fc 0x%02x len %u: 0x", serverAddress, fc, length);
    Serial.printf("TEMP %.2f\n", rdo.temperature.measuredValue);
#endif
    rdo.replies++;
    rdo.status = GET_DO;
    return;
    break;

  
  default:
    Serial.printf("id 0x%02x fc 0x%02x len %u: 0x", serverAddress, fc, length);
    break;
  }

}

/***********************************************************************************
* @function 	rxErrorRDO
* @brief  	
* @param
* @retval
**********************************************************************************/
void rxErrorRDO (esp32Modbus::Error error) {
  //Serial.printf("error: 0x%02x\n\n", static_cast<uint8_t>(error));
  
  //dejo registrado el error
  rdo.lastError = rdo.status;
  rdo.errors++;
  rdo.consecutiveErrors++;
#ifdef __DEBUG__
  Serial.printf("error: 0x%02x\n\n", static_cast<uint8_t>(error));
#endif
}



  