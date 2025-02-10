/**
 ******************************************************************************
 * @file           	: rdoApi.h
 * @author         	: Tobias Bavasso Piizzi
 * @brief			: 
 * @date			: 2024/08/13
 * @version			: 1.00
 ******************************************************************************
 * @attention		: This file it's NOT dependent from
 * 					  uc's family used
 *
 ******************************************************************************
 */

#ifndef _RDO_API_H
#define _RDO_API_H

/***********************************************
 * @brief			: Header's inclusion
 **********************************************/
#include <rdoApiDefs.h>
#include "esp_log.h"
/***********************************************
 * @brief			: Defines
 **********************************************/


/***********************************************
 * @brief			: Enum
 **********************************************/





/***********************************************
 * @brief			: Macros
 **********************************************/


/***********************************************
 * @brief			: Structs declaration
 **********************************************/

/***********************************************
 * @brief			: Typedef
 **********************************************/

/*! @typedef
 * @brief 	:
 * @details	:
 *
 * @param
 * @param
 */
typedef struct{
    uint16_t    sensorId;
    uint32_t    serialNumber;
    uint32_t    lastFactoryCal;
    uint32_t    nextFactoryCal;
    uint32_t    lastUserCal;
    uint32_t    nextUserCal;
} rdoHeadersRegisters_t;

/*! @typedef
 * @brief 	:
 * @details	:
 *
 * @param
 * @param
 */
typedef struct{
    float     measuredValue; //oxygen concentration
    uint8_t   parameterId;
    uint8_t   unitId;
    uint8_t   dataQualityId;
    float     sentinelValue;
    uint8_t   avaiableUnits;
} rdoParam_t;

/*! @typedef
 * @brief 	:
 * @details	:
 *
 * @param
 * @param
 */
typedef struct{
    float     liveSalinity;
    float     defaultSalinity;
    float     liveBarometricPressure;
    float     defaultBarometricPressure;
    float     _100saturationConcentration;
    float     _100saturationTemperature;
    float     _100saturationSalinity;
    float     _100saturationBarometric;
    float     _0saturationConcentration;
    float     _0saturationTemperature;
    float     calibrationSlope;
    float     calibrationOffset;
    uint8_t   calibrationStatus;
} rdoCalibrationRegisters_t;

/*! @typedef
 * @brief 	:
 * @details	:
 *
 * @param
 * @param
 */
typedef struct{
    rdoStatus_t                 status;

    rdoHeadersRegisters_t       headers;
    rdoParam_t                  doConcentration;
    rdoParam_t                  temperature;
    rdoParam_t                  doSaturation;
    rdoParam_t                  oxygenPartialPressure;
    rdoParam_t                  externalVoltage;
    rdoCalibrationRegisters_t   calibration;
    
    //! General communication
    uint32_t requests;  //numero de envios
    uint32_t replies;   //numero de respuestas
    uint32_t errors;    //numero de errores
    
    //! Errors track
    rdoStatus_t     lastError;          //estado/comando que provoco el ultimo error

    //! auxiliar
    bool onCalibration;
    uint16_t equilibrium;
    rdoCalibSat_t points;

} rdo_t;

/***********************************************
 * @brief			: Function prototype
 **********************************************/

/*RET	NAME			argN							*/


/***********************************************
 * @brief			: Variables declaration
 **********************************************/



#endif