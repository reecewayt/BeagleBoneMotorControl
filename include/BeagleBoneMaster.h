/*
 * Created on: March 3rd, 2024
 * Author: Reece Wayt
 *
 * This module is designed for ECE 372 Design Project #2 to initialize the I2C1 Controller. 
 * This implementation is done on the beagle bone black with AMX335x Sitara Processor.
 *
 */
#ifndef BEAGLEBONEMASTER_H_
#define BEAGLEBONEMASTER_H_

//macro to access hardware registers----------------------------------------------------------------------
#define HWREG(x) (*((volatile unsigned int *)(x)))

//extern defs & Globals
extern volatile int timerFlag;
extern volatile int push_button;

//typedefs

typedef struct { //Clock Module Peripherals
    uint32_t const CM_PER_BASE;  // Clock module base address
    uint32_t const CM_PER_GPIO1_CLKCTRL; //Location of GPIO1 clock
    uint32_t const CM_PER_I2C1_CLKCTRL; //Location of I2C1 Module
} clk_mods_t; 

//GPIO configuration registers and associated commands
typedef struct {
    uint32_t const CM_PER_CLKCTRL;  // Clock module control register
    uint32_t const BASE;            // Base address of GPIO1
    uint32_t const FALLDETECT;      // Falling edge detection register
    uint32_t const DEBOUNCE_ENBL;   // Debounce enable register
    uint32_t const DEBOUNCETIME;    // Debounce time register
    uint32_t const IRQSTATUS;       // IRQ status register
    uint32_t const IRQSTATUS_SET_0; // IRQ status set register
    uint32_t const SYSCONFIG;       // System configuration register
    // Commands
    uint32_t const TURN_ON_CLK_AND_DB;     //turn on clock and turn on debounce clock
    uint32_t const DBNC_SET_TIME;          //set time for debounce
    uint32_t const GPIO1_3_SIGNAL;         //this signal line for target GPIO1 bit
} GPIOConfigs_t;

extern const GPIOConfigs_t GPIO1;

//Interrupt controller and associated commands
typedef struct {
    uint32_t const BASE;          // Interrupt Controller Base Address
    uint32_t const SYSCONFIG;     // Offset for the System Configuration Register
    uint32_t const CONTROL;        // Offset to control register for new irq generation
    uint32_t const MIR_CLEAR2;    // Offset to clear the interrupt mask for set 2
    uint32_t const MIR_CLEAR3;    // Offset to clear the interrupt mask for set 3
    // Commands
    uint32_t const RESET;         // Command to reset the interrupt controller
    uint32_t const UNMASK_TIMER5; // Command to unmask Timer 5 interrupts
    uint32_t const UNMASK_GPIO1;  // Command to unmask GPIO1 interrupts
    uint32_t const NEW_IRQ;       // Command to allow new irq signals
} InterruptConfigs_t;

extern const InterruptConfigs_t INTCConfig;

//Beaglebone P9 Pad Registers and Mode Mux
typedef struct {
    uint32_t const BASE;              // Control Module Base Address
    uint32_t const CONF_SPI0_D1;      // Pad control register for I2C1_SDA
    uint32_t const CONF_SPI0_CS0;     // Pad control register for I2C1_SCL
    uint32_t const MODE2_SELECT;      // Mode2 select command for I2C1 pins
} P9HeaderConfig_t;

extern const P9HeaderConfig_t P9HeaderConfig;

//I2C1 Registers Set & Commands
typedef struct {
    uint32_t const BASE;            // Base address of I2C1 Module
    uint32_t const SYSC;            // Soft reset register offset
    uint32_t const PSC;             // Prescaler value register offset
    uint32_t const SCLL;            // SCL line low time register offset
    uint32_t const SCLH;            // SCL line high time register offset
    uint32_t const BUF;             // Buffer config register offset
    uint32_t const DATA;            // Data buffer register offset
    uint32_t const CON;             // Config register offset
    uint32_t const SA;              // Slave address register offset
    uint32_t const CNT;             // Count register offset
    uint32_t const IRQSTATUS_RAW;   // IRQ status register offset
    // I2C Commands
    uint32_t const SYS_CLK;         // Clock frequency for this module
    uint32_t const ICLK;            // I2C internal clock frequency
    uint32_t const FS_MD_FREQUENCE; // F/S mode frequency for the SCL line
    uint32_t const SCL_LOW_TIME;     //Low and High time for 400 KHz frequency
    uint32_t const SCL_HIGH_TIME;
    uint32_t const PSC_VALUE;       // Value to load into prescalar register
    uint32_t const START_TRANSFER;  // Start transfer command
    uint32_t const ENABLE_MODULE;   // Enable module command
    uint32_t const IRQ_RESET;       // Clear all IRQ signals command
} I2CConfig_t;

extern const I2CConfig_t I2C1;

//Function Prototypes

/*
 * Stack setup routine using inline assembly
 */
void setup_stacks(int stack_size);

/*
 * Initializes i2c bus with a 400 KHz internal clock:
 * 
 * Beagle Bone -> Bus Master
 * PCA Controller-> Slave 
 */
void I2C_init(void);

/*
 * This function performs a soft reset of the I2C1 Module
 */
void I2C_restart(void);

/*
 * This function enables the clock and debounce for GPIO1, configures GPIO1
 * to detect falling edge interrupts, and sets the debounce time.
*/
void gpio1_init(void);

/*
 * gpio1 enbale and disable functions perform a quick operation to prevent further interrupts 
 * from push button switch during the stepper motor step sequence
 */
void gpio1_enable_irq(void);
void gpio1_disable_irq(void);

/*
 * Initializes the irq signals need for this program, it unmasks the signals for 
 * timer5 and gpio1 modules
 */
void IRQ_init(void);

/*
 * IRQ signal handler, check interrupt source between gpio1 and timer5. Can be updated to
 * include additional interrupt signals.You must update the startup_ARMCA8 file (name is dependent on compiler used) 
 * to check these IRQ signals. 
 * Specifically, the irq vector table will need to be updated to include this function as shown below. 
 * -> __isr_vector:
        LDR   pc, [pc,#24]       @ 0x00 Reset
        LDR   pc, [pc,#-8]       @ 0x04 Undefined Instruction
        LDR   pc, [pc,#24]       @ 0x08 Supervisor Call
        LDR   pc, [pc,#-8]       @ 0x0C Prefetch Abort
        LDR   pc, [pc,#-8]       @ 0x10 Data Abort
        LDR   pc, [pc,#-8]       @ 0x14 Not used
        LDR   pc, =irq_director	 @ 0x18 IRQ interrupt
        LDR   pc, [pc,#-8]       @ 0x1C FIQ interrupt
 * ->
 */
void irq_director(void);

/**
 * Initializes Timer5 for one-shot operations. Configures Timer5 to operate with a 32KHz internal clock. The timer is set up
 * to generate an interrupt on overflow, which is used as a signal to delay each motor command. 
 */
void timer5_init(void);

/*
 * Starts Timer5 counting for a single operation - one shot interrupt timer. Each start loads the 
 * desired clock time using the below formula. In this program a long and short timer is used for 
 * sending motor step, and I2C commands. 
 * 
 * The Timer Load Register (TCRR) is calculated based on the desired time delay
 * using the following formula from the Sitara manual (page 4447):
 *
 * TLDR = 0xFFFFFFFF - ((DesiredTime * TimerClockFrequency) / ClockDivider) + 1
 *
 * Where:
 * - TimerClockFrequency = 32,768 Hz (32KHz internal clock)
 * - ClockDivider is set to 1 by default
 * - DesiredTime is the time in seconds after which the timer interrupt should occur
 */
void timer5_start(void);

//unmasks CPSR IRQ Bit
void clear_interrupt_mask_bit(void);

#endif
