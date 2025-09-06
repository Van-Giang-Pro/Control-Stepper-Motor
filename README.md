# Arduino Mega Stepper Nema 17 Control With TB6600 Driver

## Setup Instructions
**Connect the Hardware**:
   - Connect the TB6600 driver to the Arduino Mega:
     - **ENA-** → Pin 4
     - **ENA+** → 5V
     - **DIR-** → Pin 3
     - **DIR+** → 5V
     - **PUL-** → Pin 2
     - **PUL+** → 5V
   - Connect the Nema 17 motor to the TB6600 driver (A+, A-, B+, B-).
   - Supply a suitable power source to the TB6600 driver (VCC and GND).

**Configure the TB6600 DIP Switches**:
   - Set the DIP switches on the TB6600 driver as follows:
     - S1 : OFF
     - S2 : OFF
     - S3 : ON
     - S4 : ON
     - S5 : ON
     - S6 : ON

## Identifying Nema 17 Motor Coil Pairs (A+, A-, B+, B-)
- The Nema 17 motor has two independent coils, each with two ends (A+, A- and B+, B-).
- Measure the resistance between each pair of wires:
  - If the resistance between two wires is low (typically a few Ohms, e.g., 2-5Ω), those wires belong to the same coil (e.g., A+ and A-, or B+ and B-).
  - If there is no resistance (open circuit, infinite resistance), those wires belong to different coils.
