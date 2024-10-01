#ifndef MULTIMOD_H_
#define MULTIMOD_H_

#include "multimod_uart.h"
#include "multimod_i2c.h"
#include "multimod_BMI160.h"
#include "multimod_OPT3001.h"
#include "multimod_LaunchpadButtons.h"
#include "multimod_LaunchpadLED.h"

static void Multimod_Init() {
    BMI160_Init();
    OPT3001_Init();
    UART_Init();
    LaunchpadButtons_Init();
    LaunchpadLED_Init();
}

#endif /* MULTIMOD_H_ */
