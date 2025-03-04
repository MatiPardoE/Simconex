/**
 ******************************************************************************
 * @file           	: rdoApiGlobalVariables.h
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

#ifndef _RDO_API_GLOBAL_VARIABLES_H
#define _RDO_API_GLOBAL_VARIABLES_H


/***********************************************
 * @brief			: Header's inclusion
 **********************************************/
#include <ModbusMessage.h>
#include <esp32ModbusRTU.h>
#include <algorithm>  // for std::reverse

#include <rdoApiDefs.h>
#include <rdoApi.h>


/***********************************************
 * @brief			: Variables declaration
 **********************************************/

/* ----------------------------------------------------------------------------
   -- #IRQ Handlers
   ---------------------------------------------------------------------------- */


/* ----------------------------------------------------------------------------
   -- preDriver
   ---------------------------------------------------------------------------- */

/* ----------------------------------------------------------------------------
-- RDO
---------------------------------------------------------------------------- */

    extern esp32ModbusRTU modbus;
    extern uint32_t lastMillisRDO;
    extern volatile rdo_t   rdo;
    extern bool initRDO;

    extern uint8_t calModeOn                        [];    
    extern uint8_t saturationBarometricPressure100  [];
    extern uint8_t saturationSalinity100            [];
    extern uint8_t defaultBarometricPressure        [];
    //
    extern uint8_t calUpdate    [];
    extern uint8_t calModeOff   []; 






#endif
