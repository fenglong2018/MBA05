/*************************************************************************
*  	�ļ�����:  	Hal_Init.c
*  	�ļ�����:  	MCU�����ʼ����ʱ�ӳ�ʼ����IO��UART����ʱ�����жϡ�ADC�ȵĳ�ʼ��
*  	��ʷ��¼:
*  	����ʱ��:  	2024-05-10
*  	������:  	  ������
*  	�޸�ʱ��:
*	  �޸���:
*   ����:
*************************************************************************/

/*************************************************************************
ͷ�ļ���������
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

/*************************************************************************
��̬��������

extern �ⲿ��������
*************************************************************************/

/*************************************************************************
��������
*************************************************************************/
/*----------------------------------------------------------------------------
�������ƣ� Hal_System_Clock_Init
���������� ϵͳʱ��
----------------------------------------------------------------------------*/
void Hal_System_Clock_Init(void)
{
	
	/* ʱ������ */
  crm_reset();

  /* ����PWC��Χʱ�� */
  crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);

  /* ���õ�ѹ */
  pwc_ldo_output_voltage_set(PWC_LDO_OUTPUT_1V3);

  /* ����FLASHʱ�ӷ�Ƶ */
  flash_clock_divider_set(FLASH_CLOCK_DIV_3);

	/* ʹ���ⲿʱ�� */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_HEXT, TRUE);

  /* �ȴ��ⲿʱ�Ӿ��� */
  while(crm_hext_stable_wait() == ERROR)
  {
  }

  /* ����PLLʱ����Դ ��*/
  crm_pll_config(CRM_PLL_SOURCE_HEXT, 96, 2, CRM_PLL_FR_8);

  /* ʹ��PLL */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_PLL, TRUE);

  /* �ȴ�PLL���� */
  while(crm_flag_get(CRM_PLL_STABLE_FLAG) != SET)
  {
  }

  /* ����AHB��Ƶ */
  crm_ahb_div_set(CRM_AHB_DIV_1);

  /* ����APB2ʱ�ӣ�/1=144MHZ  */
  crm_apb2_div_set(CRM_APB2_DIV_1);

  /* ����APB1ʱ�ӣ�/1=144MHZ  */
  crm_apb1_div_set(CRM_APB1_DIV_1);

  /* ʹ���Զ�����ģʽ */
  crm_auto_step_mode_enable(TRUE);

  /* ѡ��PLLΪϵͳʱ�� */
  crm_sysclk_switch(CRM_SCLK_PLL);

  /* �ȴ�PLLʱ��Ϊϵͳʱ�� */
  while(crm_sysclk_switch_status_get() != CRM_SCLK_PLL)
  {
  }

  /* ʧ���Զ�����ģʽ */
  crm_auto_step_mode_enable(FALSE);

  /* ����ϵͳʱ��ȫ�ֱ��� */
  system_core_clock_update();
	
  /* ����GPIOAʱ�� */
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK,TRUE);
  /* ����GPIOBʱ�� */
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK,TRUE);
  /* ����GPIOCʱ�� */
  crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK,TRUE);
  /* ����GPIOHʱ�� */
  crm_periph_clock_enable(CRM_GPIOH_PERIPH_CLOCK,TRUE);
  
  crm_periph_clock_enable(CRM_SCFG_PERIPH_CLOCK, TRUE);
  
  systick_clock_source_config(SYSTICK_CLOCK_SOURCE_AHBCLK_NODIV);
  SysTick_Config(SystemCoreClock/1000); 
}
/**********************************************************************
*  ��������:  NVIC_Init
*  ��������:  �ж�����
*  ����˵��:  ��
*  ����:      ��
*  ���:      ��
*  ����ֵ:    ��
***********************************************************************/
void NVIC_Init(void)
{
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
}

/*----------------------------------------------------------------------------
�������ƣ� Mcu_Hal_Init
���������� MCUӲ�������ʼ��
----------------------------------------------------------------------------*/
void Mcu_Hal_Init(void)
{
    /* �ж������� */
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

