/************************************************************************
*  �ļ�����: Bsp_BatVol.c
*  �ļ�����: ��ص������ºͻ�ȡ
*  ��ʷ��¼:  
*  ����ʱ��: 2023-05-10
*  ������:   ������
*  �޸�ʱ��:
*  �޸���:  
*  ����:
*************************************************************************/

/************************************************************************
ͷ�ļ���������
*************************************************************************/
#include    <stdio.h>

#include    "Hal_PowManage.h"

#include    "Bsp_BatVol.h"
#include    "Bsp_Log.h"


/************************************************************************
�궨��
*************************************************************************/
#define ADC_VREF                         (3.3)
#define ADC_TEMP_BASE                    (1.26)
#define ADC_TEMP_SLOPE                   (-0.00423)
/************************************************************************
�ڲ��ṹ��ö�١�������Ƚṹ����
*************************************************************************/

/************************************************************************
�ڲ���̬��������

ȫ�ֱ�������

extern �ⲿ��������

*************************************************************************/
ADC_FILTER_T        tADCFilter = {0};
BATVOL_CTRL_T       tBatVolCtrl = {99,0,0,0,0};
uint16_t            u16VSloar = 0;
const uint16_t  BufBAT[100] = {
    3508,3518,3524,3534,3537,3547,3552,3557,3562,3569,  //9
    3574,3579,3584,3589,3594,3597,3602,3603,3608,3611,  //19
    3614,3618,3619,3623,3626,3627,3631,3632,3636,3637,  //29
    3640,3644,3645,3650,3650,3654,3656,3659,3663,3665,  //39
    3668,3672,3674,3678,3680,3686,3691,3696,3701,3706,  //49
    3714,3720,3725,3734,3736,3745,3752,3759,3763,3771,  //59
    3778,3783,3792,3799,3805,3813,3820,3829,3836,3845,  //69
    3852,3861,3867,3875,3884,3890,3899,3907,3916,3926,  //79
    3931,3942,3946,3955,3964,3970,3981,3987,3996,4005,  //89
    4012,4021,4029,4038,4048,4056,4063,4072,4081,4089,  //99
};
/************************************************************************
��̬��������

extern �ⲿ��������
*************************************************************************/

/************************************************************************
��������
*************************************************************************/
/*----------------------------------------------------------------------------
�������ƣ�Arithmetic_Average
��������������ƽ����
���������ADVal:          ��ǰ������ADֵ  
          pADSum:        �ۼӺ�ָ�� 
          ADAverage:     ��һ��ƽ��ֵ 
          Times:         ����ƽ���ĸ���

���������               ��������ƽ����ƽ��ֵ
----------------------------------------------------------------------------*/
uint16_t Arithmetic_Average(uint16_t ADVal,uint32_t * pADSum,uint16_t ADAverage,uint8_t *Times)
{
	uint16_t u16Avg;									//ƽ��ֵ
	if(*Times < ADC_SlideAverage_Times)				    //�����������û��
	{
		*Times += 1;								    //����ƽ���ĸ�����һ
		*pADSum += ADVal;							    //�ӵ�ǰֵ
		if(*Times == ADC_SlideAverage_Times)		    //������ڼ���ƽ�����ĸ���
		{
			u16Avg = (*pADSum)/ADC_SlideAverage_Times;	//ƽ��ֵ����
			*pADSum = 0;							    //�ܺ�=0
			*Times = 0;								    //����=0
			return u16Avg;								//���ص�ǰƽ��ֵ
		}
		return  ADAverage;							    //����ǰһ��ƽ��ֵ
	}
	else
	{
		u16Avg = (*pADSum)/ADC_SlideAverage_Times;		
		*pADSum = 0;
		*Times = 0;
		return u16Avg;
	}
}
/*----------------------------------------------------------------------------
�������ƣ� ADConvert
���������� ADת����ѯ
----------------------------------------------------------------------------*/
void ADConvert(void)
{
	if(tBatVolCtrl.u8StartPercentOnce | tBatVolCtrl.u8StartPercentAlw)
	{
		switch(tADCFilter.u8Branch)
		{
			//����ת��
			case 0:
			{
				Hal_Start_VBat_ADC_Convert();
				tADCFilter.u8Branch++;
			}
				break;
			//�����ֵƽ��
			case 1:
			{
				tADCFilter.u32Restult = Hal_Get_ADC1_Value();
				tADCFilter.u16ADC4Result = Arithmetic_Average(tADCFilter.u32Restult,&tADCFilter.u32ADC4Sum,tADCFilter.u16ADC4Result,&tADCFilter.u8ADC4Time);
				tADCFilter.u8Branch++;
			}
				break;
				//����ת��
			case 2:
			{
				Hal_Start_VSloar_ADC_Convert();
				tADCFilter.u8Branch++;
			}
				break;
			//�����ֵƽ��
			case 3:
			{
				tADCFilter.u32Restult = Hal_Get_ADC2_Value();
				tADCFilter.u16ADC5Result = Arithmetic_Average(tADCFilter.u32Restult,&tADCFilter.u32ADC5Sum,tADCFilter.u16ADC5Result,&tADCFilter.u8ADC5Time);
				tADCFilter.u8Branch = 0;
			}
				break;
			default:
			{
			 tADCFilter.u8Branch = 0;											//���ص�һ������ͨ��
			}
				break;
		}
	}
}

/*----------------------------------------------------------------------------
�������ƣ�   Bsp_Updata_Bat_Percent
����������   ���µ�ص����ٷֱ�/10msʱ������
----------------------------------------------------------------------------*/
void Bsp_Updata_Bat_Percent(void)
{
    uint8_t u8Percent;
    uint16_t u16Vol;
    if(tBatVolCtrl.u8StartPercentOnce)
    {
        if(tBatVolCtrl.u8StartTime++ > 10)
        {
            tBatVolCtrl.u8StartTime = 0;
            tBatVolCtrl.u8StartPercentOnce = 0;
            u16Vol = (uint16_t)((double)tADCFilter.u16ADC4Result*1.4000);
            u8Percent = tBatVolCtrl.u8Percent;
            while(u8Percent>0)
            {
                if(u16Vol > BufBAT[u8Percent])
                {
                    break;
                }
                else
                {
                    u8Percent--;
                }
            }
            tBatVolCtrl.u8Percent = u8Percent;
            tBatVolCtrl.u16CurVol = u16Vol;
            Hal_ADC_Deinit();
        }
    }
    if(tBatVolCtrl.u8StartPercentAlw)
    {
        u16Vol = (uint16_t)((double)tADCFilter.u16ADC4Result*1.4000);
        u8Percent = 99;
        while(u8Percent>0)
        {
            if(u16Vol > BufBAT[u8Percent])
            {
                break;
            }
            else
            {
                u8Percent--;
            }
        }
        tBatVolCtrl.u8Percent = u8Percent;
        tBatVolCtrl.u16CurVol = u16Vol;
    }
}

/*----------------------------------------------------------------------------
�������ƣ�   Bps_Start_Once_Time_Updata_BatVol_Percent
����������   ��ʼһ�θ��µ�ص���
----------------------------------------------------------------------------*/
void Bps_Start_Once_Time_Updata_BatVol_Percent(void)
{
    tBatVolCtrl.u8StartTime = 0;
    tBatVolCtrl.u8StartPercentOnce = 1;
    tBatVolCtrl.u8StartPercentAlw = 0;
    Hal_BatVol_Init();
}
/*----------------------------------------------------------------------------
�������ƣ�   Bps_Start_Always_Updata_BatVol_Percent
����������   ��ʼѭ�����µ�ص���
----------------------------------------------------------------------------*/
void Bps_Start_Always_Updata_BatVol_Percent(void)
{
    tBatVolCtrl.u8StartPercentAlw = 1;
    Hal_BatVol_Init();
}
/*----------------------------------------------------------------------------
�������ƣ�   Bps_Stop_Always_Updata_BatVol_Percent
����������   ����ѭ�����µ�ص���
----------------------------------------------------------------------------*/
void Bps_Stop_Always_Updata_BatVol_Percent(void)
{
    tBatVolCtrl.u8StartPercentAlw = 0;
    Hal_ADC_Deinit();
}
/*----------------------------------------------------------------------------
�������ƣ�   Bsp_Get_Updata_Bat_Voltage_Once_State
����������   ��ȡ���µ�ص���״̬
----------------------------------------------------------------------------*/
uint8_t Bsp_Get_Updata_Bat_Voltage_Once_State(void)
{
    return tBatVolCtrl.u8StartPercentOnce;
}
/*----------------------------------------------------------------------------
�������ƣ�   Bsp_Get_BatPercent
����������   ȡ�����µ���
----------------------------------------------------------------------------*/
uint8_t Bsp_Get_BatPercent(void)
{
    return tBatVolCtrl.u8Percent;
}
/*----------------------------------------------------------------------------
�������ƣ�   Bsp_Get_BatVol
����������   ȡ�����µ�ص�ѹ
----------------------------------------------------------------------------*/
uint16_t Bsp_Get_BatVol(void)
{
    return tBatVolCtrl.u16CurVol;
}
/*----------------------------------------------------------------------------
�������ƣ�   Bsp_Get_BatADValue
����������   ȡ�����µ��ADֵ
----------------------------------------------------------------------------*/
uint16_t Bsp_Get_BatADValue(void)
{
    return tADCFilter.u16ADC4Result;
}
/*----------------------------------------------------------------------------
�������ƣ�   Bsp_Get_Temp
����������   ȡ�������¶�
----------------------------------------------------------------------------*/
double Bsp_Get_Temp(void)
{
  return (ADC_TEMP_BASE-(double)tADCFilter.u16ADC4Result*ADC_VREF/4096)/ADC_TEMP_SLOPE+25;
}
/*----------------------------------------------------------------------------
�������ƣ�   Bsp_Reset_BatPercent
����������   ������λ
----------------------------------------------------------------------------*/
void Bsp_Reset_BatPercent(void)
{
    tBatVolCtrl.u8Percent = 99;
}
/*----------------------------------------------------------------------------
�������ƣ�   Bsp_Get_SloarADValue
����������   ȡ������̫����ADֵ
----------------------------------------------------------------------------*/
uint16_t Bsp_Get_SloarADValue(void)
{
    return tADCFilter.u16ADC5Result;
}
/*----------------------------------------------------------------------------
�������ƣ�   Bsp_Get_SloarVol
����������   ȡ������̫���ܵ�ѹ
----------------------------------------------------------------------------*/
uint16_t Bsp_Get_SloarVol(void)
{
  return (uint16_t)((double)tADCFilter.u16ADC5Result*1.85);
}

/*----------------------------------------------------------------------------
�������ƣ�   Bsp_Get_SloarVolFloat
����������   ȡ������̫���ܵ�ѹ
----------------------------------------------------------------------------*/
float Bsp_Get_SloarVolFloat(void)
{
  return (float)((double)tADCFilter.u16ADC5Result*0.00185);
}
