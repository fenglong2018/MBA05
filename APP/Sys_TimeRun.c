/*************************************************************************
*  	文件名称:  	Sys_TimeRun.c
*  	文件描述:  	系统时间运行
*  	历史记录:
*  	创建时间:  	2023-01-18
*  	创建人:  	江昌钤
*  	修改时间:
*	修改人:
*   描述:
*************************************************************************/

/*************************************************************************
头文件引用声明
*************************************************************************/
#include    <stdint.h>
#include    <stdio.h>


#include    "Hal_Sleep.h"

#include    "Bsp_BatVol.h"
#include    "Bsp_KeyLed.h"
#include    "Bsp_RdssSend.h"
#include    "Bsp_GnssParse.h"
#include    "Bsp_Config.h"
#include    "Bsp_Log.h"
/************************************************************************
宏定义
*************************************************************************/
/* 成功 */
#ifndef SUCC
#define SUCC						0
#endif
/* 失败 */
#ifndef FAIL
#define FAIL						1
#endif
/*************************************************************************
内部结构、枚举、公用体等结构声明
*************************************************************************/

/*************************************************************************
内部静态变量定义

全局变量定义

extern 外部变量声明

*************************************************************************/
/* 系统计时器，单位ms,uint32_t，最多可计数49.71天，各定时函数考虑了超时情况 */
uint32_t s_u32SysTickTime = 0;			//32位计数，1ms累加一次
uint8_t u81msTime = 0;
uint8_t u81msBranch = 0,u810msBranch = 0,u8100msBranch = 0,u81sBranch = 0;

/*************************************************************************
静态函数声明

extern 外部函数声明
*************************************************************************/
void SlotBranch1ms(void);
void SlotBranch10ms(void);
void SlotBranch100ms(void);
void SlotBranch1s(void);
/*************************************************************************
函数定义
*************************************************************************/
/*----------------------------------------------------------------------------
函数名称：SysTick_Handler
具体描述：SysTick中断
----------------------------------------------------------------------------*/
void SysTick_Handler(void)
{
    u81msTime = 1;
    s_u32SysTickTime++;
}
/*----------------------------------------------------------------------------
函数名称：time_routine
具体描述：1ms时间循环执行
----------------------------------------------------------------------------*/
void Time_Routime(void)
{
    if(u81msTime)
    {
        SlotBranch1ms();
        u81msTime = 0;
    }
}
/*----------------------------------------------------------------------------
函数名称：SlotBranch1ms
具体描述：1ms执行一次
----------------------------------------------------------------------------*/
void SlotBranch1ms(void)
{
    SlotBranch10ms();
}
/*----------------------------------------------------------------------------
函数名称：SlotBranch10ms
具体描述：10ms执行一次
----------------------------------------------------------------------------*/
void SlotBranch10ms(void)
{
	switch(u810msBranch ++)
	{
		case 0:
		{
            ADConvert();
		}
			break;
		case 1:
		{
            Bsp_Updata_Bat_Percent();
		}
			break;
		case 2:
		{
            Bsp_MagKey_Scan();
		}
			break;
		case 3:
		{
            //Hal_Feed_Dog();
		}
			break;
		case 4:
		{
            
		}
			break;
		case 5:
		{

		}
			break;
		case 6:
		{
            
		}
			break;
		case 7:
		{
		}
			break;
		case 8:
		{

		}
			break;
		default:
		{
            SlotBranch100ms();
            u810msBranch = 0;
		}
			break;
	}
}
/*----------------------------------------------------------------------------
函数名称：SlotBranch100ms
具体描述：100ms执行一次
----------------------------------------------------------------------------*/
void SlotBranch100ms(void)
{
	switch(u8100msBranch ++)
	{
		case 0:
		{
       Bsp_Port_InforGet_Ctrl();
		}
			break;
		case 1:
		{
            
		}
			break;
        
		case 2:
		{
            
		}
			break;
        
		case 3:
		{
            
		}
			break;
		case 4:
		{
            
		}
			break;
		case 5:
		{
            
		}
			break;
		case 6:
		{
            
		}
			break;
		case 7:
		{
            
		}
			break;
		case 8:
		{

		}
			break;
		default:
		{
            SlotBranch1s();
            u8100msBranch = 0;
		}
			break;
	}
}
/*----------------------------------------------------------------------------
函数名称：SlotBranch1s
具体描述：1s执行一次
----------------------------------------------------------------------------*/
void SlotBranch1s(void)
{
	switch(u81sBranch ++)
	{
		case 0:
		{      
        Bsp_Nor_SOS_SendCtrl();
		}
			break;
		case 1:
		{
      #if LOG
      printf("Temp = %f , VSloar = %d\n",Bsp_Get_Temp(),Bsp_Get_SloarVol());
      #endif
		}
			break;
		case 2:
		{
        Bsp_Pull_SendCtrl();
		}
			break;
		case 3:
		{
      Bsp_Bat_Vol_Display();
		}
			break;
		case 4:
		{
      Bsp_Sys_State_Ctrl();
		}
			break;
		case 5:
		{
      Bsp_Vbat_Chag_Ctrl();
		}
			break;
		case 6:
		{
      Bsp_Error_Led_Display();
		}
			break;
		case 7:
		{ 
        
		}
			break;
		case 8:
		{

		}
			break;
		default:
		{
      u81sBranch = 0;
		}
			break;
	}
}

