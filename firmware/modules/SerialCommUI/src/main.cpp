#include <Arduino.h>

void setup() {
  // Iniciar la comunicación serie a 115200 baudios
  Serial.begin(115200);
}

void loop() {
  // Si hay datos disponibles en el puerto serie
  if (Serial.available() > 0) {
    // Leer los datos
    String incomingString = Serial.readString();
    
    // Si los datos leídos son "INIT"
    if (incomingString.equals("#INIT!")) {
      // Enviar "ESP" de vuelta
      Serial.println("ESP");
    }
  }
}