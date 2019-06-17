#include "ISR.h"
#include "Timer_user.h"
#include "Key_user.h"
#include "workmode.h"

#include "vc.h"

#ifdef DEBUG_PRINT
	#include "uart.h"
#endif

//临时/////////////////////////////////////////////////
volatile unsigned char toggle=0;
volatile unsigned char Timer2_switch=0;

//时基/////////////////////////////////////////////////
volatile unsigned int Min_Time=0;
volatile unsigned char Sec_Time=0;
volatile unsigned int _Time=0;
volatile unsigned int _smoking_time=0;

//按键处理/////////////////////////////////////////////
volatile unsigned char _temp_holdon=0;
volatile unsigned char _time_toggle=0;

extern volatile unsigned short int PWM_Duty;//用于产生PWM波
/*******************************************************************************
 * BT1中断服务函数
 ******************************************************************************/
void Bt1Int(void)
{
    if (TRUE == Bt_GetIntFlag(TIM1))
    {
        Bt_ClearIntFlag(TIM1);
			
		//时基///////////////////////////////////////////////////////////////////////////////////////////		
		if(_Time>=49)//秒
		{
			_Time=0;
			Sec_Flag=1;	
			
			////////////////////////////////////////////////////////////
			if(Sec_Time>=59)
			{
				Sec_Time=0;				
				Min_Flag=1;	
				
				if(Min_Time>=65530)	Min_Time=0;	
				else				Min_Time++;	
			}
			else	
				Sec_Time++;					
		}
		else
		{
			_Time++;	
			m20_Sec_Flag=1;
		}
		
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		if(_time_toggle>=24)
		{
			_time_toggle=0;//充电过程中使用
			Half_Sec_Flag=1;//充电过程中使用
		}
		else	
			_time_toggle++;
		
		//按键滤波///////////////////////////////////////////////////////////////////////////////////////			
		if(Gpio_GetIO(3,1) == 0 )
		{						
			Key_Fall_Flag=1;
			
			if((FuncKey_time >= 1)&&(_temp_holdon==0))
			{
				_temp_holdon=1;
				Key_DownCount++;
				FuncKey_time=0;				
			}
			else
				FuncKey_time++;	
		}		
		else
		{
			_temp_holdon=0;
			FuncKey_time=0;
		}
		
		//得到按键扫描值///////////////////////////////////////////////////////////////////////////////							
		KeyValue=KeyScan();	
		
		//////////////////////////////////////////////////////////////////////////////////////////////
		if(Smoke_output==1)
		{
			if(_smoking_time>=8*50)	//抽烟时间超过8S
			{
				Smoke_Flag=0;
				Smoke_output=0;
				SmokeTimeOut_Flag=1;
			}	
			else
				_smoking_time++;		
		}
		else
			_smoking_time=0;

		//////////////////////////////////////////////////////////////////////////////////////////////	
		if((Smoke_Flag==1)&&(Smoke_output==1))
		{
			Timer2_switch = 0;
			Power_out_start=1;
			Output_start=1;
			Read_ADC_I_DET_Flag=0;
			Gpio_SetIO(0,3,0);
			Time2_Init(PWM_Duty);
		}

    }
}

/*******************************************************************************
 * BT2中断服务函数
 ******************************************************************************/
void Bt2Int(void)
{
    if (TRUE == Bt_GetIntFlag(TIM2))
    {
		Output_start=0;
		Gpio_SetIO(0,3,1);
        Bt_ClearIntFlag(TIM2);
		Bt_Stop(TIM2);  
		Read_ADC_I_DET_Flag=1;
    }
}
