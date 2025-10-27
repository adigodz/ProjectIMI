// =================== AT Command Test for ESP32 + SIM A7670C ===================

#include <HardwareSerial.h>

#define MODEM_RX 27   // RX ESP32 → TX โมดูล
#define MODEM_TX 26   // TX ESP32 → RX โมดูล
#define MODEM_PWRKEY 4
#define MODEM_POWER_ON 12
#define MODEM_RESET 5
#define MODEM_RESET_LEVEL HIGH

HardwareSerial SerialAT(1); // Serial1

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32 Dev + SIM A7670C AT Command Test");

  // เปิด POWER_ON โมดูล
  pinMode(MODEM_POWER_ON, OUTPUT);
  digitalWrite(MODEM_POWER_ON, HIGH);

  // Reset โมดูล
  pinMode(MODEM_RESET, OUTPUT);
  digitalWrite(MODEM_RESET, !MODEM_RESET_LEVEL);
  delay(100);
  digitalWrite(MODEM_RESET, MODEM_RESET_LEVEL);
  delay(2600);
  digitalWrite(MODEM_RESET, !MODEM_RESET_LEVEL);

  // Toggle PWRKEY
  pinMode(MODEM_PWRKEY, OUTPUT);
  digitalWrite(MODEM_PWRKEY, LOW);
  delay(100);
  digitalWrite(MODEM_PWRKEY, HIGH);
  delay(1000);
  digitalWrite(MODEM_PWRKEY, LOW);

  // เริ่ม Serial1
  SerialAT.begin(9600, SERIAL_8N1, MODEM_RX, MODEM_TX); // บางโมดูลใช้ 9600
  delay(3000);

  Serial.println("Sending AT command...");
  SerialAT.println("AT"); // ส่งคำสั่ง AT
}

void loop() {
  // อ่าน Serial โมดูล
  if (SerialAT.available()) {
    String response = SerialAT.readStringUntil('\n');
    response.trim();
    if (response.length() > 0) {
      Serial.print("Modem Response: ");
      Serial.println(response);
    }
  }

  // ถ้าอยากทดสอบเรื่อยๆ ให้กดปุ่มหรือส่งคำสั่ง AT ซ้ำ
}

