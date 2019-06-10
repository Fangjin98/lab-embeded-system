/**********************  说   明  ********************************
    
*************************************************************************/
#define ModBus_Interval_Min  4  //定义ModBus数据帧时间间隔（单位：mS）

#include "STC15F2K60S2.H"
#include "ModBus.h"
#include "Inic.c"
#include "Isr.c"
#include "ModBus.c"
#include "Func.c"
#include "rtc.c"
#include "eeprom.c"
#include"Iap.c"


void main()
{	IO_Init();		//初始化LED灯
	Timer0_Init();	//初始化定时器0：1mS时标
	Uart1_Init();	//初始化串口1 
	Uart2_Init();   //初始化串口2
	IE_Init();		//初始化中断
	IIC_Init();
	InitADC();
	if(DS1302Read(DS1302_second_read)&0X80)
		 Init_DS1302();

	//SaveParameter(sec2_addr,&REG1,sizeof(REG1));
	
	Init_varible();
	load_configuration();
	
	while(1)
	{  	if( flag_1mS )	       {flag_1mS=0;          Func_1mS();  }
		if(	flag_20mS )		   {flag_20mS=0;		 Func_20mS(); }
		if( flag_100mS )       {flag_100mS=0;        Func_100mS();}
		if (flag_1S!=0)    { flag_1S = 0;     Func_1S();} 
		if (flag_1Min!=0)  { flag_1Min = 0;   Func_1Min();} 
		if (flag_1Hour!=0) { flag_1Hour = 0;  Func_1Hour();} 	  	   
	}		
}
