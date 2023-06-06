  #include <HTTPClient.h>
#include <BluetoothSerial.h>
#include <WiFi.h>
#include "Servo.h"

BluetoothSerial SerialBT;


Servo servo_obj1;
Servo servo_obj2;

int count_flush = 0;
char val;

#define powerpin 13 // water level 
#define sensorpin 34 //water level


void setup() {
  Serial.begin(9600);
  SerialBT.begin("playbook");
  pinMode(32, OUTPUT);
  pinMode(powerpin, OUTPUT);
  digitalWrite(powerpin, LOW);
  servo_obj1.attach(26);
  servo_obj2.attach(27);
  servo_obj2.write(45);
}

void loop() {
  if (SerialBT.available() > 0) {
    Serial.println("Serial is available");
    val = SerialBT.read();
    Serial.print("Val: ");
    Serial.println(val);
    if (val == '0') {
      digitalWrite(32, LOW);      // LED LOW
    }
    else if (val == '1') {
      digitalWrite(32, HIGH);     //LED HIGH
    }
    else if (val == '2') {
      servo_obj1.write(0);        // LID CLOSE
    }
    else if (val == '3') {
      servo_obj1.write(180);      // LID OPEN
    }
    else if (val == '4') {
      servo_obj2.write(0);        // FLUSH
      delay(500);
      servo_obj2.write(45);
      delay(3000);
      
      digitalWrite(powerpin, HIGH);
      int reading = analogRead(sensorpin);
      double waterlevel = (reading * 40) / 4096;
      digitalWrite(powerpin, LOW);
      Serial.print("Water Level: ");
      Serial.print(waterlevel);
      Serial.println("mm");
      //servo_obj3.write(0);
      if (waterlevel > 14)
      {
        SerialBT.println("1");
        Serial.println("Flush not working");
      }
    }
  }
  delay(1000);
}
