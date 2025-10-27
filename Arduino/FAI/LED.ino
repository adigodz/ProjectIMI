    // ใช้ GPIO2
int var1;
int led1 = 13;
void setup() 
{
  Serial.begin(9600);
  pinMode(led1, OUTPUT);
  digitalWrite(led1, LOW);
}

void loop() 
{
  if (Serial.available())
  {
    var1 = Serial.read();
    if (var1 == 'a')
    {
      digitalWrite(led1, HIGH);  // เปิด LED
    }
    else if (var1 == 'b')
    {
      digitalWrite(led1, LOW);   // ปิด LED
    }
  }
}
