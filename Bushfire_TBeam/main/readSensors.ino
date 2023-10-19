
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define DEF_PIN_NUM_CO 36
#define DEF_PIN_NUM_NO2 39

#define DEF_PIN_NUM_DUST 15

// FIXME take measure resistors
const int resValue_0 = 9640;  // Value of 10kOhm resistor !change this to match your resistor
const int resValue_1 = 9450;  // Value of 10kOhm resistor !change this to match your resistor

const float Vref = 1.788;  //This is the voltage of the internal reference
long int cOff_CO = 946; 
long int cOff_NO2 = 946;

//measurement of the counts without a sensor in place. This should give a good Zero.

 // sensitivity in nA/ppm,
const float sensitivityCO = 4.12;  // CO 4.12   
const float sensitivityNO2 = 27.03; // NO2 -27.03

//this is roughly about 1/2 the sensitivity of the barcode on the sensor. Try finding a known
//concentration of CO to measure, or just approximate.

const int extraBit = 512; //Number of samples averaged, like adding 8 bits to ADC

long int sensorValueCO = 0;        // value read from the sensor
long int sensorValueNO2 = 0;        // value read from the sensor

void readSensors(uint8_t txBuffer[30]) {
  char t[32]; // used to sprintf for Serial output

  // Init. sensor data
  sensorValueCO = 0;
  sensorValueNO2 = 0;

  // Read gas sensor input
  for (int i = 0; i < extraBit; i++) {
      sensorValueCO = analogRead(DEF_PIN_NUM_CO) + sensorValueCO;
      sensorValueNO2= analogRead(DEF_PIN_NUM_NO2) + sensorValueNO2;
      delay(3);   // needs 2 ms for the analog-to-digital converter to settle after the last reading
  }

  Serial.print("CO :");Serial.println(analogRead(DEF_PIN_NUM_CO));
  Serial.print("NO2:");Serial.println(analogRead(DEF_PIN_NUM_NO2));

  float calcVoltage = analogRead(DEF_PIN_NUM_DUST)*(5.0/1024);
  float dustDensity = 0.17*calcVoltage-0.1;
  if (dustDensity < 0){
    dustDensity = 0;
  }

  sensorValueCO  = sensorValueCO  - cOff_CO  * extraBit; //subtract the offset of the resistor ladder * 256.
  sensorValueNO2 = sensorValueNO2 - cOff_NO2 * extraBit; //subtract the offset of the resistor ladder * 256.
  
  // Serial.print("[Acc]CO :");Serial.println(sensorValueCO);
  // Serial.print("[Acc]NO2:");Serial.println(sensorValueNO2);
  
  //  Get sensor values
  // CO  : 10 ppm
  float valCO  = (((float)sensorValueCO  / (float)extraBit / 2096.0f * Vref / (float)resValue_0 * 1000000000.f) / sensitivityCO ) * 1000.f;
  if (valCO < 0) valCO = 0; 
  // NO2 : 0.1 ppm
  float valNO2 = (((float)sensorValueNO2 / (float)extraBit / 2096.0f * Vref / (float)resValue_1 * 1000000000.f) / sensitivityNO2) * 1000.f;
  if (valNO2 < 0) valNO2 = 0;

  float valTemp = bme.readTemperature() * 100;
  float valHumd = bme.readHumidity() * 100;
  float valPres = bme.readPressure() / 100.0F * 100;
  float valDust = dustDensity * 100;
  delay(170);

//  Debugging
  Serial.println("");
  Serial.println("---------- Sensor values ----------");

  Serial.print("CO  = ");
  Serial.print(valCO/1000.0);
  Serial.println(" ppm");

  Serial.print("NO2 = ");
  Serial.print(valNO2/1000.0);
  Serial.println(" ppm");

  Serial.print("Temperature = ");
  Serial.print(valTemp/100.0);
  Serial.println(" °C");

  Serial.print("Humidity    = ");
  Serial.print(valHumd/100);
  Serial.println(" %");

  Serial.print("Dust        = ");
  Serial.print(valDust/100);
  Serial.println(" %");

  Serial.print("Pressure    = ");
  Serial.print(valPres/100);
  Serial.println(" hPa");
  Serial.println("-----------------------------------");

  // CO
  txBuffer[ 9] = ((uint32_t)valCO >> 24 ) & 0xFF;
  txBuffer[10] = ((uint32_t)valCO >> 16 ) & 0xFF;
  txBuffer[11] = ((uint32_t)valCO >> 8 )  & 0xFF;
  txBuffer[12] = ((uint32_t)valCO      )  & 0xFF;
  // NO2
  txBuffer[13] = ((uint32_t)valNO2 >> 24 ) & 0xFF;
  txBuffer[14] = ((uint32_t)valNO2 >> 16 ) & 0xFF;
  txBuffer[15] = ((uint32_t)valNO2 >> 8 )  & 0xFF;
  txBuffer[16] = ((uint32_t)valNO2      )  & 0xFF;
  // Temp
  txBuffer[17] = ((uint32_t)valTemp >> 24 ) & 0xFF;
  txBuffer[18] = ((uint32_t)valTemp >> 16 ) & 0xFF;
  txBuffer[19] = ((uint32_t)valTemp >> 8 )  & 0xFF;
  txBuffer[20] = ((uint32_t)valTemp      )  & 0xFF;

  // Humidity
  txBuffer[21] = ((uint32_t)valHumd >> 24 ) & 0xFF;
  txBuffer[22] = ((uint32_t)valHumd >> 16 ) & 0xFF;
  txBuffer[23] = ((uint32_t)valHumd >> 8 )  & 0xFF;
  txBuffer[24] = ((uint32_t)valHumd      )  & 0xFF;
  // txBuffer[18] = valHumd;

  // Dust
  txBuffer[25] = ((uint32_t)valDust >> 24 ) & 0xFF;
  txBuffer[26] = ((uint32_t)valDust >> 16 ) & 0xFF;
  txBuffer[27] = ((uint32_t)valDust >> 8 )  & 0xFF;
  txBuffer[28] = ((uint32_t)valDust      )  & 0xFF;
  
}
