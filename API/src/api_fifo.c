/************************************************************************
*  文件名称: api_fifo.c
*  文件描述: 环形缓冲队列简单实现
*  历史记录:  
*  创建时间: 2023-03-13
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

/*************************************************************************
静态函数声明

extern 外部函数声明
*************************************************************************/

/*************************************************************************
函数定义
*************************************************************************/
/********************************************************************** 
*  函数名称: lm_fifo_init
*  功能描述: 定义并初始化一个环形队列
*  参数说明:
*  输    入: ptFifoCtrl:环形队列的控制参数结构体；
*			 u32NodeMaxNum:环形队列节点个数；
*			 u32NodeLen:每个节点长度；
*			 pNodeArray:环形队列地址；
*  输    出: 无
*  返 回 值: FAIL: 失败;  SUCC: 成功  
***********************************************************************/
int8_t lm_fifo_init(FIFO_CTRL_T *ptFifoCtrl,uint32_t u32NodeMaxNum,uint32_t u32NodeLen,void *pNodeArray)
{
		
	memset(pNodeArray,0,u32NodeMaxNum*u32NodeLen);					//复位FIFO BUF

	ptFifoCtrl->u32NodeMaxNum = u32NodeMaxNum;
	ptFifoCtrl->u32NodeLen = u32NodeLen;
	ptFifoCtrl->u32ReadAdd = 0;
	ptFifoCtrl->u32WriteAdd = 0;

	ptFifoCtrl->ptNodeBufAddr = (uint8_t *)pNodeArray;

	return 1;
}

/********************************************************************** 
*  函数名称: lm_fifo_is_full
*  功能描述: 检查是否为满
*  参数说明: 
*  输    入: ptFifoCtrl:环形队列的控制参数结构体；
*  输    出: 
*  返 回 值: 
***********************************************************************/
int8_t lm_fifo_is_full(FIFO_CTRL_T *ptFifoCtrl)
{
	if(((ptFifoCtrl->u32WriteAdd + 1)%ptFifoCtrl->u32NodeMaxNum) == ptFifoCtrl->u32ReadAdd)
	{
		return 1;
	}
	else
	{
		return 0;
	}	
}

/********************************************************************** 
*  函数名称: lm_fifo_add_node_impolite
*  功能描述: 非礼貌方式添加一个节点，即若满则删除一个头节点，并加到尾节点；
*  参数说明: 
*  输    入: 
*  输    出: 
*  返 回 值: SUCC:添加成功
***********************************************************************/
int8_t lm_fifo_add_node_impolite(FIFO_CTRL_T *ptFifoCtrl,void *pNodeData)
{
	void *pCurNodeDataAdd = NULL;

	if(lm_fifo_is_full(ptFifoCtrl))
	{
		lm_fifo_delete_node_for_ISR(ptFifoCtrl);
	}

	pCurNodeDataAdd = (void *)(ptFifoCtrl->ptNodeBufAddr + ptFifoCtrl->u32NodeLen*ptFifoCtrl->u32WriteAdd);

	memcpy(pCurNodeDataAdd,pNodeData,ptFifoCtrl->u32NodeLen);

	ptFifoCtrl->u32WriteAdd = (ptFifoCtrl->u32WriteAdd+1) % ptFifoCtrl->u32NodeMaxNum;
	
	return 1;
}
/********************************************************************** 
*  函数名称: lm_fifo_get_node
*  功能描述: 获取一个头节点地址；
*  参数说明: 
*  输    入: 
*  输    出: 
*  返 回 值: 为空则返回NULL,否则返回节点地址；
***********************************************************************/
void * lm_fifo_get_node(FIFO_CTRL_T *ptFifoCtrl)
{
	void * pCurNodeDataAdd = NULL;

	if(ptFifoCtrl->u32ReadAdd == ptFifoCtrl->u32WriteAdd)
		return NULL;
	
	pCurNodeDataAdd = (void *)(ptFifoCtrl->ptNodeBufAddr + ptFifoCtrl->u32NodeLen*ptFifoCtrl->u32ReadAdd);	

	return pCurNodeDataAdd;
}
/********************************************************************** 
*  函数名称: lm_fifo_delete_node
*  功能描述: 删除一个头节点；
*  参数说明: 
*  输    入: 
*  输    出: 
*  返 回 值: 
***********************************************************************/
int8_t lm_fifo_delete_node(FIFO_CTRL_T *ptFifoCtrl)
{
	if(ptFifoCtrl->u32WriteAdd == ptFifoCtrl->u32ReadAdd)
		return 0;

	ptFifoCtrl->u32ReadAdd = (ptFifoCtrl->u32ReadAdd+1) % ptFifoCtrl->u32NodeMaxNum; 	
	return 1;
}
/********************************************************************** 
*  函数名称: lm_fifo_delete_node_for_ISR
*  功能描述: 删除一个头节点(用于中断调用)；
*  参数说明: 
*  输    入: 
*  输    出: 
*  返 回 值: 
***********************************************************************/
int8_t lm_fifo_delete_node_for_ISR(FIFO_CTRL_T *ptFifoCtrl)
{
	if(ptFifoCtrl->u32WriteAdd == ptFifoCtrl->u32ReadAdd)
		return 0;

	ptFifoCtrl->u32ReadAdd = (ptFifoCtrl->u32ReadAdd+1) % ptFifoCtrl->u32NodeMaxNum; 	
	return 1;
}


