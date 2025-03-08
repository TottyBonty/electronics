#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

//SIM800L declaration
SoftwareSerial mySerial(10,11);
String receiverNumber = "+63918xxxxxxx"; //-> change with the number where we send the SMS message

//LCD I2C declaration
const uint8_t lcdAddress = 0x3F;
const uint8_t lcd_cols = 16;
const uint8_t lcd_rows = 2;
LiquidCrystal_I2C lcd(lcdAddress, lcd_cols, lcd_rows);  //Defining 16 columns and 2 rows of lcd display

//Input Pin declaration
const int smokeA0 = A0;

//Output Pin declaration
const int buzzer = 9;
const int ledRed = 8;
const int ledGreen = 7;

//Sensor Smoke variables
const int smokeThreshold = 200;
float sensorValue;

int smokeDetected = 0;

const int sim800LEnabled = 1;

void setup() {
  pinMode(buzzer,OUTPUT);
  pinMode(ledGreen,OUTPUT);
  pinMode(ledRed,OUTPUT);
  
  pinMode(smokeA0,INPUT);

  Serial.begin(9600); // sets the serial port to 9600
  
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  lcdPrint("MQ2 Sensor", 0, 0);
  lcdPrint("warming up", 0, 1);  
  delay(20000); // allow the MQ-2 to warm up
  
  lcd.clear();
  lcdPrint("Testing Buzzer", 0, 0);
  tone(buzzer,1000,200);
  delay(2000);
  noTone(buzzer);

  setupSIM800L();
}

void loop() {
  sensorValue = analogRead(smokeA0);

  if (sensorValue > smokeThreshold) { // if threshold reach
    // Show the current Sensor Status in the LCD
    lcd.clear();
    lcdPrint("Value : ", 0, 0);
    lcd.print(sensorValue);
    lcdPrint("Smoke : Detected", 0, 1);
    
    tone(buzzer, 1000, 200); // sound on the buzzer
    
    digitalWrite(ledGreen,LOW); // turn off the green light
    
    digitalWrite(ledRed,HIGH); // blink the red light 
    delay(1000);
    digitalWrite(ledRed,LOW);
    
    // Send a SMS message once
    if (smokeDetected == 0) {
      sendMessage("WARNING Smoke Detected!!!");
      smokeDetected = 1;
    }
  } else {
    // Show the current Sensor Status in the LCD
    lcd.clear();
    lcdPrint("Value : ", 0, 0);
    lcd.print(sensorValue);
    lcdPrint("Smoke : None", 0, 1);

    noTone(buzzer); // sound off the buzzer
    digitalWrite(ledRed,LOW); // turn off the green light
    digitalWrite(ledGreen,HIGH); // turn on the green light
    
    // Send a SMS message once
    if (smokeDetected == 1) {
      sendMessage("No more Smoke Detected.");
      smokeDetected = 0;
    }
  }

  delay(1000); // wait 1s for next reading
}

void lcdPrint(const char message[], int x, int y) {
  lcd.setCursor(x, y);
  lcd.print(message);
}

void setupSIM800L() {
  lcd.clear();
  if (sim800LEnabled == 1) {
    mySerial.begin(9600);
  
    lcdPrint("SIM800L", 0, 0);
    lcdPrint("Initializing...", 0, 1);
    Serial.println("SIM800L Initializing..."); 
  
    delay(1000);
    mySerial.println("AT");
  } else {
    lcdPrint("SIM800L", 0, 0);
    lcdPrint("disabled", 0, 1);
    delay(1000);
  }

  lcd.clear();
}

void sendMessage(const char message[]){
  if (sim800LEnabled == 1) {
    mySerial.println("AT+CMGF=1");     //Sets the GSM Module in Text Mode
    Serial.println(readSerial());
    mySerial.println("AT+CMGS=\"" + receiverNumber + "\"");
    Serial.println(readSerial());
    mySerial.println(message);
    mySerial.println((char)26);
    Serial.println(readSerial());
  }
}

String readSerial(){
  delay(100);
  if (mySerial.available()) {
    return mySerial.readString();
  }
}
