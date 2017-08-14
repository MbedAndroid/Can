#include "main.h"
#include "wmr968.h"
#include "tlkRH.h"
#include "mcp2515.h"

extern unsigned char data[];
//REGEN   Rain;
//extern volatile u8 smspointer;
//u8 LastBaro;
//u8 LastWeather;
//u8 list[15];
//float temp,Sealevel;
//int hum,dew,dewOFLW;
//int Barometer,Weatherstatus;
int i;
//float actualpressure;
//u8  checksum;
extern tCAN message;
extern CANDEF	Can1;



char WindDirSeg(unsigned char b)
{
	/* Encrypted using: a=
	-
	a&0xf; b=a^(a>>1);
	I don’t see any easy reversed formula.
	i.e. I can’t solve “a” from the equ b=a^(a>>1)
	in one easy single formula. So I solve it
	bit by bit in this method. Does anyone have
	a better solution?
	NOT like this
	:
	b &= 0xf;
	return (
	-
	(b ^ (b >> 1) ^ (b >> 2) ^ (b >> 3))) & 0xf;
	It works... But is, of course, also a bit by bit solution.
	*/

	b ^= (b & 8) >> 1;

	/* Solve bit 2 */
	b ^= (b & 4) >> 1;

	/* Solve bit 1 */
	b ^= (b & 2) >> 1;

	/* Solve bit 0 */
	return b & 0x0f;
}


void OutputDataWindTFA(char ch)
{
//	#define MilesToMeters 0.4472222222222222f;

	message.id = Can1.id | PDO2;
	message.header.rtr = 0;
	message.header.length = 8;
	message.data[0] = ch;
	message.data[1]=data[1];
	message.data[2]=data[6];
	message.data[3]=data[7];
	message.data[4]=data[8];
	message.data[5]=data[9];
	message.data[6]=data[10];
	message.data[7]=data[11];
	
	/*
	float average,gust;
	;
	list[2]=ch;
	if ((data[2]&0xc0)==0xc0) {
	list[3]|=0x00;   //bit 6 zou battery moeten zijn
	} else {
	list[3]|=0x40;
	}

	temp=data[4]*0x0f;
	temp/=10;
	temp+=data[4]>>4;
	temp+=data[5]*10;
	//winddir
	i=360-WindDirSeg(data[11]>>4)*22.5;
	message.data[3]=i/100;
	message.data[2] =((i-list[5]*100)/10)<<4;
	message.data[2]|=i-message.data[3]*100-(message.data[2]>>4)*10;
	
	//average speed
	average=(data[9]&0x0f);
	average*=10;
	average+=(data[8]>>4);
	average*=10;
	average+=data[8]&0x0f;
	average*=MilesToMeters ;
	i=(int) average;

	message.data[5]=(i/100 &0x0f);
	message.data[4]=((i%100)/10)<<4 | (i%10);
	//gust speed
	gust=(data[10]>>4);
	gust*=10;
	gust+=data[10]& 0x0f;
	gust*=10;
	gust+=data[9]>>4;
	gust*=MilesToMeters ;
	i=(int) gust;

	message.data[7]=(i/100)<<4 | (i%100)/10;
	list[5]|=(i%10)<<4;
	list[9]=data[7]<<4 | data[6]>>4;
	*/
	mcp2515_send_message(&message);
}
void OutputDataRainTFA(char ch)
{
	message.id = Can1.id | PDO2;
	message.header.rtr = 0;
	message.header.length = 8;
	message.data[0] = ch;
	message.data[1]=data[1];
	message.data[2]=data[4];
	message.data[3]=data[5];
	message.data[4]=data[6];

	mcp2515_send_message(&message);
	/*
	float Regen;

	
	list[0]=0xff;
	list[1]=0xff;
	list[2]=ch;
	if ((data[2]&0xc0)==0xc0) {
	list[3]|=0x00;   //bit 6 zou battery moeten zijn
	} else {
	list[3]|=0x40;
	}
	//totall rain vanuit de regenmeter
	Regen=data[5];
	Regen*=256;
	Regen+=data[4];
	Regen*=7;   //omrekening naar decimal resultaat is nu *10
	i=(int) Regen;
	//  printf("%d--%d\r\n",i,Rain.seccounter);
	//binary total rain, daarna conversie naar bcd

	list[7]=(i/10000)<<4;
	list[7]|=((i%10000)/1000)&0x0f;
	list[6]=((i%1000)/100)<<4;
	list[6]|=(i%100)/10 &0x0f;
	list[5]|=i%10 <<4;
	//current rain, functie per uur meten van tijdslengte tussen de pulsen
	if (Rain.aligncounters==0)  {
	Rain.lastcounter=i;    //one time voor startup
	Rain.rainhour=0;
	Rain.aligncounters=1;
	}

	Rain.verschil=(i-Rain.lastcounter)&0xffff;
	if (Rain.verschil>0) {
	Rain.rainhour=(2520/Rain.seccounter)*Rain.verschil; //aantal tiks per 2520 eenheid is 1 mm uur
	Rain.lastcounter=i;
	//printf("%d->%d\r\n",Rain.verschil,i);
	Rain.seccounter=0;
	}
	list[4]=  ((Rain.rainhour%100)/10)<<4 | Rain.rainhour%10 ;   //crrent rain nog in te zetten ((currentrain%/100)/10) <<4 |
	list[5]=Rain.rainhour/100; //current rain
	//yesteday rain nog te maken uit tijdklok
	list[9]=(Rain.yesterdayrain/1000)<<4 | (Rain.yesterdayrain%1000)/100;
	list[8]=((Rain.yesterdayrain%100)/10)<<4 | Rain.yesterdayrain%10;
	//datum in te vullen vanuit tijdklok
	list[10]=0x55; //min since
	list[11]=0x12; //h since
	list[12]=0x28; //day since
	list[13]=0x03;  //month since
	list[14]=0x15;  //year since


	#ifdef DEBUG
	Regen=(list[5] & 0x0f)*100+ (list[4]>>4 & 0x0f)*10 + (list[4] & 0x0f); //regen/uur
	Regen*=rainmult;
	Regen*=10;
	Regengisteren=(list[9]>>4 &  0x0f)*1000+ (list[9] & 0x0f)*100+ (list[8]>>4 & 0x0f)*10 + (list[8] & 0x0f);
	Regengisteren*=rainmult;
	Regengisteren*=10;
	//regen totaal komt van de regenmeter,max is 65000*0.7mm=45 meter
	Regentotaal=list[7]>>4;
	Regentotaal*=10;
	Regentotaal+=list[7] & 0x0f;
	Regentotaal*=10;
	Regentotaal+=(list[6]>>4 & 0x0f);
	Regentotaal*=10;
	Regentotaal+=(list[6] & 0x0f);
	Regentotaal*=10;
	Regentotaal+= (list[5]>>4 & 0x0f);
	Regentotaal*=rainmult;
	RegenOFLW=list[3]>>4;
	printf("Regen: %3.1f Gisteren %3.1f  Totaal %3.1f OFLW:%d\r\n",Regen,Regengisteren,Regentotaal,RegenOFLW);

	if (CheckSumWMX(15)==0) {


	} else printf("ChecksumError5\r\n");
	#endif
	*/
}


void OutputDataTFA(char ch)
{

	message.id = Can1.id | PDO2;
	message.header.rtr = 0;
	message.header.length = 8;
	message.data[0] = ch;
	message.data[1]=data[1];
	message.data[2]=data[4];
	message.data[3]=data[5];
	message.data[4]=data[6];
	message.data[5]=data[7];
	mcp2515_send_message(&message);
}
/*
void OutputData(char ch,const  char* data)
{

	char list[10];

	list[0]=0xff;
	list[1]=0xff;
	list[2]=ch;
	list[3]=data[2]>>4 | (data[3]<<4 & 0xf0); //sensor type + flags
	list[4]=data[4]>>4 | (data[5]<<4 & 0xf0); //temp data 0.1 deg en deg
	list[5]=data[5]>>4 | (data[6]<<4 & 0xf0); //temp data 10 deg en flags
	list[6]=data[6]>>4 | (data[7]<<4 & 0xf0);//hum
	temp=(list[4]>>4)+(list[5] & 0x0f)*10;
	hum=(list[6]>>4)*10+(list[6]&0x0f);
	dew=(int) tlkRH_calculate_dewpoint(hum,temp);
	list[7]= dew/10<<4 | dew%10;

	#ifdef DEBUG
	temp=(list[5]>>4 & 0x03)*1000+(list[5] & 0x0f)*100+ (list[4]>>4 & 0x0f)*10 +(list[4] & 0x0f);
	if ((list[5] & 0x80)>0) temp*=-1;
	hum=(list[6]>>4 & 0x0f)*10+  (list[6] & 0x0f);
	dew=(list[7]>>4 & 0x0f)*10 + list[7] & 0x0f;
	dewOFLW=((list[3]>>4 & 0x0f) | (list[5] & 0xf0));
	printf("temp:%3.1f  hum:%d dew:%d dewOFLW%d\r\n",temp/10,hum,dew,dewOFLW);
	#endif

	

}
*/