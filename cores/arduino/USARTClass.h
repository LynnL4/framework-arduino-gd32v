/*
  Copyright (c) 2011 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _USART_CLASS_
#define _USART_CLASS_

#include "Arduino.h"
#include "Common.h"
#include "gd32vf103_gpio.h"
#include "HardwareSerial.h"
#include "RingBuffer.h"

#include "gd32vf103.h"

class USARTClass : public HardwareSerial
{
  protected:
    RingBuffer *_rx_buffer;

  protected:
    uint32_t _pUsart;
    rcu_periph_enum _RCU_USART;
    uint32_t _tx;
    uint32_t _rx;
    IRQn_Type _IRQn;

  public:
    USARTClass(uint32_t pUsart, uint32_t tx, uint32_t rx, rcu_periph_enum RCU_USART, IRQn_Type IRQn);
    void begin(const uint32_t dwBaudRate) ;
    
    void begin(unsigned long baudrate, uint16_t config);
    void end(void) ;
    int available(void) ;
    int peek(void) ;
    int read(void) ;
    void flush(void) ;
    size_t write(const uint8_t c) ;

    void IrqHandler(void) ;

#if defined __GNUC__ /* GCC CS3 */
    using Print::write ; // pull in write(str) and write(buf, size) from Print
#elif defined __ICCARM__ /* IAR Ewarm 5.41+ */
	// virtual void write(const char *str) ;
	// virtual void write(const uint8_t *buffer, size_t size) ;
#endif

    operator bool() { return true; }; // USART always active
};
extern USARTClass Serial;
extern USARTClass Serial1;
#endif // _USART_CLASS_
