#ifndef __Adc_user_H
#define __Adc_user_H

#include "gpio_pins.h"
#include "fsl_inputmux.h"
#include "fsl_adc.h"
#include "board.h"
#include "fsl_power.h"
#include "fsl_iocon.h"

 /*******************************************************************************
Channel index Register bit Positive input Vin+ Negative input Vin-
0 	CH_SEL[0] 	ADC0/PA00 ADC1/PA01
1 	CH_SEL[1] 	ADC2/PA04 ADC3/PA05
2 	CH_SEL[2] 	ADC4/PA08 ADC5/PA09
3 	CH_SEL[3] 	ADC6/PA10 ADC7/PA11
4 	CH_SEL[4] 	ADC0/PA00 VINN
5 	CH_SEL[5] 	ADC1/PA01 VINN
6 	CH_SEL[6] 	ADC2/PA04 VINN
7 	CH_SEL[7] 	ADC3/PA05 VINN
8 	CH_SEL[8] 	ADC4/PA08 VINN
9 	CH_SEL[9] 	ADC5/PA09 VINN
10 	CH_SEL[10] 	ADC6/PA10 VINN
11 	CH_SEL[11] 	ADC7/PA11 VINN
12 	CH_SEL[12] 	reserved
13 	CH_SEL[13] 	temperature sensor
14 	CH_SEL[14] 	0.25 ? VCC VINN
15 	CH_SEL[15] 	VINN VINN
16~19 reserved 	reserved
20 CH_SEL[20] VINN VSS
21~31 reserved reserved
 ******************************************************************************/

#define ADC0_PA00 	4u
#define ADC1_PA01 	5u
#define ADC2_PA04 	6u
#define ADC3_PA05 	7u
#define ADC4_PA08 	8u
#define ADC5_PA09 	9u
#define ADC6_PA10 	10u
#define ADC7_PA11 	11u

#define BAT_DET 	ADC3_PA05
#define R_DET_1 	ADC2_PA04
#define R_DET 		ADC1_PA01
#define CHARGE_DET 	ADC0_PA00
#define NTC_DET 	ADC5_PA09

#define BAT_DET_PIN 	5U
#define R_DET_1_PIN 	4U
#define R_DET_PIN 		1U
#define CHARGE_DET_PIN 	0U
#define NTC_DET_PIN 	9U

#define EXRef_LDOVolt 2800u

void ADC_Pin_init(void);
void ADC_Configuration(void);
void ADC_ChannelSel(unsigned char channel);
unsigned short int ADC_Conv_Data(void);
void ADC_AllClose(void);

float ADC_Refvolt_Volt(void);

float ADC_BAT_Volt(void);
float ADC_R_Volt(void);
float ADC_R1_Volt(void);
float ADC_NTC_Volt(void);
float ADC_CHANGE_Volt(void);


#endif /* __Adc_user_H */
