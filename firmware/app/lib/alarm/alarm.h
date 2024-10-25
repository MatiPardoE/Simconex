#ifndef ALARM_H
#define ALARM_H

#include "esp_timer.h"

class Alarm {
public:
    Alarm();
    // Configura el temporizador de la alarma con un intervalo en segundos
    void setAlarm(uint64_t intervalInSeconds, void (*onTimer)(void*));

    // Pausar la alarma (detiene el temporizador)
    void pauseAlarm();

    // Reanudar la alarma (reinicia el temporizador)
    void resumeAlarm();

    // Detener la alarma completamente
    void stopAndDeleteAlarm();

private:
    esp_timer_handle_t periodic_timer; // Maneja el temporizador
    bool alarmActive;  // Estado de la alarma (activa o pausada)
    bool alarmAlive;  // Estado de la alarma (creada o eliminada)
    uint64_t alarmIntrevalSeconds;  // Intervalo en segundos
};

#endif // ALARM_H
