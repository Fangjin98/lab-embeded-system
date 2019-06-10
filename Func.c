void apply_configuration();
void GetDA1302();
void update();
void init_mode(char mode);
//void read_adds(uchar addr,uchar *date,uchar num);
//void write_adds(uchar addr,uchar *date,uchar num);
//uchar read_add(uchar dev_addr,uchar addr);
//void write_add(uchar dev_addr,uchar addr,uchar date);
void ReadParameter(int addr, void *ip_data, unsigned char num_data);
void SaveParameter(int addr, void *ip_data, unsigned char num_data);


void detect_key(){
	if(Key1==0) count_Key1++;
	if(Key2==0) count_Key2++;
	if(Key3==0) count_Key3++;
	if(nav_h==5) count_Keyup++;
	if(nav_h==2) count_Keydown++;
	if(nav_h==4) count_Keyleft++;
	if(nav_h==1) count_Keyright++;

	if(count_Key1>70) Flag_Key1=1;
	if(count_Key2>70) Flag_Key2=1;
	if(count_Key3>70) Flag_Key3=1;
	if(count_Keydown>70) Flag_Keydown=1;
	if(count_Keyup>70) Flag_Keyup=1;
	if(count_Keyleft>70) Flag_Keyleft=1;
	if(count_Keyright>70) Flag_Keyright=1;

}

char get_bit(unsigned char offset){
	 return ((COIL&(0x01<<offset))>>offset)&0x01;
}

void set_bit(unsigned char offset,short value){
	COIL &= (0xFFFF  ^ (1 << offset));
	COIL |= (value << offset);
}

//enum Uart_status Uart1_print(unsigned char *pt, unsigned char num)	 //非阻塞串口发送函数（与Uart1_isr()配合）
//{  	if (flag_Uart1_Tx==BUSY) return(BUSY);
//    Uart1.Tx_Pc=pt;	           //指定发送数据的起始指针
//	Uart1.Tx_Nmax=num;	       //指定发送数据的字节数，不含校验和字节
//	Uart1.Tx_count=0;		   //发送字节计数清零
//	flag_Uart1_Tx=BUSY;
//	TI = 1;					   //启动发送
//	return(OK);
//}


enum Uart_status Uart2_Send(){
	if(flag_Uart2_Tx==BUSY) return (BUSY);
	Uart2.Tx_Pc=(unsigned char *) &Uart2.Tx_Buf;
	Uart2.Tx_count=0;		  
	flag_Uart2_Tx=BUSY;
	S2CON |= 0x02;
	TR485=1;
	return (OK);
}

void Func_1mS(void)
{   
	

  	P0=0;P2=weixuan[i_led];P0=duanxuan[display[i_led]];
	i_led=(i_led+1)%8;
	

	GetDA1302();

	detect_key();
	
    if(Uart2.Rx_interval_time > 0)	//ModBus新数据帧判断
       if(--Uart2.Rx_interval_time == 0) {
	   parase_ModBus_frame();	   
	   new_ModBus_frame();
	}

	if(Mode==5){
		if(flag_beep)
		{
			beep=~beep;				   //产生方波使得蜂鸣器发声
		}
		else
		{
			beep=0;				      //停止发声，并将beep端口置于低电平
		}
	}
}

void Func_20mS(void){
	switch(Mode){
		case 1:
			led_data[0]=1;
			led_data[3]=(display1/10)%10;
			led_data[4]=display1%10;
			led_data[1]=11;
			led_data[2]=display1/100;
			led_data[5]=11;
			led_data[6]=11;
			led_data[7]=11;
			break;
		case 2:
			led_data[0]=2;
			led_data[3]=(display1/10)%10;
			led_data[4]=display1%10;
			led_data[1]=11;
			led_data[2]=display1/100;
			led_data[5]=11;
			led_data[6]=11;
			led_data[7]=11;
			break;
		case 3:
			led_data[0]=time.hour/10;
			led_data[1]=time.hour%10;
			led_data[2]=12;
			led_data[3]=time.minute/10;
			led_data[4]=time.minute%10;
			led_data[5]=12;
			led_data[6]=time.second/10;
			led_data[7]=time.second%10;
			break;
		case 4:
			led_data[0]=Mode;
			led_data[1]=11;
			led_data[2]=11;
			led_data[3]=11;
			led_data[4]=REG[1]/1000;
			led_data[5]=REG[1]/100;
			led_data[6]=REG[1]/10;
			led_data[7]=REG[1]%10;
			break;
		case 5:
			led_data[0]=Mode;
			led_data[1]=11;
			led_data[2]=11;
			led_data[3]=11;
			led_data[4]=11;
			led_data[5]=11;
			led_data[6]=11;
			led_data[7]=flag_beep;
			break;

	}
	if(Mode!=0){
		if(flag_lt==1)
		{
			ADC_Temp_Init();	
		}
		else
		{
			ADC_Light_Init();	
		}
		flag_lt=-flag_lt;
	}
}

void Func_100mS(void)
{	
	char i;
	for(i=0;i<8;i++) display[i]=led_data[i];

	if(Flag_flash) {
		if(is_flashing) {display[flash_pos]=11; is_flashing=0;}
		else	{display[flash_pos]=led_data[flash_pos]; is_flashing=1;}
	}
	else {
		display[flash_pos]=led_data[flash_pos];
	}

	if(Flag_Key2){
	   Mode=0;
	   init_mode(0);
	   Flag_Key2=0;
	}

 	switch(Mode){
		case 0:
			if(Flag_Key1){
				apply_configuration();
				Flag_Key1=0;
			}
			if(Flag_Keyleft){
				flash_pos--;
				if(flash_pos<0) flash_pos=0;
				Flag_Keyleft=0;
			}
			if(Flag_Keyright){
				flash_pos++;
				if(flash_pos>7) flash_pos=7;
				Flag_Keyright=0;
			}
			if(Flag_Keyup){
				led_data[flash_pos]=(led_data[flash_pos]+1)%10;
				Flag_Keyup=0;
			}
			if(Flag_Keydown){
				led_data[flash_pos]=(led_data[flash_pos]<0)?9:led_data[flash_pos]-1;
				Flag_Keydown=0;
			}
			break;
	}
	count_Key1=0; count_Key2=0; count_Key3=0;
	count_Keyup=0; count_Keydown=0; count_Keyleft=0; count_Keyright=0;
}

void Func_1S(void){
	switch (Mode) {
		case 1:
			display1=tvalue;
			break;
		case 2:
			display1=lvalue;
			break;
	}
	update();
}

void Func_1Min(void){
}

void Func_1Hour(void){
}

void update(){
	switch (Mode){
		case 1:
			REG[1]=tvalue;
			break;
		case 2:
			REG[1]=lvalue;
			break;
		case 3:
			REG[1]=time.hour;
			REG[2]=time.minute;
			REG[3]=time.second;
			break;
		case 4:
			if(flag_write){
				SaveParameter(sec2_addr,&REG,sizeof(REG));
				flag_write=0;
			}
			break;
		case 5:
			if(flag_write_bit){
				flag_beep=(bit) get_bit(0);
				flag_write_bit=0;
			}
	}
}


void load_configuration(){
	ReadParameter(sec1_addr,&param,sizeof(param));

	if(param.init!=0x00){			//如果是第一次启动 非易失里没有保存配置，则使用默认配置
		Id=0x01;
		GUID=10208;
		set_mode=1;
	}
	else{
		GUID=param.GUID;		//不是则读取
		Id=param.Id;
		set_mode=param.mode;
	}
	Mode=0;
	REG[0]=GUID;
	init_mode(Mode);

}

void apply_configuration(){

	GUID=led_data[2]*10000+led_data[3]*1000+led_data[4]*100+led_data[5]*10+led_data[6];
	Id=led_data[0];
	set_mode=led_data[7];

	param.init=0x00;
	param.Id=Id;
	param.GUID=GUID;
	param.mode=set_mode;
	SaveParameter(sec1_addr, &param, sizeof(param));

	Flag_flash=0;
	REG[0]=GUID;
	Mode=set_mode;
	init_mode(Mode);

}

void init_mode(char mode){
	switch (mode){
		case 0:
			P1ASF=0x80;
			ADC_CONTR = 0X8F;
			CLK_DIV = 0X00;
			ADC_RES=0;						//寄存器ADC_RES和ADC_RESL保存A/D转化结果
			ADC_RESL=0;						//初始赋值0

			Flag_flash=1;
	   		flash_pos=0;
			is_flashing=0;

			led_data[0]=Id;
			led_data[1]=10;
			led_data[2]=(GUID/10000);
			led_data[3]=(GUID/1000)%10;
			led_data[4]=(GUID/100)%10;
			led_data[5]=(GUID/10)%10;
			led_data[6]=(GUID)%10;
			led_data[7]=set_mode;
			break;
		case 1:
			P1ASF=0xff;
			CLK_DIV=0x20;
			flag_lt=1;
			tvalue=0;
			lvalue=0;
			lsum=0;
			tsum=0;
			ltime=0;
			ttime=0;
			break;
		case 2:
			P1ASF=0xff;
			CLK_DIV=0x20;
			flag_lt=1;
			tvalue=0;
			lvalue=0;
			lsum=0;
			tsum=0;
			ltime=0;
			ttime=0;
			break;
		case 3:
			P1ASF=0x00;
			CLK_DIV=0x20;
			Time_ad=0;
			break;
		case 4:
			ReadParameter(sec2_addr,&REG,sizeof(REG));
		   	
			flag_write=0;
			break;
		case 5:
			flag_write_bit=0;
			break;
	}
}