#ifndef __LED_user_H
#define	__LED_user_H

#include "fsl_iocon.h"
#include "pin_mux.h"
#include "fsl_inputmux.h"
#include "fsl_common.h"
#include "gpio_pins.h"
#include "QN908XC.h"
#include "fsl_gpio.h"
#include "fsl_sctimer.h"

#define BUS_CLK_FREQ CLOCK_GetFreq(kCLOCK_ApbClk)


////////////LED灯的输出引脚配置
#define R_PIN_IDX  		19U
#define G_PIN_IDX  		15U
#define B_PIN_IDX  		18U

#define LED_Red		R_PIN_IDX
#define LED_Blue	B_PIN_IDX
#define LED_Green	G_PIN_IDX

/////////////sctimer的输出通道定义
#define LED_RED_CH 		kSCTIMER_Out_2	
#define LED_GREEN_CH 	kSCTIMER_Out_0
#define LED_BLUE_CH 	kSCTIMER_Out_3

typedef struct LED_RGB_config
{
	unsigned int LED_RED_Duty;
	unsigned int LED_GREEN_Duty;		
	unsigned int LED_BLUE_Duty;	
} led_rgb_set;

#define	No_LED_Flicker 		0
#define	RED_LED_Flicker  	1
#define	GREEN_LED_Flicker	2
#define	BLUE_LED_Flicker	3
#define	YELLOW_LED_Flicker	4
#define	WHITE_LED_Flicker	5

void RGB_PWM_init(void);
void PWM_DutyPercet_Change(led_rgb_set led_color);

void LED_Red_PWM_On(void);
void LED_Blue_PWM_On(void);
void LED_Green_PWM_On(void);
void LED_Yellow_PWM_On(void);
void LED_All_PWM_On(void);
void LED_All_PWM_Off(void);
void LED_White_Breath(unsigned char duty);
void LED_Breath(led_rgb_set LED_DUTY);

void LED_Pin_Init(void);

void LED_Red_On(void);
void LED_Blue_On(void);
void LED_Green_On(void);
void LED_Yellow_On(void);
void LED_All_On(void);

void LED_Red_Off(void);
void LED_Blue_Off(void);
void LED_Green_Off(void);
void LED_Yellow_Off(void);
void LED_All_Off(void);
void LED_Close(void);

void LED_Flicker(unsigned char led_colour,unsigned char num);

#endif /* __LED_user_H */
