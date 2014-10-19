#include <SoftwareSerial.h>
SoftwareSerial BT_Serial(10,11); //tx, rx

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  BT_Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (BT_Serial.available())
  {
    while (BT_Serial.available()) {
      Serial.write(BT_Serial.read());
      
      if (BT_Serial.available() == 0)
        delay(1);
    }
    Serial.write("\n");
  }
    
   if (Serial.available())
   {
     byte b = Serial.read();
     
     BT_Serial.write(b);
//     Serial.write(b);
   }
}
