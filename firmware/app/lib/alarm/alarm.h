#ifndef ALARM_H
#define ALARM_H

#include "esp_timer.h"

class Alarm {
public:
    // Configura el temporizador de la alarma con un intervalo en segundos
    void setAlarm(int intervalInSeconds);

    // Pausar la alarma (detiene el temporizador)
    void pauseAlarm();

    // Reanudar la alarma (reinicia el temporizador)
    void resumeAlarm();

    // Detener la alarma completamente
    void stopAlarm();

private:
    esp_timer_handle_t periodic_timer; // Maneja el temporizador
    static volatile bool alarmActive;  // Estado de la alarma (activa o pausada)

    // Función que se llama cuando el temporizador se dispara
    static void onTimer(void* arg);
};

#endif // ALARM_H
