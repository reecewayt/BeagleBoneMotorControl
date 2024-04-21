/*
 * Truth Table for Counter Clockwise Rotation of Stepper Motor:
 *
 * Step Sequence (binary): AIN1, AIN2, BIN1, BIN2
 * H-Bridge Inputs:
 *   AIN1 and AIN2 are connected to PC9685 Pin Signals LED4 and LED3 respectively
 *   BIN1 and BIN2 are connected to PC9685 Pin Signals LED5 and LED6 respectively
 *
 *   H = High (ON) signal
 *   L = Low (OFF) signal
 *
 *   +--------+------+-------+------+------+------+------+------+------+
 *   | Step   | AIN1 | AIN2  | BIN1 | BIN2 | LED4 | LED3 | LED5 | LED6 |
 *   +--------+------+-------+------+------+------+------+------+------+
 *   | Step 1 |  L   |  H    |  L   |  H   |  OFF |  ON  | OFF  | ON   |
 *   | Step 2 |  H   |  L    |  L   |  H   |  ON  |  OFF | OFF  | ON   |
 *   | Step 3 |  H   |  L    |  H   |  L   |  ON  |  OFF | ON   | OFF  |
 *   | Step 4 |  L   |  H    |  H   |  L   |  OFF |  ON  | ON   | OFF  |
 *   +--------+------+-------+------+------+------+------+------+------+
 *
 *   Definitions:
 *   #define STEP1 0x05 // 0101 (binary) - AIN1=L, AIN2=H, BIN1=L, BIN2=H
 *   #define STEP2 0x09 // 1001 (binary) - AIN1=H, AIN2=L, BIN1=L, BIN2=H
 *   #define STEP3 0x0A // 1010 (binary) - AIN1=H, AIN2=L, BIN1=H, BIN2=L
 *   #define STEP4 0x06 // 0110 (binary) - AIN1=L, AIN2=H, BIN1=H, BIN2=L
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "../include/BeagleBoneMaster.h"
#include "../include/MotorControllerLib.h"


const PCAConfig_t PCA_Controller = {
    .ADDRESS = 0x70, // Adjusted for 7-bit addressing
    .PRE_SCALE = 0xFE,
    .ALL_LED_OFF_H = 0xFD,
    .RESET = 0x06,
    //Motor H-Bridge Signal Line Addresses
    .AIN1_ON = 0x17,
    .AIN1_OFF = 0x19,
    .AIN2_ON = 0x13,
    .AIN2_OFF = 0x15,
    .BIN1_ON = 0x1B,
    .BIN1_OFF = 0x1D,
    .BIN2_ON = 0x1F,
    .BIN2_OFF = 0x21,
    //LED2 and LED7 need to be turned on for H-Bridge to be enabled
    .LED2_ON_H = 0x0F,
    .LED7_ON_H = 0x23,
    //Mode control regs
    .MODE1_REG = 0x00,
    .MODE2_REG = 0x01,
    //Internal Oscillator, turns on
    .MODE1_OSC_BIT_CLEAR = 0x01,
    //Turn off PWM functionality
    .DISABLE_PWM = 0x10,
    //Stepper Motor Step Sequence
    .STEP1 = 0x05,
    .STEP2 = 0x09,
    .STEP3 = 0x0A,
    .STEP4 = 0x06,
    //Turns on signal line
    .PWM_OUTPUT_ENABLE = 0x10,
    //Turns off signal line
    .PWM_OUTPUT_DISABLE = 0x00
};

void motor_init(void){
    // Prescale value for PWM frequency
    uint8_t prescl = 0x5;
    pca_write_byte(PCA_Controller.PRE_SCALE, prescl);
    delay(5000);
    pca_write_byte(PCA_Controller.MODE1_REG, PCA_Controller.MODE1_OSC_BIT_CLEAR);
    delay(5000);
    // Disable all PWM outputs
    pca_write_byte(PCA_Controller.ALL_LED_OFF_H, 0x00);
    delay(5000);
    //LED2 is at address 0x0F
    pca_write_byte(PCA_Controller.LED2_ON_H, PCA_Controller.PWM_OUTPUT_ENABLE); 
    delay(5000);
    //LED7 is at address 0x23
    pca_write_byte(PCA_Controller.LED7_ON_H, PCA_Controller.PWM_OUTPUT_ENABLE); 
    delay(5000);
}

void pca_write_byte(volatile uint8_t ctrl_reg, volatile uint8_t value){
    HWREG(I2C1.BASE + I2C1.IRQSTATUS_RAW) = I2C1.IRQ_RESET; 
    HWREG(I2C1.BASE + I2C1.SA) = PCA_Controller.ADDRESS;
    HWREG(I2C1.BASE + I2C1.CNT) = 0x2; // Number of bytes to transfer
    HWREG(I2C1.BASE + I2C1.CON) = I2C1.START_TRANSFER;
    HWREG(I2C1.BASE + I2C1.DATA) = ctrl_reg;
    HWREG(I2C1.BASE + I2C1.DATA) = value;

}

//function to set the state of a motor pin, note that OFF PWM signal takes precedent over ON PWM signal
void pca_set_motor_pin_state(uint8_t on_register, uint8_t off_register, _Bool state) {
    if (state) {
        pca_write_byte(off_register, PCA_Controller.PWM_OUTPUT_DISABLE); // Fully disable OFF PWM output first
        delay(5000);
        pca_write_byte(on_register, PCA_Controller.PWM_OUTPUT_ENABLE);   // Then fully enable ON PWM output
    } else {
        pca_write_byte(on_register, PCA_Controller.PWM_OUTPUT_DISABLE);  // Fully disable ON PWM output first
        delay(5000);
        pca_write_byte(off_register, PCA_Controller.PWM_OUTPUT_ENABLE);  // Then fully enable OFF PWM output
    }
}

// Function to write to motor pins based on the step number
void pca_write_motor_pins(uint8_t stepnum) {
    pca_set_motor_pin_state(PCA_Controller.AIN1_ON, PCA_Controller.AIN1_OFF, stepnum & (1 << AIN1));
    delay(5000);
    pca_set_motor_pin_state(PCA_Controller.AIN2_ON, PCA_Controller.AIN2_OFF, stepnum & (1 << AIN2));
    delay(5000);
    pca_set_motor_pin_state(PCA_Controller.BIN1_ON, PCA_Controller.BIN1_OFF, stepnum & (1 << BIN1));
    delay(5000);
    pca_set_motor_pin_state(PCA_Controller.BIN2_ON, PCA_Controller.BIN2_OFF, stepnum & (1 << BIN2));
}


void full_step_motor(int step){
    switch (step){
        case 1: pca_write_motor_pins(PCA_Controller.STEP1); break;
        case 2: pca_write_motor_pins(PCA_Controller.STEP2); break;
        case 3: pca_write_motor_pins(PCA_Controller.STEP3); break;
        case 4: pca_write_motor_pins(PCA_Controller.STEP4); break;
    }
}

void delay(unsigned int counts) {
    while (counts > 0) {
        asm("NOP");
        // This empty loop will just count down to zero
        counts--;
    }
}

void pca_reset(void){
    HWREG(I2C1.BASE + I2C1.IRQSTATUS_RAW) = I2C1.IRQ_RESET;
    HWREG(I2C1.BASE + I2C1.SA) = 0x00; //general call
    HWREG(I2C1.BASE + I2C1.CNT) = 0x1; // Number of bytes to transfer
    HWREG(I2C1.BASE + I2C1.CON) = I2C1.START_TRANSFER;
    HWREG(I2C1.BASE + I2C1.DATA) = 0x06;
}

