# BeagleBoneMotorController - Design Log

**Project Description**: This project was a an assignment for my class in Embedded Microprocessor Interfacing at PSU. We were tasked with writing bare-metal C code on the Beagle Bone Black to interface with an Adafruit FeatherBoard (I2C Motor Controller). This project involves using an I2C bus driver to control a PCA9685 Motor Controller IC to drive a stepper motor. The specific task is to step a stepper motor 200 steps counter-clockwise when a push button is pressed. This application was developed using a Beagle Bone Black and an Adafruit FeatherBoard equipped with an H-Bridge Motor Controller IC. 

### Hardware Requirements: 

- **Beagle Bone Black**: Acts as the host microcontroller ->[Documentation](https://docs.beagleboard.org/latest/boards/beaglebone/black/ch07.html)  
- **PCA9685 PWM I2C Controller**: Used for controlling the H-Bridge driver signals -> [Documentation](https://www.adafruit.com/product/2927)
- **Stepper Motor**: Bipolar stepper motor controlled via the PCA9685 -> [datasheet](https://cdn-shop.adafruit.com/product-files/324/C140-A+datasheet.jpg)
- **H-Bridge Motor Controller (TB6612FNG)**: Manages the direction and the speed of the stepper motor.
- **Logic analyzer** to debug I2C signals


### Software Requirements: 
- Code Composer Studio IDE (CCS) version 8.1.0 

### Implementation Details
1. I2C Bus Setup: Configured using specific registers on the Beagle Bone Black to communicate with the PCA9685.
2. Motor Controller Initialization: Involves setting up the PCA9685 to properly control the H-Bridge for motor movement.
3. Stepping Algorithm: Executes a full step sequence for the stepper motor, managed through control signals derived from the PCA9685 outputs.
4. **No Use of PWM:** For this project, PWM was not utilized; instead, control was achieved through direct full-on and full-off states managed by the PCA9685.

**Note**: Future implementations should use a timer based IRQ to send signals, and other IRQ generated events - this implementation utilizes wait loops to time signals based on the PCA9685 timing requirements. A simple IRQ via a mechanical switch connected is used, but i2c based IRQ signals are not implemented. 

### Project directory
```
/BeagleBoneMotorControl
|-- /src
    |-- main.c                   # Contains the main function.
    |-- BeagleBoneMasterLib.c    # Contains configs and commands for I2C Master
    |-- MotorControllerLib.c     # Contains motor configs and control functions.
|-- /include
    |-- BeagleBoneMasterLib.h    # Header for Master macros, defintions, and function declarations
    |-- MotorControllerLib.h     # Header for motor control definitions and function declarations
|-- README.md                    # Project description and instructions.
```


### Truth Table for Clockwise Rotation of Stepper Motor:

Step Sequence (binary): AIN1, AIN2, BIN1, BIN2
H-Bridge Inputs:
AIN1 and AIN2 are connected to PC9685 Pin Signals LED4 and LED3 respectively
BIN1 and BIN2 are connected to PC9685 Pin Signals LED5 and LED6 respectively
LED2 and LED7 both need to turned full on during operations and are implemented 
during controller initialization


H = High (ON) signal
L = Low (OFF) signal
 
| Step   | AIN1 | AIN2 | BIN1 | BIN2 | LED4 | LED3 | LED5 | LED6 |
|--------|------|------|------|------|------|------|------|------|
| Step 1 |  L   |  H   |  L   |  H   | OFF  | ON   | OFF  | ON   |
| Step 2 |  H   |  L   |  L   |  H   | ON   | OFF  | OFF  | ON   |
| Step 3 |  H   |  L   |  H   |  L   | ON   | OFF  | ON   | OFF  |
| Step 4 |  L   |  H   |  H   |  L   | OFF  | ON   | ON   | OFF  |

Definitions:  
```C
#define STEP1 0x05 // 0101 (binary) - AIN1=L, AIN2=H, BIN1=L, BIN2=H  
#define STEP2 0x09 // 1001 (binary) - AIN1=H, AIN2=L, BIN1=L, BIN2=H  
#define STEP3 0x0A // 1010 (binary) - AIN1=H, AIN2=L, BIN1=H, BIN2=L  
#define STEP4 0x06 // 0110 (binary) - AIN1=L, AIN2=H, BIN1=H, BIN2=L  
```

