# Advanced UART Communication System with STM32 and LM35 Sensor

![License](https://img.shields.io/badge/License-MIT%20(2024%20Shahab%20A.K.)-yellow.svg)
![Build Status](https://img.shields.io/github/workflow/status/ShahabAK1/STM32-Secret-Agent-Communication-Project/CI?label=Build%20Status)
![Version](https://img.shields.io/github/v/release/ShahabAK1/STM32-Secret-Agent-Communication-Project)
![Downloads](https://img.shields.io/github/downloads/ShahabAK1/STM32-Secret-Agent-Communication-Project/total)
![Last Commit](https://img.shields.io/github/last-commit/ShahabAK1/STM32-Secret-Agent-Communication-Project)
![Forks](https://img.shields.io/github/forks/ShahabAK1/STM32-Secret-Agent-Communication-Project?style=social)
![Stars](https://img.shields.io/github/stars/ShahabAK1/STM32-Secret-Agent-Communication-Project?style=social)


## Project Overview
This project demonstrates an **Advanced UART-based Communication System** using the **STM32F401RE microcontroller** and the **LM35 temperature sensor**. The system allows real-time temperature monitoring, command-based interface, encryption of data, error checking with CRC8, and self-diagnostics. The project is ideal for showcasing embedded systems development, specifically UART communication, sensor integration, and error-checking mechanisms.

[View Proteus Simulation](https://github.com/ShahabAK1/STM32-Secret-Agent-Communication-Project/blob/main/Proteus/UART_AS_SPY_Agent.PDF)

## Features
- **Real-Time Temperature Monitoring**: Measures ambient temperature using the LM35 sensor, with data transmitted via UART.
- **Command-Based Interface**: Users can start/stop data display, toggle encryption, and perform system diagnostics through UART commands.
- **Encryption Mode**: Implements simple XOR-based encryption for data transmission, activated via commands.
- **Error Checking with CRC8**: Ensures data integrity using CRC8 error-checking on transmitted data.
- **Self-Diagnostics Mode**: Validates the functionality of UART, ADC, and GPIO, providing a comprehensive status report.

## Commands
| Command | Description                        |
|---------|------------------------------------|
| `1`     | Start data display                |
| `0`     | Stop data display                 |
| `2`     | Display current status            |
| `3`     | Toggle encrypted mode             |
| `4`     | Initiate self-diagnostics         |

## Hardware Components
- **Microcontroller**: STM32F401RE
- **Temperature Sensor**: LM35
- **Virtual Terminal**: UART-based terminal for user interaction
- **Proteus Simulation**: Simulates the complete setup and hardware integration

## Software Tools
- **STM32CubeIDE**: For development and debugging.
- **Proteus**: For circuit simulation.
- **GitHub**: Version control and project documentation.

## Code Breakdown

### 1. UART Communication
The UART module is initialized to facilitate command-based communication with a virtual terminal. Commands allow users to control data display, toggle encryption, and run self-diagnostics.

```c
HAL_UART_Transmit(&huart2, (uint8_t*)"System Initialized\r\n", 20, HAL_MAX_DELAY);
HAL_UART_Transmit(&huart2, (uint8_t*)"Commands:\r\n'1' = Start\r\n'0' = Stop\r\n'2' = Status\r\n'3' = Encrypted Mode\r\n'4' = Self-Diagnostics\r\n", 95, HAL_MAX_DELAY);

2. Temperature Monitoring with CRC8
Temperature data is read from the LM35 sensor, and CRC8 is calculated to verify data integrity. The results are displayed in encrypted form if encryption mode is activated.
sprintf(msg, "\r\nTemperature: %.2f degC, CRC: %02X\r\n", temperature, calculate_crc8((uint8_t*)msg, strlen(msg)));

3. Self-Diagnostics
The self-diagnostics function checks the status of UART, ADC, and GPIO, providing the user with a comprehensive diagnostics report.
void self_diagnostics(void) {
    // Diagnostics report generation
}
Getting Started
Prerequisites
STM32CubeIDE: For code development and debugging.
Proteus: For circuit simulation.
UART Terminal Emulator: For monitoring UART output.
Installation

Clone this repository:git clone https://github.com/ShahabAK1/STM32-Secret-Agent-Communication-Project.git

Open the project in STM32CubeIDE.
Compile and load the code onto the STM32F401RE board.
Run the simulation in Proteus or connect to a UART terminal to begin interaction.

Example Commands:
Type 1 to start data display.
Type 2 to display the current status of the system.
Type 3 to toggle encryption mode.

Future Enhancements
Implement Advanced Encryption Algorithms: Enhance data security by integrating stronger encryption techniques.
Expand Self-Diagnostics: Add more comprehensive tests for peripherals such as I2C, SPI, etc.
Data Logging: Implement historical data storage for further analysis and reporting.

