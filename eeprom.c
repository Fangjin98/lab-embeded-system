void delay() 		//延时4us
{
	;;
}

void start()	 	//主机启动信号
{
 	SDA=1;
	delay();
	SCL=1;
	delay();
	SDA=0;
	delay(); 	
}
void stop()		 	//停止信号
{
 	SDA=0;
	delay();
	SCL=1;
	delay();
	SDA=1;
	delay();
}

void ack(){
	SDA=0;
	delay();
	SCL=1;
	delay();
	SCL=0;
}

void noack(){
	SDA=1;
	delay();
	SCL=1;
	delay();
	SCL=0;
}

void respons()	 	//从机应答信号
{
	uchar i=0;
	SCL=1;
	delay();
	while(SDA==1&&(i<255)){  //表示若在一段时间内没有收到从器件的应答则
		i++;}				//主器件默认从期间已经收到数据而不再等待应答信号。 
	SCL=0;
	delay();
}
void writebyte(uchar date) //对24C16写一个字节数据
{
	uchar i,temp;
	temp=date;
	for(i=0;i<8;i++)
	{
		temp=temp<<1;
		SCL=0;
		delay();
		SDA=CY;
		delay();
		SCL=1;
		delay();
	}
	SCL=0;
	delay();
	SDA=1;
	delay();	
}

uchar readbyte()  			//从24C16读一个字节数据
{
	uchar i,k;
	SCL=0;
	delay();
	SDA=1;
	delay();
	for(i=0;i<8;i++)
	{
		SCL=1;
		delay();
		k=(k<<1)|SDA;
		delay();
		SCL=0;
		delay();
	}
	delay();
	return k;	
}
void write_add(uchar addr,uchar date)
{
	start();
	writebyte(0xa0);							
	respons();
	writebyte(addr);
	respons();
	writebyte(date);
	respons();
	stop();							
}

uchar read_add(uchar addr) //从24C16的addr地址，读一个字节数据
{
	uchar date;
	start();
	writebyte(0xa0);
	respons();
	writebyte(addr);
	respons();	
	start();
	writebyte(0xa1);
	respons();
	date=readbyte();
	//response();
	stop();
	return date;
}

void read_adds(uchar addr,uchar *date,uchar num)
{
	uchar i;
	uchar *p;
	p=date;
	start();
	writebyte(0xa0);
	respons();
	writebyte(addr);
	respons();	
	start();
	writebyte(0xa1);
	respons();
	for (i=0;i<num;i++){
		*p=readbyte();
		if(i==num-1)  noack();
		else 	ack();
		p++;
	}
	stop();
}

void write_adds(uchar addr,uchar *date,uchar num)
{
	start();
	writebyte(0xa0);							
	respons();
	writebyte(addr);
	respons();
	for(;num>0;num--){
		writebyte(*date);
		respons();
		date++;	
	}
	stop();							
}
