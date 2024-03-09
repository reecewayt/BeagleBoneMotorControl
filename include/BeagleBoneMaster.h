/*
 * Created on: Feb 21, 2024
 * Author: reecwayt
 *
 * This module is designed for ECE 372 Design Project #2 to
 * initialize the I2C1 Controller. This implementation is done on
 * the beagle bone black with an AMX335x Sitara Processor
 * 
*/
#ifndef BEAGLEBONEMASTER_H_
#define BEAGLEBONEMASTER_H_

//macro to access hardware registers
#define HWREG(x) (*((volatile unsigned int *)(x)))


//GPIO1 Registers & Commands------------------------------------------------------------------------------
#define CM_PER_GPIO1_CLKCTRL 0x44E000AC  //GPIO1 Clock Module
#define GPIO1_BASE 0x4804C000            //Base of GPIO1
//Offsets---
#define GPIO1_FALLDETECT 0x14C    
//Commands---
#define TURN_ON_CLK_AND_DB 0x00040002   //Value to turn on clock and turn on debounce clock (optional)
#define DEBOUNC_ENABLE 0x8              //value to turn on debounce capabilities on switch
#define DEBOUNCE_TIME 0xA0              //set time to 5 ms
//-------------------------------------------------------------------------------------------------------

//Interrupt Registers & Commands-------------------------------------------------------------------------
#define INTC 0x48200000                 //Interrupt Controller Base Address 
//Offsets---  
#define INTC_SYSCONFIG 0x10            
#define INTC_MIR_CLEAR2 0xC8            
#define INTC_MIR_CLEAR3 0xE8        
//Commands---
#define RESET_INTC 0x2                   //Reset interrupt controller, good practice
#define UNMASK_TIMER5_INTR 0x20000000    //Clears intc #93 for timer5 interrupts
#define UNMASK_GPIO1_INTR 0x04           //Clears intc #98 for gpio1 interrupts
//-------------------------------------------------------------------------------------------------------

//Timer5 Registers & Commands----------------------------------------------------------------------------
#define TIMER5_BASE 0x48046000        
#define CM_PER_TIMER5_CLKCTRL 0x44E000EC       //Timer 5 Clock Peripheral Register
#define CLKSEL_TIMER5_CLK 0x44E00518           //Clock select module
//Offsets---  
#define TIMER5_OCP_CFG 0x10                    //OCP Config Register
#define TIMER5_IRQENABLE 0x2C      
#define TIMER5_TLDR 0x40                       //Timer Load Register            
//Commands---
#define TURN_ON_CLK 0x2
#define SELECT_32KHz_CLK 0x2            //selects 32.768 KHz timer clock     
#define IRQ_SET_TIMER5_ENB 0x2          //enable IRQ signals for timer5          
#define RESET_TIMER 0x1   
//TODO tomorrow, create function to calculate TLDR and TCRR values **(*****9()&908*9(*)87-82-8234875203489)    
//-------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------

//Offsets---  
       
//Commands---

//-------------------------------------------------------------------------------------------------------


//Control Module & Offsets Registers
#define CTRL_MODULE_BASE 0x44E10000     // Base address of Control Module
#define CONF_SPI0_D1 0x958              // Pad control reg to I2C1_SDA
#define CONF_SPI0_CS0 0x95C             // Pad control reg to I2C1_SCL

//Clock Module Base & Offsets
#define CM_PER_BASE 0x44E00000          // Base address of peripheral clock modules
#define CM_PER_I2C_CLKCTRL 0x48         // Reg for I2C1 clock


//I2C Register Set
#define I2C1_BASE 0x4802A000            // Base address of I2C1 Module
#define I2C_PSC 0xB0                    // Prescalar value
#define I2C_SCLL 0xB4                   // SCL line low time
#define I2C_SCLH 0xB8                   // SCL line high time
#define I2C_DATA 0x9C                   // Data Buffer Register
#define I2C_CON 0xA4                    // Config Register
#define I2C_SA 0xAC                     // Slave Address
#define I2C_CNT 0x98                    // Count Reg

//Macro for sending data
#define START_TRANSFER 0x8603

// TODO add later
typedef struct{
    volatile uint32_t control;
    volatile uint8_t data_rec;
    volatile uint8_t data_send;

} I2C_Reg_t;


void I2C_init(void);
void I2C_send_data(uint8_t data);


#endif 