#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C
int delayTime;

void setup() {
    bool status;
    
    Serial.begin(115200);
    Serial.println(F("BME280 test"));

    // Default settings
    status = bme.begin(0x76); // BME280 I2C address = 0x76
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x");
        Serial.println(bme.sensorID(), 16);
        Serial.println(" ID of 0xFF probably means a bad address, a BMP 180 or BMP 085");
        Serial.println(" ID of 0x56-0x58 represents a BMP 280,");
        Serial.println(" ID of 0x60 represents a BME 280.");
        Serial.println(" ID of 0x61 represents a BME 680.");
        while (1) delay(10);
    }

    Serial.println("-- Default Test --");
    delayTime = 1000;
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