#include "IIC_user.h"

#define WRITE_MASK	0xFE
#define READ_MASK	0x01

volatile unsigned char ack=0;

/*******************************************************************************
** IO口配置
*******************************************************************************/
void IIC_IO_Confing(void)
{
		/* Enable GPIO clock */    
    CLOCK_EnableClock(kCLOCK_Gpio);	
	
	const uint32_t DPS_CLK_config = (IOCON_FUNC0 | IOCON_MODE_PULLUP |  IOCON_DRIVE_LOW );    
	IOCON_PinMuxSet(IOCON, DPS310_PORT, PIN_CLK, DPS_CLK_config); /* SCL */

	const uint32_t DPS_SDA_config = (IOCON_FUNC0 | IOCON_MODE_PULLUP |  IOCON_DRIVE_LOW  );
	IOCON_PinMuxSet(IOCON, DPS310_PORT, PIN_SDA, DPS_SDA_config); /* SDA */
	
	GPIO_PinInit(GPIOA, PIN_CLK, &(gpio_pin_config_t){kGPIO_DigitalOutput, 1U});
	GPIO_PinInit(GPIOA, PIN_SDA, &(gpio_pin_config_t){kGPIO_DigitalOutput, 1U});
}

/*******************************************************************************
** DPS310_SDA口配置
*******************************************************************************/
void DPS310_SDA_IN(void)
{
	GPIO_PinInit(GPIOA, PIN_SDA, &(gpio_pin_config_t){kGPIO_DigitalInput, 1U});
}

void DPS310_SDA_OUT(void)
{
	GPIO_PinInit(GPIOA, PIN_SDA, &(gpio_pin_config_t){kGPIO_DigitalOutput, 1U});
}

//*******************************************************************
//                    模拟延时函数               
//*******************************************************************
void delay_us(unsigned int nus)
{
	__NOP();
	__NOP();
	__NOP();
	__NOP();
}

//*******************************************************************
//                     起动总线函数               
//函数原型: void  Start_I2c();  
//功能:     启动I2C总线,即发送I2C起始条件.  
//*******************************************************************
void Start_I2c_DPS310(void)
{ 	
	DPS310_SDA_OUT();
	set_SDA_DPS310;      //发送起始信号
	
	delay_us(1);  	//延时1us 
	set_SCL_DPS310;
	delay_us(1);  	//延时1us 
	DPS310_SDA_OUT();
	clr_SDA_DPS310; //发送起始信号
	delay_us(1);  	//延时1us 
	clr_SCL_DPS310; //钳住I2C总线，准备发送或接收数据
	delay_us(1);  	//延时1us 
}

//*******************************************************************
//                     结束总线函数               
//函数原型: void  Stop_I2c();  
//功能:     结束I2C总线,即发送I2C结束条件.  
//*******************************************************************
void Stop_I2c_DPS310(void)
{
	DPS310_SDA_OUT();
	clr_SDA_DPS310; 	//发送起始信号
	delay_us(1);  		//延时1us 
	set_SCL_DPS310;
	delay_us(1);  		//延时1us 
	set_SDA_DPS310; 	//发送I2C总线结束信号 
	delay_us(1);  		//延时5us 
}

//********************************************************************
//                     应答子函数
//函数原型:  void Ack_I2c(bit a);
//功能:      主控器进行应答信号(可以是应答或非应答信号，由位参数a决定)
//********************************************************************
void Ack_I2c_DPS310(unsigned char a)     //应答或者不应答  SDA=0应答，SDA=1非应答
{
	delay_us(1);  		//延时1us 
	DPS310_SDA_OUT();
	clr_SCL_DPS310;     //清时钟线，钳住I2C总线以便继续接收 
	delay_us(1);  		//延时1us 
	if(a==0)
		clr_SDA_DPS310; //在此发出应答或非应答信号
	else 
		set_SDA_DPS310;
	delay_us(1);  		//延时1us     
	set_SCL_DPS310;
	delay_us(1);  		//延时1us  
	clr_SCL_DPS310;     //清时钟线，钳住I2C总线以便继续接收 
	delay_us(1);  		//延时1us
	clr_SDA_DPS310;     //发送结束条件的数据信号 
	delay_us(1);  		//延时1us 
}

//*******************************************************************
//                 字节数据发送函数               
//功能:     将数据c发送出去,可以是地址,也可以是数据,发完后等待应答,并对
//          此状态位进行操作.(不应答或非应答都使ack=0)     
//           发送数据正常，ack=1; ack=0表示被控器无应答或损坏。
//*******************************************************************
void  SendByte_DPS310(unsigned char  dat)
{
	unsigned char  BitCnt;
	for(BitCnt=0;BitCnt<8;BitCnt++)  //要传送的数据长度为8位
	{
		 if((dat<<BitCnt)&0x80)
			 set_SDA_DPS310;   //判断发送位
		 else 
			 clr_SDA_DPS310; 
		 
		 __NOP();// //delay_us(1); //延时1us
		 set_SCL_DPS310; //置时钟线为高，通知被控器开始接收数据位
		 __NOP();// //delay_us(1);//延时3us   
		        
		 clr_SCL_DPS310; 

	}
	
	DPS310_SDA_IN();
	set_SCL_DPS310;
	delay_us(1);  //延时1us   
	if(read_SDA_DPS310==1)
		ack=0;     
	else 
		ack=1;        //判断是否接收到应答信号 
	clr_SCL_DPS310;
	delay_us(1);  //延时1us
	DPS310_SDA_OUT();
	clr_SDA_DPS310;
	delay_us(1);  //延时1us
}

//*******************************************************************
//                 字节数据接收函数               
//函数原型: UCHAR  RcvByte();
//功能:        用来接收从器件传来的数据,并判断总线错误(不发应答信号)，
//          发完后请用应答函数应答从机。  
//*******************************************************************  
unsigned char RcvByte_DPS310(void)
{
	unsigned char  retc=0;
	unsigned char  BitCnt;
	
	DPS310_SDA_IN();
	delay_us(1);
	for(BitCnt=0;BitCnt<8;BitCnt++)
	{        
		clr_SCL_DPS310;                  //置时钟线为低，准备接收数据位
		delay_us(1);  //延时5us
		set_SCL_DPS310;                  //置时钟线为高使数据线上数据有效
		delay_us(1);  //延时1us
		retc=retc<<1;
		if(read_SDA_DPS310==1)
			retc=retc+1; //读数据位,接收的数据位放入retc中 
		delay_us(1);  //延时1us
	}
	clr_SCL_DPS310;    
	delay_us(1);        //延时1us

	return(retc);
}

//*******************************************************************
//                    向有子地址器件读取多字节数据函数               
//函数原型: bit  RecndStr(UCHAR sla,UCHAR suba,ucahr *s,UCHAR no);  
//功能:     从启动总线到发送地址，子地址,读数据，结束总线的全过程,从器件
//          地址sla，子地址suba，读出的内容放入s指向的存储区，读no个字节。
//           如果返回1表示操作成功，否则操作有误。
//注意：    使用前必须已结束总线。
//********************************************************************
unsigned char IRcvStr_DPS310(unsigned char  sla,unsigned char  *s,unsigned char  no)
{
	unsigned char i;

	Start_I2c_DPS310();                  //启动总线
	SendByte_DPS310(sla);                //发送器件地址
	if(ack==0)//无应答
	{
		Stop_I2c_DPS310();
		return 0;
	}
	
	for(i=0;i<no-1;i++)
	{   
		*s=RcvByte_DPS310();               //
		Ack_I2c_DPS310(1);                 //接收就答位 
		s++;
	} 
	*s=RcvByte_DPS310();
	Ack_I2c_DPS310(0);                   //发送非应位
	Stop_I2c_DPS310();                   //结束总线 
	return 1;
}

/****************************************************
函数功能：read_block函数
输入参数：
输出参数：
备    注：
*****************************************************/	
short i2c_read_block(unsigned char  address,unsigned char length,unsigned char  *read_buffer)
{
	unsigned char i=0;
	unsigned char LEN=0;
	unsigned char addr=0;
	
	addr=0x77<<1;

	addr &= WRITE_MASK;
	
	Start_I2c_DPS310();                  //启动总线
	
	SendByte_DPS310(addr);                //发送DPS310传感器的器件地址
	
	if(ack==0)//传感器对发送的地址数据无应答
	{
		Stop_I2c_DPS310();
		return -1;
	}	
	
	SendByte_DPS310(address); //发送传感器目标寄存器地址
	
	if(ack==0)//传感器对发送的寄存器地址无应答
	{
		Stop_I2c_DPS310();
		return -1;
	}
	
	addr=0x77<<1;

	addr  |= READ_MASK;
	
	Start_I2c_DPS310(); //启动总线
	
	delay_us(1);        //延时4us
	
	SendByte_DPS310(addr);//发送DPS310传感器的器件地址

	if(ack==0)//传感器对发送的地址数据无应答
	{
		Stop_I2c_DPS310();
		return -1;
	}
	
	for(i=0;i<length;i++)
	{   
		*read_buffer=RcvByte_DPS310();//
		if(i==length-1)
			Ack_I2c_DPS310(1); //接收就答位
		else
			Ack_I2c_DPS310(0); //发送非应位
		
		read_buffer++;
		LEN++;
	} 

	delay_us(1);        //延时1us
	
	Stop_I2c_DPS310(); //结束总线 
	
	return LEN;
}

/****************************************************
函数功能：
输入参数：
输出参数：
备    注：
*****************************************************/
short i2c_write_byte(unsigned char address,unsigned char  data)
{	
	unsigned char addr=0;
	
	addr=0x77<<1;
	addr &= WRITE_MASK;
	
	Start_I2c_DPS310(); //启动总线
	
	delay_us(1);        //延时4us
	
	SendByte_DPS310(addr);//发送DPS310传感器的器件地址
	
	if(ack==0)//传感器对发送的地址数据无应答
	{
		Stop_I2c_DPS310();
		return -1;
	}	
	
	SendByte_DPS310(address); //发送传感器目标寄存器地址	
	
	if(ack==0)//传感器对发送的寄存器地址无应答
	{
		Stop_I2c_DPS310();
		return -1;
	}
	
	SendByte_DPS310(data); //发送传感器目标寄存器地址	
	
	if(ack==0)//传感器对发送的寄存器地址无应答
	{
		Stop_I2c_DPS310();
		return -1;
	}
		
	delay_us(1);        //延时1us
	
	Stop_I2c_DPS310(); //结束总线 
	
	return 1;
}
/****************************************************
函数功能：
输入参数：
输出参数：
备    注：
*****************************************************/
short i2c_read_byte(unsigned char address)
{
	unsigned char _read_byte=0;
	unsigned char addr=0;
	
	addr=0x77<<1;

	addr &= WRITE_MASK;
	
	Start_I2c_DPS310(); //启动总线
	
	SendByte_DPS310(addr);//发送DPS310传感器的器件地址	
	
	if(ack==0)//传感器对发送的地址数据无应答
	{
		Stop_I2c_DPS310();
		return -1;
	}	
	
	SendByte_DPS310(address); //发送传感器目标寄存器地址
	
	if(ack==0)//传感器对发送的寄存器地址无应答
	{
		Stop_I2c_DPS310();
		return -1;
	}
	
	addr=0x77<<1;
	addr |= READ_MASK;
	
	Start_I2c_DPS310(); //启动总线
	delay_us(1);        //延时4us
	
	SendByte_DPS310(addr);//发送DPS310传感器的器件地址
	
	if(ack==0)//传感器对发送的地址数据无应答
	{
		Stop_I2c_DPS310();
		return -1;
	}
	
	_read_byte=RcvByte_DPS310();               //

	Ack_I2c_DPS310(1); 
	
	delay_us(1);        //延时4us
	
	Stop_I2c_DPS310();                   //结束总线 
	
	return _read_byte;	
}
