/**
 ******************************************************************************
 * @file           	: main.c
 * @author         	: Tobias Bavasso Piizzi
 * @brief			:
 * @date			: 2024/11/23
 * @version			: 1.00
 ******************************************************************************
 * @attention		:
 *
 *
 ******************************************************************************
 */

/***********************************************
 * @brief			: Header's inclusion
 **********************************************/
#include "S32K116.h"
#include <MbedSW.h>
#include <fw_all.h>
#include <modbusGatewayApi.h>


/***********************************************
 * @brief			: Global variables
 **********************************************/

int main(void) {
	int i = 0;
	uint16_t iTmp = 0;
	uint16_t iRdo = 0;

	uint8_t simuTmp [] = { 0x41, 0xC1, 0xDD, 0xA8 , 0x00 , 0x01 , 0x00 , 0x01 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00, 0x03 };
	uint8_t simuRdo [] = { 0x40, 0xE0, 0x00, 0x00 , 0x00 , 0x01 , 0x00 , 0x01 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00, 0x03 };

	DISABLE_INTERRUPTS();

	WDOGInit();								// !< Inicializacion del watchdog (Deshabilitarlo)
	ClockFIRC48MhzInit(); 					// !< Inicializo el FIRC a 48Mhz
	ClockNormalRun48MhzInit(); 				// !< Activo el modo Normal a 48Mhz
	ClockSysTickInit(SYSTICK_msec(1),1); 	// !< Habilito el SysTick
	ClockGpioInit();						// !< Habilito el Clockout @ GPIOx

	initGPIOS();
	initVARS();
	initPERIPHERICS();

	ENABLE_INTERRUPTS();


    for (;;) {
    	i++;

    	if( modbusValidateRequest(&esp) == SUCCES_VAL){

    		switch( esp.request.fc ){

    			case READ_MULTIPLE_HOLDING_REGISTERS:
//    				modbusReadMultipleHoldingRegisters(
//    						&sensorRdo,
//							esp.request.slaveAddress,
//							esp.request.FC03.startingAddress,
//							esp.request.FC03.qtyRegs
//    				);


    				if( esp.request.FC03.startingAddress == 0x2500){
						modbusResponseReadMultipleHoldingRegister(
								&esp,
								1,
								0x10,
								simuRdo
						);
						if( (iRdo++) %100 == 0){
							simuRdo[1] = 0;
						}
						simuRdo[1] = simuRdo[3]++;
    				}

    				if( esp.request.FC03.startingAddress == 0x2D00){
						modbusResponseReadMultipleHoldingRegister(
								&esp,
								1,
								0x10,
								simuTmp
						);
						if( (iTmp++) %100 == 0){
							simuTmp[2] = 0xDD;
						}
						simuTmp[2] = simuTmp[2] - 5;
    				}


    				break;

    			default:
    				break;

    		}
    	}

//    	if( modbusGetResponse(&sensorRdo) == SUCCES_VAL ){
//    		LPUARTx_RI( RDO_UARTx, RX_IRQ_DIS	);
//			switch( sensorRdo.response.fc ){
//
//				case READ_MULTIPLE_HOLDING_REGISTERS:
//					modbusResponseReadMultipleHoldingRegister(
//							&esp,
//							sensorRdo.response.slaveAddress,
//							sensorRdo.response.FC03.byteCount,
//							sensorRdo.response.FC03.regsValues
//					);
//					break;
//
//
//				default:
//					break;
//
//			}
//		}

    }
}

/***********************************************************************************
  * @function 	LPUART0_RxTx_IRQHandler
  * @brief  	Handler para la Rx Tx del SENSOR RDO
  * @param  	void
  * @retval 	void
  **********************************************************************************/
void LPUART0_RxTx_IRQHandler(void){

	uint8_t uart_rx;	//se queda
	uint8_t uart_tx;	//se queda

	/*
	 * OJO SI ME LLEGA UNA IRQ ME CONVIENE LEER SOLAMENTE UNA, Y QUE QUEDE PENDIENTE EL RESTO
	 * SINO EL HECHO DE LEER LOS FLAGS ES CONDICIÓN DE BORRADO Y ME COMO DATOS DE RECEPCIÓN
	 *
	 *
	 * POR ESO LOS ORDENO DE LA SIGUIENTE FORMA Y ANIDADOS. EL RETURN TAMBIÉN ME ASEGURO QUE SE SALGA LO MÁS
	 * RÁPIDO POSIBLE.
	 *
	 * ==> NO ES TRIVIAL EL ORDEN DE LA IRQ
	 *
	 * */
	////////////////////////
	//// 	RECEPTION	////
	////////////////////////
	//! Receive data buffer full
	if( LPUARTx_RX_BUFFER_FULL(RDO_UARTx) ){
		uart_rx = * sensorRdo.response.pRead;
		modbusPushToBuffResp( &sensorRdo , uart_rx);
		return;

	}
	////////////////////////
	//// 	RECEPTION	////
	////////////////////////

	////////////////////////
	//// TRANSMISSION	////
	////////////////////////
	//! Transmission Interrupt Enable
	else if( LPUARTx_CTRL_TIE(RDO_UARTx) &&  LPUARTx_TX_COMPLETE(RDO_UARTx) ){

		if( modbusPopFromBuffReq( &sensorRdo , &uart_tx ) == SUCCES_VAL ){
			*sensorRdo.request.pWrite = uart_tx;
			if( sensorRdo.request.inxInBuff !=  sensorRdo.request.inxOutBuff ){
				sensorRdo.request.rts->writeGpio( sensorRdo.request.rts , 1 , NULL );//de
				sensorRdo.request.rts->writeGpio( sensorRdo.request.cts , 1 , NULL );//re
				*sensorRdo.request.pWrite = uart_tx;
			}
			else{
				*sensorRdo.request.pWrite = uart_tx;
				for(uint16_t i = 0; i<1250; i++)
					NOP();
				sensorRdo.request.rts->writeGpio( sensorRdo.request.rts , 0 , NULL );//de
				sensorRdo.request.rts->writeGpio( sensorRdo.request.cts , 0 , NULL );//re
			}
		}
		else{
			sensorRdo.request.txDisable(NULL);
			sensorRdo.request.rts->writeGpio( sensorRdo.request.rts , 0 , NULL );//de
			sensorRdo.request.rts->writeGpio( sensorRdo.request.cts , 0 , NULL );//re
			LPUARTx_RI( RDO_UARTx, RX_IRQ_ENA	);
		}

		return;
	}
	////////////////////////
	//// TRANSMISSION	////
	////////////////////////



}

/***********************************************************************************
  * @function 	LPUART0_RxTx_IRQHandler
  * @brief  	Handler para la Rx Tx de ESP
  * @param  	void
  * @retval 	void
  **********************************************************************************/
void LPUART1_RxTx_IRQHandler(void){

	uint8_t uart_rx;	//se queda
	uint8_t uart_tx;	//se queda

	/*
	 * OJO SI ME LLEGA UNA IRQ ME CONVIENE LEER SOLAMENTE UNA, Y QUE QUEDE PENDIENTE EL RESTO
	 * SINO EL HECHO DE LEER LOS FLAGS ES CONDICIÓN DE BORRADO Y ME COMO DATOS DE RECEPCIÓN
	 *
	 *
	 * POR ESO LOS ORDENO DE LA SIGUIENTE FORMA Y ANIDADOS. EL RETURN TAMBIÉN ME ASEGURO QUE SE SALGA LO MÁS
	 * RÁPIDO POSIBLE.
	 *
	 * ==> NO ES TRIVIAL EL ORDEN DE LA IRQ
	 *
	 * */

	////////////////////////
	//// 	RECEPTION	////
	////////////////////////
	//! Receive data buffer full
	if( LPUARTx_RX_BUFFER_FULL(ESP_UARTx) ){
		uart_rx = * esp.response.pRead;
		modbusPushToRawReq( &esp , uart_rx);
		return;
	}
	////////////////////////
	//// 	RECEPTION	////
	////////////////////////

	////////////////////////
	//// TRANSMISSION	////
	////////////////////////
	//! Transmission Interrupt Enable
	else if( LPUARTx_CTRL_TIE(ESP_UARTx) &&  LPUARTx_TX_COMPLETE(ESP_UARTx) ){

		if( modbusPopFromBuffResp( &esp , &uart_tx ) == SUCCES_VAL ){
			*esp.request.pWrite = uart_tx;
		}
		else{
			esp.request.txDisable(NULL);
		}

		return;
	}
	////////////////////////
	//// TRANSMISSION	////
	////////////////////////

}

/***********************************************************************************
 * @function SysTick_Handler
 * @brief  Estructura systick 1 msegundo
 * @param  void
 * @retval void
 **********************************************************************************/
void SysTick_Handler(void) {
	int i = 0;
	i++;
}
