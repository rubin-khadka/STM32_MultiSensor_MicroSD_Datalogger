# STM32 Multi Sensor MicroSD FatFs Datalogger Project

![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)
[![STM32](https://img.shields.io/badge/STM32-F103C8T6-blue)](https://www.st.com/en/microcontrollers-microprocessors/stm32f103c8.html)
[![CubeIDE](https://img.shields.io/badge/IDE-STM32CubeIDE-darkblue)](http://st.com/en/development-tools/stm32cubeide.html)
[![FatFS](https://img.shields.io/badge/FatFS-R0.15-green)](http://elm-chan.org/fsw/ff/00index_e.html)

## Project Overview

The **STM32 Multi Sensor MicroSD Datalogger** Project is a comprehensive embedded system that reads data from multiple sensors and logs it to a `MicroSD card` in `CSV` format. The system features real-time display on a `16x2 LCD`, user interaction via push buttons, and data retrieval over UART serial communication.

The system reads data from multiple sensors:
- **DS18B20**: Digital temperature sensor (1-Wire)
- **DHT11**: Temperature and humidity sensor (1-Wire protocol)
- **MPU6050**: 6-axis accelerometer, gyroscope, and temperature sensor (I2C)

The data acquired from these sensors are saved into a MicroSD card in a file named `saved_sensor_data.csv` in CSV format using the `FatFS` file system.

The data can be saved using a press of a button and retrieved and viewed through UART in a serial terminal in formatted table output. The data is also saved automatically every 5 seconds (currently disabled for demonstration purposes as button-based saving is easier to showcase).

The main control loop runs at a timing of 10ms, demonstrating real-time control and task scheduling without using an RTOS.

This project really demonstrates the combination of multiple embedded concepts: sensor data acquisition, data storage, real-time control, and task scheduling through a control loop.

## Video Demonstrations

**Button 1 – Display Mode Switching**

https://github.com/user-attachments/assets/6d35bd0c-daca-4b2e-b4b6-75815050c1f4

Pressing Button 1 cycles through four LCD display modes: DHT11 Temp/Hum → DS18B20 + MPU6050 Temp → Accelerometer → Gyroscope

**Button 2 & 3 – Save & Retrieve Data**

https://github.com/user-attachments/assets/e8b59623-2705-426f-be46-1be9d1364807

Button 2 saves sensor data to MicroSD card. Button 3 retrieves all stored data and sends via UART.<br>
Both buttons have a 1-second cooldown to prevent accidental multiple presses and allow time for data to be saved.

**Data Persistence – After Reset**

https://github.com/user-attachments/assets/8e4eb409-ab87-4282-be4d-3cabdc085a2d

Data persists in the SD card even after reset. The CSV file is found and data is read correctly.

**UART Output Format**  

https://github.com/user-attachments/assets/a17e9b4f-a549-4912-be80-437c818b989c

HTerm output showing formatted table.

## Hardware Components

| Component | Quantity | Description |
|-----------|----------|-------------|
| **STM32F103C8T6** | 1 | "Blue Pill" development board with 72MHz Cortex-M3 |
| **DHT11** | 1 | Temperature & Humidity sensor (1-Wire protocol) | 
| **DS18B20** | 1 | Digital temperature sensor (1-Wire protocol, ±0.5°C accuracy) |
| **MPU6050** | 1 | 6-axis inertial measurement unit (accelerometer + gyroscope + temperature) |
| **LCD 16x2 with I2C** | 1 | Character display module with I2C backpack (PCF8574) |
| **MicroSD Card Adapter** | 1 | SPI interface module for SD card |
| **MicroSD Card** | 1 | Storage media (FAT32 formatted) |
| **Push Buttons** | 3 | Two-leg tactile switches for user input |
| **USB-to-Serial Converter** | 1 | CP2102 / CH340 / FTDI for UART |

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
| **MicroSD Card Adapter** | PB3 | SCK | SPI1 Clock |
| | PB4 | MISO | SPI1 Master In Slave Out |
| | PB5 | MOSI | SPI1 Master Out Slave In |
| | PB6 | CS | Chip Select |
| | 3.3V | VCC | Power |
| | GND | GND | Common ground |
| **UART** | PA9 | TX to USB-Serial RX | 115200 baud, 8-N-1 |
| | PA10 | RX to USB-Serial TX | Optional for commands |
| **Button 1** | PA0 | Mode select | Input with internal pull-up |
| **Button 2** | PA1 | Save Data | Input with internal pull-up |
| **Button 3** | PA2 | Read Data | Input with internal pull-up |

> **Note:** MicroSD cards require 3.3V logic, but most adapter modules include onboard voltage regulators and level shifters, allowing them to be powered with 5V.

The LCD display and MPU6050 share the same I2C bus (PB10/SCL, PB11/SDA) with different addresses:

| Device | I2C Address (7-bit) | 8-bit Write | 8-bit Read |
|--------|---------------------|-------------|------------|
| **MPU6050** | 0x68 | 0xD0 | 0xD1 |
| **LCD (PCF8574)** | 0x27 | 0x4E | 0x4F |

🔗 [View Custom Written I2C Driver Source Code](https://github.com/rubin-khadka/STM32_MultiSensor_MicroSD_Datalogger/blob/main/Core/Src/i2c2.c) <br>
> **Note**: All peripherals are used as pre-built modules. The LCD module uses a PCF8574 I2C backpack. 

## Project Schematic

<img width="1448" height="617" alt="Schematic_diagram" src="https://github.com/user-attachments/assets/429ae531-0c56-496d-b5db-be08bd934584" />

*Schematic designed using EasyEDA*

## Task Scheduling

The system uses a **10ms timer-based control loop** with independent counters for each task. TIMER3 is configured to drive the main control loop, ensuring precise and non-blocking task execution.

### Task Frequencies

| Task | Frequency | Period | Execution |
|------|-----------|--------|-----------|
| **DHT11 Read** | 1 Hz | 1 seconds | Every 100 loops |
| **DS18B20 Read** | 1 Hz | 1 seconds | Every 100 loops |
| **MPU6050 Read** | 20 Hz | 50 ms | Every 5 loops |
| **LCD Update** | 10 Hz | 100 ms | Every 10 loops |
| **UART Output** | 10 Hz | 100 ms | Every 10 loops |
| **Data Logger Task** | 0.2 Hz | 5 seconds | Every 500 loops |
| **Button Status Check** | 100 Hz | 10 ms | Every loop |
| **Button Interrupts** | Event-driven | On press | EXTIx + TIM4 debounce |

### Timer Configuration

| Timer | Resolution | Purpose |
|-------|------------|---------|
| **DWT** | 1µs | DS18B20 1-Wire protocol precise timing |
| **TIMER2** | 1ms | System millisecond counter and delays |
| **TIMER3** | 0.1ms | **10ms control loop scheduler** (heartbeat) |
| **TIMER4** | 0.1ms | Button debouncing (50ms) |

🔗 [View DWT Driver (Microsecond Delay)](https://github.com/rubin-khadka/STM32_MultiSensor_MicroSD_Datalogger/blob/main/Core/Src/dwt.c)  
🔗 [View TIMER2 Driver (Millisecond Counter)](https://github.com/rubin-khadka/STM32_MultiSensor_MicroSD_Datalogger/blob/main/Core/Src/timer2.c)  
🔗 [View TIMER3 Driver (10ms Heartbeat)](https://github.com/rubin-khadka/STM32_MultiSensor_MicroSD_Datalogger/blob/main/Core/Src/timer3.c)  
🔗 [View Button & TIMER4 Driver (Debounce)](https://github.com/rubin-khadka/STM32_MultiSensor_MicroSD_Datalogger/blob/main/Core/Src/button.c)

> **Note:** DWT (Data Watchpoint and Trace) is a built-in peripheral in ARM Cortex-M3 cores that provides a cycle counter running at CPU frequency (72MHz). This gives ~13.9ns resolution, making it ideal for generating precise microsecond delays required by the DS18B20 1-Wire protocol. Unlike traditional timer-based delays, DWT does not occupy a dedicated timer peripheral and continues running in the background.

## MicroSD Card & FatFS Driver

The SD card interface handles all communication between the STM32 and the MicroSD card, from low-level SPI commands to high-level file operations.

### Complete Custom Stack
```
Application Layer (sd_data_logger.c)
↓
High-Level File API (sd_functions.c) - Mount, read, write, list files
↓
FatFS Middleware (CubeMX generated)
↓
Disk I/O Interface (sd_diskio.c) - FatFS hardware abstraction
↓
Low-Level SPI Driver (sd_spi.c) - Raw SPI commands, DMA, timing
↓
Hardware (STM32 SPI1 + SD Card)
```

#### 1. **Low-Level Driver (`sd_spi.c`)**
- Full SD protocol implementation (CMD0, CMD8, ACMD41, CMD17, CMD24, etc.)
- SDHC/SDSC detection and handling
- Single and multi-block read/write with DMA support
- Timeout handling using TIMER2
- CRC and error checking

#### 2. **FatFS Hardware Interface (`sd_diskio.c`)**
- Standard FatFS disk I/O functions (initialize, read, write, ioctl)
- Connects hardware driver to FatFS middleware
- Parameter checking and status reporting

#### 3. **High-Level File API (`sd_functions.c`)**
- **Mount/Unmount** - Filesystem detection and mounting
- **Auto-format** - Detects unformatted cards and formats them (512/4096-byte fallback)
- **File Operations** - Create, write, append, read, delete, rename
- **Directory Operations** - Create folders, recursive listing with sizes
- **Space Information** - Get free and total space in KB
- **CSV Parsing** - Read and parse CSV files
- **User Feedback** - Operation status over UART

| Feature | Description |
|---------|-------------|
| **Auto-format** | If no filesystem, tries multiple sector sizes (512B → 4096B) |
| **Data sync** | `f_sync()` after writes ensures data is written to card |
| **SDHC/SDSC support** | Properly handles both card types |
| **DMA transfers** | Block reads/writes with DMA |
| **Timeout handling** | Uses TIMER2 for consistent timing |
| **Error handling** | Return code checking and reporting |
| **Directory listing** | Recursive listing with file sizes |

🔗 [View sd_spi.c - Low-Level Driver](https://github.com/rubin-khadka/STM32_MultiSensor_MicroSD_Datalogger/blob/main/Core/Src/sd_spi.c)<br>
🔗 [View sd_diskio.c - FatFS Interface](https://github.com/rubin-khadka/STM32_MultiSensor_MicroSD_Datalogger/blob/main/Core/Src/sd_diskio.c) <br>
🔗 [View sd_functions.c - High-Level API](https://github.com/rubin-khadka/STM32_MultiSensor_MicroSD_Datalogger/blob/main/Core/Src/sd_functions.c) <br>

## MPU6050 IMU Driver

The MPU6050 is a 6-axis motion tracking device with an **I2C interface** (shared with LCD at address 0x68).

The driver provides two ways to read sensor data:
1. **Raw Data**: Direct 16-bit integer values from registers (±32768 range)
2. **Scaled Data**: Converted to physical units using sensitivity factors

**Implementation:**
- **Burst read** of all 14 data bytes in a single I2C transaction
- Data stored in global structure for access by other tasks
- **Data Flow:** I2C Read (0x3B-0x48) → 14 bytes → Store raw values → Scale for display

### Output Values

| Measurement | Raw Range | Scale Factor | Physical Unit |
|-------------|-----------|--------------|---------------|
| **Accelerometer** | ±32768 | ±2g / ±4g / ±8g / ±16g | g (gravities) |
| **Gyroscope** | ±32768 | ±250 / ±500 / ±1000 / ±2000 | °/s (dps) |
| **Temperature** | ±32768 | 340.0 per °C + 36.53 offset | °C |

> **Note**: LCD display and SD Card logging use **scaled values** in physical units. Raw 16-bit values are available for custom calculations but stored separately.

🔗 [View MPU6050 Driver Source Code](https://github.com/rubin-khadka/STM32_MultiSensor_MicroSD_Datalogger/blob/main/Core/Src/mpu6050.c)

## DHT11 Sensor Driver

The DHT11 uses a **single-wire protocol** with precise timing:

| Phase | Duration | Description |
|-------|----------|-------------|
| **Start Signal** | 18ms LOW + 20µs HIGH | MCU wakes sensor |
| **Sensor Response** | 80µs LOW + 80µs HIGH | Sensor acknowledges |
| **Bit "0"** | 50µs LOW + 26-28µs HIGH | Logic 0 |
| **Bit "1"** | 50µs LOW + 70µs HIGH | Logic 1 |
| **Data Frame** | 40 bits | 5 bytes (humidity ×2 + temp ×2 + checksum) |

Instead of measuring pulse width, I used a **simpler approach** looking at datasheet:

For each bit:
1. Wait for line to go HIGH
2. Delay exactly 40µs
3. If line still HIGH → logic 1 <br>
   If line is LOW → logic 0

To ensure the timing is not interrupted, **interrupts are disabled** while communicating with the sensor. The checksum provided by the sensor is used to verify data integrity.

🔗 [View DHT11 Driver Source Code](https://github.com/rubin-khadka/STM32_MultiSensor_MicroSD_Datalogger/blob/main/Core/Src/dht11.c)

## DS18B20 Temperature Sensor Driver

The DS18B20 uses the **1-Wire protocol** with precise timing:

| Phase | Duration | Description |
|-------|----------|-------------|
| **Reset Pulse** | 500µs LOW | Master pulls line low |
| **Presence Pulse** | 60-240µs LOW | Sensor responds |
| **Write "0"** | 60µs LOW + 2µs HIGH | Master holds line low |
| **Write "1"** | 2µs LOW + 60µs HIGH | Master pulses briefly |
| **Read Slot** | 2µs LOW + sample at 10µs | Master reads bit |

Instead of complex state machines, the driver uses **direct bit-banging** with microsecond delays.

**Temperature Read Sequence:**
1. Send reset pulse (500µs) and wait for presence
2. Send Skip ROM command (0xCC)
3. Send Read Scratchpad command (0xBE)
4. Read 2 bytes of temperature data (LSB first)
5. Calculate: `temperature = raw_12bit × 0.0625°C`

🔗 [View DS18B20 Driver Source Code](https://github.com/rubin-khadka/STM32_MultiSensor_MicroSD_Datalogger/blob/main/Core/Src/ds18b20.c)

> **Note**: The sensor is read at 1Hz frequency to accommodate the 750ms conversion time. The driver automatically starts a new conversion after each read.

## Getting Started

### Prerequisites

| Component | Details |
|-----------|---------|
| **MCU** | STM32F103C8T6 "Blue Pill" development board |
| **Sensors** | DS18B20 + MPU6050 + DHT11 |
| **Display** | 16x2 LCD with I2C backpack (PCF8574) |
| **Storage** | MicroSD card adapter + MicroSD card (FAT32 formatted) |
| **Communication** | USB-to-Serial converter (CP2102/CH340/FTDI) |
| **Programmer** | ST-Link V2 |
| **IDE** | STM32CubeIDE v1.13.0 or later |

### Installation

1. Clone the repository
```bash
git clone https://github.com/rubin-khadka/STM32_MultiSensor_MicroSD_Datalogger.git
```
2. Open this project in STM32CubeIDE:
   - `File` → `Open Projects from File System...`
   - Select the cloned directory
   - Click `Finish`

3. Build & Flash
    - Build: `Ctrl+B`
    - Debug: `F11`
    - Run: `F8` (Resume)

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

## Project Status
- **Status**: Complete
- **Version**: v1.0
- **Last Updated**: March 2026

## Contact
**Rubin Khadka Chhetri**  
📧 rubin.khadka84@gmail.com <br>
🐙 GitHub: https://github.com/rubin-khadka
