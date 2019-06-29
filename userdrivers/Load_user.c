#include "Load_user.h"
#include "Adc_user.h"
#include "workmode.h"

volatile unsigned char RES_TOOHOT_time = 0;//
volatile unsigned short int PWM_Duty= 50;//用于产生PWM波
volatile float Resistant = 0;//全局变量用于串口读取负载阻值

/**********************************************/
/* 函数功能：						          */
/* 入口参数:                              	  */
/**********************************************/
void LoadRESTest_Pin_Init(void)
{
	/* Enable GPIO clock */
    CLOCK_EnableClock(kCLOCK_Gpio);
	
	const uint32_t LOADTEST_PIN_config = (
		IOCON_FUNC0 |                                            /* Selects pin function 0 */
		IOCON_MODE_PULLDOWN |                                      /* Selects pull-up function */
		IOCON_DRIVE_HIGH                                          /* Enable low drive strength */
	);
	IOCON_PinMuxSet(IOCON, PORT_LOAD_TEST_IDX, LOAD_TEST_GPIO_PIN, LOADTEST_PIN_config); 

	GPIO_PinInit(GPIOA, LOAD_TEST_GPIO_PIN, &(gpio_pin_config_t){kGPIO_DigitalOutput, 0U});	
	GPIO_WritePinOutput(GPIOA, LOAD_TEST_GPIO_PIN, 0);//默认关闭
}

void LoadRESTest_EN(void)
{
	GPIO_WritePinOutput(GPIOA, LOAD_TEST_GPIO_PIN, 1);
}

void LoadRESTest_DIS(void)
{
	GPIO_WritePinOutput(GPIOA, LOAD_TEST_GPIO_PIN, 0);
}

/**********************************************/
/* 函数功能：						          */
/* 入口参数:                              	  */
/**********************************************/
void PWMOUT_Pin_Init(void)
{
	/* Enable GPIO clock */
    CLOCK_EnableClock(kCLOCK_Gpio);
	
	const uint32_t LOADTEST_PIN_config = (
		IOCON_FUNC0 |                                            /* Selects pin function 0 */
		IOCON_MODE_PULLDOWN |                                      /* Selects pull-up function */
		IOCON_DRIVE_HIGH                                          /* Enable low drive strength */
	);
	IOCON_PinMuxSet(IOCON, PORT_LOAD_PWM_IDX, LOAD_PWM_GPIO_PIN, LOADTEST_PIN_config); 

	GPIO_PinInit(GPIOA, LOAD_PWM_GPIO_PIN, &(gpio_pin_config_t){kGPIO_DigitalOutput, 0U});	
	GPIO_WritePinOutput(GPIOA, LOAD_PWM_GPIO_PIN, 0);//默认关闭	
}

void PWMOUT_EN(void)
{
	GPIO_WritePinOutput(GPIOA, LOAD_PWM_GPIO_PIN, 1);
}

void PWMOUT_DIS(void)
{
	GPIO_WritePinOutput(GPIOA, LOAD_PWM_GPIO_PIN, 0);
}

/**********************************************/
/* 函数功能；通过发热 端求阻值  			  */
/* 入口参数：无    							  */
/* 说明：									  */
/**********************************************/
Res_Test_Error CalculateResistant_Value(float *Res,float *NTC_Value)
{
	float Resistant=0;		
	float ADC_R_DET_Volt=0;		
	float ADC_R1_DET_Volt=0;	
	//float ADC_NTC_DET_Volt=0;
	float NTC_Res=10;
	
	PWMOUT_DIS();
	LoadRESTest_EN();	

	ADC_R_DET_Volt = ADC_R_Volt()  + 0.023f;
	ADC_R1_DET_Volt = ADC_R1_Volt() + 0.023f;
	
	Resistant = (ADC_R_DET_Volt *  R_25) / (ADC_R1_DET_Volt - ADC_R_DET_Volt ) ; // 由电压计算电阻

	//ADC_NTC_DET_Volt = ADC_NTC_Volt();
	//NTC_Res =  (ADC_NTC_DET_Volt *  10.0f) / (ADC_R1_DET_Volt - ADC_NTC_DET_Volt ) ; //计算NTC的阻值
	
	if(Smoke_Flag==1)
	{
		if(Resistant<=Resistant_MIN)//输出时短路
		{
			LoadRESTest_DIS();	
			
			ShortCircuit_Flag=1;//短路标志置位	
			Smoke_Flag=0;	
			
			#ifdef DEBUG_PRINT
					
			#endif
			
			*Res = Resistant;
			*NTC_Value = NTC_Res;
			
			return Resistant_ShortCircuit;			
		}	
		
		if(Resistant >= Resistant_MAX)//输出时开路
		{	
			ADC_R_DET_Volt = ADC_R_Volt();
			ADC_R1_DET_Volt = ADC_R1_Volt();
	
			Resistant = (ADC_R_DET_Volt *  R_25) / (ADC_R1_DET_Volt - ADC_R_DET_Volt ) ; 

			if(Resistant>=Resistant_MAX)//输出时开路，干烧时使用
			{
				LoadRESTest_DIS();
				
				OpenCircuit_Flag=1;//开路标志置位
				Smoke_Flag=0;	

				#ifdef DEBUG_PRINT
						
				#endif			

				*Res = Resistant;
				*NTC_Value = NTC_Res;				
				return Resistant_OpenCircuit;				
			}
		}
		
		if(Resistant >= Resistant_TOOHOT)//干烧
		{
			RES_TOOHOT_time++;

			if(RES_TOOHOT_time >= 5)//干烧
			{
				RES_TOOHOT_time=0; 
				
				LoadRESTest_DIS();
				
				Heater_OverTemp_Flag=1;//干烧置位
				Smoke_Flag=0;	

				#ifdef DEBUG_PRINT
					
				#endif			

				*Res = Resistant;
				*NTC_Value = NTC_Res;
				return Resistant_TooHot;				
			}
		}
		else
			RES_TOOHOT_time = 0 ;			
	}
	
	LoadRESTest_DIS();
	
	*Res = Resistant;
	*NTC_Value = NTC_Res;
	
	return Normal; 
}


/**********************************************/
/* 函数功能；根据目标电压调节占空比			  */
/* 入口参数：							   	  */
/* 说明：UDTY值为100时，是100%占空比
		 占空比最大为99%，占空比为0时停止工作 */	  										  
/**********************************************/
void CalculatePWMDuty(float TargerPower,float Resistant_Smoke,float Realtime_BatVolt)
{
	double _duty=0;
	
	_duty = (double)((TargerPower * Resistant_Smoke)/(Realtime_BatVolt*Realtime_BatVolt))*100U;
	
	if(_duty <= 5u)
		_duty =5u;
	else if(_duty >= 98u)
		_duty = 98u;
	else;
		
	PWM_Duty =(unsigned short int)_duty;
	//PWM_Duty = 50u;
}

/*********************************************************************/
/* 函数功能；测电阻，检测是否有开路和短路的情况*/
/* 入口参数：是否使能过温保护，1为使能，0为不使能
	 返回值：如果负载正常，返回值为1，不正常为0*/
/*********************************************************************/
Res_Test_Error CheckLoad(void)
{	
	float Resistant_Value = 0;
	float NTC_Value=0;
	
	PWMOUT_DIS();
	LoadRESTest_EN();
	
	CalculateResistant_Value(&Resistant_Value,&NTC_Value);//
		
	if((Resistant_Value <= Resistant_MIN)||(Resistant_Value >= Resistant_MAX))
	{	
		LoadRESTest_EN();
		CalculateResistant_Value(&Resistant_Value,&NTC_Value);//
		
		if((Resistant_Value <= Resistant_MIN)||(Resistant_Value >= Resistant_MAX))
		{
			LoadRESTest_DIS(); 
			Smoke_Flag = DISABLED;	
		
			if(Resistant_Value <= Resistant_MIN)//输出时短路
				ShortCircuit_Flag = ENABLED;//短路标志置位	
			else if(Resistant_Value >= Resistant_MAX)//输出时开路
				OpenCircuit_Flag = ENABLED;//开路标志置位
			else;
			
			return Resistant_otherError;	
		
		}
		else 
			return Normal;				
	}
	else 
		return Normal;	
}
