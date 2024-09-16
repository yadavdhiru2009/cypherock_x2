/*
 * stmpe811.h
 *
 *  Created on: Sep 15, 2024
 *      Author: Dhiru
 */

#ifndef INC_STMPE811_H_
#define INC_STMPE811_H_


#include "stm32h7xx_hal.h"


#define STMPE811_I2C_ADDRESS           0x82  // I2C address of the STMPE811
#define STMPE811_CHIP_ID_REG           0x00  // Register for Chip ID
#define STMPE811_SYS_CTRL1_REG         0x03  // Register to reset the controller
#define STMPE811_TSC_CTRL_REG          0x40  // Register to enable touch screen
#define STMPE811_TSC_CTRL_XY_EN        0x01  // Bit to enable X and Y
#define STMPE811_TSC_CTRL_EN           0x01  // Bit to enable TSC
#define STMPE811_INT_CTRL_REG          0x09  // Interrupt control register
#define STMPE811_INT_EN                0x01  // Enable interrupt for touch events
#define STMPE811_FIFO_SIZE_REG         0x4C  // Register for FIFO size (number of touch data entries)
#define STMPE811_FIFO_TH_REG           0x4A  // FIFO threshold register
#define STMPE811_TSC_DATA_X_REG        0x4D  // X-coordinate data register
#define STMPE811_TSC_DATA_Y_REG        0x4F  // Y-coordinate data register

#define STMPE811_DEVICE_ID			   0x0811 //ID
#define STMPE811_DEVICE_SOFT_RESET	   0x02 // reset

typedef struct _stmpe811
{
	//variable used for touch_panel
	uint16_t x;
	uint16_t y;
}stmpe811;

HAL_StatusTypeDef DAL_I2C_stmpe811_init(I2C_HandleTypeDef *hi2c);
void DAL_I2C_stmpe811_read(I2C_HandleTypeDef *hi2c, stmpe811 *stmpe811_struct);

#endif /* INC_STMPE811_H_ */
