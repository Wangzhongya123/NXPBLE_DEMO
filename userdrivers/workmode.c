/********************								 (C) COPYRIGHT 					  ********************
 * �ļ���  ��workmode.c
 * ����    ��         
 * ����    ��workmode
 * ����    ��
 ntc    3450 10k
**********************************************************************************/
#include <string.h>
#include "workmode.h"
#include "Timer_user.h"
#include "Adc_user.h"
#include "Key_user.h"
#include "dps310.h"
#include "IIC_user.h"
#include "Load_user.h"
#include "BatCharge_user.h"
#include "LED_user.h"
#include "userapp.h"

#include "fsl_power.h"
#include "Keyboard.h"

#include "fsl_os_abstraction.h"
#include "fsl_os_abstraction_bm.h"


#ifdef DEBUG_PRINT
	
#endif

//workmode flag/////////////////////////////////////////
volatile unsigned char Query_Flag=0;
volatile unsigned char Smoke_Flag=0;
volatile unsigned char Smokemode_start=0;
volatile unsigned char ChargeIn_Flag=0;
volatile unsigned char Sleep_Flag=0;
volatile unsigned char Lock_Unlock_byBLE_Flag=0;
volatile unsigned char ChargeOK_Flag=0;
volatile unsigned char Lowv_Flag=0;
volatile unsigned char DisCharge_Flag=0;
volatile unsigned char Idle_Flag=0;
volatile unsigned char ShortCircuit_Flag=0;
volatile unsigned char OpenCircuit_Flag=0;
volatile unsigned char Read_ADC_I_DET_Flag=0;//�����贰��
volatile unsigned char Fault_BatTemp_Flag=0;//��ص��¶ȹ���
volatile unsigned char Heater_OverTemp_Flag=0;//�����¶ȹ���
volatile unsigned char Smoke_output=0;//��ʼ�������
volatile unsigned char ON_OFF_Flag = 1;//���ػ���־,1��ʾ�����ǿ���״̬��0��ʾ�����ڹػ�
volatile unsigned char OnOffPowerByKey=0;
volatile unsigned char WakeupBykey=0;
volatile unsigned char SmokeTimeOut_Flag=0;
volatile unsigned char ChargeLink_Flag=0;
volatile unsigned char BatVoltTooLow_Flag=0;//��ص�ѹ̫�ͣ��޷�ʹ��
volatile unsigned char ChargeVoltTooLow_Flag=0;//�������ѹ̫�ͣ��޷�ʹ��
volatile unsigned char ChargeVoltTooHigh_Flag=0;//�������ѹ̫�ߣ��޷�ʹ��
volatile unsigned char Smoke_battemptoohigh_Flag=0;//ʹ�ù��̵���¶ȹ���

volatile unsigned char Power_out_start=0;//��ʼ�������  //���ڱ�ʶ����صĵ�ѹ
volatile unsigned char Output_start=0;//��������У����ڱ�־�����Ĵ���
static volatile unsigned short int ResR_EDT_i[2]={0};//���������R��DET�ϵĵ�ѹֵ

volatile unsigned char LED_Blue_Toggle_Flag=0;//������˸��־
volatile unsigned char LED_Red_Toggle_Flag=0;//�����˸��־
volatile unsigned char LED_Yellow_Toggle_Flag=0;//�Ƶ���˸��־
volatile unsigned char LED_Blue_SlowToggle_Flag=0;//����������˸

volatile unsigned int  Smoke_Sec_Time=0; //ÿ�γ���ʱ��
volatile unsigned char ChargeOutTime=0;

volatile unsigned char LowvMode_Worked=0;//���ڱ�־�Ƿ�ִ�й��˹��ž�ʾ�����=0��ʾ��û��ִ�й������=1���ʾִ�й�
volatile unsigned char DisChargeMode_Worked=0;//û��ִ�й��ϵ�����
volatile unsigned char ChargeOKMode_Worked=0;//û��ִ�й�����������
volatile unsigned char BattempFaultMode_Worked=0;//û��ִ�й�����������

volatile unsigned char Power_Lower=0;
volatile unsigned char Volt_Class=0;//ȫ�ֵ����ȼ���ʾ
volatile unsigned char Volt_Clas_Overalls=0;//ȫ�ֵĵ�ѹ�ȼ���ʾ --��������ʹ��
volatile unsigned char Volt_Class_Fresh=1;

float u32AdcResultAcc = 0;
volatile float ADC_BAT_RealTimeValue=0; //ȫ�ֵ�ص�ѹ
volatile float ADC_VIN_RealTimeValue=0; //ȫ�ֳ������ѹ

//dps310 ---data /////////////////////////////////////////////////////////////
unsigned char test_time=0;//���ڼ���20ms�Ĵ�������־����������
extern struct dps310_state drv_state;
extern dps310_bus_connection cnn;
extern double pressure; 
extern double temperature;
extern int ret;
static volatile float pressure_i[10]={0};
volatile unsigned char DPS310_Respiratory_Flag=0;//ȫ����
volatile float DPS310_CurrentData=0;//dps310ʵʱֵ
volatile float DPS310_Standard=0;//dps310��׼ֵ

extern void ble_task(void);//ִ��������Ϣ���м��

extern void USER_SendDateToAir(void *pData);
extern void USER_Serial_Printf(void * pParam);//modify by wzy
extern void BleApp_Start(void);
extern void BleApp_Init(void);
extern osaStatus_t OSA_Init(void);
extern void hardware_init(void);
extern void OSA_TimeInit(void);
extern void main_task(uint32_t param);
extern void ble_task_init(void);
extern void BOARD_InitPins(void);

char str[20]={0};//��ʱʹ��

/**********************************************/
/* �������ܣ� ʹ����Ϣ���м��ķ���������ʱ  */
/* ��ڲ�����                    		      */
/**********************************************/
void delay_dps310(unsigned int i)
{
	unsigned int k=0;
	
	RestartTiming();

	for(k=0;k<i;k++)
		ble_task();
}

/**********************************************/
/* �������ܣ� ʹ����Ϣ���м��ķ���������ʱ  */
/* ��ڲ����� ms                      		  */
/**********************************************/
void delay_bybletask(unsigned int i)
{
	unsigned int k=0;
	
	//RestartTiming();
	delaytobletask = 0;
	if(i<=20u) i=20u;
	k = i / 20u;//ϵͳ������
	
	while(delaytobletask < k)
		ble_task();
}

/**********************************************/
/* �������ܣ������飬����������״̬         */
/* ��ڲ�������                               */
/**********************************************/
WORK_MODE CheckWorkMode(void)
{	
	ble_task();
	
	ADC_BAT_RealTimeValue =  ADC_BAT_Volt();
	ADC_VIN_RealTimeValue =  ADC_CHANGE_Volt();
	
	if(ShortCircuit_Flag == ENABLED)	return Mode_ShortCircuit;
			
	if(OpenCircuit_Flag == ENABLED)		return Mode_OpenCircuit;	
		
	if(Heater_OverTemp_Flag == ENABLED) return Mode_OverTemp;//�¶ȹ��ߣ���ӦΪ��ֵ���󣬻��Ǵ��ڸ��յĴ���ֵ
	
	if(ADC_VIN_RealTimeValue >= CHARGERVOLTMIN)//������С���ӵ�ѹ���г��������
	{
		ChargeLink_Flag = ENABLED;
		if(ADC_BAT_RealTimeValue <=2.5f)
			BatVoltTooLow_Flag=ENABLED;
	}	
	else
	{
		ChargeLink_Flag = DISABLED;
		ChargeVoltTooLow_Flag = DISABLED;
		ChargeVoltTooHigh_Flag = DISABLED;
		ChargeOutTime = DISABLED;//�Ѿ��Ͽ��˳����
		Fault_BatTemp_Flag = DISABLED;		
		BattempFaultMode_Worked = DISABLED;
		ChargeOK_Flag = DISABLED;
	}	
	
	if((Lowv_Flag == ENABLED)&&(LowvMode_Worked==0))//
		return Mode_Lowv;
	
	if(((Fault_BatTemp_Flag==ENABLED)&&(BattempFaultMode_Worked==0))||(Smoke_battemptoohigh_Flag==ENABLED))	
		return Mode_Fault_BatTemp;
	
	if(BatVoltTooLow_Flag==ENABLED)	return Mode_BatVoltTooLow;
	 
	if((ChargeLink_Flag==1)&&(ChargeOK_Flag==0)&&(ChargeOutTime==0)&&(Fault_BatTemp_Flag==0)&&(Smokemode_start==0)&&
		(BatVoltTooLow_Flag==0)&&(ChargeVoltTooLow_Flag==0)&&(ChargeVoltTooHigh_Flag==0) )
	{	
		ChargeLink_Flag=0;	
		ADC_VIN_RealTimeValue =  ADC_CHANGE_Volt();
		
		if(ADC_VIN_RealTimeValue >= CHARGERVOLTMIN)
		{
			ChargeIn_Flag =  ENABLED;
			Query_Flag    =  DISABLED;
			Smoke_Flag	  =  DISABLED;	
			return Mode_ChargeIn;
		}		
		else
			ChargeIn_Flag = DISABLED;
	}

	if(ON_OFF_Flag == DISABLED)//û�д�˯��״̬���ѣ������ڹػ�ģʽ�£�
	{
		if(KeyValue==Key_LongTime )
		{
			KeyValue = Key_Invalid;
			ON_OFF_Flag = ENABLED;//������־λ
			Query_Flag  = ENABLED;
			WakeupBykey = ENABLED;//ʹ�ð�������
			
			BOARD_InitPins();	
			BleApp_Init();
			
			BleApp_Start();
			
			return Mode_Query;
		}	
		
		if( GPIO_ReadPinInput(KEY_PORT, USER_SW1_GPIO_PIN) == 0)
			RestartTiming();//���¼�ʱ	
		
		if(Sec_Time>=5)
		{
			OnOffPowerByKey = DISABLED;
			return 	Mode_Sleep;				
		}
		
		if(Lock_Unlock_byBLE_Flag == UNLOCK)//ʹ���������н���
		{
			Lock_Unlock_byBLE_Flag = INVALID;
			
			ON_OFF_Flag = ENABLED;//������־λ
			Query_Flag  = ENABLED;
			WakeupBykey = ENABLED;//ʹ�ð�������
			RestartTiming();//���¼�ʱ	
			
			return Mode_Query;		
		}
		
		if(SmokeTimeOut_Flag == ENABLED)
		{
			ON_OFF_Flag = DISABLED;//����˯��ģʽ���ػ�״̬
			OnOffPowerByKey = DISABLED;
			SmokeTimeOut_Flag = DISABLED;			
			
			return Mode_Sleep;
		}
	}
	else//����״̬��
	{	
		if(KeyValue == Key_1_Time)//��ѯ
		{
			return Mode_Query;
		}			
		else if(Smokemode_start == ENABLED)//�������״̬,�ӳ��״̬�½������ģʽ
		{
			return Mode_Smoke;
		}
		else if(KeyValue == Key_LongTime)								
		{
			KeyValue = Key_Invalid;
			OnOffPowerByKey =ENABLED;//�ð����ķ�ʽ�ػ�
			ON_OFF_Flag = DISABLED ;//����ػ�״̬	
			
			return Mode_Sleep;
		}	
		else;	
		
		if(Lock_Unlock_byBLE_Flag == LOCK)//ʹ��������������
		{
			Lock_Unlock_byBLE_Flag = INVALID;
			OnOffPowerByKey =ENABLED;//�ð����ķ�ʽ�ػ�
			ON_OFF_Flag = DISABLED ;//����ػ�״̬	
			
			return Mode_Sleep;		
		}

		if(m20_Sec_Flag==1)
		{
			m20_Sec_Flag=0;	
			
			if(test_time >= DSP_Sensibility)//����������
			{
				test_time=0;
				DPS310_Respiratory_Flag = DPS310_Respiratory();
				if(DPS310_Respiratory_Flag == DPS_smoke_ok)
				{
					DPS310_Respiratory_Flag = DPS_smoke_no ;
					return Mode_Smoke;
				}			
			}
			else
				test_time++;
		}
		
		if(((Idle_Flag==ENABLED)&&(Sec_Time>=1)&&(Min_Time==10))||(Sleep_Flag==ENABLED)||(SmokeTimeOut_Flag==ENABLED))
		{
			ON_OFF_Flag=DISABLED;//����˯��ģʽ���ػ�״̬
			
			if(SmokeTimeOut_Flag == ENABLED)
				OnOffPowerByKey = DISABLED;
			else
				OnOffPowerByKey = ENABLED;
			
			SmokeTimeOut_Flag=DISABLED;			
			
			return Mode_Sleep;
		}
		
		if(Sec_Flag == 1)
		{
			Res_Test_Error error_code;
			float res=0,ntc_res=0;		
			
			Sec_Flag = 0;

			LoadRESTest_EN();	
			delay_bybletask(20);
			error_code = CalculateResistant_Value(&res,&ntc_res);

			memset(str,'\0',15);
			memset(Send_Str,'\0',SENDDATA_NUMBER);
			sprintf(str,"%.2fohm %.2fK %.2fV\r\n",res,ntc_res,ADC_BAT_RealTimeValue);	
			sprintf(Send_Str,"%.2fohm %.2fK %.2fV",res,ntc_res,ADC_BAT_RealTimeValue);	

			USER_Serial_Printf(str);
			USER_SendDateToAir((char *)Send_Str);	
			
			ret = dps310_get_processed_data(&drv_state,&pressure,&temperature);
			memset(DPS310_Send_Str,'\0',SENDDATA_NUMBER);
			sprintf(DPS310_Send_Str,"%.3fhPa %.2fC", (float)pressure,(float)temperature);	
			DPS310SentDataToAir((char *)DPS310_Send_Str);		
		}

		return Mode_Idle;
	}
	return Mode_Idle;
}

/**********************************************/
/* �������ܣ�������Դ�ĵ�ѹ				      */
/* ��ڲ�������                               */
/**********************************************/
void Batter_Refresh(void)
{
	float _BAT_RealTimeValue=0;
	
	_BAT_RealTimeValue = ADC_BAT_Volt();

	if(_BAT_RealTimeValue>=3.74f)
		Volt_Class=4;	
	else if((_BAT_RealTimeValue>3.62f)&&(_BAT_RealTimeValue<=3.74f))
		Volt_Class=3;
	else if((_BAT_RealTimeValue>3.4f)&&(_BAT_RealTimeValue<=3.62f))
		Volt_Class=2;
	else if((_BAT_RealTimeValue>3.2f)&&(_BAT_RealTimeValue<=3.4f))
		Volt_Class=1;		
	else if(_BAT_RealTimeValue<3.2f)	
		Volt_Class=0;
	else;
}

/**********************************************/
/* �������ܣ�������ѯ����			          */
/* ��ڲ�������                               */
/**********************************************/
void Mode_Query_Work(void)
{
	unsigned char ledopentime=0;
	float realtime_VIN_volt=0;
	int ret=0;
	unsigned char t=0;//���ڵ���������ʹ��
	
	#ifdef DEBUG_PRINT
		
	#endif	
	
	Query_Flag=1;
	RestartTiming();
	ble_task();
	
	if(WakeupBykey==1)
		ledopentime=3;
	else
		ledopentime=2;
	
	Batter_Refresh();
	
	if(Volt_Class_Fresh==1)
	{
		Volt_Class_Fresh = 0;
		Volt_Clas_Overalls = Volt_Class ;//ȫ�ֵĵ����ȼ�
	}
	else
	{
		if( Volt_Class > Volt_Clas_Overalls )	
			Volt_Class = Volt_Clas_Overalls;
		else if( Volt_Class < Volt_Clas_Overalls )	
			Volt_Clas_Overalls = Volt_Class ;	
		else
			Volt_Class = Volt_Clas_Overalls;	
	}
		
	LED_All_Off();	
	
	switch(Volt_Class)
	{
		case 4:{LED_Green_On();}break;
		case 3:{LED_Yellow_On();}break;
		case 2:{LED_Red_On();}break;
		case 1:{LED_Red_On();}break;	
		case 0:{LED_Red_On();}break;	
		default:Query_Flag=0;;break;
	}

	#ifdef DEBUG_PRINT
		
	#endif		
	
	//��310���������ض�
	{
		double pressure_sum=0;
		float pressure_i[10]={0};
		unsigned char i=0,j=0,k=0,l=0;
		float press_temp=0;
		
		for(i=0;i<10;i++)
		{
			do
			{
				ble_task();
				ret = dps310_get_processed_data (&drv_state,&pressure,&temperature);//pressure��temperature��ȫ�ֱ���
			}	
			while(ret<0);
			
			pressure_i[i] = (float)pressure;
			delay_bybletask(20);
		}
		
		for(j=0;j<10-1;j++)//n�����������ܹ�����n-1��ɨ��
		{
			for(k=0;k<10-j-1;k++)
			{
				if(pressure_i[k]>pressure_i[k+1])//������������
				{
				   press_temp = pressure_i[k+1];
				   pressure_i[k+1] = pressure_i[k];
				   pressure_i[k] = press_temp;
				}
			}
		}

		for(l=1;l<9;l++)
			pressure_sum  +=  pressure_i[l];		

		DPS310_Standard = pressure_sum/8;
	}
	
	#ifdef DEBUG_PRINT
		//printf("DPS310У׼��� ,��׼ֵΪ: %d Pa\r\n ",(unsigned int)(DPS310_Standard*100));//����ʹ��	
	#endif	

	while(Query_Flag == ENABLED)
	{
		ble_task();//���������Ϣ���м��
		
		if(Sec_Time >= ledopentime)		
			Query_Flag = DISABLED;

		if(KeyValue == Key_1_Time )
		{
			KeyValue = Key_Invalid;
			Query_Flag = ENABLED;
			RestartTiming();
		}
		else if(KeyValue == Key_LongTime )
			Query_Flag = DISABLED;
		else;
		
		if(m20_Sec_Flag==1)
		{
			m20_Sec_Flag=0;
			
			if(t >= DSP_Sensibility)
			{
				t=0;
				DPS310_Respiratory_Flag = DPS310_Respiratory();
				if(DPS310_Respiratory_Flag == DPS_smoke_ok)
				{
					DPS310_Respiratory_Flag = DPS_smoke_no;
					Query_Flag = DISABLED;
					Smokemode_start = ENABLED;
				}			
			}
			else
				t++;					//����������
		}
		
		realtime_VIN_volt =  ADC_CHANGE_Volt();
		if(realtime_VIN_volt >= CHARGERVOLTMIN )
		{
			ChargeLink_Flag = ENABLED;
			
			if((ChargeLink_Flag == ENABLED)&&(ChargeOK_Flag==0)&&(ChargeOutTime==0)&&(Fault_BatTemp_Flag==0)&&
				(Smokemode_start==0)&&(BatVoltTooLow_Flag==0)&&(ChargeVoltTooLow_Flag==0)&&(ChargeVoltTooHigh_Flag==0))
				Query_Flag = DISABLED;
		}
	}
	LED_All_Off();//	
	WakeupBykey=0;
	RestartTiming();
}

/**********************************************/
/* �������ܣ�����������				      */
/* ��ڲ�������                               */
/**********************************************/
Charge_ERROR Mode_ChargeIn_Work(void)
{	
	float _BAT_RealTimeValue=0;
	float _VIN_RealTimeValue=0;
	unsigned char _charge_en=1;	
	unsigned char volt_class=0;//��ѹ�ȼ�
	unsigned char volt_class_Overall=0;
	unsigned char led_red=0;
	unsigned char led_green=0;
	unsigned char led_red_toggle=0;	
	unsigned char ntc_class=0;
	unsigned char charge_voltclass=4;//�������±�־
	float res_ntc =0;
	unsigned char bat_temp_toohot=0;
	unsigned char dps_test_time=0;
	unsigned char chargefull_time=0;

	ChargeOK_Flag=0;
	Charge_DIS();//���ʹ�ܹر�
	
	delay_bybletask(100);

	_VIN_RealTimeValue = ADC_CHANGE_Volt();
	
	if(_VIN_RealTimeValue >= ChargeVolt_MAX)//�����ѹ���������޶���ѹ���쳣���
	{
		ChargeIn_Flag =DISABLED;	
		ChargeOutTime = DISABLED;//�Ѿ��Ͽ��˳����
		LED_All_Off();				
		ChargeVoltTooHigh_Flag = ENABLED;//�������ѹ�쳣			
		Volt_Class_Fresh = ENABLED;
		
		#ifdef DEBUG_PRINT
			//printf("\r\n�������ѹ����");
		#endif	

		Charge_DIS();//���ʹ�ܹر�
		LED_Flicker(RED_LED_Flicker ,6);
		RestartTiming();//���¼�ʱ
		
		return ChargerVolt_too_high;
	}				
	else if((_VIN_RealTimeValue > CHARGERVOLTMIN)&&(_VIN_RealTimeValue <= ChargeVolt_MIN))
	{
		ChargeIn_Flag = DISABLED;	
		ChargeOutTime = DISABLED;//�Ѿ��Ͽ��˳����
		LED_All_Off();				
		
		#ifdef DEBUG_PRINT
			//printf("\r\n �������ѹ����");
		#endif	

		Charge_DIS();//���ʹ�ܹر�
		LED_Flicker(RED_LED_Flicker ,3);
		
		RestartTiming();//���¼�ʱ	
		Volt_Class_Fresh = ENABLED;
		ChargeVoltTooLow_Flag = ENABLED;//				
	
		return ChargerVolt_too_low;
	}
	else;
	
	_BAT_RealTimeValue = ADC_BAT_Volt() ;	 
	
	if(_BAT_RealTimeValue>=4.05f)
		volt_class=2;	
	else if((_BAT_RealTimeValue>3.85f)&&(_BAT_RealTimeValue<=4.05f))
		volt_class=1;	
	else if(_BAT_RealTimeValue<=3.85f)	
		volt_class=0;
	else;
	
	volt_class_Overall = volt_class ;
	
	LED_All_Off();	
	
	if(volt_class==2)//�̵Ƴ���
	{
		LED_Green_On();LED_Red_Off();LED_Blue_Off();
	}				
	else if(volt_class==1)//��Ƴ���
	{
		LED_Red_On();LED_Blue_Off();LED_Green_Off();
	}
	else if(volt_class==0)//�����----�ĳɺ�Ƴ���
	{
		LED_Red_On();LED_Blue_Off();LED_Green_Off();
	}
	else;
	
	#ifdef DEBUG_PRINT
		//printf("\r\nVIN- %d",(unsigned int)(_VIN_RealTimeValue*1000));
	#endif	

	Charge_EN();//���ʹ�ܴ�
	delay_bybletask(100);
	
	RestartTiming();//���¼�ʱ
	
	Min_Flag = 1;
	Sec_Flag = 1;
	m20_Sec_Flag = 0;
	Half_Sec_Flag = 1;
	ChargeIn_Flag = ENABLED;
	ChargeOK_Flag = DISABLED;
	
	while(ChargeIn_Flag == ENABLED)//��ʼ���г��
	{		
		ble_task();
		
		if((m20_Sec_Flag==1)&&(ON_OFF_Flag==1))
		{
			m20_Sec_Flag=0;
			if(dps_test_time >= DSP_Sensibility)
			{
				dps_test_time=0;
				DPS310_Respiratory_Flag = DPS310_Respiratory();
				if(DPS310_Respiratory_Flag == DPS_smoke_ok)
				{
					DPS310_Respiratory_Flag = DPS_smoke_no;
					Smokemode_start = ENABLED;
					ChargeIn_Flag = DISABLED;
					
					Charge_DIS();//���ʹ�ܹر�
					LED_All_Off();
					RestartTiming();//���¼�ʱ				
					
					return GoTo_smokemode;	
				}			
			}
			else
				dps_test_time++;					//����������
		}
		
		if(charge_voltclass >= 2)//ÿһ���ӽ���һ�ηּ�
		{
			charge_voltclass = 0;
			
			_BAT_RealTimeValue = ADC_BAT_Volt() ;	 
			
			if(_BAT_RealTimeValue>=4.05f)
				volt_class=2;	
			else if((_BAT_RealTimeValue>3.85f)&&(_BAT_RealTimeValue<=4.05f))
				volt_class=1;	
			else if(_BAT_RealTimeValue<=3.85f)	
				volt_class=0;
			else;

			if( volt_class > volt_class_Overall )	
				volt_class_Overall = volt_class ;	
			else if( volt_class < volt_class_Overall )	
				volt_class = volt_class_Overall ;
			else
				volt_class = volt_class_Overall;
		}
		
		if(Half_Sec_Flag==1)
		{
			Half_Sec_Flag=0;
			charge_voltclass ++ ;
			
			_BAT_RealTimeValue = ADC_BAT_Volt();
			delay_bybletask(20);
			_VIN_RealTimeValue = ADC_CHANGE_Volt();
			delay_bybletask(20);
			
			#ifdef DEBUG_PRINT
				//printf("\r\n BAT- %d ,VIN- %d, V_class- %d",(unsigned int)(_BAT_RealTimeValue*1000),(unsigned int)(_VIN_RealTimeValue*1000),volt_class);
			#endif			

			if(_VIN_RealTimeValue >= ChargeVolt_MAX)//�����ѹ���������޶���ѹ���쳣���
			{
				_charge_en=0;
				ChargeIn_Flag = DISABLED;	
				ChargeOutTime = DISABLED;//�Ѿ��Ͽ��˳����
				LED_All_Off();				
				ChargeVoltTooHigh_Flag = ENABLED;//�������ѹ�쳣			
				Volt_Class_Fresh = ENABLED;//����ˢ�µ�����־
				
				#ifdef DEBUG_PRINT
					//printf("\r\n�������ѹ����");
				#endif	

				Charge_DIS();//���ʹ�ܹر�
				CurrentChoice_Pin_Init();//���������ƽų�ʼ��
				LED_Flicker(RED_LED_Flicker ,6);
				RestartTiming();//���¼�ʱ

				Batter_Refresh();
				
				return ChargerVolt_too_high;
			}
			else if(_VIN_RealTimeValue<= ChargeVolt_MIN)//С����С�ĵ�ѹ
			{
				CurrentChoice_Pin_Init();//���������ƽų�ʼ��
				delay_bybletask(500);//�ȴ���ѹ�ȶ��ٽ��в���
				_VIN_RealTimeValue =  ADC_CHANGE_Volt();
				
				if(_VIN_RealTimeValue <= CHARGERVOLTMIN)
				{
					_charge_en=0;
					ChargeIn_Flag = DISABLED;	
					ChargeOutTime = DISABLED;//�Ѿ��Ͽ��˳����
					LED_All_Off();
					Volt_Class_Fresh = ENABLED;//ˢ�µ�����־
					
					#ifdef DEBUG_PRINT
						//printf("\r\n�Ƴ������");
					#else
						delay_bybletask(100);//�ȴ���ѹ�ȶ��ٽ��в���
					#endif
						
					Charge_DIS();//���ʹ�ܹر�
					RestartTiming();//���¼�ʱ	
					
					Batter_Refresh();
					
					return Charger_Dislink;
				}
				else if((_VIN_RealTimeValue > CHARGERVOLTMIN)&&(_VIN_RealTimeValue <= ChargeVolt_MIN))
				{
					_charge_en=0;
					ChargeIn_Flag = DISABLED;	
					ChargeOutTime = DISABLED;//�Ѿ��Ͽ��˳����
					LED_All_Off();				
					
					#ifdef DEBUG_PRINT
						//printf("\r\n �������ѹ����");
					#endif	

					Charge_DIS();//���ʹ�ܹر�
					LED_Flicker(RED_LED_Flicker ,3);
					
					RestartTiming();//���¼�ʱ	
					Volt_Class_Fresh = ENABLED;
					ChargeVoltTooLow_Flag = ENABLED;//
					
					Batter_Refresh();				
				
					return ChargerVolt_too_low;
				}
				else;
			}
			else if(_BAT_RealTimeValue >= BATTER_Volt_MAX)//������״̬	
			{	
				delay_bybletask(100);//�ȴ���ѹ�ȶ��ٽ��в���
				
				_BAT_RealTimeValue = ADC_BAT_Volt();
				
				if(_BAT_RealTimeValue >= BATTER_Volt_MAX)//������״̬
				{
					_charge_en = 0;	
					ChargeOK_Flag = ENABLED;
					ChargeIn_Flag = DISABLED;

					Charge_DIS();//���ʹ�ܹر�	
					LED_All_Off();
					RestartTiming();//���¼�ʱ	

					#ifdef DEBUG_PRINT
						//printf("\r\n charge ok");
					#endif
						
					RestartTiming();//���¼�ʱ	
					CurrentChoice_Pin_Init();//���������ƽų�ʼ��
					
					Volt_Class=4;//���������Ϊ��߼�					
					Volt_Class_Fresh = ENABLED;
					return BatterVolt_too_high;	
				}		
			}
			else if(_BAT_RealTimeValue <= BATTER_Volt_MIN)//������״̬	
			{	
				delay_bybletask(100);//�ȴ���ѹ�ȶ��ٽ��в���
				
				_BAT_RealTimeValue = ADC_BAT_Volt();
				if(_BAT_RealTimeValue <= BATTER_Volt_MIN)//������״̬
				{
					_charge_en = 0;	
					ChargeOK_Flag = DISABLED;
					ChargeIn_Flag = DISABLED;
					BatVoltTooLow_Flag = ENABLED;//�����ͱ�־λ
					
					Charge_DIS();//��ʹ�ܹر�	
					LED_All_Off();
					RestartTiming();//���¼�ʱ	

					#ifdef DEBUG_PRINT
						//printf("\r\n ��ص�ѹС��2.5");
					#endif
					
					CurrentChoice_Pin_Init();//���������ƽų�ʼ��	
					RestartTiming();//���¼�ʱ	
					Volt_Class_Fresh = ENABLED;
					Volt_Class=0;//���������Ϊ��͵ȼ�
					
					return BatterVolt_too_low;	
				}		
			}
			else if(volt_class==2)//�̵Ƴ���
			{
				_charge_en=1;
				led_green=1;led_red=0;led_red_toggle=0;	
			}				
			else if(volt_class==1)//��Ƴ���
			{
				_charge_en=1;
				led_red=1;led_green=0;led_red_toggle=0;
			}
			else if(volt_class==0)//�����
			{
				_charge_en=1;
				led_red_toggle =1;led_green=0;led_red=0;
			}		
			else;			
		}
		
		if(Min_Time>=180)	
		{
			_charge_en=0;
			ChargeOutTime = ENABLED;//��糬ʱ
			ChargeIn_Flag = DISABLED;
	
			Charge_DIS();//���ʹ�ܹر�	
			LED_All_Off();
			
			#ifdef DEBUG_PRINT
				//printf("\r\n charge outtime");
			#endif	
			
			CurrentChoice_Pin_Init();//���������ƽų�ʼ��
			LED_Flicker(RED_LED_Flicker ,3);
			RestartTiming();//���¼�ʱ
			Volt_Class_Fresh = ENABLED;
			
			return ChargeTimeout;
		}
		
		if(Sec_Flag==1)
		{
			Sec_Flag=0;
			
			if(ChargeStart_Status()==1)//������״̬	
			{	
				chargefull_time++;
				if(chargefull_time>=60)//������ȴ�1����
				{
					_charge_en = 0;	
					ChargeOK_Flag = ENABLED;
					ChargeIn_Flag = DISABLED;

					Charge_DIS();//���ʹ�ܹر�	
					CurrentChoice_Pin_Init();//���������ƽų�ʼ��
					LED_All_Off();
					RestartTiming();//���¼�ʱ	

					#ifdef DEBUG_PRINT
						//printf("\r\n �����˳����״̬");
					#endif
						
					Volt_Class_Fresh = ENABLED;
					Volt_Class=4;//���������Ϊ��߼�		
					
					RestartTiming();//���¼�ʱ							
					return Charge_Normal;	
				}				
			}
			else
				chargefull_time=0;
			
			res_ntc = NTC_Value();
			
			#ifdef DEBUG_PRINT
				//printf("\r\n NTC= %d, ntc_class: %d",(unsigned int)(res_ntc*1000),ntc_class);
			#endif				
	
			if(res_ntc < 2.923f)//����¶ȴ���61cֹͣ���
			{
				_charge_en=0;
				Fault_BatTemp_Flag = ENABLED;//
				BattempFaultMode_Worked=0;
				
				ntc_class = 6;
			}
			else if((res_ntc >= 2.923f)&&(res_ntc <4.152f ))//С��61������50
			{
				_charge_en=1;
				ntc_class = 5;
			}		
			else if((res_ntc >= 4.152f)&&(res_ntc <14.730f ))//С��50������15
			{
				_charge_en=1;
				ntc_class = 4;
			}			
			else if((res_ntc >= 14.730f )&&(res_ntc <18.031f ))//С��15������10
			{
				_charge_en=1;
				ntc_class = 3;				
			}				
			else if((res_ntc >= 18.031f)&&(res_ntc < 27.523f ))//С��10������0
			{
				_charge_en=1;
				ntc_class = 2;
			}
			else if(res_ntc >= 27.523f )//С��0
			{
				_charge_en=0;
				Charge_DIS();
				Fault_BatTemp_Flag = ENABLED;//
				BattempFaultMode_Worked = 0;
				
				ntc_class = 1;
			}
			else;

			if(res_ntc < 4.152f )//����50C
				bat_temp_toohot = ENABLED;
			
			if(_charge_en==1)
			{
				if(ntc_class ==5 )//С��61������50
				{
					if(_BAT_RealTimeValue <=3.0f)
						CurrentChoice_MIN();
					else
						CurrentChoice_MID();						
				}
				else if(ntc_class ==4 )//С��50������15
				{
					if(bat_temp_toohot==1)//һ����ص��¶ȴ���50�ȣ�������������
					{
						if(_BAT_RealTimeValue <=3.0f)
							CurrentChoice_MIN();
						else
							CurrentChoice_MID();	
					}
					else
					{
						if(_BAT_RealTimeValue <=3.0f)
							CurrentChoice_MIN();
						else
							CurrentChoice_MAX();				
					}				
				}
				else if(ntc_class ==3 )//С��15������10
				{
					if(_BAT_RealTimeValue <=3.0f)
						CurrentChoice_MIN();
					else
						CurrentChoice_MID();				
				}				
				else if(ntc_class ==2 )//С��10������0
				{
					CurrentChoice_MIN();
				}				
				else;
			}
			else//_charge_en=0;
				Charge_DIS();
		}
		
		if(ON_OFF_Flag == ENABLED)//����״̬��
		{
			if(KeyValue==Key_LongTime)	//���̹����еĿ��ػ�
			{
				KeyValue = Key_Invalid;
				ON_OFF_Flag = DISABLED;//���ػ�״̬����
				//RestartTiming();//���¼�ʱ				
			}		
		}
		else
		{
			if(KeyValue==Key_LongTime)	//���̹����еĿ��ػ�
			{
				KeyValue = Key_Invalid;
				ON_OFF_Flag = ENABLED;//���ػ�״̬����
				//RestartTiming();//���¼�ʱ				
			}		
		}

		if(_charge_en==1)//���˳���־
		{	
			Charge_EN();//�����NTCʹ�ܴ�	
			ChargeIn_Flag = ENABLED;
			PWMOUT_DIS();//���������
			LoadRESTest_DIS();//�������
			
			if(led_green==1)
			{
				LED_Green_On();LED_Red_Off();LED_Blue_Off();
			}	
			else if(led_red==1)
			{
				LED_Red_On();LED_Blue_Off();LED_Green_Off();
			}
			else if(led_red_toggle==1)				
			{
				LED_Red_On();LED_Blue_Off();LED_Green_Off();
			}
			else;
		}		
		else
		{
			ChargeIn_Flag = DISABLED;//����־λ����
			Charge_DIS();//�����NTCʹ�ܹر�	
			CurrentChoice_MIN();
		}
	}
	Charge_DIS();//�����NTCʹ�ܹر�
	CurrentChoice_Pin_Init();//���������ƽų�ʼ��
	LED_All_Off();
	RestartTiming();//���¼�ʱ
	delay_bybletask(100);//
	Volt_Class_Fresh = ENABLED;
	
	return Charge_Normal;
}
	
/*��������������ӳ�乫ʽ*/
float LinearMap(float val, float oriMin, float oriMax, float min, float max)
{
	float rate = 0;
	if(oriMax != oriMin)
	{
		rate = (val - oriMin) / (oriMax - oriMin);
	}
	return min - rate * min + rate * max;
}
/**********************************************/
/* �������ܣ���������  				     	  */
/* ��ڲ�������                               */
/**********************************************/		
Smokemode_ERROR Mode_Smoke_Work(void)
{		
	unsigned char _check=0;	
	float Realtime_BatVolt=0;
	float RDET_Volt=0;
	float RDET_Vlot_sum=0;
	float RDET_Vlot_ave=0;
	unsigned char _RDET_time=0;
	unsigned char rdet_read=0;
	float NTC_Res=0;
	int ret=0;
	Res_Test_Error res_ret=Normal;
	
	float TargerPower=0;
	float Differ_Press=0;
	float Resistant_Smoke=0;//����˿����ֵ
	float res_ntc =0;
	
	float energy=0;//���γ���ʹ�õ�����
	
	unsigned short int u16AdcResult=0;

	ChargeOK_Flag=0;
	Smokemode_start=0;//�����������н������״̬
	Charge_DIS();
	
	if(ADC_BAT_Volt()<=3.4f)
	{		
		Power_Lower = ENABLED;
		Lowv_Flag = ENABLED;//����͵���״̬
		LowvMode_Worked = 0;//	
		Smoke_Flag = DISABLED;//�����Ͳ��ܽ�����ϴ������ģʽ	
		Volt_Class = 0;
		
		return Smoke_BatterVolt_too_low;				
	}

	PWMOUT_DIS();
	LoadRESTest_EN();
	delay_bybletask(20);//��ʱ
	_check = CheckLoad();
	LoadRESTest_DIS();

	if(_check!=Normal)//��ֵ�쳣�����
		return Smoke_Res_Error;
	
	LED_All_Off();
	RestartTiming();
	Smoke_Flag = ENABLED;
	Smoke_output = DISABLED;
	SmokeTimeOut_Flag = DISABLED;
	m20_Sec_Flag = 1;
	
	LoadRESTest_EN();
	CalculateResistant_Value(&Resistant_Smoke,&res_ntc);	
	Realtime_BatVolt = ADC_BAT_RealTimeValue;//��ص�ѹ��ֵ
	
	res_ntc = NTC_Value();	
	if(res_ntc < 2.923f)//����¶ȴ���61c
	{
		Smoke_Flag=DISABLED;
		Smoke_battemptoohigh_Flag = ENABLED;//
		
		return Smoke_BatterTEMP_too_high;	
	}		
	if(res_ntc >= 43.155f )//С��-10
	{
		Smoke_Flag=DISABLED;
		Smoke_battemptoohigh_Flag=ENABLED;//
		
		return Smoke_BatterTEMP_too_low;	
	}
	
	while(Smoke_Flag==ENABLED)
	{
		ble_task();//��������ĵ���
		
		if(m20_Sec_Flag==1)
		{
			m20_Sec_Flag=0;
			{
				ret = dps310_get_processed_data (&drv_state,&pressure,&temperature);//pressure��temperature��ȫ�ֱ���
				
				if(ret<0 )
				{
					QuitSmoke_Work();
					return Smoke_DPS310_Error;
				}

				Differ_Press = DPS310_Standard-(float)pressure ;
				
				#ifdef DEBUG_PRINT
					//printf("������ѹ: %d Pa\t��׼ֵ: %d Pa\t��Ը�ѹ: %d Pa\r\n",(unsigned int)(pressure*100),(int)(DPS310_Standard*100),(int)((-1)*Differ_Press*100));	
					//printf("Absolute_Atmosphere:%dPa\tCalibration_value:%dPa\tD_value:%dPa\r\n",(unsigned int)(pressure*100),(int)(DPS310_Standard*100),(int)((-1)*Differ_Press*100));	
				#endif

				if( Differ_Press >= 2.0f)
				{
					LED_All_On();
					RestartTiming();
					Smoke_output = ENABLED;
					RestartTiming();
					{
						//TargerPower = 3.5  + Differ_Press * 0.75;//-200**-400
						TargerPower = LinearMap(Differ_Press, 2, 15, 3, 6.5);
						
						if(TargerPower >6.5f)//
							TargerPower=6.5f ;	
						else if(TargerPower <3.0f)//
							TargerPower=3.0f ;
						else;						
					}
					
					CalculatePWMDuty( TargerPower ,Resistant_Smoke,RDET_Vlot_ave);	
					
					energy	+=	TargerPower*0.02f;
				}
				else
				{
					LED_All_Off();	
					TargerPower = 0;
					Smoke_output = DISABLED;//ֹͣ����				
					Smoke_Flag = ENABLED;
					QuitSmoke_Work();
					
					return Smoke_Normal;
				}
				
				#ifdef DEBUG_PRINT				
					//printf("@%04d.%02d@%03d.%03d@%03d.%03d@%04d.%02d#",	 \
						(unsigned int)pressure,(unsigned int)((pressure-(unsigned int)pressure)*100),	\
						(int)(Differ_Press),(unsigned int)((Differ_Press-(unsigned int)Differ_Press)*100),	\
						(unsigned int)(TargerPower),(unsigned int)((TargerPower-(unsigned int)TargerPower)*1000),\
						(unsigned int)energy,(unsigned int)((energy-(unsigned int)energy)*100));	
				#endif
			}
		}
		
		if(Smoke_output == ENABLED)//��������У��Ѿ����������
		{
			if(Power_out_start == 1)//���ڱ�ʶ����صĵ�ѹ
			{
				if(rdet_read==0)
				{
					rdet_read=1;
					Realtime_BatVolt = ADC_BAT_Volt();
					
					if(Realtime_BatVolt < 3.0f)
					{
						LoadRESTest_DIS();		
						
						#ifdef DEBUG_PRINT
							//printf("\r\n ��ص���̫�ͣ����������:=%d",(unsigned int)(Realtime_BatVolt*1000));
						#endif
						
						Power_Lower = ENABLED;
						Lowv_Flag = ENABLED;
						LowvMode_Worked = 0;//
						Smoke_Flag = DISABLED;	
						QuitSmoke_Work();
						
						return Smoke_BatterVolt_too_low;	
					}				
				}
				
				_RDET_time++;				
				RDET_Volt = ADC_R_Volt();
				RDET_Vlot_sum += RDET_Volt;
				
				if(_RDET_time>=8)
				{
					_RDET_time=0;
					Power_out_start=0;
					RDET_Vlot_ave = RDET_Vlot_sum/8;					
					RDET_Vlot_sum=0;
				}
			}
			
			if(Output_start==1)
			{
				ResR_EDT_i[1] = ResR_EDT_i[0];//���Ϊ0���£�1Ϊ�ɵ�
//				ADC_ChannelSel(4);
//				
//				Adc_ClrAccResult();
//				Adc_Start();
//				while(FALSE != Adc_PollBusyState());//�ȴ�	
//				Adc_GetResult(&u16AdcResult);
				ResR_EDT_i[0] = u16AdcResult;
				
				if(Read_ADC_I_DET_Flag==0)
				{
					if((((float)ResR_EDT_i[0] / (float)ResR_EDT_i[1]) < 0.9f)&&(ResR_EDT_i[0]!=0)&&(ResR_EDT_i[1]!=0))
					{
						//Gpio_SetIO(3,5,1); //PWMOUT_DIS();
						//Gpio_SetIO(0,3,1);//LoadRESTest_DIS();
						LED_All_Off();
						Smoke_Flag	=DISABLED;
						Smoke_output=DISABLED;
						Output_start=DISABLED;
						ShortCircuit_Flag=ENABLED;//��·��־��λ	
						QuitSmoke_Work();
											
						return Smoke_Res_Short;
					}				
				}
			}
			
			if(Read_ADC_I_DET_Flag==1)	//�ж��Ƿ�������贰��
			{
				Read_ADC_I_DET_Flag=0;	
				rdet_read=0;
			
				LoadRESTest_EN();
				res_ret = CalculateResistant_Value(&Resistant_Smoke,&NTC_Res);
				
				if(res_ret != Normal)
				{
					QuitSmoke_Work();
					return Smoke_Res_Error;
				}	
				
				if(( NTC_Res<= 2.923f )||(NTC_Res >= 43.155f ))//����¶ȴ���60c //С��-10
				{
					Smoke_Flag=0;
					Smoke_battemptoohigh_Flag=1;//
					
					QuitSmoke_Work();
					return Smoke_BatterTEMP_Error;					
				}	
			}		
			else		
				LoadRESTest_DIS();		
		}
		else		
			LoadRESTest_DIS();	
		
	}
	QuitSmoke_Work();
	return Smoke_Normal;
}

/**********************************************/
/* �������ܣ����ڸ�Ӧ������Ч�������ڴ������̹���*/
/* ��ڲ�������    								 */
/* ����ֵ  ������Ϊ100ʱ����ʾ��Ӧ�����̣�
			 ����ֵΪ0ʱ����ʾû�б仯          
												 */
/**********************************************/
DPS310_respiratory DPS310_Respiratory(void)
{
	unsigned char i=8;
	int check=0; 

	check=dps310_get_processed_data (&drv_state,&pressure,&temperature);//pressure��temperature��ȫ�ֱ���
	
	#ifdef DEBUG_PRINT
	{
		float Differ_Press=0;
		float TargerPower=0;	
		float energy = 0;
		Differ_Press = DPS310_Standard-(float)pressure ;
		//printf("������ѹ: %d Pa\t��׼ֵ: %d Pa\t��Ը�ѹ: %d Pa\r\n",(unsigned int)(pressure*100),(int)(DPS310_Standard*100),(int)((-1)*Differ_Press*100));	
		//printf("Absolute_Atmosphere:%dPa\tCalibration_value:%dPa\tD_value:%dPa\r\n",(unsigned int)(pressure*100),(int)(DPS310_Standard*100),(int)((-1)*Differ_Press*100));	
		//printf("Abs:%dPa\tCal:%dPa\tD_value:%dPa\tPower:%dmW\tBatV:%dmV\r\n", \
			(unsigned int)(pressure*100),(int)(DPS310_Standard*100),(int)((-1)*Differ_Press*100),(unsigned int)(0),(unsigned int)(ADC_BAT_RealTimeValue*1000));	
		
		//printf("@%04d.%02d@%03d.%03d@%03d.%03d@%04d.%02d#",	 \
						(unsigned int)pressure,(unsigned int)((pressure-(unsigned int)pressure)*100),	\
						(int)(Differ_Press),(unsigned int)((Differ_Press-(unsigned int)Differ_Press)*100),	\
						(unsigned int)(TargerPower),(unsigned int)((TargerPower-(unsigned int)TargerPower)*1000),\
						(unsigned int)energy,(unsigned int)((energy-(unsigned int)energy)*100));	
	}	
	#endif
	
	if(check<0)
	{
		do
		{
			check=dps310_get_processed_data (&drv_state,&pressure,&temperature);
			ble_task();
		}while(check<0);	
	}	
	
	for(i=8;i>0;i--)
		pressure_i[i]=pressure_i[i-1];
	pressure_i[0]=(float)pressure;
		
	if(pressure_i[7]-pressure_i[0]>=2.0f)//ѹ�������½�����Ϊ��⵽������
	{
		if((pressure_i[6]-pressure_i[1]>=0.6f)&&(pressure_i[5]-pressure_i[2]>=0.25f)&&(pressure_i[4]-pressure_i[3]>=0.05f))
		{
			#ifdef DEBUG_PRINT
				//printf("\r\n ��Ӧ��������");//����ʹ��	
			#endif	
			
			for(i=9;i>0;i--)
				pressure_i[i] = (float)pressure;			
			
			return DPS_smoke_ok;
		
		}	
	}     
	else
		return DPS_smoke_no;
	
	return DPS_smoke_no;
}

/**********************************************/
/* ��������:�˳�����״̬			     	  */
/* ��ڲ�������                               */
/**********************************************/	
void QuitSmoke_Work(void)
{
	Smoke_Flag = DISABLED;	
	PWMOUT_DIS();
	LoadRESTest_DIS();	
	CTIMER_StopTimer(CTIMER2);
	Charge_DIS();//�����NTCʹ�ܹر�
	
	LED_All_Off();	
	
	if(SmokeTimeOut_Flag==1)
	{
		LED_Flicker(RED_LED_Flicker ,8);
		ON_OFF_Flag = DISABLED;//����ػ�״̬	
	}

	Smoke_output=DISABLED;
	Smoke_Sec_Time=0;
	Read_ADC_I_DET_Flag=0;
	RestartTiming();	
}

/**********************************************/
/* ��������:˯������  				     	  */
/* ��ڲ�������                               */
/**********************************************/		
void Mode_Sleep_Work(void)
{	
	#ifdef DEBUG_PRINT
		//printf("\r\n ����˯��ģʽ");//����ʹ��
	#endif	
	
	LED_All_Off();
	if(OnOffPowerByKey==1)//����״̬��˯��ʱ���ƣ�����ӹػ�ģʽ����˯�߲�����	,ֻ�а����ػ������ƣ���Ȼ�ػ�������		
	{
		switch (Volt_Class)
		{
			case 4:{LED_Flicker(GREEN_LED_Flicker,	3);}break;
			case 3:{LED_Flicker(YELLOW_LED_Flicker,	3);}break;
			case 2:{LED_Flicker(RED_LED_Flicker ,	3);}break;
			case 1:{LED_Flicker(RED_LED_Flicker ,	3);}break;	
			case 0:{LED_Flicker(RED_LED_Flicker ,	3);}break;	
			default:;break;
		}
	}		
	
	EnterPowerSleepMode();
}

/**********************************************/
/* �������ܣ�				                  */
/* ��ڲ�������                               */
/**********************************************/
void Mode_Lowv_Work(void)
{
	Smoke_Flag=DISABLED;//�����Ͳ��ܽ�����ϴ������ģʽ
	RestartTiming();
	Charge_DIS();//�رճ��ʹ��
	
	#ifdef DEBUG_PRINT	
		//printf("\r\n �͵���ģʽ");//����ʹ��
	#endif
	
	LED_All_Off();
	LED_Flicker(RED_LED_Flicker ,3);	
	
	Lowv_Flag = ENABLED;//����͵���״̬		
	LowvMode_Worked = 1;//ִ�й��˹��ž�ʾ
	ChargeOK_Flag = DISABLED;
	Power_Lower = DISABLED;	
	RestartTiming();
}
	
/**********************************************/
/* �������ܣ�				                  */
/* ��ڲ�������                               */
/**********************************************/	
void Mode_Idle_Work(void)
{
	Idle_Flag = ENABLED;	
	Smoke_Flag = DISABLED;
	
	PWMOUT_DIS();
	LoadRESTest_DIS();
	LED_All_Off();
	Charge_DIS();//�رճ��ʹ��
}

/**********************************************/
/* �������ܣ�				                  */
/* ��ڲ�������                               */
/**********************************************/
void Mode_ShortCircuit_Work(void)
{
	Smoke_Flag=0;
	PWMOUT_DIS();
	LoadRESTest_DIS();
	Charge_DIS();//�رճ��ʹ��
	
	#ifdef DEBUG_PRINT	
	//{
	//	float bat=0;	

	//	bat = ADC_BAT_Volt();	
	//	printf("\r\n ��·,%d",(unsigned int)(bat*1000) );
	//}	
	#endif
	
	LED_All_Off();
	LED_Flicker(RED_LED_Flicker ,3);	

	Idle_Flag = ENABLED;
	ShortCircuit_Flag = DISABLED;
	Volt_Class_Fresh = ENABLED;//���¶������
	
	RestartTiming();
}

/**********************************************/
/* �������ܣ�				                  */
/* ��ڲ�������                               */
/**********************************************/	
void Mode_OpenCircuit_Work(void)
{
	Smoke_Flag=0;
	PWMOUT_DIS();
	LoadRESTest_DIS();
	Charge_DIS();//�رճ��ʹ��
			
	#ifdef DEBUG_PRINT	
		//printf("\r\n��·");
	#endif
	
	LED_All_Off();
	LED_Flicker(RED_LED_Flicker ,3);	
	
	Idle_Flag = ENABLED;
	OpenCircuit_Flag = DISABLED;
	
	RestartTiming();
}
	
/**********************************************/
/* �������ܣ�����˿����	����				  */
/* ��ڲ�������                               */
/**********************************************/	
void Fault_OverTemp_Work(void)
{
	Smoke_Flag=0;
	PWMOUT_DIS();
	LoadRESTest_DIS();
	Charge_DIS();//�رճ��ʹ��
			
	#ifdef DEBUG_PRINT	
		//printf("\r\nworking mode res is too high");
	#endif
	
	LED_Flicker(WHITE_LED_Flicker ,5);	

	Idle_Flag=ENABLED;
	Heater_OverTemp_Flag=DISABLED;	
	RestartTiming();	

	OnOffPowerByKey=DISABLED;//�ǰ����ػ�
	ON_OFF_Flag =DISABLED;//��������
}

/**********************************************/
/* �������ܣ�����¶ȹ���	����			  */
/* ��ڲ�������                               */
/**********************************************/	
void Fault_BatTemp_Work(void)
{
	Smoke_Flag=0;
	PWMOUT_DIS();
	LoadRESTest_DIS();
	Charge_DIS();//�رճ��ʹ��
			
	#ifdef DEBUG_PRINT	
		//printf("\r\nbat temp fault");
	#endif
	
	LED_All_Off();
	LED_Flicker(RED_LED_Flicker ,3);	
	
	Idle_Flag=ENABLED;
	Fault_BatTemp_Flag=ENABLED;
	BattempFaultMode_Worked = ENABLED;
	Smoke_battemptoohigh_Flag=DISABLED;//��������¶ȹ�
	RestartTiming();
	
	OnOffPowerByKey = DISABLED;//�ǰ����ػ�
	ON_OFF_Flag = DISABLED;//��������
}

/**********************************************/
/* �������ܣ���ص�ѹ���� �쳣				  */
/* ��ڲ�������                               */
/**********************************************/
void Fault_BatVoltTooLow_Work(void)
{
	float vin_realtime_volt=0;
	
	Smoke_Flag=0;
	PWMOUT_DIS();
	LoadRESTest_DIS();
	LED_All_Off();	
	Charge_DIS();//�رճ��ʹ��
			
	while(BatVoltTooLow_Flag==1)
	{
		#ifdef DEBUG_PRINT	
			//printf("\r\nthe bat volt is < 2.5");
		#endif		
		
		NTC_DIS();//�رճ��ʹ��
		vin_realtime_volt =  ADC_CHANGE_Volt();
		
		if(vin_realtime_volt <= 1.0f)
			BatVoltTooLow_Flag=0;
		else
			BatVoltTooLow_Flag=1;
		
		LED_Red_Off();
		delay_bybletask(200);
		LED_Red_On();
		delay_bybletask(200);
	}
		
	Idle_Flag = ENABLED;
	BatVoltTooLow_Flag = DISABLED;
	RestartTiming();
}

/**********************************************/
/* �������ܣ����¿�ʼ��ʱ������������ʼʱ�ṩʱ�� */
/* ��ڲ�������                               */
/**********************************************/
void RestartTiming(void)
{
	_Time=0;//���¿�ʼ��ʱ
	delaytobletask = 0;
	Min_Time=0;//���¿�ʼ��ʱ
	Sec_Time=0;//���¿�ʼ��ʱ
	Min_Flag=0;
	Sec_Flag=0;
}

/**********************************************/
/* �������ܣ�����˯��ģʽ	                  */
/* ��ڲ�������                               */
/**********************************************/
void EnterPowerSleepMode(void)
{
	ReadyForSleepMode();
	POWER_EnterPowerDown(0);
	WakeUpFormSleepMode();	
}

static void switch_to_OSC32M(void)
{
    POWER_WritePmuCtrl1(SYSCON, SYSCON_PMU_CTRL1_OSC32M_DIS_MASK, SYSCON_PMU_CTRL1_OSC32M_DIS(0U));
    SYSCON->CLK_CTRL = (SYSCON->CLK_CTRL & ~SYSCON_CLK_CTRL_SYS_CLK_SEL_MASK) | SYSCON_CLK_CTRL_SYS_CLK_SEL(0U);
}

static void switch_to_XTAL(void)
{
    /* switch to XTAL after it is stable */
    while (!(SYSCON_SYS_MODE_CTRL_XTAL_RDY_MASK & SYSCON->SYS_MODE_CTRL)) ;
       
    SYSCON->CLK_CTRL = (SYSCON->CLK_CTRL & ~SYSCON_CLK_CTRL_SYS_CLK_SEL_MASK) | SYSCON_CLK_CTRL_SYS_CLK_SEL(1U);
    POWER_WritePmuCtrl1(SYSCON, SYSCON_PMU_CTRL1_OSC32M_DIS_MASK, SYSCON_PMU_CTRL1_OSC32M_DIS(1U));
}

/**********************************************/
/* �������ܣ�����˯��ģʽ��׼��               */
/* ��ڲ�������                               */
/**********************************************/
extern void BleApp_HandleKeys(key_event_t events);

void ReadyForSleepMode(void)
{	
	uint32_t msk, val;
	
	BleApp_HandleKeys(gKBD_EventLongPB1_c);	
	dps310_standby(&drv_state);		
	
	LED_All_Off();
	Smoke_Flag = DISABLED;	
	PWMOUT_DIS();
	LoadRESTest_DIS();
	ADC_AllClose();
	LED_Pin_Init();
	NTC_DIS();
	IIC_IO_Confing();
	ChargeIn_Pin_Init();
	ChargeStart_Init();
	CurrentChoice_Pin_Init();
	TEST_LED_Pin_Init();
	
	CTIMER_StopTimer(CTIMER1);
	CTIMER_StopTimer(CTIMER2);
	CTIMER_StopTimer(CTIMER3);
	
	CLOCK_DisableClock(kCLOCK_Ctimer1);
	CLOCK_DisableClock(kCLOCK_Ctimer2);
	CLOCK_DisableClock(kCLOCK_Ctimer3);	
	CLOCK_DisableClock(kCLOCK_Flexcomm0);
	
	__disable_irq();	
	
	/* Power down affects calibration's FSM, turn it off before power down.
     * Turn off Ble core's high frequency clock before power down.*/  
    //SYSCON->CLK_DIS = SYSCON_CLK_DIS_CLK_CAL_DIS_MASK | SYSCON_CLK_DIS_CLK_BLE_DIS_MASK;	
	//CLOCK_DisableClock(kCLOCK_Ble);
	POWER_EnableDCDC(false);
	APP_SaveBleReg();
	
	POWER_Init();
	POWER_EnablePD(kPDRUNCFG_PD_ADC_VCM);
	POWER_EnablePD(kPDRUNCFG_PD_CAP_SEN);
	POWER_EnablePD(kPDRUNCFG_PD_MEM9);
    POWER_EnablePD(kPDRUNCFG_PD_MEM8);
    POWER_EnablePD(kPDRUNCFG_PD_MEM7);	
	
	msk = SYSCON_PMU_CTRL1_XTAL32K_PDM_DIS_MASK | SYSCON_PMU_CTRL1_RCO32K_PDM_DIS_MASK |
          SYSCON_PMU_CTRL1_XTAL32K_DIS_MASK | SYSCON_PMU_CTRL1_RCO32K_DIS_MASK;
	
	val = SYSCON_PMU_CTRL1_XTAL32K_PDM_DIS(1U)  /* switch off XTAL32K during power down */
	  | SYSCON_PMU_CTRL1_RCO32K_PDM_DIS(1U) /* switch on RCO32K during power down */
	  | SYSCON_PMU_CTRL1_XTAL32K_DIS(1U)    /* switch off XTAL32K at all time */
	  | SYSCON_PMU_CTRL1_RCO32K_DIS(1U);    /* switch on RCO32K */

	POWER_WritePmuCtrl1(SYSCON, msk, val);
	
	/* Enable GPIO wakeup */
	SYSCON->PIO_WAKEUP_LVL0 = SYSCON->PIO_WAKEUP_LVL0 | USER_SW1_GPIO_PIN_MASK;
    SYSCON->PIO_WAKEUP_EN0 = SYSCON->PIO_WAKEUP_EN0 | USER_SW1_GPIO_PIN_MASK;
	POWER_EnableSwdWakeup();//ͨ��swd�ڽ��л���
	
    NVIC_ClearPendingIRQ(EXT_GPIO_WAKEUP_IRQn);
    NVIC_EnableIRQ(EXT_GPIO_WAKEUP_IRQn);	
	
	switch_to_OSC32M();
	POWER_LatchIO();
	
	Key_DownCount=0;
	Key_HodeOnTime=0;
	Key_RleaseTime=0;
	Key_RleaseTime_onetime=0;
	KeyValue=Key_Invalid;
	
	RestartTiming();	
}

/**********************************************/
/* �������ܣ���˯��ģʽ�л��Ѻ�������һ��     */
/* ��ڲ�������                               */
/**********************************************/
void WakeUpFormSleepMode(void)
{	
	int ret=0;

	switch_to_XTAL();
	BOARD_BootClockHSRUN();
    SystemCoreClockUpdate();/* Update SystemCoreClock if default clock value (16MHz) has changed */
	
	POWER_RestoreIO();
	
	POWER_EnableDCDC(true);
    POWER_EnableADC(true);
	
	CLOCK_EnableClock(kCLOCK_Ble);	
	CLOCK_EnableClock(kCLOCK_Flexcomm0);	
	
	POWER_RestoreSwd();//

	NVIC_DisableIRQ(EXT_GPIO_WAKEUP_IRQn);
	NVIC_ClearPendingIRQ(EXT_GPIO_WAKEUP_IRQn);
	
	ADC_Pin_init();
	//ADC_Configuration();
	LED_Pin_Init();//RGB_PWM_init();
	Key_Init();
	IIC_IO_Confing();	
	LoadRESTest_Pin_Init();	
	PWMOUT_Pin_Init();
	NTCPin_Init();	
	Charge_Init();
	ChargeStart_Init();
	CurrentChoice_Pin_Init();	
	TEST_LED_Pin_Init();
	BOARD_InitPins();
	
	__enable_irq();	
	
	Time1_Init(SYS_FREQUENCY);
	Time2_Init(2);
	//Time3_Init(2);
	
	delay_bybletask(40);
	
	do
	{
		ret = dps310_config(&drv_state, OSR_8,TMP_MR_4, OSR_8,PM_MR_64,drv_state.tmp_ext);
		ble_task();
	}
	while(ret<0);

	do
	{
		ret = dps310_resume(&drv_state);
		ble_task();
	}
	while(ret<0);

	Sleep_Flag 			= DISABLED;
	ShortCircuit_Flag 	= DISABLED;
	OpenCircuit_Flag 	= DISABLED;
	ChargeOutTime  		= DISABLED;//���糬ʱ��־	
	SmokeTimeOut_Flag 	= DISABLED;
	Key_Fall_Flag 		= ENABLED;	
	Fault_BatTemp_Flag	= DISABLED;		
	BattempFaultMode_Worked = DISABLED;
	Lock_Unlock_byBLE_Flag	= INVALID;	
	
	RestartTiming();			
}
