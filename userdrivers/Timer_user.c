#include "Timer_user.h"
#include "Key_user.h"
#include "workmode.h"
#include "Load_user.h"

#define BUS_CLK_FREQ CLOCK_GetFreq(kCLOCK_ApbClk)

//时基/////////////////////////////////////////////////
volatile unsigned int Min_Time=0;
volatile unsigned char Sec_Time=0;
volatile unsigned int _Time=0;//最小时间段
volatile unsigned int _time=0;//最小时间段
volatile unsigned int _smoking_time=0;//抽烟超时临时变量
volatile unsigned int delaytobletask=0;//最小时间段//自由运行节拍

//time flag/////////////////////////////////////////////
volatile unsigned int Min_Flag=0;
volatile unsigned char Sec_Flag=0;
volatile unsigned char m20_Sec_Flag=0;
volatile unsigned char Charge_Sec_Flag=0;
volatile unsigned char Half_Sec_Flag=1;

//按键处理/////////////////////////////////////////////
volatile unsigned char _temp_holdon=0;


static void ctimer_1_match0_callback(uint32_t flags);
static void ctimer_2_match0_callback(uint32_t flags);
static void ctimer_3_match0_callback(uint32_t flags);

ctimer_callback_t ctimer_callback_table[] = {
     ctimer_1_match0_callback,ctimer_2_match0_callback, ctimer_3_match0_callback, NULL, NULL, NULL, NULL, NULL};

static void ctimer_1_match0_callback(uint32_t flags)//做为主定时器使用
{
	//自由运行节拍
	if(delaytobletask > 100000u)
		delaytobletask = 0;
	else
		delaytobletask++;
	
	//时基///////////////////////////////////////////////////////////////////////////////////////////		
	if(_Time >= SYS_FREQUENCY-1)//秒
	{
		_Time=0;
		Sec_Flag=1;	
		
		////////////////////////////////////////////////////////////
		if(Sec_Time>=59U)
		{
			Sec_Time=0;				
			Min_Flag=1;	
			
			if(Min_Time>=65530U) Min_Time=0;	
			else				 Min_Time++;	
		}
		else	
			Sec_Time++;					
	}
	else
	{
		_Time++;	
		m20_Sec_Flag=1;
	}
	
	if(_time>= ((SYS_FREQUENCY/2U)-1))
	{
		_time=0;//充电过程中使用
		Half_Sec_Flag=1;//充电过程中使用
	}
	else	
		_time++;
	
	//按键滤波///////////////////////////////////////////////////////////////////////////////////////			
	if(GPIO_ReadPinInput(KEY_PORT, USER_SW1_GPIO_PIN) == 0 )
	{						
		Key_Fall_Flag=1;
		
		if((FuncKey_time >= 2)&&(_temp_holdon==0))
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
	KeyValue=User_KeyScan();	
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	if(Smoke_output==1)
	{
		if(_smoking_time>= 8 * SYS_FREQUENCY)	//抽烟时间超过8S
		{
			Smoke_Flag = DISABLED;
			Smoke_output = DISABLED;
			SmokeTimeOut_Flag = ENABLED;
		}	
		else
			_smoking_time++;		
	}
	else
		_smoking_time=0;
	
	//////////////////////////////////////////////////////////////////////////////////////////////	
	if((Smoke_Flag == ENABLED)&&(Smoke_output == ENABLED))
	{
		Time2_Init( (unsigned int)(5000u /PWM_Duty) );//注意如何频率修改，就要修改这里
		Power_out_start=1;//用于标志测电池电量的开始 
		Output_start=1;
		Read_ADC_I_DET_Flag=0;
		PWMOUT_EN();
	}
	
}

static void ctimer_2_match0_callback(uint32_t flags)
{
	PWMOUT_DIS();
	CTIMER_StopTimer(CTIMER2);
	Output_start=0;
	Read_ADC_I_DET_Flag=1;
}

static void ctimer_3_match0_callback(uint32_t flags)
{
	GPIO_TogglePinsOutput(GPIOA, 1U << PIN_text_2_IDX);	//用做测试	
}

/**********************************************/
/* 函数功能：						          */
/* 入口参数:                              	  */
/**********************************************/
void TEST_LED_Pin_Init(void)
{
	/* Enable GPIO clock */    
    CLOCK_EnableClock(kCLOCK_Gpio);

    GPIO_PinInit(GPIOA, TEST_PIN_IDX, &(gpio_pin_config_t){kGPIO_DigitalOutput, 0U});
}

/*******************************************************************************
 * BT1自动重装计时器配置,定时20ms,50HZ		//freq为定时器的频率   做为主定时器使用
 ******************************************************************************/
void Time1_Init(unsigned int freq)
{
	ctimer_config_t 		config;
	ctimer_match_config_t 	matchConfig0;/* Match Configuration for Channel 1 */
	
	CLOCK_EnableClock(kCLOCK_Ctimer1);
	
    CTIMER_GetDefaultConfig(&config);
    CTIMER_Init(CTIMER1, &config);

    /* Configuration 0 */
    matchConfig0.enableCounterReset = true;//匹配时计数器复位
    matchConfig0.enableCounterStop = false;//
    matchConfig0.matchValue = BUS_CLK_FREQ / freq;
    matchConfig0.outControl = kCTIMER_Output_NoAction;
    matchConfig0.outPinInitState = false;
    matchConfig0.enableInterrupt = true;

    CTIMER_RegisterCallBack(CTIMER1, &ctimer_callback_table[0], kCTIMER_SingleCallback);
    CTIMER_SetupMatch(CTIMER1, kCTIMER_Match_0, &matchConfig0);
	
	CTIMER_StartTimer(CTIMER1);
}

/*******************************************************************************
//freq为定时器的频率
 ******************************************************************************/
void Time2_Init(unsigned int freq)
{
 	ctimer_config_t config;
	/* Match Configuration for Channel 0 */
	static ctimer_match_config_t matchConfig0;
	
	CLOCK_EnableClock(kCLOCK_Ctimer2);
	
    CTIMER_GetDefaultConfig(&config);
    CTIMER_Init(CTIMER2, &config);

    /* Configuration 0 */
    matchConfig0.enableCounterReset = true;//匹配时计数器复位
    matchConfig0.enableCounterStop = false;//
    matchConfig0.matchValue = BUS_CLK_FREQ / freq;
    matchConfig0.outControl = kCTIMER_Output_NoAction;
    matchConfig0.outPinInitState = false;
    matchConfig0.enableInterrupt = true;

    CTIMER_RegisterCallBack(CTIMER2, &ctimer_callback_table[1], kCTIMER_SingleCallback);
    CTIMER_SetupMatch(CTIMER2, kCTIMER_Match_0, &matchConfig0);
	
	CTIMER_StartTimer(CTIMER2);
}

/*******************************************************************************
//freq为定时器的频率
 ******************************************************************************/
void Time3_Init(unsigned int freq)
{
 	ctimer_config_t config;
	/* Match Configuration for Channel 0 */
	static ctimer_match_config_t matchConfig0;
	
	CLOCK_EnableClock(kCLOCK_Ctimer3);
	
    CTIMER_GetDefaultConfig(&config);
    CTIMER_Init(CTIMER3, &config);

    /* Configuration 0 */
    matchConfig0.enableCounterReset = true;//匹配时计数器复位
    matchConfig0.enableCounterStop = false;//
    matchConfig0.matchValue = BUS_CLK_FREQ / freq;
    matchConfig0.outControl = kCTIMER_Output_NoAction;
    matchConfig0.outPinInitState = false;
    matchConfig0.enableInterrupt = true;

    CTIMER_RegisterCallBack(CTIMER3, &ctimer_callback_table[2], kCTIMER_SingleCallback);
    CTIMER_SetupMatch(CTIMER3, kCTIMER_Match_0, &matchConfig0);
	
	CTIMER_StartTimer(CTIMER3);
}
