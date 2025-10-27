// =================== Configuration ===================
#define TINY_GSM_MODEM_SIM800   // ใช้สำหรับ SIM A7670C / SIMCOM series
#include <TinyGsmClient.h>

#define GSM_PIN ""                 // ถ้ามี PIN ของซิมใส่ที่นี่
#define ADMIN_NUMBER "+66622436287" // เบอร์ผู้รับ SMS

// =================== ESP32 Hardware Serial ===================
#define MODEM_RX 27   // RX ของ ESP32 → TX โมดูล
#define MODEM_TX 26   // TX ของ ESP32 → RX โมดูล
#define MODEM_PWRKEY 4
#define MODEM_POWER_ON 12
#define MODEM_RESET 5
#define MODEM_RESET_LEVEL HIGH

HardwareSerial SerialAT(1); // Serial1 ของ ESP32
TinyGsm modem(SerialAT);

// =================== Variables ===================
String phoneNumber = "";
String smsText = "";

// =================== Setup ===================
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32 Dev + SIM A7670C SMS Test");

  // Power ON โมดูล
  pinMode(MODEM_POWER_ON, OUTPUT);
  digitalWrite(MODEM_POWER_ON, HIGH);

  // Hard reset โมดูล
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

  // เริ่ม Serial1 กับโมดูล
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(3000);

  // Initialize modem
  Serial.println("Initializing modem...");
  if (!modem.init()) {
    Serial.println("Failed to initialize modem!");
    while (true); // หยุดโค้ด
  }

  Serial.print("Modem Info: ");
  Serial.println(modem.getModemInfo());

  // Unlock SIM ถ้ามี PIN
  if (strlen(GSM_PIN) > 0 && modem.getSimStatus() != 3) {
    modem.simUnlock(GSM_PIN);
  }

  // รอเชื่อมเครือข่าย
  Serial.print("Waiting for network...");
  if (!modem.waitForNetwork()) {
    Serial.println(" fail");
    while(true);
  }
  Serial.println(" success");

  // ส่ง SMS ตัวอย่าง
  bool res = modem.sendSMS(ADMIN_NUMBER, "Hello from ESP32 Dev + SIM A7670C!");
  Serial.print("Send SMS: ");
  Serial.println(res ? "OK" : "Fail");

  // ตั้งค่า SMS Text Mode และ Notifications
  SerialAT.println("AT+CMGF=1");       // Text mode
  delay(1000);
  SerialAT.println("AT+CNMI=2,2,0,0,0"); // SMS notifications
  delay(1000);

  Serial.println("Setup done. Ready to receive SMS...");
}

// =================== Loop ===================
void loop() {
  if (SerialAT.available()) {
    String response = SerialAT.readStringUntil('\n');
    response.trim();
    if (response.length() == 0) return;

    Serial.println(response);

    // ตรวจสอบ SMS เข้า
    if (response.startsWith("+CMT:")) {
      int firstQuote = response.indexOf("\"") + 1;
      int secondQuote = response.indexOf("\"", firstQuote);
      phoneNumber = response.substring(firstQuote, secondQuote);
      Serial.print("Incoming SMS from: ");
      Serial.println(phoneNumber);

      // อ่านเนื้อหาข้อความ
      if (SerialAT.available()) {
        smsText = SerialAT.readStringUntil('\n');
        smsText.trim();
        Serial.print("Message: ");
        Serial.println(smsText);
      }
    }
  }
}
