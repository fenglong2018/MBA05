/************************************************************************
*  文件名称: Bsp_BatVol.c
*  文件描述: 电池电量更新和获取
*  历史记录:  
*  创建时间: 2023-05-10
*  创建人:   江昌钤
*  修改时间:
*  修改人:  
*  描述:
*************************************************************************/

/************************************************************************
头文件引用声明
*************************************************************************/
#include    <stdio.h>

#include    "Hal_PowManage.h"

#include    "Bsp_BatVol.h"
#include    "Bsp_Log.h"


/************************************************************************
宏定义
*************************************************************************/
#define ADC_VREF                         (3.3)
#define ADC_TEMP_BASE                    (1.26)
#define ADC_TEMP_SLOPE                   (-0.00423)
/************************************************************************
内部结构、枚举、公用体等结构声明
*************************************************************************/

/************************************************************************
内部静态变量定义

全局变量定义

extern 外部变量声明

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
静态函数声明

extern 外部函数声明
*************************************************************************/

/************************************************************************
函数定义
*************************************************************************/
/*----------------------------------------------------------------------------
函数名称：Arithmetic_Average
具体描述：算术平均法
输入参数：ADVal:          当前采样的AD值  
          pADSum:        累加和指针 
          ADAverage:     上一次平均值 
          Times:         算术平均的个数

输出参数：               本次算术平均的平均值
----------------------------------------------------------------------------*/
uint16_t Arithmetic_Average(uint16_t ADVal,uint32_t * pADSum,uint16_t ADAverage,uint8_t *Times)
{
	uint16_t u16Avg;									//平均值
	if(*Times < ADC_SlideAverage_Times)				    //如果采样个数没到
	{
		*Times += 1;								    //算术平均的个数加一
		*pADSum += ADVal;							    //加当前值
		if(*Times == ADC_SlideAverage_Times)		    //如果等于计算平均数的个数
		{
			u16Avg = (*pADSum)/ADC_SlideAverage_Times;	//平均值计算
			*pADSum = 0;							    //总和=0
			*Times = 0;								    //次数=0
			return u16Avg;								//返回当前平均值
		}
		return  ADAverage;							    //返回前一个平均值
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
函数名称： ADConvert
具体描述： AD转换轮询
----------------------------------------------------------------------------*/
void ADConvert(void)
{
	if(tBatVolCtrl.u8StartPercentOnce | tBatVolCtrl.u8StartPercentAlw)
	{
		switch(tADCFilter.u8Branch)
		{
			//触发转换
			case 0:
			{
				Hal_Start_VBat_ADC_Convert();
				tADCFilter.u8Branch++;
			}
				break;
			//结果赋值平均
			case 1:
			{
				tADCFilter.u32Restult = Hal_Get_ADC1_Value();
				tADCFilter.u16ADC4Result = Arithmetic_Average(tADCFilter.u32Restult,&tADCFilter.u32ADC4Sum,tADCFilter.u16ADC4Result,&tADCFilter.u8ADC4Time);
				tADCFilter.u8Branch++;
			}
				break;
				//触发转换
			case 2:
			{
				Hal_Start_VSloar_ADC_Convert();
				tADCFilter.u8Branch++;
			}
				break;
			//结果赋值平均
			case 3:
			{
				tADCFilter.u32Restult = Hal_Get_ADC2_Value();
				tADCFilter.u16ADC5Result = Arithmetic_Average(tADCFilter.u32Restult,&tADCFilter.u32ADC5Sum,tADCFilter.u16ADC5Result,&tADCFilter.u8ADC5Time);
				tADCFilter.u8Branch = 0;
			}
				break;
			default:
			{
			 tADCFilter.u8Branch = 0;											//返回第一个采样通道
			}
				break;
		}
	}
}

/*----------------------------------------------------------------------------
函数名称：   Bsp_Updata_Bat_Percent
具体描述：   更新电池电量百分比/10ms时基更新
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
函数名称：   Bps_Start_Once_Time_Updata_BatVol_Percent
具体描述：   开始一次更新电池电量
----------------------------------------------------------------------------*/
void Bps_Start_Once_Time_Updata_BatVol_Percent(void)
{
    tBatVolCtrl.u8StartTime = 0;
    tBatVolCtrl.u8StartPercentOnce = 1;
    tBatVolCtrl.u8StartPercentAlw = 0;
    Hal_BatVol_Init();
}
/*----------------------------------------------------------------------------
函数名称：   Bps_Start_Always_Updata_BatVol_Percent
具体描述：   开始循环更新电池电量
----------------------------------------------------------------------------*/
void Bps_Start_Always_Updata_BatVol_Percent(void)
{
    tBatVolCtrl.u8StartPercentAlw = 1;
    Hal_BatVol_Init();
}
/*----------------------------------------------------------------------------
函数名称：   Bps_Stop_Always_Updata_BatVol_Percent
具体描述：   结束循环更新电池电量
----------------------------------------------------------------------------*/
void Bps_Stop_Always_Updata_BatVol_Percent(void)
{
    tBatVolCtrl.u8StartPercentAlw = 0;
    Hal_ADC_Deinit();
}
/*----------------------------------------------------------------------------
函数名称：   Bsp_Get_Updata_Bat_Voltage_Once_State
具体描述：   获取更新电池电量状态
----------------------------------------------------------------------------*/
uint8_t Bsp_Get_Updata_Bat_Voltage_Once_State(void)
{
    return tBatVolCtrl.u8StartPercentOnce;
}
/*----------------------------------------------------------------------------
函数名称：   Bsp_Get_BatPercent
具体描述：   取得最新电量
----------------------------------------------------------------------------*/
uint8_t Bsp_Get_BatPercent(void)
{
    return tBatVolCtrl.u8Percent;
}
/*----------------------------------------------------------------------------
函数名称：   Bsp_Get_BatVol
具体描述：   取得最新电池电压
----------------------------------------------------------------------------*/
uint16_t Bsp_Get_BatVol(void)
{
    return tBatVolCtrl.u16CurVol;
}
/*----------------------------------------------------------------------------
函数名称：   Bsp_Get_BatADValue
具体描述：   取得最新电池AD值
----------------------------------------------------------------------------*/
uint16_t Bsp_Get_BatADValue(void)
{
    return tADCFilter.u16ADC4Result;
}
/*----------------------------------------------------------------------------
函数名称：   Bsp_Get_Temp
具体描述：   取得最新温度
----------------------------------------------------------------------------*/
double Bsp_Get_Temp(void)
{
  return (ADC_TEMP_BASE-(double)tADCFilter.u16ADC4Result*ADC_VREF/4096)/ADC_TEMP_SLOPE+25;
}
/*----------------------------------------------------------------------------
函数名称：   Bsp_Reset_BatPercent
具体描述：   电量复位
----------------------------------------------------------------------------*/
void Bsp_Reset_BatPercent(void)
{
    tBatVolCtrl.u8Percent = 99;
}
/*----------------------------------------------------------------------------
函数名称：   Bsp_Get_SloarADValue
具体描述：   取得最新太阳能AD值
----------------------------------------------------------------------------*/
uint16_t Bsp_Get_SloarADValue(void)
{
    return tADCFilter.u16ADC5Result;
}
/*----------------------------------------------------------------------------
函数名称：   Bsp_Get_SloarVol
具体描述：   取得最新太阳能电压
----------------------------------------------------------------------------*/
uint16_t Bsp_Get_SloarVol(void)
{
  return (uint16_t)((double)tADCFilter.u16ADC5Result*1.85);
}

/*----------------------------------------------------------------------------
函数名称：   Bsp_Get_SloarVolFloat
具体描述：   取得最新太阳能电压
----------------------------------------------------------------------------*/
float Bsp_Get_SloarVolFloat(void)
{
  return (float)((double)tADCFilter.u16ADC5Result*0.00185);
}
