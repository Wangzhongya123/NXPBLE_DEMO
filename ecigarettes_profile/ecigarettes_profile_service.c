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
#include "ecigarettes_profile_interface.h"
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

/*!wzy Service - Subscribed Client*/
deviceId_t ecig_SubscribedClientId;

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

bleResult_t ECig_Start (ecigarettesConfig_t *pServiceConfig)
{

    return gBleSuccess_c;
}

bleResult_t ECig_Stop (ecigarettesConfig_t *pServiceConfig)
{
    ecig_SubscribedClientId = gInvalidDeviceId_c;
    return gBleSuccess_c;
}

bleResult_t ECig_Subscribe(deviceId_t clientDeviceId)
{
    ecig_SubscribedClientId = clientDeviceId;
    return gBleSuccess_c;
}

bleResult_t ECig_Unsubscribe()
{
    ecig_SubscribedClientId = gInvalidDeviceId_c;
    return gBleSuccess_c;
}

bleResult_t ECig_SendData (uint8_t deviceId, uint16_t serviceHandle,uint16_t length, uint8_t *testData)
{
    uint16_t  handle;
    bleResult_t result;
    uint16_t  handleCccd;
    bool_t isNotifActive;

    bleUuid_t uuid;
    FLib_MemCpy(uuid.uuid128, uuid_ecig_data_characteristics_tx, 16);

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

bleResult_t ECig_SendCommand (uint8_t deviceId, uint16_t serviceHandle,uint16_t length, uint8_t *testData)//modify by wzy 
{
    uint16_t  handle;
    bleResult_t result;
    uint16_t  handleCccd;
    bool_t isNotifActive;

    bleUuid_t uuid;
    FLib_MemCpy(uuid.uuid128, uuid_ecig_command_characteristics_tx, 16);

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
