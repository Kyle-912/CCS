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

#include <inc/tm4c123gh6pm.h>
#include <inc/hw_i2c.h>

/********************************Public Functions***********************************/

// Buttons_Init
// Initializes buttons on the multimod by configuring the I2C module and
// relevant interrupt pin.
// Return: void
void MultimodButtons_Init()
{
    // 1. Initialize I2C0 (PB2 for SCL, PB3 for SDA)
    I2C_Init(I2C0_BASE);

    // 2. Configure PCA9555 Port 0 as input using BUTTONS_PCA9555_GPIO_ADDR
    I2CMasterSlaveAddrSet(I2C0_BASE, PCA9555_BUTTONS_ADDR, false); // Write to configuration register
    I2CMasterDataPut(I2C0_BASE, 0x06);                             // Select Configuration Register (0x06) for Port 0
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);  // Start condition
    while (I2CMasterBusy(I2C0_BASE))
    {
    }

    I2CMasterDataPut(I2C0_BASE, 0xFF);                             // Set all Port 0 pins as inputs (0xFF)
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH); // Finish condition
    while (I2CMasterBusy(I2C0_BASE))
    {
    }

    // 3. Configure the interrupt pin (PE4) connected to PCA9555 INT
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); // Enable clock to Port E
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE))
    {
    }

    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, BUTTONS_INT_PIN);              // Configure PE4 as input
    GPIOIntTypeSet(GPIO_PORTE_BASE, BUTTONS_INT_PIN, GPIO_FALLING_EDGE); // Interrupt on falling edge
    GPIOIntEnable(GPIO_PORTE_BASE, BUTTONS_INT_PIN);                     // Enable GPIO interrupt on PE4
    IntEnable(INT_GPIOE);                                                // Enable Port E interrupt in NVIC

    // 4. Enable global interrupts
    IntMasterEnable();
}

// MultimodButtons_Get
// Gets the input to GPIO bank 1, [0..7].
// Return: uint8_t
uint8_t MultimodButtons_Get()
{
    uint8_t buttonState = 0;

    // 1. Select Input Register 0 (Port 0) to read button states
    I2CMasterSlaveAddrSet(I2C0_BASE, PCA9555_BUTTONS_ADDR, false); // Address with write intent
    I2CMasterDataPut(I2C0_BASE, 0x00);                             // Select Input Register 0 (0x00)
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);       // Send command
    while (I2CMasterBusy(I2C0_BASE))
    {
    }

    // 2. Switch to read mode to retrieve button states from Input Register 0
    I2CMasterSlaveAddrSet(I2C0_BASE, PCA9555_BUTTONS_ADDR, true); // Address with read intent
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);   // Receive data
    while (I2CMasterBusy(I2C0_BASE))
    {
    }

    // 3. Get the button state from the input register
    buttonState = I2CMasterDataGet(I2C0_BASE);

    // 4. Return the states of buttons SW1-SW4 (masked for the four buttons)
    return buttonState & (SW1 | SW2 | SW3 | SW4);
}
