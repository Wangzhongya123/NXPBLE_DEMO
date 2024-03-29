/*! *********************************************************************************
* \addtogroup Private Profile Service
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright (c) 2014, Freescale Semiconductor, Inc.
* All rights reserved.
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "ble_general.h"
#include "gatt_db_app_interface.h"
#include "gatt_server_interface.h"
#include "gap_interface.h"
#include "private_profile_interface.h"
#include "SerialManager.h"
#include "gatt_db_handles.h"
/************************************************************************************
*************************************************************************************
* Private constants & macros
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/

/*! Battery Service - Subscribed Client*/
deviceId_t mQpp_SubscribedClientId;

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
extern volatile float  SmokeOutput_MAX_Power;

bleResult_t Qpp_Start (qppsConfig_t *pServiceConfig)
{
	bleResult_t result;
	uint8_t   power[2]  ={0};
	power[0] = (uint8_t)((unsigned short int)(SmokeOutput_MAX_Power * 100 ) >> 8);
	power[1] = (uint8_t)((unsigned short int)(SmokeOutput_MAX_Power * 100 ) & 0xff);
	
	ReadSmokePower(0, service_qpps,power);
	
    return gBleSuccess_c;
}

bleResult_t Qpp_Stop (qppsConfig_t *pServiceConfig)
{
    mQpp_SubscribedClientId = gInvalidDeviceId_c;
    return gBleSuccess_c;
}

bleResult_t Qpp_Subscribe(deviceId_t clientDeviceId)
{
    mQpp_SubscribedClientId = clientDeviceId;
    return gBleSuccess_c;
}

bleResult_t Qpp_Unsubscribe()
{
    mQpp_SubscribedClientId = gInvalidDeviceId_c;
    return gBleSuccess_c;
}

/************************************************************************************
	通用数据发送
************************************************************************************/
bleResult_t Qpp_SendData (uint8_t deviceId, uint16_t serviceHandle,uint16_t length, uint8_t *testData)
{
    uint16_t  handle;
    bleResult_t result;
    uint16_t  handleCccd;
    bool_t isNotifActive;

    bleUuid_t uuid;
    FLib_MemCpy(uuid.uuid128, uuid_qpps_characteristics_tx, 16);

    /* Get handle of  characteristic */
    result = GattDb_FindCharValueHandleInService(serviceHandle, gBleUuidType128_c, &uuid, &handle);
    if (result != gBleSuccess_c)
        return result;
        /* Get handle of CCCD */
    if ((result = GattDb_FindCccdHandleForCharValueHandle(handle, &handleCccd)) != gBleSuccess_c)
        return result;

    result = Gap_CheckNotificationStatus(deviceId, handleCccd, &isNotifActive);
    if ((gBleSuccess_c == result) && (TRUE == isNotifActive))
        result = GattServer_SendInstantValueNotification(deviceId, handle, length, testData);

    return result;
}

/************************************************************************************
	310发送的数据
************************************************************************************/
bleResult_t USER_DPS310_SendData (uint8_t deviceId, uint16_t serviceHandle,uint16_t length, uint8_t *testData)//modify by wzy 
{
    uint16_t  handle;
    bleResult_t result;
    uint16_t  handleCccd;
    bool_t isNotifActive;

    bleUuid_t uuid;
    FLib_MemCpy(uuid.uuid128, uuid_dps310_characteristics_tx, 16);

    /* Get handle of  characteristic */
    result = GattDb_FindCharValueHandleInService(serviceHandle, gBleUuidType128_c, &uuid, &handle);
    if (result != gBleSuccess_c)
        return result;
        /* Get handle of CCCD */
    if ((result = GattDb_FindCccdHandleForCharValueHandle(handle, &handleCccd)) != gBleSuccess_c)
        return result;

    result = Gap_CheckNotificationStatus(deviceId, handleCccd, &isNotifActive);
    if ((gBleSuccess_c == result) && (TRUE == isNotifActive))
        result = GattServer_SendInstantValueNotification(deviceId, handle, length, testData);

    return result;
}

/************************************************************************************
	当前使用的能量
************************************************************************************/
bleResult_t SmokeEnergy_SendData (uint8_t deviceId, uint16_t serviceHandle,uint16_t length, uint8_t *testData)//modify by wzy 发送抽烟使用的能量
{
    uint16_t  handle;
    bleResult_t result;
    uint16_t  handleCccd;
    bool_t isNotifActive;

    bleUuid_t uuid;
    FLib_MemCpy(uuid.uuid128, uuid_energy_characteristics_tx, 16);

    /* Get handle of  characteristic */
    result = GattDb_FindCharValueHandleInService(serviceHandle, gBleUuidType128_c, &uuid, &handle);
    if (result != gBleSuccess_c)
        return result;
        /* Get handle of CCCD */
    if ((result = GattDb_FindCccdHandleForCharValueHandle(handle, &handleCccd)) != gBleSuccess_c)
        return result;

    result = Gap_CheckNotificationStatus(deviceId, handleCccd, &isNotifActive);
    if ((gBleSuccess_c == result) && (TRUE == isNotifActive))
        result = GattServer_SendInstantValueNotification(deviceId, handle, length, testData);

    return result;
}

/************************************************************************************
	当前抽烟功率
************************************************************************************/
bleResult_t SmokePower_SendData (uint8_t deviceId, uint16_t serviceHandle,uint16_t length, uint8_t *testData)//modify by wzy 发送抽烟时的发热丝的功率
{
    uint16_t  handle;
    bleResult_t result;
    uint16_t  handleCccd;
    bool_t isNotifActive;

    bleUuid_t uuid;
    FLib_MemCpy(uuid.uuid128, uuid_power_characteristics_tx, 16);

    /* Get handle of  characteristic */
    result = GattDb_FindCharValueHandleInService(serviceHandle, gBleUuidType128_c, &uuid, &handle);
    if (result != gBleSuccess_c)
        return result;
        /* Get handle of CCCD */
    if ((result = GattDb_FindCccdHandleForCharValueHandle(handle, &handleCccd)) != gBleSuccess_c)
        return result;

    result = Gap_CheckNotificationStatus(deviceId, handleCccd, &isNotifActive);
    if ((gBleSuccess_c == result) && (TRUE == isNotifActive))
        result = GattServer_SendInstantValueNotification(deviceId, handle, length, testData);

    return result;
}

/************************************************************************************
	发送当前工作状态
************************************************************************************/
bleResult_t WorkMode_SendData (uint8_t deviceId, uint16_t serviceHandle, uint8_t mode)
{
    uint16_t  handle;
    bleResult_t result;
	
    bleUuid_t uuid;
    FLib_MemCpy(uuid.uuid128, uuid_workmode_characteristics_tx, 16);

    /* Get handle of  characteristic */
    result = GattDb_FindCharValueHandleInService(serviceHandle,gBleUuidType128_c, &uuid, &handle);
    if (result != gBleSuccess_c)
        return result;

    /* Update characteristic value and send notification */
    result = GattDb_WriteAttribute(handle, sizeof(uint8_t), &mode);

    if (result != gBleSuccess_c)
        return result;

    WorkMode_SendNotifications(deviceId,handle);

    return gBleSuccess_c;
}

static void WorkMode_SendNotifications(uint8_t deviceId,uint16_t handle)
{
    uint16_t  handleCccd;
    bool_t    isNotifActive;
	bleResult_t result;
	
    /* Get handle of CCCD */
    if (GattDb_FindCccdHandleForCharValueHandle(handle, &handleCccd) != gBleSuccess_c)
    {
        return;
    }
	
    result = Gap_CheckNotificationStatus(deviceId, handleCccd, &isNotifActive);
    if ((gBleSuccess_c == result) && (TRUE == isNotifActive))
        GattServer_SendNotification(deviceId, handle);
}

/************************************************************************************
	用于APP读取当前的抽烟功率设置
************************************************************************************/
bleResult_t ReadSmokePower(uint8_t deviceId, uint16_t serviceHandle, uint8_t * power)
{
    uint16_t  handle;
    bleResult_t result;
	
    bleUuid_t uuid;
    FLib_MemCpy(uuid.uuid128, uuid_setgetsmokepower_chara_rxtx, 16);

    /* Get handle of  characteristic */
    result = GattDb_FindCharValueHandleInService(serviceHandle,gBleUuidType128_c, &uuid, &handle);
    if (result != gBleSuccess_c)
        return result;

    /* Update characteristic value and send notification */
    result = GattDb_WriteAttribute(handle, sizeof(uint16_t), power);

    return gBleSuccess_c;
}
