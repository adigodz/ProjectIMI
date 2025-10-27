#include <SoftwareSerial.h>

SoftwareSerial sim(10, 11); // RX, TX (Arduino pins)

void setup() {
  Serial.begin(9600);  // Serial monitor
  sim.begin(9600);     // SIMCom module
  Serial.println("Starting SIMCom A7670C test...");

  delay(3000); // Give module time to power up

  sendATCommand("AT");       // Basic check
  // sendATCommand("AT+CSQ");   // Check signal quality
  // sendATCommand("AT+CREG?"); // Check network registration
  // sendATCommand("AT+CCID");  // Read SIM card ID
}

void loop() {
  while (sim.available()) {
    Serial.write(sim.read()); // Print any module responses
  }
  while (Serial.available()) {
    sim.write(Serial.read()); // Forward anything typed in Serial Monitor
  }
}

void sendATCommand(String cmd) {
  sim.println(cmd);
  delay(1000); // Wait for response
  while (sim.available()) {
    Serial.write(sim.read());
  }
  Serial.println();
}