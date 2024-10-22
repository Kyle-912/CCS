// multimod_buttons.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for button functions

/************************************Includes***************************************/

#include "../multimod_buttons.h"

#include "../multimod_i2c.h"

#include <driverlib/gpio.h>
#include <driverlib/sysctl.h>
#include <driverlib/pin_map.h>
#include <driverlib/interrupt.h>

#include <inc/tm4c123gh6pm.h>
#include <inc/hw_i2c.h>

/********************************Public Functions***********************************/

// Buttons_Init
// Initializes buttons on the multimod by configuring the I2C module and
// relevant interrupt pin.
// Return: void
void MultimodButtons_Init()
{
    I2C_Init(I2C1_BASE);

    // uint8_t configData[2] = {0x06, 0xFF}; // 0x06 is the config register, set all pins as inputs
    // I2C_WriteMultiple(I2C1_BASE, PCA9555_BUTTONS_ADDR, configData, 2);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE))
    {
    }
    GPIOPinTypeGPIOInput(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
    GPIOIntTypeSet(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN, GPIO_FALLING_EDGE);
    GPIOIntEnable(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
    IntEnable(INT_GPIOE);
    IntMasterEnable();
}

// MultimodButtons_Get
// Gets the input to GPIO bank 1, [0..7].
// Return: uint8_t
uint8_t MultimodButtons_Get()
{
    uint8_t buttonState = 0;

    /*uint8_t inputRegister[1] = {0x00}; // Command byte for Input Register 0
    I2C_WriteMultiple(I2C1_BASE, PCA9555_BUTTONS_ADDR, inputRegister, 1);

    buttonState = I2C_ReadSingle(I2C1_BASE, PCA9555_BUTTONS_ADDR);*/

    /*I2CMasterSlaveAddrSet(I2C1_BASE, PCA9555_BUTTONS_ADDR, false); // Write mode
    I2CMasterDataPut(I2C1_BASE, 0x00);                             // Select Input Register 0

    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_START); // Start (no stop)
    while (I2CMasterBusy(I2C1_BASE))
    {
    }

    I2CMasterSlaveAddrSet(I2C1_BASE, PCA9555_BUTTONS_ADDR, true);    // Read mode
    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START); // Repeated start
    while (I2CMasterBusy(I2C1_BASE))
    {
    }

    buttonState = I2CMasterDataGet(I2C1_BASE);

    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH); // NACK and stop
    while (I2CMasterBusy(I2C1_BASE))
    {
    }

    return buttonState & (SW1 | SW2 | SW3 | SW4);*/

    I2CMasterSlaveAddrSet(I2C1_BASE, BUTTONS_PCA9555_GPIO_ADDR, false); // Write mode
    I2CMasterDataPut(I2C1_BASE, 0x00);                                  // Select Input Register 0
    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    while (I2CMasterBusy(I2C1_BASE))
    {
    }

    buttonState = I2C_ReadSingle(I2C1_BASE, BUTTONS_PCA9555_GPIO_ADDR);

    return -buttonState;
}
