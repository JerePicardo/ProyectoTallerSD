#include "Hardware.h"




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

    M->payload.boton = digitalRead(BOTON_GARRA);
}

void buildPacket(Mando *M)
{
    M->payload.TIMESTAMP = millis();
}

