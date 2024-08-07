/*************************************************************************
*  	文件名称:  	Hal_Init.c
*  	文件描述:  	MCU外设初始化，时钟初始化、IO、UART、定时器、中断、ADC等的初始化
*  	历史记录:
*  	创建时间:  	2024-05-10
*  	创建人:  	  江昌钤
*  	修改时间:
*	  修改人:
*   描述:
*************************************************************************/

/*************************************************************************
头文件引用声明
*************************************************************************/
#include    "at32f435_437_crm.h"
#include    "at32f435_437.h"
#include    "at32f435_437_flash.h"

#include    "Hal_Init.h"
#include    "Hal_Uart.h"
#include    "Hal_KeyLed.h"
#include    "Hal_PowManage.h"
#include    "Hal_Sleep.h"
/************************************************************************
宏定义
*************************************************************************/

/*************************************************************************
内部结构、枚举、公用体等结构声明
*************************************************************************/

/*************************************************************************
内部静态变量定义

全局变量定义

extern 外部变量声明

*************************************************************************/

/*************************************************************************
静态函数声明

extern 外部函数声明
*************************************************************************/

/*************************************************************************
函数定义
*************************************************************************/
/*----------------------------------------------------------------------------
函数名称： Hal_System_Clock_Init
具体描述： 系统时钟
----------------------------------------------------------------------------*/
void Hal_System_Clock_Init(void)
{
	
	/* 时钟重置 */
  crm_reset();

  /* 启用PWC外围时钟 */
  crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);

  /* 配置电压 */
  pwc_ldo_output_voltage_set(PWC_LDO_OUTPUT_1V3);

  /* 设置FLASH时钟分频 */
  flash_clock_divider_set(FLASH_CLOCK_DIV_3);

	/* 使能外部时钟 */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_HEXT, TRUE);

  /* 等待外部时钟就绪 */
  while(crm_hext_stable_wait() == ERROR)
  {
  }

  /* 配置PLL时钟资源 ：*/
  crm_pll_config(CRM_PLL_SOURCE_HEXT, 96, 2, CRM_PLL_FR_8);

  /* 使能PLL */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_PLL, TRUE);

  /* 等待PLL就绪 */
  while(crm_flag_get(CRM_PLL_STABLE_FLAG) != SET)
  {
  }

  /* 配置AHB分频 */
  crm_ahb_div_set(CRM_AHB_DIV_1);

  /* 配置APB2时钟：/1=144MHZ  */
  crm_apb2_div_set(CRM_APB2_DIV_1);

  /* 配置APB1时钟：/1=144MHZ  */
  crm_apb1_div_set(CRM_APB1_DIV_1);

  /* 使能自动步进模式 */
  crm_auto_step_mode_enable(TRUE);

  /* 选择PLL为系统时钟 */
  crm_sysclk_switch(CRM_SCLK_PLL);

  /* 等待PLL时钟为系统时钟 */
  while(crm_sysclk_switch_status_get() != CRM_SCLK_PLL)
  {
  }

  /* 失能自动步进模式 */
  crm_auto_step_mode_enable(FALSE);

  /* 更新系统时钟全局变量 */
  system_core_clock_update();
	
  /* 开启GPIOA时钟 */
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK,TRUE);
  /* 开启GPIOB时钟 */
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK,TRUE);
  /* 开启GPIOC时钟 */
  crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK,TRUE);
  /* 开启GPIOH时钟 */
  crm_periph_clock_enable(CRM_GPIOH_PERIPH_CLOCK,TRUE);
  
  crm_periph_clock_enable(CRM_SCFG_PERIPH_CLOCK, TRUE);
  
  systick_clock_source_config(SYSTICK_CLOCK_SOURCE_AHBCLK_NODIV);
  SysTick_Config(SystemCoreClock/1000); 
}
/**********************************************************************
*  函数名称:  NVIC_Init
*  功能描述:  中断配置
*  参数说明:  无
*  输入:      无
*  输出:      无
*  返回值:    无
***********************************************************************/
void NVIC_Init(void)
{
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
}

/*----------------------------------------------------------------------------
函数名称： Mcu_Hal_Init
具体描述： MCU硬件外设初始化
----------------------------------------------------------------------------*/
void Mcu_Hal_Init(void)
{
    /* 中断向量表 */
    nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0x4000);
    Hal_System_Clock_Init();
    NVIC_Init();
    Hal_Key_Init();
    Hal_Led_Init();
    Hal_Pow_Switch_Init();
    Hal_Charge_Init();
    Hal_Uart2_Init();
    Hal_RTC_init(1,0,0);
    RTC_Fre_Calib();
    //Hal_Wdt_Init();
}

