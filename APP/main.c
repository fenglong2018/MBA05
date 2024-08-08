/**
  **************************************************************************
  * @file     main.c
  * @brief    main program
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */

#include "at32f435_437_board.h"
#include "at32f435_437_clock.h"
#include "FreeRTOS.h"
#include "task.h"
#include    "Hal_Init.h"
#include    "Bsp_Init.h"
#include    "Sys_TimeRun.h"
#include    "Bsp_UartRec.h"

#include 		<stdio.h>
#include 	"main.h"

#ifndef USEFREERTOS
#include    "Bsp_Config.h"
#include    "Bsp_BatVol.h"
#endif	



/**********************************************************************
*  函数名称:  main
*  功能描述:  主程序
*  参数说明:  无
*  输入:      无
*  输出:      无
*  返回值:    无
***********************************************************************/
int main(void)
{
	Mcu_Hal_Init();
	Fun_Bsp_Init();

	Bsp_Task();
	
	
	
  printf("system starting...\r\n");
  while(1)
  {
#ifndef USEFREERTOS	  
    Time_Routime();
    Bsp_Receive_Uart1_Fifo_Data();
    Bsp_Receive_Uart2_Fifo_Data();
    Bsp_Receive_Uart3_Fifo_Data();
    Bsp_Receive_Uart4_Fifo_Data();	  
#endif	  
		return 0;
  }
}




