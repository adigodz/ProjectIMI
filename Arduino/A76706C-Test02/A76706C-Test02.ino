#include <SoftwareSerial.h>

// กำหนดพินสำหรับโมดูล GSM (SIM Module)
SoftwareSerial GPRS(10, 11); // RX, TX

// Pin Definitions
int switchPin = 2;       // สวิตช์ส่ง SMS
int pbuttonpin = 4;      // ปุ่มกดควบคุมรีเลย์
int relaypin = 6;        // พินควบคุมรีเลย์

// ตัวแปรสถานะ
boolean state, lastState;
boolean sentRechargeEmpty = false;
boolean sentRechargeDone = false;
int val = 0;
int lighton = LOW;
int pushed = 0;
int lastPushed = -1;
int flagl = 0;
int flagh = 0;

// ฟังก์ชันส่งข้อความ SMS
void sendSMS(String message) {
  Serial.println("Sending SMS...");
  GPRS.println("AT+CMGF=1"); // ตั้งโหมดเป็นข้อความ
  delay(500);

  GPRS.println("AT+CMGS=\"+\""); // เบอร์ปลายทาง (เปลี่ยนตามจริง)
  delay(500);

  GPRS.print(message);
  Serial.println(message);
  GPRS.write(0x1A); // Ctrl+Z เพื่อส่ง
  delay(500);
  Serial.println("SMS Sent.");
}

void setup() {
  // Serial สำหรับ Debug
  Serial.begin(9600);
  Serial.println("Initializing...");

  // ตั้งโหมดขาพิน
  pinMode(switchPin, INPUT_PULLUP);
  pinMode(pbuttonpin, INPUT_PULLUP);
  pinMode(relaypin, OUTPUT);
  digitalWrite(relaypin, HIGH); // ปิดรีเลย์เริ่มต้น

  // เริ่มต้นการสื่อสารกับ GSM Module
  GPRS.begin(9600);
  delay(1000);
  GPRS.println("AT");

  // อ่านสถานะเริ่มต้นของสวิตช์และปุ่ม
  state = digitalRead(switchPin);
  lastState = state;
  lighton = digitalRead(pbuttonpin);
  delay(500);

  Serial.println("System Ready.");
}

void loop() {
  // ตรวจจับสวิตช์เพื่อส่ง SMS
  lastState = state;
  state = digitalRead(switchPin);

  if (state != lastState) {
    delay(50); // กันเด้ง

    if (state == LOW) {
      if ((flagl % 2) != 0) {
        sendSMS("Recharge empty.");
        sentRechargeEmpty = true;
      } else {
        flagl++;
      }
    }

    if (state == HIGH) {
      if ((flagh % 2) != 0) {
        sendSMS("Recharge done.");
        sentRechargeDone = true;
      } else {
        flagh++;
      }
    }
  }

  // ปุ่มควบคุมรีเลย์
  val = digitalRead(pbuttonpin);
  if (val == HIGH && lighton == LOW) {
    delay(50);
    if (digitalRead(pbuttonpin) == HIGH) {
      pushed = 1 - pushed; // toggle
    }
  }
  lighton = val;

  // อัปเดตสถานะรีเลย์
  if (pushed != lastPushed) {
    lastPushed = pushed;

    if (pushed == HIGH) {
      Serial.println("LIGHT ON");
      digitalWrite(relaypin, LOW); // เปิดรีเลย์
    } else {
      Serial.println("LIGHT OFF");
      digitalWrite(relaypin, HIGH); // ปิดรีเลย์
    }
  }

  delay(100);
}
