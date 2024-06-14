#include <main.h>

// put function declarations here:
int myFunction(int, int);

void preTransmission() {
  digitalWrite(MAX485_DE, HIGH); // Enable transmission
}

void postTransmission() {
  digitalWrite(MAX485_DE, LOW); // Enable reception
}

void setup() {

  
  // use Serial (port 0); initialize Modbus communication baud rate
  Serial.begin(19200);
  // communicate with Modbus slave ID 2 over Serial (port 0)
  rdoSensor.begin(2,Serial);

  // Initialize DE/RE pin
  pinMode(MAX485_DE, OUTPUT);
  digitalWrite(MAX485_DE, LOW); // Start with receiving mode

    // Callbacks for RS485 transceiver control
  rdoSensor.preTransmission(preTransmission);
  rdoSensor.postTransmission(postTransmission);

}


void preTransmission() {
  digitalWrite(MAX485_DE, HIGH); // Enable transmission
}

void postTransmission() {
  digitalWrite(MAX485_DE, LOW); // Enable reception
}


void loop() {
  //no deberia ser necesario
  static uint32_t dummy;

  // Example Modbus communication
  uint8_t result;
  uint16_t data[2];
  dummy++;


  result = rdoSensor.readHoldingRegisters(0, 2);
  if (result == 0xAA) {
    for (int i = 0; i < 2; i++) {
      data[i] = rdoSensor.getResponseBuffer(i);
    }
  } else {

  }


  delay(1000);


}

// put function definitions here: