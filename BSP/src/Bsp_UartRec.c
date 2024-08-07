/************************************************************************
*  �ļ�����: Bsp_UartRec.c
*  �ļ�����: �������ݽ��ܺ�FIFO�洢
*  ��ʷ��¼:  
*  ����ʱ��: 2024-05-10
*  ������:   ������
*  �޸�ʱ��:
*  �޸���:  
*  ����:
*************************************************************************/

/************************************************************************
ͷ�ļ���������
*************************************************************************/
#include    <string.h>

#include    "api_fifo.h"
#include    "at32f435_437_usart.h"

#include    "Hal_Uart.h"
#include    "Bsp_UartRec.h"
#include    "Bsp_GnssParse.h"
#include    "Bsp_RdssParse.h"
#include    "Bsp_Log.h"
#include    "Bsp_Config.h"
/************************************************************************
�궨��
*************************************************************************/

/************************************************************************
�ڲ��ṹ��ö�١�������Ƚṹ����
*************************************************************************/

/************************************************************************
�ڲ���̬��������

ȫ�ֱ�������

extern �ⲿ��������

*************************************************************************/
/* FIFO�ṹ�� */
FIFO_CTRL_T uart1_rx_fifo_ctrl;
FIFO_CTRL_T uart2_rx_fifo_ctrl;
FIFO_CTRL_T uart3_rx_fifo_ctrl;	
FIFO_CTRL_T uart4_rx_fifo_ctrl;	

/* FIFO�������� */
uint8_t uart1_rx_buf[RXFIFO_LEN];
uint8_t uart2_rx_buf[RXFIFO_LEN];
uint8_t uart3_rx_buf[RXFIFO_LEN];
uint8_t uart4_rx_buf[RXFIFO_LEN];
/************************************************************************
��̬��������

extern �ⲿ��������
*************************************************************************/

/************************************************************************
��������
*************************************************************************/
/*----------------------------------------------------------------------------
�������ƣ� Bsp_Receive_Uart1_Fifo_Data
���������� ��ȡ�ʹ�����1 FIFO����
----------------------------------------------------------------------------*/
void Bsp_Receive_Uart1_Fifo_Data(void)
{
  uint8_t *pUartNode = NULL;								    //�����ָ��
	uint8_t temp;
	pUartNode = lm_fifo_get_node(&uart1_rx_fifo_ctrl);		    //��FIFOȡ����
	if(pUartNode == NULL)									    //����ǿգ�����
	{
		return;
	}
	else													    //����ÿ�ֽڴ���
	{
		temp = *pUartNode;	
		lm_fifo_delete_node(&uart1_rx_fifo_ctrl);			    //ɾ��ȡ���ֽ�
    //uart2_data_send(temp);
    Bsp_Gnss_Parse_Byte(temp);
	}
}
/*----------------------------------------------------------------------------
�������ƣ� Bsp_Receive_Uart2_Fifo_Data
���������� ��ȡ�ʹ�����2 FIFO����
----------------------------------------------------------------------------*/
void Bsp_Receive_Uart2_Fifo_Data(void)
{
  uint8_t *pUartNode = NULL;								    //�����ָ��
	uint8_t temp;
	pUartNode = lm_fifo_get_node(&uart2_rx_fifo_ctrl);		    //��FIFOȡ����
	if(pUartNode == NULL)									    //����ǿգ�����
	{
		return;
	}
	else													    //����ÿ�ֽڴ���
	{
		temp = *pUartNode;	
		lm_fifo_delete_node(&uart2_rx_fifo_ctrl);			    //ɾ��ȡ���ֽ�
		//Bsp_Port_Parse_Byte(temp);
	}
}
/*----------------------------------------------------------------------------
�������ƣ� Bsp_Receive_Uart3_Fifo_Data
���������� ��ȡ�ʹ�����3 FIFO����
----------------------------------------------------------------------------*/
void Bsp_Receive_Uart3_Fifo_Data(void)
{
  uint8_t *pUartNode = NULL;								    //�����ָ��
	uint8_t temp;
	pUartNode = lm_fifo_get_node(&uart3_rx_fifo_ctrl);		    //��FIFOȡ����
	if(pUartNode == NULL)									    //����ǿգ�����
	{
		return;
	}
	else													    //����ÿ�ֽڴ���
	{
		temp = *pUartNode;	
		lm_fifo_delete_node(&uart3_rx_fifo_ctrl);			    //ɾ��ȡ���ֽ�
		Bsp_Port_Parse_Byte(temp);
	}
}
/*----------------------------------------------------------------------------
�������ƣ� Bsp_Receive_Uart4_Fifo_Data
���������� ��ȡ�ʹ�����4 FIFO����
----------------------------------------------------------------------------*/
void Bsp_Receive_Uart4_Fifo_Data(void)
{
  uint8_t *pUartNode = NULL;								    //�����ָ��
	uint8_t temp;
	pUartNode = lm_fifo_get_node(&uart4_rx_fifo_ctrl);		    //��FIFOȡ����
	if(pUartNode == NULL)									    //����ǿգ�����
	{
		return;
	}
	else													            //����ÿ�ֽڴ���
	{
		temp = *pUartNode;	
		lm_fifo_delete_node(&uart4_rx_fifo_ctrl);			    //ɾ��ȡ���ֽ�
		Bsp_Rdss_Parse_Byte(temp);
	}
}
/*----------------------------------------------------------------------------
�������ƣ� Bsp_Uart_Fifo_Init
���������� ����FIFO��ʼ��
----------------------------------------------------------------------------*/
void Bsp_Uart_Fifo_Init(void)
{
  lm_fifo_init(&uart1_rx_fifo_ctrl,RXFIFO_LEN,1,uart1_rx_buf);			    //FIFO��ʼ��
  lm_fifo_init(&uart2_rx_fifo_ctrl,RXFIFO_LEN,1,uart2_rx_buf);			    //FIFO��ʼ��
  lm_fifo_init(&uart3_rx_fifo_ctrl,RXFIFO_LEN,1,uart3_rx_buf);			    //FIFO��ʼ��
  lm_fifo_init(&uart4_rx_fifo_ctrl,RXFIFO_LEN,1,uart4_rx_buf);			    //FIFO��ʼ��
}
/*-------------------------------����1�ж�-----------------------------*/
void USART1_IRQHandler(void)
{
  uint8_t u8RxData;
  if (usart_interrupt_flag_get(USART1, USART_ROERR_FLAG) == SET)
  {   
    usart_flag_clear(USART1,USART_ROERR_FLAG);
  }
  if(usart_interrupt_flag_get(USART1, USART_RDBF_FLAG) != RESET)
  {
    usart_flag_clear(USART1,USART_RDBF_FLAG);
    u8RxData = usart_data_receive(USART1);
    lm_fifo_add_node_impolite(&uart1_rx_fifo_ctrl,&u8RxData);
  }
}
/*-------------------------------����2�ж�-----------------------------*/
void USART2_IRQHandler(void)
{
  uint8_t u8RxData;
  if (usart_interrupt_flag_get(USART2, USART_ROERR_FLAG) == SET)
  {   
    usart_flag_clear(USART2,USART_ROERR_FLAG);
  }
  if(usart_interrupt_flag_get(USART2, USART_RDBF_FLAG) != RESET)
  {
    usart_flag_clear(USART2,USART_RDBF_FLAG);
    u8RxData = usart_data_receive(USART2);
    lm_fifo_add_node_impolite(&uart2_rx_fifo_ctrl,&u8RxData);
  }
}
/*-------------------------------����3�ж�-----------------------------*/
void USART3_IRQHandler(void)
{
  uint8_t u8RxData;
  if (usart_interrupt_flag_get(USART3, USART_ROERR_FLAG) == SET)
  {   
    usart_flag_clear(USART3,USART_ROERR_FLAG);
  }
  if(usart_interrupt_flag_get(USART3, USART_RDBF_FLAG) != RESET)
  {
    usart_flag_clear(USART3,USART_RDBF_FLAG);
    u8RxData = usart_data_receive(USART3);
    lm_fifo_add_node_impolite(&uart3_rx_fifo_ctrl,&u8RxData);
  }
}
/*-------------------------------����4�ж�-----------------------------*/
void UART4_IRQHandler(void)
{
  uint8_t u8RxData;
  if (usart_interrupt_flag_get(UART4, USART_ROERR_FLAG) == SET)
  {   
    usart_flag_clear(UART4,USART_ROERR_FLAG);
  }
  if(usart_interrupt_flag_get(UART4, USART_RDBF_FLAG) != RESET)
  {
    usart_flag_clear(UART4,USART_RDBF_FLAG);
    u8RxData = usart_data_receive(UART4);
    lm_fifo_add_node_impolite(&uart4_rx_fifo_ctrl,&u8RxData);
  }
}
