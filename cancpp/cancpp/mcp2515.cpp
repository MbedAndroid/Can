/* Copyright (c) 2007 Fabian Greif
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------


#include <avr/io.h>
#include "main.h"
#include <util/delay.h>

#include "mcp2515.h"
#include "mcp2515_defs.h"

#include "global.h"
#include "defaults.h"

// -------------------------------------------------------------------------
// Schreibt/liest ein Byte ueber den Hardware SPI Bus

uint8_t spi_putc( uint8_t data )
{
	// put byte in send-buffer
	SPDR = data;
	
	// wait until byte was send
	while( !( SPSR & (1<<SPIF) ) )
		;
	
	return SPDR;
}

// -------------------------------------------------------------------------
void mcp2515_write_register( uint8_t adress, uint8_t data )
{
	RESET(MCP2515_CS);
	
	spi_putc(SPI_WRITE);
	spi_putc(adress);
	spi_putc(data);
	
	SET(MCP2515_CS);
}

// -------------------------------------------------------------------------
uint8_t mcp2515_read_register(uint8_t adress)
{
	uint8_t data;
	
	RESET(MCP2515_CS);
	
	spi_putc(SPI_READ);
	spi_putc(adress);
	
	data = spi_putc(0xff);	
	
	SET(MCP2515_CS);
	
	return data;
}

// -------------------------------------------------------------------------
void mcp2515_bit_modify(uint8_t adress, uint8_t mask, uint8_t data)
{
	RESET(MCP2515_CS);
	
	spi_putc(SPI_BIT_MODIFY);
	spi_putc(adress);
	spi_putc(mask);
	spi_putc(data);
	
	SET(MCP2515_CS);
}

// ----------------------------------------------------------------------------
uint8_t mcp2515_read_status(uint8_t type)
{
	uint8_t data;
	
	RESET(MCP2515_CS);
	
	spi_putc(type);
	data = spi_putc(0xff);
	
	SET(MCP2515_CS);
	
	return data;
}
/*********************************************************************************************************
** Function name:           mcp2515_configRate
** Descriptions:            set boadrate
*********************************************************************************************************/
u8 mcp2515_configRate(const u8 canSpeed)
{
	u8 set, cfg1, cfg2, cfg3;
	set = 1;
	switch (canSpeed)
	{
		case (CAN_5KBPS):
		cfg1 = MCP_16MHz_5kBPS_CFG1;
		cfg2 = MCP_16MHz_5kBPS_CFG2;
		cfg3 = MCP_16MHz_5kBPS_CFG3;
		break;

		case (CAN_10KBPS):
		cfg1 = MCP_16MHz_10kBPS_CFG1;
		cfg2 = MCP_16MHz_10kBPS_CFG2;
		cfg3 = MCP_16MHz_10kBPS_CFG3;
		break;

		case (CAN_20KBPS):
		cfg1 = MCP_16MHz_20kBPS_CFG1;
		cfg2 = MCP_16MHz_20kBPS_CFG2;
		cfg3 = MCP_16MHz_20kBPS_CFG3;
		break;
		
		case (CAN_25KBPS):
		cfg1 = MCP_16MHz_25kBPS_CFG1;
		cfg2 = MCP_16MHz_25kBPS_CFG2;
		cfg3 = MCP_16MHz_25kBPS_CFG3;
		break;
		
		case (CAN_31K25BPS):
		cfg1 = MCP_16MHz_31k25BPS_CFG1;
		cfg2 = MCP_16MHz_31k25BPS_CFG2;
		cfg3 = MCP_16MHz_31k25BPS_CFG3;
		break;

		case (CAN_33KBPS):
		cfg1 = MCP_16MHz_33kBPS_CFG1;
		cfg2 = MCP_16MHz_33kBPS_CFG2;
		cfg3 = MCP_16MHz_33kBPS_CFG3;
		break;

		case (CAN_40KBPS):
		cfg1 = MCP_16MHz_40kBPS_CFG1;
		cfg2 = MCP_16MHz_40kBPS_CFG2;
		cfg3 = MCP_16MHz_40kBPS_CFG3;
		break;

		case (CAN_50KBPS):
		cfg1 = MCP_16MHz_50kBPS_CFG1;
		cfg2 = MCP_16MHz_50kBPS_CFG2;
		cfg3 = MCP_16MHz_50kBPS_CFG3;
		break;

		case (CAN_80KBPS):
		cfg1 = MCP_16MHz_80kBPS_CFG1;
		cfg2 = MCP_16MHz_80kBPS_CFG2;
		cfg3 = MCP_16MHz_80kBPS_CFG3;
		break;

		case (CAN_83K3BPS):
		cfg1 = MCP_16MHz_83k3BPS_CFG1;
		cfg2 = MCP_16MHz_83k3BPS_CFG2;
		cfg3 = MCP_16MHz_83k3BPS_CFG3;
		break;

		case (CAN_95KBPS):
		cfg1 = MCP_16MHz_95kBPS_CFG1;
		cfg2 = MCP_16MHz_95kBPS_CFG2;
		cfg3 = MCP_16MHz_95kBPS_CFG3;
		break;

		case (CAN_100KBPS):                                             /* 100KBPS                  */
		cfg1 = MCP_16MHz_100kBPS_CFG1;
		cfg2 = MCP_16MHz_100kBPS_CFG2;
		cfg3 = MCP_16MHz_100kBPS_CFG3;
		break;

		case (CAN_125KBPS):
		cfg1 = MCP_16MHz_125kBPS_CFG1;
		cfg2 = MCP_16MHz_125kBPS_CFG2;
		cfg3 = MCP_16MHz_125kBPS_CFG3;
		break;

		case (CAN_200KBPS):
		cfg1 = MCP_16MHz_200kBPS_CFG1;
		cfg2 = MCP_16MHz_200kBPS_CFG2;
		cfg3 = MCP_16MHz_200kBPS_CFG3;
		break;

		case (CAN_250KBPS):
		cfg1 = MCP_16MHz_250kBPS_CFG1;
		cfg2 = MCP_16MHz_250kBPS_CFG2;
		cfg3 = MCP_16MHz_250kBPS_CFG3;
		break;

		case (CAN_500KBPS):
		cfg1 = MCP_16MHz_500kBPS_CFG1;
		cfg2 = MCP_16MHz_500kBPS_CFG2;
		cfg3 = MCP_16MHz_500kBPS_CFG3;
		break;
		
		case (CAN_666KBPS):
		cfg1 = MCP_16MHz_666kBPS_CFG1;
		cfg2 = MCP_16MHz_666kBPS_CFG2;
		cfg3 = MCP_16MHz_666kBPS_CFG3;
		break;
		
		case (CAN_1000KBPS):
		cfg1 = MCP_16MHz_1000kBPS_CFG1;
		cfg2 = MCP_16MHz_1000kBPS_CFG2;
		cfg3 = MCP_16MHz_1000kBPS_CFG3;
		break;

		default:
		set = 0;
		break;
	}
	
	if (set) {
		mcp2515_write_register(CNF1,cfg1);
		mcp2515_write_register(CNF2,cfg2);
		mcp2515_write_register(CNF3,cfg3);// Bitrate 100 kbps at 8 MHz

		// test if we could read back the value => is the chip accessible?
		if ((mcp2515_read_register(CNF1) != cfg1) &  (mcp2515_read_register(CNF2) != cfg2) & (mcp2515_read_register(CNF3) != cfg3)) return false;
		else return true;
	}
	else {
		return false;
	}
}
// -------------------------------------------------------------------------
bool mcp2515_init(void)
{
	SET(MCP2515_CS);
	SET_OUTPUT(MCP2515_CS);
	
	RESET(P_SCK);
	RESET(P_MOSI);
	RESET(P_MISO);
	
	SET_OUTPUT(P_SCK);
	SET_OUTPUT(P_MOSI);
	SET_INPUT(P_MISO);
	
	SET_INPUT(MCP2515_INT);
	SET(MCP2515_INT);
	
	// active SPI master interface
	SPCR = (1<<SPE)|(1<<MSTR) | (0<<SPR1)|(1<<SPR0);
	SPSR = 0;
	
	// reset MCP2515 by software reset.
	// After this he is in configuration mode.
	RESET(MCP2515_CS);
	spi_putc(SPI_RESET);
	SET(MCP2515_CS);
	
	// wait a little bit until the MCP2515 has restarted
	_delay_us(10);
	if (!mcp2515_configRate(CAN_200KBPS)) return false;
	
//
	// activate interrupts
	mcp2515_write_register(CANINTE,(1<<RX1IE)|(1<<RX0IE));//| (1<<MERRE) | (1<<ERRIE)); 
	
	
	// deaktivate the RXnBF Pins (High Impedance State)
	mcp2515_write_register(BFPCTRL, 0);
	// set TXnRTS as inputs
	mcp2515_write_register(TXRTSCTRL, 0);
	
	// turn off filters => receive any message
	mcp2515_write_register(RXB0CTRL, (1<<RXM1)|(1<<RXM0));
	mcp2515_write_register(RXB1CTRL, (1<<RXM1)|(1<<RXM0));
	
	// reset device to normal mode
	mcp2515_write_register(CANCTRL, 0);

	return true;
}

// ----------------------------------------------------------------------------
// check if there are any new messages waiting

uint8_t mcp2515_check_message(void) {
	return (!IS_SET(MCP2515_INT));
}

// ----------------------------------------------------------------------------
// check if there is a free buffer to send messages

bool mcp2515_check_free_buffer(void)
{
	uint8_t status = mcp2515_read_status(SPI_READ_STATUS);
	
	if ((status & 0x54) == 0x54) {
		// all buffers used
		return false;
	}
	
	return true;
}

// ----------------------------------------------------------------------------
uint8_t mcp2515_get_message(tCAN *message)
{
	// read status
	uint8_t status = mcp2515_read_status(SPI_RX_STATUS);
	uint8_t addr;
	
	if (bit_is_set(status,6)) {
		// message in buffer 0
		addr = SPI_READ_RX;
	}
	else if (bit_is_set(status,7)) {
		// message in buffer 1
		addr = SPI_READ_RX | 0x04;
	}
	else {
		// Error: no message available
		return 0;
	}

	RESET(MCP2515_CS);
	spi_putc(addr);
	
	// read id
	message->id  = (uint16_t) spi_putc(0xff) << 3;
	message->id |=            spi_putc(0xff) >> 5;
	
	spi_putc(0xff);
	spi_putc(0xff);
	
	// read DLC
	uint8_t length = spi_putc(0xff) & 0x0f;
	
	message->header.length = length;
	message->header.rtr = (bit_is_set(status, 3)) ? 1 : 0;
	
	// read data
	for (uint8_t i=0;i<length;i++) {
		message->data[i] = spi_putc(0xff);
	}
	SET(MCP2515_CS);
	
	// clear interrupt flag
	if (bit_is_set(status, 6)) {
		mcp2515_bit_modify(CANINTF, (1<<RX0IF), 0);
	}
	else {
		mcp2515_bit_modify(CANINTF, (1<<RX1IF), 0);
	}
	
	return (status & 0x07) + 1;
}

// ----------------------------------------------------------------------------
uint8_t mcp2515_send_message(tCAN *message)
{
	uint8_t status = mcp2515_read_status(SPI_READ_STATUS);
	
	/* Statusbyte:
	 *
	 * Bit	Function
	 *  2	TXB0CNTRL.TXREQ
	 *  4	TXB1CNTRL.TXREQ
	 *  6	TXB2CNTRL.TXREQ
	 */
	uint8_t address;
	if (bit_is_clear(status, 2)) {
		address = 0x00;
	}
	else if (bit_is_clear(status, 4)) {
		address = 0x02;
	} 
	else if (bit_is_clear(status, 6)) {
		address = 0x04;
	}
	else {
		// all buffer used => could not send message
		return 0;
	}
	
	RESET(MCP2515_CS);
	spi_putc(SPI_WRITE_TX | address);
	
	spi_putc(message->id >> 3);
    spi_putc(message->id << 5);
	
	spi_putc(0);
	spi_putc(0);
	
	uint8_t length = message->header.length & 0x0f;
	
	if (message->header.rtr) {
		// a rtr-frame has a length, but contains no data
		spi_putc((1<<RTR) | length);
	}
	else {
		// set message length
		spi_putc(length);
		
		// data
		for (uint8_t i=0;i<length;i++) {
			spi_putc(message->data[i]);
		}
	}
	SET(MCP2515_CS);
	
	_delay_us(1);
	
	// send message
	RESET(MCP2515_CS);
	address = (address == 0) ? 1 : address;
	spi_putc(SPI_RTS | address);
	SET(MCP2515_CS);
	
	return address;
}
/*********************************************************************************************************
** Function name:           mcp2515_read_id
** Descriptions:            read can id
*********************************************************************************************************/
void mcp2515_read_id( const u8 mcp_addr, u8* ext, u32* id )
{
	u8 tbufdata[4];

	*ext = 0;
	*id = 0;

	mcp2515_readRegisterS( mcp_addr, tbufdata, 4 );

	*id = (tbufdata[RXF0SIDH]<<3) + (tbufdata[RXF0SIDL]>>5);

	if ( (tbufdata[RXF0SIDL] & RXF2SIDH) ==  RXF2SIDH )
	{
		/* extended id                  */
		*id = (*id<<2) + (tbufdata[RXF0SIDL] & 0x03);
		*id = (*id<<8) + tbufdata[RXF0EID8];
		*id = (*id<<8) + tbufdata[RXF0EID8];
		*ext = 1;
	}
}

/*********************************************************************************************************
** Function name:           mcp2515_readRegisterS
** Descriptions:            read registerS
*********************************************************************************************************/
void mcp2515_readRegisterS(const u8 address, u8 values[], const u8 n)
{
	u8 i;

	RESET(MCP2515_CS);

	spi_putc(SPI_READ);
	spi_putc(address);
	// mcp2515 has auto-increment of address-pointer
	for (i=0; i<n && i<CAN_MAX_CHAR_IN_MESSAGE; i++) {
		values[i] = spi_putc(0xff) & 0x0f;
	}
	SET(MCP2515_CS);

}