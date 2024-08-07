/*************************************************************************
*  	�ļ�����:  	Hal_Sleep.c
*  	�ļ�����:  	���ڳ�ʼ���������ַ�������
*  	��ʷ��¼:
*  	����ʱ��:  	2024-05-10
*  	������:  	������
*  	�޸�ʱ��:
*	�޸���:
*   ����:
*************************************************************************/

/*************************************************************************
ͷ�ļ���������
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
�궨��
*************************************************************************/

/*************************************************************************
�ڲ��ṹ��ö�١�������Ƚṹ����
*************************************************************************/

/*************************************************************************
�ڲ���̬��������

ȫ�ֱ�������

extern �ⲿ��������

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
��̬��������

extern �ⲿ��������
*************************************************************************/

/*************************************************************************
��������
*************************************************************************/
void delay_us(uint8_t u8us)
{
  uint16_t  u16coni,u16conj;
  for(u16coni = 0;u16coni<u8us;u16coni++)
  for(u16conj = 0;u16conj<10;u16conj++);
}

/*----------------------------------------------------------------------------
�������ƣ� Hal_RTC_init
���������� RTC��ʼ��
��������� u8Hour    Сʱ
           u8Min     ����
           u8Sec     ��
----------------------------------------------------------------------------*/
void Hal_RTC_init(uint8_t u8Hour,uint8_t u8Min,uint8_t u8Sec)
{
  /* ������Դ����ʱ�� */
  crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);

  /* ��ع��������д��ʹ�� */
  pwc_battery_powered_domain_access(TRUE);

  /* ��ع�����ĸ�λ���� */
  crm_battery_powered_domain_reset(TRUE);
  crm_battery_powered_domain_reset(FALSE);

  /* �ڲ�����ʱ��ʹ�� */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_LICK, TRUE);

  /* �ȴ��ڲ�����ʱ�Ӿ��� */
  while(crm_flag_get(CRM_LICK_STABLE_FLAG) == RESET)
  {
  }

  /* ѡ��RTC��ʱ��Դ */
  crm_ertc_clock_select(CRM_ERTC_CLOCK_LICK);
  /* RTCʱ��ʹ�� */
  crm_ertc_clock_enable(TRUE);
  /* RTC��λ */
  ertc_reset();
  /* �ȴ�RTC�Ĵ���������� */
  ertc_wait_update();
  /* ��Ƶ������ */
  ertc_divider_set(0x7F, 0xFF);
  /* Сʱģʽ���ã�24Сʱ�� */
  ertc_hour_mode_set(ERTC_HOUR_MODE_24);
  /* ��������: 2023-05-01 */
  ertc_date_set(23, 5, 1, 5);
  /* ʱ������: 0:00:00 */
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
*  ��������:  RTC_Fre_Calib
*  ��������:  RTCƵ��У׼
***********************************************************************/
void RTC_Fre_Calib(void)
{
  lick_freq = lick_frequency_get();
  ertc_divider_set(127, (lick_freq / 128) - 1);
}
/**********************************************************************
*  ��������:  Hal_Ertc_Alarm_Config
*  ��������:  ��������
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
  
  /* �����������Σ�����A���������ڡ�Сʱ */
  ertc_alarm_mask_set(ERTC_ALA, ERTC_ALARM_MASK_DATE_WEEK);
  /* ����ʱ���ʽѡ������ */
  ertc_alarm_week_date_select(ERTC_ALA, ERTC_SLECT_DATE);
}
/*----------------------------------------------------------------------------
�������ƣ� Hal_Alarm_Init
���������� RTC��������
��������� u8min �趨˯������
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
  
  /* ��ʱ��תСʱ������ */
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
  /* �ر����� */
  ertc_alarm_enable(ERTC_ALA, FALSE);
  /* ��������ʱ�� */
  #if LOG 
  printf("Alarm Time : %02d:%02d:%02d\r\n",(uint8_t)u16SetHour, (uint8_t)u16SetMinutes, (uint8_t)u16SetSecond);
  #endif
  ertc_alarm_set(ERTC_ALA, ertc_time_struct.day, (uint8_t)u16SetHour, (uint8_t)u16SetMinutes, (uint8_t)u16SetSecond, ertc_time_struct.ampm);
  nvic_irq_enable(ERTCAlarm_IRQn, 0, 0);
  ertc_interrupt_enable(ERTC_ALA_INT, TRUE);
  /* �������� */
  ertc_alarm_enable(ERTC_ALA, TRUE);
}
/*----------------------------------------------------------------------------
�������ƣ� RTC_Printf
���������� RTCʱ���ӡ
----------------------------------------------------------------------------*/
void RTC_Printf(void)
{
  ertc_calendar_get(&timertc);
  /* display time format : hour:min:sec */
  printf("RTC Time : %02d:%02d:%02d\r\n",timertc.hour, timertc.min, timertc.sec);
}
/*----------------------------------------------------------------------------
�������ƣ� Hal_Alarm_Deinit
���������� RTC����ʧ��
----------------------------------------------------------------------------*/
void Hal_Alarm_Deinit(void)
{
  ertc_interrupt_enable(ERTC_ALA_INT, FALSE);
  ertc_alarm_enable(ERTC_ALA, FALSE);
}

/*----------------------------------------------------------------------------
�������ƣ� Hal_System_Clock_Recover
���������� ϵͳʱ�ӻָ�
----------------------------------------------------------------------------*/
void Hal_System_Clock_Recover(void)
{
  /* �ָ�SysTick���� */
  SysTick->CTRL |= systick_index;
  /* �ȴ�ʱ���ȶ� */
  delay_us(200);
    /* ʱ��Դʹ��  ���ⲿʱ��Դʹ��*/
  crm_clock_source_enable(CRM_CLOCK_SOURCE_HEXT, TRUE);
  /* �ȴ��ⲿʱ���ȶ� */
  while(crm_hext_stable_wait() == ERROR);
  /* ʹ��PLL */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_PLL, TRUE);
  /* �ȴ�PLLʱ���ȶ� */
  while(crm_flag_get(CRM_PLL_STABLE_FLAG) == RESET);
  /* �Զ�˳��ʹ������ ��ʹ��*/
  crm_auto_step_mode_enable(TRUE);
  /* ϵͳʱ��Դ �� ѡ�� PLL ʱ����Ϊϵͳʱ��*/
  crm_sysclk_switch(CRM_SCLK_PLL);
  /* �ȴ��ȴ�PLLʱ��ʹ��ʹ�� */
  while(crm_sysclk_switch_status_get() != CRM_SCLK_PLL);
}
/*----------------------------------------------------------------------------
�������ƣ� Hal_Config_Sys_From_Deep_Sleep
���������� ���лָ�
----------------------------------------------------------------------------*/
void Hal_Config_Sys_From_Deep_Sleep(void)
{
    Hal_System_Clock_Recover();
}
/*----------------------------------------------------------------------------
�������ƣ� Hal_Wdt_Init
���������� ���Ź���ʼ��
----------------------------------------------------------------------------*/
void Hal_Wdt_Init(void)
{
  /* WWDTʱ�ӿ��� */
  crm_periph_clock_enable(CRM_WWDT_PERIPH_CLOCK, TRUE);

  /* ���ÿ��Ź�ʱ�ӷ�Ƶ */
  wwdt_divider_set(WWDT_PCLK1_DIV_32768);

  /* �����ؼ���ֵ

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
�������ƣ� Hal_Wdt_Deinit
���������� ���Ź�ʧ��
----------------------------------------------------------------------------*/
void Hal_Wdt_Deinit(void)
{
  wwdt_reset();
}
/*----------------------------------------------------------------------------
�������ƣ� Hal_Feed_Dog
���������� ι��
----------------------------------------------------------------------------*/
void Hal_Feed_Dog(void)
{
  wwdt_counter_set(0x7F);
}
/*----------------------------------------------------------------------------
�������ƣ� Hal_Get_Into_Sleep
���������� ����˯��ģʽ
��������� u8min �趨˯������
----------------------------------------------------------------------------*/
void Hal_Get_Into_Sleep(uint16_t u16sec)
{    
  /* �����жϹر� */
  Hal_Uart1_Deinit();
  Hal_Uart2_Deinit();
  Hal_Uart3_Deinit();
  Hal_Uart4_Deinit();
  /* �ⲿ��Դ�ر� */
  USB_CH_DIS();
  GNSS_POW_DIS();
  RS422_POW_DIS();
  RDSS_POW_DIS();
  
  /* IO�ر� */
  /* ���Ź��ر� */
  //Hal_Wdt_Deinit();
  /* ����SYSTICK���� */
  systick_index = SysTick->CTRL;
  systick_index &= ~((uint32_t)0xFFFFFFFE);
  /* �ر�systick */
  SysTick->CTRL &= (uint32_t)0xFFFFFFFE;
  Hal_Alarm_Init(u16sec);
  Hal_Exit_Side_Init();
  Hal_Exit_Bot_Init();
  Hal_Exit_Slide_Init();
  
  /* ����˯��ģʽ */
  pwc_deep_sleep_mode_enter(PWC_DEEP_SLEEP_ENTER_WFI);
  Hal_Config_Sys_From_Deep_Sleep();
  Hal_Exit_Side_Deinit();
  Hal_Exit_Bot_Deinit();
  Hal_Exit_Slide_Deinit();
  //Hal_Wdt_Init();
}
/*----------------------------------------------------------------------------
�������ƣ� ERTC_IRQHandler
���������� RTC�жϷ���
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
