# ⚡ Bare-Metal STM32 FOC BLDC Motor Controller

## 🚀 Overview

This project implements a from-scratch bare-metal BLDC motor control system on an STM32F429 Discovery board with the X-NUCLEO-IHM08M1 three-phase inverter board.

The first milestone validated Hall-sensor-based six-step commutation and successfully spun the 42BLF01 BLDC motor using direct register-level programming. The next development phase focuses on building a professional field-oriented control architecture, starting with a Hall-based electrical angle subsystem before moving toward current control, Clarke/Park transformations, SVPWM, and later observer-based rotor-flux estimation.

The project is developed in C using VS Code and PlatformIO, without STM32 HAL, STM32 Motor Control SDK, or third-party motor-control libraries.
----
## 🧭 Development Workflow

This project follows a professional embedded development workflow inspired by the V-model, where requirements and design are defined before implementation, and each development layer is verified before moving to the next stage.
<img width="2779" height="880" alt="bldc_foc_development_workflow (1)" src="https://github.com/user-attachments/assets/e86db42f-1e31-446f-82a7-27998317dbe4" />
---

