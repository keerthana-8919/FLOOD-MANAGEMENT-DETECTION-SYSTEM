#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// LCD (I2C type)
LiquidCrystal_I2C lcd(0x27, 16, 2);   // If nothing displays, try 0x3F

// GSM module connection
SoftwareSerial gsm(2, 3);  // RX, TX (connect GSM TX -> 7, GSM RX -> 8)

// Ultrasonic sensor pins
#define trigPin 11
#define echoPin 10
#define Float_Switch  12

// LED and Buzzer pins
#define ledPin 6
#define buzzerPin 5

// Phone number to alert
String phoneNumber "xxxxxxxxxx";   // ← Replace with your number

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(Float_Switch,INPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, LOW);

  Serial.begin(9600);
  gsm.begin(9600);
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Welcome to our");
  lcd.setCursor(0, 1);
  lcd.print("project");
  delay(2000);
  lcd.clear();

  // GSM initialization
  gsm.println("AT");
  delay(1000);
  gsm.println("AT+CMGF=1"); // Set SMS text mode
  delay(1000);
}

void loop() {
  long duration;
  int distance;
  int floatstate=digitalRead(Float_Switch);

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;  // distance in cm

  lcd.setCursor(0, 0);
  lcd.print("Water Level:");
  lcd.print(distance);
  lcd.print("cm ");

  if (distance <5) {
    lcd.setCursor(0, 1);
    lcd.print(" FLOOD  ALERT!!");
    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    sendAlert();
    delay(1000); // wait 10s to avoid repeated SMS
  } else {
    lcd.setCursor(0, 1);
    lcd.print("SAFE LEVEL ");
    digitalWrite(ledPin, LOW);
    digitalWrite(buzzerPin, LOW);
  }

  delay(1000);
}

void sendAlert() {
  // Send SMS
  gsm.println("AT+CMGF=1");    
  delay(1000);
  gsm.print("AT+CMGS=\"");
  gsm.print(phoneNumber);
  gsm.println("\"");
  delay(1000);
  gsm.println("ALERT!  Flood detected! go to safe palce stay safely");
  delay(1000);
  gsm.write(26); // Ctrl+Z to send SMS
  delay(5000);

  // Make a call
  gsm.print("ATD");
  gsm.print(phoneNumber);
  gsm.println(";");
  delay(10000); // Ring for 10s
  gsm.println("ATH"); // Hang up
}