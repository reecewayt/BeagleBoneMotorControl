#ifndef MOTOR_CONTROLLER_H_
#define MOTOR_CONTROLLER_H_


// Defines PCA9685 (motor controller) configuration settings and register addresses
typedef struct {
    uint8_t const ADDRESS;          // I2C Address of PCA9685
    uint8_t const PRE_SCALE;        // Prescaler for PWM frequency
    uint8_t const ALL_LED_OFF_H;    // Register to turn off all LEDs
    uint8_t const RESET;            // Command for soft reset

    // Individual LED (motor control pin) registers
    uint8_t const AIN1_ON;      //corresponds to LED4_ON_H control reg 
    uint8_t const AIN1_OFF;     //corresponds to LED4_OFF_H control reg
    uint8_t const AIN2_ON;      // corresponds to LED3_ON_H control reg
    uint8_t const AIN2_OFF;     //corresponds to LED3_OFF_H control reg
    uint8_t const BIN1_ON;      //corresponds to LED5_ON_H control reg
    uint8_t const BIN1_OFF;     //corresponds to LED5_OFF_H control reg
    uint8_t const BIN2_ON;      //corresponds to LED6_ON_H control reg
    uint8_t const BIN2_OFF;     //corresponds to LED6_OFF_H control reg

    // Registers for constant high signal (PWM enabled)
    uint8_t const LED2_ON_H;
    uint8_t const LED7_ON_H;

    // Mode registers
    uint8_t const MODE1_REG;
    uint8_t const MODE2_REG;

    // Initial values to configure modes
    uint8_t const MODE1_OSC_BIT_CLEAR;
    uint8_t const DISABLE_PWM;

    // Step sequence values (AIN1, AIN2, BIN1, BIN2)
    uint8_t const STEP1;    //0101'b
    uint8_t const STEP2;    //1001'b
    uint8_t const STEP3;    //1010'b
    uint8_t const STEP4;    //0110'b

    // PWM output control
    uint8_t const PWM_OUTPUT_ENABLE;
    uint8_t const PWM_OUTPUT_DISABLE;
} PCAConfig_t;

extern const PCAConfig_t PCA_Controller;

typedef enum inputs{
    BIN2,
    BIN1,
    AIN2,
    AIN1
} inputs_t;

//TODO: Make a struct to hold PCA values??? Future implementation
//FIXME: tomorrow finish fixing these functions then you are ready to submit. 

uint8_t calc_prescale(int osc_clk_mhz, int update_rate_khz);

void motor_init(void);

void pca_write_byte(volatile uint8_t ctrl_reg, volatile uint8_t value);

void full_step_motor(int step);
void pca_write_motor_pins(uint8_t stepnum);
void pca_set_motor_pin_state(uint8_t on_register, uint8_t off_register, _Bool state);
void delay(unsigned int counts);
void pca_reset(void);

#endif
