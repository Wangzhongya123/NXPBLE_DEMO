/*
 * Copyright (c) 2015-2016 Infineon Technologies AG
 *
 * Driver for Infineon DPS310 Digital Barometric Pressure Sensor
 *
 *
 */


#ifndef DPS310_H_INCLUDED
#define DPS310_H_INCLUDED

/* Attributes: Product identification and version */
/*��Ʒ���ܺͰ汾��Ϣ*/

#define     VENDOR_NAME                                 "Infineon"
#define     DRIVER_NAME                                 "IFXDD"
#define     DEVICE_NAME                                 "Digital Barometric Pressure Sensor"
#define     DEVICE_MODEL_NAME                           "DPS310"
#define     DEVICE_HW_VERSION                           1.0
#define     DRIVER_VERSION                              1.0
#define     DEVICE_PROD_REV_ID                          0x10

/* Attributes: Device performance :Pressure Sensing */
/* ����: �������� :ѹ����Ӧ */
#define     IFX_DPS310_PROD_REV_ID_REG_ADDR             0x0D
#define     IFX_DPS310_PROD_REV_ID_LEN                  1
#define     IFX_DSPS310_PROD_REV_ID_VAL                 DEVICE_PROD_REV_ID

#define     IFX_DPS310_SOFT_RESET_REG_ADDR              0x0C	//�����λ�Ĵ�����ַ
#define     IFX_DPS310_SOFT_RESET_REG_DATA              0x09	//�����λ�Ĵ�������
#define     IFX_DPS310_SOFT_RESET_REG_LEN               1
#define     IFX_DPS310_SOFT_RESET_VERIFY_REG_ADDR       0x06	//��λ��־�Ĵ�����ַ

#define     IFX_DPS310_COEF_REG_ADDR                    0x10	//ϵ���Ĵ�����ַ	
#define     IFX_DPS310_COEF_LEN                         18   	// Length in bytes

#define     IFX_DPS310_TMP_COEF_SRCE_REG_ADDR           0x28	//�¶�ϵ����λ�Ĵ�����ַ
#define     IFX_DPS310_TMP_COEF_SRCE_REG_LEN            1    	// Length in bytes��
#define     IFX_DPS310_TMP_COEF_SRCE_REG_POS_MASK       7    	// Length in bytes

#define     IFX_DPS310_PSR_TMP_READ_REG_ADDR            0x00 	//ѹ�����¶����ݼĴ�����ַ
#define     IFX_DPS310_PSR_TMP_READ_LEN                 6		//ѹ�����¶����ݵĳ���

#define     IFX_DPS310_PRS_CFG_REG_ADDR                 0x06	//ѹ���������üĴ�����ַ
#define     IFX_DPS310_PRS_CFG_REG_LEN                  1		//���ݳ���

#define     IFX_DPS310_TMP_CFG_REG_ADDR                 0x07	//�¶Ȳ������üĴ�����ַ
#define     IFX_DPS310_TMP_CFG_REG_LEN                  1		//���ݳ���

#define     IFX_DPS310_MEAS_CFG_REG_ADDR                0x08	//����������ģʽ��״̬�Ĵ�����ַ
#define     IFX_DPS310_MEAS_CFG_REG_LEN                 1		//

#define     IFX_DPS310_CFG_REG_ADDR                     0x09	//�жϺ�FIFO���üĴ�����ַ
#define     IFX_DPS310_CFG_REG_LEN                      1		//

#define     IFX_DPS310_CFG_TMP_SHIFT_EN_SET_VAL         0x08	//�����¶���λʹ��
#define     IFX_DPS310_CFG_PRS_SHIFT_EN_SET_VAL         0x04	//����ѹ��������λʹ��


#define     IFX_DPS310_FIFO_READ_REG_ADDR               0x00	//��FIFO�Ĵ�����ַ
#define     IFX_DPS310_FIFO_REG_READ_LEN                3		//
#define     IFX_DPS310_FIFO_BYTES_PER_ENTRY             3		//

#define     IFX_DPS310_FIFO_FLUSH_REG_ADDR              0x0C	//�����λ��FIFO����Ĵ�����ַ
#define     IFX_DPS310_FIFO_FLUSH_REG_VAL               0b1000000U	//

#define     IFX_DPS310_CFG_SPI_MODE_POS                 0	//����SPI����ģʽ
#define     IFX_DPS310_CFG_SPI_MODE_3_WIRE_VAL          1	//
#define     IFX_DPS310_CFG_SPI_MODE_4_WIRE_VAL          0	//

#define     IFX_DPS310_CFG_FIFO_ENABLE_POS              1	//����FIFOʹ��
#define     IFX_DPS310_CFG_FIFO_ENABLE_VAL              1	//
#define     IFX_DPS310_CFG_FIFO_DISABLE_VAL             0	//

#define     IFX_DPS310_CFG_INTR_PRS_ENABLE_POS          4	//
#define     IFX_DPS310_CFG_INTR_PRS_ENABLE_VAL          1U	//
#define     IFX_DPS310_CFG_INTR_PRS_DISABLE_VAL         0U	//

#define     IFX_DPS310_CFG_INTR_TEMP_ENABLE_POS         5	//
#define     IFX_DPS310_CFG_INTR_TEMP_ENABLE_VAL         1U	//
#define     IFX_DPS310_CFG_INTR_TEMP_DISABLE_VAL        0U	//

#define     IFX_DPS310_CFG_INTR_FIFO_FULL_ENABLE_POS    6	//
#define     IFX_DPS310_CFG_INTR_FIFO_FULL_ENABLE_VAL    1U	//
#define     IFX_DPS310_CFG_INTR_FIFO_FULL_DISABLE_VAL   0U	//

#define     IFX_DPS310_CFG_INTR_LEVEL_TYP_SEL_POS       7	//
#define     IFX_DPS310_CFG_INTR_LEVEL_TYP_ACTIVE_H      1U	//
#define     IFX_DPS310_CFG_INTR_LEVEL_TYP_ACTIVE_L      0U	//

#define     IFX_DPS310_INTR_SOURCE_PRESSURE             0	//ѹ�������ж�
#define     IFX_DPS310_INTR_SOURCE_TEMPERATURE          1	//
#define     IFX_DPS310_INTR_SOURCE_BOTH                 2	//

#define     IFX_DPS310_INTR_STATUS_REG_ADDR             0x0A	//�ж�״̬�Ĵ�����ַ
#define     IFX_DPS310_INTR_STATUS_REG_LEN              1
#define     IFX_DPS310_INTR_DISABLE_ALL                (uint8_t)0b10001111

#define     EINVAL                                      1
#define     EIO                                         2

#ifndef NULL
#define     NULL                                        ((void*)0)
#endif // NULL

/* _______________________________________________________ */

#define POW_2_23_MINUS_1	0x7FFFFF   //implies 2^23-1
#define POW_2_24			0x1000000
#define POW_2_15_MINUS_1	0x7FFF
#define POW_2_16			0x10000
#define POW_2_11_MINUS_1	0x7FF
#define POW_2_12			0x1000
#define POW_2_20			0x100000
#define POW_2_19_MINUS_1	524287

/* _______________________________________________________ */

/*Some aliases*/
typedef unsigned char       u8;

typedef char                s8;

typedef unsigned short      u16;

typedef short               s16;

typedef long                s32;

typedef	long long           s64;

typedef	unsigned int      	u32;

typedef	unsigned long long  u64;

typedef float               f32;

typedef double              f64;

//typedef u8                  bool;

#define false               0
#define true                1

/* Struct to hold calibration coefficients read from device*/
/*���ڻ��У׼ֵ�Ľṹ��*/
typedef struct
{
	/* calibration registers */
	/*У׼ֵ�Ĵ���*/

  s16 	C0;	// 12bit
  s16 	C1;	// 12bit
  s32	C00;	// 20bit
  s32   C10;	// 20bit
  s16 	C01;	// 16bit
  s16	C11;	// 16bit
  s16	C20;	// 16bit
  s16	C21;	// 16bit
  s16	C30;	// 16bit

}dps310_cal_coeff_regs_s;


/* enum for seeting/getting device operating mode*/
/*�������ô������Ĺ���ģʽ*/
typedef enum
{
//  DPS310_MODE_IDLE                   =  0b00000000,	//����ģʽ
//  DPS310_MODE_COMMAND_PRESSURE       =  0b00000001,	//ѹ���ֹ�ģʽ
//  DPS310_MODE_COMMAND_TEMPERATURE    =  0b00000010,	//�¶��ֹ�ģʽ
//  DPS310_MODE_BACKGROUND_PRESSURE    =  0b00000101,	//ѹ���Զ�ģʽ
//  DPS310_MODE_BACKGROUND_TEMPERATURE =  0b00000110,	//�¶��Զ�ģʽ
//  DPS310_MODE_BACKGROUND_ALL         =  0b00000111,	//ѹ�����¶ȶ����Զ�ģʽ
	
  DPS310_MODE_IDLE                   =  0x00,	//����ģʽ
  DPS310_MODE_COMMAND_PRESSURE       =  0x01,	//ѹ���ֹ�ģʽ
  DPS310_MODE_COMMAND_TEMPERATURE    =  0x02,	//�¶��ֹ�ģʽ
  DPS310_MODE_BACKGROUND_PRESSURE    =  0x05,	//ѹ���Զ�ģʽ
  DPS310_MODE_BACKGROUND_TEMPERATURE =  0x06,	//�¶��Զ�ģʽ
  DPS310_MODE_BACKGROUND_ALL         =  0x07,	//ѹ�����¶ȶ����Զ�ģʽ	

}dps310_operating_modes_e;



/* enum of scaling coefficients either Kp or Kt*/
/*�õ�У׼ֵKP��KT*/
typedef enum
{
    OSR_SF_1   = 524288,
    OSR_SF_2   = 1572864,
    OSR_SF_4   = 3670016,
    OSR_SF_8   = 7864320,
    OSR_SF_16  = 253952,
    OSR_SF_32  = 516096,
    OSR_SF_64  = 1040384,
    OSR_SF_128 = 2088960,

} dps310_scaling_coeffs_e;

/* enum of oversampling rates for pressure and temperature*/
/*�¶Ⱥ�ѹ���Ĳ����ʣ�����ÿ�β����೤��ʱ������*/
typedef enum
{
//    OSR_1   = 0b00000000,
//    OSR_2   = 0b00000001,
//    OSR_4   = 0b00000010,
//    OSR_8   = 0b00000011,
//    OSR_16  = 0b00000100,
//    OSR_32  = 0b00000101,
//    OSR_64  = 0b00000110,
//    OSR_128 = 0b00000111,
	
    OSR_1   = 0x00,
    OSR_2   = 0x01,
    OSR_4   = 0x02,
    OSR_8   = 0x03,
    OSR_16  = 0x04,
    OSR_32  = 0x05,
    OSR_64  = 0x06,
    OSR_128 = 0x07,
	
} dps310_osr_e;

/* enum of measurement rates for pressure*/
/*ѹ�������ʣ�����ÿ����ж��ٴβ���*/
typedef enum
{
//    PM_MR_1   = 0b00000000,
//    PM_MR_2   = 0b00010000,
//    PM_MR_4   = 0b00100000,
//    PM_MR_8   = 0b00110000,
//    PM_MR_16  = 0b01000000,
//    PM_MR_32  = 0b01010000,
//    PM_MR_64  = 0b01100000,
//    PM_MR_128 = 0b01110000,
	
    PM_MR_1   = 0x00,
    PM_MR_2   = 0x10,
    PM_MR_4   = 0x20,
    PM_MR_8   = 0x30,
    PM_MR_16  = 0x40,
    PM_MR_32  = 0x50,
    PM_MR_64  = 0x60,
    PM_MR_128 = 0x70,	

} dps310_pm_rate_e;

/* enum of measurement rates for temperature*/
/*�¶Ȳ����ʣ�����ÿ����ж��ٴβ���*/
typedef enum
{
//    TMP_MR_1   = 0b00000000,
//    TMP_MR_2   = 0b00010000,
//    TMP_MR_4   = 0b00100000,
//    TMP_MR_8   = 0b00110000,
//    TMP_MR_16  = 0b01000000,
//    TMP_MR_32  = 0b01010000,
//    TMP_MR_64  = 0b01100000,
//    TMP_MR_128 = 0b01110000,
	
    TMP_MR_1   = 0x00,
    TMP_MR_2   = 0x10,
    TMP_MR_4   = 0x20,
    TMP_MR_8   = 0x30,
    TMP_MR_16  = 0x40,
    TMP_MR_32  = 0x50,
    TMP_MR_64  = 0x60,
    TMP_MR_128 = 0x70,	

} dps310_tmp_rate_e;


/* enum of oversampling and measurement rates*/
/**/
typedef enum
{
    TMP_EXT_ASIC = 0x00,
    TMP_EXT_MEMS = 0x80,

}dps310_temperature_src_e;


/*Please update callbacks for bus communication
* callbacks are protocol agnostic/abstract so
* as to wrap around I2C or SPI low level protocols
*/

typedef struct 
{
        /*Provide a wrapper for single byte read/write and multi byte read
        * all callbacks return negative values to indicate error
        * however, read_byte must return the content in case of successful read
        * and read_block shall return number of bytes read successfully
        * For write_byte non zero return value shall indicate successful write
        */
        s16 (*read_byte)(u8 address);

        s16 (*read_block)(u8 address, u8 length, u8 *read_buffer);

        s16 (*write_byte)(u8 address, u8 data);

        /*It is expected to provide a wrapper for incorporating delay
        * the delay shall be in milliseconds. This is required as
        * after powering up the sensor, it takes 40ms until fused
        * calibration coefficients are ready to read.
        * in case this delay is handled appropriately by caller by other mechanism
        * please set this callback to NULL
        */
        void (*delay_ms)(u32 duration);

}dps310_bus_connection;


struct dps310_state 
{

        dps310_scaling_coeffs_e   tmp_osr_scale_coeff;    /* Temperature scaling coefficient�¶ȱ���ϵ��*/
        dps310_scaling_coeffs_e   prs_osr_scale_coeff;    /* Pressure scaling coefficientѹ������ϵ��*/
        dps310_cal_coeff_regs_s   calib_coeffs;           /* Calibration coefficients index У׼ϵ������*/
        dps310_operating_modes_e  dev_mode;               /* Current operating mode of device��ǰ�����Ĺ���ģʽ*/
        dps310_pm_rate_e	      press_mr;				  /* Current measurement readout rate (ODR) for pressure ��ǰѹ��������*/
        dps310_tmp_rate_e         temp_mr;				  /* Current measurement readout rate (ODR) for temperature ��ǰ�¶ȶ�����*/
        dps310_osr_e		      temp_osr;				  /* Current oversampling rate (OSR) for temperature ��ǰ�¶Ȳ�����*/
        dps310_osr_e		      press_osr;			  /* Current oversampling rate (OSR) for pressure��ǰѹ�������� */
        dps310_temperature_src_e  tmp_ext;                /* Temperature ASIC or MEMS. Should always be set MEMS*/
        u8                        cfg_word;               /* Keep the contents of CFG register as it gets configured                                                                            to avoid excessive bus transactions */
		unsigned char	          enable;
	dps310_bus_connection         *io;                    /*To access bus communication call backs */
};


/* public function prototypes */ /*���ù���ԭ��*/
int dps310_init( struct dps310_state *drv_state,dps310_bus_connection *io);

int dps310_get_processed_data(struct dps310_state *drv_state, f64 *pressure, f64 *temperature );

int dps310_config(struct dps310_state *drv_state,dps310_osr_e osr_temp,dps310_tmp_rate_e mr_temp,
					dps310_osr_e osr_press,dps310_pm_rate_e mr_press, dps310_temperature_src_e temp_src );

int dps310_standby(struct dps310_state *drv_state);

int dps310_resume(struct dps310_state *drv_state);

#endif // DPS310_H_INCLUDED
