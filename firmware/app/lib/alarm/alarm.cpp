#include "Alarm.h"
#include <ArduinoLog.h>

volatile bool Alarm::alarmActive = true;  // Inicializamos el estado de la alarma

// Configura el temporizador de la alarma con un intervalo en segundos
void Alarm::setAlarm(int intervalInSeconds) {
    Log.info("Configurando alarma por software para %d segundos.\n", intervalInSeconds);
    
    // Configuración del temporizador
    const esp_timer_create_args_t timer_args = {
        .callback = &onTimer,
        .arg = nullptr,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "alarm_timer"
    };

    esp_timer_create(&timer_args, &periodic_timer);
    esp_timer_start_periodic(periodic_timer, intervalInSeconds * 1000000);  // Intervalo en microsegundos
}

// Pausar la alarma (detiene el temporizador)
void Alarm::pauseAlarm() {
    Log.info("Alarma pausada.\n");
    alarmActive = false;
    esp_timer_stop(periodic_timer);
}

// Reanudar la alarma (reinicia el temporizador)
void Alarm::resumeAlarm() {
    Log.info("Alarma reanudada.\n");
    alarmActive = true;
    //esp_timer_start_periodic(periodic_timer, intervalInSeconds * 1000000);  // Intervalo en microsegundos
}

// Detener la alarma completamente
void Alarm::stopAlarm() {
    Log.info("Alarma detenida.\n");
    esp_timer_stop(periodic_timer);
    esp_timer_delete(periodic_timer);
}

// Función que se llama cuando el temporizador se dispara
void Alarm::onTimer(void* arg) {
    if (alarmActive) {
        Log.info("Alarma activada: ejecutando tarea programada.\n");
        // Aquí puedes ejecutar tu código para cuando la alarma se dispare
    }
}
