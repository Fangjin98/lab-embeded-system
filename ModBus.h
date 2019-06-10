#define Dis_B     0         //数码管显示的起始位
#define Dis_E     8			//
#define Nmax_Rx  256		//接收数据包最大字节数
#define Nmax_Tx  256		//发送数据包最大字节数
#define MAX_REG_COUNT 16
#define MAX_COIL_COUNT 16
#define FUNC_NUM 3
#define uchar unsigned char	
#define uint unsigned int
#define ulint unsigned long
#define sec1_addr 0x8000
#define sec2_addr 0x8200


//DS1302寄存器的定义
#define DS1302_second_write  0X80
#define DS1302_minutes_write  0X82
#define DS1302_hour_write  0X84
#define DS1302_date_write  0X86
#define DS1302_week_write  0X8A
#define DS1302_month_write  0X88
#define DS1302_year_write  0X8C

#define DS1302_second_read  0X81
#define DS1302_minutes_read  0X83
#define DS1302_hour_read  0X85
#define DS1302_date_read  0X87
#define DS1302_week_read  0X8B
#define DS1302_month_read  0X89
#define DS1302_year_read  0X8D						

short REG1[MAX_REG_COUNT]={
	2,2,2,3,4,5,6,7,8,9,10,11,12,13,14,15
};
	
unsigned char Id=0x01;
short GUID=0x27E0;
char set_mode;
char Mode;
char flag_write;
char flag_write_bit;
							  
enum Uart_status {BUSY,OK,FREE}; 

sbit TR485 = P3^7;   //定义MAX485使能引脚，为1时发送，为0时接收

sbit SDA=P4^0;		  	//I2C总线的数据线
sbit SCL=P5^5;		  	//I2C总线的时钟线

sbit Key1=P3^2;		  	//按下key1
sbit Key2=P3^3;		  	//按下key2
sbit Key3=P1^7;		 	//按下key3

sbit RTC_sclk=P1^5;//时钟控制引脚，控制数据的输入输出
sbit RTC_rst=P1^6;//CE引脚，读写时必须置高电平
sbit RTC_io=P5^4;//数据引脚	

sbit beep = P3^4;					//蜂鸣器引脚
bit flag_beep;

xdata struct Uart
{  	unsigned char Tx_Buf[Nmax_Tx];	 
	unsigned short Tx_Nmax;         //帧的发送字节数，<256
	unsigned char Tx_count;		   //发送字节计数
	unsigned char *Tx_Pc;
	unsigned char Tx_interval_time;
	unsigned char Rx_Buf[Nmax_Rx];
	unsigned char Rx_count;	       //接收字节计数 
	unsigned char *Rx_Pc; 
	unsigned char Rx_interval_time; //接收字节计时    
} Uart1,Uart2;

xdata struct P{
	unsigned char init;			//是否改变过
	unsigned char Id;			//从机地址
	unsigned short GUID;			//唯一标识符
	unsigned char mode;			//工作模式
} param;

typedef struct _systemtime_
{
   	 uchar second;
	 uchar minute;
	 uchar hour;
	 uchar day;
	 uchar week;
	 uchar  month;
   	 uchar year;
}systemtime;
systemtime time;

//bit flag_Uart1_Rx=FREE;         //    正在接收一个数据包
bit flag_Uart1_Tx=FREE;
bit flag_Uart1_newcmd=0;          //    收到一帧数据包

bit flag_Uart2_Rx=FREE;           //    正在接收一个数据包
bit flag_Uart2_Tx=FREE;

short REG[MAX_REG_COUNT];					//ji cun qi
short COIL;									//xian quan 
 
bit flag_1mS=0;           //=1     1mS时刻标记
bit flag_20mS=0;
bit flag_100mS=0;
bit flag_1S=0;
bit flag_1Min=0;
bit flag_1Hour=0;
bit Flag_Key1;
bit Flag_Key2;
bit Flag_Key3;
bit Flag_Keyup;
bit Flag_Keydown;
bit Flag_Keyleft;
bit Flag_Keyright;

char i_led;
						
unsigned char count_1mS=0;
unsigned char count_20mS=0;
unsigned char count_100mS=0;
unsigned char count_1S=0;
unsigned char count_1Min=0;
unsigned char count_Key1;
unsigned char count_Key2;
unsigned char count_Key3;
unsigned char count_Key;
unsigned char count_Keyup;
unsigned char count_Keydown;
unsigned char count_Keyleft;
unsigned char count_Keyright;

char Flag_flash;
char flash_pos;
char is_flashing;

unsigned char code duanxuan[12]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x40,0x00};
unsigned char code weixuan[8]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07};
char display[8]={1,2,3,4,5,11,11,11};
char led_data[8]={0,0,0,0,0,0,0,0};


int code tdata[]={239,197,175,160,150,142,135,129,124,120,116,113,109,107,104,101, 
			  99, 97, 95, 93, 91, 90, 88, 86, 85, 84, 82, 81, 80, 78, 77, 76, 
			  75, 74, 73, 72, 71, 70, 69, 68, 67, 67, 66, 65, 64, 63, 63, 62, 
			  61, 61, 60, 59, 58, 58, 57, 57, 56, 55, 55, 54, 54, 53, 52, 52, 
			  51, 51, 50, 50, 49, 49, 48, 48, 47, 47, 46, 46, 45, 45, 44, 44, 
			  43, 43, 42, 42, 41, 41, 41, 40, 40, 39, 39, 38, 38, 38, 37, 37, 
			  36, 36, 36, 35, 35, 34, 34, 34, 33, 33, 32, 32, 32, 31, 31, 31, 
			  30, 30, 29, 29, 29, 28, 28, 28, 27, 27, 27, 26, 26, 26, 25, 25,
			  24, 24, 24, 23, 23, 23, 22, 22, 22, 21, 21, 21, 20, 20, 20, 19, 
			  19, 19, 18, 18, 18, 17, 17, 16, 16, 16, 15, 15, 15, 14, 14, 14, 
			  13, 13, 13, 12, 12, 12, 11, 11, 11, 10, 10, 9, 9, 9, 8, 8, 8, 7, 
			  7, 7, 6, 6,5, 5, 54,4, 3, 3,3, 2, 2, 1, 1, 1, 0, 0, -1, -1, -1, 
			  -2, -2, -3, -3, -4, -4, -5, -5, -6, -6, -7, -7, -8, -8, -9, -9, 
			  -10, -10, -11, -11, -12, -13, -13, -14, -14, -15, -16, -16, -17, 
			  -18, -19, -19, -20, -21, -22, -23, -24, -25, -26, -27, -28, -29, 
			  -30, -32, -33, -35, -36, -38, -40, -43, -46, -50, -55, -63, 361
};
uint flag_lt;				//温度光照测量检测
uint lvalue;				//光照值
uint tvalue;				//温度值
ulint lsum;
ulint tsum;
uint Time_ad;
uint ltime;
uint ttime;
uint count_ADC;
uint display1;
 
 
uint global = 0;
uint date_h = 0;
ulint sum = 0;
uint x = 0;
uint y = 0;
uchar nav_h;
uchar nav_l;       