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

/***********************************************
 * @brief			: 	Function initialization prototype
 **********************************************/
/* ----------------------------------------------------------------------------
   -- RDO
   ---------------------------------------------------------------------------- */
/***********************************************************************************
  * @function 	initGpiosRDO
  * @brief  	Inicializa cosas basicas del RDO
  * @param
  * @retval
  **********************************************************************************/
 void initGpiosRDO (void){

   //pinMode(RDO_DE_RE_GPIO, OUTPUT);
   ESP_ERROR_CHECK(uart_set_pin(RDO_UART_NUM, _RDO_GPIOS_ ));
 }

 /***********************************************************************************
  * @function 	initUartRDO
  * @brief  	Inicializa el periferico UART
  * @param
  * @retval
  **********************************************************************************/
 void initUartRDO (void){
    rdoUARTconfig = {
      .baud_rate  = RDO_BAUD_RATE,
      .data_bits  = RDO_DATA_BITS,
      .parity     = RDO_PARITY,
      .stop_bits  = RDO_STOP_BITS,
      .flow_ctrl  = RDO_FLOW_CONTROL,
      .rx_flow_ctrl_thresh = RDO_FLOW_CONTROL_THR,
   };

   // Configure UART parameters
   ESP_ERROR_CHECK(uart_param_config(RDO_UART_NUM, &rdoUARTconfig));

 }

  /***********************************************************************************
  * @function 	driverUartRDO
  * @brief  	instala el driver
  * @param
  * @retval
  **********************************************************************************/
 void driverUartRDO (void){
    ESP_ERROR_CHECK(uart_driver_install(RDO_UART_NUM, RDO_BUFFER_SIZE, 0, 0, NULL, 0));
 }