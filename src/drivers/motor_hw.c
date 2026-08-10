#include "motor_hw.h"

extern volatile uint16_t DMABuffer[2];

void Clock_Init(void);
void GPIO_Init(void);
void Timer_Init(void);
void ADC_Init(void);
void Int_Init(void);
void DMA2_Init(void);
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

typedef struct
{
    float Ialpha;
    float Ibeta;
}

void GPIO_Init(void);
void Timer_Init(void);
void ADC_Init(void);
void Int_Init(void);
void DMA2_Init(void);
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

typedef struct
{
    float Ialpha;
    float Ibeta;
}

void Timer_Init(void);
void ADC_Init(void);
void Int_Init(void);
void DMA2_Init(void);
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

typedef struct
{
    float Ialpha;
    float Ibeta;
}

void ADC_Init(void);
void Int_Init(void);
void DMA2_Init(void);
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

typedef struct
{
    float Ialpha;
    float Ibeta;
}

void Int_Init(void);
void DMA2_Init(void);
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

typedef struct
{
    float Ialpha;
    float Ibeta;
}

void DMA2_Init(void);
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

typedef struct
{
    float Ialpha;
    float Ibeta;
}
