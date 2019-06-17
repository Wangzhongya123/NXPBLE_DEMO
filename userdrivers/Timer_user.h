#ifndef __Timer_user_H
#define __Timer_user_H

#include "fsl_ctimer.h"
#include "clock_config.h"
#include "pin_mux.h"
#include "fsl_iocon.h"
#include "pin_mux.h"
#include "fsl_iocon.h"
#include "fsl_gpio.h"

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------

// <h>系统频率---Configuration
// =======================
//
//   <o>系统的频率
//   <i> Default: 50
	#define SYS_FREQUENCY 50

// </h>
//------------- <<< end of configuration section >>> -----------------------


extern volatile unsigned int Min_Time;
extern volatile unsigned char Sec_Time;
extern volatile unsigned int _Time;
extern volatile unsigned int delaytobletask;//最小时间段

extern volatile unsigned int Min_Flag;
extern volatile unsigned char Sec_Flag;
extern volatile unsigned char m20_Sec_Flag;
extern volatile unsigned char Charge_Sec_Flag;
extern volatile unsigned char Half_Sec_Flag;


#define TEST_PIN_IDX  	14U

#define PIN_text_1_IDX                       8u   /*!< Pin number for pin 10 in a port */
#define PIN_text_2_IDX                       14u   /*!< Pin number for pin 14 in a port */

void TEST_LED_Pin_Init(void);

void Time1_Init(unsigned int freq);
void Time2_Init(unsigned int freq);
void Time3_Init(unsigned int freq);

#endif /* __Timer_user_H */
