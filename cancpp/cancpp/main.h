/*
* IncFile1.h
*
* Created: 23-2-2017 15:29:01
*  Author: bert
*/
//#define TFA	


#ifndef MAIN_H_
#define MAIN_H_

typedef unsigned int UINT;
typedef unsigned char UCHAR;
typedef unsigned long ULONG;
typedef unsigned char byte;
typedef unsigned int  word;
typedef unsigned long lword;
typedef unsigned char   u8;
typedef unsigned int    u16;
typedef unsigned long   u32;
typedef signed char     s8;
typedef signed int      s16;
typedef signed long     s32;
#define	REL	D,4
#define LED D,7
#define RELSTAT D,4
#define A1	C,1
#define A2	C,2
#define A3	C,3
#define A4	C,4


#define F_CPU 16000000UL
#define TIMEROVERFLOW	0xffff-F_CPU/(1024*10) //=0.1 SEC INTERRUPT
#define true	1
#define false 0
#define PB_SIZE 512

typedef unsigned int UINT;
typedef unsigned char UCHAR;
typedef unsigned long ULONG;
typedef unsigned char u8;

typedef struct
{
	u8 state;
	u8	id;
}	CANDEF;
#define serial_l	8		//aantal bytes van de ds18b20

#define CS_STRN		0x01 //Start Remote Node
#define CS_SPRN		0x02 //Stop Remote Node
#define CS_POS		128 //Enter Pre-operational Sta
#define	CS_RES		129 //Reset Nod
#define CS_RESC		130	//Reset Communication
#define ST_BOOTUP	1 //Boot-up
#define ST_STOPPED	4 //Stopped
#define ST_OPERATIONAL	5 //Operational
#define ST_PREOPERATIONAL	17 //Pre-operational
//pdo transmit commands
#define PDO1		0x180 //analoog input
#define PDO2		0x280 //meteo
#define PDO3		0x380
#define PDO4		0x480
#define SDO			0x580
//pdo receive commands
#define PDOR1		0x200
#define PDOR2		0x300
#define PDOR3		0x400
#define PDOR4		0x500
#define SDOR		0x600

#define SYNC	0x80h
#define TIMESTAMP	0x100
//heartbeat definitions
#define NMT_GUARD	0x700 //+node ID remote frame
#define  BootUp 0
#define  Stopped 4
#define  Operational  5
#define  Pre_operational 	127
//NMT module control
//byte0 =CS byte 1 Node ID
//command specifier
#define Start_Remote_Node 1
#define Stop_Remote_Node 2
#define Enter_Preperational_State	128          
#define Reset_Node	129         
#define ResetCommunication 130  
#define SerialNumberloc	10
#define CHECKSUM_POS	1 
#define CHECKSUM 12345789

// defines for eeprom access
#define read_eeprom_byte(address) eeprom_read_byte ((const uint8_t*)address)
#define write_eeprom_byte(address,value) eeprom_write_byte ((uint8_t*)address,(uint8_t)value)
#define read_eeprom_word(address) eeprom_read_word ((const uint16_t*)address)
#define write_eeprom_word(address,value) eeprom_write_word ((uint16_t*)address,(uint16_t)value)
#define read_eeprom_dword(address) eeprom_read_dword ((const uint32_t*)address)
#define write_eeprom_dword(address,value) eeprom_write_dword ((uint32_t*)address,(uint32_t)value)
#define read_eeprom_array(address,value_p,length) eeprom_read_block ((void *)value_p, (const void *)address, length)
#define write_eeprom_array(address,value_p,length) eeprom_write_block ((const void *)value_p, (void *)address, length)       
#endif /* INCFILE1_H_ */