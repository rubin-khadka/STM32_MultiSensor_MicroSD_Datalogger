# STM32 Multi Sensor MicroSD FatFs Datalogger Project

![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)
![STM32](https://img.shields.io/badge/STM32-F103C8T6-blue)
![CubeIDE](https://img.shields.io/badge/IDE-STM32CubeIDE-darkblue)

## Video Demonstrations

## Hardware Components

| Component | Quantity | Description |
|-----------|----------|-------------|
| **STM32F103C8T6** | 1 | "Blue Pill" development board with 72MHz Cortex-M3 |
| **DHT11 Sensor** | 1 | Temperature & Humidity sensor (1-Wire protocol) | 
| **DS18B20** | 1 | Digital temperature sensor (1-Wire protocol, ±0.5°C accuracy) |
| **MPU6050** | 1 | 6-axis inertial measurement unit (accelerometer + gyroscope + temperature) |
| **LCD 16x2 with I2C** | 1 | Character display module with I2C backpack (PCF8574) |
| **Push Buttons** | 3 | Two-leg tactile switches for user input |
| **USB-to-Serial Converter** | 1 | CP2102 / CH340 / FTDI for UART communication and debugging |

### Pin Configuration

| Peripheral | Pin | Connection | Notes |
|------------|-----|------------|-------|
| **DHT11** | PB13 | DATA | Built-in 1kΩ pull-up |
| | 5V | VCC | Power |
| | GND | GND | Common ground |
| **DS18B20** | PB0 | DQ | 1-Wire data (module has built-in pull-up) |
| | 5V | VCC | Power |
| | GND | GND | Common ground |
| **MPU6050** | PB10 | SCL | I2C2 clock (shared with LCD) |
| | PB11 | SDA | I2C2 data (shared with LCD) |
| | 5V | VCC | Power (module includes voltage regulator) |
| | GND | GND | Common ground |
| **LCD 16x2 I2C** | PB10 | SCL | I2C2 clock (shared with MPU6050) |
| | PB11 | SDA | I2C2 data (shared with MPU6050) |
| | 5V | VCC | Power |
| | GND | GND | Common ground |
| **UART** | PA9 | TX to USB-Serial RX | 115200 baud, 8-N-1 |
| | PA10 | RX to USB-Serial TX | Optional for commands |
| **Button 1** | PA0 | Mode select | Input with internal pull-up |
| **Button 2** | PA1 | Save Data | Input with internal pull-up |
| **Button 3** | PA2 | Read Data | Input with internal pull-up |

The LCD display and MPU6050 share the same I2C bus (PB10/SCL, PB11/SDA) with different addresses:

| Device | I2C Address (7-bit) | 8-bit Write | 8-bit Read |
|--------|---------------------|-------------|------------|
| **MPU6050** | 0x68 | 0xD0 | 0xD1 |
| **LCD (PCF8574)** | 0x27 | 0x4E | 0x4F |

## Project Schematic

<img width="1448" height="617" alt="Schematic_diagram" src="https://github.com/user-attachments/assets/429ae531-0c56-496d-b5db-be08bd934584" />

*Schematic designed using EasyEDA*

## Related Projects 
- [STM32_MultiSensor_FlashStorage](https://github.com/rubin-khadka/STM32_MultiSensor_FlashStorage) - Similar project but with W25Q64 Flash storage instead of MicroSD card
- [STM32_DHT11_MPU6050_LCD](https://github.com/rubin-khadka/STM32_DHT11_MPU6050_LCD) - Sensor display project with DHT11 and MPU6050 on LCD and UART
- [STM32_DHT11_UART_BareMetal](https://github.com/rubin-khadka/STM32_DHT11_UART_BareMetal) - Basic DHT11 sensor reading with UART output using bare metal programming
- [STM32_DHT11_LCD16x2](https://github.com/rubin-khadka/STM32_DHT11_LCD16x2) - DHT11 sensor data displayed on 16x2 LCD character display using HAL

## Resources
- [STM32F103 Datasheet](https://www.st.com/resource/en/datasheet/stm32f103c8.pdf)
- [STM32F103 Reference Manual](https://www.st.com/resource/en/reference_manual/rm0008-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [DHT11 Sensor Datasheet](https://www.mouser.com/datasheet/2/758/DHT11-Technical-Data-Sheet-Translated-Version-1143054.pdf)
- [DS18B20 Temperature Sensor Datasheet](https://cdn.sparkfun.com/datasheets/Sensors/Temp/DS18B20.pdf)
- [MPU6050 Sensor Datasheet](https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf)
- [PCF8574 I2C Backpack Datasheet](https://www.ti.com/lit/ds/symlink/pcf8574.pdf)