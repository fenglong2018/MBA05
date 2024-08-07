/*************************************************************************
*  	文件名称:  	Hal_PowManage.c
*  	文件描述:  	电源开关初始化、充电检测初始化、电量检测初始化、获取充电状态
*  	历史记录:
*  	创建时间:  	2023-03-09
*  	创建人:  	江昌钤
*  	修改时间:
*	修改人:
*   描述:
*************************************************************************/

/*************************************************************************
头文件引用声明
*************************************************************************/
#include    "at32f435_437_gpio.h"
#include    "at32f435_437_adc.h"
#include    "at32f435_437_crm.h"

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

/*************************************************************************
静态函数声明

extern 外部函数声明
*************************************************************************/

/*************************************************************************
函数定义
*************************************************************************/
/*----------------------------------------------------------------------------
函数名称： Hal_Pow_Switch_Init
具体描述： 电源开关初始化
----------------------------------------------------------------------------*/
void Hal_Pow_Switch_Init(void)
{
  gpio_init_type  gpio_init_struct;

  gpio_default_para_init(&gpio_init_struct);
  
  /*   电源使能   */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_pins = IO_POW_EN;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init(PT_POW_EN, &gpio_init_struct);
   
  /*   RDSS电源使能   */
  gpio_init_struct.gpio_pins = IO_RDSS_PA_EN;
  gpio_init(PT_RDSS_PA_EN, &gpio_init_struct);
  
  gpio_init_struct.gpio_pins = IO_RDSS_VCC_EN;
  gpio_init(PT_RDSS_VCC_EN, &gpio_init_struct); 
  
  /*   GNSS电源使能   */
  gpio_init_struct.gpio_pins = IO_GNSS_POW_EN;
  gpio_init(PT_GNSS_POW_EN, &gpio_init_struct);
  
    /*   422电源使能   */
  gpio_init_struct.gpio_pins = IO_422_POW_EN;
  gpio_init(PT_422_POW_EN, &gpio_init_struct);
  
  SYS_POW_DIS();
  GNSS_POW_DIS();
  RDSS_POW_DIS();
  RS422_POW_EN();
}
/*----------------------------------------------------------------------------
函数名称： Hal_Charge_Init
具体描述： 充电检测初始化
----------------------------------------------------------------------------*/
void Hal_Charge_Init(void)
{
  gpio_init_type  gpio_init_struct;
  
  gpio_default_para_init(&gpio_init_struct);
  
  /*   USB插入   */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init_struct.gpio_pins = IO_USBIN;
  gpio_init(PT_USBIN, &gpio_init_struct);
  
  /*   充电充满   */
  gpio_init_struct.gpio_pins = IO_CHAGFULL;
  gpio_init(PT_CHAGFULL, &gpio_init_struct);
  
  /*   充电使能   */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_pins = IO_CH_EN;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init(PT_CH_EN, &gpio_init_struct);
  
  USB_CH_EN();
}

/*----------------------------------------------------------------------------
函数名称： Hal_BatVol_Init
具体描述： 电池电量检测初始化
----------------------------------------------------------------------------*/
void Hal_BatVol_Init(void)
{
  gpio_init_type gpio_initstructure;
  adc_base_config_type adc_base_struct;
  adc_common_config_type adc_common_struct;

  /* AD GPIO初始化 */
  gpio_default_para_init(&gpio_initstructure);
  gpio_initstructure.gpio_mode = GPIO_MODE_ANALOG;
  gpio_initstructure.gpio_pins = IO_VBAT_D;
  gpio_init(PT_VBAT_D, &gpio_initstructure);
  
  gpio_initstructure.gpio_pins = IO_VSLOAR_D;
  gpio_init(PT_VSLOAR_D, &gpio_initstructure);
  /* 开AD时钟 */
  crm_periph_clock_enable(CRM_ADC1_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_ADC2_PERIPH_CLOCK, TRUE);
  
  adc_common_default_para_init(&adc_common_struct);
  
  /* 配置组合模式:独立模式 */
  adc_common_struct.combine_mode = ADC_INDEPENDENT_MODE;

  /* 配置分频：HCLK/4分频 */
  adc_common_struct.div = ADC_HCLK_DIV_4;

  /* 配置DMA模式：DMA失能 */
  adc_common_struct.common_dma_mode = ADC_COMMON_DMAMODE_DISABLE;

  /* 配置dma请求 */
  adc_common_struct.common_dma_request_repeat_state = FALSE;

  /* 配置ADC采样间隔 */
  adc_common_struct.sampling_interval = ADC_SAMPLING_INTERVAL_5CYCLES;

  /* 配置内部温度传感器：失能 */
  adc_common_struct.tempervintrv_state = TRUE;

  /* 配置电池电压：失能 */
  adc_common_struct.vbat_state = FALSE;
  /* ADC_COMMON初始化 */
  adc_common_config(&adc_common_struct);
  
  adc_base_default_para_init(&adc_base_struct);
  adc_base_struct.sequence_mode = FALSE;              //单一通道
  adc_base_struct.repeat_mode = FALSE;                 //反复转换
  adc_base_struct.data_align = ADC_RIGHT_ALIGNMENT;   //右对齐
  adc_base_struct.ordinary_channel_length = 1;        //普通转换序列长度 = 3
  /* ADC_BASE初始化 */
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
函数名称： Hal_ADC_Deinit
具体描述： ADC失能
----------------------------------------------------------------------------*/
void Hal_ADC_Deinit(void)
{
    adc_enable(ADC1, FALSE);
    adc_enable(ADC2, FALSE);
}
/*----------------------------------------------------------------------------
函数名称： Hal_Start_VBat_ADC_Convert
具体描述： 开始电池电压AD转换
----------------------------------------------------------------------------*/
void Hal_Start_VBat_ADC_Convert(void)
{
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_16, 1, ADC_SAMPLETIME_640_5);
  adc_ordinary_software_trigger_enable(ADC1, TRUE);
}
/*----------------------------------------------------------------------------
函数名称： Hal_Start_VSloar_ADC_Convert
具体描述： 开始太阳能电压AD转换
----------------------------------------------------------------------------*/
void Hal_Start_VSloar_ADC_Convert(void)
{
  adc_ordinary_channel_set(ADC2, ADC_CHANNEL_5, 2, ADC_SAMPLETIME_47_5);
  adc_ordinary_software_trigger_enable(ADC2, TRUE);
}
/*----------------------------------------------------------------------------
函数名称： Hal_Get_ADC1_Value
具体描述： 获取ADC1值
----------------------------------------------------------------------------*/
uint16_t Hal_Get_ADC1_Value(void)
{
    return adc_ordinary_conversion_data_get(ADC1); 
}
/*----------------------------------------------------------------------------
函数名称： Hal_Get_ADC2_Value
具体描述： 获取ADC1值
----------------------------------------------------------------------------*/
uint16_t Hal_Get_ADC2_Value(void)
{
    return adc_ordinary_conversion_data_get(ADC2); 
}
/*----------------------------------------------------------------------------
函数名称： Hal_Get_USB_In_State
具体描述： 获取USB插入状态
输出参数： 1 有USB插入   
           0 无USB插入
----------------------------------------------------------------------------*/
uint8_t Hal_Get_USB_In_State(void)
{
    if(gpio_input_data_bit_read(PT_USBIN,IO_USBIN))          				
        return 0;
    else
        return 1;
}

/*----------------------------------------------------------------------------
函数名称： Hal_Get_ChagFull_State
具体描述： 获取充满状态
输出参数： 1 充满   
           0 未充满
----------------------------------------------------------------------------*/
uint8_t Hal_Get_ChagFull_State(void)
{
    if(gpio_input_data_bit_read(PT_CHAGFULL,IO_CHAGFULL))          			
        return 0;
    else
        return 1;
}

