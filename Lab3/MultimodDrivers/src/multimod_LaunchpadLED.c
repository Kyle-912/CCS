// multimod_OPT3001.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for OPT3001 functions

/************************************Includes***************************************/

#include "../multimod_LaunchpadLED.h"

#include <stdint.h>
#include <stdbool.h>

#include <inc/tm4c123gh6pm.h>
#include <inc/hw_types.h>
#include <inc/hw_memmap.h>
#include <inc/hw_gpio.h>

#include <driverlib/pin_map.h>
#include <driverlib/pwm.h>
#include <driverlib/gpio.h>
#include <driverlib/sysctl.h>

/************************************Includes***************************************/

/********************************Public Functions***********************************/

// LaunchpadButtons_Init
// Initializes the GPIO port & pins necessary for the button switches on the
// launchpad. Also configures it so that the LEDs are controlled via PWM signal.
// Initial default period of 400.
// Return: void
void LaunchpadLED_Init()
{
    // Enable clock to port F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Enable PWM module

    // Configure necessary pins as PWM

    // Configure necessary PWM generators in count down mode, no sync

    // Set generator periods

    // Set the default pulse width (duty cycles).

    // Enable the PWM generators

    // Enable PWM output
}

// LaunchpadLED_PWMSetDuty
// Sets the duty cycle of the PWM generator associated with the LED.
// Return: void
void LaunchpadLED_PWMSetDuty(LED_Color_t LED, float duty)
{
    uint32_t pulse_width = (duty * PWM_Per);

    // If pulse width < 1, set as 1
    if (pulse_width < 1)
    {
        pulse_width = 1;
    }

    // If pulse width > set period, cap it
    if (pulse_width > PWM_Per)
    {
        pulse_width = PWM_Per - 1;
    }

    // Depending on chosen LED(s), adjust corresponding duty cycle of the PWM output
    // Your code below.

    return;
}

/********************************Public Functions***********************************/
