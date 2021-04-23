/*
 * SPI.h
 *
 *  Created on: Dec 28, 2020
 *      Author: Kristjan
 */

#ifndef INC_SPI_H_
#define INC_SPI_H_

// => SPI wrapper
// Send [length] bytes stored in the [data] array over SPI and overwrite [data]
// with the reply. The first byte sent/received is data[0].

void tmc2130_readWriteArray(uint8_t channel, uint8_t *data, size_t length);


#endif /* INC_SPI_H_ */
