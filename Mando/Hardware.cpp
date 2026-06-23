

#include <Hardware.h>



void initButtonInterrupt(uint8_t BUTTON_PIN,void (*callback)())
{
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    attachInterrupt(
        digitalPinToInterrupt(BUTTON_PIN),
        callback,
        FALLING
    );
}


void initMPUInterrupt(uint8_t pin, void (*callback)()){

    pinMode(pin, INPUT);


    attachInterrupt(
        digitalPinToInterrupt(pin),
        callback,
        RISING
    );

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

void disableMPUWakeup()
{

    mpu1.setIntMotionEnabled(false);

    mpu2.setIntMotionEnabled(false);



    detachInterrupt(
        digitalPinToInterrupt(MPU_INT_PIN)
    );

}
