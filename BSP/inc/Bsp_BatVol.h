/********************************************************************** 
*  文件名称:  Bsp_BatVol.h
*  文件描述:  电池电量检测
*  历史记录:  
*  创建时间:  2024-05-10 
*  创建人:    江昌钤
*  修改时间:
*  修改人:  
*  描述: 	  
***********************************************************************/

#ifndef _BSP_BATVOL_H_
#define _BSP_BATVOL_H_

/********************************************************************** 
头文件引用声明
***********************************************************************/
#include "stdint.h"

/********************************************************************** 
宏定义
***********************************************************************/
/* ADC平均取值次数 */
#define ADC_SlideAverage_Times	        5

/********************************************************************** 
结构、枚举、公用体等结构定义
***********************************************************************/
typedef struct
{
    uint8_t     u8Percent;              //电量百分比
    uint16_t    u16CurVol;
    uint8_t     u8StartPercentOnce;     //转换电量一次
    uint8_t     u8StartPercentAlw;      //循环转换电量
    uint8_t     u8StartTime;            //转换电量计时时间
}BATVOL_CTRL_T;

typedef struct
{
    uint32_t    u32Restult;
    uint32_t    u32ADC4Sum;
    uint8_t     u8ADC4Time;
    uint8_t     u8Branch;
    uint16_t    u16ADC4Result;
    uint32_t    u32ADC5Sum;
    uint8_t     u8ADC5Time;
    uint16_t    u16ADC5Result;
}ADC_FILTER_T;
/********************************************************************** 
源文件定义变量声明

extern 外部变量声明
***********************************************************************/


/********************************************************************** 
源文件函数声明

extern 外部函数声明
***********************************************************************/


/*----------------------------------------------------------------------------
函数名称： ADConvert
具体描述： AD转换轮询
----------------------------------------------------------------------------*/
void ADConvert(void);
/*----------------------------------------------------------------------------
函数名称：   Bsp_Updata_Bat_Percent
具体描述：   更新电池电量百分比/10ms时基更新
----------------------------------------------------------------------------*/
void Bsp_Updata_Bat_Percent(void);
/*----------------------------------------------------------------------------
函数名称：   Bps_Start_Once_Time_Updata_BatVol_Percent
具体描述：   开始一次更新电池电量
----------------------------------------------------------------------------*/
void Bps_Start_Once_Time_Updata_BatVol_Percent(void);
/*----------------------------------------------------------------------------
函数名称：   Bps_Start_Always_Updata_BatVol_Percent
具体描述：   开始循环更新电池电量
----------------------------------------------------------------------------*/
void Bps_Start_Always_Updata_BatVol_Percent(void);
/*----------------------------------------------------------------------------
函数名称：   Bps_Stop_Always_Updata_BatVol_Percent
具体描述：   结束循环更新电池电量
----------------------------------------------------------------------------*/
void Bps_Stop_Always_Updata_BatVol_Percent(void);
/*----------------------------------------------------------------------------
函数名称：   Bsp_Get_Updata_Bat_Voltage_Once_State
具体描述：   获取更新电池电量状态
----------------------------------------------------------------------------*/
uint8_t Bsp_Get_Updata_Bat_Voltage_Once_State(void);
/*----------------------------------------------------------------------------
函数名称：   Bsp_Get_BatPercent
具体描述：   取得最新电量
----------------------------------------------------------------------------*/
uint8_t Bsp_Get_BatPercent(void);
/*----------------------------------------------------------------------------
函数名称：   get_bat_voltage
具体描述：   取得最新电池电压
----------------------------------------------------------------------------*/
uint16_t Bsp_Get_BatVol(void);
/*----------------------------------------------------------------------------
函数名称：   Bsp_Reset_BatPercent
具体描述：   电量复位
----------------------------------------------------------------------------*/
void Bsp_Reset_BatPercent(void);
/*----------------------------------------------------------------------------
函数名称：   Bsp_Get_SloarVol
具体描述：   取得最新太阳能电压
----------------------------------------------------------------------------*/
uint16_t Bsp_Get_SloarVol(void);
/*----------------------------------------------------------------------------
函数名称：   Bsp_Get_BatADValue
具体描述：   取得最新电池AD值
----------------------------------------------------------------------------*/
uint16_t Bsp_Get_BatADValue(void);
/*----------------------------------------------------------------------------
函数名称：   Bsp_Get_SloarADValue
具体描述：   取得最新太阳能AD值
----------------------------------------------------------------------------*/
uint16_t Bsp_Get_SloarADValue(void);
/*----------------------------------------------------------------------------
函数名称：   Bsp_Get_SloarVolFloat
具体描述：   取得最新太阳能电压
----------------------------------------------------------------------------*/
float Bsp_Get_SloarVolFloat(void);
double Bsp_Get_Temp(void);
#endif
