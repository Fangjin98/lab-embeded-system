void delay();
void DS1302Write(uchar cmd,uchar dat);
uchar DS1302Read(uchar cmd);

void Init_varible(){
	Mode=0;

	Flag_flash=0;
	is_flashing=0;
	flash_pos=0;

	flag_lt=0;
}
void IO_Init(void)		   //   初识化8个LED灯
{  	P2M1 = 0x00;       //   设置：P2推挽
	P2M0 = 0xff;		   
	P0M1 = 0x00;       //   设置：P0推挽   
	P0M0 = 0xff;
}

void Timer0_Init(void)  //1000微秒@11.0592MHz
{  	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xCD;		//设置定时初值
	TH0 = 0xD4;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时	
	ET0=1;          //开启定时器0中断
}

void Uart1_Init(void)   //115200bps@11.0592MHz
{	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设定定时器1为16位自动重装方式
	TL1 = 0xE8;		    //设定定时初值
	TH1 = 0xFF;		    //设定定时初值
	ET1 = 0;		    //禁止定时器1中断
	TR1 = 1;		    //启动定时器1
	ES=1;               //开启串口0中断
}

void Uart2_Init(void)		//9600bps@11.0592MHz	
{  	S2CON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x04;		//定时器2时钟为Fosc,即1T
	T2L = 0xE0;	  	    //设定定时初值
	T2H = 0xFE;		    //设定定时初值
	AUXR |= 0x10;		//启动定时器2
	P_SW2 |= 0x01 ;     //切换串口2的管脚到P4.6,P4.7
	TR485=0;			//置接收状态
	IE2 |=0x01;         //使能串口2中断
}

void IE_Init(void)
{	
	EA=1;			
}

void IIC_init()
{
	SCL=1;
	delay();
	SDA=1;	
	delay();
}

void Init_DS1302()
{
	uchar temp;
	DS1302Write(0X8E,0X00);//写保护关
	temp=DS1302Read(DS1302_second_read)&0x7f;
	DS1302Write(DS1302_second_write,temp);
	DS1302Write(0X8E,0X80);//写保护置1
}

void InitADC()
{
	P1ASF = 0x80;			
	ADC_RES = 0;			
	ADC_CONTR = 0X8F;		 
	CLK_DIV = 0X00;			
	EADC = 1;				
	PADC = 1;				
}

void ADC_Temp_Init()			//初始化ADC
{
	//P1ASF=0xff;						//将P1口作为模拟功能A/D使用
	ADC_RES=0;						//寄存器ADC_RES和ADC_RESL保存A/D转化结果
	ADC_RESL=0;						//初始赋值0
	CLK_DIV=0x20;					//ADRJ=1：ADC_RES[1:0]存放高2位ADC结果，ADC_RESL[7:0]存放低8位ADC结果	
	ADC_CONTR=0x8c;
}

void ADC_Light_Init()			//初始化ADC
{
	//P1ASF=0xff;						//将P1口作为模拟功能A/D使用
	ADC_RES=0;						//寄存器ADC_RES和ADC_RESL保存A/D转化结果
	ADC_RESL=0;						//初始赋值0
	CLK_DIV=0x20;					//ADRJ=1：ADC_RES[1:0]存放高2位ADC结果，ADC_RESL[7:0]存放低8位ADC结果	
	ADC_CONTR=0x8b;
}