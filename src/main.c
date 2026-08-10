/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Musaab Taha
 * @brief          : STM32F429 BLDC motor control: six-step startup and Hall-sensored FOC
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>
#include <math.h>

/* ========================================================================== */
/* Hardware definitions                                                       */
/* ========================================================================== */


#include "motor_hw.h"
#include "foc_math.h"

/* ========================================================================== */
/* Firmware interfaces                                                        */
/* ========================================================================== */

uint8_t ReadHallState(void);
float HallElectricalAngle(uint8_t hallState);
void SaveEdgeSample(uint8_t hallState);
void ControlSync_Init(void);
void FOC_ControlStep(void);


/* ========================================================================== */
/* Test and diagnostic data                                                   */
/* ========================================================================== */

#define EDGE_SAMPLE_COUNT         12U
#define ANGLE_SAMPLE_COUNT        24U
#define ANGLE_SAMPLE_PERIOD_TICKS 4000U
#define TIM5_TICK_HZ              16000000.0f

/* Hall-edge test results */
volatile uint32_t dtSamples[EDGE_SAMPLE_COUNT];
volatile float speedSamples[EDGE_SAMPLE_COUNT];
volatile uint8_t hallSamples[EDGE_SAMPLE_COUNT];
volatile uint32_t edgeSampleIndex = 0U;

/* Angle-interpolation test results */
volatile float angleSamples[ANGLE_SAMPLE_COUNT];
volatile float sectorAngleSamples[ANGLE_SAMPLE_COUNT];
volatile int baseAngleSamples[ANGLE_SAMPLE_COUNT];
volatile uint8_t angleHallSamples[ANGLE_SAMPLE_COUNT];
volatile uint32_t angleSampleIndex = 0U;
volatile uint32_t lastAngleSampleTick = 0U;

volatile uint8_t speedValid = 0U;

volatile int hallA, hallB, hallC;
volatile int rawIDR;
int lastHallValue;

int hallValue = 0;
volatile int firstInt = 0;
int CurrentHallState = 0;
int angle = 0;
volatile int dt = 0;
volatile int lastTimeStamp = 0;
volatile int CurrentTimeStamp = 0;
int now = 0;
volatile int baseAngele = 0;
volatile float currentangle = 0;
volatile float speedsec = 0;
volatile uint16_t DMABuffer[2];
float offsetVoltA = 0;
float offsetVoltB = 0;
float voltA;
float voltB;
float currA;
float currB;
float currC;


#define CURRENT_SAMPLE_COUNT          256U
#define CURRENT_SAMPLE_PERIOD_TICKS   16000U   // 1 ms at 16 MHz TIM5

volatile uint16_t currentASamples[CURRENT_SAMPLE_COUNT];
volatile uint16_t currentBSamples[CURRENT_SAMPLE_COUNT];
volatile uint8_t  currentHallSamples[CURRENT_SAMPLE_COUNT];

volatile uint32_t currentSampleIndex = 0U;
volatile uint32_t lastCurrentSampleTick = 0U;


/* ========================================================================== */
/* FOC control blocks                                                         */
/* ========================================================================== */





float kpd = 1.0f;
float Kid = 0.0f;
float kpq = 4.0f;
float Kiq = 500.0f;
float Ts = 0.00005f;

float Id_ref = 0.0f;
float Iq_ref = 0.3f;
float Vbus = 12.0f;

#define HALL_ANGLE_OFFSET_DEG (-60.0f)
#define FOC_VOLTAGE_LIMIT      6.0f
#define FOC_PWM_DIVIDER        1U
#define SIX_STEP_TIME_TICKS    160000000U   // 10 s at 16 MHz TIM5
volatile uint8_t focEnabled = 0U;
volatile uint8_t adcBusy = 0U;
volatile uint32_t focPwmCounter = 0U;
volatile uint32_t focExecTicks = 0U;
volatile uint32_t focExecTicksMax = 0U;
volatile uint32_t focExecCoreCycles = 0U;
volatile uint32_t focExecCoreCyclesMax = 0U;
volatile uint32_t focAngleTicks = 0U;
volatile uint32_t focCurrentTicks = 0U;
volatile uint32_t focParkTicks = 0U;
volatile uint32_t focPiTicks = 0U;
volatile uint32_t focSvpwmTicks = 0U;

__attribute__((optimize("O2")))
float dPI(float idref, float id)
{
    float e;
    float Pd;
    float Vd;
    static float intd = 0.0f;

    e = idref - id;
    Pd = kpd * e;
    intd = intd + (Kid * Ts * e);
    Vd = Pd + intd;

    return Vd;
}

__attribute__((optimize("O2")))
float qPI(float iqref, float iq)
{
    float e;
    float Pq;
    float Vq;
    static float intq = 0.0f;

    e = iqref - iq;
    Pq = kpq * e;
    intq = intq + (Kiq * Ts * e);

    if (intq > FOC_VOLTAGE_LIMIT)
        intq = FOC_VOLTAGE_LIMIT;
    else if (intq < -FOC_VOLTAGE_LIMIT)
        intq = -FOC_VOLTAGE_LIMIT;

    Vq = Pq + intq;

    return Vq;
}



/* ========================================================================== */
/* FOC integration                                                            */
/* ========================================================================== */

__attribute__((optimize("O2")))
void FOC_ControlStep(void)
{
    uint32_t focStartCore = DWT->CYCCNT;
    uint32_t focStartTick = TIM5->CNT;
    uint32_t focSectionTick = focStartTick;

    now = TIM5->CNT - lastTimeStamp;

    float hallInterpolation =
        ((float)now / TIM5_TICK_HZ) * speedsec;

    if (hallInterpolation > 60.0f)
        hallInterpolation = 60.0f;

    currentangle =
        -((float)baseAngele + hallInterpolation);

    float angleDeg = currentangle + HALL_ANGLE_OFFSET_DEG;

    while (angleDeg < 0.0f)
        angleDeg += 360.0f;

    while (angleDeg >= 360.0f)
        angleDeg -= 360.0f;

    float angleRad =
        angleDeg * (3.14159265359f / 180.0f);

    float sinTheta = sinf(angleRad);
    float cosTheta = cosf(angleRad);

    focAngleTicks = (uint32_t)(TIM5->CNT - focSectionTick);
    focSectionTick = TIM5->CNT;

    voltA = (3.0f * (float)DMABuffer[0]) / 4095.0f;
    voltB = (3.0f * (float)DMABuffer[1]) / 4095.0f;

    currA = (voltA - offsetVoltA) / (0.01f * 5.18f);
    currB = (voltB - offsetVoltB) / (0.01f * 5.18f);
    currC = -(currA + currB);

    Clark_t ClarkCurrent = clark_trans(currA, currB);

    focCurrentTicks = (uint32_t)(TIM5->CNT - focSectionTick);
    focSectionTick = TIM5->CNT;

    Park_t ParkCurrent =
        Park_trans(ClarkCurrent, sinTheta, cosTheta);

    focParkTicks = (uint32_t)(TIM5->CNT - focSectionTick);
    focSectionTick = TIM5->CNT;

    float Vd = dPI(Id_ref, ParkCurrent.Id);
    float Vq = qPI(Iq_ref, ParkCurrent.Iq);

    float voltageMagnitude = sqrtf((Vd * Vd) + (Vq * Vq));

    if (voltageMagnitude > FOC_VOLTAGE_LIMIT)
    {
        float voltageScale = FOC_VOLTAGE_LIMIT / voltageMagnitude;
        Vd *= voltageScale;
        Vq *= voltageScale;
    }

    focPiTicks = (uint32_t)(TIM5->CNT - focSectionTick);
    focSectionTick = TIM5->CNT;

    InvPark_t InvParkVoltage =
        invPark(Vd, Vq, sinTheta, cosTheta);

    Svpwm_t svVec =
        svpwmcalc(
            InvParkVoltage.Valpha,
            InvParkVoltage.Vbeta,
            (int)TIM1->ARR,
            Vbus);

    if (svVec.tona < 0.0f)
        svVec.tona = 0.0f;
    else if (svVec.tona > (float)TIM1->ARR)
        svVec.tona = (float)TIM1->ARR;

    if (svVec.tonb < 0.0f)
        svVec.tonb = 0.0f;
    else if (svVec.tonb > (float)TIM1->ARR)
        svVec.tonb = (float)TIM1->ARR;

    if (svVec.tonc < 0.0f)
        svVec.tonc = 0.0f;
    else if (svVec.tonc > (float)TIM1->ARR)
        svVec.tonc = (float)TIM1->ARR;

    TIM1->CCR1 = (uint32_t)svVec.tona;
    TIM1->CCR2 = (uint32_t)svVec.tonb;
    TIM1->CCR3 = (uint32_t)svVec.tonc;

    focSvpwmTicks = (uint32_t)(TIM5->CNT - focSectionTick);

    focExecTicks = (uint32_t)(TIM5->CNT - focStartTick);
    focExecCoreCycles = (uint32_t)(DWT->CYCCNT - focStartCore);

    if (focExecTicks > focExecTicksMax)
    {
        focExecTicksMax = focExecTicks;
    }

    if (focExecCoreCycles > focExecCoreCyclesMax)
    {
        focExecCoreCyclesMax = focExecCoreCycles;
    }
}

/* ========================================================================== */
/* PWM / ADC control-loop synchronization                                     */
/* ========================================================================== */

void ControlSync_Init(void)
{
    DMA2->LIFCR =
        (1U << 0) |
        (1U << 2) |
        (1U << 3) |
        (1U << 4) |
        (1U << 5);

    DMA2->S[0].CR |= (1U << 4);
    NVIC->ISER[1] |= (1U << 24);

    TIM1->SR &= ~(1U << 0);
    TIM1->DIER |= (1U << 0);
    NVIC->ISER[0] |= (1U << 25);
}

/* ========================================================================== */
/* Program flow                                                               */
/* ========================================================================== */

int main(void)
{
    Clock_Init();

    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0U;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    GPIO_Init();
    Timer_Init();

    TIM1->BDTR |= (1U << 11);

    DMA2_Init();
    ADC_Init();
    Int_Init();

    const uint32_t tim1NPolarity =
        (1U << 3) | (1U << 7) | (1U << 11);

    TIM1->CCER = tim1NPolarity;
    TIM1->CCR1 = 0;
    TIM1->CCR2 = 0;
    TIM1->CCR3 = 0;

    CurrentHallState =
        (((GPIOB->IDR & 1U) << 2) |
         (((GPIOB->IDR >> 1) & 1U) << 1) |
         ((GPIOB->IDR >> 2) & 1U));

    for (int i = 0; i < 100; i++)
    {
        DMA2->LIFCR = (1U << 5);
        ADC1->CR2 |= (1U << 30);

        while ((DMA2->LISR & (1U << 5)) == 0U);

        DMA2->LIFCR = (1U << 5);

        offsetVoltA +=
            (3.0f * (float)DMABuffer[0]) / 4095.0f;

        offsetVoltB +=
            (3.0f * (float)DMABuffer[1]) / 4095.0f;
    }

    offsetVoltA /= 100.0f;
    offsetVoltB /= 100.0f;

    ControlSync_Init();

    uint32_t sixStepStartTick = TIM5->CNT;

    while (1)
    {
        rawIDR = GPIOB->IDR;

        int hallValue =
            (((rawIDR & 1U) << 2) |
             (((rawIDR >> 1) & 1U) << 1) |
             ((rawIDR >> 2) & 1U));

        if ((hallValue == 0) || (hallValue == 7))
        {
            TIM1->CCER = tim1NPolarity;
            TIM1->CCR1 = 0;
            TIM1->CCR2 = 0;
            TIM1->CCR3 = 0;
            lastHallValue = 0;
            continue;
        }

        if (focEnabled == 0U)
        {
            if (hallValue != lastHallValue)
            {
                TIM1->CCER = tim1NPolarity;
                TIM1->CCR1 = 0;
                TIM1->CCR2 = 0;
                TIM1->CCR3 = 0;
                TIM1->EGR = (1U << 0);

                for (volatile int i = 0; i < 200; i++);

                if (hallValue == 4)
                {
                    TIM1->CCR1 = 200;
                    TIM1->CCR2 = TIM1->ARR;
                    TIM1->CCR3 = 0;
                    TIM1->EGR = (1U << 0);
                    TIM1->CCER =
                        tim1NPolarity |
                        (1U << 0) |
                        (1U << 6);
                }
                else if (hallValue == 5)
                {
                    TIM1->CCR1 = 200;
                    TIM1->CCR2 = 0;
                    TIM1->CCR3 = TIM1->ARR;
                    TIM1->EGR = (1U << 0);
                    TIM1->CCER =
                        tim1NPolarity |
                        (1U << 0) |
                        (1U << 10);
                }
                else if (hallValue == 1)
                {
                    TIM1->CCR1 = 0;
                    TIM1->CCR2 = TIM1->ARR;
                    TIM1->CCR3 = 200;
                    TIM1->EGR = (1U << 0);
                    TIM1->CCER =
                        tim1NPolarity |
                        (1U << 8) |
                        (1U << 6);
                }
                else if (hallValue == 3)
                {
                    TIM1->CCR1 = TIM1->ARR;
                    TIM1->CCR2 = 200;
                    TIM1->CCR3 = 0;
                    TIM1->EGR = (1U << 0);
                    TIM1->CCER =
                        tim1NPolarity |
                        (1U << 4) |
                        (1U << 2);
                }
                else if (hallValue == 2)
                {
                    TIM1->CCR1 = 0;
                    TIM1->CCR2 = 200;
                    TIM1->CCR3 = TIM1->ARR;
                    TIM1->EGR = (1U << 0);
                    TIM1->CCER =
                        tim1NPolarity |
                        (1U << 4) |
                        (1U << 10);
                }
                else if (hallValue == 6)
                {
                    TIM1->CCR1 = TIM1->ARR;
                    TIM1->CCR2 = 0;
                    TIM1->CCR3 = 200;
                    TIM1->EGR = (1U << 0);
                    TIM1->CCER =
                        tim1NPolarity |
                        (1U << 8) |
                        (1U << 2);
                }

                lastHallValue = hallValue;
            }

            if (((uint32_t)(TIM5->CNT - sixStepStartTick) >= SIX_STEP_TIME_TICKS) &&
                (speedValid != 0U) &&
                (edgeSampleIndex >= 12U) &&
                (hallValue == 5))
            {
                TIM1->CCER = tim1NPolarity;
                TIM1->CCR1 = TIM1->ARR / 2U;
                TIM1->CCR2 = TIM1->ARR / 2U;
                TIM1->CCR3 = TIM1->ARR / 2U;
                TIM1->EGR = (1U << 0);

                TIM1->CCER =
                    tim1NPolarity |
                    (1U << 0) |
                    (1U << 2) |
                    (1U << 4) |
                    (1U << 6) |
                    (1U << 8) |
                    (1U << 10);

                focEnabled = 1U;
            }
        }
    }
}

void Clock_Init(void)
{
    /* 160 MHz SYSCLK from 16 MHz HSI.
       APB1 = 40 MHz, APB2 = 80 MHz. */

    RCC->APB1ENR |= (1U << 28);       // PWR clock
    PWR_CR |= (3U << 14);             // Voltage scale 1

    FLASH_ACR =
        (5U << 0) |                   // 5 wait states
        (1U << 8) |                   // Prefetch
        (1U << 9) |                   // I-cache
        (1U << 10);                   // D-cache

    RCC->CR |= (1U << 0);             // HSI ON
    while ((RCC->CR & (1U << 1)) == 0U);

    RCC->CR &= ~(1U << 24);           // PLL OFF
    while ((RCC->CR & (1U << 25)) != 0U);

    /* HSI 16 MHz / 8 * 160 / 2 = 160 MHz */
    RCC->PLLCFGR =
        (8U << 0) |                   // PLLM
        (160U << 6) |                 // PLLN
        (0U << 16) |                  // PLLP = 2
        (7U << 24);                   // PLLQ

    RCC->CFGR &= ~((0xFU << 4) | (7U << 10) | (7U << 13));
    RCC->CFGR |=
        (5U << 10) |                  // APB1 /4
        (4U << 13);                   // APB2 /2

    RCC->CR |= (1U << 24);            // PLL ON
    while ((RCC->CR & (1U << 25)) == 0U);

    RCC->CFGR &= ~(3U << 0);
    RCC->CFGR |= (2U << 0);            // SYSCLK = PLL
    while (((RCC->CFGR >> 2) & 3U) != 2U);

    /* ADC clock = APB2 / 4 = 20 MHz */
    ADC_CCR &= ~(3U << 16);
    ADC_CCR |=  (1U << 16);
}

void GPIO_Init(void){

	RCC->AHB1ENR |= (1 << 1); //GPIOB clock enable
	RCC->AHB1ENR |= (1 << 2); //GPIOC clock enable
	RCC->AHB1ENR |= (1 << 4); //GPIOE clock enable

	GPIOB->MODER &= ~(3 << 0); //Hall A input confg.
	GPIOB->MODER &= ~(3 << 2); //Hall B input confg.
	GPIOB->MODER &= ~(3 << 4); //Hall C input confg.

	GPIOB->MODER |= (1 << 20); //low side output 10
	GPIOB->MODER |= (1 << 22); //low side output 11
	GPIOB->MODER |= (1 << 24); //low side output 12

	GPIOB->MODER |= (2 << 6); //enable af for PB3
	GPIOB->AFRL  |= (1 << 12); //to enable tim2 pwm output

	GPIOB->MODER |= (2 << 8); //enable af for PB4
	GPIOB->AFRL  |= (2 << 16); //to enable tim2 pwm output

	GPIOB->MODER |= (2 << 12); //enable af for PB6
	GPIOB->AFRL  |= (2 << 24); //to enable tim2 pwm output

	GPIOC->MODER |= (3 << 0); //enable pin C0 analog mode for the ADC1
	GPIOC->MODER |= (3 << 2); //enable pin C1 analog mode for the ADC1

	/************** 3-phase PWM **********************************/

	GPIOE->MODER |= (2 << 16); //low side output 8
	GPIOE->AFRH  |= (1 << 0); //to enable tim1 pwm output
	GPIOE->MODER |= (2 << 20); //low side output 10
	GPIOE->AFRH  |= (1 << 8); //to enable tim1 pwm output
	GPIOE->MODER |= (2 << 24); //low side output 12
	GPIOE->AFRH  |= (1 << 16); //to enable tim1 pwm output

	GPIOE->MODER |= (2 << 18); //enable af for PE9
	GPIOE->AFRH  |= (1 << 4); //to enable tim1 pwm output
	GPIOE->MODER |= (2 << 22); //enable af for PE11
	GPIOE->AFRH  |= (1 << 12); //to enable tim1 pwm output
	GPIOE->MODER |= (2 << 26); //enable af for PE13
	GPIOE->AFRH  |= (1 << 20); //to enable tim1 pwm output

}

void Timer_Init(void){

	RCC->APB1ENR |= (15 << 0);
	
	/************** TIM5 ***********************************/
	TIM5->ARR = 0xFFFFFFFF;
	TIM5->PSC = 4;

	//TIM4->CCMR1 |= (6 << 4);
	//TIM4->CCMR1 |= (1 << 3);
	TIM5->CR1   |= (1 << 7);
	//TIM4->EGR   |= (1 << 0);
	//TIM4->CCER  |= (1 << 0);
	TIM5->CR1   |= (1 << 0);

	/************** TIM5 ***********************************/
	/************** TIM1 ***********************************/

	RCC->APB2ENR |= (1 << 0);

	TIM1->ARR = 0x190;
	TIM1->PSC = 9;
	TIM1->RCR = 0;
	TIM1->CR1 |= (1 << 5); //CMS
	TIM1->CR1   |= (1 << 7); //ARPE

	TIM1->CCR1 = 0;          // Duty cycle for          TIM1_CH1, about 50%
    TIM1->CCR2 = 0;          // Duty cycle for          TIM1_CH2, about 50%
    TIM1->CCR3 = 0;          // Duty cycle for          TIM1_CH3, about 50%

	TIM1->CCMR1 |= (6 << 4); //OC1M
	TIM1->CCMR1 |= (6 << 12); //OC2M
	TIM1->CCMR2 |= (6 << 4); //OC3M

	TIM1->CCMR1 |= (1 << 3); //OC1PE
	TIM1->CCMR1 |= (1 << 11); //OC2PE
	TIM1->CCMR2 |= (1 << 3); //OC3PE

    TIM1->CCER  |= (1 << 3); //CC1NP
    TIM1->CCER  |= (1 << 7); //CC2NP
    TIM1->CCER  |= (1 << 11); //CC3NP

    /*TIM1->CCER  |= (1 << 0); //CC1E
    TIM1->CCER  |= (1 << 4); //CC2E
    TIM1->CCER  |= (1 << 8); //CC3E

    TIM1->CCER  |= (1 << 2); //CC1NE
    TIM1->CCER  |= (1 << 6); //CC2NE
    TIM1->CCER  |= (1 << 10); //CC3NE*/

    TIM1->BDTR |= (16 << 0); //Dead time configuration 
    TIM1->BDTR |= (1 << 15); //MOE

	TIM1->EGR   |= (1 << 0);
	
	TIM1->CR1   |= (1 << 0); //CEN

}

void ADC_Init(void){

	RCC->APB2ENR |= (1 << 8); //Enable ADC1 clock

	ADC1->SQR3 |= (10 << 0);
	ADC1->SQR3 |= (11 << 5);
	ADC1->SQR1 |= (1 << 20);

	ADC1->SMPR1 &= ~((7U << 0) | (7U << 3));

	ADC1->SMPR1 |= (1U << 0);   // Channel 10: 15 cycles
	ADC1->SMPR1 |= (1U << 3);   // Channel 11: 15 cycles

	//ADC2->SQR3 |= (3 << 0);

	//ADC1->CR2 |= (1 << 0); //Enable the ADC
	ADC1->CR2 |= (1 << 8); //Enable dma
	ADC1->CR2 |= (1 << 9); //Enable dma continuous

	//ADC1->CR1 |= (1 << 5); //Enable Interrupt
	ADC1->CR1 |= (1 << 8); //Enable scan mode
	ADC1->CR2 &= ~(1U << 1);
	ADC1->CR2 &= ~(1U << 10);   // EOC after complete sequence//EOC Selection setting
	ADC1->CR2 |= (1U << 0);     // ADON

	for (volatile uint32_t i = 0; i < 1000U; i++);


}

void Int_Init(void){

	RCC->APB2ENR |= (1 << 14);

	/********* EXTI Configuration ***********/

	EXTI->IMR |= (1 << 0);
	EXTI->IMR |= (1 << 1);
	EXTI->IMR |= (1 << 2);

	EXTI->RTSR |= (1 << 0);
	EXTI->RTSR |= (1 << 1);
	EXTI->RTSR |= (1 << 2);

	EXTI->FTSR |= (1 << 0);
	EXTI->FTSR |= (1 << 1);
	EXTI->FTSR |= (1 << 2);
	
	/********* EXTI Configuration ***********/

	SYSCFG->EXTICR1 |= (1 << 0);
	SYSCFG->EXTICR1 |= (1 << 4);
	SYSCFG->EXTICR1 |= (1 << 8);

	/********* NVIC Configuration ***********/

	NVIC->ISER[0] |= (1 << 6);
	NVIC->ISER[0] |= (1 << 7);
	NVIC->ISER[0] |= (1 << 8);

}

void DMA2_Init(void){

	RCC->AHB1ENR |= (1 << 22);

	DMA2->S[0].CR &= ~(1 << 0);

	while((DMA2->S[0].CR) & 1);

	DMA2->LIFCR = (1 << 0);
	DMA2->LIFCR = (1 << 2);
	DMA2->LIFCR = (1 << 3);
	DMA2->LIFCR = (1 << 4);
	DMA2->LIFCR = (1 << 5);

	DMA2->S[0].PAR = 0x4001204C;
	DMA2->S[0].M0AR = (uint32_t) &DMABuffer[0];
	DMA2->S[0].NDTR = 2;

	DMA2->S[0].CR &= ~(7 << 25); 
	DMA2->S[0].CR &= ~(3 << 6); 
	DMA2->S[0].CR |= (1 << 10);
    DMA2->S[0].CR |= (1 << 11);
    DMA2->S[0].CR |= (1 << 13);
    DMA2->S[0].CR |= (1 << 8);

    DMA2->S[0].CR |= (1 << 0);

}

/* ========================================================================== */
/* Interrupt handlers and hardware integration                                */
/* ========================================================================== */

void TIM1_UP_TIM10_IRQHandler(void)
{
    if ((TIM1->SR & (1U << 0)) != 0U)
    {
        TIM1->SR &= ~(1U << 0);

        if ((TIM1->CR1 & (1U << 4)) != 0U)
        {
            focPwmCounter++;

            if (focPwmCounter >= FOC_PWM_DIVIDER)
            {
                focPwmCounter = 0U;

                if (adcBusy == 0U)
                {
                    adcBusy = 1U;
                    ADC1->CR2 |= (1U << 30);
                }
            }
        }
    }
}

void DMA2_Stream0_IRQHandler(void)
{
    if ((DMA2->LISR & (1U << 5)) != 0U)
    {
        DMA2->LIFCR = (1U << 5);
        adcBusy = 0U;

        if (focEnabled != 0U)
        {
            FOC_ControlStep();
        }
    }
}

uint8_t ReadHallState(void)
{
    uint32_t idr = GPIOB->IDR;

    return (uint8_t)(
        ((idr & 1U) << 2) |
        (((idr >> 1) & 1U) << 1) |
        ((idr >> 2) & 1U)
    );
}

float HallElectricalAngle(uint8_t hallState)
{
    if (hallState == 5U) return 0.0f;
    if (hallState == 4U) return 60.0f;
    if (hallState == 6U) return 120.0f;
    if (hallState == 2U) return 180.0f;
    if (hallState == 3U) return 240.0f;
    if (hallState == 1U) return 300.0f;

    return 0.0f;
}

void SaveEdgeSample(uint8_t hallState)
{
    if (edgeSampleIndex >= EDGE_SAMPLE_COUNT)
        return;

    uint32_t index = edgeSampleIndex;

    dtSamples[index] = dt;
    speedSamples[index] = speedsec;
    hallSamples[index] = hallState;

    edgeSampleIndex++;
}

void EXTI0_IRQHandler(void)
{
    CurrentTimeStamp = TIM5->CNT;
    dt = CurrentTimeStamp - lastTimeStamp;

    uint8_t hallState = ReadHallState();

    if (firstInt == 0)
    {
        baseAngele = (int)HallElectricalAngle(hallState);
        firstInt = 1;
    }
    else
    {
        speedsec = (60.0f * TIM5_TICK_HZ) / (float)dt;
        baseAngele = (int)HallElectricalAngle(hallState);
        speedValid = 1U;

        SaveEdgeSample(hallState);
    }

    lastTimeStamp = CurrentTimeStamp;
    EXTI->PR = (1U << 0);
}

void EXTI1_IRQHandler(void)
{
    CurrentTimeStamp = TIM5->CNT;
    dt = CurrentTimeStamp - lastTimeStamp;

    uint8_t hallState = ReadHallState();

    if (firstInt == 0)
    {
        baseAngele = (int)HallElectricalAngle(hallState);
        firstInt = 1;
    }
    else
    {
        speedsec = (60.0f * TIM5_TICK_HZ) / (float)dt;
        baseAngele = (int)HallElectricalAngle(hallState);
        speedValid = 1U;

        SaveEdgeSample(hallState);
    }

    lastTimeStamp = CurrentTimeStamp;
    EXTI->PR = (1U << 1);
}

void EXTI2_IRQHandler(void)
{
    CurrentTimeStamp = TIM5->CNT;
    dt = CurrentTimeStamp - lastTimeStamp;

    uint8_t hallState = ReadHallState();

    if (firstInt == 0)
    {
        baseAngele = (int)HallElectricalAngle(hallState);
        firstInt = 1;
    }
    else
    {
        speedsec = (60.0f * TIM5_TICK_HZ) / (float)dt;
        baseAngele = (int)HallElectricalAngle(hallState);
        speedValid = 1U;

        SaveEdgeSample(hallState);
    }

    lastTimeStamp = CurrentTimeStamp;
    EXTI->PR = (1U << 2);

}
