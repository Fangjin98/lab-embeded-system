void Timer0_isr(void) interrupt 1
{	flag_1mS = 1;
	count_1mS++;
	if( count_1mS == 20 ) 
	     { count_1mS = 0;    
		   flag_20mS = 1;
		   count_20mS++;
		   if(count_20mS==5){
		       count_20mS=0;
			   flag_100mS=1;
			   count_100mS++;
			   if (count_100mS==10) 
				 { count_100mS=0;			 
				 	flag_1S=1;
					count_1S++;
						 if(count_1S==60) 
							{ count_1S=0;						
							  flag_1Min=1;
							  count_1Min++;
								if(count_1Min==60) 
									{ count_1Min=0;	flag_1Hour=1;
									}
							} 
				 }
         	}
	}							
}

void ADC_isr() interrupt 5
{

	if(Mode==0){
		global++;
		if(global>500)			  //将采集500次后的数据求出平均值
		{
			date_h=(sum+250)/500;//四舍五入
			nav_h=(date_h>>5)&0x07;
			nav_l=date_h&0x1F;
			global=0;
			sum=0;
		}
		sum+=ADC_RES;
		ADC_CONTR&=~0X10;		  //将ADC_FLAG位清零
		ADC_CONTR|=0X08;		 //将ADC_START进行软件置位
	}
	else{
		Time_ad++;
		if(Time_ad>500){
			if(flag_lt==1)
			{
				uint ii=(tsum+ttime/2)/ttime;
				tvalue=tdata[ii-1];
				tsum=0;
				ttime=0;
				Time_ad=0;
			}
			if(flag_lt==-1)
			{
				lvalue=(lsum+ltime/2)/ltime;
				lsum=0;
				ltime=0;
				Time_ad=0;
			}
		}
		else {
			if(flag_lt==1)
			{
				//对应温度的数据处理
				tsum+=((ADC_RES*256+ADC_RESL)/4);//由10位AD值转换为8位AD值
				ttime++;
			}
			if(flag_lt==-1)
			{
				//处理光部分的数据
				lsum+=(ADC_RES*256+ADC_RESL);
				ltime++;
			}
		}
		ADC_CONTR&=~0X10;  					 //转换完成后，ADC_FLAG清零
		ADC_CONTR|=0X08;	 					 //转换完成后，ADC_START赋1
	}
}

void Uart1_isr(void) interrupt 4 		   //串口中断函数   
{	if (TI) 
        { TI = 0;                      // 如果上一个数据发完，TI = 0; 
		  if (Uart1.Tx_count++ >= Uart1.Tx_Nmax)      // 如果数据包发送完毕	   
			   flag_Uart1_Tx=FREE;
		  else SBUF=*Uart1.Tx_Pc++;    // 否则发送数据包下一字节	       	   					 
        }       
	if (RI) 
	    { RI=0;			       				
	    }
}

void Uart2_isr(void) interrupt 8
{   if(S2CON & 0x02)
      { S2CON &= ~0x02;	    //清发送中断标志位
	  	if(Uart2.Tx_count++ >=Uart2.Tx_Nmax){
			flag_Uart2_Tx=FREE;
			TR485=0;
		}
		else {
			S2BUF=*Uart2.Tx_Pc++;
		}  
	  }
	if(S2CON & 0x01)
      { S2CON &= ~0x01;		//清接收中断标志位 
	    if(flag_Uart2_Rx==FREE)		      //如果可以接收数据帧
		{   *Uart2.Rx_Pc++=S2BUF;
			Uart2.Rx_count++;
			Uart2.Rx_interval_time=ModBus_Interval_Min;	
		}
	  }
}

