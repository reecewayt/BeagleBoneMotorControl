/*
 * Created on: March 3rd, 2024
 * Author: Reece Wayt
 *
 * This module is designed for ECE 372 Design Project #2 to
 * initialize the I2C1 Controller. This implementation is done on
 * the beagle bone black with an AMX335x Sitara Processor
 * 
*/
#ifndef BEAGLEBONEMASTER_H_
#define BEAGLEBONEMASTER_H_

//macro to access hardware registers----------------------------------------------------------------------
#define HWREG(x) (*((volatile unsigned int *)(x)))


//GPIO1 Registers & Commands------------------------------------------------------------------------------
#define CM_PER_GPIO1_CLKCTRL 0x44E000AC  //GPIO1 Clock Module
#define GPIO1_BASE 0x4804C000            //Base of GPIO1
//Offsets---
#define GPIO1_FALLDETECT 0x14C          //Interrupt to detect falling edge of mechanical switch
#define GPIO1_DEBOUNCE_ENBL 0x150       //GPIO register to enable debounce of mechanical switch
#define GPIO1_DEBOUNCETIME 0x154        //time of debounce
#define GPIO1_IRQSTATUS 0x30             //Can read or write to detect and clear IRQ signal
//Commands---
#define TURN_ON_CLK_AND_DB 0x00040002   //Value to turn on clock and turn on debounce clock (optional)
#define FALL_EDGE_DETECT_IRQ 0x8        //Value to set IRQ interrupt of GPIO1_3
#define DEBOUNCE_ENABLE 0x8             //value to turn on debounce capabilities on switch
#define DEBOUNCE_TIME 0xA0              //set time to 5 ms
#define GPIO1_3_SIGLINE 0x8             //this signal line is bit 3, hex 0x8
//-------------------------------------------------------------------------------------------------------

//Interrupt Registers & Commands-------------------------------------------------------------------------
#define INTC_BASE 0x48200000                 //Interrupt Controller Base Address 
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
#define TIMER5_IRQSTATUS 0x28                  //For checking IRQ signals
#define TIMER5_IRQENABLE 0x2C                  //IRQ Enable 
#define TIMER5_TLDR 0x40                       //Timer Load Register  
#define TIMER5_TCRR 0x3C                       //Timer Count Register  
#define TIMER5_TCLR 0x38                       //Timer control register        
//Commands---
#define TURN_ON_CLK 0x2
#define SELECT_32KHz_CLK 0x2            //selects 32.768 KHz timer clock     
#define IRQ_SET_TIMER5_ENB 0x2          //enable IRQ signals for timer5    
#define TIMER5_OVFL_CHECK 0x2           //bit 1 will be set if overflow happens     
#define RESET_TIMER 0x1
#define START_TIMER 0x1                 //value to perform one shot timer, TCRR will need to be reset
#define TIMER5_COUNT_TIME 0.5           //Count time in seconds
//-------------------------------------------------------------------------------------------------------


//P9 BeagleBone Header Registers & Commands----------------------------------------------------------------------------
#define CTRL_MODULES_BASE 0x44E10000 
//Offsets---  
#define CONF_SPI0_D1 0x958              // Pad control reg to I2C1_SDA
#define CONF_SPI0_CS0 0x95C             // Pad control reg to I2C1_SCL       
//Commands---
#define MODE2_SELECT 0x32               //Mode2 MUXES select I2C1 SDA SCL on P9 Pins, see BeagleBone Manual       
//-------------------------------------------------------------------------------------------------------

//I2C1 Registers Set & Commands---------------------------------------------------------------------------

//Clock Module Base & Offsets---
#define CM_PER_BASE 0x44E00000          // Base address of peripheral clock modules
#define CM_PER_I2C_CLKCTRL 0x48         // Reg for I2C1 clock
//I2C Register Set---
#define I2C1_BASE 0x4802A000            // Base address of I2C1 Module
#define I2C_PSC 0xB0                    // Prescalar value
#define I2C_SCLL 0xB4                   // SCL line low time
#define I2C_SCLH 0xB8                   // SCL line high time
#define I2C_DATA 0x9C                   // Data Buffer Register
#define I2C_CON 0xA4                    // Config Register
#define I2C_SA 0xAC                     // Slave Address
#define I2C_CNT 0x98                    // Count Reg
//I2C Commands---
#define I2C_SYS_CLK 24                  // 24 MHz is the clock frequency for this module
#define I2C_ICLK 12                     // We are operating in F/S Mode, 12 MHz is recommended (see Sitara Manual)
#define I2C_FS_MD_FREQUENCE 400         // 400 KHz is the standard for F/S mode for the SCL line
#define I2C_START_TRANSFER 0x8603     
#define I2C_DISABLE_RESET 0x8600        //You need this to take module out of reset upon initialization      


//Globals and TypeDefs----------------------------------------------------------------------------------
extern const float desiredTimerDelay;

// TODO add later
typedef struct{
    volatile uint32_t control;
    volatile uint8_t data_rec;
    volatile uint8_t data_send;

} I2C_Reg_t;

//Function Prototypes----------------------------------------------------------------------------------

uint32_t calculateTLDR(float desiredTimeSec);

uint32_t calculatePSC(int sys_clk_mhz, int iclk_mhz);

uint32_t calculateSCLL(int frequency_khz, int iclk_mhz);

uint32_t calculateSCLH(int frequency_khz, int iclk_mhz);

void gpio1_init(void);
void IRQ_init(void);
void timer5_init(void);
void timer5_start(void);
void I2C_init(void);
void I2C_send_data(uint8_t data);


#endif 