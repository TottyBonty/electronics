#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

//SIM800L declaration
SoftwareSerial mySerial(10,11);
String number = "+639189798239"; //-> change with the number where we send the SMS message

//LCD I2C declaration
const uint8_t lcdAddress = 0x3F;
const uint8_t lcd_cols = 16;
const uint8_t lcd_rows = 2;
LiquidCrystal_I2C lcd(lcdAddress, lcd_cols, lcd_rows);  //Defining 16 columns and 2 rows of lcd display

//Input Pin declaration
const int smokeA0 = A0;

//Output Pin declaration
//int buzzer = 11;
const int ledGreen = 12;
const int ledRed = 8;

//Sensor Smoke variables
const int smokeThreshold = 200;
float sensorValue;

int smokeDetected = 0;

void setup() {
  //pinMode(buzzer,OUTPUT);
  //pinMode(ledGreen,OUTPUT);
  //pinMode(ledRed,OUTPUT);
  
  pinMode(smokeA0,INPUT);

  Serial.begin(9600); // sets the serial port to 9600
  
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  lcdPrint("MQ2 Sensor", 0, 0);
  lcdPrint("warming up", 0, 1);
  
  //noTone(buzzer);

  delay(20000); // allow the MQ-2 to warm up
  
  setupSIM800L();
}

void lcdPrint(String message, int x, int y) {
  lcd.setCursor(x, y);
  lcd.print(message);  
}

void setupSIM800L() {
  mySerial.begin(9600);
  
  lcd.clear();
  lcdPrint("SIM800L Initializing...", 0, 0);
  Serial.println("SIM800L Initializing..."); 
  
  delay(1000);
  mySerial.println("AT");
}

void loop() {
  sensorValue = analogRead(smokeA0);
 
  lcdPrint("Smoke : ", 0, 0);
      
  if (sensorValue > smokeThreshold) {
    lcd.print("Detected");
    
    if (smokeDetected == 0) {
      SendMessage("WARNING Smoke Detected!!!");
      smokeDetected = 1;
    }
    
    //tone(buzzer,1000,200);
    //digitalWrite(ledRed,HIGH);
    //digitalWrite(ledGreen,LOW);
  } else {
    lcd.print("None    ");
    
    if (smokeDetected == 1) {
      SendMessage("No more Smoke Detected.");
      smokeDetected = 0;
    }

    // noTone(buzzer);    
    // digitalWrite(ledRed,LOW);
    // digitalWrite(ledGreen,HIGH);
  }

  lcdPrint("Value : ", 0, 1);
  lcd.print(sensorValue);
  
  delay(1000); // wait 1s for next reading
  
  // if (mySerial.available()) {
  //   delay(1000);
  //   Serial.println(mySerial.readString());
  // }
}

void SendMessage(String message){
  mySerial.println("AT+CMGF=1");     //Sets the GSM Module in Text Mode
  Serial.println(readSerial());
  mySerial.println("AT+CMGS=\"" + number + "\"");
  Serial.println(readSerial());
  mySerial.println(message);
  mySerial.println((char)26);
  Serial.println(readSerial());
}

String readSerial(){
  delay(100);
  if (mySerial.available()) {
    return mySerial.readString();
  }
}
