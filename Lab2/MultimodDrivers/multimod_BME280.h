#ifndef MULTIMOD_BME280_H
#define MULTIMOD_BME280_H

#include <stdint.h>

// BME280 I2C Address
#define BME280_I2C_ADDR 0x76 // Default I2C address, adjust if needed

// BME280 Register Addresses
#define BME280_REG_TEMP_XLSB 0xFC
#define BME280_REG_TEMP_LSB 0xFB
#define BME280_REG_TEMP_MSB 0xFA
#define BME280_REG_CTRL_MEAS 0xF4
#define BME280_REG_RESET 0xE0

// Temperature Data Structure
typedef struct
{
    float temperature; // Temperature in Celsius
} BME280_Data;

// Function Prototypes
void BME280_Init(void);
BME280_Data BME280_ReadTemperature(void);

#endif // MULTIMOD_BME280_H
