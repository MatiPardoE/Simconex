/**
 ******************************************************************************
 * @file           	: apis.h
 * @author         	: Tobias Bavasso Piizzi
 * @brief			: 
 * @date			: 2025/02/01
 * @version			: 1.00
 ******************************************************************************
 * @attention		: This file it's NOT dependent from
 * 					  uc's family used
 *
 ******************************************************************************
 */

#ifndef _APIS_H
#define _APIS_H

/***********************************************
 * @brief			: Header's inclusion
 **********************************************/
#include <rdoApiCode.h>

/***********************************************
 * @brief			: Defines
 **********************************************/
#define TIME_WAIT_CALIBRATION 40000

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


/***********************************************
 * @brief			: Function prototype
 **********************************************/

/*RET	NAME			argN							*/
rdoCalibStatus_t isDoSatCalibrationSucces(void);

/***********************************************
 * @brief			: Variables declaration
 **********************************************/
extern unsigned long millis_init;
extern bool waitingEndCalibration;
extern volatile rdo_t rdo;


#endif