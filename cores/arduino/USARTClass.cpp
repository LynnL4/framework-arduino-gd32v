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


USARTClass::USARTClass( uint32_t pUsart, IRQn_Type dwIrq, uint32_t dwId, RingBuffer* pRx_buffer )
{
	_rx_buffer = pRx_buffer;
	_pUsart = pUsart;
	_dwIrq = dwIrq;
	_dwId = dwId;
}

void USARTClass::begin( const uint32_t dwBaudRate )
{
	// Enable USART Clock
	
	rcu_periph_clock_enable(RCU_USART0);
	rcu_periph_clock_enable(RCU_USART1);

	// Configure USART Rx as input floating
	pinMode(PA10, INPUT);
	pinMode(PA3, INPUT);
	// Configure USART Tx as alternate function push-pull
	pinMode(PA9, OUTPUT_AF_PP);
  	pinMode(PA2, OUTPUT_AF_PP);

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
}

void USARTClass::begin(unsigned long baudrate, uint16_t config)
{


}
void USARTClass::end( void )
{
	// clear any received data
	_rx_buffer->_iHead = _rx_buffer->_iTail ;

	// Wait for any outstanding data to be sent
	flush();
  
	usart_disable(_pUsart);

	// Disable USART Clock
	rcu_periph_clock_disable(RCU_USART0);
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
	if(usart_flag_get(USART0, USART_FLAG_RBNE)!= RESET)
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
		Serial.IrqHandler();
	}
}
USARTClass Serial(USART0,USART0_IRQn,0,NULL);
USARTClass Serial1(USART1,USART1_IRQn,0,NULL);