#ifndef MULTIMOD_BME280_H
#define MULTIMOD_BME280_H

#include <stdint.h>

// BME280 I2C Address
#define BME280_I2C_ADDR 0x76 // Default I2C address, can be 0x77 depending on the configuration

// BME280 Register Addresses
#define BME280_REG_TEMP_XLSB 0xFC
#define BME280_REG_TEMP_LSB 0xFB
#define BME280_REG_TEMP_MSB 0xFA
#define BME280_REG_CTRL_MEAS 0xF4
#define BME280_REG_RESET 0xE0
#define BME280_REG_ID 0xD0

// BME280 Calibration Data Registers
#define BME280_CALIB_DATA_START 0x88

// Function Prototypes
void BME280_Init(void);
void BME280_ReadTemperature(void);
void BME280_WriteRegister(uint8_t addr, uint8_t data);
void BME280_ReadRegister(uint8_t addr, uint8_t *data, uint8_t len);
float BME280_CompensateTemperature(int32_t adc_T);

// Calibration data variables
extern uint16_t dig_T1;
extern int16_t dig_T2, dig_T3;

#endif // MULTIMOD_BME280_H
