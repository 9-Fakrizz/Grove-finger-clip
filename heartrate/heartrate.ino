#include <Wire.h>

#define trigPin 5 

long timer0 = 0;

void setup() {
    Wire.begin();
    Serial.begin(9600);
    Serial.println("heart rate sensor:");
    pinMode(trigPin, OUTPUT);
    digitalWrite(trigPin, LOW);
    timer0 = millis();
}
void loop() {
    if(millis() - timer0 >= 30000 ){   // trigger every 20 second
      digitalWrite(trigPin, HIGH);
      delay(1000);
      digitalWrite(trigPin, LOW);
      Serial.println("New ROUND -----------------------------------");
      timer0 = millis();
    }

    Serial.print("heart rate sensor: ");
    Wire.requestFrom(0xA0 >> 1, 1);    // request 1 bytes from slave device
    while(Wire.available()) {          // slave may send less than requested
        unsigned char c = Wire.read();   // receive heart rate value (a byte)
        Serial.print(c, DEC);         // print heart rate value
    }
    Serial.println();

    delay(5000);
}