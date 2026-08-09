# ⚡ Bare-Metal STM32 FOC BLDC Motor Controller

## 🚀 Overview

This project implements a from-scratch bare-metal BLDC motor control system on an STM32F429 Discovery board with the X-NUCLEO-IHM08M1 three-phase inverter board.

The first milestone validated Hall-sensor-based six-step commutation and successfully spun the 42BLF01 BLDC motor using direct register-level programming. The next development phase focuses on building a professional field-oriented control architecture, starting with a Hall-based electrical angle subsystem before moving toward current control, Clarke/Park transformations, SVPWM, and later observer-based rotor-flux estimation.

The project is developed in C using VS Code and PlatformIO, without STM32 HAL, STM32 Motor Control SDK, or third-party motor-control libraries.
## 🧭 Development Workflow

This project follows a professional embedded development workflow inspired by the **V-model**, where requirements and design are defined before implementation, and each development layer has a matching verification activity. The goal is to keep the firmware traceable from system requirements down to low-level drivers and back up through testing.

The first diagram shows the practical workflow used in this project, from requirements and prototype validation toward custom PCB preparation.

<img width="2779" height="880" alt="bldc_foc_development_workflow (2)" src="https://github.com/user-attachments/assets/96fd72d2-352c-40d5-a856-133593312012" />

The second diagram shows the general V-model principle used to connect design phases with their corresponding verification phases.

<img width="3976" height="2776" alt="F1_-_Traditional_V_Model original" src="https://github.com/user-attachments/assets/53c95245-8e87-4edd-aa42-9425ac15a8c4" />


<div align="center">

⚙️ STM32F429 Bare-Metal BLDC Motor Control

Hall Six-Step Startup → Hall-Sensored FOC → SVPWM

From-scratch BLDC motor control on STM32F429 Discovery + X-NUCLEO-IHM08M1 + ACT Motor 42BLF01

<br>





</div>

📌 Overview

This project implements a from-scratch bare-metal BLDC motor control system on an STM32F429 Discovery board with the X-NUCLEO-IHM08M1 three-phase inverter board.

The first milestone validated Hall-sensor-based six-step commutation and successfully spun the ACT Motor 42BLF01 using direct register-level programming.

The next development phase builds the complete Field-Oriented Control (FOC) chain:

Hall position
→ electrical angle
→ phase-current measurement
→ Clarke transform
→ Park transform
→ Id/Iq current control
→ inverse Park
→ SVPWM
→ TIM1 complementary PWM
→ inverter
→ motor

The project is developed in C using VS Code and PlatformIO, without STM32 HAL, STM32 Motor Control SDK, or third-party motor-control libraries.

🎯 Current objective: transition from the known-good Hall six-step startup to Hall-sensored FOC and keep the shaft rotating smoothly without rocking or losing torque.

🧭 Development Workflow

This project follows a professional embedded development workflow inspired by the V-model.

Requirements and design are defined before implementation, and every development layer has a corresponding validation activity.

flowchart LR
    A[📋 Requirements] --> B[⚙️ Technical Specifications]
    B --> C[🔌 Hardware Design]
    C --> D[🏗️ Firmware Architecture]
    D --> E[🧩 Firmware Design]
    E --> F[🔄 Program Flow]
    F --> G[💻 Implementation]
    G --> H[🧪 Testing & Validation]

The development itself is intentionally bottom-up:

Hardware bring-up
→ PWM
→ Hall sensing
→ six-step commutation
→ current sensing
→ electrical-angle estimation
→ Clarke/Park
→ current PI
→ inverse Park
→ SVPWM
→ closed-loop FOC

Each block is validated before the next block is trusted.

🔁 Validation philosophy

flowchart LR
    A[Implement one block] --> B[Measure]
    B --> C{Valid?}
    C -- Yes --> D[Continue]
    C -- No --> E[Modify only failed block]
    E --> B

This avoids changing multiple control blocks at once and makes each failure traceable.

📋 1. Requirements

The motor controller shall:

Drive a three-phase BLDC motor using bare-metal STM32 firmware

Generate complementary three-phase PWM with dead time

Read the three Hall sensors for rotor-position information

Measure two motor phase currents

Reconstruct the third phase current

Perform Clarke and Park transformations

Regulate Id and Iq

Perform inverse Park transformation

Generate three-phase PWM using SVPWM

Synchronize current sampling with PWM

Start reliably using Hall-based six-step commutation

Transfer from six-step control to FOC after valid Hall and speed information is available

Keep the complete control path directly observable in the debugger

✅ Main system requirement

The motor shall transition from the known-good six-step startup to Hall-sensored FOC and continue rotating smoothly in the same direction.

⚙️ 2. Technical Specifications

🧠 MCU

STM32F429 Discovery

Item

Configuration

Core

ARM Cortex-M4F

Language

C

Firmware

Bare-metal

Register access

Direct CMSIS/register programming

IDE

VS Code

Build system

PlatformIO

PWM timer

TIM1

Hall timing

TIM5

Current ADC

ADC1

Current DMA

DMA2

⚡ Power Stage

ST X-NUCLEO-IHM08M1

Three-phase inverter

Complementary high-side / low-side PWM

Dead-time control

3-shunt current-sensing configuration

Active-low low-side inputs

🌀 Motor

ACT Motor 42BLF01

Parameter

Value

Motor type

NEMA17 BLDC

Phases

3

Poles

8

Pole pairs

4

Rated voltage

24 VDC

Rated speed

4000 rpm

No-load speed

5000 rpm

Rated current

1.9 A

Peak current

5.7 A

Rated torque

0.063 N·m

Peak torque

0.18 N·m

Torque constant

0.035 N·m/A

Back-EMF constant

3.7 V/krpm

Phase-to-phase resistance

2.0 Ω ±10%

Rotor inertia

24 g·cm²

Motor wiring

Phase U = Yellow
Phase V = Green
Phase W = Blue

Hall U  = Yellow
Hall V  = Green
Hall W  = Blue
Hall +5V = Red
Hall GND = Black

🔌 3. Hardware Design

flowchart LR
    MOTOR[🌀 42BLF01 BLDC]

    subgraph MCU[STM32F429 Discovery]
        PWM[TIM1<br/>Complementary PWM]
        HALL[PB0 / PB1 / PB2<br/>Hall Inputs]
        ADC[ADC1 + DMA2<br/>Current Sampling]
        TIM5[TIM5<br/>Hall Timing]
    end

    subgraph POWER[X-NUCLEO-IHM08M1]
        INV[3-Phase Inverter]
        SHUNT[3-Shunt Current Sense]
    end

    PWM --> INV
    INV --> MOTOR
    MOTOR --> HALL
    HALL --> TIM5
    MOTOR --> SHUNT
    SHUNT --> ADC

PWM assignment

Phase U
PE9  → TIM1_CH1
PE8  → TIM1_CH1N

Phase V
PE11 → TIM1_CH2
PE10 → TIM1_CH2N

Phase W
PE13 → TIM1_CH3
PE12 → TIM1_CH3N

Hall inputs

PB0 → Hall U
PB1 → Hall V
PB2 → Hall W

Observed forward Hall sequence:

5 → 4 → 6 → 2 → 3 → 1 → 5

Current sensing

PC0 → ADC1_IN10 → Phase A current
PC1 → ADC1_IN11 → Phase B current

voltA = (3.0f * (float)DMABuffer[0]) / 4095.0f;
voltB = (3.0f * (float)DMABuffer[1]) / 4095.0f;

currA = (voltA - offsetVoltA) / (0.01f * 5.18f);
currB = (voltB - offsetVoltB) / (0.01f * 5.18f);
currC = -(currA + currB);

🏗️ 4. Firmware Architecture

The current bring-up firmware intentionally remains in one .c file.

This keeps register configuration, control logic, breakpoints, timing measurements, and six-step/FOC comparison in one place while the control chain is still being validated.

flowchart TB

    APP[🎛️ Motor-Control State Machine]

    APP --> SIX[Six-Step Startup]
    APP --> FOC[FOC Control]

    SIX --> HALL[Hall Position + Speed]
    SIX --> PWM[PWM Output]

    FOC --> CUR[Current Acquisition]
    FOC --> ANG[Electrical-Angle Estimator]

    CUR --> CLARKE[Clarke]
    CLARKE --> PARK[Park]
    ANG --> PARK

    PARK --> PI[Id / Iq PI]
    PI --> IPARK[Inverse Park]
    ANG --> IPARK

    IPARK --> SVPWM[SVPWM]
    SVPWM --> PWM

    ADC[ADC + DMA] --> CUR
    HALL --> ANG

🧩 5. Firmware Design

5.1 🔄 Six-Step Startup

The six-step implementation is the known-good baseline.

Hall

Commutation

4

U+ / V-

5

U+ / W-

1

W+ / V-

3

V+ / U-

2

V+ / W-

6

W+ / U-

For the current development build:

10 seconds six-step
→ valid Hall / speed
→ FOC takeover

The 10-second period is only used to make debugging and handover observation easier.

5.2 🧭 Hall Electrical Angle

Each Hall transition represents one 60° electrical sector.

The Hall-derived angle is already an electrical angle, therefore the 4 pole pairs are not applied again.

Hall sector angle
+
time interpolation inside the 60° sector
=
continuous electrical angle

5.3 📐 Clarke Transform

Ialpha = Ia;
Ibeta  = (Ia + 2.0f * Ib) / sqrtf(3.0f);

Third current reconstruction:

Ic = -(Ia + Ib);

5.4 🧭 Park Transform

Id = Ialpha * cos(theta) + Ibeta * sin(theta);
Iq = -Ialpha * sin(theta) + Ibeta * cos(theta);

Initial current references:

Id_ref = 0.0f;
Iq_ref = small positive test current;

Iq is used for torque production while Id is initially commanded to zero.

5.5 🎛️ d/q Current Control

Two current controllers generate:

Vd
Vq

Target execution:

PWM frequency         = 20 kHz
Current-loop frequency = 20 kHz
Control period Ts      = 50 µs

PI gains are still under validation.

5.6 🔁 Inverse Park

Valpha = Vd * cos(theta) - Vq * sin(theta);
Vbeta  = Vd * sin(theta) + Vq * cos(theta);

5.7 🔺 SVPWM

Valpha / Vbeta
→ determine SVPWM sector
→ calculate phase on-times
→ CCR1 / CCR2 / CCR3

⚠️ Hall sector ≠ SVPWM sector

Hall sector describes rotor electrical position

SVPWM sector describes the commanded voltage vector

5.8 ⏱️ PWM-Synchronized Current Sampling

sequenceDiagram
    participant PWM as TIM1 PWM
    participant ADC as ADC1
    participant DMA as DMA2
    participant FOC as FOC Control
    participant CCR as TIM1 CCR1/2/3

    PWM->>ADC: Trigger current sample
    ADC->>DMA: IA + IB
    DMA->>FOC: Conversion complete
    FOC->>FOC: Clarke + Park
    FOC->>FOC: Id/Iq control
    FOC->>FOC: Inverse Park + SVPWM
    FOC->>CCR: Update duty cycles

🔄 6. Program Flow

flowchart TD

    A[🚀 Power-Up] --> B[Peripheral Initialization]
    B --> C[Current Offset Calibration]
    C --> D[Six-Step Commutation]
    D --> E[Read Hall Sensors]
    E --> F[Estimate Speed + Electrical Angle]

    F --> G{Startup elapsed<br/>and Hall valid?}

    G -- No --> D
    G -- Yes --> H[Enable all complementary PWM pairs]

    H --> I[FOC Enabled]

    I --> J[PWM-Synchronized ADC]
    J --> K[DMA Complete]
    K --> L[Clarke]
    L --> M[Park]
    M --> N[Id / Iq PI]
    N --> O[Inverse Park]
    O --> P[SVPWM]
    P --> Q[Update CCR1 / CCR2 / CCR3]
    Q --> J

💻 7. Implementation

The current project intentionally uses a minimal structure:

BLDC-FOC/
│
├── src/
│   └── main.c
│
├── platformio.ini
│
└── README.md

The goal is to validate the complete motor-control behavior first.

After the control chain is stable, the firmware can be separated into dedicated modules without changing the validated behavior.

Why one .c file during bring-up?

Easy breakpoint placement

Easy register inspection

Direct six-step / FOC comparison

Simple timing measurements

Easy rollback during debugging

No unnecessary abstraction before the control algorithm works

🧪 8. Testing & Validation

The project follows the V-model principle by matching each development block with a verification activity.

flowchart LR

    R[Requirements] -.-> ST[System Test]
    TS[Technical Specs] -.-> IT[Integration Test]
    ARCH[Firmware Architecture] -.-> SIT[Subsystem / Integration Test]
    DESIGN[Firmware Design] -.-> UT[Unit / Block Test]

    R --> TS --> ARCH --> DESIGN --> CODE[Implementation]
    CODE --> UT --> SIT --> IT --> ST

Unit / Block Validation

Hall states
Current acquisition
Current offset
Clarke
Park
PI
Inverse Park
SVPWM

Integration Validation

Hall + speed
Hall + electrical angle
ADC + DMA
Current sensing + Clarke/Park
Angle + inverse Park + SVPWM
FOC + PWM

System Validation

Six-step startup
→ FOC takeover
→ smooth shaft rotation
→ current tracking
→ load response

⏱️ Performance Testing

At 20 kHz PWM:

PWM period = 50 µs

Therefore, the complete current-control loop must finish within its available execution window.

Execution-time measurement is currently an active debugging item.

Rather than hiding timing problems by changing PI gains, the project treats real-time execution as a dedicated performance requirement.

📊 Development Status

Development Block

Status

GPIO / peripheral clocks

✅ Complete

TIM1 complementary PWM

✅ Complete

PWM dead time / polarity

✅ Complete

Hall sensor inputs

✅ Complete

Hall sequence validation

✅ Complete

Hall six-step commutation

✅ Stable

TIM5 Hall timing

✅ Complete

Speed estimation

✅ Complete

Electrical-angle interpolation

✅ Implemented

ADC phase-current acquisition

✅ Complete

DMA phase-current transfer

✅ Complete

Current offset calibration

✅ Complete

Clarke transform

✅ Complete

Park transform

✅ Complete

d/q PI structure

✅ Complete

Inverse Park

✅ Complete

SVPWM

✅ Complete

PWM-synchronized current sampling

✅ Implemented

Six-step → FOC transition

🧪 Under validation

20 kHz FOC execution timing

🧪 Under validation

Electrical-angle alignment

🧪 Under validation

Stable closed-loop FOC rotation

⏳ In progress

Final PI tuning

⏳ Pending

Protection / fault handling

⏳ Pending

🔬 Current Debugging Plan

The working six-step implementation remains the reference baseline.

1. Verify FOC execution time
2. Verify phase-current signs
3. Verify Hall electrical-angle direction
4. Verify Hall interpolation
5. Test fixed Vd = 0 / fixed Vq without PI
6. Validate electrical-angle offset
7. Validate Id / Iq signs
8. Close the current loops
9. Tune PI gains
10. Add final protection and limits

One block is modified and validated at a time.

📸 Hardware Setup

<!-- Replace this placeholder with the real hardware photo -->

<p align="center">
  <b>📷 Add STM32F429 + X-NUCLEO-IHM08M1 + 42BLF01 hardware setup photo here</b>
</p>

![Hardware Setup](docs/images/hardware-setup.jpg)

🗺️ Project Roadmap

✅ Hardware bring-up
✅ Complementary PWM
✅ Hall six-step commutation
✅ Hall speed measurement
✅ Current sensing + DMA
✅ Current offset calibration
✅ Clarke / Park
✅ d/q PI implementation
✅ Inverse Park
✅ SVPWM
🧪 Six-step → FOC handover
🧪 FOC execution-time optimization
🧪 Hall electrical-angle alignment
⏳ Stable closed-loop FOC
⏳ PI tuning
⏳ Current / voltage protection
⏳ Speed control loop
⏳ Observer-based rotor-flux estimation
⏳ Dedicated custom motor-controller PCB

🎯 Next Milestone

Transition from the known-good Hall six-step startup to Hall-sensored FOC while maintaining smooth continuous rotation.

After this milestone:

FOC stability
→ current-loop tuning
→ current/voltage limits
→ fault handling
→ speed loop
→ observer-based angle estimation
→ custom PCB

🧰 Development Stack

<div align="center">





🧠 MCU

STM32F429 Discovery

⚡ Power stage

X-NUCLEO-IHM08M1

🌀 Motor

ACT Motor 42BLF01

🧲 Rotor sensing

Hall sensors

🎛️ Control

Six-step + FOC

🔺 Modulation

SVPWM

📏 Current sensing

3-shunt / 2 ADC channels

💻 Language

C

🔧 Framework

CMSIS / direct registers

🧱 Build

PlatformIO

🖥️ IDE

VS Code

</div>

💡 Project Philosophy

This project does not use a prebuilt motor-control library.

The objective is to understand, implement, measure, and validate the complete BLDC control chain from the MCU registers to the rotating motor:

GPIO
→ timers
→ ADC / DMA
→ current reconstruction
→ rotor electrical angle
→ Clarke / Park
→ d/q control
→ inverse Park
→ SVPWM
→ inverter
→ BLDC motor

The project is therefore both:

a working BLDC motor-control platform

a practical bare-metal embedded firmware development project

a step-by-step FOC implementation

a real-time control and debugging exercise

<div align="center">

⚙️ Built from registers up. Validated block by block.

STM32F429 • Bare-Metal C • BLDC • Hall Sensors • FOC • SVPWM

</div>

The current prototype is used to validate hardware behavior, peripheral configuration, timing, safety logic, and control algorithms before migrating the design toward a dedicated custom BLDC motor-controller PCB.
