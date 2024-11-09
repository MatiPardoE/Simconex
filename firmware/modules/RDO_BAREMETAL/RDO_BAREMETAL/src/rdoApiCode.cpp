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

#include <rdoApiGlobalVariables.h>


/***********************************************
 * @brief			: 	Function project prototype
 **********************************************/

#define __DEBUG__

/***********************************************************************************
* @function 	clearRDO
* @brief  	
* @param
* @retval
**********************************************************************************/
void clearRDO (void){

  //rdo.status = SENSOR_ID;
  rdo.status = GET_TEMP;

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
      
      rdo->requests++;
      break;

    case SERIAL_NUMBER:

      rdo->requests++;
      break;

    case GET_DO:

      rdo->requests++;
      break;

    case GET_TEMP:

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




  