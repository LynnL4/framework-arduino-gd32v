/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 * SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef _SPI_H_INCLUDED
#define _SPI_H_INCLUDED

#include <stdio.h>
#include <Arduino.h>

#define SPI_SPEED_CLOCK_DIV2_MHZ    ((uint32_t)2)
#define SPI_SPEED_CLOCK_DIV4_MHZ    ((uint32_t)4)
#define SPI_SPEED_CLOCK_DIV8_MHZ    ((uint32_t)8)
#define SPI_SPEED_CLOCK_DIV16_MHZ   ((uint32_t)16)
#define SPI_SPEED_CLOCK_DIV32_MHZ   ((uint32_t)32)
#define SPI_SPEED_CLOCK_DIV64_MHZ   ((uint32_t)64)
#define SPI_SPEED_CLOCK_DIV128_MHZ  ((uint32_t)128)
#define SPI_SPEED_CLOCK_DIV256_MHZ  ((uint32_t)256)



#define SPI_MODE0 0x00
#define SPI_MODE1 0x04
#define SPI_MODE2 0x08
#define SPI_MODE3 0x0C

#define  SPI_FLASH_PAGE_SIZE       0x100
#define  SPI_CS_LOW()       digitalWrite(SPI_CS, LOW)
#define  SPI_CS_HIGH()      digitalWrite(SPI_CS, HIGH)




#define	SPI_CS    PB1
#define	SPI_CLK   PB13
#define	SPI_MISO  PB14
#define	SPI_MOSI  PB15

class SPISettings {
  public:
    SPISettings(uint32_t clock, BitOrder bitOrder, uint8_t dataMode) {
      clk = clock;

      if(bitOrder == MSBFIRST) {
        msb = 1;
      } else {
        msb = 0;
      }
      bOrder = bitOrder;

      if(SPI_MODE0 == dataMode) {
        dMode = SPI_MODE0;
      } else if(SPI_MODE1 == dataMode) {
        dMode = SPI_MODE1;
      } else if(SPI_MODE2 == dataMode) {
        dMode = SPI_MODE2;
      } else if(SPI_MODE3 == dataMode) {
        dMode = SPI_MODE3;
      }

    }
    SPISettings() {
      pinCS = PB1;
      clk = SPI_PSC_8;
      bOrder = MSBFIRST;
      msb = 1;
      dMode = SPI_MODE0;
    }
  private:
    int16_t pinCS;      //CS pin associated to the configuration
    uint32_t clk;       //specifies the spi bus maximum clock speed
    BitOrder bOrder;    //bit order (MSBFirst or LSBFirst)
    int16_t dMode;   //one of the data mode
                        //Mode          Clock Polarity (CPOL)   Clock Phase (CPHA)
                        //SPI_MODE0             0                     0
                        //SPI_MODE1             0                     1
                        //SPI_MODE2             1                     0
                        //SPI_MODE3             1                     1
    uint8_t msb;        //set to 1 if msb first
    friend class SPIClass;
};

class SPIClass {
public:
  byte transfer(byte _data);
  
  uint8_t transfer(uint8_t *_data,uint16_t len);
  uint16_t transfer16(uint16_t _data);
  // SPI Configuration methods

  inline static void attachInterrupt();
  inline static void detachInterrupt(); // Default
  void begin();
  void begin(uint32_t spi_periph,uint32_t cs,uint32_t miso,uint32_t mosi,uint32_t clk); // Default
  void beginTransaction(SPISettings settings);
  void end();
  void endTransaction();

  
  void setBitOrder(BitOrder);
  void setDataMode(uint16_t);
  void setClockDivider(uint32_t);
private:
  uint32_t spi;
   
  spi_parameter_struct SPI_InitStructure;
  
  uint16_t bitOrder;
};

extern SPIClass SPI;



#endif
