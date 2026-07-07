#ifndef Hardware_h
#define Hardware_h
#include <Arduino.h>
#include <RF24.h>
#include <MPU6050.h>
#include "definiciones.h"
#define BOTON_GARRA 9
#define CE_PIN 9
#define CSN_PIN 10

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