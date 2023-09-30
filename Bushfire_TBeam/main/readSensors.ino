
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define DEF_PIN_NUM_CO 1
#define DEF_PIN_NUM_NO2 2


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


void readSensors(uint8_t txBuffer[30]) {
  char t[32]; // used to sprintf for Serial output
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
  float valTemp = bme.readTemperature();
  float valHumd = bme.readHumidity();
  float valPres = bme.readPressure() / 100.0F;

 // Print sensor values
  valCO = 1234;
  valNO2 = 5678;

  delay(170);

  uint32_t val1 = valCO;
  uint32_t val2 = valNO2;
  uint32_t val3 = (uint32_t)valTemp;
  uint32_t val4 = (uint32_t)valHumd;
  uint32_t val5 = (uint32_t)valPres;

 // Debugging
  Serial.println("");
  Serial.println("---------- Sensor values ----------");

  Serial.print("CO  = ");
  Serial.println(val1);

  Serial.print("NO2 = ");
  Serial.println(val2);

  Serial.print("Temperature = ");
  Serial.print(val3);
  Serial.println(" °C");

  Serial.print("Humidity    = ");
  Serial.print(val4);
  Serial.println(" %");

  Serial.print("Pressure    = ");
  Serial.print(val5);
  Serial.println(" hPa");
  Serial.println("-----------------------------------");

  // CO
  txBuffer[9] = ( val1 >> 16 ) & 0xFF;
  txBuffer[10] = ( val1 >> 8 ) & 0xFF;
  txBuffer[11] = val1 & 0xFF;
  //NO2
  txBuffer[12] = ( val2 >> 16 ) & 0xFF;
  txBuffer[13] = ( val2>> 8 ) & 0xFF;
  txBuffer[14] = val2 & 0xFF;
  // Temp
  txBuffer[15] = ( val3 >> 16 ) & 0xFF;
  txBuffer[16] = ( val3 >> 8 ) & 0xFF;
  txBuffer[17] = val3 & 0xFF;
  // Humidity
  txBuffer[18] = ( val4 >> 16 ) & 0xFF;
  txBuffer[19] = ( val4 >> 8 ) & 0xFF;
  txBuffer[20] = val4 & 0xFF;
  // Pressure
  txBuffer[21] = ( val5 >> 16 ) & 0xFF;
  txBuffer[22] = ( val5 >> 8 ) & 0xFF;
  txBuffer[23] = val5 & 0xFF;
}
