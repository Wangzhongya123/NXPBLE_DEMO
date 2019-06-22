#include "LED_user.h"

/**********************************************/
/* 函数功能：						          */
/* 入口参数:                              	  */
/**********************************************/
void RGB_PWM_init(void)
{
    uint32_t sctimerClock = BUS_CLK_FREQ;
    sctimer_config_t 			sctimerInfo;
    sctimer_pwm_signal_param_t 	pwmParam;
	uint32_t event;
	
	/* Enable GPIO clock */    
    CLOCK_EnableClock(kCLOCK_Gpio);
		
	const uint32_t LED_R_config = (IOCON_FUNC2 | IOCON_MODE_HIGHZ |  IOCON_DRIVE_LOW  );   
	IOCON_PinMuxSet(IOCON, 0, R_PIN_IDX, LED_R_config); /* RGB --R */
	const uint32_t LED_G_config = (IOCON_FUNC2 | IOCON_MODE_HIGHZ |IOCON_DRIVE_LOW  );     
	IOCON_PinMuxSet(IOCON, 0, G_PIN_IDX, LED_G_config); /* RGB --G */
	const uint32_t LED_B_config = (IOCON_FUNC2 | IOCON_MODE_HIGHZ |IOCON_DRIVE_LOW  );     
	IOCON_PinMuxSet(IOCON, 0, B_PIN_IDX, LED_B_config); /* RGB --B */	
	
	#define PWM_LEVEL 		 kSCTIMER_LowTrue
	#define PWM_ALIGENDMODE	 kSCTIMER_EdgeAlignedPwm
	
	/* Initialize SCTimer module */	
    SCTIMER_GetDefaultConfig(&sctimerInfo);
    SCTIMER_Init(SCT0, &sctimerInfo);

    /* Configure second PWM with different duty cycle but same frequency as before */
    pwmParam.output = LED_RED_CH;
    pwmParam.level = PWM_LEVEL;
    pwmParam.dutyCyclePercent = 1;
	SCTIMER_SetupPwm(SCT0, &pwmParam, PWM_ALIGENDMODE, 24000U, sctimerClock, &event);
	
    /* Configure first PWM with frequency 24kHZ from output 0 */
    pwmParam.output = LED_GREEN_CH;
    pwmParam.level = PWM_LEVEL;
    pwmParam.dutyCyclePercent = 1;
    SCTIMER_SetupPwm(SCT0, &pwmParam, PWM_ALIGENDMODE, 24000U, sctimerClock, &event);	
	
    /* Configure first PWM with frequency 24kHZ from output 3 */
    pwmParam.output = LED_BLUE_CH;
    pwmParam.level = PWM_LEVEL;
    pwmParam.dutyCyclePercent = 1;
    SCTIMER_SetupPwm(SCT0, &pwmParam, PWM_ALIGENDMODE, 24000U, sctimerClock, &event);		
	
    /* Start the timer */
    SCTIMER_StartTimer(SCT0, kSCTIMER_Counter_L);
}

//static void PWM_DutyPercet_Change(sctimer_out_t out_channel,unsigned int dutyCyclePercent)
void PWM_DutyPercet_Change(led_rgb_set led_color)
{
    sctimer_config_t sctimerInfo;
    sctimer_pwm_signal_param_t pwmParam;
    uint32_t event;
	uint32_t sctimerClock = BUS_CLK_FREQ;
	
	SCTIMER_StopTimer(SCT0, kSCTIMER_Counter_L);
	
	/* Initialize SCTimer module */
	SCTIMER_GetDefaultConfig(&sctimerInfo);
	SCTIMER_Init(SCT0, &sctimerInfo);
	
	//RED
	pwmParam.output = LED_RED_CH;
	pwmParam.level = PWM_LEVEL;
	pwmParam.dutyCyclePercent = led_color.LED_RED_Duty;
	SCTIMER_SetupPwm(SCT0, &pwmParam, PWM_ALIGENDMODE, 24000U, sctimerClock, &event); 	
	
	//GREEN
	pwmParam.output = LED_GREEN_CH;
	pwmParam.level = PWM_LEVEL;
	pwmParam.dutyCyclePercent = led_color.LED_GREEN_Duty;
	SCTIMER_SetupPwm(SCT0, &pwmParam, PWM_ALIGENDMODE, 24000U, sctimerClock, &event); 	
	
	//BLUE
	pwmParam.output = LED_BLUE_CH;
	pwmParam.level = PWM_LEVEL;
	pwmParam.dutyCyclePercent = led_color.LED_BLUE_Duty;
	SCTIMER_SetupPwm(SCT0, &pwmParam, PWM_ALIGENDMODE, 24000U, sctimerClock, &event); 
	
	SCTIMER_StartTimer(SCT0, kSCTIMER_Counter_L);
}

/**********************************************/
/* 函数功能：	呼吸使用			          */
/* 入口参数:                              	  */
/**********************************************/
void LED_Red_PWM_On(void)
{
	led_rgb_set led_set={
		.LED_BLUE_Duty	=	1,
		.LED_RED_Duty 	=	100,
		.LED_GREEN_Duty =	1,
	};

	PWM_DutyPercet_Change(led_set);
}
void LED_Blue_PWM_On(void)
{
	led_rgb_set led_set={
		.LED_BLUE_Duty	=	100,
		.LED_RED_Duty 	=	1,
		.LED_GREEN_Duty =	1,	
	};

	PWM_DutyPercet_Change(led_set);
}
void LED_Green_PWM_On(void)
{
	led_rgb_set led_set={
		.LED_BLUE_Duty	=	1,
		.LED_RED_Duty 	=	1,
		.LED_GREEN_Duty =	100,	
	};
	
	PWM_DutyPercet_Change(led_set);
}
void LED_Yellow_PWM_On(void)
{
	led_rgb_set led_set={
		.LED_BLUE_Duty	=	1,
		.LED_RED_Duty 	=	100,
		.LED_GREEN_Duty =	100,	
	};
	
	PWM_DutyPercet_Change(led_set);
}
void LED_All_PWM_On(void)
{
	led_rgb_set led_set={
		.LED_BLUE_Duty	=	100,
		.LED_RED_Duty 	=	100,
		.LED_GREEN_Duty =	100,	
	};
	
	PWM_DutyPercet_Change(led_set);
}

void LED_Breath(led_rgb_set LED_DUTY)
{
	PWM_DutyPercet_Change(LED_DUTY);
}

void LED_All_PWM_Off(void)
{
	led_rgb_set led_set={
		.LED_BLUE_Duty	=	1,
		.LED_RED_Duty 	=	1,
		.LED_GREEN_Duty =	1,	
	};
	
	PWM_DutyPercet_Change(led_set);
}

void LED_White_Breath(unsigned char duty)
{
	led_rgb_set led_set={
		.LED_BLUE_Duty	=	duty,
		.LED_RED_Duty 	=	duty,
		.LED_GREEN_Duty =	duty,	
	};
	
	PWM_DutyPercet_Change(led_set);
}

/**********************************************/
/* 函数功能：						          */
/* 入口参数:                              	  */
/**********************************************/
void LED_Pin_Init(void)
{
	/* Enable GPIO clock */    
    CLOCK_EnableClock(kCLOCK_Gpio);
		
	const uint32_t LED_R_config = (IOCON_FUNC0 | IOCON_MODE_PULLUP |IOCON_DRIVE_LOW  );   
	IOCON_PinMuxSet(IOCON, 0, R_PIN_IDX, LED_R_config); /* RGB --R */
	GPIO_PinInit(GPIOA, R_PIN_IDX, &(gpio_pin_config_t){kGPIO_DigitalOutput, 1U});
	
	const uint32_t LED_G_config = (IOCON_FUNC0 | IOCON_MODE_PULLUP |IOCON_DRIVE_LOW  );     
	IOCON_PinMuxSet(IOCON, 0, G_PIN_IDX, LED_G_config); /* RGB --G */
	GPIO_PinInit(GPIOA, G_PIN_IDX, &(gpio_pin_config_t){kGPIO_DigitalOutput, 1U});
	
	const uint32_t LED_B_config = (IOCON_FUNC0 | IOCON_MODE_PULLUP |IOCON_DRIVE_LOW  );     
	IOCON_PinMuxSet(IOCON, 0, B_PIN_IDX, LED_B_config); /* RGB --B */	
	GPIO_PinInit(GPIOA, B_PIN_IDX, &(gpio_pin_config_t){kGPIO_DigitalOutput, 1U});
}

/**********************************************/
/* 函数功能：						          */
/* 入口参数:                              	  */
/**********************************************/
void LED_Red_On(void)
{
	GPIO_WritePinOutput(GPIOA, R_PIN_IDX, 0);
}
void LED_Blue_On(void)
{
	GPIO_WritePinOutput(GPIOA, B_PIN_IDX, 0);
}
void LED_Green_On(void)
{
	GPIO_WritePinOutput(GPIOA, G_PIN_IDX, 0);
}
void LED_Yellow_On(void)
{
	GPIO_WritePinOutput(GPIOA, R_PIN_IDX, 0);
	GPIO_WritePinOutput(GPIOA, G_PIN_IDX, 0);
}
void LED_All_On(void)
{
	GPIO_WritePinOutput(GPIOA, R_PIN_IDX, 0);	
	GPIO_WritePinOutput(GPIOA, B_PIN_IDX, 0);
	GPIO_WritePinOutput(GPIOA, G_PIN_IDX, 0);
}


/**********************************************/
/* 函数功能：						          */
/* 入口参数:                              	  */
/**********************************************/
void LED_Red_Off(void)
{
	GPIO_WritePinOutput(GPIOA, R_PIN_IDX, 1);
}
void LED_Blue_Off(void)
{
	GPIO_WritePinOutput(GPIOA, B_PIN_IDX, 1);
}
void LED_Green_Off(void)
{
	GPIO_WritePinOutput(GPIOA, G_PIN_IDX, 1);
}
void LED_Yellow_Off(void)
{
	GPIO_WritePinOutput(GPIOA, R_PIN_IDX, 1);
	GPIO_WritePinOutput(GPIOA, G_PIN_IDX, 1);	
}
void LED_All_Off(void)
{
	GPIO_WritePinOutput(GPIOA, R_PIN_IDX, 1);	
	GPIO_WritePinOutput(GPIOA, B_PIN_IDX, 1);
	GPIO_WritePinOutput(GPIOA, G_PIN_IDX, 1);	
}

/**********************************************/
/* 函数功能：LED灯闪烁				          */
/* 入口参数: 
#define	No_LED_Flicker 		0
#define	RED_LED_Flicker  	1
#define	GREEN_LED_Flicker	2
#define	BLUE_LED_Flicker	3
#define	YELLOW_LED_Flicker	4
#define	WHITE_LED_Flicker	5
***********************************************/
extern void ble_task(void);
extern void RestartTiming(void);
extern volatile unsigned int delaytobletask;//最小时间段

void LED_Flicker(unsigned char led_colour,unsigned char num)
{
	unsigned char i;
	
	for(i=0;i<num;i++)
	{	
		delaytobletask = 0;
		LED_All_Off();
		
		while(delaytobletask<=13u)
			ble_task();		
		
		delaytobletask = 0;
		
		switch(led_colour)
		{
			case RED_LED_Flicker:  		LED_Red_On(); 	;break;
			case GREEN_LED_Flicker: 	LED_Green_On();	;break;
			case BLUE_LED_Flicker:  	LED_Blue_On();	;break;
			case YELLOW_LED_Flicker: 	LED_Yellow_On();;break;
			case WHITE_LED_Flicker: 	LED_All_On(); 	;break;		
			default:					LED_All_Off();	;break;	
		}
		
		while(delaytobletask<=13u)
			ble_task();
	}
	LED_All_Off();
}	
