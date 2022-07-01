/*
 * spiio.c
 *
 *  Created on: Apr 11, 2022
 *      Author: Rui Gomes
 */
#include "spiio.h"

void spiprint(void* str)
{
	HAL_SPI_Transmit_DMA(&hspi1, str, sizeof(str));
	while(getspitxcplt() == false);
}

void startspirx()
{
	HAL_SPI_Receive_DMA(&hspi1, getspirxptr(), 1);
}
