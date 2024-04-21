#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../include/BeagleBoneMaster.h"
#include "../include/MotorControllerLib.h"


//Typedef constant declarations
const clk_mods_t clocks = {
    .CM_PER_BASE = 0x44E00000,
    .CM_PER_GPIO1_CLKCTRL = 0xAC,
    .CM_PER_I2C1_CLKCTRL = 0x48 
};

const P9HeaderConfig_t P9HeaderConfig = {
    .BASE = 0x44E10000,
    .CONF_SPI0_D1 = 0x958,
    .CONF_SPI0_CS0 = 0x95C,
    .MODE2_SELECT = 0x32
};

const GPIOConfigs_t GPIO1 = {
    .BASE = 0x4804C000,
    .FALLDETECT = 0x14C,
    .DEBOUNCE_ENBL = 0x150,
    .DEBOUNCETIME = 0x154,
    .IRQSTATUS = 0x2C,
    .IRQSTATUS_SET_0 = 0x34,
    .SYSCONFIG = 0x10,
    .TURN_ON_CLK_AND_DB = 0x00040002, 
    .DBNC_SET_TIME = 0xA0,     
    .GPIO1_3_SIGNAL = 0x8     //gpio1_3 signal bit 1000'b
};

const InterruptConfigs_t INTCConfig = {
    .BASE = 0x48200000,
    .SYSCONFIG = 0x10,
    .CONTROL = 0x48,
    .MIR_CLEAR2 = 0xC8, 
    .MIR_CLEAR3 = 0xE8,
    .RESET = 0x2,
    .UNMASK_TIMER5 = 0x20000000,
    .UNMASK_GPIO1 = 0x04,
    .NEW_IRQ = 0x01
};

const I2CConfig_t I2C1 = {
    .BASE = 0x4802A000,            
    .SYSC = 0x10,                  
    .PSC = 0xB0,                  
    .SCLL = 0xB4,                  
    .SCLH = 0xB8,                  
    .BUF = 0x94,                   
    .DATA = 0x9C,                 
    .CON = 0xA4,                  
    .SA = 0xAC,                    
    .CNT = 0x98,                   
    .IRQSTATUS_RAW = 0x24,        
    // Commands
    .SYS_CLK = 24,                 
    .ICLK = 12,                    
    .FS_MD_FREQUENCE = 400,        
    .SCL_LOW_TIME = 0x17,                 
    .SCL_HIGH_TIME = 0x19,
    .PSC_VALUE = 0x01,
    .START_TRANSFER = 0x8603,      
    .ENABLE_MODULE = 0x8000,       
    .IRQ_RESET = 0x00             
};

//Setup interrupt stack routine
void setup_stacks(int stack_size){
    extern volatile unsigned int svc_stack[];
    extern volatile unsigned int irq_stack[];

    volatile unsigned int* svc_stack_top = svc_stack + stack_size;
    volatile unsigned int* irq_stack_top = irq_stack + stack_size;

    __asm(
          "MOV R13, %0\n\t"
          "CPS #0x12\n\t"
          "MOV R13, %1\n\t"
          "CPS #0x13\n\t"
          : //no outputs
          : "r"(svc_stack_top), "r"(irq_stack_top)
          : "r13"
    );
}

// Initializes GPIO1 for handling external interrupts and debouncing
void gpio1_init(void) {
    // Enable clock and debounce for GPIO1.
    HWREG(clocks.CM_PER_BASE + clocks.CM_PER_GPIO1_CLKCTRL) = GPIO1.TURN_ON_CLK_AND_DB; 
    HWREG(GPIO1.BASE + GPIO1.SYSCONFIG) = 0x02; // Soft reset

    // Configure GPIO1 to detect falling edge interrupts and debounce.
    HWREG(GPIO1.BASE + GPIO1.FALLDETECT) = GPIO1.GPIO1_3_SIGNAL;
    HWREG(GPIO1.BASE + GPIO1.DEBOUNCE_ENBL) = GPIO1.GPIO1_3_SIGNAL;
    HWREG(GPIO1.BASE + GPIO1.DEBOUNCETIME) = GPIO1.DBNC_SET_TIME;
    
    // Enable GPIO1_3 IRQ signal
    HWREG(GPIO1.BASE + GPIO1.IRQSTATUS_SET_0) = GPIO1.GPIO1_3_SIGNAL;
}

// Disables IRQ for GPIO1_3 to prevent further interrupts during handling
void gpio1_disable_irq(void){
    uint32_t temp = HWREG(GPIO1.BASE + GPIO1.FALLDETECT);
    temp &= ~GPIO1.GPIO1_3_SIGNAL; // Clear bit 3 to disable IRQ.
    HWREG(GPIO1.BASE + GPIO1.FALLDETECT) = temp;
}

// Re-enables GPIO falling edge detection IRQ on pin GPIO1_3 after handling
void gpio1_enable_irq(void){
    HWREG(GPIO1.BASE + GPIO1.FALLDETECT) = GPIO1.GPIO1_3_SIGNAL;
    push_button = 0; //push_button is a flag, reset once enabled again
}

// Initializes the interrupt controller for Timer5 and GPIO1 interrupts.
void IRQ_init(void){
    // Reset the interrupt controller.
    HWREG(INTCConfig.BASE + INTCConfig.SYSCONFIG) = INTCConfig.RESET;
    // Unmask interrupts for GPIO1
    HWREG(INTCConfig.BASE + INTCConfig.MIR_CLEAR3) = INTCConfig.UNMASK_GPIO1;

    //Clear existing IRQ signals and enable new generation
    HWREG(INTCConfig.BASE + INTCConfig.CONTROL) = INTCConfig.NEW_IRQ;
}

// Handles IRQ signals from GPIO1_3 and Timer5, performing necessary actions.
void irq_director(void){
    uint32_t temp; 

    temp = HWREG(GPIO1.BASE + GPIO1.IRQSTATUS);
    if(temp & GPIO1.GPIO1_3_SIGNAL){
        HWREG(GPIO1.BASE + GPIO1.IRQSTATUS) = temp;
        //disable further interrupts
        gpio1_disable_irq(); 
        push_button = 1; //set flag to true, proceed with stepper motor sequence
    }

   clear_interrupt_mask_bit(); 
}

// Initializes I2C1 module, configuring clock, speed, and module reset.
void I2C_init(void) {
    // Configure I2C1 pins on P9 header.
    HWREG(P9HeaderConfig.BASE + P9HeaderConfig.CONF_SPI0_CS0) = P9HeaderConfig.MODE2_SELECT;
    HWREG(P9HeaderConfig.BASE + P9HeaderConfig.CONF_SPI0_D1) = P9HeaderConfig.MODE2_SELECT;
    
    // Enable clock for I2C1 module and perform a soft reset.
    HWREG(I2C1.BASE + I2C1.SYSC) = I2C1.ENABLE_MODULE;
    
    // Clear FIFO buffer and configure I2C speed.
    HWREG(I2C1.BASE + I2C1.BUF) = I2C1.IRQ_RESET; // Clear FIFO
    HWREG(I2C1.BASE + I2C1.PSC) = I2C1.PSC_VALUE;
    HWREG(I2C1.BASE + I2C1.SCLL) = I2C1.SCL_LOW_TIME;
    HWREG(I2C1.BASE + I2C1. SCLH) = I2C1.SCL_HIGH_TIME;
    
    //Enable and wake up I2C Module
    HWREG(I2C1.BASE + I2C1.CON) = I2C1.ENABLE_MODULE;
    
}

// Clears the interrupt mask bit, enabling IRQ handling. Represented here for future implementations
void clear_interrupt_mask_bit(void){
    uint32_t cpsr;
    asm("MRS %0, CPSR" : "=r" (cpsr));
    cpsr &= ~(1 << 7);
    asm("MSR CPSR_c, %0" :: "r" (cpsr));
}
