/************************************************************************
*  文件名称: Bsp_KeyLed.c
*  文件描述: 按键、LED
*  历史记录:  
*  创建时间: 2024-05-10
*  创建人:   江昌钤
*  修改时间:
*  修改人:  
*  描述:
*************************************************************************/

/************************************************************************
头文件引用声明
*************************************************************************/
#include    <stdint.h>
#include    <stdio.h>

#include    "at32f435_437_gpio.h"

#include    "Hal_KeyLed.h"
#include    "Hal_PowManage.h"
#include    "Hal_Uart.h"

#include    "Bsp_RdssSend.h"
#include    "Bsp_KeyLed.h"
#include    "Bsp_BatVol.h"
#include    "Bsp_Log.h"
/************************************************************************
宏定义
*************************************************************************/

/************************************************************************
内部结构、枚举、公用体等结构声明
*************************************************************************/

/************************************************************************
内部静态变量定义

全局变量定义

extern 外部变量声明

*************************************************************************/
uint16_t u16MagKey_Count = 0,u16SideKey_Count = 0,u16BotKey_Count = 0,u16SlideKey_Count = 0;
uint8_t  u8MagKey_State = 0,u8SideKey_State = 0,u8BotKey_State = 0,u8SlideKey_State = 0;
STATE_CTRL_T            tSysState_Ctrl;
uint8_t  u8PowOnState = POWOFF;
VBAT_CHAG_CTRL_T        tVbatChag_Ctrl;
uint16_t    u16PercentB = 100;
/************************************************************************
静态函数声明

extern 外部函数声明
*************************************************************************/

/************************************************************************
函数定义
*************************************************************************/
/*----------------------------------------------------------------------------
函数名称： Bsp_Get_Sys_State
具体描述： 获取系统状态
----------------------------------------------------------------------------*/
uint8_t Bsp_Get_Sys_State(void)
{
    return tSysState_Ctrl.u8System_State;
}
/*----------------------------------------------------------------------------
函数名称： Bsp_Get_Pow_State
具体描述： 获取系统状态
----------------------------------------------------------------------------*/
uint8_t Bsp_Get_Pow_State(void)
{
    return u8PowOnState;
}
/*----------------------------------------------------------------------------
函数名称： Key_Start_Normal_Mode
具体描述： 开启正常定位发送
----------------------------------------------------------------------------*/
void Key_Start_Normal_Mode(void)
{
  #if   LOG
  printf("Start Normal Mode\n");
  #endif
  tSysState_Ctrl.u8System_State = STATE_NORMAL;
  Bsp_Start_Normal_Mode();
}
/*----------------------------------------------------------------------------
函数名称： Key_Start_SOS_Mode
具体描述： 开启SOS定位发送
----------------------------------------------------------------------------*/
void Key_Start_SOS_Mode(void)
{
  #if   LOG
  printf("Start SOS Mode\n");
  #endif
  tSysState_Ctrl.u8System_State = STATE_SOS;
  Bsp_Start_SOS_Mode();
}
/*----------------------------------------------------------------------------
函数名称： Key_Start_PULL_Mode
具体描述： 开启SOS定位发送
----------------------------------------------------------------------------*/
void Key_Start_PULL_Mode(void)
{
  #if   LOG
  printf("Start PULL Mode\n");
  #endif
  tSysState_Ctrl.u8System_State = STATE_PULL;
  Bsp_Start_PULL_Mode();
}
/*----------------------------------------------------------------------------
函数名称： Key_Start_MOVE_Mode
具体描述： 开启SOS定位发送
----------------------------------------------------------------------------*/
void Key_Start_Move_Mode(void)
{
  #if   LOG
  printf("Start Move Mode\n");
  #endif
  tSysState_Ctrl.u8System_State = STATE_MOVE;
  Bsp_Start_Move_Mode();
}
/*----------------------------------------------------------------------------
函数名称： Sys_Start_Charge_Mode
具体描述： 进入充电模式
----------------------------------------------------------------------------*/
void Sys_Start_Charge_Mode(void)
{
    #if   LOG
    printf("Start Charge Mode\n");
    #endif
    tVbatChag_Ctrl.eChagState = CHAG_START;     //充电模式
    USB_CH_EN();                                //充电使能
    Hal_Uart3_Init(); 
}

/*----------------------------------------------------------------------------
函数名称： Sys_Stop_Charge_Mode
具体描述： 停止充电模式
----------------------------------------------------------------------------*/
void Sys_Stop_Charge_Mode(void)
{
    #if   LOG
    printf("Stop Charge Mode\n");
    #endif
    //USB_CH_DIS();                               //充电使能
    tVbatChag_Ctrl.eChagState = CHAG_NULL;     //充电模式
    Hal_Uart3_Deinit();                         //串口3失能
}

/*----------------------------------------------------------------------------
函数名称： Bsp_MagKey_Scan
具体描述： 磁吸按键扫描/10ms时基
----------------------------------------------------------------------------*/
void Bsp_MagKey_Scan(void)
{
    if(!Hal_Get_MagKey_State())
    {
        u16MagKey_Count = 0;
        u8MagKey_State = KEY_NULL;
    }
    else
    {
        if(u8PowOnState == POWOFF)                    //如果不处于正常发送状态
        {
            if(u8MagKey_State == KEY_NULL)
            {
                if(u16MagKey_Count++ > POWERKEY_LONGTIME)          //长按
                {  
                    #if   LOG
                    printf("MagKey On and Pow ON\n");
                    #endif
                    u8MagKey_State = KEY_POWON;                    //按键锁定不重复触发
                    u16MagKey_Count = 0;
                    SYS_POW_EN();                               //开启供电
                    u8PowOnState = POWON;
                    tSysState_Ctrl.u8System_State = STATE_NULL;
                    tSysState_Ctrl.u32Count = 0;
                    tSysState_Ctrl.u8Priority_B = PRIORITY_NULL;
                }
            }
        }
    }
}

/*----------------------------------------------------------------------------
函数名称： Bsp_Sys_State_Ctrl
具体描述： 状态控制/1s时基
----------------------------------------------------------------------------*/
void Bsp_Sys_State_Ctrl(void)
{
  if(u8PowOnState == POWON)
  {
    tSysState_Ctrl.u8Priority = PRIORITY_NORMAL;
    if(Hal_Get_SideKey_State())
      tSysState_Ctrl.u8Priority = PRIORITY_SOS;
    if(Hal_Get_BotKey_State())
      tSysState_Ctrl.u8Priority = PRIORITY_MOVE;
    if(Hal_Get_SlideKey_State())
      tSysState_Ctrl.u8Priority = PRIORITY_PULL;
    #if   LOG
    printf("Priority = %d\n",tSysState_Ctrl.u8Priority);
    #endif  
    switch(tSysState_Ctrl.u8System_State)
    {
      case STATE_NULL:
      {
        if(tSysState_Ctrl.u8Priority != tSysState_Ctrl.u8Priority_B)
        {
          tSysState_Ctrl.u8Priority_B = tSysState_Ctrl.u8Priority;
          tSysState_Ctrl.u32Count = 0;                                                                                                                  
        }
        else
        {
          if(tSysState_Ctrl.u8Priority == PRIORITY_NORMAL)
          {
            if(tSysState_Ctrl.u32Count++ > 3)
            {
              tSysState_Ctrl.u8System_State = STATE_NORMAL;
              Key_Start_Normal_Mode();
            }
          }
          else if(tSysState_Ctrl.u8Priority == PRIORITY_SOS)
          {
            if(tSysState_Ctrl.u32Count++ > 3)
            {
              Key_Start_SOS_Mode();                       //开启SOS发送模式
            }
          }
          else if(tSysState_Ctrl.u8Priority == PRIORITY_MOVE)
          {
            if(tSysState_Ctrl.u32Count++ > 3)
            {
              Key_Start_Move_Mode();                       //开启MOVE发送模式
            }
          }
          else if(tSysState_Ctrl.u8Priority == PRIORITY_PULL)
          {
            if(tSysState_Ctrl.u32Count++ > 3)
            {
              Key_Start_PULL_Mode();                       //开启MOVE发送模式
            }
          }
          else
          {
            tSysState_Ctrl.u32Count = 0;
          }
        }
        break;
      }
      case STATE_NORMAL:
      {
        if(tSysState_Ctrl.u8Priority != tSysState_Ctrl.u8Priority_B)
        {
          tSysState_Ctrl.u8Priority_B = tSysState_Ctrl.u8Priority;
          tSysState_Ctrl.u32Count = 0;
        }
        else
        {
          if(tSysState_Ctrl.u8Priority == PRIORITY_SOS)
          {
            if(tSysState_Ctrl.u32Count++ > 3)
            {
              Key_Start_SOS_Mode();                       //开启SOS发送模式
            }
          }
          else if(tSysState_Ctrl.u8Priority == PRIORITY_MOVE)
          {
            if(tSysState_Ctrl.u32Count++ > 3)
            {
              Key_Start_Move_Mode();                       //开启MOVE发送模式
            }
          }
          else if(tSysState_Ctrl.u8Priority == PRIORITY_PULL)
          {
            if(tSysState_Ctrl.u32Count++ > 3)
            {
              Key_Start_PULL_Mode();                       //开启MOVE发送模式
            }
          }
          else
          {
            tSysState_Ctrl.u32Count = 0;
          }
        }
        break;
      }
      case STATE_SOS:
      {
        if(tSysState_Ctrl.u8Priority != tSysState_Ctrl.u8Priority_B)
        {
          tSysState_Ctrl.u8Priority_B = tSysState_Ctrl.u8Priority;
          tSysState_Ctrl.u32Count = 0;
        }
        else
        {
          if(tSysState_Ctrl.u8Priority == PRIORITY_NORMAL)
          {
            if(tSysState_Ctrl.u32Count++ > 20)
            {
              tSysState_Ctrl.u8System_State = STATE_NORMAL;
              Key_Start_Normal_Mode();
            }
          }
          else if(tSysState_Ctrl.u8Priority == PRIORITY_MOVE)
          {
            if(tSysState_Ctrl.u32Count++ > 3)
            {
              Key_Start_Move_Mode();                       //开启MOVE发送模式
            }
          }
          else if(tSysState_Ctrl.u8Priority == PRIORITY_PULL)
          {
            if(tSysState_Ctrl.u32Count++ > 3)
            {
              Key_Start_PULL_Mode();                       //开启MOVE发送模式
            }
          }
          else
          {
            tSysState_Ctrl.u32Count = 0;
          }
        }
        break;
      }
      case STATE_MOVE:
      {
        if(tSysState_Ctrl.u8Priority != tSysState_Ctrl.u8Priority_B)
        {
          tSysState_Ctrl.u8Priority_B = tSysState_Ctrl.u8Priority;
          tSysState_Ctrl.u32Count = 0;
        }
        else
        {
          if(tSysState_Ctrl.u8Priority == PRIORITY_NORMAL)
          {
            if(tSysState_Ctrl.u32Count++ > 20)
            {
              tSysState_Ctrl.u8System_State = STATE_NORMAL;
              Key_Start_Normal_Mode();
            }
          }
          else if(tSysState_Ctrl.u8Priority == PRIORITY_PULL)
          {
            if(tSysState_Ctrl.u32Count++ > 3)
            {
              Key_Start_PULL_Mode();                       //开启MOVE发送模式
            }
          }
          else
          {
            tSysState_Ctrl.u32Count = 0;
          }
        }
        break;
      }
      case STATE_PULL:
      {
        if(tSysState_Ctrl.u8Priority != tSysState_Ctrl.u8Priority_B)
        {
          tSysState_Ctrl.u8Priority_B = tSysState_Ctrl.u8Priority;
          tSysState_Ctrl.u32Count = 0;
        }
        else
        {
          if(tSysState_Ctrl.u8Priority == PRIORITY_NORMAL)
          {
            if(tSysState_Ctrl.u32Count++ > 20)
            {
              tSysState_Ctrl.u8System_State = STATE_NORMAL;
              Key_Start_Normal_Mode();
            }
          }
          else
          {
            tSysState_Ctrl.u32Count = 0;
          }
        }
        break;
      }
      default:
        break;
    }
  }
}
/*----------------------------------------------------------------------------
函数名称： Bsp_Vbat_Chag_Ctrl
具体描述： 充电显示控制/1s时基
----------------------------------------------------------------------------*/
void Bsp_Vbat_Chag_Ctrl(void)
{
  uint8_t     u8i;
  switch(tVbatChag_Ctrl.eChagState)
  {
    case CHAG_NULL:
    {
      if(Hal_Get_USB_In_State())
      {
        Sys_Start_Charge_Mode();                //进入充电模式
      }
      break;
    }
    case CHAG_START:
    {
        tVbatChag_Ctrl.u8GetPercent = 100;
        tVbatChag_Ctrl.u16Count = 0;
        tVbatChag_Ctrl.u8LedNum = 1;
        tVbatChag_Ctrl.u8LedNumB = 0;
        Bps_Start_Always_Updata_BatVol_Percent();
        tVbatChag_Ctrl.eChagState = CHAG_ING;
        break;
    }
    case CHAG_ING:
    {
        if(Hal_Get_ChagFull_State())
        {
            Hal_Bat_Led_Display_Enable(1,1);
            Hal_Bat_Led_Display_Enable(2,1);
            Hal_Bat_Led_Display_Enable(3,1);
            Hal_Bat_Led_Display_Enable(4,1);
            tVbatChag_Ctrl.eChagState = CHAG_FULL;
        }
        else
        {
            if(Bsp_Get_BatPercent() != tVbatChag_Ctrl.u8GetPercent)
            {
                tVbatChag_Ctrl.u8GetPercent = Bsp_Get_BatPercent();
                if(tVbatChag_Ctrl.u8GetPercent <25)
                    tVbatChag_Ctrl.u8LedNum = 1;
                else if(tVbatChag_Ctrl.u8GetPercent >=25 && tVbatChag_Ctrl.u8GetPercent <50)
                    tVbatChag_Ctrl.u8LedNum = 2;
                else if(tVbatChag_Ctrl.u8GetPercent >=50 && tVbatChag_Ctrl.u8GetPercent <75)
                    tVbatChag_Ctrl.u8LedNum = 3;
                else 
                    tVbatChag_Ctrl.u8LedNum = 4;
                if(tVbatChag_Ctrl.u8LedNumB != tVbatChag_Ctrl.u8LedNum)
                {
                    tVbatChag_Ctrl.u8LedNumB = tVbatChag_Ctrl.u8LedNum;
                    for(u8i = 1;u8i<=tVbatChag_Ctrl.u8LedNum;u8i++)
                    {
                        Hal_Bat_Led_Display_Enable(u8i,1);
                    }
                    for(u8i = 4;u8i>tVbatChag_Ctrl.u8LedNum;u8i--)
                    {
                        Hal_Bat_Led_Display_Enable(u8i,0);
                    }
                }
            }
            Hal_Bat_Led_Heartbeat(tVbatChag_Ctrl.u8LedNum);
        }
        if(!Hal_Get_USB_In_State())
        {
            Sys_Stop_Charge_Mode();
        }
        break;
    }
    case CHAG_FULL:
    {
        if(!Hal_Get_ChagFull_State())
        {
            tVbatChag_Ctrl.eChagState = CHAG_ING;
        }
		if(!Hal_Get_USB_In_State())
        {
            Sys_Stop_Charge_Mode();
        }
        break;
    }
  }
}

/*----------------------------------------------------------------------------
函数名称： Bsp_Bat_Vol_Display
具体描述： 电量显示
----------------------------------------------------------------------------*/
void Bsp_Bat_Vol_Display(void)
{
    uint8_t  u8i,u8LedNum = 0;
    if(u8PowOnState == POWON)
    {
        if(Bsp_Get_BatPercent() != u16PercentB)
        {
          u16PercentB = Bsp_Get_BatPercent();
          if(u16PercentB <25)
            u8LedNum = 1;
          else if(u16PercentB >=25 && u16PercentB <50)
            u8LedNum = 2;
          else if(u16PercentB >=50 && u16PercentB <75)
            u8LedNum = 3;
          else 
            u8LedNum = 4;
          for(u8i = 1;u8i<=u8LedNum;u8i++)
          {
            Hal_Bat_Led_Display_Enable(u8i,1);
          }
          for(u8i = 4;u8i>u8LedNum;u8i--)
          {
            Hal_Bat_Led_Display_Enable(u8i,0);
          }
        }
    }
}
/*----------------------------------------------------------------------------
函数名称： Bsp_Stop_Bat_Vol_Display
具体描述： 停止电量显示
----------------------------------------------------------------------------*/
void Bsp_Stop_Bat_Vol_Display(void)
{
    Hal_Bat_Led_Display_Enable(1,0);
    Hal_Bat_Led_Display_Enable(2,0);
    Hal_Bat_Led_Display_Enable(3,0);
    Hal_Bat_Led_Display_Enable(4,0);
    u16PercentB = 100;
}
/*----------------------------------------------------------------------------
函数名称： Bsp_Error_Led_Display
具体描述： 告警灯显示
----------------------------------------------------------------------------*/
void Bsp_Error_Led_Display(void)
{
  if((tSysState_Ctrl.u8System_State == STATE_SOS) || (tSysState_Ctrl.u8System_State == STATE_PULL))
  {
    Hal_Error_Led_Display_Enable(1);
  }
  else
    Hal_Error_Led_Display_Enable(0);
}
