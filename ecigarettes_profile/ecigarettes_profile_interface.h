/*! *********************************************************************************
* \defgroup Private profile Service
* @{
********************************************************************************** */
/*! *********************************************************************************
* Copyright (c) 2014, Freescale Semiconductor, Inc.
* All rights reserved.
*
* \file
*
* This file is the interface file for the WZY Service
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef _ECIGARETTES_PROFILE_INTERFACE_H_
#define _ECIGARETTES_PROFILE_INTERFACE_H_

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public constants & macros
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

/*! wzy Service - Configuration */
typedef struct ecigarettesConfig_tag
{
    uint16_t    serviceHandle;
} ecigarettesConfig_t;


typedef struct wzy_tmcConfig_tag
{
    uint16_t    hService;
    uint16_t    hTxData;
    uint16_t    hTxCccd;
    uint16_t    hRxData;
} wzyConfig_t;

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*!**********************************************************************************
* \brief        Starts ECig Service functionality
*
* \param[in]    pServiceConfig  Pointer to structure that contains server
*                               configuration information.
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t ECig_Start(ecigarettesConfig_t *pServiceConfig);

/*!**********************************************************************************
* \brief        Stops ECig Service functionality
*
* \param[in]    pServiceConfig  Pointer to structure that contains server
*                               configuration information.
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t ECig_Stop(ecigarettesConfig_t *pServiceConfig);

/*!**********************************************************************************
* \brief        Subscribes a GATT client to the QPP service
*
* \param[in]    pClient  Client Id in Device DB.
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t ECig_Subscribe(deviceId_t clientDeviceId);

/*!**********************************************************************************
* \brief        Unsubscribes a GATT client from the ECig service
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t ECig_Unsubscribe(void);

/*!**********************************************************************************
* \brief        ECig SendData to   APP.
*
* \param[in]    deviceId        Peer device ID.
* \param[in]    serviceHandle   Service handle.
* \param[in]    length          Length of TestData to send .
* \param[in]    testData        TestData to send .
*
* \return       gBleSuccess_c or error.
************************************************************************************/
//
bleResult_t ECig_SendData (uint8_t deviceId, uint16_t serviceHandle, uint16_t length, uint8_t *testData);
//
bleResult_t ECig_SendCommand (uint8_t deviceId, uint16_t serviceHandle,uint16_t length, uint8_t *testData);



#ifdef __cplusplus
}
#endif

#endif /* _ECIGARETTES_PROFILE_INTERFACE_H_ */

/*! *********************************************************************************
* @}
********************************************************************************** */
