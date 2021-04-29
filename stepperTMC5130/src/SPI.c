/*
 * SPI.c
 *
 *  Created on: Dec 28, 2020
 *      Author: Kristjan
 */

#include "TMC5130/TMC5130.h"
#include "main.h"
#include "assert.h"

// => SPI wrappers

uint32_t tmc5130_readArray(TMC5130TypeDef *tmc5130, uint8_t *data, size_t length)
{
	uint8_t rxBuff[5] = {0, 0, 0, 0, 0};

	HAL_GPIO_WritePin(tmc5130->cs_port, tmc5130->cs_pin, GPIO_PIN_RESET);

	HAL_Delay(10);

	HAL_SPI_TransmitReceive(tmc5130->spi, data, rxBuff, length, HAL_MAX_DELAY);

	HAL_Delay(10);

	HAL_GPIO_WritePin(tmc5130->cs_port, tmc5130->cs_pin, GPIO_PIN_SET);

	return ((rxBuff[1] << 24) | (rxBuff[2] << 16) | (rxBuff[3] << 8) | rxBuff[4]);

}

void tmc5130_writeArray(TMC5130TypeDef *tmc5130, uint8_t *data, size_t length)
{

	HAL_GPIO_WritePin(tmc5130->cs_port, tmc5130->cs_pin, GPIO_PIN_RESET);

	HAL_Delay(10);

	HAL_SPI_Transmit(tmc5130->spi, data, length, HAL_MAX_DELAY);

	HAL_Delay(10);

	HAL_GPIO_WritePin(tmc5130->cs_port, tmc5130->cs_pin, GPIO_PIN_SET);

}

