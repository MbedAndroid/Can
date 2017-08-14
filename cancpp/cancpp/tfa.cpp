//#include "main.h"
#include "tfa.h"

char status=0;


int bytes=0;
bool laststate=false;
unsigned char data[30];
int index=0;
int tempdate=0;
char length;
char bitcounter=0;
//DigitalOut pr (LED3);

void ResetDecoder(void)
{

	bytes=0;
	laststate=false;
	status=0;
	index=0;
	bitcounter=0;
	length=0;
	//pr=0;
	tempdate=0;
	data[0]=0xff;

}
void print(unsigned char a)
{
	#ifdef DEBUG
//	printf(" %02X ",a);
	#endif
}
char ParityCheck(int data)
{
	char i;
	char flip=0;
	for (i=0; i<8; i++) {
		if (data&1) flip=~flip;
		data>>=1;
	}
	return ((flip ^ data)&1) ? 1:0;

}

char Checksum(void)
{
	int i;
	unsigned char check=0;
	for (i=1; i<length+2; i++) check^=data[i];
	#ifdef DEBUG
//	if (check==0)     printf("\r\n");
	#endif
	return check;
}

char Manchester(char w)
{

	bytes++;
	#ifdef DEBUG
//	if (w>0) printf("1");  else printf("0");
	#endif
	tempdate>>=1;
	if (w>0) tempdate|=0x100;
	else tempdate&=0xff;

	bitcounter++;
	if (bitcounter==9) {
		if (ParityCheck(tempdate)) {
			ResetDecoder();
			return 1;
		}
		tempdate&=0xff;
		data[index]=tempdate;
		bitcounter=0;
		if (index==2) length=data[2]>>1 & 0x1f;
		
		if ((index==length+2)) return (Checksum());
		print(data[index]);
		if (data[0]==0x9f) {
			index++;
		//	pr=1;
			} else {
			ResetDecoder();
			return 1;
		}
	}
	return 1;
}

bool nextPulseTFA(int width)
{

	if (400<width && width<1200) {
		bool w = width >= 700;

		if (w) {

			if (Manchester(1)==0) return (true);
			status=0;

			} else {
			if (status==0) status++;
			else {
				status=0;

				if (Manchester(0)==0) return (true);

			}
		}

		return false;

		} else {
		ResetDecoder();
	}
	return false;
}


