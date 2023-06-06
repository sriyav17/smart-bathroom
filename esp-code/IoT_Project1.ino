//code without app
#include <HTTPClient.h>
//#include <BluetoothSerial.h>
#include <WiFi.h>
//#include "time.h"
//#include "ArduinoJson.h"
#include "Servo.h"

//BluetoothSerial SerialBT;

// DONT CHANGE PINSSSSSS
#define trigpin 2   // yellow
#define echopin 4   // yellow
#define ledpin 32   // blue and brown
#define powerpin 13 // water level 
#define sensorpin 34 //water level

#define servo_pin1 26   // blue and white
#define servo_pin2 27   // purple and brown
//#define servo_pin3 35

char* wifi_ssid = "Anush";
char* wifi_pwd = "12345678";
String cse_ip = "192.168.163.119"; 
String cse_port = "8080";
String server = "http://" + cse_ip + ":" + cse_port + "/~/in-cse/in-name/";
String ae = "IoT_Project";
String cnt1 = "People_Count";
String cnt2 = "Flush_Count";

HTTPClient http;

Servo servo_obj1;
Servo servo_obj2;
//Servo servo_obj3;

double prevdistance = 70;
int count_ppl=0;
int count_flush = 0;

int createCI1(String& val) {
  HTTPClient http;
  http.begin(server + ae + "/" + cnt1 + "/");

  http.addHeader("X-M2M-Origin", "admin:admin");
  http.addHeader("Content-Type", "application/json;ty=4");

  int code = http.POST("{\"m2m:cin\": {\"cnf\":\"application/json\",\"con\": " + String(val) + "}}");

  Serial.print("Response Code: ");
  Serial.println(code);
  if (code == -1) {
    Serial.println("Unable To Connect To The Server");
  }
  
  http.end();
}

int createCI2(String& val) {
  HTTPClient http;
  http.begin(server + ae + "/" + cnt2 + "/");

  http.addHeader("X-M2M-Origin", "admin:admin");
  http.addHeader("Content-Type", "application/json;ty=4");

  int code = http.POST("{\"m2m:cin\": {\"cnf\":\"application/json\",\"con\": " + String(val) + "}}");

  Serial.print("Response Code: ");
  Serial.println(code);
  if (code == -1) {
    Serial.println("Unable To Connect To The Server");
  }
  
  http.end();
}

void setup() {
  Serial.begin(9600);
//  SerialBT.begin("playbook");
  pinMode(echopin,INPUT);
  pinMode(trigpin,OUTPUT); 
  pinMode(ledpin, OUTPUT);
  pinMode(powerpin, OUTPUT);
  digitalWrite(powerpin, LOW);
  servo_obj1.attach(26);
  servo_obj2.attach(27);
  //servo_obj3.attach(35);
  servo_obj2.write(45);

    WiFi.begin(wifi_ssid,wifi_pwd); 
  
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  Serial.println("Succesfully connected to WiFi.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(10);
}

void loop() {
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin,LOW);
  long duration=pulseIn(echopin,HIGH);
  double distance=duration*0.034/2;
  Serial.print("Distance:");
  Serial.print(distance);
  Serial.println(" cm");
    // user entering - open toilet lid and switch on the LED
   if(distance < 25)
   {
    digitalWrite(ledpin, HIGH);
         
    if(distance < 15)
    {
      servo_obj1.write(180);
      if(prevdistance > 15)
      {
        count_ppl++;
      }
    }
    else
    {
      servo_obj1.write(0);
  
      // flush the toilet
      if(prevdistance < 15)
      {
        servo_obj2.write(0);
        //servo_obj3.write(180);
        delay(500);
        servo_obj2.write(45);
        delay(1000);
        digitalWrite(powerpin, HIGH);
        int reading = analogRead(sensorpin);
        double waterlevel = (reading*40)/4096;
        digitalWrite(powerpin, LOW);
        Serial.print("Water Level: ");
        Serial.print(waterlevel); 
        Serial.println("mm");
        //servo_obj3.write(0);
        if(waterlevel > 14)
        {
          Serial.println("Flush not working");
        }
        else
        {
          count_flush++;
        }
      }
    }
   }
  
    // user leaving
    else
    {
      // switch off LED and close the lid
      digitalWrite(ledpin, LOW);
      servo_obj1.write(0);

      if(prevdistance < 15)
      {
        servo_obj2.write(0);
        delay(500);
        servo_obj2.write(45);
        delay(1000);

        digitalWrite(powerpin, HIGH);
        int reading = analogRead(sensorpin);
        double waterlevel = (reading*40)/4095;
        digitalWrite(powerpin, LOW);
        Serial.print("Water Level: ");
        Serial.print(waterlevel); 
        Serial.println("mm");
   
        if(waterlevel > 14)
        {
          Serial.println("Flush not working");
        }
        else
        {
          count_flush++;
        }
        
      }
    }
    prevdistance = distance;

    Serial.print("No. of people: ");
    Serial.println(count_ppl);
    String val1 = String(count_ppl);
    createCI1(val1);

    Serial.print("Flush Count: ");
    Serial.println(count_flush);
    String val2 = String(count_flush);
    createCI2(val2);
    delay(1000);
}
