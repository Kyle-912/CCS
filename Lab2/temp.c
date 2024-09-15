/************************************Includes***************************************/
#include <stdint.h>
#include <stdbool.h>

#include <inc/tm4c123gh6pm.h>
#include <inc/hw_memmap.h>
#include <inc/hw_gpio.h>

#include <driverlib/uartstdio.h>
#include <driverlib/gpio.h>
#include <driverlib/uart.h>
#include <driverlib/sysctl.h>
#include <driverlib/pin_map.h>
#include <driverlib/interrupt.h>
#include <driverlib/timer.h>

#include "MultimodDrivers/multimod.h"

/*************************************Defines***************************************/

/********************************Public Variables***********************************/

uint8_t read_imu_flag = 0;
uint8_t read_opt_flag = 0;
uint8_t toggle_led_flag = 0;
uint8_t print_uart_flag = 0;

/********************************Public Functions***********************************/

void TIMER0A_Handler(void);
void TIMER0B_Handler(void);
void TIMER1A_Handler(void);
void TIMER1B_Handler(void);

void Timer_Init()
{
    // Initialize timers
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); // For UART output and LED toggle
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0))
    {
    }
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1); // For BMI160 and OPT3001 sampling
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER1))
    {
    }

    // Disable timers
    TimerDisable(TIMER0_BASE, TIMER_A); // Timer 0A for UART output
    TimerDisable(TIMER0_BASE, TIMER_B); // Timer 0B for LED toggle
    TimerDisable(TIMER1_BASE, TIMER_A); // Timer 1A for BMI160 sampling
    TimerDisable(TIMER1_BASE, TIMER_B); // Timer 1B for OPT3001 sampling

    // Configure timers as half-width, periodic 16-bit timers
    TimerConfigure(TIMER0_BASE, TIMER_CFG_A_PERIODIC | TIMER_CFG_B_PERIODIC);
    TimerConfigure(TIMER1_BASE, TIMER_CFG_A_PERIODIC | TIMER_CFG_B_PERIODIC);

    // Set prescalers
    // Assuming no prescaler is needed, the system clock is used directly.

    // Load initial timer values
    TimerLoadSet(TIMER0_BASE, TIMER_A, (SysCtlClockGet() / 2));            // 500 ms for UART output
    TimerLoadSet(TIMER0_BASE, TIMER_B, (SysCtlClockGet() / 10));           // 100 ms for LED toggle
    TimerLoadSet(TIMER1_BASE, TIMER_A, (SysCtlClockGet() / 10));           // 100 ms for BMI160 sampling
    TimerLoadSet(TIMER1_BASE, TIMER_B, (SysCtlClockGet() / (1000 / 150))); // 150 ms for OPT3001 sampling

    // Enable timer interrupts
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMB_TIMEOUT);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMB_TIMEOUT);

    // Enable timers
    TimerEnable(TIMER0_BASE, TIMER_A);
    TimerEnable(TIMER0_BASE, TIMER_B);
    TimerEnable(TIMER1_BASE, TIMER_A);
    TimerEnable(TIMER1_BASE, TIMER_B);
}

void Int_Init(void)
{
    IntMasterDisable();

    // Enable timer interrupt, set interrupt priorities
    IntEnable(INT_TIMER0A); // UART output interrupt
    IntEnable(INT_TIMER0B); // LED toggle interrupt
    IntEnable(INT_TIMER1A); // BMI160 sampling interrupt
    IntEnable(INT_TIMER1B); // OPT3001 sampling interrupt

    // Optionally set interrupt priorities (0 being the highest)
    IntPrioritySet(INT_TIMER0A, 0x00);
    IntPrioritySet(INT_TIMER0B, 0x20);
    IntPrioritySet(INT_TIMER1A, 0x40);
    IntPrioritySet(INT_TIMER1B, 0x60);

    IntMasterEnable();
}

void LED_Init(void)
{
    // Enable clock to port F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Set pins [2..0] to output, set as digital
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1, 0x00);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1);
}

/************************************MAIN*******************************************/
// Use timers to enforce specific schedules for each event.
int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    Int_Init();
    Timer_Init();
    UART_Init();

    LED_Init();
    BMI160_Init();
    OPT3001_Init();

    int16_t x_accel_value = 0;
    uint16_t opt_value = 0;

    while (1)
    {
        // Write code to read the x-axis accelerometer value,
        // opt3001 sensor, toggle the red led, and print
        // values out to the console when needed.

        if (print_uart_flag)
        {
            print_uart_flag = 0; // Clear flag after handling
            UARTprintf("Message: Output every 500 ms\n");
        }

        if (read_imu_flag)
        {
            read_imu_flag = 0;                        // Clear flag after handling
            x_accel_value = BMI160_AccelXGetResult(); // Read x-axis acceleration
            UARTprintf("Accelerometer X: %d\n", x_accel_value);
        }

        if (read_opt_flag)
        {
            read_opt_flag = 0;               // Clear flag after handling
            opt_value = OPT3001_GetResult(); // Read optical sensor data
            UARTprintf("Optical: %d\n", opt_value);
        }
