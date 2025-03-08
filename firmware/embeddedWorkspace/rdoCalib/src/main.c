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
uint8_t qReplies = 0;
uint8_t estado = 0;
uint8_t enterCalibMode [] 	= { 0xE0, 0x00 };
uint8_t tempCalib [] 		= { 0x41, 0xA0 , 0x00 , 0x00  }; //20
uint8_t updateCalib [] 		= { 0xE0, 0x01 };

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


    	if( modbusGetResponse(&sensorRdo) == SUCCES_VAL ){
    		LPUARTx_RI( RDO_UARTx, RX_IRQ_DIS	);
    		ticks = 0;
			switch( sensorRdo.response.fc ){

				case READ_MULTIPLE_HOLDING_REGISTERS:

					if( estado == 0){
						estado = 1;
						modbusReadMultipleHoldingRegisters(
								&sensorRdo	,
								0x01		,	//slave Address
								0x2D00		, 	//register Address
								0x0800			//qty
						);
					}

					else if( estado == 1){
						estado = 2;
//						modbusWriteSingleHoldingRegister(
//								&sensorRdo,
//								0x01	, //slaveAddress
////								0x2458	, //regAddress
//								0x5824	, //regAddress
////								0xE000
//								0x00E0
//						);

						modbusWriteMultipleHoldingRegisters(
								&sensorRdo,
								0x01	, //slaveAddress
								0x5824	, //startAddress
								0x0100	, //qtyRegs
								2		, //byte count
								enterCalibMode
						);
					}

					break;

				case WRITE_MULTIPLE_HOLDING_REGISTERS:

					if( sensorRdo.response.FC16.startingAddress == 0x2458 ){

						if( estado == 2 ){
							estado = 3;
							//1. DONE!
							//Write the Calibration Mode On command (0xE000) to the sensor command register.

							//4. TODO:
							/*
							 *  Read the oxygen concentration and temperature parameters.
							 *  When these values have reached equilibrium,
							 *  record them in their respective 100% saturation calibration registers.
							 *  Write the current live salinity and barometric pressure readings to
							 *  their respective calibration registers.
							 *
							 * */

							//genero un delay para simular que la gente
							//realiza alguna accion
							//delay( 100 );

							/*
							 *  record oxygen concentration and temperature
							 *  in their respective 100% saturation calibration registers.
							 *
							 * */
							//no me importa lo que guardo, sino que se guarde
							modbusWriteMultipleHoldingRegisters(
									&sensorRdo,
									0x01	, //slaveAddress
									0x7F00	, //startAddress
									0x0200	, //qtyRegs
									4		, //byte count
									tempCalib
							);
						}

						else if(estado == 4)
							estado = 5;

					}

					else if( sensorRdo.response.FC16.startingAddress == 0x7F ){
						estado = 4;

						modbusWriteMultipleHoldingRegisters(
								&sensorRdo,
								0x01	, //slaveAddress
								0x5824	, //startAddress
								0x0100	, //qtyRegs
								2		, //byte count
								updateCalib
						);
					}



					break;


				default:
					break;

			}
		}

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

	ticks++;

	if(ticks == 2000 ){

		if( estado == 0 )
			modbusReadMultipleHoldingRegisters(
					&sensorRdo	,
					0x01		,	//slave Address
					0x2500		, 	//register Address
					0x0800			//qty
			);

		else if( estado == 1){
			modbusReadMultipleHoldingRegisters(
					&sensorRdo	,
					0x01		,	//slave Address
					0x2D00		, 	//register Address
					0x0800			//qty
			);
		}

		else if( estado == 2 ){
			modbusWriteMultipleHoldingRegisters(
					&sensorRdo,
					0x01	, //slaveAddress
					0x5824	, //startAddress
					0x0100	, //qtyRegs
					2		, //byte count
					enterCalibMode
			);
		}

		else if( estado == 3){
			modbusWriteMultipleHoldingRegisters(
					&sensorRdo,
					0x01	, //slaveAddress
					0x7F00	, //startAddress
					0x0200	, //qtyRegs
					4		, //byte count
					tempCalib
			);
		}

		else if( estado == 4){
			modbusWriteMultipleHoldingRegisters(
					&sensorRdo,
					0x01	, //slaveAddress
					0x5824	, //startAddress
					0x0100	, //qtyRegs
					2		, //byte count
					updateCalib
			);
		}

		ticks = 0;
	}


}
