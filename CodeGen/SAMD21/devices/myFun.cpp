#include <Arduino.h>
#include <Wire.h>
#include <LSM6DSL.h>
#include <ESC.h>
#include "Adafruit_VL6180X.h"
#include <SPI.h>

extern "C"{
  void setTimer(unsigned long period, void (*call)());
  void serialInit();
  void serialWrite(char* buffer, int len);
  int serialRead(char* buffer, int len);
  void setPinMode(int pin, int mode);
  void initADDA();
  void initPWM(int pin, int freq);
  void writeDA(int pin, double val);
  double readAD(int pin);
  void writeDO(int pin, int val);
  int readDI(int pin);
  void writePWM(int pin, double val);
  void * setESC(int pin);
  void writeESC(void * esc, double val);
  int initIMU();
  void readAcc(double * meas);
  void readGyro(double * meas);
  double readTemp();
  int initVLsensor();
  double getLux();
  double getRange();
  void initRGB();
  void setRGB(int br, int r, int g, int b, int mode);
  void initIPC();
  void sendIPC();
  void debugSerial(char * buff);
}

#define ADDARES 4095
#define ADDABIT 12
#define PWMRES 4095
#define ESCRES 2000
#define ESCDELTA 500
#define VREF 3.0

LSM6DSL imu(LSM6DSL_MODE_I2C, 0x6B);

Adafruit_VL6180X vl = Adafruit_VL6180X();
int VLcnt = 0;

void setTimer(unsigned long period, void (* call)())
{
  int timID = createTimer(period, call);
  startTimer(timID);
}

void serialInit()
{
  static int serialFlag = 0;
  
  if (serialFlag==0){    
    Serial.setTimeout(1);
    Serial.begin(115200);
    //Serial.flush();
    serialFlag = 1;
  }
}

void serialWrite(char* buffer, int len)
{
  Serial.write(buffer, len);
}

int serialRead(char* buffer, int len)
{
  if(Serial.available()){
    Serial.readBytes(buffer, len);
    return 1;
  }
  else return 0;
}

void setPinMode(int pin, int mode)
{
  pinMode(pin, mode);
}

void initADDA()
{
  analogWriteResolution(ADDABIT);
  analogReadResolution(ADDABIT);
}

void initPWM(int pin, int freq)
{
  analogWriteResolution(ADDABIT);
  pinMode(pin, OUTPUT);
  analogWriteFreq(pin, 0, freq);
}

void writeDA(int pin, double val)
{
  analogWrite(pin, (int) (val*ADDARES));  
}

double readAD(int pin)
{
  double value;
  value = (double) analogRead(pin);
  return value/ADDARES;
}

void writeDO(int pin, int val)
{
  if(val>0) digitalWrite(pin, HIGH);
  else        digitalWrite(pin, LOW);
}

int readDI(int pin)
{
  int input = digitalRead(pin);
  if(input == HIGH) return 1;
  else                      return 0; 
}

void writePWM(int pin, double val)
{
  analogWrite(pin, (int) (val*PWMRES));
}

void * setESC(int pin)
{
  ESC *esc = new ESC(pin, 500, 2500, 500);
  esc->arm();
  return (void *) esc;
}

void writeESC(void * esc, double val)
{
  ESC* e = (ESC*) esc;
  e->speed((int) (val*ESCRES+ESCDELTA));
}

int initIMU()
{
  static int IMUcnt = 0;
 
  if(IMUcnt==0){
    IMUcnt=1;
    if(imu.begin()) return 1;
    else                 return 0;
  }
  else                   return 1;
}

void readAcc(double * meas)
{
  meas[0] = (double) imu.readFloatAccelX();
  meas[1] = (double) imu.readFloatAccelY();
  meas[2] = (double) imu.readFloatAccelZ();
}

void readGyro(double * meas)
{
  meas[0] = (double) imu.readFloatGyroX();
  meas[1] = (double) imu.readFloatGyroY();
  meas[2] = (double) imu.readFloatGyroZ();
}

double readTemp()
{
  return (double) imu.readTemperatureC();
}

int initVLsensor()
{
  if(VLcnt==0){
    VLcnt++;
    
    vl.begin();
    delay(100);
    return 1;
  }
  else               return 1;
}

double getLux()
{
  float lux = vl.readLux(VL6180X_ALS_GAIN_5);
  return (double) lux;
}

double getRange()
{
  uint8_t range = vl.readRange();
  uint8_t status = vl.readRangeStatus();
  if (status == VL6180X_ERROR_NONE) return 1.0*range;
  else                                                       return (-1.0);
}

void initRGB()
{
   pinMode(LED_BUILTIN, OUTPUT);
   setRGB(0, 0, 0, 0, 0);

}

void setRGB(int br, int r, int g, int b, int mode)
{
  ledBuiltinSetBrightness(br);
  ledBuiltinSetColor(r, g, b);
  if(mode==1) digitalWrite(LED_BUILTIN, HIGH);
  else               digitalWrite(LED_BUILTIN, LOW);   
}

char rxBuff[80];
char txBuff[80];
int txLen = 0;

/* Callback function for IPC communicatio */
void dataReceivedCallback(uint32_t len)
{
  /* Do nothing */
}

void initIPC()
{
  /* Communication to ESP32  */
  /* Output to ESP32 :     ID = 0x01 */
  /* Input from ESP32:    ID = 0x02 */
  communication.registerCommand(0x01, NULL, NULL);
  communication.registerCommand(0x02, dataReceivedCallback, rxBuff);  
}

void sendIPC()
{
    communication.sendCommand(0x01, txBuff, txLen);
    txLen = 0;
}

void debugSerial(char * buff)
{
  Serial.println(buff);
}
