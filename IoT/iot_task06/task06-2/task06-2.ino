#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Define pins for SPI communication
#define BME_CS 5 // Chip Select (CS) pin for ESP32 VSPI
#define SEALEVELPRESSURE_HPA (1013.25)

// Initialize BME280 sensor with hardware SPI
Adafruit_BME280 bme(BME_CS); // For hardware SPI communication
int delayTime;

void setup() {
    Serial.begin(115200);
    Serial.println(F("BME280 test"));

    bool status;

    // Begin communication with the sensor
    status = bme.begin();
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x");
        Serial.println(bme.sensorID(), 16);
        Serial.println(" ID of 0xFF probably means a bad address, a BMP 180 or BMP 085");
        Serial.println(" ID of 0x56-0x58 represents a BMP 280,");
        Serial.println(" ID of 0x60 represents a BME 280.");
        Serial.println(" ID of 0x61 represents a BME 680.");
        while (1) delay(10); // Infinite loop in case of failure
    }

    Serial.println("-- Default Test --");
    delayTime = 1000; // Set delay time to 1000 ms
    Serial.println();
}

void loop() {
    printValues();
    delay(delayTime);
}

void printValues() {
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");

    Serial.print("Pressure = ");
    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.println();
}