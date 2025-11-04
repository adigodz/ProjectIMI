#include <SoftwareSerial.h>

#define RX_PIN 2
#define TX_PIN 3

SoftwareSerial simSerial(RX_PIN, TX_PIN);

void setup() {
  Serial.begin(115200);           // Serial Monitor
  simSerial.begin(115200);        // เริ่ม SoftwareSerial ที่ baud ปัจจุบันของโมดูล

  delay(1000);

  // ตั้งค่าโมดูลเป็น 9600 และบันทึก
  sendCommand("AT+IPR=9600"); // เปลี่ยน baud โมดูล
  sendCommand("AT&W");        // บันทึกค่า

  // เปลี่ยน Arduino ให้ตรงกับโมดูลใหม่
  simSerial.end();             // ปิด SoftwareSerial เก่าก่อน
  simSerial.begin(9600);       // เปิด SoftwareSerial ที่ 9600

  Serial.println("Start module A7670E...");
  delay(3000);

  sendCommand("AT");
  sendCommand("ATE1");
  sendCommand("AT+CPIN?");
  sendCommand("AT+CSQ");
  sendCommand("AT+CREG?");

  Serial.println("Type 'call' or 'hang' or 'answer'");
}

void loop() {
  String command = Serial.readStringUntil('\n');
  command.trim();

  if (command == "call") {
    makeCall("+66937529340");
 // เปลี่ยนเบอร์ปลายทาง
  } else if (command == "hang") {
    hangUp();
  } else if (command == "answer") {
    answerCall();
  } else if (command.length() > 0) {
    simSerial.println(command);
  }

  if (simSerial.available()) {
    String resp = simSerial.readString();
    Serial.println(resp);
  }
}

void makeCall(String phoneNumber) {
  String cmd = "ATD" + phoneNumber + ";";
  Serial.println("Calling...");
  sendCommand(cmd);
}

void hangUp() {
  Serial.println("Hang up...");
  sendCommand("ATH");
}

void answerCall() {
  Serial.println("Answer...");
  sendCommand("ATA");
}

// 🧩 เพิ่มฟังก์ชันนี้
void sendCommand(String cmd) {
  simSerial.println(cmd);
  delay(1000);
  while (simSerial.available()) {
    String response = simSerial.readString();
    Serial.println(response);
  }
}
