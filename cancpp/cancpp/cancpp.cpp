/*
* cancpp.cpp
*
* Created: 8-7-2017 21:52:50
*  Author: bert
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "main.h"
#include <avr/sleep.h>
#include <stdio.h>
#include <inttypes.h>

//#include "uart.h"
#include "mcp2515.h"
#include "global.h"
#include "defaults.h"
#include "therm_ds18b20.h"
#include <util/delay.h>
#include "Timers.h"
#include "avr/wdt.h"
#include <avr/eeprom.h>
#include "tfa.h"
#include "DecodeOOK.h"
#include "wmr968.h"
extern unsigned char data[];
static int pulse_buffer[PB_SIZE];
static volatile int pb_head = 0;
static volatile int pb_tail = 0;
OregonDecoderV2 orscV2;
UCHAR ROM[serial_l];
tCAN message;
char buf[60];
CANDEF	Can1;
void init(void)
{
	SET(P_CANRESET);
	SET_OUTPUT(P_CANRESET);
	_delay_ms(10);
	RESET(P_CANRESET);
	RESET(REL);
	SET_OUTPUT(REL);
	SET(LED);
	SET_OUTPUT(LED);
	SET_INPUT(A1);
	SET_INPUT(A2);
	SET_INPUT(A3);
	SET_INPUT(A4);
	SET(A1);
	SET(A2);
	SET(A3);
	SET(A4);
}
void LoopBackTest (void)
{
	// einige Testwerte
	message.id = 0x123;
	message.header.rtr = 0;
	message.header.length = 2;
	message.data[0] = 0xab;
	message.data[1] = 0xcd;
	
	
	mcp2515_bit_modify(CANCTRL, (1<<REQOP2)|(1<<REQOP1)|(1<<REQOP0), (1<<REQOP1)); //set mcp in loopback mode
	// Sende eine Nachricht
	if (mcp2515_send_message(&message)) {

	}
	else {

	}
	
	// warte ein bisschen
	_delay_ms(10);
	
	if (mcp2515_check_message()) {

		//therm_read_temperature();
		// read the message from the buffers
		if (mcp2515_get_message(&message)) 	Blink(1);	else {
			
			Blink(4);
		}
		
	}
	else Blink(6);
	
	mcp2515_bit_modify(CANCTRL, (1<<REQOP2)|(1<<REQOP1)|(1<<REQOP0), 0); //set mcp back to normal mode
}
void ResetEeprom(void)
{
	u16 randomNumber=0;
	u8 i;
	for (i=0;i<16;i++) {randomNumber|=ReadAD() & 1;
	randomNumber<<=1;}
	
	while ((randomNumber & 0x7f)==0)	 randomNumber>>=1;
	Blink(5);
	write_eeprom_dword(CHECKSUM_POS,CHECKSUM);
	write_eeprom_word(SerialNumberloc,randomNumber & 0x7f);
	usart_pstr("Eprom Init\r\n");
	
}
void ControleerChecksum(void)
{
	if (read_eeprom_dword(CHECKSUM_POS)!=CHECKSUM) {ResetEeprom();Can1.state=ST_PREOPERATIONAL;return;}
	Can1.state=BootUp;
}


#ifdef TFA
ISR(TIMER1_CAPT_vect)
{
	static int lastcount=0;
	int width;
	width=lastcount-ICR1;
	if (200 <= width && width < 1200) {  pulse_buffer[pb_head] = width;
		pb_head = (pb_head + 1) % PB_SIZE;
	}
	lastcount=TCNT1;
}
#endif



#ifdef TFA
void reportTFA(void)
{
	if ((data[3] & 0x1f)==0x1E)  OutputDataTFA(2);
	if ((data[3] & 0x1f)==0x0E)  OutputDataRainTFA(1);
	if ((data[3] & 0x1f)==0x0C)  OutputDataWindTFA(0);
	// if ((data[3] & 0x1f)==0x0D)  OutputDataRainTFA(0); //uv sensor
}
#endif
int main(void)
{
	wdt_reset();
	MCUSR=0;
	WDTCSR|=_BV(WDCE) | _BV(WDE);
	WDTCSR=0;
	
	init();
	InitTimer();
	EnableInterrupt(0);
	
	USART_Init(MYUBRR);

	usart_pstr("start-up Uart\r\n");
	// Aktiviere Interrupts
	
	// Versuche den MCP2515 zu initilaisieren
	if (!mcp2515_init()) {
		usart_pstr("MCP2515 Init failure\r\n");
		Blink(6);
	}
	else Blink(1);

	//
	Can1.id=0x7f;
	Can1.state=ST_PREOPERATIONAL;
	read_serial();
	sprintf(buf,"serial:%d %d %d %d %d %d \r\n",ROM[1],ROM[2],ROM[3],ROM[4],ROM[5],ROM[6]);
	ROM[1]=1;
	ROM[2]=2;
	ROM[3]=3;
	ROM[4]=4;
	usart_pstr(buf);
	//ControleerChecksum();
	//Can1.id=(u8) read_eeprom_word(SerialNumberloc);
	//Can1.id=(u8) 123456;
	//sprintf(buf,"SerialNumber:%d\r\n",Can1.id);
	

	LoopBackTest();
	_delay_ms(200);

	SendPDO(PDO4);
	usart_pstr("Can Request CanID\r\n");
	

	PCMSK1=(1<<PCINT9) | (1<<PCINT10) | (1<<PCINT11) |(1<<PCINT12);
	PCICR=(1<<PCIF1);

	wdt_enable (WDTO_8S);
	sei();
	#ifndef TFA
	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_mode();
	#endif

	while(1)
	{
		#ifdef TFA
		int p = 0;
		if (pb_head != pb_tail) {
			p = pulse_buffer[pb_tail];
			pb_tail = (pb_tail + 1) % PB_SIZE;
		}

		if (p != 0) {
			if (orscV2.nextPulse(p)) {
				if (orscV2.nextPulse(p)) {
					// puls=!puls;

					 //reportSerial("OSV2", orscV2);
				}
			}
			if (nextPulseTFA(p)) {
				// puls=!puls;

				reportTFA();

			}
		}
		#endif
		//TODO:: Please write your application code
	}
}