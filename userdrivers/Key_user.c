#include "Key_user.h"

Key_Value KeyValue ; // �����µĴ�����

volatile unsigned char 	Key_Fall_Flag=0; //���ɰ������±�־
volatile unsigned int 	Key_HodeOnTime=0;	// �����µ�ʱ��	
volatile unsigned int 	Key_RleaseTime=0; // �����ͷŵ�ʱ��
volatile unsigned char 	Key_DownCount=0; // �����µĴ���
volatile unsigned int 	Key_RleaseTime_onetime=0; // �����ͷŵ�ʱ��,�������Ƽ����ڰ�����
volatile unsigned char  FuncKey_time=0;

/**********************************************/
/* ��������:������������ ��ʼ��  0			  */
/* ��ڲ�������                               */
/**********************************************/
void Key_Init(void)
{	
  const uint32_t USER_KEY_config = (
    IOCON_FUNC0 |                                            /* Selects pin function 0 */
    IOCON_MODE_PULLUP |                                      /* Selects pull-up function */
    IOCON_DRIVE_LOW                                          /* Enable low drive strength */
  );
  IOCON_PinMuxSet(IOCON, PORTKEY_IDX, USER_SW1_GPIO_PIN, USER_KEY_config); /* GPIOA ---KEY */	
}

/************************************************************/
/* ��������:����ɨ�����		     		  				*/
/* ��ڲ�������     										*/ 
/* volatile unsigned int Key_PressTime=0;  �����µ�ʱ��	*/ 
/* volatile unsigned int Key_RleaseTime=0; �����ͷŵ�ʱ��	*/
/* volatile unsigned int Key_DownCount=0;  �����µĴ���	 	*/ 	
/************************************************************/
Key_Value User_KeyScan(void)
{
	if(Key_Fall_Flag==1)		
	{
		if( GPIO_ReadPinInput(KEY_PORT, USER_SW1_GPIO_PIN) == 0)	//����
		{		
			Key_RleaseTime=0;//���ɿ���ʱ��0	
			Key_HodeOnTime++;
			
			Key_RleaseTime_onetime++;
			
			if( Key_HodeOnTime>= 100)//2��
			{ 	
				Key_DownCount=0;
				Key_HodeOnTime=0;
				Key_RleaseTime=0;
				Key_Fall_Flag=0;
				
				Key_RleaseTime_onetime=0;
				
				KeyValue=Key_LongTime;
				return  Key_LongTime;
			} 
		}	
		else				
		{
			 Key_HodeOnTime=0;//���¼�ʱ��0	  
			 Key_RleaseTime++;//	
		}	
	
		if(Key_RleaseTime >= 10)  // �ɿ���ʱ�䳬����10*20MS
		{						
			Key_Fall_Flag=0;
			Key_HodeOnTime=0;//
			Key_RleaseTime=0;//���ɿ���ʱ��0
			
			//if((Key_DownCount==1)&&(Key_RleaseTime_onetime<=25))
			if(Key_DownCount==1)
			{
				Key_RleaseTime_onetime=0;
				
				Key_DownCount=0;
				KeyValue=Key_1_Time;
				
				return  Key_1_Time;
			}
			else if(Key_DownCount==3)
			{
				Key_RleaseTime_onetime=0;
				
				Key_DownCount=0;
				KeyValue=Key_3_Time;
				
				return  Key_3_Time;
			}
			else if(Key_DownCount==5)
			{
				Key_RleaseTime_onetime=0;
				
				Key_DownCount=0;
				KeyValue=Key_5_Time;
				
				return  Key_5_Time;
			}
			else
			{
				Key_RleaseTime_onetime=0;
				Key_DownCount=0;
				KeyValue=Key_Invalid;
				
				return  Key_Invalid;
			}							
		} 
		else
			return  Key_Invalid;					
	}
	else
		return  Key_Invalid;	
}

/**********************************************/
/* ��������:�����жϺ��� 		  			  */
/* ��ڲ�������                               */
/**********************************************/
static void pint_intr_callback(pint_pin_int_t pintr, uint32_t pmatch_status)//�����жϴ�����
{
    GPIO_TogglePinsOutput(GPIOA, 1U << 8u);  
}

/**********************************************/
/* ��������:�ж�����	  			 		  */
/* ��ڲ�������                               */
/**********************************************/
void PINT_INIT(void)
{
    /* Connect trigger sources to PINT */
    INPUTMUX_Init(INPUTMUX);
    INPUTMUX_AttachSignal(INPUTMUX, kPINT_PinInt0, USER_PINT_INT0_SRC);
    INPUTMUX_AttachSignal(INPUTMUX, kPINT_PinInt1, USER_PINT_INT1_SRC);

    /* Turnoff clock to inputmux to save power. Clock is only needed to make changes */
    INPUTMUX_Deinit(INPUTMUX);

    /* Initialize PINT */
    PINT_Init(PINT);

    /* Setup Pin Interrupt 0 for rising edge */
    PINT_PinInterruptConfig(PINT, kPINT_PinInt0, kPINT_PinIntEnableRiseEdge, pint_intr_callback);

    /* Setup Pin Interrupt 1 for falling edge */
    PINT_PinInterruptConfig(PINT, kPINT_PinInt1, kPINT_PinIntEnableFallEdge, pint_intr_callback);

    /* Enable callbacks for PINT */
    PINT_EnableCallback(PINT);
}

