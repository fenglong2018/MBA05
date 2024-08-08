/********************************************************************** 
*  文件名称:  Bsp_Task.h
*  文件描述:  创建任务
*  历史记录:  
*  创建时间:  2024-08-08  
*  创建人:    何凤龙
*  修改时间:
*  修改人:  
*  描述: 	  
***********************************************************************/

#ifndef _BSP_TASK_H_
#define _BSP_TASK_H_

/********************************************************************** 
头文件引用声明
***********************************************************************/
#include    <stdint.h>
#include 	"FreeRTOS.h"
#include 	"task.h"
#include 	<stdio.h>

#include    "Bsp_Config.h"
#include    "Bsp_RdssSend.h"
#include    "Bsp_BatVol.h"
#include    "Bsp_Log.h"
#include    "Bsp_KeyLed.h"
/********************************************************************** 
宏定义
***********************************************************************/

/********************************************************************** 
结构、枚举、公用体等结构定义
***********************************************************************/
TaskHandle_t 	adc_task;
TaskHandle_t	Bat_Percent;
TaskHandle_t	MagKey_Scan;
TaskHandle_t	Port_InforGet;
TaskHandle_t	Nor_SOS;
TaskHandle_t	Pull_SendCtrl;
TaskHandle_t	Bat_Vol_Display;
TaskHandle_t	Sys_State_Ctrl;
TaskHandle_t	Vbat_Chag_Ctrl;
TaskHandle_t	Error_Led_Display;
/********************************************************************** 
源文件定义变量声明

extern 外部变量声明
***********************************************************************/


/********************************************************************** 
源文件函数声明
extern 外部函数声明
***********************************************************************/




/*----------------------------------------------------------------------------
函数名称： Bsp_Task
具体描述： 任务创建程序
----------------------------------------------------------------------------*/
void Bsp_Task(void);
void setupADCTask(void);
void adc_task_function(void *pvParameters);
void setupBatTask(void);
void Bat_Percent_function(void *pvParameters);
void setupMagKeyTask(void);
void MagKey_Scan_function(void *pvParameters);
void setupPortInforGetTask(void);
void Port_InforGet_function(void *pvParameters);
void setupNorSOSTask(void);
void Nor_SOS_function(void *pvParameters);
void setupPullSendCtrlTask(void);
void Pull_SendCtrl_function(void *pvParameters);
void setupBatVolDisplaytask(void);
void Bat_Vol_Display_function(void *pvParameters);
void setupSysStateCtrltask(void);
void Sys_State_Ctrl_function(void *pvParameters);
void setupBspVbatChagCtrltask(void);
void Vbat_Chag_Ctrl_function(void *pvParameters);
void setupErrorLedDisplaytask(void);
void Error_Led_Display_function(void *pvParameters);
#endif


