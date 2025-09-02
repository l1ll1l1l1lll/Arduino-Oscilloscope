# Arduino-Oscilloscope

A simple Arduino-based oscilloscope capable of measuring voltage signals from **-10V to +10V**.

## Features
- Measurement range: **-10V ~ +10V**
- Arduino as the core sampling device
- Basic waveform visualization
- Great for beginners and small experiments
- Example circuit converts Arduino’s native **0–5V range** into **-10V to +10V**
- A check button is added at the top: when pressed, the displayed voltage should show **5V**

## Hardware Requirements
- Arduino board (e.g., Arduino Uno / Nano)
- Voltage divider and protection circuit (resistors, capacitors, etc. to keep input within safe range)
- 1.3 inch OLED Display
- 2 × 10 kΩ resistors
- 2 × 1 MΩ resistors
- *(Optional)* 1 × push button (for voltage check)

## Wiring
- OLED **SCK → A5**
- OLED **SDA → A4**
- **A0** as the signal input (yellow wire in the picture)

## Software Requirements
- Arduino IDE
- Required libraries (depending on your display, e.g., U8g2lib)

![Oscilloscope Preview](IMG_6631.jpg)
