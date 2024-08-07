/*************************************************************************
*  	�ļ�����:  	Sys_TimeRun.c
*  	�ļ�����:  	ϵͳʱ������
*  	��ʷ��¼:
*  	����ʱ��:  	2023-01-18
*  	������:  	������
*  	�޸�ʱ��:
*	�޸���:
*   ����:
*************************************************************************/

/*************************************************************************
ͷ�ļ���������
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
�궨��
*************************************************************************/
/* �ɹ� */
#ifndef SUCC
#define SUCC						0
#endif
/* ʧ�� */
#ifndef FAIL
#define FAIL						1
#endif
/*************************************************************************
�ڲ��ṹ��ö�١�������Ƚṹ����
*************************************************************************/

/*************************************************************************
�ڲ���̬��������

ȫ�ֱ�������

extern �ⲿ��������

*************************************************************************/
/* ϵͳ��ʱ������λms,uint32_t�����ɼ���49.71�죬����ʱ���������˳�ʱ��� */
uint32_t s_u32SysTickTime = 0;			//32λ������1ms�ۼ�һ��
uint8_t u81msTime = 0;
uint8_t u81msBranch = 0,u810msBranch = 0,u8100msBranch = 0,u81sBranch = 0;

/*************************************************************************
��̬��������

extern �ⲿ��������
*************************************************************************/
void SlotBranch1ms(void);
void SlotBranch10ms(void);
void SlotBranch100ms(void);
void SlotBranch1s(void);
/*************************************************************************
��������
*************************************************************************/
/*----------------------------------------------------------------------------
�������ƣ�SysTick_Handler
����������SysTick�ж�
----------------------------------------------------------------------------*/
void SysTick_Handler(void)
{
    u81msTime = 1;
    s_u32SysTickTime++;
}
/*----------------------------------------------------------------------------
�������ƣ�time_routine
����������1msʱ��ѭ��ִ��
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
�������ƣ�SlotBranch1ms
����������1msִ��һ��
----------------------------------------------------------------------------*/
void SlotBranch1ms(void)
{
    SlotBranch10ms();
}
/*----------------------------------------------------------------------------
�������ƣ�SlotBranch10ms
����������10msִ��һ��
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
�������ƣ�SlotBranch100ms
����������100msִ��һ��
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
�������ƣ�SlotBranch1s
����������1sִ��һ��
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

