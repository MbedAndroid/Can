 void OutputData(char,const  char*);
 void OutputDataBar(char,const  char*);
 void OutputDataRain(char,const  char*);
 void OutputDataWind(char,const  char*);
 void OutputData(char,const  char*);
 void Sms_Rx_interrupt();
 void InitSms(void);
 void OutputDataTFA(char ch);
 void OutputDataRainTFA(char ch);
 void OutputDataWindTFA(char ch);
 void InsertBaroInternal(void);
 void SendAlarmStatus(char);
 void SendSerialToRasp(char,char);

 void PollSMS(void);
 void serial_writable();
 void test();
 typedef struct {
	 UINT    lastcounter;
	 UINT    seccounter;
	 UINT    rainhour;
	 UINT    yesterdayrain;
	 UINT    verschil;
	 char    aligncounters;
 } REGEN;