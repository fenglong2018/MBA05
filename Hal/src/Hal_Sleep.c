/*************************************************************************
*  	文件名称:  	Hal_Sleep.c
*  	文件描述:  	串口初始化、串口字符串发送
*  	历史记录:
*  	创建时间:  	2024-05-10
*  	创建人:  	江昌钤
*  	修改时间:
*	修改人:
*   描述:
*************************************************************************/

/*************************************************************************
头文件引用声明
*************************************************************************/
#include    <stdio.h>

#include    "at32f435_437_gpio.h"
#include    "at32f435_437_pwc.h"
#include    "at32f435_437_ertc.h"
#include    "at32f435_437_exint.h"
#include    "at32f435_437_misc.h"
#include    "at32f435_437_tmr.h"
#include    "at32f435_437_wwdt.h"

#include    "Bsp_Log.h"

#include    "Hal_Sleep.h"
#include    "Hal_KeyLed.h"
#include    "Hal_Uart.h"
#include    "Hal_PowManage.h"
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
__IO uint32_t   systick_index = 0;
__IO uint32_t   lick_freq = 0;
__IO uint32_t   capture_number = 0;
__IO uint32_t   period_value = 0;
uint16_t        tmp_c4[2] = {0, 0};
volatile uint32_t asynch_prediv = 0;
volatile uint32_t synch_prediv = 0;
ertc_time_type    timertc;
uint8_t  u8SleepUp = 0;
/*************************************************************************
静态函数声明

extern 外部函数声明
*************************************************************************/

/*************************************************************************
函数定义
*************************************************************************/
void delay_us(uint8_t u8us)
{
  uint16_t  u16coni,u16conj;
  for(u16coni = 0;u16coni<u8us;u16coni++)
  for(u16conj = 0;u16conj<10;u16conj++);
}

/*----------------------------------------------------------------------------
函数名称： Hal_RTC_init
具体描述： RTC初始化
输入参数： u8Hour    小时
           u8Min     分钟
           u8Sec     秒
----------------------------------------------------------------------------*/
void Hal_RTC_init(uint8_t u8Hour,uint8_t u8Min,uint8_t u8Sec)
{
  /* 开启电源管理时钟 */
  crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);

  /* 电池供电区域的写入使能 */
  pwc_battery_powered_domain_access(TRUE);

  /* 电池供电域的复位设置 */
  crm_battery_powered_domain_reset(TRUE);
  crm_battery_powered_domain_reset(FALSE);

  /* 内部低速时钟使能 */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_LICK, TRUE);

  /* 等待内部低速时钟就绪 */
  while(crm_flag_get(CRM_LICK_STABLE_FLAG) == RESET)
  {
  }

  /* 选择RTC的时钟源 */
  crm_ertc_clock_select(CRM_ERTC_CLOCK_LICK);
  /* RTC时钟使能 */
  crm_ertc_clock_enable(TRUE);
  /* RTC复位 */
  ertc_reset();
  /* 等待RTC寄存器更新完成 */
  ertc_wait_update();
  /* 分频器设置 */
  ertc_divider_set(0x7F, 0xFF);
  /* 小时模式设置：24小时制 */
  ertc_hour_mode_set(ERTC_HOUR_MODE_24);
  /* 日期设置: 2023-05-01 */
  ertc_date_set(23, 5, 1, 5);
  /* 时间设置: 0:00:00 */
  ertc_time_set(u8Hour, u8Min, u8Sec, ERTC_24H);
}

uint32_t lick_frequency_get(void)
{
  tmr_input_config_type  tmr_input_config_struct;
  crm_clocks_freq_type   crm_clock_freq;

  /* enable tmr5 apb1 clocks */
  crm_periph_clock_enable(CRM_TMR5_PERIPH_CLOCK, TRUE);

  /* connect internally the tmr5_ch4 to the lick clock output */
  tmr_iremap_config(TMR5, TMR2_PTP_TMR5_LICK);

  /* configure tmr5 divider */
  tmr_div_value_set(TMR5, 0);
  tmr_event_sw_trigger(TMR5, TMR_OVERFLOW_SWTRIG);

  /* tmr5 channel4 input capture mode configuration */
  tmr_input_config_struct.input_channel_select = TMR_SELECT_CHANNEL_4;
  tmr_input_config_struct.input_mapped_select = TMR_CC_CHANNEL_MAPPED_DIRECT;
  tmr_input_config_struct.input_polarity_select = TMR_INPUT_RISING_EDGE;
  tmr_input_config_struct.input_filter_value = 0;
  tmr_input_channel_init(TMR5, &tmr_input_config_struct, TMR_CHANNEL_INPUT_DIV_1);
  tmr_input_channel_divider_set(TMR5, TMR_SELECT_CHANNEL_4, TMR_CHANNEL_INPUT_DIV_8);

  /* enable tmr5 interrupt channel */
  nvic_irq_enable(TMR5_GLOBAL_IRQn, 0, 0);

  /* enable tmr5 counter */
  tmr_counter_enable(TMR5, TRUE);

  /* reset the flags */
  TMR5->swevt  = 0;

  /* enable the cc4 interrupt request */
  tmr_interrupt_enable(TMR5, TMR_C4_INT, TRUE);

  /* wait the tmr5 measuring operation to be completed */
  while(capture_number != 2);

  /* deinitialize the tmr5 peripheral */
  tmr_reset(TMR5);

  /* get the frequency value */
  crm_clocks_freq_get(&crm_clock_freq);

  /* get pclk1 divider */
  if ((CRM->cfg_bit.apb1div) > 0)
  {
    /* pclk1 divider different from 1 => timclk = 2 * pclk1 */
    return (((2 * crm_clock_freq.apb1_freq) / period_value) * 8) ;
  }
  else
  {
    /* pclk1 divider equal to 1 => timclk = pclk1 */
    return ((crm_clock_freq.apb1_freq / period_value) * 8);
  }
}
void TMR5_GLOBAL_IRQHandler(void)
{
  if (tmr_interrupt_flag_get(TMR5, TMR_C4_FLAG) != RESET)
  {
    /* get the input capture value */
    tmp_c4[capture_number++] = tmr_channel_value_get(TMR5, TMR_SELECT_CHANNEL_4);

    /* clear c4 interrupt pending bit */
    tmr_flag_clear(TMR5, TMR_C4_FLAG);

    if (capture_number >= 2)
    {
      /* compute the period length */
      period_value = (uint16_t)(0xFFFF - tmp_c4[0] + tmp_c4[1] + 1);
    }
  }
}
/**********************************************************************
*  函数名称:  RTC_Fre_Calib
*  功能描述:  RTC频率校准
***********************************************************************/
void RTC_Fre_Calib(void)
{
  lick_freq = lick_frequency_get();
  ertc_divider_set(127, (lick_freq / 128) - 1);
}
/**********************************************************************
*  函数名称:  Hal_Ertc_Alarm_Config
*  功能描述:  闹钟设置
***********************************************************************/
void Hal_Ertc_Alarm_Config(void)
{
  exint_init_type exint_init_struct;

  /* config the exint line of the ertc alarm */
  exint_init_struct.line_select   = EXINT_LINE_17;
  exint_init_struct.line_enable   = TRUE;
  exint_init_struct.line_mode     = EXINT_LINE_INTERRUPUT;
  exint_init_struct.line_polarity = EXINT_TRIGGER_RISING_EDGE;
  exint_init(&exint_init_struct);
  
  /* 设置闹钟屏蔽：闹钟A，屏蔽日期、小时 */
  ertc_alarm_mask_set(ERTC_ALA, ERTC_ALARM_MASK_DATE_WEEK);
  /* 闹钟时间格式选择：日期 */
  ertc_alarm_week_date_select(ERTC_ALA, ERTC_SLECT_DATE);
}
/*----------------------------------------------------------------------------
函数名称： Hal_Alarm_Init
具体描述： RTC闹钟配置
输入参数： u8min 设定睡眠秒数
----------------------------------------------------------------------------*/
void Hal_Alarm_Init(uint16_t u16Sec)
{  
	ertc_time_type ertc_time_struct;
  
  uint16_t u16SetSecond = 0,u16SetMinutes = 0,u16SetHour = 0;
  ertc_calendar_get(&ertc_time_struct);
  u16SetSecond = u16Sec + ertc_time_struct.sec;
  u16SetMinutes = ertc_time_struct.min;
  u16SetHour = ertc_time_struct.hour;
  
  Hal_Ertc_Alarm_Config();
  
  /* 定时秒转小时分钟秒 */
  if(u16SetSecond>59)
  {
      u16SetMinutes+=(u16SetSecond/60);
      u16SetSecond = u16SetSecond%60;  
  }
  if(u16SetMinutes>59)
  {
      u16SetHour+=(u16SetMinutes/60);
      u16SetMinutes = u16SetMinutes%60;
  }
  if(u16SetHour>23)
  {
      u16SetHour = u16SetHour%24;
  }
  /* 关闭闹钟 */
  ertc_alarm_enable(ERTC_ALA, FALSE);
  /* 设置闹钟时间 */
  #if LOG 
  printf("Alarm Time : %02d:%02d:%02d\r\n",(uint8_t)u16SetHour, (uint8_t)u16SetMinutes, (uint8_t)u16SetSecond);
  #endif
  ertc_alarm_set(ERTC_ALA, ertc_time_struct.day, (uint8_t)u16SetHour, (uint8_t)u16SetMinutes, (uint8_t)u16SetSecond, ertc_time_struct.ampm);
  nvic_irq_enable(ERTCAlarm_IRQn, 0, 0);
  ertc_interrupt_enable(ERTC_ALA_INT, TRUE);
  /* 开启闹钟 */
  ertc_alarm_enable(ERTC_ALA, TRUE);
}
/*----------------------------------------------------------------------------
函数名称： RTC_Printf
具体描述： RTC时间打印
----------------------------------------------------------------------------*/
void RTC_Printf(void)
{
  ertc_calendar_get(&timertc);
  /* display time format : hour:min:sec */
  printf("RTC Time : %02d:%02d:%02d\r\n",timertc.hour, timertc.min, timertc.sec);
}
/*----------------------------------------------------------------------------
函数名称： Hal_Alarm_Deinit
具体描述： RTC闹钟失能
----------------------------------------------------------------------------*/
void Hal_Alarm_Deinit(void)
{
  ertc_interrupt_enable(ERTC_ALA_INT, FALSE);
  ertc_alarm_enable(ERTC_ALA, FALSE);
}

/*----------------------------------------------------------------------------
函数名称： Hal_System_Clock_Recover
具体描述： 系统时钟恢复
----------------------------------------------------------------------------*/
void Hal_System_Clock_Recover(void)
{
  /* 恢复SysTick配置 */
  SysTick->CTRL |= systick_index;
  /* 等待时钟稳定 */
  delay_us(200);
    /* 时钟源使能  ：外部时钟源使能*/
  crm_clock_source_enable(CRM_CLOCK_SOURCE_HEXT, TRUE);
  /* 等待外部时钟稳定 */
  while(crm_hext_stable_wait() == ERROR);
  /* 使能PLL */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_PLL, TRUE);
  /* 等待PLL时钟稳定 */
  while(crm_flag_get(CRM_PLL_STABLE_FLAG) == RESET);
  /* 自动顺滑使能设置 ：使能*/
  crm_auto_step_mode_enable(TRUE);
  /* 系统时钟源 ： 选择 PLL 时钟作为系统时钟*/
  crm_sysclk_switch(CRM_SCLK_PLL);
  /* 等待等待PLL时钟使能使能 */
  while(crm_sysclk_switch_status_get() != CRM_SCLK_PLL);
}
/*----------------------------------------------------------------------------
函数名称： Hal_Config_Sys_From_Deep_Sleep
具体描述： 运行恢复
----------------------------------------------------------------------------*/
void Hal_Config_Sys_From_Deep_Sleep(void)
{
    Hal_System_Clock_Recover();
}
/*----------------------------------------------------------------------------
函数名称： Hal_Wdt_Init
具体描述： 看门狗初始化
----------------------------------------------------------------------------*/
void Hal_Wdt_Init(void)
{
  /* WWDT时钟开启 */
  crm_periph_clock_enable(CRM_WWDT_PERIPH_CLOCK, TRUE);

  /* 设置看门狗时钟分频 */
  wwdt_divider_set(WWDT_PCLK1_DIV_32768);

  /* 设置重加载值

   window_value:   (0x7F - 0x7E)
   timeout_value:  (0x7F - 0x40) + 1

   timeout = timeout_value * (divider / pclk1_freq )    (s)
   window  = window_value  * (divider / pclk1_freq )    (s)

   pclk1_freq   = 144 MHz
   divider      = 32768
   reload_value = 0x40 = 64
   window_value = 0x01 = 01

   timeout = 64 * (32768 / 144000000 ) = 0.0174s = 17.4ms
   window  = 16 * (32768 / 120000000 ) = 0.0044s = 4.4ms
  */
  wwdt_enable(0x7F);
}
/*----------------------------------------------------------------------------
函数名称： Hal_Wdt_Deinit
具体描述： 看门狗失能
----------------------------------------------------------------------------*/
void Hal_Wdt_Deinit(void)
{
  wwdt_reset();
}
/*----------------------------------------------------------------------------
函数名称： Hal_Feed_Dog
具体描述： 喂狗
----------------------------------------------------------------------------*/
void Hal_Feed_Dog(void)
{
  wwdt_counter_set(0x7F);
}
/*----------------------------------------------------------------------------
函数名称： Hal_Get_Into_Sleep
具体描述： 进入睡眠模式
输入参数： u8min 设定睡眠秒数
----------------------------------------------------------------------------*/
void Hal_Get_Into_Sleep(uint16_t u16sec)
{    
  /* 串口中断关闭 */
  Hal_Uart1_Deinit();
  Hal_Uart2_Deinit();
  Hal_Uart3_Deinit();
  Hal_Uart4_Deinit();
  /* 外部电源关闭 */
  USB_CH_DIS();
  GNSS_POW_DIS();
  RS422_POW_DIS();
  RDSS_POW_DIS();
  
  /* IO关闭 */
  /* 看门狗关闭 */
  //Hal_Wdt_Deinit();
  /* 保存SYSTICK配置 */
  systick_index = SysTick->CTRL;
  systick_index &= ~((uint32_t)0xFFFFFFFE);
  /* 关闭systick */
  SysTick->CTRL &= (uint32_t)0xFFFFFFFE;
  Hal_Alarm_Init(u16sec);
  Hal_Exit_Side_Init();
  Hal_Exit_Bot_Init();
  Hal_Exit_Slide_Init();
  
  /* 进入睡眠模式 */
  pwc_deep_sleep_mode_enter(PWC_DEEP_SLEEP_ENTER_WFI);
  Hal_Config_Sys_From_Deep_Sleep();
  Hal_Exit_Side_Deinit();
  Hal_Exit_Bot_Deinit();
  Hal_Exit_Slide_Deinit();
  //Hal_Wdt_Init();
}
/*----------------------------------------------------------------------------
函数名称： ERTC_IRQHandler
具体描述： RTC中断服务
----------------------------------------------------------------------------*/
void ERTCAlarm_IRQHandler(void)
{
  if(ertc_interrupt_flag_get(ERTC_ALAF_FLAG) != RESET)
  {
    /* clear ertc alarm flag */
    ertc_flag_clear(ERTC_ALAF_FLAG);

    /* clear exint line flag */
    exint_flag_clear(EXINT_LINE_17);
  }
}
