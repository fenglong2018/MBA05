/*************************************************************************
*  	�ļ�����:  	Hal_PowManage.c
*  	�ļ�����:  	��Դ���س�ʼ����������ʼ������������ʼ������ȡ���״̬
*  	��ʷ��¼:
*  	����ʱ��:  	2023-03-09
*  	������:  	������
*  	�޸�ʱ��:
*	�޸���:
*   ����:
*************************************************************************/

/*************************************************************************
ͷ�ļ���������
*************************************************************************/
#include    "at32f435_437_gpio.h"
#include    "at32f435_437_adc.h"
#include    "at32f435_437_crm.h"

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

/*************************************************************************
��̬��������

extern �ⲿ��������
*************************************************************************/

/*************************************************************************
��������
*************************************************************************/
/*----------------------------------------------------------------------------
�������ƣ� Hal_Pow_Switch_Init
���������� ��Դ���س�ʼ��
----------------------------------------------------------------------------*/
void Hal_Pow_Switch_Init(void)
{
  gpio_init_type  gpio_init_struct;

  gpio_default_para_init(&gpio_init_struct);
  
  /*   ��Դʹ��   */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_pins = IO_POW_EN;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init(PT_POW_EN, &gpio_init_struct);
   
  /*   RDSS��Դʹ��   */
  gpio_init_struct.gpio_pins = IO_RDSS_PA_EN;
  gpio_init(PT_RDSS_PA_EN, &gpio_init_struct);
  
  gpio_init_struct.gpio_pins = IO_RDSS_VCC_EN;
  gpio_init(PT_RDSS_VCC_EN, &gpio_init_struct); 
  
  /*   GNSS��Դʹ��   */
  gpio_init_struct.gpio_pins = IO_GNSS_POW_EN;
  gpio_init(PT_GNSS_POW_EN, &gpio_init_struct);
  
    /*   422��Դʹ��   */
  gpio_init_struct.gpio_pins = IO_422_POW_EN;
  gpio_init(PT_422_POW_EN, &gpio_init_struct);
  
  SYS_POW_DIS();
  GNSS_POW_DIS();
  RDSS_POW_DIS();
  RS422_POW_EN();
}
/*----------------------------------------------------------------------------
�������ƣ� Hal_Charge_Init
���������� ������ʼ��
----------------------------------------------------------------------------*/
void Hal_Charge_Init(void)
{
  gpio_init_type  gpio_init_struct;
  
  gpio_default_para_init(&gpio_init_struct);
  
  /*   USB����   */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init_struct.gpio_pins = IO_USBIN;
  gpio_init(PT_USBIN, &gpio_init_struct);
  
  /*   ������   */
  gpio_init_struct.gpio_pins = IO_CHAGFULL;
  gpio_init(PT_CHAGFULL, &gpio_init_struct);
  
  /*   ���ʹ��   */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_pins = IO_CH_EN;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init(PT_CH_EN, &gpio_init_struct);
  
  USB_CH_EN();
}

/*----------------------------------------------------------------------------
�������ƣ� Hal_BatVol_Init
���������� ��ص�������ʼ��
----------------------------------------------------------------------------*/
void Hal_BatVol_Init(void)
{
  gpio_init_type gpio_initstructure;
  adc_base_config_type adc_base_struct;
  adc_common_config_type adc_common_struct;

  /* AD GPIO��ʼ�� */
  gpio_default_para_init(&gpio_initstructure);
  gpio_initstructure.gpio_mode = GPIO_MODE_ANALOG;
  gpio_initstructure.gpio_pins = IO_VBAT_D;
  gpio_init(PT_VBAT_D, &gpio_initstructure);
  
  gpio_initstructure.gpio_pins = IO_VSLOAR_D;
  gpio_init(PT_VSLOAR_D, &gpio_initstructure);
  /* ��ADʱ�� */
  crm_periph_clock_enable(CRM_ADC1_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_ADC2_PERIPH_CLOCK, TRUE);
  
  adc_common_default_para_init(&adc_common_struct);
  
  /* �������ģʽ:����ģʽ */
  adc_common_struct.combine_mode = ADC_INDEPENDENT_MODE;

  /* ���÷�Ƶ��HCLK/4��Ƶ */
  adc_common_struct.div = ADC_HCLK_DIV_4;

  /* ����DMAģʽ��DMAʧ�� */
  adc_common_struct.common_dma_mode = ADC_COMMON_DMAMODE_DISABLE;

  /* ����dma���� */
  adc_common_struct.common_dma_request_repeat_state = FALSE;

  /* ����ADC������� */
  adc_common_struct.sampling_interval = ADC_SAMPLING_INTERVAL_5CYCLES;

  /* �����ڲ��¶ȴ�������ʧ�� */
  adc_common_struct.tempervintrv_state = TRUE;

  /* ���õ�ص�ѹ��ʧ�� */
  adc_common_struct.vbat_state = FALSE;
  /* ADC_COMMON��ʼ�� */
  adc_common_config(&adc_common_struct);
  
  adc_base_default_para_init(&adc_base_struct);
  adc_base_struct.sequence_mode = FALSE;              //��һͨ��
  adc_base_struct.repeat_mode = FALSE;                 //����ת��
  adc_base_struct.data_align = ADC_RIGHT_ALIGNMENT;   //�Ҷ���
  adc_base_struct.ordinary_channel_length = 1;        //��ͨת�����г��� = 3
  /* ADC_BASE��ʼ�� */
  adc_base_config(ADC1, &adc_base_struct);
  adc_base_config(ADC2, &adc_base_struct);
  
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_16, 1, ADC_SAMPLETIME_640_5);
  adc_ordinary_channel_set(ADC2, ADC_CHANNEL_5, 2, ADC_SAMPLETIME_47_5);
  adc_ordinary_software_trigger_enable(ADC1, TRUE);
  adc_ordinary_software_trigger_enable(ADC2, TRUE);
  adc_enable(ADC1, TRUE);
  adc_enable(ADC2, TRUE);
}
/*----------------------------------------------------------------------------
�������ƣ� Hal_ADC_Deinit
���������� ADCʧ��
----------------------------------------------------------------------------*/
void Hal_ADC_Deinit(void)
{
    adc_enable(ADC1, FALSE);
    adc_enable(ADC2, FALSE);
}
/*----------------------------------------------------------------------------
�������ƣ� Hal_Start_VBat_ADC_Convert
���������� ��ʼ��ص�ѹADת��
----------------------------------------------------------------------------*/
void Hal_Start_VBat_ADC_Convert(void)
{
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_16, 1, ADC_SAMPLETIME_640_5);
  adc_ordinary_software_trigger_enable(ADC1, TRUE);
}
/*----------------------------------------------------------------------------
�������ƣ� Hal_Start_VSloar_ADC_Convert
���������� ��ʼ̫���ܵ�ѹADת��
----------------------------------------------------------------------------*/
void Hal_Start_VSloar_ADC_Convert(void)
{
  adc_ordinary_channel_set(ADC2, ADC_CHANNEL_5, 2, ADC_SAMPLETIME_47_5);
  adc_ordinary_software_trigger_enable(ADC2, TRUE);
}
/*----------------------------------------------------------------------------
�������ƣ� Hal_Get_ADC1_Value
���������� ��ȡADC1ֵ
----------------------------------------------------------------------------*/
uint16_t Hal_Get_ADC1_Value(void)
{
    return adc_ordinary_conversion_data_get(ADC1); 
}
/*----------------------------------------------------------------------------
�������ƣ� Hal_Get_ADC2_Value
���������� ��ȡADC1ֵ
----------------------------------------------------------------------------*/
uint16_t Hal_Get_ADC2_Value(void)
{
    return adc_ordinary_conversion_data_get(ADC2); 
}
/*----------------------------------------------------------------------------
�������ƣ� Hal_Get_USB_In_State
���������� ��ȡUSB����״̬
��������� 1 ��USB����   
           0 ��USB����
----------------------------------------------------------------------------*/
uint8_t Hal_Get_USB_In_State(void)
{
    if(gpio_input_data_bit_read(PT_USBIN,IO_USBIN))          				
        return 0;
    else
        return 1;
}

/*----------------------------------------------------------------------------
�������ƣ� Hal_Get_ChagFull_State
���������� ��ȡ����״̬
��������� 1 ����   
           0 δ����
----------------------------------------------------------------------------*/
uint8_t Hal_Get_ChagFull_State(void)
{
    if(gpio_input_data_bit_read(PT_CHAGFULL,IO_CHAGFULL))          			
        return 0;
    else
        return 1;
}

