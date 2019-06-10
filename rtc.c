void DS1302WriteByte(uchar dat)
{
	uchar i;
  RTC_sclk=0;//初始时钟线置0
	for(i=0;i<8;i++)//开始传输8位数据
	{
		RTC_io=dat&0x01;//取最低位
		
		RTC_sclk=1;//时钟线拉高，制造上升沿，数据被传输 
		
		RTC_sclk=0;//时钟线拉低，为下一个上升沿做准备
		dat>>=1;//数据右移一位，准备传输下一位数据
	}
}

////DS1302读一个字节的数据
uchar DS1302ReadByte()
{
	uchar i,dat;
	
	for(i=0;i<8;i++)
	{
		dat>>=1;//要返回的数据右移一位
		if(RTC_io==1)//当数据线为高时，证明该位数据为1
		dat|=0x80;
		RTC_sclk=1;
		
		RTC_sclk=0;//制造下降沿
		
	}
	return dat;//返回读取出的数据
}

//读相应地址中写一个字节的数据
uchar DS1302Read(uchar cmd)
{
	uchar dat;
	RTC_rst=0;//初始CE置0
	RTC_sclk=0;//初始时钟置0
	RTC_rst=1;//初始CE置1，传输开始
	DS1302WriteByte(cmd);//传输命令字，
	dat=DS1302ReadByte();//读取得到的时间
	RTC_sclk=1;//时钟线拉高
	RTC_rst=0;//读取结束，CE置0，结束数据传输
	return dat;//返回得到的时间日期
}

//在相应地址中写数据
void DS1302Write(uchar cmd,uchar dat)
{
	RTC_rst=0;//初始CE置0
	RTC_sclk=0;//初始时钟置0
	RTC_rst=1;//置1，传输开始
	DS1302WriteByte(cmd);//传输命令字，要写入的时间的地址
	DS1302WriteByte(dat);//写入修改的时间
	RTC_sclk=1;//时钟线拉高
	RTC_rst=0;//读取结束，CE=0，结束数据的传输
}
//DS1302的时间值获取程序
void GetDA1302()
{
	uchar realvalue;
	realvalue=DS1302Read(DS1302_second_read);
	time.second=((realvalue&0x70)>>4)*10+(realvalue&0x0f);
		realvalue=DS1302Read(DS1302_minutes_read);
	time.minute=((realvalue&0x70)>>4)*10+(realvalue&0x0f);
		realvalue=DS1302Read(DS1302_hour_read );
	time.hour=((realvalue&0x70)>>4)*10+(realvalue&0x0f);
		realvalue=DS1302Read(DS1302_date_read );
	time.day=((realvalue&0x70)>>4)*10+(realvalue&0x0f);
		realvalue=DS1302Read(DS1302_week_read);
	time.week=((realvalue&0x70)>>4)*10+(realvalue&0x0f);
		realvalue=DS1302Read(DS1302_month_read);
	time.month=((realvalue&0x70)>>4)*10+(realvalue&0x0f);
			realvalue=DS1302Read(DS1302_year_read);
	time.year=((realvalue&0x70)>>4)*10+(realvalue&0x0f);
}