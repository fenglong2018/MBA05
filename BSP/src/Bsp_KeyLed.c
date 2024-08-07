/************************************************************************
*  �ļ�����: Bsp_KeyLed.c
*  �ļ�����: ������LED
*  ��ʷ��¼:  
*  ����ʱ��: 2024-05-10
*  ������:   ������
*  �޸�ʱ��:
*  �޸���:  
*  ����:
*************************************************************************/

/************************************************************************
ͷ�ļ���������
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
�궨��
*************************************************************************/

/************************************************************************
�ڲ��ṹ��ö�١�������Ƚṹ����
*************************************************************************/

/************************************************************************
�ڲ���̬��������

ȫ�ֱ�������

extern �ⲿ��������

*************************************************************************/
uint16_t u16MagKey_Count = 0,u16SideKey_Count = 0,u16BotKey_Count = 0,u16SlideKey_Count = 0;
uint8_t  u8MagKey_State = 0,u8SideKey_State = 0,u8BotKey_State = 0,u8SlideKey_State = 0;
STATE_CTRL_T            tSysState_Ctrl;
uint8_t  u8PowOnState = POWOFF;
VBAT_CHAG_CTRL_T        tVbatChag_Ctrl;
uint16_t    u16PercentB = 100;
/************************************************************************
��̬��������

extern �ⲿ��������
*************************************************************************/

/************************************************************************
��������
*************************************************************************/
/*----------------------------------------------------------------------------
�������ƣ� Bsp_Get_Sys_State
���������� ��ȡϵͳ״̬
----------------------------------------------------------------------------*/
uint8_t Bsp_Get_Sys_State(void)
{
    return tSysState_Ctrl.u8System_State;
}
/*----------------------------------------------------------------------------
�������ƣ� Bsp_Get_Pow_State
���������� ��ȡϵͳ״̬
----------------------------------------------------------------------------*/
uint8_t Bsp_Get_Pow_State(void)
{
    return u8PowOnState;
}
/*----------------------------------------------------------------------------
�������ƣ� Key_Start_Normal_Mode
���������� ����������λ����
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
�������ƣ� Key_Start_SOS_Mode
���������� ����SOS��λ����
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
�������ƣ� Key_Start_PULL_Mode
���������� ����SOS��λ����
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
�������ƣ� Key_Start_MOVE_Mode
���������� ����SOS��λ����
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
�������ƣ� Sys_Start_Charge_Mode
���������� ������ģʽ
----------------------------------------------------------------------------*/
void Sys_Start_Charge_Mode(void)
{
    #if   LOG
    printf("Start Charge Mode\n");
    #endif
    tVbatChag_Ctrl.eChagState = CHAG_START;     //���ģʽ
    USB_CH_EN();                                //���ʹ��
    Hal_Uart3_Init(); 
}

/*----------------------------------------------------------------------------
�������ƣ� Sys_Stop_Charge_Mode
���������� ֹͣ���ģʽ
----------------------------------------------------------------------------*/
void Sys_Stop_Charge_Mode(void)
{
    #if   LOG
    printf("Stop Charge Mode\n");
    #endif
    //USB_CH_DIS();                               //���ʹ��
    tVbatChag_Ctrl.eChagState = CHAG_NULL;     //���ģʽ
    Hal_Uart3_Deinit();                         //����3ʧ��
}

/*----------------------------------------------------------------------------
�������ƣ� Bsp_MagKey_Scan
���������� ��������ɨ��/10msʱ��
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
        if(u8PowOnState == POWOFF)                    //�����������������״̬
        {
            if(u8MagKey_State == KEY_NULL)
            {
                if(u16MagKey_Count++ > POWERKEY_LONGTIME)          //����
                {  
                    #if   LOG
                    printf("MagKey On and Pow ON\n");
                    #endif
                    u8MagKey_State = KEY_POWON;                    //�����������ظ�����
                    u16MagKey_Count = 0;
                    SYS_POW_EN();                               //��������
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
�������ƣ� Bsp_Sys_State_Ctrl
���������� ״̬����/1sʱ��
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
              Key_Start_SOS_Mode();                       //����SOS����ģʽ
            }
          }
          else if(tSysState_Ctrl.u8Priority == PRIORITY_MOVE)
          {
            if(tSysState_Ctrl.u32Count++ > 3)
            {
              Key_Start_Move_Mode();                       //����MOVE����ģʽ
            }
          }
          else if(tSysState_Ctrl.u8Priority == PRIORITY_PULL)
          {
            if(tSysState_Ctrl.u32Count++ > 3)
            {
              Key_Start_PULL_Mode();                       //����MOVE����ģʽ
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
              Key_Start_SOS_Mode();                       //����SOS����ģʽ
            }
          }
          else if(tSysState_Ctrl.u8Priority == PRIORITY_MOVE)
          {
            if(tSysState_Ctrl.u32Count++ > 3)
            {
              Key_Start_Move_Mode();                       //����MOVE����ģʽ
            }
          }
          else if(tSysState_Ctrl.u8Priority == PRIORITY_PULL)
          {
            if(tSysState_Ctrl.u32Count++ > 3)
            {
              Key_Start_PULL_Mode();                       //����MOVE����ģʽ
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
              Key_Start_Move_Mode();                       //����MOVE����ģʽ
            }
          }
          else if(tSysState_Ctrl.u8Priority == PRIORITY_PULL)
          {
            if(tSysState_Ctrl.u32Count++ > 3)
            {
              Key_Start_PULL_Mode();                       //����MOVE����ģʽ
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
              Key_Start_PULL_Mode();                       //����MOVE����ģʽ
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
�������ƣ� Bsp_Vbat_Chag_Ctrl
���������� �����ʾ����/1sʱ��
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
        Sys_Start_Charge_Mode();                //������ģʽ
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
�������ƣ� Bsp_Bat_Vol_Display
���������� ������ʾ
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
�������ƣ� Bsp_Stop_Bat_Vol_Display
���������� ֹͣ������ʾ
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
�������ƣ� Bsp_Error_Led_Display
���������� �澯����ʾ
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
