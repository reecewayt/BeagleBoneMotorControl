#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../include/BeagleBoneMaster.h"
#include "../include/MotorControllerLib.h"

//number of steps for stepper motor
#define NUMSTEPS 200
#define DELAY_TIME 0.5           //Count time in seconds

#define EXEC_ASM_INSTRUCTION(instr) asm (instr);

int main(void) {

    //Initialize BeagleBone Peripheral Modules
    gpio1_init();
    timer5_init();
    IRQ_init();
    I2C_init();
    //Initialize adafruit FeatherWing - motor controller
    motor_controller_init();

    //Enable IRQ interrupts in CPSR bit 7
    clear_interrupt_mask_bit();

    while(1){
        //Wait for interrupt, before starting program
        EXEC_ASM_INSTRUCTION("WFI");

        int step = 0;
        for(int i = 0; i < NUMSTEPS; i++){
            step = (step % 4) + 1; //send data
            full_step_motor(step);
            timer5_start(DELAY_TIME);
            EXEC_ASM_INSTRUCTION("WFI");
        }

        gpio1_enable_irq(); //re-enable irq once step sequence is complete

    }
    

    return 0;
}