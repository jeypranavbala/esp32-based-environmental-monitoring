i have this though saved in whatsapp
#include <MQ135.h>
#include <DHT.h>
#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"
#include <SPI.h>
#include <LoRa.h>

// Pin configuration for sensors
#define MQ135_PIN 34      // Analog pin for MQ135
#define DHT_PIN 4         // Digital pin for DHT11
#define DHT_TYPE DHT11    // DHT11 sensor type
#define MAX_BRIGHTNESS 255

// Pin configuration for LoRa module
#define ss 5    // LoRa SS pin
#define rst 14  // LoRa reset pin
#define dio0 2  // LoRa DIO0 pin

// Initialize sensors
MQ135 gasSensor = MQ135(MQ135_PIN);
DHT dht(DHT_PIN, DHT_TYPE);
MAX30105 particleSensor;

// Buffer configuration for MAX30105
uint32_t irBuffer[100]; // Infrared LED sensor data
uint32_t redBuffer[100];  // Red LED sensor data

int32_t bufferLength; // Data length
int32_t spo2; // SpO2 value
int8_t validSPO2; // Indicator for valid SpO2
int32_t heartRate; // Heart rate value
int8_t validHeartRate; // Indicator for valid heart rate

float calibrationOffset = 0; // Variable for calibration offset

void setup() {
  Serial.begin(115200); // Start Serial communication
  while (!Serial);

  dht.begin();          // Initialize DHT11 sensor

  // Initialize LoRa module with pin definitions
  LoRa.setPins(ss, rst, dio0);
  while (!LoRa.begin(433E6)) { // Initialize LoRa at 433 MHz
    Serial.println("Initializing LoRa...");
    delay(500);
  }
  LoRa.setSyncWord(0xA5); // Sync word for communication
  Serial.println("LoRa Initializing OK!");

  // Initialize MAX30105 sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println(F("MAX30105 was not found. Please check wiring/power."));
    while (1);
  }

  // Sensor setup for MAX30105
  byte ledBrightness = 60; // LED brightness
  byte sampleAverage = 4;  // Options for averaging
  byte ledMode = 2;        // Red + IR mode
  byte sampleRate = 100;   // Sample rate
  int pulseWidth = 411;    // Pulse width
  int adcRange = 4096;     // ADC range

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); // Configure MAX30105

  Serial.println("MQ135, DHT11, and MAX30105 Sensor Integration with LoRa");
  
  // Calibration step for MQ135
  calibrateSensor();
}

void calibrateSensor() {
  Serial.println("Calibrating sensor...");
  float total = 0;
  int samples = 100; // Number of samples for averaging

  for (int i = 0; i < samples; i++) {
    total += gasSensor.getPPM(); // Read the current PPM value
    delay(100); // Wait for a short period before the next reading
  }

  calibrationOffset = total / samples; // Calculate average reading
  Serial.print("Calibration complete. Offset: ");
  Serial.println(calibrationOffset);
}

void loop() {
  // Read air quality from MQ135 sensor
  float air_quality = gasSensor.getPPM() - calibrationOffset;

  // Indirect oxygen concentration in air (approximation)
  float oxygen_concentration = 20.95 - (air_quality / 1000); // Assume pollutants displace oxygen slightly

  // Read temperature and humidity from DHT11 sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check for DHT11 sensor read errors
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT11 sensor!");
    return; // Exit if DHT11 read fails
  }

  // Read samples from MAX30105
  bufferLength = 100; // Buffer length of 100 samples
  for (byte i = 0; i < bufferLength; i++) {
    while (!particleSensor.available()) particleSensor.check(); // Wait for new data
    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample(); // Move to next sample
  }

  // Calculate heart rate and SpO2 after reading samples
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

  // Display sensor readings in Serial Monitor
  Serial.println("---------- Sensor Readings ----------");
  Serial.print("Air Quality: ");
  Serial.print(air_quality);
  Serial.println(" PPM");

  Serial.print("Oxygen Concentration: ");
  Serial.print(oxygen_concentration);
  Serial.println(" %");

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("SpO2: ");
  Serial.print(spo2);
  Serial.println(" %");

  // Send the sensor values over LoRa
  LoRa.beginPacket();
  LoRa.print("AirQuality:");
  LoRa.print(air_quality);
  LoRa.print(",Temp:");
  LoRa.print(temperature);
  LoRa.print(",Humidity:");
  LoRa.print(humidity);
  LoRa.print(",SpO2:");
  LoRa.print(spo2);
  LoRa.print(",Oxygen Concentration:");
  LoRa.print(oxygen_concentration);
  LoRa.endPacket();
  
  Serial.println("Data Sent via LoRa");

  Serial.println("-------------------------------------");
  
  delay(1000); // Delay for 2 seconds before the next reading
}
