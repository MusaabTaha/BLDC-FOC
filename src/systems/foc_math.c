#include "foc_math.h"

__attribute__((optimize("O2")))
Clark_t clark_trans(float Ia, float Ib)
{
    Clark_t ClarkTrans;

    ClarkTrans.Ialpha = Ia;
    ClarkTrans.Ibeta  = (Ia + (2.0f * Ib)) * 0.57735026919f;

    return ClarkTrans;
}

__attribute__((optimize("O2")))
Park_t Park_trans(Clark_t ClarkTrans, float sinTheta, float cosTheta)
{
    Park_t ParkTrans;

    ParkTrans.Id =
        (ClarkTrans.Ialpha * cosTheta) +
        (ClarkTrans.Ibeta  * sinTheta);

    ParkTrans.Iq =
        (-ClarkTrans.Ialpha * sinTheta) +
        (ClarkTrans.Ibeta   * cosTheta);

    return ParkTrans;
}

__attribute__((optimize("O2")))
InvPark_t invPark(float Vd, float Vq, float sinTheta, float cosTheta)
{
    InvPark_t invPark1;

    invPark1.Valpha = Vd * cosTheta - Vq * sinTheta;
    invPark1.Vbeta  = Vd * sinTheta + Vq * cosTheta;

    return invPark1;
}

__attribute__((optimize("O2")))
Svpwm_t svpwmcalc(float Valpha, float Vbeta, int TsTicks, float VbusValue)
{
    Svpwm_t svVec;

    float Varef = Valpha;
    float Vbref = -0.5f * Valpha + 0.86602540378f * Vbeta;
    float Vcref = -0.5f * Valpha - 0.86602540378f * Vbeta;

    int sector;

    if (Vbeta >= 0.0f)
    {
        if (Valpha >= 0.0f)
            sector = (Vbeta <= (1.73205080757f * Valpha)) ? 1 : 2;
        else
            sector = (Vbeta > (-1.73205080757f * Valpha)) ? 2 : 3;
    }
    else
    {
        if (Valpha < 0.0f)
            sector = (Vbeta >= (1.73205080757f * Valpha)) ? 4 : 5;
        else
            sector = (Vbeta < (-1.73205080757f * Valpha)) ? 5 : 6;
    }

    if ((sector == 1) || (sector == 4))
    {
        svVec.tona =
            ((float)TsTicks / (2.0f * VbusValue)) *
            (VbusValue + Varef - Vcref);

        svVec.tonb =
            ((float)TsTicks / (2.0f * VbusValue)) *
            (VbusValue + 2.0f * Vbref - Varef - Vcref);

        svVec.tonc =
            ((float)TsTicks / (2.0f * VbusValue)) *
            (VbusValue + Vcref - Varef);
    }
    else if ((sector == 2) || (sector == 5))
    {
        svVec.tona =
            ((float)TsTicks / (2.0f * VbusValue)) *
            (VbusValue + 2.0f * Varef - Vbref - Vcref);

        svVec.tonb =
            ((float)TsTicks / (2.0f * VbusValue)) *
            (VbusValue + Vbref - Vcref);

        svVec.tonc =
            ((float)TsTicks / (2.0f * VbusValue)) *
            (VbusValue + Vcref - Vbref);
    }
    else
    {
        svVec.tona =
            ((float)TsTicks / (2.0f * VbusValue)) *
            (VbusValue + Varef - Vbref);

        svVec.tonb =
            ((float)TsTicks / (2.0f * VbusValue)) *
            (VbusValue + Vbref - Varef);

        svVec.tonc =
            ((float)TsTicks / (2.0f * VbusValue)) *
            (VbusValue + 2.0f * Vcref - Varef - Vbref);
    }

    return svVec;
}
