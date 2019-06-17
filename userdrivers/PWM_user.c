#include "PWM_user.h"

void Adt4_PWM_Init(void)
{
    en_adt_unit_t enAdt;
    stc_adt_basecnt_cfg_t stcAdtBaseCntCfg;
    stc_adt_CHxX_port_cfg_t stcAdtTIM4ACfg;
    stc_adt_CHxX_port_cfg_t stcAdtTIM4BCfg;	
    uint16_t u16Period;
    en_adt_compare_t enAdtCompareA;
    uint16_t u16CompareA;
    en_adt_compare_t enAdtCompareB;
    uint16_t u16CompareB;
    
    DDL_ZERO_STRUCT(stcAdtBaseCntCfg);
    DDL_ZERO_STRUCT(stcAdtTIM4ACfg);
    DDL_ZERO_STRUCT(stcAdtTIM4BCfg);

	enAdt = AdTIM4;
	
    stcAdtBaseCntCfg.enCntMode = AdtSawtoothMode;
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0Div2;
    Adt_Init(enAdt, &stcAdtBaseCntCfg);                      //ADT�ز�������ģʽ��ʱ������
    
    u16Period = 0xC000;
    Adt_SetPeriod(enAdt, u16Period);                         //��������
    
    enAdtCompareA = AdtCompareA;
    u16CompareA = 0x5000;
    Adt_SetCompareValue(enAdt, enAdtCompareA, u16CompareA);  //ͨ�ñȽϻ�׼ֵ�Ĵ���A����
    
    enAdtCompareB = AdtCompareB;
    u16CompareB = 0x9000;
    Adt_SetCompareValue(enAdt, enAdtCompareB, u16CompareB);  //ͨ�ñȽϻ�׼ֵ�Ĵ���B����
    
    stcAdtTIM4ACfg.enCap = AdtCHxCompareOutput;
    stcAdtTIM4ACfg.bOutEn = TRUE;
    stcAdtTIM4ACfg.enPerc = AdtCHxPeriodLow;
    stcAdtTIM4ACfg.enCmpc = AdtCHxCompareHigh;
    stcAdtTIM4ACfg.enStaStp = AdtCHxStateSelSS;
    stcAdtTIM4ACfg.enStaOut = AdtCHxPortOutLow;
    Adt_CHxXPortConfig(enAdt, AdtCHxA, &stcAdtTIM4ACfg);    //�˿�CHA����
    
    stcAdtTIM4BCfg.enCap = AdtCHxCompareOutput;
    stcAdtTIM4BCfg.bOutEn = TRUE;
    stcAdtTIM4BCfg.enPerc = AdtCHxPeriodInv;
    stcAdtTIM4BCfg.enCmpc = AdtCHxCompareInv;
    stcAdtTIM4BCfg.enStaStp = AdtCHxStateSelSS;
    stcAdtTIM4BCfg.enStaOut = AdtCHxPortOutLow;
    Adt_CHxXPortConfig(enAdt, AdtCHxB, &stcAdtTIM4BCfg);    //�˿�CHB����
    
    Adt_StartCount(enAdt);
}
