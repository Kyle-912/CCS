// multimod_PCA9956b.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for PCA9956b functions

/************************************Includes***************************************/

#include "../multimod_PCA9956b.h"


#include <driverlib/sysctl.h>
#include <driverlib/pin_map.h>

#include <stdint.h>
#include "../multimod_i2c.h"

/************************************Includes***************************************/

/********************************Public Functions***********************************/

// PCA9956b_Init
// Initializes the PCA9956b, initializes the relevant output enable pins
// Return: void
void PCA9956b_Init() {
    // Initialize I2C module
    I2C_Init(I2C_A_BASE);

    // Configure relevant Output Enable pin

    return;
}

// PCA9956b_SetAllMax
// Writes to the IREFALL, PWMALL registers, sets LEDs to maximum.
// Return: void
void PCA9956b_SetAllMax() {
    // Set IREF to max
    // Set PWM to max
}

// PCA9956b_SetAllOff
// Writes to the IREFALL, PWMALL registers, sets LEDs to off.
// Return: void
void PCA9956b_SetAllOff() {
    // Set IREF to 0
    // Set PWM to 0
}

uint8_t PCA9956b_GetChipID() {
    I2C_WriteSingle(I2C_A_BASE, PCA9956B_ADDR, 0x01);
    return I2C_ReadSingle(I2C_A_BASE, PCA9956B_ADDR);
}

/********************************Public Functions***********************************/
