enum Uart_status Uart1_print(unsigned char *, unsigned char);
enum Uart_status Uart2_Send();
char get_bit(unsigned char offset);
void set_bit(unsigned char offset,short value);

unsigned short ModBusCRC(unsigned char *ptr,unsigned char size)
{                                  //ptr 指向当前发送的字节 size 数据包的大小 
    unsigned short i,j,tmp,CRC16; //,check_sum;
	CRC16 = 0xffff; //CRC寄存器初始值
	for(i=0;i<size;i++)
	{ 	CRC16 = *ptr ^ CRC16;
	    for(j=0;j<8;j++)
		{ 	tmp = CRC16 & 0x0001;//取出右移1位的移出位 
		    CRC16 = CRC16>>1;
		    if(tmp)	CRC16 = CRC16 ^ 0xa001;//异或多项式 
	    }
        ptr++;
    }
	return CRC16;   //结果为CRCH在前，CRCL在后（ModBus协议上需先传CRCL，再CRCH）
}

void send_ModBus_frame(unsigned short size){
	unsigned short res;
	 res=ModBusCRC((unsigned char *)&Uart2.Tx_Buf,size);
	 Uart2.Tx_Buf[size+1]=(res>>8)&0xff;
	 Uart2.Tx_Buf[size]=res&0xff;
	 Uart2.Tx_Nmax=size+2;
	 Uart2_Send();
}

//void send_error_ModBus_frame(unsigned char correct_code,unsigned char error_code){
//	Uart2.Tx_Buf[0]=correct_code;
//	Uart2.Tx_Buf[1]=0xff;
//	Uart2.Tx_Buf[2]=error_code;
//	Uart2.Tx_Nmax=3;
//	Uart2_Send();
//}

char parase_ModBus_frame(void){
	unsigned char i,j,byte_count;
	short base_addr,rd_count;
	flag_Uart2_Rx=BUSY;
	if((Uart2.Rx_count!=0)&&(Uart2.Rx_Buf[0]==Id)){
		unsigned short tmp;
		if((tmp=ModBusCRC((unsigned char *)&Uart2.Rx_Buf,Uart2.Rx_count))==0){
			char func_code=Uart2.Rx_Buf[1];
			if(func_code==0x01){
				base_addr=0;					
				base_addr=((base_addr|Uart2.Rx_Buf[2])<<8)|Uart2.Rx_Buf[3];		//ji di zhi
				if(base_addr>MAX_COIL_COUNT) {
					flag_Uart2_Rx=FREE;
					//send_error_ModBus_frame(MAX_COIL_COUNT,base_addr);
					return 0;
				}
				rd_count=0;
				rd_count=((rd_count|Uart2.Rx_Buf[4])<<8)|Uart2.Rx_Buf[5];	//bit shu
				byte_count=(rd_count>8)?2:1; 			//zi jie shu
				Uart2.Tx_Buf[0]=Id;
				Uart2.Tx_Buf[1]=func_code;
				Uart2.Tx_Buf[2]=byte_count;
				for(i=0;i<byte_count;i++){	 		//ti qu wei 
					j=0;
					Uart2.Tx_Buf[3+i]=0;
					while((j<8) && ((j+i*8)<=rd_count)){
						Uart2.Tx_Buf[3+i]=Uart2.Tx_Buf[3+i]|(get_bit(base_addr+j+i*8)<<j);
						j++;
					}
				}
				send_ModBus_frame(3+byte_count);
			}
			if(func_code==0x03){			
				base_addr=0;					
				base_addr=((base_addr|Uart2.Rx_Buf[2])<<8)|Uart2.Rx_Buf[3];
				if(base_addr>MAX_REG_COUNT) {
					//send_error_ModBus_frame(func_code,(char) MAX_REG_COUNT);
					flag_Uart2_Rx=FREE;
					return 0;
				}
				rd_count=0;
				rd_count=((rd_count|Uart2.Rx_Buf[4])<<8)|Uart2.Rx_Buf[5];
				Uart2.Tx_Buf[0]=Id;
				Uart2.Tx_Buf[1]=func_code; 
				Uart2.Tx_Buf[2]=rd_count*2;
				j=3;
				for(i=0;i<rd_count;i++){
					Uart2.Tx_Buf[j]=(REG[base_addr+i]>>8)&0xff;	//高字节
					Uart2.Tx_Buf[j+1]=(REG[base_addr+i])&0xff;	//低字节
					j+=2;
				}
				send_ModBus_frame(3+Uart2.Tx_Buf[2]);
			}
			else if(func_code==0x05){
				base_addr=0;					
				base_addr=((base_addr|Uart2.Rx_Buf[2])<<8)|Uart2.Rx_Buf[3];		//ji di zhi
				if(base_addr>MAX_COIL_COUNT) {
					flag_Uart2_Rx=FREE;
					//send_error_ModBus_frame(MAX_COIL_COUNT,base_addr);
					return 0;
				}
				rd_count=0;
				rd_count=((rd_count|Uart2.Rx_Buf[4])<<8)|Uart2.Rx_Buf[5];
				flag_write_bit=1;
				if(rd_count==0xff00){
					set_bit(base_addr,1);
				}
				else{
					set_bit(base_addr,0);
				}
				for(i=0;i<6;i++){
					Uart2.Tx_Buf[i]=Uart2.Rx_Buf[i];
				}
				//send_error_ModBus_frame(COIL>>8,COIL);
				send_ModBus_frame(6);				
			}
			else if(func_code==0x06){
				base_addr=0;					
				base_addr=((base_addr|Uart2.Rx_Buf[2])<<8)|Uart2.Rx_Buf[3];		//ji di zhi
				if(base_addr>MAX_COIL_COUNT) {
					flag_Uart2_Rx=FREE;
					return 0;
				}
				rd_count=0;
				rd_count=((rd_count|Uart2.Rx_Buf[4])<<8)|Uart2.Rx_Buf[5];
				REG[base_addr]=rd_count;
				flag_write=1;
				for(i=0;i<6;i++){
					Uart2.Tx_Buf[i]=Uart2.Rx_Buf[i];
				}
				send_ModBus_frame(6);	
			}
			else if(func_code==0x10){
				base_addr=0;					
				base_addr=((base_addr|Uart2.Rx_Buf[2])<<8)|Uart2.Rx_Buf[3];
				if(base_addr>MAX_REG_COUNT) {
					flag_Uart2_Rx=FREE;
					//send_error_ModBus_frame(func_code,(char) MAX_REG_COUNT);
					return 0;
				}
				rd_count=(short) Uart2.Rx_Buf[6];
				//rd_count=((rd_count|Uart2.Rx_Buf[4])<<8)|Uart2.Rx_Buf[5];
				//write to REG
				j=0;
				for(i=0;i<rd_count;i+=2){
					REG[base_addr+j]=0;
					REG[base_addr+j]=((REG[base_addr+i]|Uart2.Rx_Buf[7+i])<<8)|Uart2.Rx_Buf[7+i+1];
					j++;
					flag_write=1;	
				}
				Uart2.Tx_Buf[0]=Uart2.Rx_Buf[0];
				Uart2.Tx_Buf[1]=Uart2.Rx_Buf[1]; 
				Uart2.Tx_Buf[2]=Uart2.Rx_Buf[2]; 		//addr
				Uart2.Tx_Buf[3]=Uart2.Rx_Buf[3];
				Uart2.Tx_Buf[4]=Uart2.Rx_Buf[4];
				Uart2.Tx_Buf[5]=Uart2.Rx_Buf[5];
				send_ModBus_frame(6);
			}
			flag_Uart2_Rx=FREE;
			return 1;
		}
		else {
			flag_Uart2_Rx=FREE;
			return 0;
		}
	}
	else{
		//send_error_ModBus_frame(Id,Uart2.Rx_Buf[0]);
		flag_Uart2_Rx=FREE;
		return 0;
	}
}


void new_ModBus_frame(void)
{  
	Uart2.Rx_Pc=&Uart2.Rx_Buf;             //准备接收新ModBus数据帧
    Uart2.Rx_count=0;
    flag_Uart2_Rx=FREE;
}

