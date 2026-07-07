#include "Hardware.h"





void initButtonInterrupt(uint8_t BTN_PIN){
      pinMode(BTN_PIN, INPUT_PULLUP);

    attachInterrupt(
        digitalPinToInterrupt(BTN_PIN),
        buttonISR,
        FALLING
    );
}

void buttonISR(void)
{
    buttonFlag = true;
}

void mpuMotionISR()
{
    mpuMotionFlag = true;
}
void enableMPUWakeup()
{
    mpu1.setMotionDetectionThreshold(20);
    mpu1.setMotionDetectionDuration(5);
    mpu1.setIntMotionEnabled(true);
    mpu2.setMotionDetectionThreshold(20);
    mpu2.setMotionDetectionDuration(5);
    mpu2.setIntMotionEnabled(true);

    attachInterrupt(
        digitalPinToInterrupt(MPU_INT_PIN),
        mpuMotionISR,
        RISING
    );

}

void disableMPUWakeup(void)
{
    mpu1.setIntMotionEnabled(false);
    mpu2.setIntMotionEnabled(false);

    detachInterrupt(
        digitalPinToInterrupt(MPU_INT_PIN)
    );

}
void enableSamplingTimer(void){
  cli();
TIMSK1 |= (1 << OCIE1A);
sei();
}

void disableSamplingTimer(void){
    cli();
   TIMSK1 &= ~(1 << OCIE1A);
    sei();

}

void readSensors(Mando *M)
{
    int16_t ax, ay, az;
    int16_t gx, gy, gz;

    // MPU 1
    mpu1.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    M->payload.acelerometros[0][0] = ax;
    M->payload.acelerometros[0][1] = ay;
    M->payload.acelerometros[0][2] = az;

    M->payload.giroscopios[0][0] = gx;
    M->payload.giroscopios[0][1] = gy;
    M->payload.giroscopios[0][2] = gz;

    // MPU 2
    mpu2.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    M->payload.acelerometros[1][0] = ax;
    M->payload.acelerometros[1][1] = ay;
    M->payload.acelerometros[1][2] = az;

    M->payload.giroscopios[1][0] = gx;
    M->payload.giroscopios[1][1] = gy;
    M->payload.giroscopios[1][2] = gz;

    M->payload.pote = analogRead(A0);

    
}

void buildPacket(Mando *M)
{
    M->payload.TIMESTAMP = millis();
     if(M->gripperClosed)
        M->payload.flag |= FLAG_APRETAR_PINZA;
    else
        M->payload.flag &= ~FLAG_APRETAR_PINZA;
}

void transmitPacket(Mando *M)
{
    radio.write(&M->payload, sizeof(M->payload));   
}