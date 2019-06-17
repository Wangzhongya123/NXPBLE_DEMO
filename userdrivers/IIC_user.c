#include "IIC_user.h"

#define WRITE_MASK	0xFE
#define READ_MASK	0x01

volatile unsigned char ack=0;

/*******************************************************************************
** IO������
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
** DPS310_SDA������
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
//                    ģ����ʱ����               
//*******************************************************************
void delay_us(unsigned int nus)
{
	__NOP();
	__NOP();
	__NOP();
	__NOP();
}

//*******************************************************************
//                     �����ߺ���               
//����ԭ��: void  Start_I2c();  
//����:     ����I2C����,������I2C��ʼ����.  
//*******************************************************************
void Start_I2c_DPS310(void)
{ 	
	DPS310_SDA_OUT();
	set_SDA_DPS310;      //������ʼ�ź�
	
	delay_us(1);  	//��ʱ1us 
	set_SCL_DPS310;
	delay_us(1);  	//��ʱ1us 
	DPS310_SDA_OUT();
	clr_SDA_DPS310; //������ʼ�ź�
	delay_us(1);  	//��ʱ1us 
	clr_SCL_DPS310; //ǯסI2C���ߣ�׼�����ͻ��������
	delay_us(1);  	//��ʱ1us 
}

//*******************************************************************
//                     �������ߺ���               
//����ԭ��: void  Stop_I2c();  
//����:     ����I2C����,������I2C��������.  
//*******************************************************************
void Stop_I2c_DPS310(void)
{
	DPS310_SDA_OUT();
	clr_SDA_DPS310; 	//������ʼ�ź�
	delay_us(1);  		//��ʱ1us 
	set_SCL_DPS310;
	delay_us(1);  		//��ʱ1us 
	set_SDA_DPS310; 	//����I2C���߽����ź� 
	delay_us(1);  		//��ʱ5us 
}

//********************************************************************
//                     Ӧ���Ӻ���
//����ԭ��:  void Ack_I2c(bit a);
//����:      ����������Ӧ���ź�(������Ӧ����Ӧ���źţ���λ����a����)
//********************************************************************
void Ack_I2c_DPS310(unsigned char a)     //Ӧ����߲�Ӧ��  SDA=0Ӧ��SDA=1��Ӧ��
{
	delay_us(1);  		//��ʱ1us 
	DPS310_SDA_OUT();
	clr_SCL_DPS310;     //��ʱ���ߣ�ǯסI2C�����Ա�������� 
	delay_us(1);  		//��ʱ1us 
	if(a==0)
		clr_SDA_DPS310; //�ڴ˷���Ӧ����Ӧ���ź�
	else 
		set_SDA_DPS310;
	delay_us(1);  		//��ʱ1us     
	set_SCL_DPS310;
	delay_us(1);  		//��ʱ1us  
	clr_SCL_DPS310;     //��ʱ���ߣ�ǯסI2C�����Ա�������� 
	delay_us(1);  		//��ʱ1us
	clr_SDA_DPS310;     //���ͽ��������������ź� 
	delay_us(1);  		//��ʱ1us 
}

//*******************************************************************
//                 �ֽ����ݷ��ͺ���               
//����:     ������c���ͳ�ȥ,�����ǵ�ַ,Ҳ����������,�����ȴ�Ӧ��,����
//          ��״̬λ���в���.(��Ӧ����Ӧ��ʹack=0)     
//           ��������������ack=1; ack=0��ʾ��������Ӧ����𻵡�
//*******************************************************************
void  SendByte_DPS310(unsigned char  dat)
{
	unsigned char  BitCnt;
	for(BitCnt=0;BitCnt<8;BitCnt++)  //Ҫ���͵����ݳ���Ϊ8λ
	{
		 if((dat<<BitCnt)&0x80)
			 set_SDA_DPS310;   //�жϷ���λ
		 else 
			 clr_SDA_DPS310; 
		 
		 __NOP();// //delay_us(1); //��ʱ1us
		 set_SCL_DPS310; //��ʱ����Ϊ�ߣ�֪ͨ��������ʼ��������λ
		 __NOP();// //delay_us(1);//��ʱ3us   
		        
		 clr_SCL_DPS310; 

	}
	
	DPS310_SDA_IN();
	set_SCL_DPS310;
	delay_us(1);  //��ʱ1us   
	if(read_SDA_DPS310==1)
		ack=0;     
	else 
		ack=1;        //�ж��Ƿ���յ�Ӧ���ź� 
	clr_SCL_DPS310;
	delay_us(1);  //��ʱ1us
	DPS310_SDA_OUT();
	clr_SDA_DPS310;
	delay_us(1);  //��ʱ1us
}

//*******************************************************************
//                 �ֽ����ݽ��պ���               
//����ԭ��: UCHAR  RcvByte();
//����:        �������մ���������������,���ж����ߴ���(����Ӧ���ź�)��
//          ���������Ӧ����Ӧ��ӻ���  
//*******************************************************************  
unsigned char RcvByte_DPS310(void)
{
	unsigned char  retc=0;
	unsigned char  BitCnt;
	
	DPS310_SDA_IN();
	delay_us(1);
	for(BitCnt=0;BitCnt<8;BitCnt++)
	{        
		clr_SCL_DPS310;                  //��ʱ����Ϊ�ͣ�׼����������λ
		delay_us(1);  //��ʱ5us
		set_SCL_DPS310;                  //��ʱ����Ϊ��ʹ��������������Ч
		delay_us(1);  //��ʱ1us
		retc=retc<<1;
		if(read_SDA_DPS310==1)
			retc=retc+1; //������λ,���յ�����λ����retc�� 
		delay_us(1);  //��ʱ1us
	}
	clr_SCL_DPS310;    
	delay_us(1);        //��ʱ1us

	return(retc);
}

//*******************************************************************
//                    �����ӵ�ַ������ȡ���ֽ����ݺ���               
//����ԭ��: bit  RecndStr(UCHAR sla,UCHAR suba,ucahr *s,UCHAR no);  
//����:     ���������ߵ����͵�ַ���ӵ�ַ,�����ݣ��������ߵ�ȫ����,������
//          ��ַsla���ӵ�ַsuba�����������ݷ���sָ��Ĵ洢������no���ֽڡ�
//           �������1��ʾ�����ɹ��������������
//ע�⣺    ʹ��ǰ�����ѽ������ߡ�
//********************************************************************
unsigned char IRcvStr_DPS310(unsigned char  sla,unsigned char  *s,unsigned char  no)
{
	unsigned char i;

	Start_I2c_DPS310();                  //��������
	SendByte_DPS310(sla);                //����������ַ
	if(ack==0)//��Ӧ��
	{
		Stop_I2c_DPS310();
		return 0;
	}
	
	for(i=0;i<no-1;i++)
	{   
		*s=RcvByte_DPS310();               //
		Ack_I2c_DPS310(1);                 //���վʹ�λ 
		s++;
	} 
	*s=RcvByte_DPS310();
	Ack_I2c_DPS310(0);                   //���ͷ�Ӧλ
	Stop_I2c_DPS310();                   //�������� 
	return 1;
}

/****************************************************
�������ܣ�read_block����
���������
���������
��    ע��
*****************************************************/	
short i2c_read_block(unsigned char  address,unsigned char length,unsigned char  *read_buffer)
{
	unsigned char i=0;
	unsigned char LEN=0;
	unsigned char addr=0;
	
	addr=0x77<<1;

	addr &= WRITE_MASK;
	
	Start_I2c_DPS310();                  //��������
	
	SendByte_DPS310(addr);                //����DPS310��������������ַ
	
	if(ack==0)//�������Է��͵ĵ�ַ������Ӧ��
	{
		Stop_I2c_DPS310();
		return -1;
	}	
	
	SendByte_DPS310(address); //���ʹ�����Ŀ��Ĵ�����ַ
	
	if(ack==0)//�������Է��͵ļĴ�����ַ��Ӧ��
	{
		Stop_I2c_DPS310();
		return -1;
	}
	
	addr=0x77<<1;

	addr  |= READ_MASK;
	
	Start_I2c_DPS310(); //��������
	
	delay_us(1);        //��ʱ4us
	
	SendByte_DPS310(addr);//����DPS310��������������ַ

	if(ack==0)//�������Է��͵ĵ�ַ������Ӧ��
	{
		Stop_I2c_DPS310();
		return -1;
	}
	
	for(i=0;i<length;i++)
	{   
		*read_buffer=RcvByte_DPS310();//
		if(i==length-1)
			Ack_I2c_DPS310(1); //���վʹ�λ
		else
			Ack_I2c_DPS310(0); //���ͷ�Ӧλ
		
		read_buffer++;
		LEN++;
	} 

	delay_us(1);        //��ʱ1us
	
	Stop_I2c_DPS310(); //�������� 
	
	return LEN;
}

/****************************************************
�������ܣ�
���������
���������
��    ע��
*****************************************************/
short i2c_write_byte(unsigned char address,unsigned char  data)
{	
	unsigned char addr=0;
	
	addr=0x77<<1;
	addr &= WRITE_MASK;
	
	Start_I2c_DPS310(); //��������
	
	delay_us(1);        //��ʱ4us
	
	SendByte_DPS310(addr);//����DPS310��������������ַ
	
	if(ack==0)//�������Է��͵ĵ�ַ������Ӧ��
	{
		Stop_I2c_DPS310();
		return -1;
	}	
	
	SendByte_DPS310(address); //���ʹ�����Ŀ��Ĵ�����ַ	
	
	if(ack==0)//�������Է��͵ļĴ�����ַ��Ӧ��
	{
		Stop_I2c_DPS310();
		return -1;
	}
	
	SendByte_DPS310(data); //���ʹ�����Ŀ��Ĵ�����ַ	
	
	if(ack==0)//�������Է��͵ļĴ�����ַ��Ӧ��
	{
		Stop_I2c_DPS310();
		return -1;
	}
		
	delay_us(1);        //��ʱ1us
	
	Stop_I2c_DPS310(); //�������� 
	
	return 1;
}
/****************************************************
�������ܣ�
���������
���������
��    ע��
*****************************************************/
short i2c_read_byte(unsigned char address)
{
	unsigned char _read_byte=0;
	unsigned char addr=0;
	
	addr=0x77<<1;

	addr &= WRITE_MASK;
	
	Start_I2c_DPS310(); //��������
	
	SendByte_DPS310(addr);//����DPS310��������������ַ	
	
	if(ack==0)//�������Է��͵ĵ�ַ������Ӧ��
	{
		Stop_I2c_DPS310();
		return -1;
	}	
	
	SendByte_DPS310(address); //���ʹ�����Ŀ��Ĵ�����ַ
	
	if(ack==0)//�������Է��͵ļĴ�����ַ��Ӧ��
	{
		Stop_I2c_DPS310();
		return -1;
	}
	
	addr=0x77<<1;
	addr |= READ_MASK;
	
	Start_I2c_DPS310(); //��������
	delay_us(1);        //��ʱ4us
	
	SendByte_DPS310(addr);//����DPS310��������������ַ
	
	if(ack==0)//�������Է��͵ĵ�ַ������Ӧ��
	{
		Stop_I2c_DPS310();
		return -1;
	}
	
	_read_byte=RcvByte_DPS310();               //

	Ack_I2c_DPS310(1); 
	
	delay_us(1);        //��ʱ4us
	
	Stop_I2c_DPS310();                   //�������� 
	
	return _read_byte;	
}
