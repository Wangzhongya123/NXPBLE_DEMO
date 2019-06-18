#ifndef _USERAPP_H_
#define _USERAPP_H_

/*************************************************************************************
**************************************************************************************
* Public macros
**************************************************************************************
*************************************************************************************/

/* Parameters */
#define SENDDATA_NUMBER  30   /*传送的字符串的长度*/
#define RECEIVEDATA_NUMBER  30   /*传送的字符串的长度*/


extern char Send_Str[SENDDATA_NUMBER];
extern char DPS310_Send_Str[SENDDATA_NUMBER];
extern char Receive_Str[RECEIVEDATA_NUMBER];

extern void USER_SendDateToAir(void *pData);
extern void USER_Serial_Printf(void * pParam);//modify by wzy
extern void DPS310SentDataToAir(void* pParam);//modify by wzy
extern void WorkMode_SendToAir(void* pParam);//modify by wzy
extern void ble_task(void);
extern void ble_task_init(void);

#endif /* _APP_H_ */
