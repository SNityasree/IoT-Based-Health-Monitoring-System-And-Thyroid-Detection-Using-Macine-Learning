#include <Wire.h>
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#include <Wire.h> //Include wire library
#include <MPU6050_light.h> //Include library for MPU communication
MPU6050 mpu(Wire); //Create object mpu
#include <DFRobot_MAX30102.h>
#include <OneWire.h>
#include <DallasTemperature.h>
DFRobot_MAX30102 particleSensor;
#define DS18B20 D4
#define REPORTING_PERIOD_MS 1000
int falldetection;
float bodytemperature,temp;
unsigned long timer = 0;
WiFiClient client;
unsigned long myChannelNumber = 3;
const char * myWriteAPIKey = "ZOYB2BI1PQ9UQRVV";
const char* ssid = "Iot2023"; // Enter SSID here
const char* password = "testing2023"; //Enter Password here
uint32_t tsLastReport = 0;
OneWire oneWire(DS18B20);
DallasTemperature sensors(&oneWire);
void setup()
{
Serial.begin(115200);
WiFi.mode(WIFI_STA); 
ThingSpeak.begin(client);
Serial.print("Initializing...");
while (!particleSensor.begin()) {
Serial.println("MAX30102 was not found");
delay(1000);
}
particleSensor.sensorConfiguration(/*ledBrightness=*/50, /*sampleAverage=*/SAMPLEAVG_4, \
/*ledMode=*/MODE_MULTILED, /*sampleRate=*/SAMPLERATE_100, \
/*pulseWidth=*/PULSEWIDTH_411, /*adcRange=*/ADCRANGE_16384);
mpu.begin(); 
Serial.print(F("MPU6050 status: "));
Serial.println(F("Calculating offsets, do not move MPU6050")); 
delay(1000);
mpu.calcGyroOffsets(); //Calibrate gyroscope
Serial.println("Done!\n");
}
int32_t SPO2; //SPO2
int8_t SPO2Valid; //Flag to display if SPO2 calculation is valid
int32_t heartRate; //Heart-rate
int8_t heartRateValid; //Flag to display if heart-rate calculation is valid
void loop()
{
if(WiFi.status() != WL_CONNECTED){
Serial.print("Attempting to connect");
while(WiFi.status() != WL_CONNECTED){
WiFi.begin(ssid, password);
delay(4000); 
} }
Serial.println("\nConnected.");
sensors.requestTemperatures(); 
bodytemperature = sensors.getTempCByIndex(0);
mpu.update(); //Get values from MPU
if ((millis() - timer) > 100) { // print data every 100ms
timer = millis();
delay(10);
Serial.print(int(mpu.getAngleX()); //Print Z angle value on LCD
falldetection=(int(mpu.getAngleX()));
delay(10);
}
Serial.println(F("Wait about four seconds"));
particleSensor.heartrateAndOxygenSaturation(/**SPO2=*/&SPO2, /**SPO2Valid=*/&SPO2Valid, 
/**heartRate=*/&heartRate, /**heartRateValid=*/&heartRateValid);
//Print result
Serial.print(F("heartRate="));
Serial.print(heartRate, DEC);
Serial.print(F("; SPO2="));
Serial.print(SPO2, DEC);
temp=(bodytemperature*9/5+32);
Serial.print("Body Temperature: ");
Serial.print(temp);
Serial.println("°F");
Serial.println("*********************************");
Serial.println();
ThingSpeak.setField(1, heartRate);
ThingSpeak.setField(2, temp);
ThingSpeak.setField(3, SPO2);
ThingSpeak.setField(4, falldetection);
int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey); 
tsLastReport = millis();
}
