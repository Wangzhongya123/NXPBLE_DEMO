#ifndef __Key_user_H
#define	__Key_user_H

#include "fsl_inputmux.h"
#include "fsl_pint.h"
#include "fsl_gpio.h"
#include "fsl_iocon.h"

typedef enum key_value
{
	Key_Invalid =	0,
	Key_1_Time  =	1,
	Key_3_Time  =	3,
	Key_5_Time  =	5,
	Key_8_Time  =	8,
	Key_10_Time =	10,
	Key_LongTime= 	100,
	
}Key_Value;

#define USER_PINT_INT0_SRC kINPUTMUX_GpioPort0Pin11ToPintsel
#define USER_PINT_INT1_SRC kINPUTMUX_GpioPort0Pin0ToPintsel

#define USER_SW1_GPIO_PIN 	11U
#define PORTKEY_IDX		  	0U
#define KEY_PORT		  	GPIOA

extern Key_Value KeyValue;      // 键按下的处理结果

extern volatile unsigned int Key_HodeOnTime; // 键按下的时间	
extern volatile unsigned int Key_RleaseTime; // 按键释放的时间
extern volatile unsigned char Key_DownCount; // 键按下的次数
extern volatile unsigned char Key_Fall_Flag;//生成按键按下标志
extern volatile unsigned int  Key_RleaseTime_onetime;  // 按键释放的时间
extern volatile unsigned char  FuncKey_time;

void Key_Init(void);
Key_Value User_KeyScan(void);

void PINT_INIT(void);

#endif /* __Key_user_H */
