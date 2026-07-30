#include <Wire.h>
#include <TinyGPSPlus.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "MAX30105.h"
#include "heartRate.h"

#define SDA_PIN       8
#define SCL_PIN       9
#define GPS_RX_PIN    16
#define DS18B20_PIN   4

HardwareSerial gpsSerial(1);
TinyGPSPlus gps;

Adafruit_MPU6050 mpu;
bool mpuOK = false;

OneWire oneWire(DS18B20_PIN);
DallasTemperature ds18b20(&oneWire);
float tempC = DEVICE_DISCONNECTED_C;

MAX30105 particleSensor;
bool heartOK = false;

long lastBeat = 0;
float beatsPerMinute = 0;
long irValue = 0;

unsigned long lastPrint = 0;
unsigned long lastTempRequest = 0;
unsigned long lastTempRead = 0;
bool tempRequested = false;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=== ESP32-S3 FULL SYSTEM FAST HEART TEST ===");

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(400000);

  gpsSerial.begin(9600, SERIAL_8N1, GPS_RX_PIN, -1);

  if (mpu.begin(0x68, &Wire)) {
    mpuOK = true;
    Serial.println("MPU6050 OK");
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  } else {
    Serial.println("MPU6050 NOT FOUND");
  }

  ds18b20.begin();
  ds18b20.setWaitForConversion(false); // สำคัญ: ไม่ให้ DS18B20 block loop
  ds18b20.requestTemperatures();
  lastTempRequest = millis();
  tempRequested = true;

  if (particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    heartOK = true;
    Serial.println("MAX30102 OK");

    particleSensor.setup(0x1F, 4, 2, 100, 411, 4096);
    particleSensor.setPulseAmplitudeRed(0x1F);
    particleSensor.setPulseAmplitudeIR(0x1F);
    particleSensor.setPulseAmplitudeGreen(0);
  } else {
    Serial.println("MAX30102 NOT FOUND");
  }

  Serial.println("Setup done");
}

void loop() {
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  // อ่าน Heart ให้ถี่ที่สุด
  if (heartOK) {
    irValue = particleSensor.getIR();

    if (checkForBeat(irValue)) {
      long delta = millis() - lastBeat;
      lastBeat = millis();

      float currentBPM = 60.0 / (delta / 1000.0);

      if (currentBPM >= 40 && currentBPM <= 180) {
        beatsPerMinute = currentBPM;
      }
    }
  }

  // DS18B20 non-blocking
  if (tempRequested && millis() - lastTempRequest >= 800) {
    tempC = ds18b20.getTempCByIndex(0);
    tempRequested = false;
    lastTempRead = millis();
  }

  if (!tempRequested && millis() - lastTempRead >= 1000) {
    ds18b20.requestTemperatures();
    lastTempRequest = millis();
    tempRequested = true;
  }

  if (millis() - lastPrint >= 1000) {
    lastPrint = millis();

    Serial.println();
    Serial.println("========== DATA ==========");

    Serial.print("Temperature: ");
    if (tempC == DEVICE_DISCONNECTED_C) Serial.println("Not Found");
    else {
      Serial.print(tempC);
      Serial.println(" C");
    }

    if (mpuOK) {
      sensors_event_t acc, gyro, temp;
      mpu.getEvent(&acc, &gyro, &temp);

      Serial.print("ACC: ");
      Serial.print(acc.acceleration.x);
      Serial.print(", ");
      Serial.print(acc.acceleration.y);
      Serial.print(", ");
      Serial.println(acc.acceleration.z);

      Serial.print("GYRO: ");
      Serial.print(gyro.gyro.x);
      Serial.print(", ");
      Serial.print(gyro.gyro.y);
      Serial.print(", ");
      Serial.println(gyro.gyro.z);
    }

    if (heartOK) {
      Serial.print("MAX30102 IR: ");
      Serial.println(irValue);

      Serial.print("Heart BPM: ");
      if (irValue < 50000) Serial.println("No finger");
      else if (beatsPerMinute > 0) Serial.println((int)beatsPerMinute);
      else Serial.println("Detecting...");
    }

    if (gps.location.isValid()) {
      Serial.print("GPS: ");
      Serial.print(gps.location.lat(), 6);
      Serial.print(", ");
      Serial.println(gps.location.lng(), 6);
    } else {
      Serial.println("GPS: Waiting...");
    }

    Serial.print("Satellites: ");
    Serial.println(gps.satellites.value());

    Serial.println("==========================");
  }

  delay(5);
}