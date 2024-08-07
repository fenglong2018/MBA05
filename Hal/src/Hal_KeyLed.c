/*************************************************************************
*  	文件名称:  	Hal_KeyLed.c
*  	文件描述:  	按键初始化、LED初始化、按键检测、LED驱动
*  	历史记录:
*  	创建时间:  	2024-05-09
*  	创建人:  	江昌钤
*  	修改时间:
*	修改人:
*   描述:
*************************************************************************/

/*************************************************************************
头文件引用声明
*************************************************************************/
#include    "at32f435_437_gpio.h"
#include    "at32f435_437_scfg.h"

#include    "Hal_KeyLed.h"
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
函数名称： Hal_Key_Init
具体描述： 按键初始化
----------------------------------------------------------------------------*/
void Hal_Key_Init(void)
{
  gpio_init_type  gpio_init_struct;
  
  gpio_default_para_init(&gpio_init_struct);
  
  /*   磁吸按键   */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init_struct.gpio_pins = IO_MAGKEY;
  gpio_init(PT_MAGKEY, &gpio_init_struct);
  
  
  /*   侧面开关   */
  gpio_init_struct.gpio_pins = IO_SIDEKEY;
  gpio_init(PT_SIDEKEY, &gpio_init_struct); 

  /*   底面开关   */
  gpio_init_struct.gpio_pins = IO_BOTKEY;
  gpio_init(PT_BOTKEY, &gpio_init_struct); 

  /*   滑动开关   */
  gpio_init_struct.gpio_pins = IO_SLIDEKEY;
  gpio_init(PT_SLIDEKEY, &gpio_init_struct);   
}
/*----------------------------------------------------------------------------
函数名称： Hal_Led_Init
具体描述： LED初始化
----------------------------------------------------------------------------*/
void Hal_Led_Init(void)
{
  gpio_init_type  gpio_init_struct;
     
  gpio_default_para_init(&gpio_init_struct);
  /*   电量灯1   */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_pins = IO_LED_BAT1;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(PT_LED_BAT1, &gpio_init_struct);
  /*   电量灯2   */
  gpio_init_struct.gpio_pins = IO_LED_BAT2;
  gpio_init(PT_LED_BAT2, &gpio_init_struct);
  /*   电量灯3  */
  gpio_init_struct.gpio_pins = IO_LED_BAT3;
  gpio_init(PT_LED_BAT3, &gpio_init_struct);
  /*   电量灯4   */
  gpio_init_struct.gpio_pins = IO_LED_BAT4;
  gpio_init(PT_LED_BAT4, &gpio_init_struct);
  
  /*   故障灯   */
  gpio_init_struct.gpio_pins = IO_LED_ERROR;
  gpio_init(PT_LED_ERROR, &gpio_init_struct);
  
  LED_ERLED_DIS();
  LED_BAT1_DIS();
  LED_BAT2_DIS();
  LED_BAT3_DIS();
  LED_BAT4_DIS();
}
/*----------------------------------------------------------------------------
函数名称： Hal_Exit_Side_Init
具体描述： 侧面开关外部中断线初始化
----------------------------------------------------------------------------*/
void Hal_Exit_Side_Init(void)
{ 
  exint_init_type exint_init_struct;

  scfg_exint_line_config(SCFG_PORT_SOURCE_GPIOA, SCFG_PINS_SOURCE1);

  exint_default_para_init(&exint_init_struct);
  exint_init_struct.line_enable = TRUE;
  exint_init_struct.line_mode = EXINT_LINE_INTERRUPUT;
  exint_init_struct.line_select = EXINT_LINE_1;
  exint_init_struct.line_polarity = EXINT_TRIGGER_FALLING_EDGE;
  exint_init(&exint_init_struct);

  nvic_irq_enable(EXINT1_IRQn, 1, 0);
}
/*----------------------------------------------------------------------------
函数名称： Hal_Exit_Side_Deinit
具体描述： 侧面开关外部中断线失能
----------------------------------------------------------------------------*/
void Hal_Exit_Side_Deinit(void)
{ 
	gpio_init_type gpio_init_struct;
  
  exint_reset();
  nvic_irq_disable(EXINT1_IRQn);
  
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pins = IO_SIDEKEY;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(PT_SIDEKEY, &gpio_init_struct);
}
/*----------------------------------------------------------------------------
函数名称： Hal_Exit_Bot_Init
具体描述： 底面开关外部中断线初始化
----------------------------------------------------------------------------*/
void Hal_Exit_Bot_Init(void)
{ 
  exint_init_type exint_init_struct;

  scfg_exint_line_config(SCFG_PORT_SOURCE_GPIOA, SCFG_PINS_SOURCE0);

  exint_default_para_init(&exint_init_struct);
  exint_init_struct.line_enable = TRUE;
  exint_init_struct.line_mode = EXINT_LINE_INTERRUPUT;
  exint_init_struct.line_select = EXINT_LINE_0;
  exint_init_struct.line_polarity = EXINT_TRIGGER_RISING_EDGE;
  exint_init(&exint_init_struct);

  nvic_irq_enable(EXINT0_IRQn, 1, 0);
}
/*----------------------------------------------------------------------------
函数名称： Hal_Exit_Bot_Deinit
具体描述： 底面开关外部中断线失能
----------------------------------------------------------------------------*/
void Hal_Exit_Bot_Deinit(void)
{ 
	gpio_init_type gpio_init_struct;
  
  exint_reset();
  nvic_irq_disable(EXINT0_IRQn);
  
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pins = IO_BOTKEY;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(PT_BOTKEY, &gpio_init_struct);
}
/*----------------------------------------------------------------------------
函数名称： Hal_Exit_Bot_Init
具体描述： 滑动开关外部中断线初始化
----------------------------------------------------------------------------*/
void Hal_Exit_Slide_Init(void)
{ 
  exint_init_type exint_init_struct;

  scfg_exint_line_config(SCFG_PORT_SOURCE_GPIOB, SCFG_PINS_SOURCE5);

  exint_default_para_init(&exint_init_struct);
  exint_init_struct.line_enable = TRUE;
  exint_init_struct.line_mode = EXINT_LINE_INTERRUPUT;
  exint_init_struct.line_select = EXINT_LINE_5;
  exint_init_struct.line_polarity = EXINT_TRIGGER_RISING_EDGE;
  exint_init(&exint_init_struct);

  nvic_irq_enable(EXINT9_5_IRQn, 1, 0);
}
/*----------------------------------------------------------------------------
函数名称： Hal_Exit_Bot_Deinit
具体描述： 滑动开关外部中断线失能
----------------------------------------------------------------------------*/
void Hal_Exit_Slide_Deinit(void)
{ 
	gpio_init_type gpio_init_struct;
  
  exint_reset();
  nvic_irq_disable(EXINT9_5_IRQn);
  
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pins = IO_SLIDEKEY;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(PT_SLIDEKEY, &gpio_init_struct);
}
/*----------------------------------------------------------------------------
函数名称： Hal_Get_MagKey_State
具体描述： 获取磁感应开关状态
输出参数： 1 有磁感应   
           0 无磁感应
----------------------------------------------------------------------------*/
uint8_t Hal_Get_MagKey_State(void)
{
  if(gpio_input_data_bit_read(PT_MAGKEY,IO_MAGKEY))          //按键输入
      return 0;
  else
      return 1;
}

/*----------------------------------------------------------------------------
函数名称： Hal_Get_SideKey_State
具体描述： 获取侧面开关状态
输出参数： 1 按下   
           0 弹开
----------------------------------------------------------------------------*/
uint8_t Hal_Get_SideKey_State(void)
{
  if(gpio_input_data_bit_read(PT_SIDEKEY,IO_SIDEKEY))          //按键输入
      return 0;
  else
      return 1;
}

/*----------------------------------------------------------------------------
函数名称： Hal_Get_BotKey_State
具体描述： 获取底面开关状态
输出参数： 1 按下   
           0 弹开
----------------------------------------------------------------------------*/
uint8_t Hal_Get_BotKey_State(void)
{
  if(gpio_input_data_bit_read(PT_BOTKEY,IO_BOTKEY))          //按键输入
      return 1;
  else
      return 0;
}

/*----------------------------------------------------------------------------
函数名称： Hal_Get_SlideKey_State
具体描述： 获取滑动开关状态
输出参数： 1 按下   
           0 弹开
----------------------------------------------------------------------------*/
uint8_t Hal_Get_SlideKey_State(void)
{
  if(gpio_input_data_bit_read(PT_SLIDEKEY,IO_SLIDEKEY))          //按键输入
      return 1;
  else
      return 0;
}

/*----------------------------------------------------------------------------
函数名称： Heartbeat_BAT1_Led
具体描述： 电量1心跳
----------------------------------------------------------------------------*/
void Heartbeat_BAT1_Led(void)
{
	uint8_t state_led = 0;
  state_led = gpio_output_data_bit_read(PT_LED_BAT1,IO_LED_BAT1);
  if(state_led)
  {
      gpio_bits_write(PT_LED_BAT1,IO_LED_BAT1,FALSE);
  }
  else
  {
      gpio_bits_write(PT_LED_BAT1,IO_LED_BAT1,TRUE);
  }
}
/*----------------------------------------------------------------------------
函数名称： Heartbeat_BAT2_Led
具体描述： 电量2心跳
----------------------------------------------------------------------------*/
void Heartbeat_BAT2_Led(void)
{
	uint8_t state_led = 0;
  state_led = gpio_output_data_bit_read(PT_LED_BAT2,IO_LED_BAT2);
  if(state_led)
  {
      gpio_bits_write(PT_LED_BAT2,IO_LED_BAT2,FALSE);
  }
  else
  {
      gpio_bits_write(PT_LED_BAT2,IO_LED_BAT2,TRUE);
  }
}

/*----------------------------------------------------------------------------
函数名称： Heartbeat_BAT3_Led
具体描述： 电量3心跳
----------------------------------------------------------------------------*/
void Heartbeat_BAT3_Led(void)
{
	uint8_t state_led = 0;
  state_led = gpio_output_data_bit_read(PT_LED_BAT3,IO_LED_BAT3);
  if(state_led)
  {
      gpio_bits_write(PT_LED_BAT3,IO_LED_BAT3,FALSE);
  }
  else
  {
      gpio_bits_write(PT_LED_BAT3,IO_LED_BAT3,TRUE);
  }
}
/*----------------------------------------------------------------------------
函数名称： Heartbeat_BAT4_Led
具体描述： 电量4心跳
----------------------------------------------------------------------------*/
void Heartbeat_BAT4_Led(void)
{
	uint8_t state_led = 0;
  state_led = gpio_output_data_bit_read(PT_LED_BAT4,IO_LED_BAT4);
  if(state_led)
  {
      gpio_bits_write(PT_LED_BAT4,IO_LED_BAT4,FALSE);
  }
  else
  {
      gpio_bits_write(PT_LED_BAT4,IO_LED_BAT4,TRUE);
  }
}
/*----------------------------------------------------------------------------
函数名称： Hal_Bat_Led_Display_Enable
具体描述： 电量显示LED使能
输入参数： u8LedNum  要显示的LED
           u8Enable  使能
----------------------------------------------------------------------------*/
void Hal_Bat_Led_Display_Enable(uint8_t u8LedNum,uint8_t u8Enable)
{
    if(u8LedNum == 1)
    {
        if(u8Enable)
            LED_BAT1_EN();
        else
            LED_BAT1_DIS();
    }
    else if(u8LedNum == 2)
    {
        if(u8Enable)
            LED_BAT2_EN();
        else
            LED_BAT2_DIS();
    }
    else if(u8LedNum == 3)
    {
        if(u8Enable)
            LED_BAT3_EN();
        else
            LED_BAT3_DIS();
    }
    else if(u8LedNum == 4)
    {
        if(u8Enable)
            LED_BAT4_EN();
        else
            LED_BAT4_DIS();
    }
    else
        return;
}
/*----------------------------------------------------------------------------
函数名称： Hal_Error_Led_Display_Enable
具体描述： 告警显示LED使能
输入参数： u8Enable  使能
----------------------------------------------------------------------------*/
void Hal_Error_Led_Display_Enable(uint8_t u8Enable)
{
  uint8_t state_led = 0;
  state_led = gpio_output_data_bit_read(PT_LED_ERROR,IO_LED_ERROR);
  if(u8Enable)
  {
    if(!state_led)
    {
      LED_ERLED_EN();
    }
  }
  else
  {
    if(state_led)
    {
      LED_ERLED_DIS();
    }
  } 
}
/*----------------------------------------------------------------------------
函数名称： Hal_Bat_Led_Heartbeat
具体描述： 电量显示LED闪
输入参数： u8LedNum  要闪烁的LED
----------------------------------------------------------------------------*/
void Hal_Bat_Led_Heartbeat(uint8_t u8LedNum)
{
    if(u8LedNum == 1)
        Heartbeat_BAT1_Led();
    else if(u8LedNum == 2)
        Heartbeat_BAT2_Led();
    else if(u8LedNum == 3)
        Heartbeat_BAT3_Led();
    else if(u8LedNum == 4)
        Heartbeat_BAT4_Led();
    else
        return;
}

void EXINT0_IRQHandler(void)
{
  if(exint_interrupt_flag_get(EXINT_LINE_0) != RESET)
  {
    exint_flag_clear(EXINT_LINE_0);
  }
}
void EXINT1_IRQHandler(void)
{
  if(exint_interrupt_flag_get(EXINT_LINE_1) != RESET)
  {
    exint_flag_clear(EXINT_LINE_1);
  }
}
void EXINT9_5_IRQHandler(void)
{
  if(exint_interrupt_flag_get(EXINT_LINE_5) != RESET)
  {
    exint_flag_clear(EXINT_LINE_5);
  }
}
