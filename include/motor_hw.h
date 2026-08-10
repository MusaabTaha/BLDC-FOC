#ifndef MOTOR_HW_H
#define MOTOR_HW_H

#include <stdint.h>

#define GPIOB_BASE 	0x40020400U
#define GPIOC_BASE  0x40020800U
#define GPIOE_BASE  0x40021000U
#define RCC_BASE    0x40023800U
#define TIM1_BASE 	0x40010000U
#define TIM2_BASE 	0x40000000U
#define TIM3_BASE 	0x40000400U
#define TIM4_BASE 	0x40000800U
#define TIM5_BASE 	0x40000C00U
#define EXTI_BASE 	0x40013C00U
#define NVIC_BASE   0xE000E100U
#define ADC1_BASE   0x40012000U
#define ADC2_BASE   0x40012100U
#define DMA2_BASE   0x40026400U
#define SYSCFG_BASE 0x40013800U
#define PWR_BASE     0x40007000U
#define FLASH_BASE   0x40023C00U
#define ADC_COMMON_BASE 0x40012300U

typedef struct {

	volatile uint32_t MODER;
	volatile uint32_t OTYPER;
	volatile uint32_t OSPEEDR;
	volatile uint32_t PUPDR;
	volatile uint32_t IDR;
	volatile uint32_t ODR;
	volatile uint32_t BSRR;
	volatile uint32_t LCKR;
	volatile uint32_t AFRL;
	volatile uint32_t AFRH;

} GPIO_t;


typedef struct
{
    volatile uint32_t CR;          // 0x00
    volatile uint32_t PLLCFGR;     // 0x04
    volatile uint32_t CFGR;        // 0x08
    volatile uint32_t CIR;         // 0x0C

    volatile uint32_t AHB1RSTR;    // 0x10
    volatile uint32_t AHB2RSTR;    // 0x14
    volatile uint32_t AHB3RSTR;    // 0x18
    volatile uint32_t RESERVED0;   // 0x1C

    volatile uint32_t APB1RSTR;    // 0x20
    volatile uint32_t APB2RSTR;    // 0x24
    volatile uint32_t RESERVED1[2];// 0x28, 0x2C

    volatile uint32_t AHB1ENR;     // 0x30
    volatile uint32_t AHB2ENR;     // 0x34
    volatile uint32_t AHB3ENR;     // 0x38
    volatile uint32_t RESERVED2;   // 0x3C

    volatile uint32_t APB1ENR;     // 0x40
    volatile uint32_t APB2ENR;     // 0x44
    volatile uint32_t RESERVED3[2];// 0x48, 0x4C

    volatile uint32_t AHB1LPENR;   // 0x50
    volatile uint32_t AHB2LPENR;   // 0x54
    volatile uint32_t AHB3LPENR;   // 0x58
    volatile uint32_t RESERVED4;   // 0x5C

    volatile uint32_t APB1LPENR;   // 0x60
    volatile uint32_t APB2LPENR;   // 0x64
    volatile uint32_t RESERVED5[2];// 0x68, 0x6C

    volatile uint32_t BDCR;        // 0x70
    volatile uint32_t CSR;         // 0x74
    volatile uint32_t RESERVED6[2];// 0x78, 0x7C

    volatile uint32_t SSCGR;       // 0x80
    volatile uint32_t PLLI2SCFGR;  // 0x84
    volatile uint32_t PLLSAICFGR;  // 0x88
    volatile uint32_t DCKCFGR;     // 0x8C

} RCC_t;

typedef struct {

	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t SMCR;
	volatile uint32_t DIER;
	volatile uint32_t SR;
	volatile uint32_t EGR;
	volatile uint32_t CCMR1;
	volatile uint32_t CCMR2;
	volatile uint32_t CCER;
	volatile uint32_t CNT;
	volatile uint32_t PSC;
	volatile uint32_t ARR;
	volatile uint32_t RESERVED;
	volatile uint32_t CCR1;
	volatile uint32_t CCR2;
	volatile uint32_t CCR3;
	volatile uint32_t CCR4;
	volatile uint32_t RESERVED1;
	volatile uint32_t DCR;
	volatile uint32_t DMAR;
	volatile uint32_t OR;

} TIM_t;

typedef struct
{
    volatile uint32_t CR1;      // 0x00
    volatile uint32_t CR2;      // 0x04
    volatile uint32_t SMCR;     // 0x08
    volatile uint32_t DIER;     // 0x0C
    volatile uint32_t SR;       // 0x10
    volatile uint32_t EGR;      // 0x14
    volatile uint32_t CCMR1;    // 0x18
    volatile uint32_t CCMR2;    // 0x1C
    volatile uint32_t CCER;     // 0x20
    volatile uint32_t CNT;      // 0x24
    volatile uint32_t PSC;      // 0x28
    volatile uint32_t ARR;      // 0x2C
    volatile uint32_t RCR;      // 0x30
    volatile uint32_t CCR1;     // 0x34
    volatile uint32_t CCR2;     // 0x38
    volatile uint32_t CCR3;     // 0x3C
    volatile uint32_t CCR4;     // 0x40
    volatile uint32_t BDTR;     // 0x44
    volatile uint32_t DCR;      // 0x48
    volatile uint32_t DMAR;     // 0x4C
    volatile uint32_t OR;       // 0x50

} TIM1_t;

typedef struct {

	volatile uint32_t IMR;
	volatile uint32_t EMR;
	volatile uint32_t RTSR;
	volatile uint32_t FTSR;
	volatile uint32_t SWIER;
	volatile uint32_t PR;

} EXTI_t;

typedef struct
{
    volatile uint32_t ISER[8];        // 0x000 - 0x01C
    uint32_t RESERVED0[24];           // 0x020 - 0x07C

    volatile uint32_t ICER[8];        // 0x080 - 0x09C
    uint32_t RESERVED1[24];           // 0x0A0 - 0x0FC

    volatile uint32_t ISPR[8];        // 0x100 - 0x11C
    uint32_t RESERVED2[24];           // 0x120 - 0x17C

    volatile uint32_t ICPR[8];        // 0x180 - 0x19C
    uint32_t RESERVED3[24];           // 0x1A0 - 0x1FC

    volatile uint32_t IABR[8];        // 0x200 - 0x21C
    uint32_t RESERVED4[56];           // 0x220 - 0x2FC

    volatile uint8_t IPR[240];        // 0x300 - 0x3EF
    uint32_t RESERVED5[644];          // 0x3F0 - 0xDFF

    volatile uint32_t STIR;           // 0xE00

} NVIC_t;

typedef struct
{

	volatile uint32_t MEMRMP;
	volatile uint32_t PMC;
	volatile uint32_t EXTICR1;
	volatile uint32_t EXTICR2;
	volatile uint32_t EXTICR3;
	volatile uint32_t EXTICR4;
	volatile uint32_t CMPCR;

} SYSCFG_t;

typedef struct
{

    volatile uint32_t SR;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMPR1;
    volatile uint32_t SMPR2;
    volatile uint32_t JOFR1;
    volatile uint32_t JOFR2;
    volatile uint32_t JOFR3;
    volatile uint32_t JOFR4;
    volatile uint32_t HTR;
    volatile uint32_t LTR;
    volatile uint32_t SQR1;
    volatile uint32_t SQR2;
    volatile uint32_t SQR3;
    volatile uint32_t JSQR;
    volatile uint32_t JDR1;
    volatile uint32_t JDR2;
    volatile uint32_t JDR3;
    volatile uint32_t JDR4;
    volatile uint32_t DR;

} ADC_t;

typedef struct
{
    volatile uint32_t CR;
    volatile uint32_t NDTR;
    volatile uint32_t PAR;
    volatile uint32_t M0AR;
    volatile uint32_t M1AR;
    volatile uint32_t FCR;

} DMA_Stream_t;

typedef struct
{
    volatile uint32_t LISR;     // 0x00
    volatile uint32_t HISR;     // 0x04
    volatile uint32_t LIFCR;    // 0x08
    volatile uint32_t HIFCR;    // 0x0C

    DMA_Stream_t S[8];          // Stream 0 to Stream 7

} DMA_t;

#define GPIOB  ((GPIO_t *) 	   GPIOB_BASE)
#define GPIOC  ((GPIO_t *) 	   GPIOC_BASE)
#define GPIOE  ((GPIO_t *) 	   GPIOE_BASE)
#define RCC    ((RCC_t *) 	  	 RCC_BASE)
#define TIM1   ((TIM1_t *) 	 	TIM1_BASE)
#define TIM2   ((TIM_t *) 	 	TIM2_BASE)
#define TIM3   ((TIM_t *) 	 	TIM3_BASE)
#define TIM4   ((TIM_t *) 	 	TIM4_BASE)
#define TIM5   ((TIM_t *) 	 	TIM5_BASE)
#define EXTI   ((EXTI_t *) 	 	EXTI_BASE)
#define NVIC   ((NVIC_t *) 	    NVIC_BASE)
#define SYSCFG ((SYSCFG_t *)  SYSCFG_BASE)
#define ADC1   ((ADC_t *) 	    ADC1_BASE)
#define DMA2   ((DMA_t *) 	    DMA2_BASE)

#define PWR_CR      (*(volatile uint32_t *)(PWR_BASE + 0x00U))
#define PWR_CSR     (*(volatile uint32_t *)(PWR_BASE + 0x04U))
#define FLASH_ACR   (*(volatile uint32_t *)(FLASH_BASE + 0x00U))
#define ADC_CCR     (*(volatile uint32_t *)(ADC_COMMON_BASE + 0x04U))

#define DUTY 12

void Clock_Init(void);
void GPIO_Init(void);
void Timer_Init(void);
void ADC_Init(void);
void Int_Init(void);
void DMA2_Init(void);

#endif
