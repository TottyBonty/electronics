//How to use SIM800L  with arduion
//PS works 

// COnnect the RX TX to Pin 10,11 on Arduion

#include <SoftwareSerial.h>

SoftwareSerial mySerial(10,11);

int timeout;
String number = "+639189798239"; //-> change with your number

void setup(){
  Serial.begin(9600); 
  mySerial.begin(9600);
  Serial.println("Type\n s) to send an SMS\n r) to receive an SMS\n c) to make a call");
  Serial.println("System Initializing..."); 
  delay(1000);
  mySerial.println("AT");
}

void loop(){
  if (Serial.available() > 0){
    switch (Serial.read()){
      case 's':
        SendMessage();
        break;
      case 'r':
        RecieveMessage();
        break;
      case 'c':
        CallNumber();
        break;
    }
 }
  if (mySerial.available()) {
    delay(1000);
    Serial.println(mySerial.readString());
  }
}

String readSerial(){
  delay(100);
  if (mySerial.available()) {
    return mySerial.readString();
  }
}

void CallNumber() {
  //mySerial.println("ATD+ +94712729677;");   //Mobile phone number to call
  mySerial.println("ATD+ " + number + ";");   //Mobile phone number to call
  Serial.println(readSerial());
  delay(20000); 
  mySerial.println("ATH"); 
  delay(200);
  Serial.println(readSerial());
}

void SendMessage(){
  mySerial.println("AT+CMGF=1");     //Sets the GSM Module in Text Mode
  Serial.println(readSerial());
  //mySerial.println("AT+CMGS=\"+94712729677\"");    //Mobile phone number to send message
  mySerial.println("AT+CMGS=\"" + number + "\""); //Mobile phone number to send message
  Serial.println(readSerial());
  mySerial.println("Hi, This Test Msg From SIM800");
  mySerial.println((char)26);
  Serial.println(readSerial());
}

void RecieveMessage(){
  Serial.println ("SIM800  Read an SMS");
  mySerial.println("AT+CMGF=1");    // AT Command to receive a live SMS
  Serial.println(readSerial());
  mySerial.println("AT+CNMI=1,2,0,0,0"); 
  Serial.println(readSerial());
}
