# ⚡ Bare-Metal STM32 FOC BLDC Motor Controller

## 🚀 Overview

This project implements a **from-scratch bare-metal BLDC motor control system** using an **STM32F429 Discovery** board and the **X-NUCLEO-IHM08M1** three-phase inverter board.

The first milestone successfully validated **Hall-sensor-based six-step commutation** and spun the **42BLF01 BLDC motor** using direct register-level programming.

The next development phase focuses on building a professional **Field-Oriented Control (FOC)** architecture, starting with a **Hall-based electrical angle subsystem**, then moving toward current control, Clarke/Park transformations, SVPWM, and later observer-based rotor-flux estimation.

---
