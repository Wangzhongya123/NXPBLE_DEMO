#ifndef __Load_user_H
#define	__Load_user_H

#include "pin_mux.h"
#include "fsl_iocon.h"
#include "pin_mux.h"
#include "fsl_iocon.h"
#include "fsl_gpio.h"

#define Resistant_MIN 		1.0f
#define Resistant_MAX		3.0f
#define Resistant_TOOHOT  	3.1f//干烧值
#define R_25				3.0f

#define PORT_LOAD_TEST_IDX 	0
#define LOAD_TEST_GPIO_PIN 	29U
#define PORT_LOAD_PWM_IDX 	0
#define LOAD_PWM_GPIO_PIN 	26U

//测电阻返回
typedef enum _test_res_error
{
	Normal =0,
	Resistant_ShortCircuit =1,
	Resistant_OpenCircuit =2,
	Resistant_TooHot =3,
	Resistant_otherError=4,
}Res_Test_Error;


extern volatile unsigned short int PWM_Duty;//用于产生PWM波

void LoadRESTest_Pin_Init(void);
void LoadRESTest_EN(void);
void LoadRESTest_DIS(void);

void PWMOUT_Pin_Init(void);
void PWMOUT_EN(void);
void PWMOUT_DIS(void);

Res_Test_Error CalculateResistant_Value(float *Res,float *NTC_Value);
void CalculatePWMDuty(float TargerPower,float Resistant_Smoke,float Realtime_BatVolt);
Res_Test_Error CheckLoad(void);


#endif /* __Load_user_H */
