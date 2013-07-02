#include "spi_sd.h"

void SD_SPI_Config(void) 
{

	SPI_InitTypeDef Spi_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //CS
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 |  GPIO_Pin_14 | GPIO_Pin_15; //SCK, MOSI
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	Spi_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	Spi_InitStructure.SPI_Mode = SPI_Mode_Master;
	Spi_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	Spi_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	Spi_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	Spi_InitStructure.SPI_NSS = SPI_NSS_Soft;
	Spi_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
	Spi_InitStructure.SPI_FirstBit =SPI_FirstBit_MSB;
	Spi_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI2, &Spi_InitStructure);
	SPI_Cmd(SPI2, ENABLE);
}

uint8_t SD_SPI_RW( uint8_t data )
{
	/* Send byte through the SPI1 peripheral */
	SPI2->DR = data;

	/* Wait to receive a byte */
	while ((SPI2->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET); 

	/* Return the byte read from the SPI bus */
	return SPI2->DR;
}



