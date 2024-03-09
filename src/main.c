#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../include/BeagleBoneMaster.h"



int main(void) {
    uint32_t ctrl;

    I2C_init();

    ctrl = HWREG(I2C1_BASE + I2C_CON);

    I2C_send_data(0x64); //test address

    //poll bit 1 of the control reg, to see if stop condition detected
    while(ctrl & 0x2){
        //This loop should wait for stop condition
    }

    //stop condition detected and transmission complete
    printf("Test transmission successful\n");


    return 0;
}