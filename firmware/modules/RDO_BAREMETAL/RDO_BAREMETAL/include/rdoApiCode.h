/**
 ******************************************************************************
 * @file           	: powerTrainApi.h
 * @author         	: Tobias Bavasso Piizzi
 * @brief			: 
 * @date			: 2024/08/13
 * @version			: 1.00
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

#ifndef _RDO_API_CODE_H
#define _RDO_API_CODE_H

/***********************************************
 * @brief			: Header's inclusion
 **********************************************/
#include <rdoApiDefs.h>
#include <rdoApi.h>
#include <rdoApiGlobalVariables.h>

#include <cBuff.h>


/***********************************************
 * @brief			: 	Function initialization prototype
 **********************************************/
/*RET	NAME			argN							*/


/***********************************************
 * @brief			: 	Function project prototype
 **********************************************/

/*RET	NAME			argN							*/
extern void clearRDO   ( void  );
extern void requestRDO ( volatile rdo_t * rdo );



#endif