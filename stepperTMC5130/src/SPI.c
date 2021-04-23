/*
 * SPI.c
 *
 *  Created on: Dec 28, 2020
 *      Author: Kristjan
 */

#include "TMC5130/TMC5130.h"
#include "main.h"
#include "assert.h"

SPI_HandleTypeDef hspi3;
SPI_HandleTypeDef hspi2;

// => SPI wrappers

uint32_t tmc5130_readArray(TMC5130TypeDef *tmc5130, uint8_t *data, size_t length)
{
	uint8_t rxBuff[5] = {0, 0, 0, 0, 0};

	if(tmc5130->spi == &hspi3)
	{
		//HAL_GPIO_WritePin(SPI3_CS_GPIO_Port, SPI3_CS_Pin, GPIO_PIN_RESET);
	}
	else if( tmc5130->spi == &hspi2)
	{
		HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET);
	}

	HAL_Delay(10);

	HAL_SPI_TransmitReceive(tmc5130->spi, data, rxBuff, length, HAL_MAX_DELAY);

	HAL_Delay(10);

	if(tmc5130->spi == &hspi3)
	{
		//HAL_GPIO_WritePin(SPI3_CS_GPIO_Port, SPI3_CS_Pin, GPIO_PIN_SET);
	}
	else if( tmc5130->spi == &hspi2)
	{
		HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET);
	}

	return ((rxBuff[1] << 24) | (rxBuff[2] << 16) | (rxBuff[3] << 8) | rxBuff[4]);

}

void tmc5130_writeArray(TMC5130TypeDef *tmc5130, uint8_t *data, size_t length)
{
	if(tmc5130->spi == &hspi3)
	{
		//HAL_GPIO_WritePin(SPI3_CS_GPIO_Port, SPI3_CS_Pin, GPIO_PIN_RESET);
	}
	else if( tmc5130->spi == &hspi2)
	{
		HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET);
	}

	HAL_Delay(10);

	HAL_SPI_Transmit(tmc5130->spi, data, length, HAL_MAX_DELAY);

	HAL_Delay(10);

	if(tmc5130->spi == &hspi3)
	{
		//HAL_GPIO_WritePin(SPI3_CS_GPIO_Port, SPI3_CS_Pin, GPIO_PIN_SET);
	}
	else if( tmc5130->spi == &hspi2)
	{
		HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET);
	}

}

