/*
 * ADXL345.h
 *
 *  Created on: Aug 1, 2021
 *      Author: Abdullah ÖZDURMAZ
 */

#ifndef INC_ADXL345_H_
#define INC_ADXL345_H_

#include <main.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define DEV_ID 0xE5
#define GET_DIV_ID 0x00

#define OFS_X  0x1E
#define OFS_Y  0x1F
#define OFS_Z  0x20

#define POWER_CTL 0x2D
#define DATA_FORMAT 0x31

#define DATA_X0     0x32
#define DATA_X1		0x33
#define DATA_Y0		0x34
#define DATA_Y1		0x35
#define DATA_Z0		0x36
#define DATA_Z1		0x37

typedef struct{
	signed short X_RAW;
	signed short Y_RAW;
	signed short Z_RAW;

	float Ax;
	float Ay;
	float Az;

	float multiplier_value;
}ADXL345;

void ADXL345_Init(SPI_HandleTypeDef *accSPI, GPIO_TypeDef *cs_GPIO_PORT, uint16_t cs_GPIO_Pin, uint8_t range);

uint8_t ADXL345_Connection(void);

void ADXL345_Write(uint8_t addr, uint8_t data);
void ADXL345_Read(uint8_t addr);

void ADXL345_GetAcc(void);

#endif /* INC_ADXL345_H_ */
