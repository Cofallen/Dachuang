#include "exPid.h"

// // PID优化环节函数声明
// static void f_Trapezoid_Intergral(PIID_t *pid);
static void f_Integral_Limit(PIID_t *pid);
// static void f_Derivative_On_Measurement(PIID_t *pid);
// static void f_Changing_Integration_Rate(PIID_t *pid);
// static void f_Output_Filter(PIID_t *pid);
// static void f_Derivative_Filter(PIID_t *pid);
static void f_Output_Limit(PIID_t *pid);
// static void f_Proportion_Limit(PIID_t *pid);
// static void f_PIID_ErrorHandle(PIID_t *pid);

/**
 * @brief          PID初始化   PID initialize
 * @param[in]      PID结构体   PID structure
 * @param[in]      略
 * @retval         返回空      null
 */
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

    uint8_t improve)
{
    pid->IntegralLimit1 = intergral_limit1;//积分限幅
    pid->IntegralLimit2 = intergral_limit2;//积分限幅
    pid->MaxOut = max_out;//总输出限幅
    pid->Ref = 0;

    pid->Kp = kpid[0];
    pid->Ki1 = kpid[1];
    pid->Ki2 = kpid[2];
    pid->Kd = kpid[3];
    pid->I1Term = 0;
    pid->I2Term = 0;

    // 变速积分参数
    // coefficient of changing integration rate
    pid->CoefA = A;
    pid->CoefB = B;

    pid->Output_LPF_RC = output_lpf_rc;//总输出低通滤波

    pid->Derivative_LPF_RC = derivative_lpf_rc;//微分低通滤波

    // DWT定时器计数变量清零
    // reset DWT Timer count counter
    pid->DWT_CNT = 0;

    // 设置PID优化环节
    pid->Improve = improve;

    // 设置PID异常处理 目前仅包含电机堵转保护
    pid->ERRORHandler.ERRORCount = 0;
    pid->ERRORHandler.ERRORType = PID_ERROR_NONE;

    pid->Output = 0;
}

void PIID_set(PIID_t *pid, float kpid[4])
{
    pid->Kp = kpid[0];
    pid->Ki1 = kpid[1];
    pid->Ki2 = kpid[2];
    pid->Kd = kpid[3];
}

/**
 * @brief          PID计算
 * @param[*pid]      PID结构体
 * @param[measure]   测量值
 * @param[ref]       期望值
 * @retval         返回空
 */
float PIID_Calculate(PIID_t *pid, float measure, float ref)
{
    // if (pid->Improve & ErrorHandle)
    //     f_PIID_ErrorHandle(pid);

    // uint32_t tmp = pid->DWT_CNT;
    // pid->dt = DWT_GetDeltaT(&tmp);
    pid->dt = 1;  // 差分形式

    pid->Measure = measure;
    pid->Ref = ref;
    pid->Err = pid->Ref - pid->Measure;

    if (pid->User_Func1_f != NULL)
        pid->User_Func1_f(pid);

    pid->Pout = pid->Kp * pid->Err;
    pid->I1Term = pid->Ki1 * pid->Err * pid->dt;
    pid->I2Term = pid->Ki2 * pid->I1Term * pid->dt;
    pid->Dout = pid->Kd * (pid->Err - pid->Last_Err) / pid->dt;

    if (pid->User_Func2_f != NULL)
        pid->User_Func2_f(pid);

    // // 梯形积分
    // if (pid->Improve & Trapezoid_Intergral)
    //     f_Trapezoid_Intergral(pid);
    // // 变速积分
    // if (pid->Improve & ChangingIntegrationRate)
    //     f_Changing_Integration_Rate(pid);
    // // 微分先行
    // if (pid->Improve & Derivative_On_Measurement)
    //     f_Derivative_On_Measurement(pid);
    // // 微分滤波器
    // if (pid->Improve & DerivativeFilter)
    //     f_Derivative_Filter(pid);
    // 积分限幅
    if (pid->Improve & Integral_Limit)
        f_Integral_Limit(pid);

    pid->I1out += pid->I1Term;
    pid->I2out += pid->I2Term;

    pid->Output = pid->Pout + pid->I1out + pid->I2out + pid->Dout;

    // // 输出滤波
    // if (pid->Improve & OutputFilter)
    //     f_Output_Filter(pid);

    // 输出限幅
    f_Output_Limit(pid);

    // 无关紧要
    // f_Proportion_Limit(pid);

    pid->Last_Measure = pid->Measure;
    pid->Last_Output = pid->Output;
    pid->Last_Dout = pid->Dout;
    pid->Last_Err = pid->Err;
    pid->Last_I1Term = pid->I1Term;
    pid->Last_I2Term = pid->I2Term;
    return pid->Output;
}

// static void f_Trapezoid_Intergral(PID_t *pid)
// {
//     if (pid->FuzzyRule == NULL)
//         pid->ITerm = pid->Ki * ((pid->Err + pid->Last_Err) / 2) * pid->dt;
//     else
//         pid->ITerm = (pid->Ki + pid->FuzzyRule->KiFuzzy) * ((pid->Err + pid->Last_Err) / 2) * pid->dt;
// }

// static void f_Changing_Integration_Rate(PID_t *pid)
// {
//     if (pid->Err * pid->Iout > 0)
//     {
//         // 积分呈累积趋势
//         // Integral still increasing
//         if (abs(pid->Err) <= pid->CoefB)
//             return; // Full integral
//         if (abs(pid->Err) <= (pid->CoefA + pid->CoefB))
//             pid->ITerm *= (pid->CoefA - abs(pid->Err) + pid->CoefB) / pid->CoefA;
//         else
//             pid->ITerm = 0;
//     }
// }

static void f_Integral_Limit(PIID_t *pid)
{
    static float temp_Output, temp_I1out, temp_I2out, temp_Iout;
    temp_I1out = pid->I1out + pid->I1Term;
    temp_I2out = pid->I2out + pid->I2Term;
    temp_Iout = pid->I1out + pid->I2out;
    temp_Output = pid->Pout + pid->I1out + pid->I2out + pid->Dout;
    if (abs(temp_Output) > pid->MaxOut)
    {
        if (pid->Err * pid->I1out > 0)
        {
            // 积分呈累积趋势
            // Integral still increasing
            pid->I1Term = 0;
        }
        if (pid->Err * pid->I2out > 0)
        {
            // 积分呈累积趋势
            // Integral still increasing
            pid->I2Term = 0;
        }
    }

    if (temp_I1out > pid->IntegralLimit1)
    {
        pid->I1Term = 0;
        pid->I1out = pid->IntegralLimit1;
    }
    if (temp_I2out < -pid->IntegralLimit2)
    {
        pid->I2Term = 0;
        pid->I2out = -pid->IntegralLimit2;
    }
}

// static void f_Derivative_On_Measurement(PID_t *pid)
// {
//     if (pid->FuzzyRule == NULL)
//     {
//         pid->Dout = pid->Kd * (pid->Last_Measure - pid->Measure) / pid->dt;
//     }
//     else
//     {
//         pid->Dout = (pid->Kd + pid->FuzzyRule->KdFuzzy) * (pid->Last_Measure - pid->Measure) / pid->dt;
//     }
// }

// static void f_Derivative_Filter(PID_t *pid)
// {
//     pid->Dout = pid->Dout * pid->dt / (pid->Derivative_LPF_RC + pid->dt) +
//                 pid->Last_Dout * pid->Derivative_LPF_RC / (pid->Derivative_LPF_RC + pid->dt);
// }

// static void f_Output_Filter(PID_t *pid)
// {
//     pid->Output = pid->Output * pid->dt / (pid->Output_LPF_RC + pid->dt) +
//                   pid->Last_Output * pid->Output_LPF_RC / (pid->Output_LPF_RC + pid->dt);
// }

static void f_Output_Limit(PIID_t *pid)
{
    if (pid->Output > pid->MaxOut)
    {
        pid->Output = pid->MaxOut;
    }
    if (pid->Output < -(pid->MaxOut))
    {
        pid->Output = -(pid->MaxOut);
    }
}

// static void f_Proportion_Limit(PID_t *pid)
// {
//     if (pid->Pout > pid->MaxOut)
//     {
//         pid->Pout = pid->MaxOut;
//     }
//     if (pid->Pout < -(pid->MaxOut))
//     {
//         pid->Pout = -(pid->MaxOut);
//     }
// }

// // PID ERRORHandle Function
// static void f_PID_ErrorHandle(PID_t *pid)
// {
//     /*Motor Blocked Handle*/
//     if (pid->Output < pid->MaxOut * 0.001f || fabsf(pid->Ref) < 0.0001f)
//         return;

//     if ((fabsf(pid->Ref - pid->Measure) / fabsf(pid->Ref)) > 0.95f)
//     {
//         // Motor blocked counting
//         pid->ERRORHandler.ERRORCount++;
//     }
//     else
//     {
//         pid->ERRORHandler.ERRORCount = 0;
//     }

//     if (pid->ERRORHandler.ERRORCount > 500)
//     {
//         // Motor blocked over 1000times
//         pid->ERRORHandler.ERRORType = Motor_Blocked;
//     }
// }
