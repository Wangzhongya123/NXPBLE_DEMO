#ifndef __WORKMODE_H
#define	__WORKMODE_H

#include "pin_mux.h"
#include "fsl_iocon.h"
#include "pin_mux.h"
#include "fsl_iocon.h"
#include "fsl_gpio.h"

//�����Ǵ��Ĺ���ģʽ
typedef enum _workmode
{
	Mode_Idle 		=		0,   //����	
	Mode_Query		=		1,   //��ѯ����
	Mode_Smoke		=		2,   //����ģʽ
	Mode_ChargeIn	=	    3,   //������ģʽ
	Mode_Sleep 		=		4,   //����
	Mode_ChargeOK 	=		5,   //������
	Mode_Lowv 		=		6,   //�Լ��ĵ�ص�����
	Mode_DisCharge 	=		7,   //�ϵ�״̬
	Mode_ShortCircuit  =	8,   //��·ģʽ
	Mode_OpenCircuit   =	9,   //��·ģʽ
	Mode_OverTemp	   =    10,	 //���Ȱ�����
	Mode_Fault_BatTemp =    11,  //����¶��쳣
	Mode_BatVoltTooLow =    12,  //��ص�ѹ����
}WORK_MODE;

//����״̬
enum _enable_or_disable
{
	DISABLED=0,
	ENABLED=1,
};

//����״̬
enum _lock_or_unlock
{
	INVALID=0,
	LOCK=1,
	UNLOCK=2
};


//��緵��
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

//���̷���
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

//DPS310����
typedef enum _dps310_respiratory
{
	DPS_smoke_no,
	DPS_smoke_ok,
	
}DPS310_respiratory;


//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <h>�������ѹConfiguration
// =======================
//
//   <o>������ѹ
//   <i> Default: 5.7f
#define ChargeVolt_MAX  5.7f
//
//   <o>��С����ѹ
//   <i> Default: 4.2f
#define ChargeVolt_MIN  4.2f
//
//   <o>�ж�������Ƴ�
//   <i> Default: 1.0f
#define CHARGERVOLTMIN  1.0f
//
//   <o>������������ص�ѹ
//   <i> Default: 4.35f
#define BATTER_Volt_MAX 4.35f
//
//   <o>����������С��ص�ѹ
//   <i> Default: 2.5f
#define BATTER_Volt_MIN 2.5f

// </h>


// <h>DPS310---Configuration
// =======================
//
//   <o>����״̬��DPS310�������ȵ���
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
extern volatile unsigned char Smoke_output;//��ʼ�������
extern volatile unsigned char Heater_OverTemp_Flag;//���Ȱ��¶ȹ���
extern volatile unsigned char Fault_BatTemp_Flag;
extern volatile unsigned char ON_OFF_Flag;
extern volatile unsigned char SmokeTimeOut_Flag;//����ʱ�䳬ʱ
extern volatile unsigned char ChargeLink_Flag;
extern volatile unsigned char Power_Lower;//���ǰ���������������͵���������˸
extern volatile unsigned char LowvMode_Worked;//���ڱ�־�Ƿ�ִ�й��˹��ž�ʾ�����=0��ʾ��û��ִ�й������=1���ʾִ�й�
extern volatile unsigned char ChargeOutTime;//�����ж�����Ƿ�ʱ

extern volatile unsigned char Volt_Clas_Overalls;//ȫ�ֵĵ�ѹ�ȼ���ʾ
extern volatile unsigned char Output_start;//��������У���ʶ�е�ѹ����Ľ׶�
extern volatile unsigned char Power_out_start;//��ʼ������� //���ڱ�ʶ����صĵ�ѹ
extern volatile unsigned char Read_ADC_I_DET_Flag;//�����贰���ڬ����ڱ�־�����Ĵ���

extern volatile unsigned char LED_Blue_Toggle_Flag;//������˸��־
extern volatile unsigned char LED_Red_Toggle_Flag;//�����˸��־
extern volatile unsigned char LED_Yellow_Toggle_Flag;//�Ƶ���˸��־
extern volatile unsigned char LED_Blue_SlowToggle_Flag;//����������˸

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
void RestartTiming(void);//���¿�ʼ��ʱ

void APP_SaveBleReg(void);
void APP_RestoreBleReg(void);

void EnterPowerSleepMode(void);
void ReadyForSleepMode(void);
void WakeUpFormSleepMode(void);

#endif /* __WORKMODE */
