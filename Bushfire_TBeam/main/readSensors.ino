
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define DEF_PIN_NUM_CO 1
#define DEF_PIN_NUM_NO2 2
#define DEF_PIN_NUM_BME 3


Adafruit_BME280 bme; // I2C

// these constants won't change.  They're used to give names to the pins used:


const int resValue_0 = 9950;  // Value of 10kOhm resistor !change this to match your resistor
const int resValue_1 = 9900;  // Value of 10kOhm resistor !change this to match your resistor

const float Vref = 1.1;  //This is the voltage of the internal reference
long int cOff = 279; //286mV offset due to resistor ladder. Try taking the average of a long

//measurement of the counts without a sensor in place. This should give a good Zero.

 // sensitivity in nA/ppm,
const float sensitivityCO = 2.145;  // CO 4.29
const float sensitivityNO2 = 13.51; // NO2 -27.03

//this is roughly about 1/2 the sensitivity of the barcode on the sensor. Try finding a known
//concentration of CO to measure, or just approximate.

const int extraBit = 256; //Number of samples averaged, like adding 8 bits to ADC
const int calibNum = 1024; //Number of samples averaged, like adding 8 bits to ADC

long int sensorValueCO = 0;        // value read from the sensor
long int sensorValueNO2 = 0;        // value read from the sensor

void readSensors() {
  bme.begin(DEF_PIN_NUM_BME);
  //analogReference(EXTERNAL);

  // Init. sensor data
    sensorValueCO = 0;
    sensorValueNO2 = 0;

  // Read gas sensor input
  for (int i = 0; i < extraBit; i++) {
      sensorValueCO = analogRead(DEF_PIN_NUM_CO) + sensorValueCO;
      sensorValueNO2= analogRead(DEF_PIN_NUM_NO2) + sensorValueNO2;
      delay(3);   // needs 2 ms for the analog-to-digital converter to settle after the last reading
  }

  // Accumulate sensor values
    sensorValueCO = sensorValueCO  - cOff * extraBit; //subtract the offset of the resistor ladder * 256.
    sensorValueNO2 = sensorValueNO2 - cOff * extraBit; //subtract the offset of the resistor ladder * 256.

  //  Get sensor values
  float valCO  = ((float) sensorValueCO   / extraBit / 1024 * Vref / resValue_0 * 1000000000) / sensitivityCO;
  float valNO2 = ((float) sensorValueNO2 / extraBit / 1024 * Vref / resValue_1 * 1000000000) / sensitivityNO2;
  float valTemp = (float)bme.readTemperature();
  float valHumd = bme.readHumidity();

 // Print sensor values
  Serial.print(valCO); Serial.print(" ");
  Serial.print(valNO2); Serial.print(" ");
  Serial.print(valTemp); Serial.print(" ");
  Serial.println(valHumd);
  delay(170);
}
