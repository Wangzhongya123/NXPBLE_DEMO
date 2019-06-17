#include "BatCharge_user.h"
#include "Adc_user.h"

/**********************************************/
/* �������ܣ���������	���ڸ��������͵����Ϣ*/
/* ��ڲ���:                              	  */
/**********************************************/
unsigned char USER_GetBatteryLevel(void)
{
	float fresult=0;//v
	
	fresult = ADC_BAT_Volt();

    /* Return battery level percentage. */
    return (uint8_t)(100*(fresult -BATTERY_MIN_VOLTAGE)/(BATTERY_FULL_VOLTAGE -BATTERY_MIN_VOLTAGE));
}

/**********************************************/
/* �������ܣ�����NTCVCC��ʹ�����������
			 ע��Ӧ�ó����ı仯				  */
/* ��ڲ���:                              	  */
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
/* �������ܣ����¶�													 */
/* ��ڲ���:
	 ����ֵ��NTC��ֵ												 */
/*********************************************************************/
float NTC_Value(void)
{
	float ADC_NTC_DET_Volt=0;
	float NTC_VCC = (float)EXRef_LDOVolt / 1000;
	float res_ntc=0;

	NTC_EN();
	ADC_NTC_DET_Volt = ADC_NTC_Volt();
	res_ntc =  (ADC_NTC_DET_Volt *  10.0f) / ( NTC_VCC - ADC_NTC_DET_Volt ) ; //����NTC����ֵ
	NTC_DIS(); 
	
	return res_ntc;
}

/**********************************************/
/* �������ܣ����оƬ����ʹ��			  	  */
/* ��ڲ���:                              	  */
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
/* �������ܣ��ⲿ�ж���������		          */
/* ��ڲ���:                              	  */
/**********************************************/
void ChargeIn_Pin_Init(void)
{

}

/**********************************************/
/* �������ܣ�ע�������븴λ���Ǹ��ù�ϵ 
			 ע��Ӧ�ó�����ͬ�����Ź��ܵ��л� */
/* ��ڲ���:                              	  */
/**********************************************/
void ChargeStart_Init(void)
{
	const uint32_t USER_ChartStart_config = (IOCON_FUNC0 |IOCON_MODE_PULLUP | IOCON_DRIVE_HIGH  );  	                                    
	IOCON_PinMuxSet(IOCON, PORT_CHARGE_IDX, CHARGESTART_PIN, USER_ChartStart_config); /* GPIOA ---CHARGE_START */	
	
    GPIO_PinInit(GPIOA, CHARGESTART_PIN, &(gpio_pin_config_t){kGPIO_DigitalInput, 1U});
}

unsigned char ChargeStart_Status(void)
{
	return GPIO_ReadPinInput(GPIOA,CHARGESTART_PIN);	
}

/**********************************************/
/* �������ܣ�����ѡ�����ţ��������õ��� 	  */					     
/* ��ڲ���:                              	  */
/**********************************************/
void CurrentChoice_Pin_Init(void)
{
	const uint32_t USER_CurrentChoice_1_config = (IOCON_FUNC0 |IOCON_MODE_HIGHZ | IOCON_DRIVE_HIGH  );  	                                    
	IOCON_PinMuxSet(IOCON, PORT_CHARGE_IDX, CurrentChoice_1_PIN, USER_CurrentChoice_1_config); /* GPIOA ---CHARGE_CURRENT_CHOICE_1 */
	const uint32_t USER_CurrentChoice_2_config = (IOCON_FUNC0 |IOCON_MODE_HIGHZ | IOCON_DRIVE_HIGH  );  	                                    
	IOCON_PinMuxSet(IOCON, PORT_CHARGE_IDX, CurrentChoice_2_PIN, USER_CurrentChoice_2_config); /* GPIOA ---CHARGE_CURRENT_CHOICE_2 */	
	
    GPIO_PinInit(GPIOA, CurrentChoice_1_PIN, &(gpio_pin_config_t){kGPIO_DigitalOutput, 0U});
    GPIO_PinInit(GPIOA, CurrentChoice_2_PIN, &(gpio_pin_config_t){kGPIO_DigitalOutput, 0U});	
}

void CurrentChoice_MIN(void)
{
	GPIO_WritePinOutput(GPIOA, CurrentChoice_1_PIN, 1);
	GPIO_WritePinOutput(GPIOA, CurrentChoice_2_PIN, 1);	
}

void CurrentChoice_MAX(void)
{
	GPIO_WritePinOutput(GPIOA, CurrentChoice_1_PIN, 0);
	GPIO_WritePinOutput(GPIOA, CurrentChoice_2_PIN, 1);		
}

void CurrentChoice_MID(void)
{	
	GPIO_WritePinOutput(GPIOA, CurrentChoice_1_PIN, 1);
	GPIO_WritePinOutput(GPIOA, CurrentChoice_2_PIN, 0);	
}

/**********************************************/
/* �������ܣ���صĵ͹��Ĺ��� ����			  */					     
/* ��ڲ���:                              	  */
/**********************************************/
/* Reinitialize peripherals after waked up from PD, 
	this function will be called in critical area */
static void USER_WakeupRestore(void)
{

}

/* ����˯��ǰ��׼������,  */
static void USER_ToSleep_Intend(void)
{
	uint32_t msk, val;

	/* Enable GPIO wakeup */
    NVIC_ClearPendingIRQ(EXT_GPIO_WAKEUP_IRQn);
    NVIC_EnableIRQ(EXT_GPIO_WAKEUP_IRQn);
}

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

void USER_ToSleep(void)
{
	__disable_irq();
	
	USER_ToSleep_Intend();//׼������˯��
	
	switch_to_OSC32M();

	POWER_LatchIO();
	POWER_EnterPowerDown(0);
	POWER_RestoreIO();
	switch_to_XTAL();

	NVIC_DisableIRQ(EXT_GPIO_WAKEUP_IRQn);
	NVIC_ClearPendingIRQ(EXT_GPIO_WAKEUP_IRQn);

	/* after waking up from pwoer down, usart config is lost, recover it */
	USER_WakeupRestore();//��˯���л���
	__enable_irq();
}

