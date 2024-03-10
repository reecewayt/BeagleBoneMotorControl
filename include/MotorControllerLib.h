#ifndef MOTOR_CONTROLLER_H_
#define MOTOR_CONTROLLER_H_

//PCA Control Registers---
#define PCA_ADDRESS_W 0xE0        //LSB 0 is a write to slave address
#define PCA_ADDRESS_R 0xE1        //LSB 1 is a read from slave
#define PCA_PRE_SCALE 0xFE        //Address of control register on featherboard
#define PCA_ALL_LED_OFF_H 0xFD    //Address to access all LED PWM lines

#define PCA_AIN1 0x17             //AIN1 corresponds to LED4_ON_H control reg
#define PCA_AIN2 0x13             //AIN2 corresponds to LED3_ON_H control reg
#define PCA_BIN1 0x1B             //BIN1 corresponds to LED5_ON_H control reg
#define PCA_BIN2 0x1F             //BIN2 corresponds to LED6_ON_H control reg


#define MODE1_REG 0x00            //functional mode1, see PCA datasheet
#define MODE2_REG 0x01            //mode2 register

//Init Values
#define MODE1_OSC_BIT_CLEAR 0x01   //clears bit 4 to start oscillator
#define DISABLE_PWM 0x10           //set bit 4 which turns off pwm functionality
#define OSC_CLK_MHZ 25 
#define UPDATE_RATE_KHZ 1       

//Step Sequence in binary (AIN1, AIN2, BIN1, BIN2)
#define STEP1 0x05      //0101
#define STEP2 0x09      //1001
#define STEP3 0x0A      //1010
#define STEP4 0x06      //0110

//Turn X_IN ON OR OFF
#define INPUT_FULL_ON 0x10 
#define INPUT_FULL_OFF 0x00

typedef enum inputs{
    BIN2,
    BIN1,
    AIN2,
    AIN1
} inputs_t; 

//TODO: Make a struct to hold PCA values??? Future implementation

uint8_t calc_prescale(int osc_clk_mhz, int update_rate_khz);

void motor_controller_init(void);
void pca_write_byte(uint8_t ctrl_reg, uint8_t value);

void full_step_motor(int step);
static void pca_write_motor_pins(uint8_t stepnum);

#endif