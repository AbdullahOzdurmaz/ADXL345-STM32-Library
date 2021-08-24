/*
 * ADXL345.c
 *
 *  Created on: Aug 1, 2021
 *      Author: Abdullah ÖZDURMAZ
 */
#include "ADXL345.h"

SPI_HandleTypeDef *SPIx;
GPIO_TypeDef *csPin_PORT;

uint16_t csPin_Pin;

uint8_t data[6];
uint8_t g_Range;

ADXL345 ADXL;

void ADXL345_Init(SPI_HandleTypeDef *accSPI, GPIO_TypeDef *cs_GPIO_PORT, uint16_t cs_GPIO_Pin, uint8_t range){

	SPIx       = accSPI;
	csPin_PORT = cs_GPIO_PORT;
	csPin_Pin  = cs_GPIO_Pin;
	g_Range = range;

	uint8_t data;
	//DATA_FORMAT(0x31) --> SELF_TEST(D7)=0, SPI(D6)=0, INT_INVERT(D5)=0, D4=X, FULL_RES(D3)=0, JUSTIFY(D2)=0, RANGE(D1-D0)=0-1 -> 00000001 = 1 = 0x01
	//D1  D0   Range
	//0 - 0 =  +-2g
	//0 - 1 =  +-4g
	//1 - 0 =  +-8g
	//1 - 1 =  +-16g

	switch(g_Range){
		case 2:
			data = 0x00;
			ADXL.multiplier_value = 0.0039;
		break;
		case 4:
			data = 0x01;
			ADXL.multiplier_value = 0.0078;
		break;
		case 8:
			data = 0x02;
			ADXL.multiplier_value = 0.0156;
		break;
		case 16:
			data = 0x03;
			ADXL.multiplier_value = 0.0312;
		break;
	}
	ADXL345_Write(DATA_FORMAT, data);	//Justify = LSB

	//POWER_CTL(0x2D) ----> D7=X, D6=X, LINK(D5)=0, AUTO SLEEP(D4)=0, MEASURE(D3)=1, SLEEP(D2)=0, WAKEUP(D1-D0)=0-0 ----> 00001000 = 8 = 0x08
	//D1  D0   Wakeup
	//0 - 0 =  8Hz
	//0 - 1 =  4Hz
	//1 - 0 =  2Hz
	//1 - 1 =  1Hz

	ADXL345_Write(POWER_CTL, 0x00); //Clear all bits

	data = 0x08;
	ADXL345_Write(POWER_CTL, data); //Open measure mod, data rate 8Hz

}

uint8_t ADXL345_Connection(void){

	uint8_t data = GET_DIV_ID;
	uint8_t status;

	HAL_GPIO_WritePin(csPin_PORT, csPin_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(SPIx, &data, 1, 100);
	HAL_SPI_Receive(SPIx, &status, 1, 100);
	HAL_GPIO_WritePin(csPin_PORT, csPin_Pin, GPIO_PIN_SET);

	if (status == DEV_ID)
	    return 1;
	else
	    return 0;

}

void ADXL345_Write(uint8_t addr, uint8_t data){

	uint8_t send[2];

	send[0] = addr | 0x40;
	send[1] = data;

	HAL_GPIO_WritePin(csPin_PORT, csPin_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(SPIx, send, 2, 100);
	HAL_GPIO_WritePin(csPin_PORT, csPin_Pin, GPIO_PIN_SET);

}

void ADXL345_Read(uint8_t addr){

	addr |= 0x80;
	addr |= 0x40;

	HAL_GPIO_WritePin(csPin_PORT, csPin_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(SPIx, &addr, 1, 100);
	HAL_SPI_Receive(SPIx, data, 6, 100);
	HAL_GPIO_WritePin(csPin_PORT, csPin_Pin, GPIO_PIN_SET);

}

void ADXL345_GetAcc(void){

	ADXL345_Read(DATA_X0);

	ADXL.X_RAW = (data[1] << 8) | data[0];
	ADXL.Y_RAW = (data[3] << 8) | data[2];
	ADXL.Z_RAW = (data[5] << 8) | data[4];

	// +-2g = 10 bits, +-4g = 11 bits, +-8g = 12 bits, +-16g = 13 bits
	// +-2g = 0.0039, +-4g = 0.0078, +-8g = 0.0156, +-16g = 0.0312

	ADXL.Ax = (float)ADXL.X_RAW * ADXL.multiplier_value;
	ADXL.Ay = (float)ADXL.Y_RAW * ADXL.multiplier_value;
	ADXL.Az = (float)ADXL.Z_RAW * ADXL.multiplier_value;

}

