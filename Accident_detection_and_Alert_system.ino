#include <Wire.h>
#include <MPU6050.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h> // GPS library

MPU6050 mpu;
SoftwareSerial gpsSerial(4, 3); // RX, TX for GPS
SoftwareSerial gsmSerial(7, 8); // RX, TX for GSM
TinyGPSPlus gps;

const int pulseSensorPin = A0;
const int soundSensorPin = A1;
const int vibrationSensorPin = A2;
int heartRate = 0;

void setup() {
  Serial.begin(9600);
  mpu.initialize();
  gpsSerial.begin(9600); // GPS module
  gsmSerial.begin(9600); // GSM module
  
  // Initialize sensors here
}

void loop() {
  // Read pulse sensor data
  heartRate = analogRead(pulseSensorPin);
  
  // Check for pulse abnormality
  if (heartRate < 60 || heartRate > 120) {
    alertDriver(); // Warn driver if abnormal
  }
  
  // Read MPU6050 sensor data
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);
  
  // Read sound and vibration sensor
  int soundLevel = analogRead(soundSensorPin);
  int vibrationLevel = analogRead(vibrationSensorPin);
  
  // Accident detection logic
  if (abs(ax) > 2000 && soundLevel > 500 && vibrationLevel > 500) {
    sendAccidentAlert(); // Accident detected
  }
  
  delay(1000); // Adjust delay based on real-time needs
}

void sendAccidentAlert() {
  Serial.println("Accident Detected!");
  // Get GPS location
  if (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }
  
  if (gps.location.isValid()) {
    String message = "Accident detected! Location: ";
    message += "Lat: "; message += gps.location.lat();
    message += " Lon: "; message += gps.location.lng();
    
    // Send SMS through GSM module
    gsmSerial.println("AT+CMGF=1"); // Set SMS mode
    delay(100);
    gsmSerial.println("AT+CMGS=\"+1234567890\""); // Emergency number
    delay(100);
    gsmSerial.println(message); // Accident message
    delay(100);
    gsmSerial.println((char)26); // End SMS with Ctrl+Z
    delay(1000);
  }
}

void alertDriver() {
  // Activate buzzer or display warning
  Serial.println("Abnormal heart rate detected. Please rest.");
}