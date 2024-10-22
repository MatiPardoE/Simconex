#include "Alarm.h"
#include <ArduinoLog.h>

Alarm::Alarm()
{
    alarmActive = false;
    alarmAlive = false;
    alarmIntrevalSeconds = 0;
}
// Configura el temporizador de la alarma con un intervalo en segundos
void Alarm::setAlarm(int intervalInSeconds, void (*onTimer)(void *))
{
    Log.info("Configurando alarma por software para %d segundos.\n", intervalInSeconds);
    alarmIntrevalSeconds = intervalInSeconds;
    // Configuración del temporizador
    const esp_timer_create_args_t timer_args = {
        .callback = onTimer,
        .arg = nullptr,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "alarm_timer"};

    esp_err_t err;

    err = esp_timer_create(&timer_args, &periodic_timer);
    if (err != ESP_OK)
    {
        Log.error("Error al crear el temporizador: %d\n", err);
        return;
    }

    err = esp_timer_start_periodic(periodic_timer, intervalInSeconds * 1000000); // Intervalo en microsegundos
    if (err != ESP_OK)
    {
        Log.error("Error al iniciar el temporizador: %d\n", err);
        esp_timer_delete(periodic_timer);
        return;
    }
    alarmAlive = true;
    alarmActive = true;
    Log.info("Alarma configurada y activada para %d segundos.\n", intervalInSeconds);
}

// Pausar la alarma (detiene el temporizador)
void Alarm::pauseAlarm()
{
    if(alarmAlive){
        Log.info("Alarma pausada.\n");
        alarmActive = false;
        esp_timer_stop(periodic_timer);
    }else{
        Log.error("La alarma no ha sido configurada.\n");
    }
}

// Reanudar la alarma (reinicia el temporizador)
void Alarm::resumeAlarm()
{
    if (alarmAlive)
    {
        Log.info("Alarma reanudada.\n");
        alarmActive = true;
        esp_timer_start_periodic(periodic_timer, alarmIntrevalSeconds * 1000000); // Intervalo en microsegundos
    }
    else
    {
        Log.error("La alarma no ha sido configurada.\n");
    }
}

// Detener la alarma completamente
void Alarm::stopAndDeleteAlarm()
{
    if (alarmAlive == true)
    {
        Log.info("Alarma detenida.\n");
        esp_timer_stop(periodic_timer);
        esp_timer_delete(periodic_timer);
        alarmAlive = false;
    }
    else
    {
        Log.error("La alarma ya fue desactivada y no existe.\n");
    }
    return;
}
