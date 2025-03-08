#define         MQ_PIN                       (0)    //define which  analog input channel you are going to use
#define         RL_VALUE                     (5)     //define the load resistance on the board, in kilo ohms
#define         RO_CLEAN_AIR_FACTOR          (9.83)  //RO_CLEAR_AIR_FACTOR=(Sensor resistance in clean air)/RO,
                                                     //which is derived from the  chart in datasheet
 
/**********************Software Related Macros***********************************/
#define         CALIBARAION_SAMPLE_TIMES     (50)    //define how many samples you are  going to take in the calibration phase
#define         CALIBRATION_SAMPLE_INTERVAL  (500)   //define the time interal(in milisecond) between each samples in the
                                                     //cablibration phase
#define         READ_SAMPLE_INTERVAL         (50)    //define how many samples you are  going to take in normal operation
#define         READ_SAMPLE_TIMES            (5)     //define the time interal(in milisecond) between each samples in 

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
 
/*********************Application Related Macros*********************************/
#define         GAS_SMOKE                    (2)
 
/****************************Globals**********************************************/
float           SmokeCurve[3] ={2.3,0.53,-0.44};    //two points are taken from the curve.  
                                                    //with these two points,  a line is formed which is "approximately equivalent" 
                                                    //to  the original curve.
                                                    //data  format:{ x, y, slope}; point1: (lg200, 0.53), point2: (lg10000,  -0.22)                                                     
float           Ro           =  10;                 //Ro is initialized to 10 kilo ohms

//SIM800L declaration
SoftwareSerial mySerial(10,11);
String receiverNumber = "+639189798239"; //-> change with the number where we send the SMS message

//LCD I2C declaration
const uint8_t lcdAddress = 0x3F;
const uint8_t lcd_cols = 16;
const uint8_t lcd_rows = 2;
LiquidCrystal_I2C lcd(lcdAddress, lcd_cols, lcd_rows);  //Defining 16 columns and 2 rows of lcd display

//Output Pin declaration
const int buzzer = 9;
const int ledRed = 8;
const int ledGreen = 7;

//Sensor Smoke variables
const int smokeThreshold = 100; //adjust this as needed, but not below 50
float sensorValue;

int smokeDetected = 0;

const int sim800LEnabled = 1;

void setup() {
  pinMode(buzzer, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledRed, OUTPUT);
  
  pinMode(MQ_PIN, INPUT);

  Serial.begin(9600); // sets the serial port to 9600
  
  lcd.init();
  lcd.backlight();
  
  setupMQ();

  setupBuzzzer();

  setupSIM800L();
}

void loop() {
  sensorValue = MQGetPercentage(MQRead(MQ_PIN)/Ro,SmokeCurve);  

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

  delay(500); // wait 1s for next reading
}

void lcdPrint(const char message[], int x, int y) {
  lcd.setCursor(x, y);
  lcd.print(message);
}

void setupMQ() {
  lcd.clear();

  lcdPrint("MQ2 Sensor", 0, 0);
  lcdPrint("Calibrating...", 0, 1);

  Ro = MQCalibration(MQ_PIN); //Calibrating the sensor. Please  make sure the sensor is in clean air 

  lcd.clear();

  lcdPrint("Calibration done", 0, 0);
  lcdPrint("Ro=", 0, 1);
  lcd.print(Ro);
  lcd.print("kohm");

  delay(2000);
}

void setupBuzzzer() {
  lcd.clear();
  
  lcdPrint("Testing Buzzer", 0, 0);

  tone(buzzer,1000,200);
  delay(2000);
  noTone(buzzer);
}

void setupSIM800L() {
  lcd.clear();

  if (sim800LEnabled == 1) {
    mySerial.begin(9600);
  
    lcdPrint("SIM800L", 0, 0);
    lcdPrint("Initializing...", 0, 1);
  
    delay(2000);
    mySerial.println("AT");
  } else {
    lcdPrint("SIM800L", 0, 0);
    lcdPrint("disabled", 0, 1);
    delay(2000);
  }
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

/****************  MQResistanceCalculation **************************************
Input:   raw_adc  - raw value read from adc, which represents the voltage
Output:  the calculated  sensor resistance
Remarks: The sensor and the load resistor forms a voltage divider.  Given the voltage
         across the load resistor and its resistance, the resistance  of the sensor
         could be derived.
**********************************************************************************/  
float MQResistanceCalculation(int raw_adc)
{
  return ( ((float)RL_VALUE*(1023-raw_adc)/raw_adc));
}
  
/*************************** MQCalibration **************************************
Input:   mq_pin - analog channel
Output:  Ro of the sensor
Remarks: This function  assumes that the sensor is in clean air. It use  
         MQResistanceCalculation  to calculates the sensor resistance in clean air 
         and then divides it  with RO_CLEAN_AIR_FACTOR. RO_CLEAN_AIR_FACTOR is about 
         10, which differs  slightly between different sensors.
**********************************************************************************/  
float MQCalibration(int mq_pin) {
  int i;
  float val=0;
 
  for (i=0;i<CALIBARAION_SAMPLE_TIMES;i++) {            //take multiple samples
    val += MQResistanceCalculation(analogRead(mq_pin));
    delay(CALIBRATION_SAMPLE_INTERVAL);
  }
  val = val/CALIBARAION_SAMPLE_TIMES;                   //calculate the average  value
 
  val = val/RO_CLEAN_AIR_FACTOR;                        //divided  by RO_CLEAN_AIR_FACTOR yields the Ro 
                                                        //according  to the chart in the datasheet 
 
  return val; 
}
/***************************  MQRead *******************************************
Input:   mq_pin - analog  channel
Output:  Rs of the sensor
Remarks: This function use MQResistanceCalculation  to caculate the sensor resistenc (Rs).
         The Rs changes as the sensor  is in the different consentration of the target
         gas. The sample times  and the time interval between samples could be configured
         by changing  the definition of the macros.
**********************************************************************************/  
float MQRead(int mq_pin) {
  int i;
  float rs=0;
 
  for (i=0;i<READ_SAMPLE_TIMES;i++)  {
    rs += MQResistanceCalculation(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL);
  }
 
  rs = rs/READ_SAMPLE_TIMES;
 
  return rs;  
}
 
/***************************  MQGetPercentage  ********************************
Input:   rs_ro_ratio - Rs divided by Ro
         pcurve      - pointer to the curve of the target gas
Output:  ppm of  the target gas
Remarks: By using the slope and a point of the line. The x(logarithmic  value of ppm) 
         of the line could be derived if y(rs_ro_ratio) is provided.  As it is a 
         logarithmic coordinate, power of 10 is used to convert the  result to non-logarithmic 
         value.
**********************************************************************************/  
int  MQGetPercentage(float rs_ro_ratio, float *pcurve)
{
  return (pow(10,(  ((log(rs_ro_ratio)-pcurve[1])/pcurve[2]) + pcurve[0])));
}
