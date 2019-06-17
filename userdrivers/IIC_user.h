#ifndef __IIC_user_H
#define	__IIC_user_H

#include "fsl_iocon.h"
#include "pin_mux.h"
#include "fsl_inputmux.h"
#include "fsl_common.h"
#include "gpio_pins.h"
#include "QN908XC.h"
#include "fsl_gpio.h"

#define PIN_CLK       6u   /*!< Pin number for pin 6 in a port */
#define PIN_SDA       7u   /*!< Pin number for pin 7 in a port */
#define PORTA_IDX     0   /*!< Port index */
#define DPS310_PORT	  PORTA_IDX
#define DPS310_GPIO	  GPIOA

#define read_SDA_DPS310	 GPIO_ReadPinInput(DPS310_GPIO , PIN_SDA)  	// SDA

#define set_SDA_DPS310	 GPIO_WritePinOutput(DPS310_GPIO, PIN_SDA , 1)	// SDA
#define clr_SDA_DPS310	 GPIO_WritePinOutput(DPS310_GPIO, PIN_SDA , 0)	// SDA

#define set_SCL_DPS310 	 GPIO_WritePinOutput(DPS310_GPIO, PIN_CLK , 1)// SCL
#define clr_SCL_DPS310	 GPIO_WritePinOutput(DPS310_GPIO, PIN_CLK , 0)// SCL


void delay_us(unsigned int nus);

void IIC_IO_Confing(void);
void DPS310_SDA_IN(void);
void DPS310_SDA_OUT(void);

void Start_I2c_DPS310(void);
void Stop_I2c_DPS310(void);
void Ack_I2c_DPS310(unsigned char a); 
void  SendByte_DPS310(unsigned char  dat);	
unsigned char RcvByte_DPS310(void);
unsigned char IRcvStr_DPS310(unsigned char  sla,unsigned char  *s,unsigned char  no);

short i2c_read_block(unsigned char  address,unsigned char length,unsigned char  *read_buffer);
short i2c_write_byte(unsigned char address,unsigned char  data);
short i2c_read_byte(unsigned char address);

#endif /* __IIC_user_H */
