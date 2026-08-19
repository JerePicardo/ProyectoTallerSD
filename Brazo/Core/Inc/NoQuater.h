/*
 * NoQuater.h
 *
 *  Created on: 11 ago 2026
 *      Author: Usuario
 */

#ifndef INC_NOQUATER_H_
#define INC_NOQUATER_H_


#include <math.h>

#define DEG_TO_RAD 0.01745329252f
#define KP 1
#define KI 0.05f



typedef struct {
    float w;
    float x;
    float y;
    float z;
} Quaternion;

typedef struct
{
    Quaternion q;

    float ix;
    float iy;
    float iz;

} Mahony;








Quaternion quatMultiply(Quaternion a, Quaternion b);
void quatNormalize(Quaternion *q);
void Mahony_Update(Mahony *m,float ax, float ay, float az,float gx, float gy, float gz,float dt);

#endif /* INC_NOQUATER_H_ */
