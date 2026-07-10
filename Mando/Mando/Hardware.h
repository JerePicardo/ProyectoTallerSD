#ifndef Hardware_h
#define Hardware_h
#include <Arduino.h>
#include <RF24.h>
#include <MPU6050.h>
#include "definiciones.h"


extern RF24 radio;

extern MPU6050 mpu1;
extern MPU6050 mpu2;

void readSensors(Mando *M);
void buildPacket(Mando *M);
void transmitPacket(Mando *M);
void disableMPUWakeup(void);
void enableSamplingTimer(void);
void disableSamplingTimer(void);

void mpuMotionISR(void);
void enableMPUWakeup(void);
void buttonISR(void);
void initButtonInterrupt(uint8_t BTN_PIN);
#endif