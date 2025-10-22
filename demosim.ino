#include <SoftwareSerial.h>

#define RX_PIN 7
#define TX_PIN 8

SoftwareSerial simSerial(RX_PIN, TX_PIN);

void setup() {
  //swicth baud rate if cannot connect
  Serial.begin(9600);
  simSerial.begin(155200);

  Serial.println("Start module A7670E...");
  delay(3000);

  sendCommand("AT");
  sendCommand("ATE1");
  sendCommand("AT+CPIN?");
  sendCommand("AT+CSQ");
  sendCommand("AT+CREG?");

  Serial.println("type 'call' or 'hang' or 'answer");
}

void loop() {
  String command = Serial.readStringUntil('\n');

  command.trim();
  if (command == "call"){
    makeCall("0812345678"); // change to destination number
  } else if (command == "hang") {
    hangUp();
  } else if (command == "answer") {
    answerCall();
  } else {
    simSerial.println(command);
  }

  if (simSerial.available()) {
    String resp = simSerial.readString();
    Serial.println(resp);
  }

}

void makeCall(String phoneNumber) {
  String cmd = "ATD" + phoneNumber +";";
  Serial.println("Calling...");
  sendCommand(cmd);
}

void hangUp() {
  Serial.println("hang up...");
  sendCommand("ATH");
}

void answerCall() {
  Serial.println("Answer...");
  sendCommand("ATA");
}
