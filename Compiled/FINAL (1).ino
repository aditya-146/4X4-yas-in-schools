#include <DHT.h>
#include <Wire.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);
bool carLocked = true; 
const String correctKeyID = "A3 9C EF 1D"; 
String command;
int currentAngle = 90; // Initial angle of the servo motor
const int trigPin = 4;
const int echoPin = 7;
long duration, distance_cm;
float temperature;
const int waterSensorPin = A1;
const int waterLevelThreshold = 500;
int waterLevel;

int head_light1 = A2;
int head_light2 = A3 ;
int light_sensor = A0;
int light_sensor_reading;
int buzzer = 6;
int tempThreshold = 70;  // Temperature threshold in degrees Celsius

Servo servoMotor;
int servoPin = 3;
SoftwareSerial bluetooth(2, 8);

int servoAngle = 90;
int deadband = 5;

const int MPU_addr = 0x68;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

int minVal = 265;
int maxVal = 402;

double x;
double y;
double z;
char c;
DHT dht(5, DHT11); // DHT11 sensor connected to pin 5

void setup() {
  Serial.begin(9600);
  pinMode(head_light1, OUTPUT);
  pinMode(head_light2, OUTPUT);
  pinMode(light_sensor, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(waterSensorPin, INPUT);
  
  servoMotor.attach(servoPin);
  bluetooth.begin(9600);

  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  dht.begin();

  SPI.begin();
  rfid.PCD_Init();

  analogWrite(head_light1, 0);
  analogWrite(head_light2, 0);
  digitalWrite(buzzer, LOW);
}

void loop() {
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String keyID = getCardID();
    Serial.println(keyID);

    if (keyID == correctKeyID) {
      // Unlock the car and turn on the headlights
      carLocked = false;
      turnOnHeadlights();
      delay(200);  // Headlights on for 2 seconds
      turnOffHeadlights();
      delay(200); 
      turnOnHeadlights();
      delay(200);  // Headlights on for 2 seconds
      turnOffHeadlights();
    } else {
      // Wrong key detected
      activateAlarm();
      
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }

  if (bluetooth.available()) {
    while (bluetooth.available()) {
      char c = bluetooth.read();
      command += c;
      delay(10); // Delay for smooth reception of command
    }
    command.trim(); // Remove leading/trailing spaces if any
    Serial.println(command);

    if (command == "D") {
      // Move the servo motor slowly up
      for (int angle = currentAngle; angle < 180; angle++) {
        servoMotor.write(angle);
        delay(10);  // Delay for smoother movement
      }
      currentAngle = 180;  // Update the current angle
    } else if (command == "U") {
      // Move the servo motor slowly down
      for (int angle = currentAngle; angle > 0; angle--) {
        servoMotor.write(angle);
        delay(10);  // Delay for smoother movement
      }
      currentAngle = 0;  // Update the current angle
    } else if (command == "S") {
      // Stop the servo motor
      servoMotor.write(currentAngle);  // Hold the current position
    } else {
      // Set the servo motor angle based on the received number
      int angle = command.toInt();
      if (angle >= 0 && angle <= 180) {
        servoMotor.write(angle);
        currentAngle = angle;
      }
    }
    command = ""; // Reset the command string for the next input
  }
  
  
  Serial.println("-------------------------------------------------------------------------------------------------------------");
  Serial.println("|  LDR Reading  |  Tilt Sensor  |  Servo Angle  |  Water Level  |  Distance  |  Temperature  |  Bluetooth  | ");
  Serial.println("--------------------------------------------------------------------------------------------------------------");

  light_sensor_reading = analogRead(light_sensor);
  analogWrite(head_light1, 0);
  analogWrite(head_light2, 0);
  water_level_detector();
  object_detector();
  temperature_monitor();
  automatic_headlight();
  tilt_feature();

  Serial.print("|      ");
  Serial.print(light_sensor_reading);
  Serial.print("      |      ");
  Serial.print(y);
  Serial.print("       |      ");
  Serial.print(servoAngle);
  Serial.print("       |      ");
  Serial.print(waterLevel);
  Serial.print("       |      ");
  Serial.print(distance_cm);
  Serial.print("       |      ");
  Serial.print(temperature);
  Serial.print("       |      ");
  Serial.print(command);
  Serial.print("       |");
  Serial.println();
  Serial.println("------------------------------------------------");

  delay(500);
}


String getCardID() {
  String cardID = "";

  for (byte i = 0; i < rfid.uid.size; i++) {
    cardID += String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    cardID += String(rfid.uid.uidByte[i], HEX);
  }

  cardID.toUpperCase();
  return cardID.substring(1);
}

void turnOnHeadlights() {
  analogWrite(head_light1, 255);  // Turn on LED 1 at full brightness
  analogWrite(head_light2, 255);  // Turn on LED 2 at full brightness
}

void turnOffHeadlights() {
  analogWrite(head_light1, 0);  // Turn off LED 1
  analogWrite(head_light2, 0);  // Turn off LED 2
}

void activateAlarm() {
  unsigned long alarmStartTime = millis();
  unsigned long headlightsStartTime = alarmStartTime + 2000;  // Start the headlights 2 seconds after the alarm
  
  // Sound the alarm (activate the buzzer)
  tone(buzzer, 1000);  // Set the buzzer frequency to 1000Hz
  
  while (millis() - alarmStartTime < 2000) {
    turnOnHeadlights();
    delay(200);
    turnOffHeadlights();
    delay(200);
    turnOnHeadlights();
    delay(200);
    turnOffHeadlights();
    delay(200);
    turnOnHeadlights();
    delay(200);
    turnOffHeadlights();
    delay(200);
    turnOnHeadlights();
    delay(200);
    turnOffHeadlights();
    delay(200);
  }
  
  noTone(buzzer);  // Turn off the buzzer
  
}

void automatic_headlight() {
  if (light_sensor_reading < 35) {
    analogWrite(head_light1, 255);
    analogWrite(head_light2, 255);
  } else {
    analogWrite(head_light1, 0);
    analogWrite(head_light2, 0);
  }
}

void tilt_feature() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true);
  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  int xAng = map(AcX, minVal, maxVal, -90, 90);
  int yAng = map(AcY, minVal, maxVal, -90, 90);
  int zAng = map(AcZ, minVal, maxVal, -90, 90);

  y = RAD_TO_DEG * (atan2(-xAng, -zAng) + PI);

  if (y > 271) {
    tone(buzzer, 1000, 1000);  // Set the buzzer frequency to 1000Hz for 1000ms
  } else {
    noTone(buzzer);             // Stop the buzzer tone
  }
}

//void tow_system() {
//  if (command == "U") {
//      // Move the servo motor slowly up
//      for (int angle = currentAngle; angle < 180; angle++) {
//        servoMotor.write(angle);
//        delay(10);  // Delay for smoother movement
//      }
//      currentAngle = 180;  // Update the current angle
//    } else if (command == "D") {
//      // Move the servo motor slowly down
//      for (int angle = currentAngle; angle > 0; angle--) {
//        servoMotor.write(angle);
//        delay(10);  // Delay for smoother movement
//      }
//      currentAngle = 0;  // Update the current angle
//    } else if (command == "S") {
//      // Stop the servo motor
//      servoMotor.write(currentAngle);  // Hold the current position
//    } else {
//      // Set the servo motor angle based on the received number
//      int angle = command.toInt();
//      if (angle >= 0 && angle <= 180) {
//        servoMotor.write(angle);
//        currentAngle = angle;
//      }
//    }
//    command = "";
//}

void object_detector() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance_cm = duration * 0.034 / 2;

  if (distance_cm >= 100 && distance_cm <= 500) {
    digitalWrite(buzzer, HIGH);    // Turn on the buzzer
    delay(100);                    // Keep the buzzer on for 100ms
    digitalWrite(buzzer, LOW);     // Turn off the buzzer
    delay(900);                    // Delay for 900ms
  }
}

void water_level_detector() {
  waterLevel = analogRead(waterSensorPin);

  if (waterLevel > waterLevelThreshold) {
    digitalWrite(buzzer, HIGH);    // Turn on the buzzer
  } else {
    digitalWrite(buzzer, LOW);     // Turn off the buzzer
  }
}


void temperature_monitor() {
  temperature = dht.readTemperature();

  if (temperature > tempThreshold) {
    digitalWrite(buzzer, HIGH);    // Turn on the buzzer
    delay(100);                    // Keep the buzzer on for 100ms
    digitalWrite(buzzer, LOW);     // Turn off the buzzer
    delay(900);                    // Delay for 900ms
  }
}
