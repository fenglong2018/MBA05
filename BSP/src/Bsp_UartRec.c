/************************************************************************
*  文件名称: Bsp_UartRec.c
*  文件描述: 串口数据接受和FIFO存储
*  历史记录:  
*  创建时间: 2024-05-10
*  创建人:   江昌钤
*  修改时间:
*  修改人:  
*  描述:
*************************************************************************/

/************************************************************************
头文件引用声明
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
宏定义
*************************************************************************/

/************************************************************************
内部结构、枚举、公用体等结构声明
*************************************************************************/

/************************************************************************
内部静态变量定义

全局变量定义

extern 外部变量声明

*************************************************************************/
/* FIFO结构体 */
FIFO_CTRL_T uart1_rx_fifo_ctrl;
FIFO_CTRL_T uart2_rx_fifo_ctrl;
FIFO_CTRL_T uart3_rx_fifo_ctrl;	
FIFO_CTRL_T uart4_rx_fifo_ctrl;	

/* FIFO缓存数组 */
uint8_t uart1_rx_buf[RXFIFO_LEN];
uint8_t uart2_rx_buf[RXFIFO_LEN];
uint8_t uart3_rx_buf[RXFIFO_LEN];
uint8_t uart4_rx_buf[RXFIFO_LEN];
/************************************************************************
静态函数声明

extern 外部函数声明
*************************************************************************/

/************************************************************************
函数定义
*************************************************************************/
/*----------------------------------------------------------------------------
函数名称： Bsp_Receive_Uart1_Fifo_Data
具体描述： 获取和处理串口1 FIFO数据
----------------------------------------------------------------------------*/
void Bsp_Receive_Uart1_Fifo_Data(void)
{
  uint8_t *pUartNode = NULL;								    //定义空指针
	uint8_t temp;
	pUartNode = lm_fifo_get_node(&uart1_rx_fifo_ctrl);		    //从FIFO取数据
	if(pUartNode == NULL)									    //如果是空，跳出
	{
		return;
	}
	else													    //否则每字节处理
	{
		temp = *pUartNode;	
		lm_fifo_delete_node(&uart1_rx_fifo_ctrl);			    //删除取的字节
    //uart2_data_send(temp);
    Bsp_Gnss_Parse_Byte(temp);
	}
}
/*----------------------------------------------------------------------------
函数名称： Bsp_Receive_Uart2_Fifo_Data
具体描述： 获取和处理串口2 FIFO数据
----------------------------------------------------------------------------*/
void Bsp_Receive_Uart2_Fifo_Data(void)
{
  uint8_t *pUartNode = NULL;								    //定义空指针
	uint8_t temp;
	pUartNode = lm_fifo_get_node(&uart2_rx_fifo_ctrl);		    //从FIFO取数据
	if(pUartNode == NULL)									    //如果是空，跳出
	{
		return;
	}
	else													    //否则每字节处理
	{
		temp = *pUartNode;	
		lm_fifo_delete_node(&uart2_rx_fifo_ctrl);			    //删除取的字节
		//Bsp_Port_Parse_Byte(temp);
	}
}
/*----------------------------------------------------------------------------
函数名称： Bsp_Receive_Uart3_Fifo_Data
具体描述： 获取和处理串口3 FIFO数据
----------------------------------------------------------------------------*/
void Bsp_Receive_Uart3_Fifo_Data(void)
{
  uint8_t *pUartNode = NULL;								    //定义空指针
	uint8_t temp;
	pUartNode = lm_fifo_get_node(&uart3_rx_fifo_ctrl);		    //从FIFO取数据
	if(pUartNode == NULL)									    //如果是空，跳出
	{
		return;
	}
	else													    //否则每字节处理
	{
		temp = *pUartNode;	
		lm_fifo_delete_node(&uart3_rx_fifo_ctrl);			    //删除取的字节
		Bsp_Port_Parse_Byte(temp);
	}
}
/*----------------------------------------------------------------------------
函数名称： Bsp_Receive_Uart4_Fifo_Data
具体描述： 获取和处理串口4 FIFO数据
----------------------------------------------------------------------------*/
void Bsp_Receive_Uart4_Fifo_Data(void)
{
  uint8_t *pUartNode = NULL;								    //定义空指针
	uint8_t temp;
	pUartNode = lm_fifo_get_node(&uart4_rx_fifo_ctrl);		    //从FIFO取数据
	if(pUartNode == NULL)									    //如果是空，跳出
	{
		return;
	}
	else													            //否则每字节处理
	{
		temp = *pUartNode;	
		lm_fifo_delete_node(&uart4_rx_fifo_ctrl);			    //删除取的字节
		Bsp_Rdss_Parse_Byte(temp);
	}
}
/*----------------------------------------------------------------------------
函数名称： Bsp_Uart_Fifo_Init
具体描述： 串口FIFO初始化
----------------------------------------------------------------------------*/
void Bsp_Uart_Fifo_Init(void)
{
  lm_fifo_init(&uart1_rx_fifo_ctrl,RXFIFO_LEN,1,uart1_rx_buf);			    //FIFO初始化
  lm_fifo_init(&uart2_rx_fifo_ctrl,RXFIFO_LEN,1,uart2_rx_buf);			    //FIFO初始化
  lm_fifo_init(&uart3_rx_fifo_ctrl,RXFIFO_LEN,1,uart3_rx_buf);			    //FIFO初始化
  lm_fifo_init(&uart4_rx_fifo_ctrl,RXFIFO_LEN,1,uart4_rx_buf);			    //FIFO初始化
}
/*-------------------------------串口1中断-----------------------------*/
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
/*-------------------------------串口2中断-----------------------------*/
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
/*-------------------------------串口3中断-----------------------------*/
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
/*-------------------------------串口4中断-----------------------------*/
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
