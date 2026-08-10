#ifndef FOC_MATH_H
#define FOC_MATH_H

typedef struct
{
    float Ialpha;
    float Ibeta;
} Clark_t;

typedef struct
{
    float Id;
    float Iq;
} Park_t;

typedef struct
{
    float Valpha;
    float Vbeta;
} InvPark_t;

typedef struct
{
    float tona;
    float tonb;
    float tonc;
} Svpwm_t;

Clark_t clark_trans(float Ia, float Ib);
Park_t Park_trans(Clark_t ClarkTrans, float sinTheta, float cosTheta);
InvPark_t invPark(float Vd, float Vq, float sinTheta, float cosTheta);
Svpwm_t svpwmcalc(float Valpha, float Vbeta, int TsTicks, float VbusValue);

#endif
