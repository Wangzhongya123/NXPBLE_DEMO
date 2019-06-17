#include "stdio.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_ctimer.h"

#include "clock_config.h"
#include "pin_mux.h"
#include "fsl_iocon.h"

#include "fsl_sctimer.h"
#include "fsl_inputmux.h"
#include "fsl_adc.h"
#include "fsl_power.h"
#include "fsl_pint.h"

#include "EmbeddedTypes.h"
#include "fsl_os_abstraction.h"
#include "fsl_os_abstraction_bm.h"
#include "fsl_common.h"
#include <string.h>
#include "GenericList.h"
#include "SerialManager.h"//串口数据

#include "SerialManager.h"
#include "Panic.h"
#include "MemManager.h"
#include "Messaging.h"
#include "FunctionLib.h"
#include "GPIO_Adapter.h"
#include "gpio_pins.h"

#include "fsl_device_registers.h"
#include "fsl_os_abstraction.h"
#include "fsl_common.h"
#include <string.h>

///////////////////////////////////////
#include "Adc_user.h"
#include "LED_user.h"
#include "Timer_user.h"
#include "Key_user.h"
#include "dps310.h"
#include "IIC_user.h"
#include "workmode.h"
#include "Load_user.h"
#include "BatCharge_user.h"
#include "userapp.h"

#define BUS_CLK_FREQ CLOCK_GetFreq(kCLOCK_ApbClk)

volatile unsigned char tick=0;	//系统工作节拍
WORK_MODE WorkMode 		=	Mode_Idle;	//系统的工作模式
WORK_MODE WorkMode_0	=	Mode_Idle;	//临时工作模式0
WORK_MODE WorkMode_1	=	Mode_Idle;	//临时工作模式1

double pressure=0; 
double temperature=0;
int ret=0;
struct dps310_state drv_state;
dps310_bus_connection cnn;
extern volatile float DPS310_Standard;//dps310标准值


/*******************************************************************************
 * Definitions
 ******************************************************************************/
extern uint8_t gAppSerMgrIf;
uint32_t i=0;

void userBOARD_InitPins(void) 
{	
	/* Enable GPIO clock */    
    CLOCK_EnableClock(kCLOCK_Gpio);
}

typedef enum ble_data_decode_out	
{
	noVaild=0,
	COM_1,
	COM_2,
	COM_3,
	COM_4,
	
}BLE_DataDecode_out;

typedef enum ble_data_decode_error	
{
	NORMAL=0,
	ERROR_1,
	ERROR_2,
	
}BLE_DataDecode_ERROR;

BLE_DataDecode_ERROR Decoding_data_from_BLE(uint8_t *pBuf,BLE_DataDecode_out *out)
{
	if(pBuf[0] == '0')
	{
		return ERROR_1;
	}
	
	switch(pBuf[0])
	{
		case 'A':*out = COM_1 ;break;
		case 'B':*out = COM_2 ;break;	
		case 'C':*out = COM_3 ;break;		
		case 'D':*out = COM_4 ;break;		
		
		default: *out = noVaild;break;
	
	}	
	memset(pBuf,0,RECEIVEDATA_NUMBER);
	
	return NORMAL;
}


char Send_Str[SENDDATA_NUMBER];
char DPS310_Send_Str[SENDDATA_NUMBER];
char Receive_Str[RECEIVEDATA_NUMBER];

void SYS_Init(void)
{
	cnn.read_byte  = &i2c_read_byte,
	cnn.read_block = &i2c_read_block,
	cnn.write_byte = &i2c_write_byte,
	cnn.delay_ms   = &delay_bybletask;	

    /* Init hardware*/
    userBOARD_InitPins();

	Serial_Print(gAppSerMgrIf, "\n\r 综合外设功能测试\n\r", gAllowToBlock_d);

	Time1_Init(SYS_FREQUENCY);
	Time2_Init(2);
	Time3_Init(2);
	ADC_Pin_init();
	ADC_Configuration();
	RGB_PWM_init();
	Key_Init();
	LoadRESTest_Pin_Init();
	PWMOUT_Pin_Init();
	IIC_IO_Confing();
	NTCPin_Init();
	Charge_Init();
	ChargeStart_Init();
	CurrentChoice_Pin_Init();
	
	TEST_LED_Pin_Init();

	LED_Flicker(GREEN_LED_Flicker,3);
	
	{
		do
		{
			ble_task();
			ret = dps310_init(&drv_state,&cnn);			
		}
		while(ret<0);
		delay_bybletask(60);
		
		do
		{
			ble_task();
			ret = dps310_get_processed_data(&drv_state,&pressure,&temperature);			
		}
		while(ret<0);
	
		{
			double pressure_sum=0;
			float pressure_i[10]={0};
			unsigned char i=0,j=0,k=0,l=0;
			float press_temp=0;

			for(i=0;i<10;i++)
			{
				do
				{
					ble_task();
					ret=dps310_get_processed_data (&drv_state,&pressure,&temperature);//pressure和temperature是全局变量
				}
				while(ret<0);
				
				pressure_i[i] = (float)pressure;
				delay_bybletask(20);
			}
			
			for(j=0;j<10-1;j++)//n个数的数列总共进行n-1次扫描
			{
				for(k=0;k<10-j-1;k++)
				{
					if(pressure_i[k]>pressure_i[k+1])//进行升序排列
					{
					   press_temp = pressure_i[k+1];
					   pressure_i[k+1] = pressure_i[k];
					   pressure_i[k] = press_temp;
					}
				}
			}

			for(l=1;l<9;l++)
				pressure_sum  +=  pressure_i[l];		

			DPS310_Standard = pressure_sum/8;
		}	
	}
	
	Batter_Refresh();
	
	Volt_Clas_Overalls =Volt_Class ;//全局的电量等级 
	
}


int user_main(void)
{
	SYS_Init();
	
    while (1)
    {
		ble_task();		

		tick=!tick;
		WorkMode=CheckWorkMode();//

		if(tick==0)   	WorkMode_0=WorkMode;	
		else			WorkMode_1=WorkMode;
			
		if(WorkMode_0==WorkMode_1);
		else
		{		
			switch(WorkMode)
			{
			//查询模式////////////////////////////////////////////////////////////////////////////////////		
				case Mode_Query:	
				{	
					Mode_Query_Work();
				}break;
			//吸烟模式////////////////////////////////////////////////////////////////////////////////////					
				case Mode_Smoke:
				{
					Mode_Smoke_Work();//		
				}break;	
			//自充模式////////////////////////////////////////////////////////////////////////////////////					
				case Mode_ChargeIn:
				{
					Mode_ChargeIn_Work();//		
				}break;
			//睡眠模式//////////////////////////////////////////////////////////////////////////////////					
				case Mode_Sleep:
				{
					Mode_Sleep_Work();
				}break;											
			//低电量模式//////////////////////////////////////////////////////////////////////////////////	
				case Mode_Lowv:
				{
					Mode_Lowv_Work();				
				}break;	
			//空闲模式////////////////////////////////////////////////////////////////////////////////////	
				case Mode_Idle:
				{
					Mode_Idle_Work();
				}break;
			//短路异常////////////////////////////////////////////////////////////////////////////////////				
				case Mode_ShortCircuit :
				{
					Mode_ShortCircuit_Work();
				}break;	
			//开路异常////////////////////////////////////////////////////////////////////////////////////				
				case Mode_OpenCircuit :
				{
					Mode_OpenCircuit_Work();
				}break;	
			//发热棒过温//////////////////////////////////////////////////////////////////////////////////				
				case Mode_OverTemp:
				{
					Fault_OverTemp_Work(); 
				}break;
			 //电池温度异常///////////////////////////////////////////////////////////////////////////////			
				case Mode_Fault_BatTemp:
				{
					Fault_BatTemp_Work();
				}break;	
			 //电池电压过低//////////////////////////////////////////////////////////////////////////////			
				case Mode_BatVoltTooLow:
				{
					Fault_BatVoltTooLow_Work();
				}break;	
				//////////////////////////////////////////////////////////////////////////////////////////////	
				default:;break;				
			}
		}	
    }
}


	//void hello(void *pParam)
	//{
	//	//float test1;
	//	
	//	//test1 = ADC_BAT_Volt();
	//	//memset(Send_Str,0,SENDDATA_NUMBER);
	//	//sprintf(Send_Str,"%.3fv\r\n", test1);	
	//	//USER_SendDateToAir((char *)Send_Str);	
	//}


	//unsigned int j=0,k=0;
	//BLE_DataDecode_out usart_out;
	//BLE_DataDecode_ERROR usart_error;

		//if(KeyValue == Key_1_Time)
		//{
		//	char str[10]={0};
		//	Res_Test_Error error_code;
		//	float res=0,ntc_res=0;
		//	
		//	KeyValue = Key_Invalid;
		//	LED_Blue_On();
		//	
		//	LoadRESTest_EN();	
		//	delay_bybletask(500);
		//	error_code = CalculateResistant_Value(&res,&ntc_res);
		//	
		//	sprintf(str,"%.2f %.2f\r\n",res,ntc_res);	
		//	USER_Serial_Printf(str);
		//}
		//else if(KeyValue == Key_3_Time)
		//{
		//	char str[8]={0};
		//	float ntc_res=0;
		//	
		//	KeyValue = Key_Invalid;
		//	LED_Green_On();	
		//	
		//	NTC_EN();
		//	delay_bybletask(500);
		//	ntc_res = NTC_Value();
		//	sprintf(str,"%.2f\r\n",ntc_res);	
		//	USER_Serial_Printf(str);	
		//}
		//else if(KeyValue == Key_5_Time)
		//{
		//	KeyValue = Key_Invalid;
		//	LED_Yellow_On();	

		//	
		//}		
		//else
		//{
		//	if(j>=10000)
		//	{
		//		j=0;
		//		
		//		if(k>=99) k=0;
		//		else k++;
		//		
		//		//LED_Breath(k);
		//		//usart_error = Decoding_data_from_BLE((uint8_t *)Receive_Str,&usart_out);
		//	}
		//	else
		//		j++;			
		//}
