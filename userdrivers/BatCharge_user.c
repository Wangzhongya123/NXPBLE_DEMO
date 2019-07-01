#include "BatCharge_user.h"
#include "Adc_user.h"
#include "Key_user.h"

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
		IOCON_DRIVE_HIGH                                         /* Enable HIGH drive strength */
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
	const uint32_t USER_ChartStart_config = (IOCON_FUNC0 |IOCON_MODE_PULLUP | IOCON_DRIVE_LOW  );  	                                    
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
/* �������ܣ����õ͵�ѹ �ж��븴λ			  */					     
/* ��ڲ���:                              	  */
/**********************************************/
void BOD_init(void)
{
	bod_config_t config;
	
	/* Clear reset source */
    RESET_ClearResetSource();
	
    BOD_GetDefaultConfig(&config);
    config.int_thr = kBOD_InterruptThreshold2; /*2.72V   ��ѹ�ж�ֵ*/
    config.reset_thr = kBOD_ResetThreshold2;   /*2.0V	��ѹ��λֵ */

    /*Init BOD module*/
    BOD_Init(SYSCON, &config);
    BOD_Enable(SYSCON, kBOD_InterruptEnable | kBOD_ResetEnable);//��ѹ��λ����
    NVIC_EnableIRQ(BOD_IRQn);
}
