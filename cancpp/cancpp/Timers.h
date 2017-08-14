/*
 * IncFile1.h
 *
 * Created: 13-3-2017 13:55:58
 *  Author: bert
 */ 


#ifndef TIMERS_H_
#define TIMERS_H_

typedef struct TIMER
{
	u16	scounter;
	u16	countdown;
	u8	flag;
}timers_p;

#define BAUD 19200
#define MYUBRR F_CPU/16/BAUD-1
void Blink(char nr);
void InitTimer(void);
void EnableInterrupt(char nr);
void USART_Init(unsigned int ubrr);
void usart_pstr(char const *s);
void usart_putchar(char data);
void SendHeartBeat(u8 state);
void StartTimer(struct TIMER *timer, u16 counts);
u8 CompareTime(struct TIMER *timer);
u16 ReadAD(void);
u8 CheckCanId(u8 newId);
void SendPDO(u16 service);
#endif /* INCFILE1_H_ */