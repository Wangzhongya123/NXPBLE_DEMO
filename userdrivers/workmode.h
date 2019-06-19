#ifndef __WORKMODE_H
#define	__WORKMODE_H

#include "pin_mux.h"
#include "fsl_iocon.h"
#include "pin_mux.h"
#include "fsl_iocon.h"
#include "fsl_gpio.h"

//以下是处的工作模式
typedef enum _workmode
{
	Mode_Idle 		=		0,   //空闲	
	Mode_Query		=		1,   //查询电量
	Mode_Smoke		=		2,   //吸烟模式
	Mode_ChargeIn	=	    3,   //自身充电模式
	Mode_Sleep 		=		4,   //待机
	Mode_ChargeOK 	=		5,   //充电完成
	Mode_Lowv 		=		6,   //自己的电池电量低
	Mode_DisCharge 	=		7,   //断电状态
	Mode_ShortCircuit  =	8,   //短路模式
	Mode_OpenCircuit   =	9,   //开路模式
	Mode_OverTemp	   =    10,	 //发热棒过热
	Mode_Fault_BatTemp =    11,  //电池温度异常
	Mode_BatVoltTooLow =    12,  //电池电压过低
}WORK_MODE;

//工作状态
enum _enable_or_disable
{
	DISABLED=0,
	ENABLED=1,
};

//工作状态
enum _lock_or_unlock
{
	INVALID=0,
	LOCK=1,
	UNLOCK=2
};


//充电返回
typedef enum _charge_error
{
	Charge_Normal ,
	ChargerVolt_too_high,
	ChargerVolt_too_low ,
	Charger_Dislink,	
	BatterVolt_too_high ,
	BatterVolt_too_low  ,
	ChargeTimeout,
	BatterTEMP_too_high,
	BatterTEMP_too_low,
	GoTo_smokemode,
}Charge_ERROR;

//抽烟返回
typedef enum _smokemode_error
{
	Smoke_Normal ,
	Smoke_Res_Short,
	Smoke_Res_Open ,
	Smoke_Res_Error ,
	Smoke_Charger_link,	
	Smoke_BatterVolt_too_low  ,
	SmokeTimeout,
	Smoke_BatterTEMP_too_high,
	Smoke_BatterTEMP_too_low,
	Smoke_BatterTEMP_Error,
	Smoke_DPS310_Error,
	
}Smokemode_ERROR;

//DPS310返回
typedef enum _dps310_respiratory
{
	DPS_smoke_no,
	DPS_smoke_ok,
	
}DPS310_respiratory;


//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <h>充电器电压Configuration
// =======================
//
//   <o>最大充电电压
//   <i> Default: 5.7f
#define ChargeVolt_MAX  5.7f
//
//   <o>最小充电电压
//   <i> Default: 4.2f
#define ChargeVolt_MIN  4.2f
//
//   <o>判定充电器移除
//   <i> Default: 1.0f
#define CHARGERVOLTMIN  1.0f
//
//   <o>充电过程中最大电池电压
//   <i> Default: 4.35f
#define BATTER_Volt_MAX 4.35f
//
//   <o>充电过程中最小电池电压
//   <i> Default: 2.5f
#define BATTER_Volt_MIN 2.5f

// </h>


// <h>DPS310---Configuration
// =======================
//
//   <o>空闲状态下DPS310的灵敏度调整
//   <i> Default: 3
#define DSP_Sensibility 3

// </h>

//------------- <<< end of configuration section >>> -----------------------


extern volatile unsigned int Min_Flag;
extern volatile unsigned char Sec_Flag;
extern volatile unsigned char m20_Sec_Flag;
extern volatile unsigned char Half_Sec_Flag;
extern volatile unsigned int  Smoke_Sec_Time; 

extern volatile unsigned char Query_Flag;
extern volatile unsigned char Smoke_Flag;
extern volatile unsigned char ChargeIn_Flag;
extern volatile unsigned char Sleep_Flag;
extern volatile unsigned char Lock_Unlock_byBLE_Flag;
extern volatile unsigned char ChargeOK_Flag;
extern volatile unsigned char Lowv_Flag;
extern volatile unsigned char DisCharge_Flag;
extern volatile unsigned char Idle_Flag;
extern volatile unsigned char ShortCircuit_Flag;
extern volatile unsigned char OpenCircuit_Flag;
extern volatile unsigned char Smoke_output;//开始输出功率
extern volatile unsigned char Heater_OverTemp_Flag;//发热棒温度过高
extern volatile unsigned char Fault_BatTemp_Flag;
extern volatile unsigned char ON_OFF_Flag;
extern volatile unsigned char SmokeTimeOut_Flag;//抽烟时间超时
extern volatile unsigned char ChargeLink_Flag;
extern volatile unsigned char Power_Lower;//输出前或输出过程中如果低电量，则闪烁
extern volatile unsigned char LowvMode_Worked;//用于标志是否执行过了过放警示，如果=0表示还没有执行过，如果=1则表示执行过
extern volatile unsigned char ChargeOutTime;//用于判定充电是否超时

extern volatile unsigned char Volt_Clas_Overalls;//全局的电压等级标示
extern volatile unsigned char Output_start;//输出过程中，标识有电压输出的阶段
extern volatile unsigned char Power_out_start;//开始输出功率 //用于标识量电池的电压
extern volatile unsigned char Read_ADC_I_DET_Flag;//读电阻窗串口用于标志测电阻的窗口

extern volatile unsigned char LED_Blue_Toggle_Flag;//蓝灯闪烁标志
extern volatile unsigned char LED_Red_Toggle_Flag;//红灯闪烁标志
extern volatile unsigned char LED_Yellow_Toggle_Flag;//黄灯闪烁标志
extern volatile unsigned char LED_Blue_SlowToggle_Flag;//蓝灯慢速闪烁

extern volatile float  ADC_BAT_RealTimeValue;
extern volatile float  ADC_VIN_RealTimeValue;
extern volatile unsigned char Volt_Class;

extern volatile unsigned char toggle;



void Batter_Refresh(void);
void delay_dps310(unsigned int i);
void delay_bybletask(unsigned int i);
WORK_MODE CheckWorkMode(void);
void Mode_Query_Work(void);
Charge_ERROR  Mode_ChargeIn_Work(void);//
Smokemode_ERROR  Mode_Smoke_Work(void);
void QuitSmoke_Work(void);
void Mode_Sleep_Work(void);
void Mode_Lowv_Work(void);
void Mode_Idle_Work(void);
void Mode_ShortCircuit_Work(void);
void Mode_OpenCircuit_Work(void);
void Fault_OverTemp_Work(void);
void Fault_BatTemp_Work(void);
void Fault_BatVoltTooLow_Work(void);
DPS310_respiratory DPS310_Respiratory(void);
void RestartTiming(void);//重新开始计时

void APP_SaveBleReg(void);
void APP_RestoreBleReg(void);

void EnterPowerSleepMode(void);
void ReadyForSleepMode(void);
void WakeUpFormSleepMode(void);

#endif /* __WORKMODE */
