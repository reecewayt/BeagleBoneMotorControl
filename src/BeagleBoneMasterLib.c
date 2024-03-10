
#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include "../include/BeagleBoneMaster.h"

const float desiredTimerDelay = 1.0f;

uint32_t calculateTLDR(float desiredTimeSec){
    const float timerClockPeriod = 1.0f / 32768.0f;
    uint32_t tldr = 0xFFFFFFFF - (uint32_t)((desiredTimeSec/ timerClockPeriod) - 1);
    return tldr;
}

uint32_t calculatePSC(int sys_clk, int internal_clk){
    uint32_t PSC = (uint32_t)((sys_clk/internal_clk)-1);
    return PSC;
}


uint32_t calculateSCLL(int frequency_khz, int iclk_mhz) {
    // Convert the ICLK from MHz to micro second period
    float t_period_iclk = 1.0 / (iclk_mhz * 1e6) * 1e6; 
    //t_low in micro seconds
    float t_low = 1.0 / (frequency_khz * 1e3 * 2) * 1e6;
    //calculate SCLL value, see sitara manual for formula
    uint32_t scll = (uint32_t)(t_low / t_period_iclk) - 7;
    // Return the calculated SCLL value
    return scll;
}

uint32_t calculateSCLH(int frequency_khz, int iclk_mhz) {
    // Convert the ICLK from MHz to µs period
    float t_period_iclk = 1.0 / (iclk_mhz * 1e6) * 1e6; 
    //t_high in seconds
    float t_high = 1.0 / (frequency_khz * 1e3 * 2)* 1e6;
    //calculate SCHL value, see sitara manual for formula
    uint32_t sclh = (uint32_t)(t_high / t_period_iclk) - 5;
    // Return the calculated SCLH value
    return sclh;
}

void gpio1_init(void) {
    HWREG(CM_PER_GPIO1_CLKCTRL) = TURN_ON_CLK_AND_DB;
    HWREG(GPIO1_BASE + GPIO1_FALLDETECT) = FALL_EDGE_DETECT_IRQ;
    HWREG(GPIO1_BASE + GPIO1_DEBOUNCE_ENBL) = DEBOUNCE_ENABLE;
    HWREG(GPIO1_BASE + GPIO1_DEBOUNCETIME) = DEBOUNCE_TIME;
}
void gpio1_disable_irq(void){
    uint32_t temp = HWREG(GPIO1_BASE + GPIO1_FALLDETECT);
    temp &= ~GPIO1_3_SIGLINE; //clears bit 3 
    HWREG(GPIO1_BASE + GPIO1_FALLDETECT) = temp; //disable further irqs
}

//reactives gpio falling detect irq signal on pin gpio1_3 
void gpio1_enable_irq(void){
    HWREG(GPIO1_BASE + GPIO1_FALLDETECT) = GPIO1_3_SIGLINE;
}


//TODO: Figure out how to catch IRQ signals in C
void IRQ_init(void){
    HWREG(INTC_BASE + INTC_SYSCONFIG) = RESET_INTC;
    HWREG(INTC_BASE + INTC_MIR_CLEAR2) = UNMASK_TIMER5_INTR;
    HWREG(INTC_BASE + INTC_MIR_CLEAR3) = UNMASK_GPIO1_INTR;
}

void timer5_init(void) {
    HWREG(CM_PER_TIMER5_CLKCTRL) = TURN_ON_CLK;
    HWREG(CLKSEL_TIMER5_CLK) = SELECT_32KHz_CLK;
    HWREG(TIMER5_BASE + TIMER5_OCP_CFG) = RESET_TIMER;
    HWREG(TIMER5_BASE + TIMER5_IRQENABLE) = IRQ_SET_TIMER5_ENB;
    //Converts count time in second to appropriate register value, see Sitara Manual
    uint32_t count_value= calculateTLDR(TIMER5_COUNT_TIME); 
     //Note TLDR is equal to TCRR 
    HWREG(TIMER5_BASE + TIMER5_TLDR) = count_value;
    //FIXME: TCRR will need to be initialized before each use, not using auto-reload
    HWREG(TIMER5_BASE + TIMER5_TCRR) = count_value;

}


//One shot timer, IRQ will signal on overflow based on timer5_init() settings
void timer5_start(void){
    HWREG(TIMER5_BASE + TIMER5_TCLR) = START_TIMER; 
}

//TODO: You will need to update startup file in Code composer studio to catch this
void irq_director(void) {
    //get address of GPIO1_3 irq signal
    uint32_t temp = HWREG(GPIO1_BASE + GPIO1_IRQSTATUS);
    //checks if IRQ signal is from GPIO1_3 switch button
    if(temp & GPIO1_3_SIGLINE){
        //IRQ is from buttonm, clear IRQ and handle appropriately
        HWREG(GPIO1_BASE + GPIO1_IRQSTATUS) = temp; //TODO: it might be best to clear after IRQ is services
        //TODO: disable IRQs and Send Step Sequence
    }
    //get address of timer5 irq signal
    temp = HWREG(TIMER5_BASE + TIMER5_IRQSTATUS);
    if(temp & TIMER5_OVFL_CHECK) {
        //overflow detected, ready to send next step
        HWREG(TIMER5_BASE + TIMER5_IRQSTATUS) = temp; //writing back clears flag
        temp = HWREG(TIMER5_BASE + TIMER5_TLDR); //timer load value
        HWREG(TIMER5_BASE + TIMER5_TCRR) = temp; //reload timer count register
        //TODO: send next data step
    }
    else{
        printf("System detected IRQ signal, but source not found...\n");
    }

}


void I2C_init(void) {
    //Set P9 Header mux to I2C1 Mode2
    HWREG(CTRL_MODULES_BASE + CONF_SPI0_CS0) = 0x32;
    HWREG(CTRL_MODULES_BASE + CONF_SPI0_D1) = 0x32;
    //Turn on clock for I2C1 Module
    HWREG(CM_PER_BASE + CM_PER_I2C_CLKCTRL) = 0x2;
    //Take I2C1 Module out of reset
    HWREG(I2C1_BASE + I2C_CON) = I2C_DISABLE_RESET;
    //Set up I2C internal clock and data transmission speed
    HWREG(I2C1_BASE + I2C_PSC) = calculatePSC(I2C_SYS_CLK, I2C_ICLK);
    HWREG(I2C1_BASE + I2C_SCLL) = calculateSCLL(I2C_FS_MD_FREQUENCE, I2C_ICLK);
    HWREG(I2C1_BASE + I2C_SCLH) = calculateSCLH(I2C_FS_MD_FREQUENCE, I2C_ICLK);
}

//FIXME:You will need to update the way you send data later. 
void I2C_send_data(uint8_t data) {
    //Write Test Address to Slave Address Reg
    HWREG(I2C1_BASE + I2C_SA) = data;
    HWREG(I2C1_BASE + I2C_CNT) = 0x01;

    //Start data transfer
    HWREG(I2C1_BASE + I2C_CON) = I2C_START_TRANSFER;
}

