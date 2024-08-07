/********************************************************************** 
*  文件名称:  Hal_KeyLed.h
*  文件描述:  按键初始化、LED初始化、按键检测、LED驱动
*  历史记录:  
*  创建时间:  2024-05-9  
*  创建人:    江昌钤
*  修改时间:
*  修改人:  
*  描述: 	  
***********************************************************************/

#ifndef _HAL_KEYLED_H_
#define _HAL_KEYLED_H_

/********************************************************************** 
头文件引用声明
***********************************************************************/
#include    <stdint.h>
/********************************************************************** 
宏定义
***********************************************************************/
//=====================================================================
//IO口定义
//=====================================================================
/* 故障灯				            LED_ERROR:PC9  */
#define PT_LED_ERROR		    GPIOC
#define IO_LED_ERROR		    GPIO_PINS_9

/* 电量灯1					        LED_BAT1:PH2    */
#define PT_LED_BAT4		      GPIOH
#define IO_LED_BAT4		      GPIO_PINS_2

/* 电量灯2					        LED_BAT2:PA12    */
#define PT_LED_BAT3		      GPIOA
#define IO_LED_BAT3		      GPIO_PINS_12

/* 电量灯3					        LED_BAT3:PA11    */
#define PT_LED_BAT2		      GPIOA
#define IO_LED_BAT2		      GPIO_PINS_11

/* 电量灯4					        LED_BAT4:PA8    */
#define PT_LED_BAT1		      GPIOA
#define IO_LED_BAT1		      GPIO_PINS_8

/* 磁吸开关                 MAGKEY:PB13    */
#define PT_MAGKEY           GPIOB
#define IO_MAGKEY           GPIO_PINS_13

/* 侧面按键                 SIDEKEY:PA1    */
#define PT_SIDEKEY          GPIOA
#define IO_SIDEKEY          GPIO_PINS_1

/* 底部按键                 BOTKEY:PA0    */
#define PT_BOTKEY           GPIOA
#define IO_BOTKEY           GPIO_PINS_0

/* 滑动开关                 SLIDEKEY:PB5    */
#define PT_SLIDEKEY         GPIOB
#define IO_SLIDEKEY         GPIO_PINS_5

#define LED_ERLED_EN()          gpio_bits_set(PT_LED_ERROR, IO_LED_ERROR)
#define LED_ERLED_DIS()         gpio_bits_reset(PT_LED_ERROR, IO_LED_ERROR)

#define LED_BAT1_EN()           gpio_bits_set(PT_LED_BAT1, IO_LED_BAT1)
#define LED_BAT1_DIS()          gpio_bits_reset(PT_LED_BAT1, IO_LED_BAT1)

#define LED_BAT2_EN()           gpio_bits_set(PT_LED_BAT2, IO_LED_BAT2)
#define LED_BAT2_DIS()          gpio_bits_reset(PT_LED_BAT2, IO_LED_BAT2)

#define LED_BAT3_EN()           gpio_bits_set(PT_LED_BAT3, IO_LED_BAT3)
#define LED_BAT3_DIS()          gpio_bits_reset(PT_LED_BAT3, IO_LED_BAT3)

#define LED_BAT4_EN()           gpio_bits_set(PT_LED_BAT4, IO_LED_BAT4)
#define LED_BAT4_DIS()          gpio_bits_reset(PT_LED_BAT4, IO_LED_BAT4)
/********************************************************************** 
结构、枚举、公用体等结构定义
***********************************************************************/


/********************************************************************** 
源文件定义变量声明

extern 外部变量声明
***********************************************************************/

/********************************************************************** 
源文件函数声明

extern 外部函数声明
***********************************************************************/
/*----------------------------------------------------------------------------
函数名称： Hal_Key_Init
具体描述： 按键初始化
----------------------------------------------------------------------------*/
void Hal_Key_Init(void);
/*----------------------------------------------------------------------------
函数名称： Hal_Led_Init
具体描述： LED初始化
----------------------------------------------------------------------------*/
void Hal_Led_Init(void);
/*----------------------------------------------------------------------------
函数名称： Hal_Exit_Side_Init
具体描述： 侧面开关外部中断线初始化
----------------------------------------------------------------------------*/
void Hal_Exit_Side_Init(void);
/*----------------------------------------------------------------------------
函数名称： Hal_Exit_Side_Deinit
具体描述： 侧面开关外部中断线失能
----------------------------------------------------------------------------*/
void Hal_Exit_Side_Deinit(void);
/*----------------------------------------------------------------------------
函数名称： Hal_Exit_Bot_Init
具体描述： 底面开关外部中断线初始化
----------------------------------------------------------------------------*/
void Hal_Exit_Bot_Init(void);
/*----------------------------------------------------------------------------
函数名称： Hal_Exit_Bot_Deinit
具体描述： 底面开关外部中断线失能
----------------------------------------------------------------------------*/
void Hal_Exit_Bot_Deinit(void);
/*----------------------------------------------------------------------------
函数名称： Hal_Exit_Bot_Init
具体描述： 滑动开关外部中断线初始化
----------------------------------------------------------------------------*/
void Hal_Exit_Slide_Init(void);
/*----------------------------------------------------------------------------
函数名称： Hal_Exit_Bot_Deinit
具体描述： 滑动开关外部中断线失能
----------------------------------------------------------------------------*/
void Hal_Exit_Slide_Deinit(void);
/*----------------------------------------------------------------------------
函数名称： Hal_Get_MagKey_State
具体描述： 获取磁感应开关状态
输出参数： 1 有磁感应   
           0 无磁感应
----------------------------------------------------------------------------*/
uint8_t Hal_Get_MagKey_State(void);
/*----------------------------------------------------------------------------
函数名称： Hal_Get_SideKey_State
具体描述： 获取侧面开关状态
输出参数： 1 按下   
           0 弹开
----------------------------------------------------------------------------*/
uint8_t Hal_Get_SideKey_State(void);
/*----------------------------------------------------------------------------
函数名称： Hal_Get_BotKey_State
具体描述： 获取底面开关状态
输出参数： 1 按下   
           0 弹开
----------------------------------------------------------------------------*/
uint8_t Hal_Get_BotKey_State(void);
/*----------------------------------------------------------------------------
函数名称： Hal_Get_SlideKey_State
具体描述： 获取滑动开关状态
输出参数： 1 按下   
           0 弹开
----------------------------------------------------------------------------*/
uint8_t Hal_Get_SlideKey_State(void);
/*----------------------------------------------------------------------------
函数名称： Hal_Bat_Led_Display_Enable
具体描述： 电量显示LED使能
输入参数： u8LedNum  要显示的LED
           u8Enable  使能
----------------------------------------------------------------------------*/
void Hal_Bat_Led_Display_Enable(uint8_t u8LedNum,uint8_t u8Enable);
/*----------------------------------------------------------------------------
函数名称： Hal_Bat_Led_Heartbeat
具体描述： 电量显示LED闪
输入参数： u8LedNum  要闪烁的LED
----------------------------------------------------------------------------*/
void Hal_Bat_Led_Heartbeat(uint8_t u8LedNum);
/*----------------------------------------------------------------------------
函数名称： Hal_Error_Led_Display_Enable
具体描述： 告警显示LED使能
输入参数： u8Enable  使能
----------------------------------------------------------------------------*/
void Hal_Error_Led_Display_Enable(uint8_t u8Enable);
#endif
