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

The current prototype is used to validate hardware behavior, peripheral configuration, timing, safety logic, and control algorithms before migrating the design toward a dedicated custom BLDC motor-controller PCB.
