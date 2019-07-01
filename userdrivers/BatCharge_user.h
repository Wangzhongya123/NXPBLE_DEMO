#ifndef __BatCharge_user_H
#define	__BatCharge_user_H

#include "fsl_common.h"
#include "pin_mux.h"
#include "fsl_iocon.h"
#include "pin_mux.h"
#include "fsl_iocon.h"
#include "fsl_gpio.h"
#include "fsl_power.h"

#include "fsl_bod.h"

#define PORT_NTCVCC_IDX    		0
#define NTCVCC_GPIO_PIN 		10u

#define PORT_CHARGE_IDX    		0
#define CHARGE_EN_PIN 			30u
#define CHARGESTART_PIN 		31u

#define CurrentChoice_1_PIN 	25u
#define CurrentChoice_2_PIN 	27u

#define BATTERY_FULL_VOLTAGE   4.2f /* milliVolts */
#define BATTERY_MIN_VOLTAGE    2.8f /* milliVolts */

#define USER_SW1_GPIO_PIN_MASK 		(1U << USER_SW1_GPIO_PIN)
#define USER_CHARGE_GPIO_PIN_MASK 	(1U << CHARGE_DET_PIN)

void NTCPin_Init(void);
void NTC_EN(void);
void NTC_DIS(void);
float NTC_Value(void);

void Charge_Init(void);
void Charge_EN(void);
void Charge_DIS(void);

void ChargeIn_Pin_Init(void);//配置中断时使用

void ChargeStart_Init(void);
unsigned char ChargeStart_Status(void);

void CurrentChoice_Pin_Init(void);
void CurrentChoice_MIN(void);
void CurrentChoice_MAX(void);
void CurrentChoice_MID(void);

void BOD_init(void);

#endif /* __BatCharge_user_H */
