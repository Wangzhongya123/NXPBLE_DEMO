#include "Adc_user.h"

volatile uint32_t g_AdcVinn;
volatile float g_AdcBandgap;
/*********************************************************
**ADC 通道和模式设置
*********************************************************/
void ADC_Pin_init(void)
{
	/* Enable GPIO clock */    
    CLOCK_EnableClock(kCLOCK_Gpio);
	
	const uint32_t ADC_BATDET_PIN_config = (IOCON_FUNC1 | IOCON_MODE_HIGHZ | IOCON_DRIVE_LOW );     
	IOCON_PinMuxSet(IOCON, 0, BAT_DET_PIN, ADC_BATDET_PIN_config); /**/
	
	const uint32_t ADC_RDET_1_PIN_config = (IOCON_FUNC1 | IOCON_MODE_HIGHZ | IOCON_DRIVE_LOW );     
	IOCON_PinMuxSet(IOCON, 0, R_DET_1_PIN, ADC_RDET_1_PIN_config); /**/	
	
	const uint32_t ADC_RDET_PIN_config = (IOCON_FUNC1 | IOCON_MODE_HIGHZ | IOCON_DRIVE_LOW );     
	IOCON_PinMuxSet(IOCON, 0, R_DET_PIN, ADC_RDET_PIN_config); /**/
	
	const uint32_t ADC_CHARGEDET_PIN_config = (IOCON_FUNC1 | IOCON_MODE_HIGHZ | IOCON_DRIVE_LOW );     
	IOCON_PinMuxSet(IOCON, 0, CHARGE_DET_PIN, ADC_CHARGEDET_PIN_config); /**/
	
	const uint32_t ADC_NTCDET_config = (IOCON_FUNC1 | IOCON_MODE_HIGHZ | IOCON_DRIVE_LOW );     
	IOCON_PinMuxSet(IOCON, 0, NTC_DET_PIN, ADC_NTCDET_config); /**/
}


void ADC_Configuration(void)
{
    adc_config_t adcConfigStruct;
    adc_sd_config_t adcSdConfigStruct;

	#define USER_DEFAULT_ADC_CHANNEL 		BAT_DET                    /* Select channel */
	#define USER_DEFAULT_ADC_CFG_IDX 		0                          /* Select configuration */
	#define USER_DEFAULT_ADC_TRIGGER 		kADC_TriggerSelectSoftware /* Software trigger */
	#define USER_DEFAULT_ADC_CONVMODE 		kADC_ConvModeSingle	
	#define USER_DEFAULT_ADC_REFSOURCE 		kADC_RefSourceVccWithDriver
	#define USER_DEFAULT_ADC_CLOCK          kADC_Clock2M
	#define LDO_V							EXRef_LDOVolt
	#define SDADC_DOWNSAMPLE				kADC_DownSample64
	
	/* Power on ADC */
    POWER_EnableADC(true);

    /*** Initial ADC to default configuration.*/
    ADC_GetDefaultConfig(&adcConfigStruct);
    adcConfigStruct.channelEnable = (1U << USER_DEFAULT_ADC_CHANNEL);
    adcConfigStruct.channelConfig = (USER_DEFAULT_ADC_CFG_IDX << USER_DEFAULT_ADC_CHANNEL);
    adcConfigStruct.triggerSource = USER_DEFAULT_ADC_TRIGGER;//配置触发源
    adcConfigStruct.convMode = USER_DEFAULT_ADC_CONVMODE;
	adcConfigStruct.clock = USER_DEFAULT_ADC_CLOCK;
	adcConfigStruct.refSource = USER_DEFAULT_ADC_REFSOURCE;
    ADC_Init(ADC, &adcConfigStruct);

	#define USER_DEFAULT_SD_VINNSELECT       kADC_VinnSelectVref0P75
	#define USER_DEFAULT_SD_REFGAIN			 kADC_RefGain1
	
    /* Initial ADC Sigma Delta(SD) configuration */
    ADC_GetSdDefaultConfig(&adcSdConfigStruct);
	adcSdConfigStruct.refGain= USER_DEFAULT_SD_REFGAIN;
	adcSdConfigStruct.downSample = SDADC_DOWNSAMPLE;
	adcSdConfigStruct.vinnSelect = USER_DEFAULT_SD_VINNSELECT;
    ADC_SetSdConfig(ADC, USER_DEFAULT_ADC_CFG_IDX, &adcSdConfigStruct);
	ADC_PgaChopperEnable(ADC, true);

    /* Bandgap voltage */
    g_AdcBandgap = ADC_GetBandgapCalibrationResult(ADC, USER_DEFAULT_ADC_CFG_IDX);
    /* Calibration VINN value */
    g_AdcVinn = ADC_GetVinnCalibrationResult(ADC, &adcConfigStruct);

    /* Enable ADC */
    ADC_Enable(ADC, true);
}

/*********************************************************
**ADC 通道和模式设置
*********************************************************/
static float ADC_Channel_Result(unsigned char ADC_CHANNEL)
{
    adc_config_t adcConfigStruct;
    adc_sd_config_t adcSdConfigStruct;
    float fresult;
    uint32_t adcConvResult;
	float vref;

    /*** Initial ADC to default configuration.*/
    ADC_GetDefaultConfig(&adcConfigStruct);
    adcConfigStruct.channelEnable = (1U << ADC_CHANNEL);/* Select channel */
    adcConfigStruct.channelConfig = (USER_DEFAULT_ADC_CFG_IDX << ADC_CHANNEL);/* Select configuration */
    adcConfigStruct.triggerSource = USER_DEFAULT_ADC_TRIGGER;//配置触发源/* Software trigger */
    adcConfigStruct.convMode = USER_DEFAULT_ADC_CONVMODE;
	adcConfigStruct.clock = USER_DEFAULT_ADC_CLOCK;
	adcConfigStruct.refSource = USER_DEFAULT_ADC_REFSOURCE;
    ADC_Init(ADC, &adcConfigStruct);

    /* Initial ADC Sigma Delta(SD) configuration */
    ADC_GetSdDefaultConfig(&adcSdConfigStruct);
	adcSdConfigStruct.vinnSelect = USER_DEFAULT_SD_VINNSELECT;
	adcSdConfigStruct.refGain= USER_DEFAULT_SD_REFGAIN;
	adcSdConfigStruct.downSample = SDADC_DOWNSAMPLE;
    ADC_SetSdConfig(ADC, USER_DEFAULT_ADC_CFG_IDX, &adcSdConfigStruct);
	ADC_PgaChopperEnable(ADC, true);

    /* Enable ADC */
    ADC_Enable(ADC, true);
	
	ADC_DoSoftwareTrigger(ADC);
	/* Wait for convert complete */
	while (!(ADC_GetStatusFlags(ADC) & kADC_DataReadyFlag)) {;}			
	/* Get the result */
	adcConvResult = ADC_GetConversionResult(ADC);
		
	#ifdef USE_REF_ADCBANDGAP
		vref = 	g_AdcBandgap;
	#else
		vref = 	LDO_V;
	#endif	

	fresult = ADC_ConversionResult2Mv(ADC, ADC_CHANNEL, USER_DEFAULT_ADC_CFG_IDX, vref, g_AdcVinn,adcConvResult);	
	
	if(fresult<0)
		fresult = 0;
		
	return fresult;
}

/*********************************************************
**ADC启动转换
*********************************************************/


/*********************************************************
**ADC转换关闭
*********************************************************/
void ADC_AllClose(void)
{
    /* Enable ADC */
    ADC_Enable(ADC, false);
	POWER_EnableADC(false);
}

/*********************************************************
**ADC，通过AdcVref1P2Input的电压,得到电池的电压
*********************************************************/
float ADC_Refvolt_Volt(void)
{
	return 0;
}

/*********************************************************
**ADC
*********************************************************/
float ADC_BAT_Volt(void)
{
	float _cur_volt=0; 	
	
	_cur_volt = 4 * ADC_Channel_Result(BAT_DET) /1000;

	return _cur_volt;
}

/*********************************************************
**ADC
*********************************************************/
float ADC_R_Volt(void)
{
	float _cur_volt=0; 
	
	_cur_volt = 2 * ADC_Channel_Result(R_DET)/1000 ;

	return _cur_volt;
}


/*********************************************************
**ADC
*********************************************************/
float ADC_R1_Volt(void)
{
	float _cur_volt=0; 

	_cur_volt = 2 * ADC_Channel_Result(R_DET_1) /1000;
	
	return _cur_volt;
}

/*********************************************************
**ADC
*********************************************************/
float ADC_NTC_Volt(void)
{
	float _cur_volt=0; 

	_cur_volt = ADC_Channel_Result(NTC_DET)/1000;

	return _cur_volt;
}

/*********************************************************
**ADC
*********************************************************/
float ADC_CHANGE_Volt(void)
{
	float _cur_volt=0; 

	_cur_volt = 2.01f * ADC_Channel_Result(CHARGE_DET)/1000;

	return _cur_volt;
}
