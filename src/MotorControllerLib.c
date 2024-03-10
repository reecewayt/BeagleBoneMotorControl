#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include "../include/BeagleBoneMaster.h"
#include "../include/MotorControllerLib.h"


uint8_t calc_prescale(int osc_clk_mhz, int update_rate_khz){
    uint8_t prescl = (osc_clk_mhz * 1e6/(4096 *(update_rate_khz * 1e3)) - 1);
    return prescl; 
}

void pca_write_byte(uint8_t ctrl_reg, uint8_t value){
    HWREG(I2C1_BASE + I2C_SA) = PCA_ADDRESS_W;
    HWREG(I2C1_BASE + I2C_DATA) = ctrl_reg;
    HWREG(I2C1_BASE + I2C_DATA) = value;
    HWREG(I2C1_BASE + I2C_CNT) = 0x2; //Number of bytes to transfer
    HWREG(I2C1_BASE + I2C_CON) = I2C_START_TRANSFER;

    while(HWREG(I2C1_BASE + I2C_IRQSTATUS_RAW) & I2C_BUS_BSY_CHECK){
        //wait loop until I2C bus is freed and transmission complete 
    }
}

static void pca_write_motor_pins(uint8_t stepnum){
    (0x01 & (stepnum >> AIN1)) ? pca_write_byte(PCA_AIN1,INPUT_FULL_ON):pca_write_byte(PCA_AIN1,INPUT_FULL_OFF);
    (0x01 & (stepnum >> AIN2)) ? pca_write_byte(PCA_AIN2, INPUT_FULL_ON):pca_write_byte(PCA_AIN2,INPUT_FULL_OFF);
    (0x01 & (stepnum >> BIN1)) ? pca_write_byte(PCA_BIN1, INPUT_FULL_ON):pca_write_byte(PCA_BIN1, INPUT_FULL_OFF);
    (0x01 & (stepnum >> BIN2)) ? pca_write_byte(PCA_BIN2, INPUT_FULL_ON): pca_write_byte(PCA_BIN2, INPUT_FULL_OFF);

}

void motor_contoller_init(void){
    //init Mode 1 oscillator
    pca_write_byte(MODE1_REG, MODE1_OSC_BIT_CLEAR);

    //set psc value
    uint8_t prescl = calc_prescale(OSC_CLK_MHZ, UPDATE_RATE_KHZ);
    pca_write_byte(PCA_PRE_SCALE, prescl);

    //Disable PWM, ALL LED OFF
    pca_write_byte(PCA_ALL_LED_OFF_H, DISABLE_PWM);

}

void full_step_motor(int step){  //(AIN1, AIN2, BIN1, BIN3)
    switch (step){
        case 1: //0101
            pca_write_motor_pins(STEP1);
            break;
        case 2:
            pca_write_motor_pins(STEP2);
            break;
        case 3: 
            pca_write_motor_pins(STEP3);
            break;
        case 4: 
            pca_write_motor_pins(STEP4);
            break;
    }

}