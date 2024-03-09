
#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include "../include/BeagleBoneMaster.h"



void I2C_init(void) {

    //Set P9 Header mux to I2C1 Mode2
    HWREG(CTRL_MODULE_BASE + CONF_SPI0_CS0) = 0x32;
    HWREG(CTRL_MODULE_BASE + CONF_SPI0_D1) = 0x32;

    //Turn on clock for I2C1 Module
    HWREG(CM_PER_BASE + CM_PER_I2C_CLKCTRL) = 0x2;

    //Set up I2C internal clock and data transmission speed
    HWREG(I2C1_BASE + I2C_PSC) = 0x1;
    HWREG(I2C1_BASE + I2C_SCLL) = 0x17;
    HWREG(I2C1_BASE + I2C_SCLH) = 0x19;
    HWREG(I2C1_BASE + I2C_CON) = 0x8600;
}


void I2C_send_data(uint8_t data) {
    //Write Test Address to Slave Address Reg
    HWREG(I2C1_BASE + I2C_SA) = data;
    HWREG(I2C1_BASE + I2C_CNT) = 0x01;

    //Start data transfer
    HWREG(I2C1_BASE + I2C_CON) = START_TRANSFER;
}

