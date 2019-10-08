/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 * SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */


#include "SPI.h"


#include "gd32vf103.h"
#include <WString.h>

SPIClass SPI;

void SPIClass::begin(uint32_t spi_periph,uint32_t cs,uint32_t miso,uint32_t mosi,uint32_t clk) {


	rcu_periph_clock_enable(RCU_SPI1);

	pinMode(cs, OUTPUT);
 	pinMode(miso, OUTPUT_AF_PP);
	pinMode(mosi, OUTPUT_AF_PP);
 	pinMode(clk, OUTPUT_AF_PP);

	spi = spi_periph;

    /* chip select invalid*/
    SPI_CS_HIGH();

    /* SPI0 parameter config */
    SPI_InitStructure.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    SPI_InitStructure.device_mode          = SPI_MASTER;
    SPI_InitStructure.frame_size           = SPI_FRAMESIZE_8BIT;
    SPI_InitStructure.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    SPI_InitStructure.nss                  = SPI_NSS_SOFT;
    SPI_InitStructure.prescale             = SPI_PSC_8;
    SPI_InitStructure.endian               = SPI_ENDIAN_MSB;
    spi_init(spi, &SPI_InitStructure);
 
    /* set crc polynomial */
    spi_crc_polynomial_set(spi,7);
    /* enable SPI0 */
    spi_enable(spi);

}
void SPIClass::begin() {

	rcu_periph_clock_enable(RCU_SPI1);
	pinMode(SPI_CS, OUTPUT);
 	pinMode(SPI_MISO, OUTPUT_AF_PP);
	pinMode(SPI_MOSI, OUTPUT_AF_PP);
 	pinMode(SPI_CLK, OUTPUT_AF_PP);
 
	spi = SPI1;

    /* chip select invalid*/
    digitalWrite(SPI_CS, HIGH);

    /* SPI0 parameter config */
    SPI_InitStructure.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    SPI_InitStructure.device_mode          = SPI_MASTER;
    SPI_InitStructure.frame_size           = SPI_FRAMESIZE_8BIT;
    SPI_InitStructure.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    SPI_InitStructure.nss                  = SPI_NSS_SOFT;
    SPI_InitStructure.prescale             = SPI_PSC_8;
    SPI_InitStructure.endian               = SPI_ENDIAN_MSB;
    spi_init(spi, &SPI_InitStructure);

    /* set crc polynomial */
    spi_crc_polynomial_set(spi,7);
    /* enable SPI0 */
    spi_enable(spi);

}

void SPIClass::end() {
	
	spi_disable(spi);
}

void SPIClass::setBitOrder(BitOrder bitOrder)
{
	spi_disable(spi);
	if(bitOrder == LSBFIRST)
	{
		SPI_InitStructure.endian = SPI_ENDIAN_LSB;
	}
	else
	{
		SPI_InitStructure.endian = SPI_ENDIAN_MSB;
	}
	spi_init(spi, &SPI_InitStructure);
	spi_enable(spi);
}

void SPIClass::setDataMode(uint16_t mode)
{
	spi_disable(spi);
	if (mode == SPI_MODE0)
	{
		SPI_InitStructure.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
	} 
	else if(mode == SPI_MODE1)
	{
		SPI_InitStructure.clock_polarity_phase = SPI_CK_PL_LOW_PH_2EDGE;
	} 
	else if(mode == SPI_MODE2)
	{
		SPI_InitStructure.clock_polarity_phase = SPI_CK_PL_HIGH_PH_1EDGE;
	} 
	else if(mode == SPI_MODE3)
	{
		SPI_InitStructure.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
	}

	spi_init(spi, &SPI_InitStructure);
	spi_enable(spi);
}

void SPIClass::setClockDivider(uint32_t rate)
{
	uint32_t spi_freq = 0;
	spi_disable(spi);

	if(spi != SPI0){
		spi_freq = rcu_clock_freq_get(CK_APB2);
		
	}else
	{
		spi_freq = rcu_clock_freq_get(CK_APB1);
	}
    if(rate >= (spi_freq/SPI_SPEED_CLOCK_DIV2_MHZ)) {
    SPI_InitStructure.prescale= SPI_PSC_2;
  } else if(rate >= (spi_freq/SPI_SPEED_CLOCK_DIV4_MHZ)) {
    SPI_InitStructure.prescale = SPI_PSC_4;
  } else if (rate >= (spi_freq/SPI_SPEED_CLOCK_DIV8_MHZ)) {
    SPI_InitStructure.prescale = SPI_PSC_8;
  } else if (rate >= (spi_freq/SPI_SPEED_CLOCK_DIV16_MHZ)) {
    SPI_InitStructure.prescale = SPI_PSC_16;
  } else if (rate >= (spi_freq/SPI_SPEED_CLOCK_DIV32_MHZ)) {
    SPI_InitStructure.prescale = SPI_PSC_32;
  } else if (rate >= (spi_freq/SPI_SPEED_CLOCK_DIV64_MHZ)) {
    SPI_InitStructure.prescale = SPI_PSC_64;
  } else if (rate >= (spi_freq/SPI_SPEED_CLOCK_DIV128_MHZ)) {
    SPI_InitStructure.prescale = SPI_PSC_128;
  } else if (rate >= (spi_freq/SPI_SPEED_CLOCK_DIV256_MHZ)) {
    SPI_InitStructure.prescale = SPI_PSC_256;
  } else if (rate >= 380000) {
    SPI_InitStructure.prescale = SPI_PSC_256;
  } 
  else {
    SPI_InitStructure.prescale = SPI_PSC_8;
  }

	spi_init(spi, &SPI_InitStructure);
	spi_enable(spi);
}
byte SPIClass::transfer(byte _data) {
	uint8_t d = _data;
 /* send half word through the SPI0 peripheral */
    while(RESET == spi_i2s_flag_get(spi,SPI_FLAG_TBE));
    spi_i2s_data_transmit(spi,d);
  /* wait to receive a half word */
    while(RESET == spi_i2s_flag_get(spi,SPI_FLAG_RBNE));
    d = spi_i2s_data_receive(spi);
    return d ;

}


uint8_t SPIClass::transfer(uint8_t *buf, uint16_t count){
  uint8_t *buffer = reinterpret_cast<uint8_t *>(buf);
  for (size_t i=0; i<count; i++) {
    *buffer = transfer(*buffer);
    buffer++;
  }
  return count ; 

}
uint16_t SPIClass::transfer16(uint16_t _data) {
	uint16_t d = _data;
 /* send half word through the SPI0 peripheral */
    while(RESET == spi_i2s_flag_get(spi,SPI_FLAG_TBE));
    spi_i2s_data_transmit(spi,d);
  /* wait to receive a half word */
    while(RESET == spi_i2s_flag_get(spi,SPI_FLAG_RBNE));
    d = spi_i2s_data_receive(spi);
    return d ;

}
void SPIClass::beginTransaction(SPISettings settings)
{
	setClockDivider(settings.clk);
	setBitOrder(settings.bOrder);
	setDataMode(settings.dMode);
}
void SPIClass::endTransaction()
{
	
}

void SPIClass::attachInterrupt(void) {
	// Should be enableInterrupt()
}

void SPIClass::detachInterrupt(void) {
	// Should be disableInterrupt()
}

