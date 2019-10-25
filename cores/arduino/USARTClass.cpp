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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "USARTClass.h"
#include "Arduino.h"


USARTClass:: USARTClass(uint32_t pUsart, uint32_t tx, uint32_t rx, rcu_periph_enum RCU_USART, IRQn_Type IRQn)
{
	_rx_buffer = new RingBuffer();
	_rx_buffer->clear();
	_pUsart = pUsart;
	_RCU_USART = RCU_USART;
	_IRQn = IRQn;
	_tx = tx;
	_rx = rx;
}

void USARTClass::begin( const uint32_t dwBaudRate )
{
	
	eclic_global_interrupt_enable();
    eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL3_PRIO1);
	eclic_irq_enable(_IRQn, 1, 0);
	
	// Enable USART Clock
	rcu_periph_clock_enable(_RCU_USART);

	// Configure USART Rx as input floating
	pinMode(_rx, INPUT);
	// Configure USART Tx as alternate function push-pull
	pinMode(_tx, OUTPUT_AF_PP);

    /* USART configure */
    usart_deinit(_pUsart);
    usart_baudrate_set(_pUsart, dwBaudRate);
    usart_word_length_set(_pUsart, USART_WL_8BIT);
    usart_stop_bit_set(_pUsart, USART_STB_1BIT);
    usart_parity_config(_pUsart, USART_PM_NONE);
    usart_hardware_flow_rts_config(_pUsart, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(_pUsart, USART_CTS_DISABLE);
    usart_receive_config(_pUsart, USART_RECEIVE_ENABLE);
    usart_transmit_config(_pUsart, USART_TRANSMIT_ENABLE);
    usart_enable(_pUsart);

    usart_interrupt_enable(_pUsart, USART_INT_RBNE);
}

void USARTClass::begin(unsigned long baudrate, uint16_t config)
{
	begin(baudrate);
}
void USARTClass::end( void )
{
	// clear any received data
	_rx_buffer->_iHead = _rx_buffer->_iTail ;

	// Wait for any outstanding data to be sent
	flush();
  
	usart_disable(_pUsart);

	// Disable USART Clock
	rcu_periph_clock_disable(_RCU_USART);
}

int USARTClass::available( void )
{
	return (uint32_t)(SERIAL_BUFFER_SIZE + _rx_buffer->_iHead - _rx_buffer->_iTail) % SERIAL_BUFFER_SIZE ;
}

int USARTClass::peek( void )
{
	if ( _rx_buffer->_iHead == _rx_buffer->_iTail )
	{
		return -1 ;
	}
	return _rx_buffer->_aucBuffer[_rx_buffer->_iTail] ;
}

int USARTClass::read( void )
{
	// if the head isn't ahead of the tail, we don't have any characters
	if ( _rx_buffer->_iHead == _rx_buffer->_iTail )
	{
		return -1 ;
	}

	uint8_t uc = _rx_buffer->_aucBuffer[_rx_buffer->_iTail] ;
	_rx_buffer->_iTail = (unsigned int)(_rx_buffer->_iTail + 1) % SERIAL_BUFFER_SIZE ;
	return uc ;
}

void USARTClass::flush( void )
{

}

size_t USARTClass::write( const uint8_t uc_data )
{
	// Send one byte from USART
	usart_data_transmit(_pUsart,uc_data);
	// Loop until USART DR register is empty

	while (usart_flag_get(_pUsart, USART_FLAG_TBE)== RESET){
    }
	return 1;
}

void USARTClass::IrqHandler( void )
{
	if(usart_flag_get(_pUsart, USART_FLAG_RBNE)!= RESET)
	{
		// Read one byte from the receive data register
		uint8_t RxBuffer;
	
		RxBuffer = usart_data_receive(_pUsart);
		_rx_buffer->store_char( RxBuffer ) ;
	}
}

extern "C"
{
	void USART0_IRQHandler(void)
	{
		Serial.IrqHandler();
	}

	void USART1_IRQHandler(void)
	{
		Serial1.IrqHandler();
	}
}
USARTClass Serial(USART0,PA9,PA10, RCU_USART0, USART0_IRQn);
USARTClass Serial1(USART1,PA2,PA3,RCU_USART1, USART1_IRQn);