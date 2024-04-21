#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../include/BeagleBoneMaster.h"
#include "../include/MotorControllerLib.h"

#define NUMSTEPS 200
#define STACK_SIZE 1024

//program stacks, used for IRQ Service
volatile unsigned int svc_stack[STACK_SIZE];
volatile unsigned int irq_stack[STACK_SIZE];
//push button flag to start step sequence
volatile int push_button;

int main(void) {
    //initialize button to false
    push_button = 0;
    //Setup stacks with asm (optional and dependant on development env)
    setup_stacks(STACK_SIZE);
    //initialize gpio interrupts and fall edge detection
    gpio1_init();
    //unmask gpio1 interrupt from interrupt controller
    IRQ_init();
    //setup i2c bus
    I2C_init();
    //initialize pca motor controller settings
    motor_init();

    //clear_interrupt_mask_bit() of CPSR;
    uint32_t cpsr;
    asm("MRS %0, CPSR" : "=r" (cpsr));
    cpsr &= ~(1 << 7);
    asm("MSR CPSR_c, %0" :: "r" (cpsr));

    while(1){
        //wait for push_button irq
        while(push_button == 0){
            asm("NOP");
        }
        //Stepper Motor Loop
        int step = 0;
        for(int i = 0; i < NUMSTEPS; i++){
            step = (step % 4) + 1;
            full_step_motor(step);
            //delay between each step for motor to adjust
            delay(400000);

        }
        // Re-enable irq once step sequence is complete
        gpio1_enable_irq(); 
    }
    return 0;
}