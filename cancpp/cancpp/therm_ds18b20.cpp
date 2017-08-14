#include <avr/io.h> 
#include "main.h"
#include <util/delay.h>
#include <avr/eeprom.h>
#include "therm_ds18b20.h"

extern UCHAR ROM[];
////////////////////////////////////////
//void therm_delay(uint16_t delay)
//{
//	while(delay--) asm volatile("nop");
//}


uint8_t therm_reset()
{
	uint8_t i;
	//Pull line low and wait for 480uS
	THERM_LOW();
	
	THERM_OUTPUT_MODE();
	_delay_us(480);

	//Release line and wait for 60uS
	THERM_INPUT_MODE();
	_delay_us(60);
;
	//Store line value and wait until the completion of 480uS period
	i=(THERM_PIN & (1<<THERM_DQ));
	_delay_us(420);

	//Return the value read from the presence pulse (0=OK, 1=WRONG)
	return i;
}

void therm_write_bit(uint8_t bit)
{
	//Pull line low for 1uS
	THERM_LOW();
	THERM_OUTPUT_MODE();
	_delay_us(1);

	//If we want to write 1, release the line (if not will keep low)
	if(bit) THERM_INPUT_MODE();
	//Wait for 60uS and release the line
	_delay_us(60);

	THERM_INPUT_MODE();
}

uint8_t therm_read_bit(void)
{
	uint8_t bit=0;
	//Pull line low for 1uS
	THERM_LOW();
	THERM_OUTPUT_MODE();
	_delay_us(1);
	
	//Release line and wait for 14uS
	THERM_INPUT_MODE();
	_delay_us(14);
	//Read line value
	if(THERM_PIN&(1<<THERM_DQ)) bit=1;

	//Wait for 45uS to end and return read value
	_delay_us(45);

	return bit;
}

uint8_t therm_read_byte(void)
{
	uint8_t i=8, n=0;
	while(i--)
	{
		//Shift one position right and store read value
		n>>=1;
		n|=(therm_read_bit()<<7);
	}
	return n;
}

void therm_write_byte(uint8_t byte)
{
	uint8_t i=8;
	while(i--)
	{
		//Write actual bit and shift one position right to make the next bit ready
		therm_write_bit(byte&1);
		byte>>=1;
	}
}
char read_serial(void)
{

int i;
	therm_reset();
	therm_write_byte(THERM_CMD_READROM);
	
	for (i=0;i<serial_l;i++) ROM[i]=therm_read_byte();
	if (ROM[0]!=0x28)	return -1; //error indien device niet matched

UCHAR  j;
UCHAR crc = 0;
    
    for (i = 0; i < serial_l-1; i++) 
    {
        UCHAR inbyte = ROM[i];
        for (j = 0; j < 8; j++) 
        {
            UCHAR mix = (crc ^ inbyte) & 0x01;
            crc >>= 1;
            if (mix) crc ^= 0x8C;
            inbyte >>= 1;
        }
    }


  if (( crc==ROM[7] ) & ((ROM[1]|ROM[2]|ROM[3]|ROM[4]|ROM[5]|ROM[6])!=0)) return 1;
  										
  					  


return -1;
}

void therm_read_temperature(void)
{
	//Reset, skip ROM and start temperature conversion
	therm_reset();
	therm_write_byte(THERM_CMD_SKIPROM);
	
	therm_write_byte(THERM_CMD_CONVERTTEMP);

	//Wait until conversion is complete
	

	while(!therm_read_bit());
	//Reset, skip ROM and send command to read Scratchpad
	
	therm_reset();

	therm_write_byte(THERM_CMD_SKIPROM);
	therm_write_byte(THERM_CMD_RSCRATCHPAD);
	//Read Scratchpad (only 2 first bytes)
	//txbuffer[temppos_lo]=therm_read_byte();
	//txbuffer[temppos_hi]=therm_read_byte();

}


