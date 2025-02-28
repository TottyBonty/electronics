
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//I2C pins declaration
LiquidCrystal_I2C lcd(0x3F, 16, 2);

int smokeA0 = A0;
//int buzzer = 11;
int ledGreen = 12;
int ledRed = 8;
float sensorValue;

void setup() {
  //pinMode(buzzer,OUTPUT);
  pinMode(ledGreen,OUTPUT);
  pinMode(ledRed,OUTPUT);
  pinMode(smokeA0,INPUT);
  Serial.begin(9600); // sets the serial port to 9600
  //Serial.println("Gas sensor warming up!");
  delay(20000); // allow the MQ-2 to warm up
  //noTone(buzzer);
  
}

void loop() {
  sensorValue=analogRead(smokeA0);
  
  if (sensorValue > 300) {
    Serial.print(" | Smoke detected!");
    //tone(buzzer,1000,200);
    digitalWrite(ledRed,HIGH);
    digitalWrite(ledGreen,LOW);
  } else {
    Serial.print(" | Smoke  not detected!");
    //noTone(buzzer);    
    digitalWrite(ledRed,LOW);
    digitalWrite(ledGreen,HIGH);
  }
  
  delay(500); // wait 2s for next reading
}
