/*! *********************************************************************************
* \addtogroup Private Profile Server
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright (c) 2014, Freescale Semiconductor, Inc.
* Copyright 2016-2018 NXP
* All rights reserved.
*
* \file
*
* This file is the source file for the QPP Server application
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
/* #undef CR_INTEGER_PRINTF to force the usage of the sprintf() function provided
 * by the compiler in this file. The sprintf() function is #included from
 * the <stdio.h> file. */
#ifdef CR_INTEGER_PRINTF
    #undef CR_INTEGER_PRINTF
#endif

/* Framework / Drivers */
#include "stdio.h"
#include "RNG_Interface.h"
#include "Keyboard.h"
#include "LED.h"
#include "TimersManager.h"
#include "FunctionLib.h"
#include "SerialManager.h"
#include "MemManager.h"
#include "Panic.h"

/* BLE Host Stack */
#include "gatt_server_interface.h"
#include "gatt_client_interface.h"
#include "gap_interface.h"
#include "gatt_db_handles.h"

/* Profile / Services */
#include "battery_interface.h"
#include "device_info_interface.h"
#include "private_profile_interface.h"
#include "ecigarettes_profile_interface.h"

/* Connection Manager */
#include "ble_conn_manager.h"

#include "board.h"
#include "ApplMain.h"
#include "private_profile_server.h"


#include "Timer_user.h"

extern unsigned char USER_GetBatteryLevel(void);

/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/
#define mBatteryLevelReportInterval_c                (1)          /* battery level report interval in seconds  */
//#define mQppsThroughputStatisticsInterval_c        (10000)      /* Throughput Statistics interval in miliseconds  */
//#define mQppsTxInterval_c                          (0)          /* Qpps send data interval in miliseconds  */
#define mQppsMaxTestDataLength_c                     (244)        /* the length of data that Qpps send every time*/

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/
typedef enum
{
#if gAppUseBonding_d
    fastWhiteListAdvState_c,
#endif
    fastAdvState_c,
    slowAdvState_c
}advType_t;

typedef struct advState_tag{
    bool_t      advOn;
    advType_t   advType;
}advState_t;

typedef struct appPeerInfo_tag
{
    uint8_t deviceId;
    uint8_t ntf_cfg;
    uint64_t bytsSentPerInterval;
    uint64_t bytsReceivedPerInterval;
}appPeerInfo_t;

typedef struct appTxInfo_tag
{
    uint32_t TakenSeconds;
    uint32_t RxSpeed[gAppMaxConnections_c];
    uint32_t TxSpeed[gAppMaxConnections_c];
}txInfo_t;

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
uint8_t gAppSerMgrIf;
/* Adv State */
static advState_t  mAdvState;
static bool_t      mRestartAdv;
static uint32_t    mAdvTimeout;
/* Service Data*/
static bool_t          		basValidClientList[gAppMaxConnections_c] = { FALSE };
static basConfig_t      	basServiceConfig = {service_battery, 0, basValidClientList, gAppMaxConnections_c};
static disConfig_t 			disServiceConfig = {service_device_info};
static qppsConfig_t 		qppServiceConfig = {service_qpps};
static ecigarettesConfig_t 	ecigServiceConfig = {service_ecig};//modify by wzy

static uint16_t cpHandles[] = {value_qpps_rx,value_locking_rx,value_ecig_command_rx,value_ecig_data_rx,value_setgetsmokepower_tx};//收到的常规数据  ，收到的锁定数据

/* Application specific data*/
//static txInfo_t mTxInfo;

static tmrTimerID_t 	mAdvTimerId;
static tmrTimerID_t 	mBatteryMeasurementTimerId;
//static tmrTimerID_t 	mQppsThroughputStatisticsTimerId;
static appPeerInfo_t 	mPeerInformation[gAppMaxConnections_c];
static uint8_t 			mQppsTestDataLength = (gAttDefaultMtu_c-3);

static uint8_t isBatterTimerAllowed=0;//用于标识是否定时器已被申请  modify by wzy

/************************************************************************************
*************************************************************************************
* Private functions prototypes
*************************************************************************************
************************************************************************************/
/* Gatt and Att callbacks */
static void BleApp_AdvertisingCallback (gapAdvertisingEvent_t* pAdvertisingEvent);
static void BleApp_ConnectionCallback (deviceId_t peerDeviceId, gapConnectionEvent_t* pConnectionEvent);
static void BleApp_GattServerCallback (deviceId_t deviceId, gattServerEvent_t* pServerEvent);
void BleApp_Config(void);

/* Timer Callbacks */
static void AdvertisingTimerCallback (void *);
static void BatteryMeasurementTimerCallback (void *);
//static void QppsThoughputStatisticsTimerCallback(void* pParam);

//static void TxPrintCallback(void* pParam);
//static void QppsTxCallback(void * pParam);

static void BleApp_Advertise(void);
static void BleApp_ReceivedDataHandler(deviceId_t deviceId, uint8_t* aValue,uint16_t valueLength);

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
* \brief    Initializes application specific functionality before the BLE stack init.
*
********************************************************************************** */
void BleApp_Init(void)
{
    /* Initialize application support for drivers */
    //BOARD_InitAdc();//原板的电池电量测试 //modify by wzy

    /* UI */
    SerialManager_Init();
    /* Register Serial Manager interface */
    Serial_InitInterface(&gAppSerMgrIf, APP_SERIAL_INTERFACE_TYPE, APP_SERIAL_INTERFACE_INSTANCE);
    Serial_SetBaudRate(gAppSerMgrIf, gUARTBaudRate115200_c);
}

/*! *********************************************************************************
* \brief    Starts the BLE application.
*
********************************************************************************** */
void BleApp_Start(void)
{
#if gAppUseBonding_d
    if (gcBondedDevices > 0)
    {
        mAdvState.advType = fastWhiteListAdvState_c;
    }
    else
    {
#endif
        mAdvState.advType = fastAdvState_c;
#if gAppUseBonding_d
    }
#endif

    BleApp_Advertise();
}

/*! *********************************************************************************
* \brief        Handles keyboard events.
*
* \param[in]    events    Key event structure.
********************************************************************************** */
void BleApp_HandleKeys(key_event_t events)
{
    uint8_t i;

    switch (events)
    {
        case gKBD_EventPressPB1_c:
        {
			for (i = 0; i < gAppMaxConnections_c; i++)
			{
			  if (mPeerInformation[i].deviceId == gInvalidDeviceId_c)
				break;
			}

			if(i < gAppMaxConnections_c)
			{
				mRestartAdv = TRUE;
				BleApp_Start();			
			} break; 
        }
        case gKBD_EventLongPB1_c:
        {
            for (i = 0; i < gAppMaxConnections_c; i++)
            {
				if(mAdvState.advOn)
				{
					Gap_StopAdvertising();
					mAdvState.advOn = FALSE;
					mRestartAdv = TRUE;
				}

				if (mPeerInformation[i].deviceId != gInvalidDeviceId_c)
				{
					Gap_Disconnect(mPeerInformation[i].deviceId);
					Bas_Unsubscribe(&basServiceConfig, i);
					/* qpps Unsubscribe client */
					Qpp_Unsubscribe();
					ECig_Unsubscribe();
					mPeerInformation[i].bytsReceivedPerInterval = 0;
					mPeerInformation[i].bytsSentPerInterval = 0;
					mPeerInformation[i].ntf_cfg = QPPS_VALUE_NTF_OFF;
					mPeerInformation[i].deviceId = gInvalidDeviceId_c;
					
					TMR_StopTimer(mBatteryMeasurementTimerId);
					//TMR_StopTimer(mQppsThroughputStatisticsTimerId);			
				}	
            }
            break;
        }
        case gKBD_EventLongPB2_c:
        {
            break;
        }
        default:
            break;
    }
}

/*! *********************************************************************************
* \brief        Handles BLE generic callback.
*
* \param[in]    pGenericEvent    Pointer to gapGenericEvent_t.
********************************************************************************** */
void BleApp_GenericCallback (gapGenericEvent_t* pGenericEvent)
{
    /* Call BLE Conn Manager */
    BleConnManager_GenericEvent(pGenericEvent);

    switch (pGenericEvent->eventType)
    {
    case gInitializationComplete_c:
        {
            BleApp_Config();
        }
        break;

    case gAdvertisingParametersSetupComplete_c:
        {
            App_StartAdvertising(BleApp_AdvertisingCallback, BleApp_ConnectionCallback);
        }
        break;

    case gTxEntryAvailable_c:
        break;

    default:
        break;
    }
}

/*! *********************************************************************************
* \brief        Configures BLE Stack after initialization. Usually used for
*               configuring advertising, scanning, white list, services, et al.
*
********************************************************************************** */
void BleApp_Config()
{
    /* Configure as GAP peripheral */
    BleConnManager_GapPeripheralConfig();

    /* Register for callbacks*/
    GattServer_RegisterHandlesForWriteNotifications(NumberOfElements(cpHandles), cpHandles);
    App_RegisterGattServerCallback(BleApp_GattServerCallback);

    mAdvState.advOn = FALSE;
    for (uint8_t i = 0; i < gAppMaxConnections_c; i++)
    {
        mPeerInformation[i].deviceId= gInvalidDeviceId_c;
    }

    basServiceConfig.batteryLevel = USER_GetBatteryLevel();
    Bas_Start(&basServiceConfig);
    Dis_Start(&disServiceConfig);
    Qpp_Start (&qppServiceConfig);
	ECig_Start (&ecigServiceConfig);//modify by wzy
	
	if(isBatterTimerAllowed ==0)//modify by wzy
	{
		/* Allocate application timers */
		mAdvTimerId = TMR_AllocateTimer();
		mBatteryMeasurementTimerId = TMR_AllocateTimer();	
	
		isBatterTimerAllowed =1;
	}
	else
	{
		TMR_StartLowPowerTimer(mBatteryMeasurementTimerId, gTmrLowPowerIntervalMillisTimer_c,
             TmrSeconds(mBatteryLevelReportInterval_c), BatteryMeasurementTimerCallback, NULL);
	}

    //mQppsThroughputStatisticsTimerId = TMR_AllocateTimer();//modify by wzy
	
    /* UI */
    //Serial_Print(gAppSerMgrIf, "\r\nPress ADVSW to connect to a QPP Client!\r\n", gNoBlock_d);//modify by wzy
	
	BleApp_HandleKeys(gKBD_EventPressPB1_c);
}

/*! *********************************************************************************
* \brief        Configures GAP Advertise parameters. Advertise will start after
*               the parameters are set.
*
********************************************************************************** */
static void BleApp_Advertise(void)
{
    switch (mAdvState.advType)
    {
	#if gAppUseBonding_d
    case fastWhiteListAdvState_c:
        {
            gAdvParams.minInterval = gFastConnMinAdvInterval_c;
            gAdvParams.maxInterval = gFastConnMaxAdvInterval_c;
            gAdvParams.filterPolicy = gProcessWhiteListOnly_c;
            mAdvTimeout = gFastConnWhiteListAdvTime_c;
        }
        break;
	#endif
    case fastAdvState_c:
        {
            gAdvParams.minInterval = gFastConnMinAdvInterval_c;
            gAdvParams.maxInterval = gFastConnMaxAdvInterval_c;
            gAdvParams.filterPolicy = gProcessAll_c;
            mAdvTimeout = gFastConnAdvTime_c - gFastConnWhiteListAdvTime_c;
        }
        break;

    case slowAdvState_c:
        {
            gAdvParams.minInterval = gReducedPowerMinAdvInterval_c;
            gAdvParams.maxInterval = gReducedPowerMinAdvInterval_c;
            gAdvParams.filterPolicy = gProcessAll_c;
            mAdvTimeout = gReducedPowerAdvTime_c;
        }
        break;
    }
	
    /* Set advertising parameters*/
    Gap_SetAdvertisingParameters(&gAdvParams);
	
	TEST_LED_Pin_Init();//modify by wzy
	Time3_Init(2);//start led  //modify by wzy
}

/*! *********************************************************************************
* \brief        Handles BLE Advertising callback from host stack.
*
* \param[in]    pAdvertisingEvent    Pointer to gapAdvertisingEvent_t.
********************************************************************************** */
static void BleApp_AdvertisingCallback (gapAdvertisingEvent_t* pAdvertisingEvent)
{
    switch (pAdvertisingEvent->eventType)
    {
    case gAdvertisingStateChanged_c:
        {
            mAdvState.advOn = !mAdvState.advOn;

            if (!mAdvState.advOn && mRestartAdv)
            {
                BleApp_Advertise();
                break;
            }

            if(!mAdvState.advOn)
            {
               	CTIMER_StopTimer(CTIMER3);//modify by wzy
				TEST_LED_Pin_Init();//modify by wzy
            }
            else
            {
                Serial_Print(gAppSerMgrIf, "\r\nAdvertising...\r\n", gNoBlock_d);
                TMR_StartLowPowerTimer(mAdvTimerId,gTmrLowPowerSecondTimer_c,
                                       TmrSeconds(mAdvTimeout), AdvertisingTimerCallback, NULL);
            }
        }
        break;

    case gAdvertisingCommandFailed_c:
        {
            panic(0,0,0,0);
        }
        break;

    default:
        break;
    }
}

/*! *********************************************************************************
* \brief        Handles advertising timer callback.
*
* \param[in]    pParam        Calback parameters.
********************************************************************************** */
static void AdvertisingTimerCallback(void * pParam)
{
    /* Stop and restart advertising with new parameters */
    Gap_StopAdvertising();

    switch (mAdvState.advType)
    {
#if gAppUseBonding_d
    case fastWhiteListAdvState_c:
        {
            mAdvState.advType = fastAdvState_c;
            mRestartAdv = TRUE;
        }
        break;
#endif
    case fastAdvState_c:
        {
            mAdvState.advType = slowAdvState_c;
            mRestartAdv = TRUE;
        }
        break;

    default:
        {
            mRestartAdv = FALSE;
        }
        break;
    }
}

/*! *********************************************************************************
* \brief        Handles BLE Connection callback from host stack.
*
* \param[in]    peerDeviceId        Peer device ID.
* \param[in]    pConnectionEvent    Pointer to gapConnectionEvent_t.
********************************************************************************** */
static void BleApp_ConnectionCallback (deviceId_t peerDeviceId, gapConnectionEvent_t* pConnectionEvent)
{
    /* Connection Manager to handle Host Stack interactions */
    BleConnManager_GapPeripheralEvent(peerDeviceId, pConnectionEvent);

    switch (pConnectionEvent->eventType)
    {
    case gConnEvtConnected_c:
        {
            mPeerInformation[peerDeviceId].deviceId = peerDeviceId;
            Serial_Print(gAppSerMgrIf,"Connected with peerDeviceId = 0x",gNoBlock_d);
            Serial_PrintHex(gAppSerMgrIf, &peerDeviceId, 1, gNoBlock_d);
            Serial_Print(gAppSerMgrIf, "\r\n", gNoBlock_d);
            /* Advertising stops when connected */
            mAdvState.advOn = FALSE;

            /* Subscribe client*/
            Bas_Subscribe(&basServiceConfig, peerDeviceId);
            Qpp_Subscribe(peerDeviceId);
			ECig_Subscribe(peerDeviceId);
			
            /* UI */
            //LED_StopFlashingAllLeds();//modify by wzy
            //Led1On();//modify by wzy
			
			CTIMER_StopTimer(CTIMER3);//modify by wzy
			TEST_LED_Pin_Init();//modify by wzy
			
            /* Stop Advertising Timer*/
            mAdvState.advOn = FALSE;
            TMR_StopTimer(mAdvTimerId);

            /* Start battery measurements */
            //if(!TMR_IsTimerActive(mBatteryMeasurementTimerId))//modify by wzy
            {
                TMR_StartLowPowerTimer(mBatteryMeasurementTimerId, gTmrLowPowerIntervalMillisTimer_c,
                                       TmrSeconds(mBatteryLevelReportInterval_c), BatteryMeasurementTimerCallback, NULL);
            }
			
            //if(!TMR_IsTimerActive(mQppsThroughputStatisticsTimerId))//modify by wzy
            //{
            //    TMR_StartLowPowerTimer(mQppsThroughputStatisticsTimerId, gTmrLowPowerIntervalMillisTimer_c,
            //                           mQppsThroughputStatisticsInterval_c, QppsThoughputStatisticsTimerCallback, NULL);
            //}
        }
        break;

    case gConnEvtDisconnected_c:
        {
			Serial_Print(gAppSerMgrIf, "\r\n Disconnected from device ", gAllowToBlock_d);//modify by wzy
            Serial_PrintDec(gAppSerMgrIf, peerDeviceId);//modify by wzy
            Serial_Print(gAppSerMgrIf, ".\n\r", gAllowToBlock_d);//modify by wzy

            /* Unsubscribe client */
            Bas_Unsubscribe(&basServiceConfig, peerDeviceId);
            /* qpps Unsubscribe client */
            Qpp_Unsubscribe();
			ECig_Unsubscribe();
            mPeerInformation[peerDeviceId].bytsReceivedPerInterval = 0;
            mPeerInformation[peerDeviceId].bytsSentPerInterval = 0;
            mPeerInformation[peerDeviceId].ntf_cfg = QPPS_VALUE_NTF_OFF;
            mPeerInformation[peerDeviceId].deviceId = gInvalidDeviceId_c;

			CTIMER_StopTimer(CTIMER3);//modify by wzy
			TEST_LED_Pin_Init();//modify by wzy

            for (uint8_t i = 0; i < gAppMaxConnections_c; i++)
            {
                if(mPeerInformation[i].deviceId != gInvalidDeviceId_c)
                    break;
                if(i==(gAppMaxConnections_c-1))
                {
                    TMR_StopTimer(mBatteryMeasurementTimerId);
                    //TMR_StopTimer(mQppsThroughputStatisticsTimerId);//modify by wzy
                }
            }

            if (pConnectionEvent->eventData.disconnectedEvent.reason == gHciConnectionTimeout_c)
            {
                /* Link loss detected*/
                BleApp_Start();
            }
            else
            {
                /* Connection was terminated by peer or application */
                BleApp_Start();
            }
        }
        break;
    default:
        break;
    }
}

/*! *********************************************************************************
* \brief        Handles GATT server callback from host stack.
*
* \param[in]    deviceId        Peer device ID.
* \param[in]    pServerEvent    Pointer to gattServerEvent_t.
********************************************************************************** */
extern volatile unsigned char Lock_Unlock_byBLE_Flag;
extern volatile float  SmokeOutput_MAX_Power;

enum ecig_lock_or_unlock
{
	invalid=0,
	lock=1,
	unlock=2
};	

static void BleApp_GattServerCallback (deviceId_t deviceId, gattServerEvent_t* pServerEvent)
{
    uint16_t handle;
    uint8_t status;
    uint8_t notifMaxPayload = 0;
	
	uint8_t   power[3]  ={0};
	float power_rx =0;
	
    switch (pServerEvent->eventType)
    {
    case gEvtMtuChanged_c:
        notifMaxPayload = pServerEvent->eventData.mtuChangedEvent.newMtu - 3;
        if( notifMaxPayload <= mQppsMaxTestDataLength_c )
        {
            mQppsTestDataLength = notifMaxPayload;
        }
        break;
     
    case gEvtAttributeWritten_c:
        {
            handle = pServerEvent->eventData.attributeWrittenEvent.handle;
            status = gAttErrCodeNoError_c;
			
			if (handle == value_locking_rx)//开发板  read  //modify by wzy  收到锁定信号
            {
                //BleApp_ReceivedDataHandler(deviceId, pServerEvent->eventData.attributeWrittenEvent.aValue, pServerEvent->eventData.attributeWrittenEvent.cValueLength);
				
				if((memcmp((char *)(pServerEvent->eventData.attributeWrittenEvent.aValue),"lock",pServerEvent->eventData.attributeWrittenEvent.cValueLength))==0)
				{
					Lock_Unlock_byBLE_Flag = lock;	//modify by wzy
				}
				
				if((memcmp((char *)(pServerEvent->eventData.attributeWrittenEvent.aValue),"unlock",pServerEvent->eventData.attributeWrittenEvent.cValueLength))==0)
				{
					Lock_Unlock_byBLE_Flag = unlock;	//modify by wzy
				}	
			 }	
			
			if (handle == value_ecig_command_rx)//开发板接收到了事件,事件的类型为带回应的可写事件
			{
				if((memcmp((char *)(pServerEvent->eventData.attributeWrittenEvent.aValue),"A",pServerEvent->eventData.attributeWrittenEvent.cValueLength))==0)
				{
						//LED_StopFlashingAllLeds();//modify by wzy
						//Led3Flashing();//modify by wzy
				}
				if((memcmp((char *)(pServerEvent->eventData.attributeWrittenEvent.aValue),"B",pServerEvent->eventData.attributeWrittenEvent.cValueLength))==0)
				{
						//LED_StopFlashingAllLeds();//modify by wzy
				}				
			}
			
			if (handle == value_setgetsmokepower_tx)//开发板接收到了事件,事件的类型为带回应的可写事件
			{
				memcpy(power,(char *)(pServerEvent->eventData.attributeWrittenEvent.aValue),3);
				
				//power_rx = (float)((((unsigned short int)power[0])<<8) + power[1]) / 100;
				power_rx = (float)(power[0] - 0x30) +(float)(power[2] - 0x30) * 0.1f;
				
				if(power_rx > 8.0f)
					power_rx =8.0f;
				else if(power_rx < 4.0f)
					power_rx =4.0f;
				else
				{
					SmokeOutput_MAX_Power = power_rx;
					
					uint8_t   _power[2]  ={0};
					_power[0] = (uint8_t)((unsigned short int)(SmokeOutput_MAX_Power * 100 ) >> 8);
					_power[1] = (uint8_t)((unsigned short int)(SmokeOutput_MAX_Power * 100 ) & 0xff);
					
					ReadSmokePower (0, service_qpps, _power);	
				}
			}
			
            GattServer_SendAttributeWrittenStatus(deviceId, handle, status);
        }
        break;

    case gEvtAttributeWrittenWithoutResponse_c:
        {
            handle = pServerEvent->eventData.attributeWrittenEvent.handle;

            if (handle == value_qpps_rx)
            {
                BleApp_ReceivedDataHandler(deviceId, pServerEvent->eventData.attributeWrittenEvent.aValue, \
											pServerEvent->eventData.attributeWrittenEvent.cValueLength);
            }
			
			if (handle == value_ecig_data_rx)//开发板接收到了事件
            {
				
            }
        }
        break;

    case gEvtCharacteristicCccdWritten_c:
        {
            handle = pServerEvent->eventData.charCccdWrittenEvent.handle;
            if ((handle == cccd_qpps_tx) || (handle ==cccd_dsp310_tx) ||(handle ==cccd_energy_tx) \
				||(handle ==cccd_ecig_data_tx)||(handle ==cccd_ecig_command_tx)||(handle ==cccd_power_tx)||(handle ==cccd_workmode_tx))//modify by wzy
            {
				pServerEvent->eventData.charCccdWrittenEvent.newCccd = gCccdNotification_c;
                mPeerInformation[deviceId].ntf_cfg = pServerEvent->eventData.charCccdWrittenEvent.newCccd;
            }
        }
        break;

    default:
        break;
    }
}

/*! *********************************************************************************
* \brief        user tx api.
* \param[in]    pParam        call parameters.
*    			modify by wzy
********************************************************************************** */
char *pMsg=NULL;//modify by wzy

static void user_serial_printf(void * pParam)//modify by wzy 将手机送来的信息串口打印
{
	Serial_Print(gAppSerMgrIf, (char *)pParam, gAllowToBlock_d);
	/* Free Buffer */
    MEM_BufferFree(pMsg);
}

static void USER_serial_tx(void * pParam)//modify by wzy//将手机送来的信息放入队列
{
    App_PostCallbackMessage(user_serial_printf,pParam);
}

static void BleApp_ReceivedDataHandler//收到手机发送过来的信息
(
    deviceId_t deviceId,
    uint8_t*    aValue,
    uint16_t    valueLength
)
{
    mPeerInformation[deviceId].bytsReceivedPerInterval += valueLength;
	
	pMsg = MEM_BufferAlloc(valueLength);//modify by wzy
	
	if (pMsg == NULL)//modify by wzy
    	return;

	memcpy(pMsg,aValue,valueLength);//modify by wzy
	
	USER_serial_tx((char *)pMsg);//modify by wzy
}

//将数据发送到蓝牙////////////////////////////////////////////////////////////////////////
static void BLE_SendDataToAir(void * pParam)//modify by wzy
{
      char *tx_data = NULL;
      uint8_t i;
      bleResult_t result;

      for (i = 0; i < gAppMaxConnections_c; i++)
      {
          if ((mPeerInformation[i].deviceId != gInvalidDeviceId_c) && (mPeerInformation[i].ntf_cfg == QPPS_VALUE_NTF_ON))
          {
			  tx_data = MEM_BufferAlloc(strlen((char *)pParam));
			  
			  if(tx_data ==NULL)
				  return;
			  
			  memcpy(tx_data,(char *)pParam,strlen((char *)pParam) );
			  
              //result = Qpp_SendData(mPeerInformation[i].deviceId, service_qpps, mQppsTestDataLength, (uint8_t *)tx_data);
			  result = Qpp_SendData(mPeerInformation[i].deviceId, service_qpps, strlen((char *)pParam), (uint8_t *)tx_data);

              if(result == gBleSuccess_c)
              {
                  //mPeerInformation[i].bytsSentPerInterval += mQppsTestDataLength;
				  mPeerInformation[i].bytsSentPerInterval += strlen((char *)pParam);
              }
              else if (result == gBleOverflow_c)
              {
                  /* Tx overflow. Stop Tx and restart when gTxEntryAvailable_c event is received. */
                  mPeerInformation[i].ntf_cfg = QPPS_VALUE_NTF_OFF;
              }
			  
				/* Free Buffer */
				MEM_BufferFree(tx_data);
          }
      }
}

void USER_SendDateToAir(void *pData)//modify by wzy
{
	App_PostCallbackMessage(BLE_SendDataToAir, pData);
}

///将数据发送到串口输出打印////////////////////////////////////////////////////////
static void TxSerial_Printf(void * pParam)//中是用于串口的打印输出
{
   Serial_Print(gAppSerMgrIf, (char*)pParam, gAllowToBlock_d);
}

void USER_Serial_Printf(void * pParam)//modify by wzy
{
    App_PostCallbackMessage(TxSerial_Printf, pParam);
}

/*! *********************************************************************************
* \brief        user command callback.
*
* \param[in]    pParam        Calback parameters.  //modify by wzy
********************************************************************************** */
static void DPS310_Data_TxCallback(void * pParam)
{
	char *tx_data = NULL;
	uint8_t i;
	bleResult_t result;

	for (i = 0; i < gAppMaxConnections_c; i++)
	{
		if ((mPeerInformation[i].deviceId != gInvalidDeviceId_c) && (mPeerInformation[i].ntf_cfg == QPPS_VALUE_NTF_ON))
		{
			tx_data = MEM_BufferAlloc(strlen((char *)pParam));

			if(tx_data ==NULL)
			  return;

			memcpy(tx_data,(char *)pParam,strlen((char *)pParam) );

			result = USER_DPS310_SendData(mPeerInformation[i].deviceId, service_qpps, strlen((char *)pParam), (uint8_t *)tx_data);

			if(result == gBleSuccess_c)
			{
			  mPeerInformation[i].bytsSentPerInterval += mQppsTestDataLength;
			}
			else if (result == gBleOverflow_c)
			{
			  /* Tx overflow. Stop Tx and restart when gTxEntryAvailable_c event is received. */
			  mPeerInformation[i].ntf_cfg = QPPS_VALUE_NTF_OFF;
			}

			/* Free Buffer */
			MEM_BufferFree(tx_data);
		}
	}
}

void DPS310SentDataToAir(void* pParam)//modify by wzy
{
	App_PostCallbackMessage(DPS310_Data_TxCallback, pParam);
}

/*! *********************************************************************************
* \brief        user command callback.
*
* \param[in]    pParam        Calback parameters.  //modify by wzy
********************************************************************************** */
static void WorkMode_TxCallback(void * pParam)
{
      uint8_t tx_data = 0;
      uint8_t i;
      bleResult_t result;

      for (i = 0; i < gAppMaxConnections_c; i++)
      {
          if ((mPeerInformation[i].deviceId != gInvalidDeviceId_c) && (mPeerInformation[i].ntf_cfg == QPPS_VALUE_NTF_ON))
          {
			  
			  tx_data = (uint8_t)pParam ;
			  result = WorkMode_SendData (mPeerInformation[i].deviceId, service_qpps, tx_data);

              if(result == gBleSuccess_c)
              {
                  mPeerInformation[i].bytsSentPerInterval += mQppsTestDataLength;
              }
              else if (result == gBleOverflow_c)
              {
                  /* Tx overflow. Stop Tx and restart when gTxEntryAvailable_c event is received. */
                  mPeerInformation[i].ntf_cfg = QPPS_VALUE_NTF_OFF;
              }
          }
      }
}

void WorkMode_SendToAir(void* pParam)//modify by wzy
{
	App_PostCallbackMessage(WorkMode_TxCallback, pParam);
}

/*! *********************************************************************************
* \brief        抽烟功率发送
*
* \param[in]    pParam        Calback parameters.  //modify by wzy
********************************************************************************** */
static void SmokePower_TxCallback(void * pParam)
{
      char *tx_data = NULL;
      uint8_t i;
      bleResult_t result;

      for (i = 0; i < gAppMaxConnections_c; i++)
      {
          if ((mPeerInformation[i].deviceId != gInvalidDeviceId_c) && (mPeerInformation[i].ntf_cfg == QPPS_VALUE_NTF_ON))
          {
			  tx_data = MEM_BufferAlloc(strlen((char *)pParam));
			  
			  if(tx_data ==NULL)
				  return;
			  
			  memcpy(tx_data,(char *)pParam,strlen((char *)pParam) );
			  
              result = SmokePower_SendData(mPeerInformation[i].deviceId, service_qpps, strlen((char *)pParam), (uint8_t *)tx_data);

              if(result == gBleSuccess_c)
              {
                  mPeerInformation[i].bytsSentPerInterval += mQppsTestDataLength;
              }
              else if (result == gBleOverflow_c)
              {
                  /* Tx overflow. Stop Tx and restart when gTxEntryAvailable_c event is received. */
                  mPeerInformation[i].ntf_cfg = QPPS_VALUE_NTF_OFF;
              }
				
			  MEM_BufferFree(tx_data);/* Free Buffer */
          }
      }
}

void SmokePower_DataToAir(void* pParam)//modify by wzy
{
	App_PostCallbackMessage(SmokePower_TxCallback, pParam);
}

/*! *********************************************************************************
* \brief        抽烟功率发送
*
* \param[in]    pParam        Calback parameters.  //modify by wzy
********************************************************************************** */
static void SmokeEnergy_TxCallback(void * pParam)
{
      char *tx_data = NULL;
      uint8_t i;
      bleResult_t result;

      for (i = 0; i < gAppMaxConnections_c; i++)
      {
          if ((mPeerInformation[i].deviceId != gInvalidDeviceId_c) && (mPeerInformation[i].ntf_cfg == QPPS_VALUE_NTF_ON))
          {
			  tx_data = MEM_BufferAlloc(strlen((char *)pParam));
			  
			  if(tx_data ==NULL)
				  return;
			  
			  memcpy(tx_data,(char *)pParam,strlen((char *)pParam) );
			  
              result = SmokeEnergy_SendData(mPeerInformation[i].deviceId, service_qpps, strlen((char *)pParam), (uint8_t *)tx_data);

              if(result == gBleSuccess_c)
              {
                  mPeerInformation[i].bytsSentPerInterval += mQppsTestDataLength;
              }
              else if (result == gBleOverflow_c)
              {
                  /* Tx overflow. Stop Tx and restart when gTxEntryAvailable_c event is received. */
                  mPeerInformation[i].ntf_cfg = QPPS_VALUE_NTF_OFF;
              }
				
			  MEM_BufferFree(tx_data);/* Free Buffer */
          }
      }
}

void SmokeEnergy_DataToAir(void* pParam)//modify by wzy
{
	App_PostCallbackMessage(SmokeEnergy_TxCallback, pParam);
}


/*! *********************************************************************************
* \brief        Handles QPP tx timer callback.
*
* \param[in]    pParam        Calback parameters.
********************************************************************************** */
//#if mQppsTxInterval_c
//static void QppsTxTimerCallback(void * pParam)
//{
//    App_PostCallbackMessage(QppsTxCallback, NULL);
//}
//#endif

/*! *********************************************************************************
* \brief        QPP tx callback.
*
* \param[in]    pParam        Calback parameters.
********************************************************************************** */
//static void QppsTxCallback(void * pParam)			//modify by wzy
//{
//      static uint8_t tx_data[mQppsMaxTestDataLength_c];
//      static uint8_t index = 0;
//      uint8_t i;
//      bleResult_t result;
//      uint8_t txCnt = 0;

//      for(i = 1; i<mQppsTestDataLength; i++)
//      {
//          tx_data[i] = i;
//      }
//      tx_data[0] = index;

//      for (i = 0; i < gAppMaxConnections_c; i++)
//      {
//          if ((mPeerInformation[i].deviceId != gInvalidDeviceId_c) && (mPeerInformation[i].ntf_cfg == QPPS_VALUE_NTF_ON))
//          {
//              result = Qpp_SendData(mPeerInformation[i].deviceId, service_qpps, mQppsTestDataLength, tx_data);

//              if(result == gBleSuccess_c)
//              {
//                  mPeerInformation[i].bytsSentPerInterval += mQppsTestDataLength;
//                  txCnt++;
//              }
//              else if (result == gBleOverflow_c)
//              {
//                  /* Tx overflow. Stop Tx and restart when gTxEntryAvailable_c event is received. */
//                  mPeerInformation[i].ntf_cfg = QPPS_VALUE_NTF_OFF;
//              }
//          }
//      }
//      
//      if (txCnt > 0)
//      {
//          index++;
//      }
//}

/*! *********************************************************************************
* \brief        Handles QPPS Thoughput timer callback.
*
* \param[in]    pParam        Calback parameters.
********************************************************************************** */
//static void QppsThoughputStatisticsTimerCallback(void* pParam)   //modify by wzy
//{
//    uint8_t i;
//    bool_t print_stats = FALSE;
//    mTxInfo.TakenSeconds = mQppsThroughputStatisticsInterval_c/1000;

//    for (i = 0; i < gAppMaxConnections_c; i++)
//    {
//        if(mPeerInformation[i].deviceId != gInvalidDeviceId_c)
//        {
//            mTxInfo.RxSpeed[i] = (mPeerInformation[i].bytsReceivedPerInterval / mTxInfo.TakenSeconds) * 8;
//            mTxInfo.TxSpeed[i] = (mPeerInformation[i].bytsSentPerInterval / mTxInfo.TakenSeconds) * 8;
//            mPeerInformation[i].bytsReceivedPerInterval = 0;
//            mPeerInformation[i].bytsSentPerInterval = 0;
//            print_stats = TRUE;
//        }
//    }
//    
//    if (print_stats)
//    {
//        App_PostCallbackMessage(TxPrintCallback, NULL);
//    }
//}

//static void TxPrintCallback(void * pParam)  //modify by wzy
//{
//    uint8_t i;

//    for (i = 0; i < gAppMaxConnections_c; i++)
//    {
//        if(mPeerInformation[i].deviceId != gInvalidDeviceId_c)
//        {
//            //sprintf((char*)printBuffer, "\r\n-->QPP server, deviceId = 0x%x,RX speed = %d bps,TX speed = %d bps.\r\n ",mPeerInformation[i].deviceId,mTxInfo.RxSpeed[i],mTxInfo.TxSpeed[i]);
//            //Serial_Print(gAppSerMgrIf, (char*)printBuffer, gAllowToBlock_d);
//        }
//    }
//}

/*! *********************************************************************************
* \brief        Handles battery measurement timer callback.
*
* \param[in]    pParam        Calback parameters.
********************************************************************************** */
static void BatteryMeasurementTimerCallback(void * pParam)
{
    //basServiceConfig.batteryLevel = BOARD_GetBatteryLevel();
	basServiceConfig.batteryLevel = USER_GetBatteryLevel();
    Bas_RecordBatteryMeasurement(&basServiceConfig);
}

/*! *********************************************************************************
* @}
********************************************************************************** */
