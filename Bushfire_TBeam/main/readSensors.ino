
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define DEF_PIN_NUM_CO 15
#define DEF_PIN_NUM_NO2 4

// FIXME take measure resistors
const int resValue_0 = 10000;  // Value of 10kOhm resistor !change this to match your resistor
const int resValue_1 = 10000;  // Value of 10kOhm resistor !change this to match your resistor

const float Vref = 1.1;  //This is the voltage of the internal reference
long int cOff_0 = 2300; //286mV offset due to resistor ladder. Try taking the average of a long
long int cOff_1 = 2100; //286mV offset due to resistor ladder. Try taking the average of a long

//measurement of the counts without a sensor in place. This should give a good Zero.

 // sensitivity in nA/ppm,
const float sensitivityCO = 2.06;  // CO 4.12   
const float sensitivityNO2 = 13.51; // NO2 -27.03

//this is roughly about 1/2 the sensitivity of the barcode on the sensor. Try finding a known
//concentration of CO to measure, or just approximate.

const int extraBit = 256; //Number of samples averaged, like adding 8 bits to ADC
const int calibNum = 10240; //Number of samples averaged, like adding 8 bits to ADC

long int sensorValueCO = 0;        // value read from the sensor
long int sensorValueNO2 = 0;        // value read from the sensor


bool bFlagCalib = 0;

void calibration(){
  long int sensorValue_0 = 0;
  long int sensorValue_1 = 0;
  Serial.println("Start calibration");
  for (int i = 0; i < calibNum; i++) {
    sensorValue_0 = analogRead(DEF_PIN_NUM_CO)  + sensorValue_0;        
    sensorValue_1 = analogRead(DEF_PIN_NUM_NO2) + sensorValue_1;        

    delay(3);   // needs 2 ms for the analog-to-digital converter to settle after the last reading
  }
  Serial.println("Calibration is ended");
  
  int zeroRefValue_0;
  int zeroRefValue_1;

  zeroRefValue_0 = sensorValue_0/calibNum;  
  zeroRefValue_1 = sensorValue_1/calibNum;  

  //Serial.println(zeroRefValue_0);  
  cOff_0 = zeroRefValue_0;  
  cOff_1 = zeroRefValue_1;  

}

void readSensors(uint8_t txBuffer[30]) {
  // if (bFlagCalib == 0){
  //   calibration();
  //   bFlagCalib = 1;
  // }
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


  sensorValueCO  = sensorValueCO /extraBit - cOff_0; //subtract the offset of the resistor ladder * 256.
  sensorValueNO2 = sensorValueNO2/extraBit - cOff_1; //subtract the offset of the resistor ladder * 256.

  Serial.println(sensorValueCO /extraBit);
  Serial.println(sensorValueNO2/extraBit);

  //  Get sensor values
  float valCO  = ((float) sensorValueCO  / 1024 * Vref / resValue_0 * 1000000000) / sensitivityCO * 1000;
  float valNO2 = ((float) sensorValueNO2 / 1024 * Vref / resValue_1 * 1000000000) / sensitivityNO2 * 1000;
  float valTemp = bme.readTemperature() * 100;
  float valHumd = bme.readHumidity() * 100;
  float valPres = bme.readPressure() / 100.0F * 100;

  delay(170);

 // Debugging
  Serial.println("");
  Serial.println("---------- Sensor values ----------");

  Serial.print("CO  = ");
  Serial.println(valCO/1000.0);

  Serial.print("NO2 = ");
  Serial.println(valNO2/1000.0);

  Serial.print("Temperature = ");
  Serial.print(valTemp/100.0);
  Serial.println(" °C");

  Serial.print("Humidity    = ");
  Serial.print(valHumd/100);
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
  // // Pressure
  txBuffer[25] = ((uint32_t)valPres >> 24 ) & 0xFF;
  txBuffer[26] = ((uint32_t)valPres >> 16 ) & 0xFF;
  txBuffer[27] = ((uint32_t)valPres >> 8 )  & 0xFF;
  txBuffer[28] = ((uint32_t)valPres      )  & 0xFF;
  
}
