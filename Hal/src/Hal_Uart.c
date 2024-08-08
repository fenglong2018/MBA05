/*************************************************************************
*  	文件名称:  	Hal_Uart.c
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
#include    "at32f435_437_usart.h"
#include    "at32f435_437_misc.h"

#include    "Hal_Uart.h"
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
函数名称： Hal_Uart1_Init
具体描述： 串口1初始化
----------------------------------------------------------------------------*/
void Hal_Uart1_Init(void)
{
  gpio_init_type gpio_init_struct;
  
  /* 开启USART1时钟 */
  crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK,TRUE);
  
  /* 串口GPIO配置 */
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = IO_UART1_TX ;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(PT_UART1_TX, &gpio_init_struct);
  
  gpio_init_struct.gpio_pins = IO_UART1_RX ;
  gpio_init(PT_UART1_RX, &gpio_init_struct);
  
  /* 串口引脚IOMUX配置 */
  gpio_pin_mux_config(PT_UART1_TX, NUM_UART1_TX, IOMUX_UART1);
  gpio_pin_mux_config(PT_UART1_RX, NUM_UART1_RX, IOMUX_UART1);
  
  /* 中断优先级配置 */
  nvic_irq_enable(USART1_IRQn, 0, 0);
  
  /* 串口初始化 */
  usart_init(USART1, 115200, USART_DATA_8BITS, USART_STOP_1_BIT);
  /* 串口发送使能 */
  usart_transmitter_enable(USART1, TRUE);
  /* 串口接收使能 */
  usart_receiver_enable(USART1, TRUE);
  /* 串口中断使能 */
  usart_interrupt_enable(USART1, USART_RDBF_INT, TRUE);
  /* 串口使能 */
  usart_enable(USART1, TRUE);
}
/*----------------------------------------------------------------------------
函数名称： Hal_Uart1_Deinit
具体描述： 串口1失能
----------------------------------------------------------------------------*/
void Hal_Uart1_Deinit(void)
{
    usart_enable(USART1, FALSE);
    usart_interrupt_enable(USART1, USART_RDBF_INT, FALSE);
}
/*----------------------------------------------------------------------------
函数名称： Hal_Uart2_Init
具体描述： 串口2初始化
----------------------------------------------------------------------------*/
void Hal_Uart2_Init(void)
{
  gpio_init_type gpio_init_struct;
  
  /* 开启USART2时钟 */
  crm_periph_clock_enable(CRM_USART2_PERIPH_CLOCK,TRUE);
  
  /* 串口GPIO配置 */
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = IO_UART2_TX ;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(PT_UART2_TX, &gpio_init_struct);
  
  gpio_init_struct.gpio_pins = IO_UART2_RX ;
  gpio_init(PT_UART2_RX, &gpio_init_struct);
  
  /* 串口引脚IOMUX配置 */
  gpio_pin_mux_config(PT_UART2_TX, NUM_UART2_TX, IOMUX_UART2);
  gpio_pin_mux_config(PT_UART2_RX, NUM_UART2_RX, IOMUX_UART2);
  
  /* 中断优先级配置 */
  nvic_irq_enable(USART2_IRQn, 0, 0);
  
  /* 串口初始化 */
  usart_init(USART2, 115200, USART_DATA_8BITS, USART_STOP_1_BIT);
  /* 串口发送使能 */
  usart_transmitter_enable(USART2, TRUE);
  /* 串口接收使能 */
  usart_receiver_enable(USART2, TRUE);
  /* 串口中断使能 */
  usart_interrupt_enable(USART2, USART_RDBF_INT, TRUE);
  /* 串口使能 */
  usart_enable(USART2, TRUE);
}
/*----------------------------------------------------------------------------
函数名称： Hal_Uart2_Deinit
具体描述： 串口2失能
----------------------------------------------------------------------------*/
void Hal_Uart2_Deinit(void)
{
    usart_enable(USART2, FALSE);
    usart_interrupt_enable(USART2, USART_RDBF_INT, FALSE);
}
/*----------------------------------------------------------------------------
函数名称： Hal_Uart3_Init
具体描述： 串口3初始化
----------------------------------------------------------------------------*/
void Hal_Uart3_Init(void)
{
  gpio_init_type gpio_init_struct;
  
  /* 开启USART3时钟 */
  crm_periph_clock_enable(CRM_USART3_PERIPH_CLOCK,TRUE);
  
  /* 串口GPIO配置 */
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = IO_UART3_TX ;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(PT_UART3_TX, &gpio_init_struct);
  
  gpio_init_struct.gpio_pins = IO_UART3_RX ;
  gpio_init(PT_UART3_RX, &gpio_init_struct);
  
  /* 串口引脚IOMUX配置 */
  gpio_pin_mux_config(PT_UART3_TX, NUM_UART3_TX, IOMUX_UART3);
  gpio_pin_mux_config(PT_UART3_RX, NUM_UART3_RX, IOMUX_UART3);
  
  /* 中断优先级配置 */
  nvic_irq_enable(USART3_IRQn, 0, 0);
  
  /* 串口初始化 */
  usart_init(USART3, 115200, USART_DATA_8BITS, USART_STOP_1_BIT);
  /* 串口发送使能 */
  usart_transmitter_enable(USART3, TRUE);
  /* 串口接收使能 */
  usart_receiver_enable(USART3, TRUE);
  usart_transmit_receive_pin_swap(USART3, TRUE);
  /* 串口中断使能 */
  usart_interrupt_enable(USART3, USART_RDBF_INT, TRUE);
  /* 串口使能 */
  usart_enable(USART3, TRUE);
}
/*----------------------------------------------------------------------------
函数名称： Hal_Uart3_Deinit
具体描述： 串口3失能
----------------------------------------------------------------------------*/
void Hal_Uart3_Deinit(void)
{
    usart_enable(USART3, FALSE);
    usart_interrupt_enable(USART3, USART_RDBF_INT, FALSE);
}
/*----------------------------------------------------------------------------
函数名称： Hal_Uart4_Init
具体描述： 串口4初始化
----------------------------------------------------------------------------*/
void Hal_Uart4_Init(void)
{
  gpio_init_type gpio_init_struct;
  
  /* 开启USART1时钟 */
  crm_periph_clock_enable(CRM_UART4_PERIPH_CLOCK,TRUE);
  
  /* 串口GPIO配置 */
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = IO_UART4_TX ;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(PT_UART4_TX, &gpio_init_struct);
  
  gpio_init_struct.gpio_pins = IO_UART4_RX ;
  gpio_init(PT_UART4_RX, &gpio_init_struct);
  
  /* 串口引脚IOMUX配置 */
  gpio_pin_mux_config(PT_UART4_TX, NUM_UART4_TX, IOMUX_UART4);
  gpio_pin_mux_config(PT_UART4_RX, NUM_UART4_RX, IOMUX_UART4);
  
  /* 中断优先级配置 */
  nvic_irq_enable(UART4_IRQn, 0, 0);
  
  /* 串口初始化 */
  usart_init(UART4, 115200, USART_DATA_8BITS, USART_STOP_1_BIT);
  /* 串口发送使能 */
  usart_transmitter_enable(UART4, TRUE);
  /* 串口接收使能 */
  usart_receiver_enable(UART4, TRUE);
  /* 串口中断使能 */
  usart_interrupt_enable(UART4, USART_RDBF_INT, TRUE);
  /* 串口使能 */
  usart_enable(UART4, TRUE);
}
/*----------------------------------------------------------------------------
函数名称： Hal_Uart4_Deinit
具体描述： 串口4失能
----------------------------------------------------------------------------*/
void Hal_Uart4_Deinit(void)
{
    usart_enable(UART4, FALSE);
    usart_interrupt_enable(UART4, USART_RDBF_INT, FALSE);
}
/*----------------------------------------------------------------------------
函数名称： uart1_tx_string
具体描述： uart1发送字符串
输入参数： * p_u8arrary 字符串数据   length 字符串长度
----------------------------------------------------------------------------*/
void uart1_tx_string(uint8_t * p_u8arrary,uint16_t length)			
{
	uint16_t i;
  for(i=0;i<length;i++)								//发送字符串
	{
    usart_data_transmit(USART1, p_u8arrary[i]);
    while(RESET == usart_flag_get(USART1, USART_TDC_FLAG));
	}
}

/*----------------------------------------------------------------------------
函数名称： uart2_tx_string
具体描述： uart2发送字符串
输入参数： * p_u8arrary 字符串数据   length 字符串长度
----------------------------------------------------------------------------*/
void uart2_tx_string(uint8_t * p_u8arrary,uint16_t length)			
{
	uint16_t i;
  for(i=0;i<length;i++)								//发送字符串
	{
    usart_data_transmit(USART2, p_u8arrary[i]);
    while(RESET == usart_flag_get(USART2, USART_TDC_FLAG));
	}
}
/*----------------------------------------------------------------------------
函数名称： uart3_tx_string
具体描述： uart3发送字符串
输入参数： * p_u8arrary 字符串数据   length 字符串长度
----------------------------------------------------------------------------*/
void uart3_tx_string(uint8_t * p_u8arrary,uint16_t length)			
{
	uint16_t i;
  for(i=0;i<length;i++)								//发送字符串
	{
    usart_data_transmit(USART3, p_u8arrary[i]);
    while(RESET == usart_flag_get(USART3, USART_TDC_FLAG));
	}
}
/*----------------------------------------------------------------------------
函数名称： uart4_tx_string
具体描述： uart4发送字符串
输入参数： * p_u8arrary 字符串数据   length 字符串长度
----------------------------------------------------------------------------*/
void uart4_tx_string(uint8_t * p_u8arrary,uint16_t length)			
{
	uint16_t i;
  for(i=0;i<length;i++)								//发送字符串
	{
    usart_data_transmit(UART4, p_u8arrary[i]);
    while(RESET == usart_flag_get(UART4, USART_TDC_FLAG));
	}
}
/*----------------------------------------------------------------------------
函数名称： uart1_data_send
具体描述： uart1发送字符
输入参数： value 字符数据
----------------------------------------------------------------------------*/
void uart1_data_send(uint16_t value)
{    
  usart_data_transmit(USART1, value);
  while(usart_flag_get(USART1, USART_TDC_FLAG) == RESET);	
}
/*----------------------------------------------------------------------------
函数名称： uart2_data_send
具体描述： uart2发送字符
输入参数： value 字符数据
----------------------------------------------------------------------------*/
void uart2_data_send(uint16_t value)
{    
  usart_data_transmit(USART2, value);
  while(usart_flag_get(USART2, USART_TDC_FLAG) == RESET);	
}
/*----------------------------------------------------------------------------
函数名称： uart3_data_send
具体描述： uart3发送字符
输入参数： value 字符数据
----------------------------------------------------------------------------*/
void uart3_data_send(uint16_t value)
{    
  usart_data_transmit(USART3, value);
  while(usart_flag_get(USART3, USART_TDC_FLAG) == RESET);	
}
/*----------------------------------------------------------------------------
函数名称： uart4_data_send
具体描述： uart4发送字符
输入参数： value 字符数据
----------------------------------------------------------------------------*/
void uart4_data_send(uint16_t value)
{    
  usart_data_transmit(UART4, value);
  while(usart_flag_get(UART4, USART_TDC_FLAG) == RESET);	
}
/*----------------------------------------------------------------------------
函数名称：fputc
具体描述：重定向fputc
----------------------------------------------------------------------------*/
#pragma import(__use_no_semihosting)
  struct __FILE
  {
    int handle;
  };
  FILE __stdout;
  void _sys_exit(int x)
  {
    x = x;
  }
  /* __use_no_semihosting was requested, but _ttywrch was */
  void _ttywrch(int ch)
  {
    ch = ch;
  }
int fputc(int ch, FILE *f)
{
  usart_data_transmit(USART3, (uint16_t)ch);
  while(RESET == usart_flag_get(USART3, USART_TDC_FLAG));
  return ch;
}
