#include "BatCharge_user.h"
#include "Adc_user.h"
#include "Key_user.h"

/**********************************************/
/* 函数功能：电量测试	用于给蓝牙发送电池信息*/
/* 入口参数:                              	  */
/**********************************************/
unsigned char USER_GetBatteryLevel(void)
{
	float fresult=0;//v
	
	fresult = ADC_BAT_Volt();

    /* Return battery level percentage. */
    return (uint8_t)(100*(fresult -BATTERY_MIN_VOLTAGE)/(BATTERY_FULL_VOLTAGE -BATTERY_MIN_VOLTAGE));
}

/**********************************************/
/* 函数功能：这里NTCVCC的使能与输出复用
			 注意应用场景的变化				  */
/* 入口参数:                              	  */
/**********************************************/
void NTCPin_Init(void)
{
	/* Enable GPIO clock */    
	CLOCK_EnableClock(kCLOCK_Gpio);
	
	const uint32_t USER_NTCVCC_config = (
		IOCON_FUNC0 |                                            /* Selects pin function 0 */
		IOCON_MODE_PULLUP |                                      /* Selects pull-up function */
		IOCON_DRIVE_HIGH                                          /* Enable HIGH drive strength */
	);
	IOCON_PinMuxSet(IOCON, PORT_NTCVCC_IDX, NTCVCC_GPIO_PIN, USER_NTCVCC_config); /* GPIOA ---NTC-vcc */
	
	GPIO_PinInit(GPIOA, NTCVCC_GPIO_PIN, &(gpio_pin_config_t){kGPIO_DigitalOutput, 0U});	
}

void NTC_EN(void)
{
	GPIO_WritePinOutput(GPIOA, NTCVCC_GPIO_PIN, 1);
}

void NTC_DIS(void)
{
	GPIO_WritePinOutput(GPIOA, NTCVCC_GPIO_PIN, 0);
}

/*********************************************************************/
/* 函数功能；测温度													 */
/* 入口参数:
	 返回值：NTC阻值												 */
/*********************************************************************/
float NTC_Value(void)
{
	float ADC_NTC_DET_Volt=0;
	float NTC_VCC = (float)EXRef_LDOVolt / 1000;
	float res_ntc=0;

	NTC_EN();
	ADC_NTC_DET_Volt = ADC_NTC_Volt();
	res_ntc =  (ADC_NTC_DET_Volt *  10.0f) / ( NTC_VCC - ADC_NTC_DET_Volt ) ; //计算NTC的阻值
	NTC_DIS(); 
	
	return res_ntc;
}

/**********************************************/
/* 函数功能：充电芯片控制使能			  	  */
/* 入口参数:                              	  */
/**********************************************/
void Charge_Init(void)
{
	/* Enable GPIO clock */    
	CLOCK_EnableClock(kCLOCK_Gpio);
	
	const uint32_t USER_CHARGE_EN_config = (
		IOCON_FUNC0 |                                            /* Selects pin function 0 */
		IOCON_MODE_PULLDOWN |                                     /* Selects pull-down function */
		IOCON_DRIVE_HIGH                                          /* Enable high drive strength */
	);
	IOCON_PinMuxSet(IOCON, PORT_CHARGE_IDX, CHARGE_EN_PIN, USER_CHARGE_EN_config); /* GPIOA ---CHARGE_EN */
	
	GPIO_PinInit(GPIOA, CHARGE_EN_PIN, &(gpio_pin_config_t){kGPIO_DigitalOutput, 0U});	
}

void Charge_EN(void)
{
	GPIO_WritePinOutput(GPIOA, CHARGE_EN_PIN, 1);
}

void Charge_DIS(void)
{
	GPIO_WritePinOutput(GPIOA, CHARGE_EN_PIN, 0);
}

/**********************************************/
/* 函数功能：外部中断输入引脚		          */
/* 入口参数:                              	  */
/**********************************************/
void ChargeIn_Pin_Init(void)
{

}

/**********************************************/
/* 函数功能：注意这里与复位脚是复用关系 
			 注意应用场景不同的引脚功能的切换 */
/* 入口参数:                              	  */
/**********************************************/
void ChargeStart_Init(void)
{
	const uint32_t USER_ChartStart_config = (IOCON_FUNC0 |IOCON_MODE_PULLUP | IOCON_DRIVE_LOW  );  	                                    
	IOCON_PinMuxSet(IOCON, PORT_CHARGE_IDX, CHARGESTART_PIN, USER_ChartStart_config); /* GPIOA ---CHARGE_START */	
	
    GPIO_PinInit(GPIOA, CHARGESTART_PIN, &(gpio_pin_config_t){kGPIO_DigitalInput, 1U});
}

unsigned char ChargeStart_Status(void)
{
	return GPIO_ReadPinInput(GPIOA,CHARGESTART_PIN);	
}

/**********************************************/
/* 函数功能：电流选择引脚，用于配置电流 	  */					     
/* 入口参数:                              	  */
/**********************************************/
void CurrentChoice_Pin_Init(void)
{
	const uint32_t USER_CurrentChoice_1_config = (IOCON_FUNC0 |IOCON_MODE_HIGHZ | IOCON_DRIVE_LOW  );  	                                    
	IOCON_PinMuxSet(IOCON, PORT_CHARGE_IDX, CurrentChoice_1_PIN, USER_CurrentChoice_1_config); /* GPIOA ---CHARGE_CURRENT_CHOICE_1 */
	GPIO_PinInit(GPIOA, CurrentChoice_1_PIN, &(gpio_pin_config_t){kGPIO_DigitalInput, 0U});
	
	const uint32_t USER_CurrentChoice_2_config = (IOCON_FUNC0 |IOCON_MODE_HIGHZ | IOCON_DRIVE_LOW  );  	                                    
	IOCON_PinMuxSet(IOCON, PORT_CHARGE_IDX, CurrentChoice_2_PIN, USER_CurrentChoice_2_config); /* GPIOA ---CHARGE_CURRENT_CHOICE_2 */	
    GPIO_PinInit(GPIOA, CurrentChoice_2_PIN, &(gpio_pin_config_t){kGPIO_DigitalInput, 0U});	
}

void CurrentChoice_MIN(void)
{
	const uint32_t USER_CurrentChoice_1_config = (IOCON_FUNC0 |IOCON_MODE_HIGHZ | IOCON_DRIVE_LOW  );  	                                    
	IOCON_PinMuxSet(IOCON, PORT_CHARGE_IDX, CurrentChoice_1_PIN, USER_CurrentChoice_1_config); /* GPIOA ---CHARGE_CURRENT_CHOICE_1 */
	GPIO_PinInit(GPIOA, CurrentChoice_1_PIN, &(gpio_pin_config_t){kGPIO_DigitalInput, 0U});
	
	const uint32_t USER_CurrentChoice_2_config = (IOCON_FUNC0 |IOCON_MODE_HIGHZ | IOCON_DRIVE_LOW  );  	                                    
	IOCON_PinMuxSet(IOCON, PORT_CHARGE_IDX, CurrentChoice_2_PIN, USER_CurrentChoice_2_config); /* GPIOA ---CHARGE_CURRENT_CHOICE_2 */	
    GPIO_PinInit(GPIOA, CurrentChoice_2_PIN, &(gpio_pin_config_t){kGPIO_DigitalInput, 0U});	
}

void CurrentChoice_MAX(void)
{
	const uint32_t USER_CurrentChoice_1_config = (IOCON_FUNC0 |IOCON_MODE_HIGHZ | IOCON_DRIVE_LOW  );  	                                    
	IOCON_PinMuxSet(IOCON, PORT_CHARGE_IDX, CurrentChoice_1_PIN, USER_CurrentChoice_1_config); /* GPIOA ---CHARGE_CURRENT_CHOICE_1 */
	GPIO_PinInit(GPIOA, CurrentChoice_1_PIN, &(gpio_pin_config_t){kGPIO_DigitalInput, 0U});
	
	const uint32_t USER_CurrentChoice_2_config = (IOCON_FUNC0 |IOCON_MODE_PULLDOWN | IOCON_DRIVE_LOW  );  	                                    
	IOCON_PinMuxSet(IOCON, PORT_CHARGE_IDX, CurrentChoice_2_PIN, USER_CurrentChoice_2_config); /* GPIOA ---CHARGE_CURRENT_CHOICE_2 */	
    GPIO_PinInit(GPIOA, CurrentChoice_2_PIN, &(gpio_pin_config_t){kGPIO_DigitalOutput, 0U});		
	GPIO_WritePinOutput(GPIOA, CurrentChoice_2_PIN, 0);		
}

void CurrentChoice_MID(void)
{	
	const uint32_t USER_CurrentChoice_1_config = (IOCON_FUNC0 | IOCON_MODE_PULLDOWN| IOCON_DRIVE_LOW  );  	                                    
	IOCON_PinMuxSet(IOCON, PORT_CHARGE_IDX, CurrentChoice_1_PIN, USER_CurrentChoice_1_config); /* GPIOA ---CHARGE_CURRENT_CHOICE_1 */
	GPIO_PinInit(GPIOA, CurrentChoice_1_PIN, &(gpio_pin_config_t){kGPIO_DigitalOutput, 0U});
	GPIO_WritePinOutput(GPIOA, CurrentChoice_1_PIN, 0);
	
	const uint32_t USER_CurrentChoice_2_config = (IOCON_FUNC0 |IOCON_MODE_HIGHZ | IOCON_DRIVE_LOW  );  	                                    
	IOCON_PinMuxSet(IOCON, PORT_CHARGE_IDX, CurrentChoice_2_PIN, USER_CurrentChoice_2_config); /* GPIOA ---CHARGE_CURRENT_CHOICE_2 */	
    GPIO_PinInit(GPIOA, CurrentChoice_2_PIN, &(gpio_pin_config_t){kGPIO_DigitalInput, 0U});			
}

/**********************************************/
/* 函数功能：电池的低功耗功能 设置			  */					     
/* 入口参数:                              	  */
/**********************************************/
static void switch_to_OSC32M(void)
{
    POWER_WritePmuCtrl1(SYSCON, SYSCON_PMU_CTRL1_OSC32M_DIS_MASK, SYSCON_PMU_CTRL1_OSC32M_DIS(0U));
    SYSCON->CLK_CTRL = (SYSCON->CLK_CTRL & ~SYSCON_CLK_CTRL_SYS_CLK_SEL_MASK) | SYSCON_CLK_CTRL_SYS_CLK_SEL(0U);
}

static void switch_to_XTAL(void)
{
    /* switch to XTAL after it is stable */
    while (!(SYSCON_SYS_MODE_CTRL_XTAL_RDY_MASK & SYSCON->SYS_MODE_CTRL))
        ;
    SYSCON->CLK_CTRL = (SYSCON->CLK_CTRL & ~SYSCON_CLK_CTRL_SYS_CLK_SEL_MASK) | SYSCON_CLK_CTRL_SYS_CLK_SEL(1U);
    POWER_WritePmuCtrl1(SYSCON, SYSCON_PMU_CTRL1_OSC32M_DIS_MASK, SYSCON_PMU_CTRL1_OSC32M_DIS(1U));
}

/* Reinitialize peripherals after waked up from PD, 
	this function will be called in critical area */
void USER_WakeupRestore(void)
{
	POWER_RestoreIO();
	switch_to_XTAL();

	NVIC_DisableIRQ(EXT_GPIO_WAKEUP_IRQn);
	NVIC_ClearPendingIRQ(EXT_GPIO_WAKEUP_IRQn);

	
	__enable_irq();
}

/* 进入睡眠前的准备工作,  */
void USER_ToSleep_Intend(void)
{
	uint32_t msk, val;
	
	msk = SYSCON_PMU_CTRL1_XTAL32K_PDM_DIS_MASK | SYSCON_PMU_CTRL1_RCO32K_PDM_DIS_MASK |
          SYSCON_PMU_CTRL1_XTAL32K_DIS_MASK | SYSCON_PMU_CTRL1_RCO32K_DIS_MASK;
	
	val = SYSCON_PMU_CTRL1_XTAL32K_PDM_DIS(1U)  /* switch off XTAL32K during power down */
	  | SYSCON_PMU_CTRL1_RCO32K_PDM_DIS(1U) /* switch off RCO32K during power down */
	  | SYSCON_PMU_CTRL1_XTAL32K_DIS(1U)    /* switch off XTAL32K at all time */
	  | SYSCON_PMU_CTRL1_RCO32K_DIS(1U);    /* switch off RCO32K */

	POWER_WritePmuCtrl1(SYSCON, msk, val);
	
	/* Enable GPIO wakeup */
    NVIC_ClearPendingIRQ(EXT_GPIO_WAKEUP_IRQn);
    NVIC_EnableIRQ(EXT_GPIO_WAKEUP_IRQn);

	SYSCON->PIO_WAKEUP_LVL0 = SYSCON->PIO_WAKEUP_LVL0 | USER_SW1_GPIO_PIN_MASK;
    SYSCON->PIO_WAKEUP_EN0 = SYSCON->PIO_WAKEUP_EN0 | USER_SW1_GPIO_PIN_MASK;
	
	__disable_irq();
	
	POWER_LatchIO();
    CLOCK_DisableClock(kCLOCK_Flexcomm0);
}
