
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//LCD I2C declaration
uint8_t lcdAddress = 0x3F;
uint8_t lcd_cols = 16;
uint8_t lcd_rows = 2;
LiquidCrystal_I2C lcd(lcdAddress, lcd_cols, lcd_rows);  //Defining 16 columns and 2 rows of lcd display

//Input Pin declaration
int smokeA0 = A0;

//Output Pin declaration
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
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MQ2 Sensor");
  lcd.setCursor(0, 1);
  lcd.print("warming up");
  
  //noTone(buzzer);

  delay(10000); // allow the MQ-2 to warm up  
}

void loop() {
  //lcd.clear();
  
  sensorValue=analogRead(smokeA0);
  // lcd.setCursor(0, 0);
  // lcd.print("Sensor Value : ");
  // lcd.setCursor(0, 1);
  // lcd.print(sensorValue);
  lcd.setCursor(0, 0);
  lcd.print("Smoke : ");
      
  if (sensorValue > 300) {
    lcd.print("Yes");
    //tone(buzzer,1000,200);
    digitalWrite(ledRed,HIGH);
    digitalWrite(ledGreen,LOW);
  } else {
    lcd.print("No ");
    //noTone(buzzer);    
    digitalWrite(ledRed,LOW);
    digitalWrite(ledGreen,HIGH);
  }

  lcd.setCursor(0, 1);
  lcd.print("Value : ");
  lcd.print(sensorValue);
  
  delay(1000); // wait 1s for next reading
}
