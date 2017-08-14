/*
* CFile1.c
*
* Created: 13-3-2017 13:55:08
*  Author: bert
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "main.h"
#include "global.h"
#include "defaults.h"
#include <stdio.h>
#include <inttypes.h>
#include "main.h"
#include "Timers.h"
#include <util/delay.h>
#include "mcp2515.h"
#include "avr/wdt.h"

extern tCAN message;
extern 	char buf[];
extern CANDEF	Can1;
u16 systemtimer;
timers_p HeartBeat,WDT_timer,Req_timer;
extern UCHAR ROM[];

u8	CompareTime(struct TIMER *timer)
{
	if (timer->flag==0) return 1;
	if (timer->countdown==0) {timer->flag=0;return 1;} //safety
	if (timer->scounter!=systemtimer) {timer->countdown--;timer->scounter=systemtimer;}
	if (timer->countdown==0) {timer->flag=0;return 1;} else return 0;
}

void	StartTimer(struct TIMER *timer, u16 counts)
{
	timer->flag=1;
	timer->countdown=counts;
	timer->scounter=systemtimer;
}
void ControleerCanId(void)
{
u8 i;
if (Can1.state!=ST_PREOPERATIONAL) return;
for (i=0;i<4;i++) if (message.data[1+i]!=ROM[1+i]) {usart_pstr("Serial not ok");return;}
Can1.id=message.id & 0x7f;
if (message.data[0] & 1) SET(REL); else RESET(REL);
Can1.state=Operational;
usart_pstr("CanId accepted\r\n");
}

void SwitchLed(void)
{
if (IS_SET(LED)) RESET(LED); else SET(LED);	
}
void DecodeNMTMessage(void)
{
u8 i;
	sprintf(buf,"%d-%d\r\n",message.id,Can1.id);
	usart_pstr(buf);
	switch (message.id & 0x780)
	{
		case PDOR4: {SwitchLed();ControleerCanId();break;}	
	}
	
	if ((message.id & 0x7f)==Can1.id ) {
		SwitchLed();
		
		switch (message.id & 0x780)
		{
			case PDOR1: {
				for (i=0;i<8;i++) {sprintf(buf,"%d ",message.data[i]);usart_pstr(buf);}
				usart_pstr("data\r\n");
				if (message.data[0] & 1) SET(REL); else RESET(REL);
				break;
			}
			case PDOR2: break;
			case PDOR3: break;
			case SDOR: break;
		}
	} else  if  ((message.id==0)	&  ((message.data[1]==Can1.id) | (message.data[1]==0) ))
	{
		SwitchLed();
		usart_pstr("NMTcontrol1");
		switch (message.data[0])
		{
			case Start_Remote_Node: break;
			case Stop_Remote_Node: break;
			case Enter_Preperational_State: break;
			case Reset_Node:{
							do                          \
							{                           \
								wdt_enable(WDTO_15MS);  \
								for(;;)                 \
								{                       \
								}                       \
							} while(0);
							break;}
			case ResetCommunication:mcp2515_init();break;
			default: usart_pstr("Error CS\r\n");break;
		}
	}

}
// Hauptprogram
void ErrorService (u8 flag)
{
mcp2515_bit_modify(CANINTF, (1<<MERRF), 0);	
mcp2515_bit_modify(CANINTF, (1<<ERRIF), 0);	
	message.id = Can1.id | SDO;
	message.header.rtr = 0;
	message.header.length = 2;
	message.data[0] = flag;
	message.data[1]=mcp2515_read_register(EFLG);
	mcp2515_send_message(&message);
	mcp2515_bit_modify(CANINTF, (1<<MERRF), 0);
	mcp2515_bit_modify(CANINTF, (1<<ERRIF), 0);
	usart_pstr("ErrorCondition found");
	mcp2515_init();
	_delay_ms(255);
	}



ISR(INT0_vect)
{

	u8 flags;

	flags=mcp2515_read_register(CANINTF);
	if (flags & (1<<MERRF)) {ErrorService(1);}
	if (flags & (1<<ERRIF)) {ErrorService(2);}
	if (mcp2515_check_message()) {
		
		// Lese die Nachricht aus dem Puffern des MCP2515
		if (mcp2515_get_message(&message)) DecodeNMTMessage();
		
	}
}
ISR(PCINT1_vect) {    // Interrupt service routine. Every single PCINT8..14 (=ADC0..5) change
	// will generate an interrupt: but this will always be the same interrupt routine

if (IS_SET(A1) & !IS_SET(REL)) SET(REL);
if (!IS_SET(A1) & IS_SET(REL)) RESET(REL);
usart_pstr("Anoloog input");
SendPDO(PDO1);
}
void SendPDO(u16 service)
{
	u8 a=0;
	SwitchLed();
	//service= NMT_GUARD;

	if (IS_SET(RELSTAT)) a=1;
	message.id = Can1.id | service;
	message.header.rtr = 0;
	message.header.length = 6;
	message.data[0] = PORTC>>1;
	message.data[1] = a;
	for (a=0;a<4;a++) message.data[2+a]=ROM[1+a];

	mcp2515_send_message(&message);	
}
void SendHeartBeat(u8 state)
{
	if (Can1.state==ST_PREOPERATIONAL) return;

	u8 a=0;
	u16 res=ReadAD();
	SwitchLed();

	if (IS_SET(RELSTAT)) a=1;
		sprintf(buf,"SendToMbed:%d\r\n",a);
		usart_pstr(buf);
	message.id = Can1.id | NMT_GUARD;
	message.header.rtr = 0;
	message.header.length = 6;
	message.data[0] = state;
	message.data[1] = a;
	message.data[2]=res & 0xff;
	message.data[3]=res>>8;
	message.data[4]=mcp2515_read_register(TEC);	
	message.data[5]=mcp2515_read_register(REC);	
	mcp2515_send_message(&message);
}

void Blink(char nr)
{
	char i;
	for (i=0;i<nr;i++) {SET(LED);
		_delay_ms(100);
		RESET(LED);
	_delay_ms(100);}
}

void	InitTimer(void)
{
	TCCR1A=0;
	TCCR1B=(1<<CS12) | (1<<CS10);
	TIMSK1=(1<<TOIE1);
	TCNT1=TIMEROVERFLOW;
	systemtimer=0;

	StartTimer(&HeartBeat,6000);
	StartTimer(&WDT_timer,60);
	StartTimer(&Req_timer,30);
}

void	InitTimer2(void)
{
	TCCR2A=0;
	TCCR2B=(1<<CS22) | (1<<CS20);
	TIMSK2=(1<<TOIE2);	

}

ISR(TIMER1_OVF_vect)
{
	TCNT1=TIMEROVERFLOW; //100 hz interrupt
	systemtimer++;
	//usart_pstr("TEST");
	if (CompareTime(&HeartBeat)>0) {StartTimer(&HeartBeat,6000);SendHeartBeat(Can1.state);}
	if (CompareTime(&WDT_timer)>0) {StartTimer(&WDT_timer,60);wdt_reset();}
	if (CompareTime(&Req_timer)>0) {if (Can1.state==ST_PREOPERATIONAL) {SendPDO(PDO4);StartTimer(&Req_timer,30);usart_pstr("Can Request CanID\r\n");}}
	
}

///////////////////////////////////////
void EnableInterrupt(char nr)
{
	if (nr==0) EIMSK|=1<<INT0; 	else EIMSK|=1<<INT1;
}
void DisableInterrupt(char nr)
{
	if (nr==0) EIMSK&=~(1<<INT0); 	else EIMSK&=~(1<<INT1);
}
// ----------------------------------------------------------------------------
/////////////////////////////////


void USART_Init(unsigned int ubrr)
{
	/*
	Set baud rate
	*/
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char	)ubrr;
	/*Enable receiver and transmitter
	*/
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/*
	Set frame format: 8data, 2stop bit
	*/
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

void usart_putchar(char data) {
	// Wait for empty transmit buffer
	while ( !(UCSR0A & (1<<UDRE0)) );
	// Start transmission
	UDR0 = data;
}

void usart_pstr(char const *s)
{
	
	// loop through entire string
	while (*s) {
		usart_putchar(*s);
		s++;
	}

}

u16 ReadAD(void)
{
	u16	res;
	ADMUX = (1<<REFS0); //(1<<MUX0) | (1<<MUX2) | (1<<MUX1) ;// MUX set to ADC3. VCC as ref. Right justified
	ADCSRA = (1<<ADEN) | (1<<ADPS2); // turn ADC on and initialise. no auto-trigger, no interrupt. prescaler to div2
	DIDR0=(1<<ADC0D);
	_delay_ms(10);
	//start a conversion
	ADCSRA |= (1<<ADSC);	
	while (ADCSRA & (1<<ADSC)) {};
	res=ADC;
	ADCSRA=0;
	return (res & 0x3ff);
}
