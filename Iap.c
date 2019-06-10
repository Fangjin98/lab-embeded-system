//void delay() 		//ÑÓÊ±4us
//{
//	;;
//}
//void _nop_(){
//	;;
//} 

void IapIdle(void)
{   IAP_CONTR=0;
    IAP_CMD=0;
	IAP_TRIG=0;
	IAP_ADDRH=0x80;
	IAP_ADDRH=0;
}

char IapRead(int addr)
{   idata char dat;
    IAP_CONTR=0x83;   //for 12MHz;
    IAP_CMD=1;
	IAP_ADDRL=addr;
	IAP_ADDRH=addr>>8;
    IAP_TRIG=0x5a;
	IAP_TRIG=0xa5;
	//delay();
	dat=IAP_DATA;
	IapIdle();
	return dat;
}

void IapProgram(int addr, char dat)
{	IAP_CONTR=0x83;	   //for  12MHz
    IAP_CMD=2;
	IAP_ADDRL=addr;
	IAP_ADDRH=addr>>8;
	IAP_DATA=dat;
    IAP_TRIG=0x5a;
	IAP_TRIG=0xa5;
	//_nop_();
	IapIdle();
}

void IapErase(int addr)
{	IAP_CONTR=0x83;	    //for  12MHz
    IAP_CMD=3;
	IAP_ADDRL=addr;
	IAP_ADDRH=addr>>8;
    IAP_TRIG=0x5a;
	IAP_TRIG=0xa5;
	//_nop_();
	IapIdle();
}

void SaveParameter(int addr, void *ip_data, unsigned char num_data)
{   data unsigned char i;
    data char *pt;
    IapErase(addr);
	pt=ip_data;
	for(i=0;i<num_data;i++)IapProgram(addr+i, *pt++);   
}

void ReadParameter(int addr, void *ip_data, unsigned char num_data)
{   idata unsigned char i;
    idata char *pt;
	pt=ip_data;
	for(i=0;i<num_data;i++) *pt++=IapRead(addr+i);   
}
