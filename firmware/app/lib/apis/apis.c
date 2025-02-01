/***********************************************
 * @brief			: Header's inclusion
 **********************************************/
#include <Arduino.h>
#include <apis.h>

rdoCalibStatus_t isDoSatCalibrationSucces(void){
    
    //EXPLAIN:
    /*  1 VEZ TOMO EL TIEMPO
    *   LUEGO DEVUELVO SI ESTOY DENTRO DEL TIEMPO DE ESPERA
    *   DEVUELVO SI SE HIZO UNA CALIBRACION SEGUN CORRESPONDA
    *   SI SE CUMPLIO EL TIEMPO DEVUELVO FALSO
    */
    if(waitingEndCalibration == false){
        waitingEndCalibration = true;
        millis_init = millis();
    }

    if( (unsigned long) (millis()-millis_init) < TIME_WAIT_CALIBRATION ){
        if (isAnyCalibrationDone(&rdo))
        {
            waitingEndCalibration = false;
            return CALIBRATION_SUCCES;
        }
        return CALIBRATION_IN_PROCESS;
    }
    else
        return CALIBRATION_FAIL;

}