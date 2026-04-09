#ifndef __EXPID_H__
#define __EXPID_H__

#include "main.h"
#include "main.h"
#include "stdint.h"
#include "string.h"
#include "stdlib.h"
#include "bsp_dwt.h"
#include "user_lib.h"
// #include "arm_math.h"
#include <math.h>
#include "controller.h"

#ifndef abs
#define abs(x) ((x > 0) ? x : -x)
#endif

#ifndef user_malloc
#ifdef _CMSIS_OS_H
#define user_malloc pvPortMalloc
#else
#define user_malloc malloc
#endif
#endif


// typedef enum piid_Improvement_e
// {
//     NONE = 0X00,                        //0000 0000
//     Integral_Limit = 0x01,              //0000 0001
//     Derivative_On_Measurement = 0x02,   //0000 0010
//     Trapezoid_Intergral = 0x04,         //0000 0100
//     Proportional_On_Measurement = 0x08, //0000 1000
//     OutputFilter = 0x10,                //0001 0000
//     ChangingIntegrationRate = 0x20,     //0010 0000
//     DerivativeFilter = 0x40,            //0100 0000
//     ErrorHandle = 0x80,                 //1000 0000
// } PIID_Improvement_e;

// typedef enum errorType_e
// {
//     PID_ERROR_NONE = 0x00U,
//     Motor_Blocked = 0x01U
// } ErrorType_e;

typedef struct __packed
{
    uint64_t ERRORCount;
    ErrorType_e ERRORType;
} PIID_ErrorHandler_t;

typedef struct __packed piid_t
{
    float Ref;
    float Kp;
    float Ki1;
    float Ki2;
    float Kd;

    float Measure;
    float Last_Measure;
    float Err;
    float Last_Err;
    float Last_I1Term;
    float Last_I2Term;

    float Pout;
    float I1out;
    float I2out;
    float Dout;
    float I1Term;
    float I2Term;

    float Output;
    float Last_Output;
    float Last_Dout;

    float MaxOut;
    float IntegralLimit1;
    float IntegralLimit2;
    float DeadBand;
    float ControlPeriod;
    float CoefA;         //For Changing Integral
    float CoefB;         //ITerm = Err*((A-abs(err)+B)/A)  when B<|err|<A+B
    float Output_LPF_RC; // RC = 1/omegac
    float Derivative_LPF_RC;

    uint32_t DWT_CNT;
    float dt;

    uint8_t Improve;

    PIID_ErrorHandler_t ERRORHandler;

    void (*User_Func1_f)(struct piid_t *pid);
    void (*User_Func2_f)(struct piid_t *pid);
} PIID_t;

void PIID_Init(
    PIID_t *pid,
    float max_out,
    float intergral_limit1,
    float intergral_limit2,

    float kpid[3],

    float A,
    float B,

    float output_lpf_rc,
    float derivative_lpf_rc,

    uint16_t ols_order,

    uint8_t improve);
void PIID_set(PIID_t *pid, float kpid[4]);
float PIID_Calculate(PIID_t *pid, float measure, float ref);


#endif // !__EXPID_H__