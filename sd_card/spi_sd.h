#ifndef SPI_H_
#define SPI_H_

#include "stm32f10x_spi.h"

void SD_SPI_Config(void);
uint8_t SD_SPI_RW( uint8_t data );

#endif /* SPI_H_ */
