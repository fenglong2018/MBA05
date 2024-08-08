/*************************************************************************
*  	�ļ�����:  	Hal_Uart.c
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
#include    "at32f435_437_usart.h"
#include    "at32f435_437_misc.h"

#include    "Hal_Uart.h"
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
�������ƣ� Hal_Uart1_Init
���������� ����1��ʼ��
----------------------------------------------------------------------------*/
void Hal_Uart1_Init(void)
{
  gpio_init_type gpio_init_struct;
  
  /* ����USART1ʱ�� */
  crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK,TRUE);
  
  /* ����GPIO���� */
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = IO_UART1_TX ;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(PT_UART1_TX, &gpio_init_struct);
  
  gpio_init_struct.gpio_pins = IO_UART1_RX ;
  gpio_init(PT_UART1_RX, &gpio_init_struct);
  
  /* ��������IOMUX���� */
  gpio_pin_mux_config(PT_UART1_TX, NUM_UART1_TX, IOMUX_UART1);
  gpio_pin_mux_config(PT_UART1_RX, NUM_UART1_RX, IOMUX_UART1);
  
  /* �ж����ȼ����� */
  nvic_irq_enable(USART1_IRQn, 0, 0);
  
  /* ���ڳ�ʼ�� */
  usart_init(USART1, 115200, USART_DATA_8BITS, USART_STOP_1_BIT);
  /* ���ڷ���ʹ�� */
  usart_transmitter_enable(USART1, TRUE);
  /* ���ڽ���ʹ�� */
  usart_receiver_enable(USART1, TRUE);
  /* �����ж�ʹ�� */
  usart_interrupt_enable(USART1, USART_RDBF_INT, TRUE);
  /* ����ʹ�� */
  usart_enable(USART1, TRUE);
}
/*----------------------------------------------------------------------------
�������ƣ� Hal_Uart1_Deinit
���������� ����1ʧ��
----------------------------------------------------------------------------*/
void Hal_Uart1_Deinit(void)
{
    usart_enable(USART1, FALSE);
    usart_interrupt_enable(USART1, USART_RDBF_INT, FALSE);
}
/*----------------------------------------------------------------------------
�������ƣ� Hal_Uart2_Init
���������� ����2��ʼ��
----------------------------------------------------------------------------*/
void Hal_Uart2_Init(void)
{
  gpio_init_type gpio_init_struct;
  
  /* ����USART2ʱ�� */
  crm_periph_clock_enable(CRM_USART2_PERIPH_CLOCK,TRUE);
  
  /* ����GPIO���� */
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = IO_UART2_TX ;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(PT_UART2_TX, &gpio_init_struct);
  
  gpio_init_struct.gpio_pins = IO_UART2_RX ;
  gpio_init(PT_UART2_RX, &gpio_init_struct);
  
  /* ��������IOMUX���� */
  gpio_pin_mux_config(PT_UART2_TX, NUM_UART2_TX, IOMUX_UART2);
  gpio_pin_mux_config(PT_UART2_RX, NUM_UART2_RX, IOMUX_UART2);
  
  /* �ж����ȼ����� */
  nvic_irq_enable(USART2_IRQn, 0, 0);
  
  /* ���ڳ�ʼ�� */
  usart_init(USART2, 115200, USART_DATA_8BITS, USART_STOP_1_BIT);
  /* ���ڷ���ʹ�� */
  usart_transmitter_enable(USART2, TRUE);
  /* ���ڽ���ʹ�� */
  usart_receiver_enable(USART2, TRUE);
  /* �����ж�ʹ�� */
  usart_interrupt_enable(USART2, USART_RDBF_INT, TRUE);
  /* ����ʹ�� */
  usart_enable(USART2, TRUE);
}
/*----------------------------------------------------------------------------
�������ƣ� Hal_Uart2_Deinit
���������� ����2ʧ��
----------------------------------------------------------------------------*/
void Hal_Uart2_Deinit(void)
{
    usart_enable(USART2, FALSE);
    usart_interrupt_enable(USART2, USART_RDBF_INT, FALSE);
}
/*----------------------------------------------------------------------------
�������ƣ� Hal_Uart3_Init
���������� ����3��ʼ��
----------------------------------------------------------------------------*/
void Hal_Uart3_Init(void)
{
  gpio_init_type gpio_init_struct;
  
  /* ����USART3ʱ�� */
  crm_periph_clock_enable(CRM_USART3_PERIPH_CLOCK,TRUE);
  
  /* ����GPIO���� */
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = IO_UART3_TX ;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(PT_UART3_TX, &gpio_init_struct);
  
  gpio_init_struct.gpio_pins = IO_UART3_RX ;
  gpio_init(PT_UART3_RX, &gpio_init_struct);
  
  /* ��������IOMUX���� */
  gpio_pin_mux_config(PT_UART3_TX, NUM_UART3_TX, IOMUX_UART3);
  gpio_pin_mux_config(PT_UART3_RX, NUM_UART3_RX, IOMUX_UART3);
  
  /* �ж����ȼ����� */
  nvic_irq_enable(USART3_IRQn, 0, 0);
  
  /* ���ڳ�ʼ�� */
  usart_init(USART3, 115200, USART_DATA_8BITS, USART_STOP_1_BIT);
  /* ���ڷ���ʹ�� */
  usart_transmitter_enable(USART3, TRUE);
  /* ���ڽ���ʹ�� */
  usart_receiver_enable(USART3, TRUE);
  usart_transmit_receive_pin_swap(USART3, TRUE);
  /* �����ж�ʹ�� */
  usart_interrupt_enable(USART3, USART_RDBF_INT, TRUE);
  /* ����ʹ�� */
  usart_enable(USART3, TRUE);
}
/*----------------------------------------------------------------------------
�������ƣ� Hal_Uart3_Deinit
���������� ����3ʧ��
----------------------------------------------------------------------------*/
void Hal_Uart3_Deinit(void)
{
    usart_enable(USART3, FALSE);
    usart_interrupt_enable(USART3, USART_RDBF_INT, FALSE);
}
/*----------------------------------------------------------------------------
�������ƣ� Hal_Uart4_Init
���������� ����4��ʼ��
----------------------------------------------------------------------------*/
void Hal_Uart4_Init(void)
{
  gpio_init_type gpio_init_struct;
  
  /* ����USART1ʱ�� */
  crm_periph_clock_enable(CRM_UART4_PERIPH_CLOCK,TRUE);
  
  /* ����GPIO���� */
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = IO_UART4_TX ;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(PT_UART4_TX, &gpio_init_struct);
  
  gpio_init_struct.gpio_pins = IO_UART4_RX ;
  gpio_init(PT_UART4_RX, &gpio_init_struct);
  
  /* ��������IOMUX���� */
  gpio_pin_mux_config(PT_UART4_TX, NUM_UART4_TX, IOMUX_UART4);
  gpio_pin_mux_config(PT_UART4_RX, NUM_UART4_RX, IOMUX_UART4);
  
  /* �ж����ȼ����� */
  nvic_irq_enable(UART4_IRQn, 0, 0);
  
  /* ���ڳ�ʼ�� */
  usart_init(UART4, 115200, USART_DATA_8BITS, USART_STOP_1_BIT);
  /* ���ڷ���ʹ�� */
  usart_transmitter_enable(UART4, TRUE);
  /* ���ڽ���ʹ�� */
  usart_receiver_enable(UART4, TRUE);
  /* �����ж�ʹ�� */
  usart_interrupt_enable(UART4, USART_RDBF_INT, TRUE);
  /* ����ʹ�� */
  usart_enable(UART4, TRUE);
}
/*----------------------------------------------------------------------------
�������ƣ� Hal_Uart4_Deinit
���������� ����4ʧ��
----------------------------------------------------------------------------*/
void Hal_Uart4_Deinit(void)
{
    usart_enable(UART4, FALSE);
    usart_interrupt_enable(UART4, USART_RDBF_INT, FALSE);
}
/*----------------------------------------------------------------------------
�������ƣ� uart1_tx_string
���������� uart1�����ַ���
��������� * p_u8arrary �ַ�������   length �ַ�������
----------------------------------------------------------------------------*/
void uart1_tx_string(uint8_t * p_u8arrary,uint16_t length)			
{
	uint16_t i;
  for(i=0;i<length;i++)								//�����ַ���
	{
    usart_data_transmit(USART1, p_u8arrary[i]);
    while(RESET == usart_flag_get(USART1, USART_TDC_FLAG));
	}
}

/*----------------------------------------------------------------------------
�������ƣ� uart2_tx_string
���������� uart2�����ַ���
��������� * p_u8arrary �ַ�������   length �ַ�������
----------------------------------------------------------------------------*/
void uart2_tx_string(uint8_t * p_u8arrary,uint16_t length)			
{
	uint16_t i;
  for(i=0;i<length;i++)								//�����ַ���
	{
    usart_data_transmit(USART2, p_u8arrary[i]);
    while(RESET == usart_flag_get(USART2, USART_TDC_FLAG));
	}
}
/*----------------------------------------------------------------------------
�������ƣ� uart3_tx_string
���������� uart3�����ַ���
��������� * p_u8arrary �ַ�������   length �ַ�������
----------------------------------------------------------------------------*/
void uart3_tx_string(uint8_t * p_u8arrary,uint16_t length)			
{
	uint16_t i;
  for(i=0;i<length;i++)								//�����ַ���
	{
    usart_data_transmit(USART3, p_u8arrary[i]);
    while(RESET == usart_flag_get(USART3, USART_TDC_FLAG));
	}
}
/*----------------------------------------------------------------------------
�������ƣ� uart4_tx_string
���������� uart4�����ַ���
��������� * p_u8arrary �ַ�������   length �ַ�������
----------------------------------------------------------------------------*/
void uart4_tx_string(uint8_t * p_u8arrary,uint16_t length)			
{
	uint16_t i;
  for(i=0;i<length;i++)								//�����ַ���
	{
    usart_data_transmit(UART4, p_u8arrary[i]);
    while(RESET == usart_flag_get(UART4, USART_TDC_FLAG));
	}
}
/*----------------------------------------------------------------------------
�������ƣ� uart1_data_send
���������� uart1�����ַ�
��������� value �ַ�����
----------------------------------------------------------------------------*/
void uart1_data_send(uint16_t value)
{    
  usart_data_transmit(USART1, value);
  while(usart_flag_get(USART1, USART_TDC_FLAG) == RESET);	
}
/*----------------------------------------------------------------------------
�������ƣ� uart2_data_send
���������� uart2�����ַ�
��������� value �ַ�����
----------------------------------------------------------------------------*/
void uart2_data_send(uint16_t value)
{    
  usart_data_transmit(USART2, value);
  while(usart_flag_get(USART2, USART_TDC_FLAG) == RESET);	
}
/*----------------------------------------------------------------------------
�������ƣ� uart3_data_send
���������� uart3�����ַ�
��������� value �ַ�����
----------------------------------------------------------------------------*/
void uart3_data_send(uint16_t value)
{    
  usart_data_transmit(USART3, value);
  while(usart_flag_get(USART3, USART_TDC_FLAG) == RESET);	
}
/*----------------------------------------------------------------------------
�������ƣ� uart4_data_send
���������� uart4�����ַ�
��������� value �ַ�����
----------------------------------------------------------------------------*/
void uart4_data_send(uint16_t value)
{    
  usart_data_transmit(UART4, value);
  while(usart_flag_get(UART4, USART_TDC_FLAG) == RESET);	
}
/*----------------------------------------------------------------------------
�������ƣ�fputc
�����������ض���fputc
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
