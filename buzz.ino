// Buzzer control via Serial (ON / OFF)
int buzzerPin = 8;

void setup() {
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
  Serial.begin(9600); // อย่าลืมค่า baud ให้ตรงกับ LabVIEW
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n'); // อ่านจนเจอ newline
    cmd.trim();
    if (cmd == "1") {
      digitalWrite(buzzerPin, HIGH);
    } else if (cmd == "0") {
      digitalWrite(buzzerPin, LOW);
    }
  }
}
