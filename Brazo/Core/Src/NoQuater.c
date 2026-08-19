
#include <NoQuater.h>


Quaternion quatMultiply(Quaternion a, Quaternion b)
{
    Quaternion r;

    r.w = a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z;
    r.x = a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y;
    r.y = a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x;
    r.z = a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w;

    return r;
}
void quatNormalize(Quaternion *q)
{
    float n = sqrtf(q->w*q->w +
                    q->x*q->x +
                    q->y*q->y +
                    q->z*q->z);
    q->w /= n;
    q->x /= n;
    q->y /= n;
    q->z /= n;
}

 void Mahony_Update(Mahony *m,float ax, float ay, float az,float gx, float gy, float gz,float dt)
{
    // 1) Normalizar acelerómetro
    float norm = sqrtf(ax*ax + ay*ay + az*az);

    if (norm == 0.0f)
        return;

    ax /= norm;
    ay /= norm;
    az /= norm;

    // 2) Quaternion actual
    float w = m->q.w;
    float x = m->q.x;
    float y = m->q.y;
    float z = m->q.z;

    // 3) Gravedad estimada por el quaternion
    float vx = 2.0f * (x*z - w*y);
    float vy = 2.0f * (w*x + y*z);
    float vz = w*w - x*x - y*y + z*z;

    // 4) Error entre gravedad medida y estimada
    float ex = ay*vz - az*vy;
    float ey = az*vx - ax*vz;
    float ez = ax*vy - ay*vx;

    // 5) Integral del error
    m->ix += ex * dt;
    m->iy += ey * dt;
    m->iz += ez * dt;

    // 6) Corregir gyro
    gx += KP * ex + KI * m->ix;
    gy += KP * ey + KI * m->iy;
    gz += KP * ez + KI * m->iz;

    // 7) Integrar quaternion
    float half_dt = 0.5f * dt;

    m->q.w += (-x*gx - y*gy - z*gz) * half_dt;

    m->q.x += ( w*gx + y*gz - z*gy) * half_dt;

    m->q.y += ( w*gy - x*gz + z*gx) * half_dt;

    m->q.z += ( w*gz + x*gy - y*gx) * half_dt;

    // 8) Normalizar quaternion
    quatNormalize(&m->q);
}


