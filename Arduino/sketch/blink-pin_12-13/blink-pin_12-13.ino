// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(12, OUTPUT); // green light
  pinMode(11, OUTPUT); // red light
  Serial.println("Setup done");
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(12, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);             // wait for a second
  digitalWrite(12, LOW);   // turn the LED off by making the voltage LOW in pin 12
  delay(1000);             // wait for a second
  digitalWrite(11, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);             // wait for a second
  digitalWrite(11, LOW);   // turn the LED off by making the voltage LOW in pin 13
  delay(1000);             // wait for a second
}