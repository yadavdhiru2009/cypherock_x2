/*
 * stmpe811.c
 *
 *  Created on: Sep 15, 2024
 *      Author: Dhiru
 */

#include "stmpe811.h"

HAL_StatusTypeDef DAL_I2C_stmpe811_init(I2C_HandleTypeDef *hi2c1)
{
	HAL_StatusTypeDef HAL_status = HAL_OK;
	uint8_t data;

	// Read Chip ID to verify communication
	HAL_I2C_Mem_Read(hi2c1, STMPE811_I2C_ADDRESS, STMPE811_CHIP_ID_REG, 1, &data, 1, HAL_MAX_DELAY);
	if (data != STMPE811_DEVICE_ID) {
		// Error handling, chip ID mismatch
		HAL_status = HAL_ERROR;
	}

	// Reset the STMPE811
	data = STMPE811_DEVICE_SOFT_RESET; // Reset command
	HAL_I2C_Mem_Write(hi2c1, STMPE811_I2C_ADDRESS, STMPE811_SYS_CTRL1_REG, 1, &data, 1, HAL_MAX_DELAY);

	// Wait for reset to complete
	HAL_Delay(10);

	// Enable TSC (Touch Screen Controller)
	data = STMPE811_TSC_CTRL_EN | (STMPE811_TSC_CTRL_XY_EN<<1);
	HAL_I2C_Mem_Write(hi2c1, STMPE811_I2C_ADDRESS, STMPE811_TSC_CTRL_REG, 1, &data, 1, HAL_MAX_DELAY);

	return HAL_status;
}
void DAL_I2C_stmpe811_read(I2C_HandleTypeDef *hi2c1, stmpe811 *stmpe811_struct)
{
	uint8_t read_data[2];
    // Read X coordinate
    HAL_I2C_Mem_Read(hi2c1, STMPE811_I2C_ADDRESS, STMPE811_TSC_DATA_X_REG, 1, read_data, 2, HAL_MAX_DELAY);
    stmpe811_struct->x = (read_data[0] << 8) | read_data[1];

    // Read Y coordinate
    HAL_I2C_Mem_Read(hi2c1, STMPE811_I2C_ADDRESS, STMPE811_TSC_DATA_Y_REG, 1, read_data, 2, HAL_MAX_DELAY);
    stmpe811_struct->y = (read_data[0] << 8) | read_data[1];
}
