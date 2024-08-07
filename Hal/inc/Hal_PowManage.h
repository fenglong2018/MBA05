/********************************************************************** 
*  文件名称:  Hal_PowManage.h
*  文件描述:  电源开关初始化、充电检测初始化、电量检测初始化、获取充电状态
*  历史记录:  
*  创建时间:  2024-05-9  
*  创建人:    江昌钤
*  修改时间:
*  修改人:  
*  描述: 	  
***********************************************************************/

#ifndef _HAL_POWMANAGE_H_
#define _HAL_POWMANAGE_H_

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
/* 电源使能                 POW_EN:PB14     */
#define PT_POW_EN           GPIOB
#define IO_POW_EN           GPIO_PINS_14

/* RDSS_PA电源使能          RDSS_PA_EN:PC8     */
#define PT_RDSS_PA_EN       GPIOC
#define IO_RDSS_PA_EN       GPIO_PINS_8

/* RDSS_VCC使能             RDSS_VCC_EN:PC7     */
#define PT_RDSS_VCC_EN      GPIOC
#define IO_RDSS_VCC_EN      GPIO_PINS_7

/* GNSS电源使能             GNSS_POW_EN:PC6     */
#define PT_GNSS_POW_EN      GPIOC
#define IO_GNSS_POW_EN      GPIO_PINS_6

/* GNSS电源使能             422_POW_EN:PA6     */
#define PT_422_POW_EN       GPIOA
#define IO_422_POW_EN       GPIO_PINS_6

/* USB插入检测              USBIN:PB10     */
#define PT_USBIN    		    GPIOB
#define IO_USBIN    		    GPIO_PINS_10

/* 充电充满检测             CHAGFULL:PB12     */
#define PT_CHAGFULL    		  GPIOB
#define IO_CHAGFULL    		  GPIO_PINS_12

/* 充电使能                 CH_EN:PB15     */
#define PT_CH_EN    		    GPIOB
#define IO_CH_EN    		    GPIO_PINS_15

/* 电量检测                 VBAT_D:PA4     */
#define PT_VBAT_D           GPIOA  
#define IO_VBAT_D           GPIO_PINS_4

/* 太阳能检测               VSLOAR_D:PA5     */
#define PT_VSLOAR_D         GPIOA  
#define IO_VSLOAR_D         GPIO_PINS_5

#define USB_CH_EN()             gpio_bits_set(PT_CH_EN, IO_CH_EN)
#define USB_CH_DIS()            gpio_bits_reset(PT_CH_EN, IO_CH_EN)

#define GNSS_POW_EN()           gpio_bits_set(PT_GNSS_POW_EN, IO_GNSS_POW_EN)
#define GNSS_POW_DIS()          gpio_bits_reset(PT_GNSS_POW_EN, IO_GNSS_POW_EN)

#define RS422_POW_EN()          gpio_bits_set(PT_422_POW_EN, IO_422_POW_EN)
#define RS422_POW_DIS()         gpio_bits_reset(PT_422_POW_EN, IO_422_POW_EN)

#define RDSS_POW_EN()           gpio_bits_reset(PT_RDSS_PA_EN, IO_RDSS_PA_EN);\
                                gpio_bits_reset(PT_RDSS_VCC_EN, IO_RDSS_VCC_EN)
                                
#define RDSS_POW_DIS()          gpio_bits_set(PT_RDSS_PA_EN, IO_RDSS_PA_EN);\
                                gpio_bits_set(PT_RDSS_VCC_EN, IO_RDSS_VCC_EN)

#define SYS_POW_EN()            gpio_bits_set(PT_POW_EN, IO_POW_EN)
#define SYS_POW_DIS()           gpio_bits_reset(PT_POW_EN, IO_POW_EN)
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
函数名称： Hal_Pow_Switch_Init
具体描述： 电源开关初始化
----------------------------------------------------------------------------*/
void Hal_Pow_Switch_Init(void);
/*----------------------------------------------------------------------------
函数名称： Hal_Charge_Init
具体描述： 充电检测初始化
----------------------------------------------------------------------------*/
void Hal_Charge_Init(void);
/*----------------------------------------------------------------------------
函数名称： Hal_BatVol_Init
具体描述： 电池电量检测初始化
----------------------------------------------------------------------------*/
void Hal_BatVol_Init(void);
/*----------------------------------------------------------------------------
函数名称： Hal_ADC_Deinit
具体描述： ADC失能
----------------------------------------------------------------------------*/
void Hal_ADC_Deinit(void);
/*----------------------------------------------------------------------------
函数名称： Hal_Start_ADC_Convert
具体描述： 开始通道AD转换
----------------------------------------------------------------------------*/
void Hal_Start_VBat_ADC_Convert(void);
/*----------------------------------------------------------------------------
函数名称： Hal_Start_VSloar_ADC_Convert
具体描述： 开始太阳能电压AD转换
----------------------------------------------------------------------------*/
void Hal_Start_VSloar_ADC_Convert(void);
/*----------------------------------------------------------------------------
函数名称： Hal_Get_ADC1_Value
具体描述： 获取ADC1值
----------------------------------------------------------------------------*/
uint16_t Hal_Get_ADC1_Value(void);
/*----------------------------------------------------------------------------
函数名称： Hal_Get_ADC2_Value
具体描述： 获取ADC1值
----------------------------------------------------------------------------*/
uint16_t Hal_Get_ADC2_Value(void);
/*----------------------------------------------------------------------------
函数名称： Hal_Get_USB_In_State
具体描述： 获取USB插入状态
输出参数： 1 有USB插入   
           0 无USB插入
----------------------------------------------------------------------------*/
uint8_t Hal_Get_USB_In_State(void);
/*----------------------------------------------------------------------------
函数名称： Hal_Get_ChagFull_State
具体描述： 获取充满状态
输出参数： 1 充满   
           0 未充满
----------------------------------------------------------------------------*/
uint8_t Hal_Get_ChagFull_State(void);
#endif
