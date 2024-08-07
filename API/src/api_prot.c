/************************************************************************
*  文件名称: lm_prot.c
*  文件描述: 通讯协议解析和封装
*  历史记录:  
*  创建时间: 
*  创建人:   林开洲
*  修改时间:20220225
*  修改人:  
*  描述:
*************************************************************************/


/************************************************************************
头文件引用声明
*************************************************************************/
#include    <stdint.h>
#include    <string.h>
#include    "api_prot.h"
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
/*----------------------------------------------------------------------------
函数名称：lm_prot_get_rx_pkt_len
具体描述：获取接收包长度
输入参数：pu8RxPkt  接收数组
----------------------------------------------------------------------------*/
uint16_t lm_prot_get_rx_pkt_len(uint8_t *pu8RxPkt)
{
	//返回接收包的长度信息，第2和3字节的16位数据，小端序
    return BUILD_UINT16(pu8RxPkt[PROT_IDX_PKT_LEN+1], pu8RxPkt[PROT_IDX_PKT_LEN]);
}
/*----------------------------------------------------------------------------
函数名称：lm_prot_get_rx_pkt_ver
具体描述：获取接收包版本号
输入参数：pu8RxPkt  接收数组
----------------------------------------------------------------------------*/
uint16_t lm_prot_get_rx_pkt_ver(uint8_t *pu8RxPkt)
{
	//返回接收包的版本号信息，第4和5字节的16位数据，小端序
    return BUILD_UINT16(pu8RxPkt[PROT_IDX_VER+1],pu8RxPkt[PROT_IDX_VER]);
}
/*----------------------------------------------------------------------------
函数名称：lm_prot_get_rx_pkt_option
具体描述：获取接收包功能控制位
输入参数：pu8RxPkt  接收数组
----------------------------------------------------------------------------*/
uint8_t lm_prot_get_rx_pkt_option(uint8_t *pu8RxPkt)
{
	//返回接收包的功能控制位，第6字节
    return pu8RxPkt[PROT_IDX_OPTION];
}
/*----------------------------------------------------------------------------
函数名称：lm_prot_get_rx_pkt_checksum
具体描述：获取接收包检验和
输入参数：pu8RxPkt  接收数组
----------------------------------------------------------------------------*/
uint16_t lm_prot_get_rx_pkt_checksum(uint8_t *pu8RxPkt)
{
	//获取接收包长度
    uint16_t len = lm_prot_get_rx_pkt_len(pu8RxPkt);
    //返回接收包的检验和，最后两位，小端序
    return BUILD_UINT16(pu8RxPkt[len - 1],pu8RxPkt[len - 2]);
}
/*----------------------------------------------------------------------------
函数名称：lm_prot_get_rx_pkt_pld_len
具体描述：获取接收包包体长度
输入参数：pu8RxPkt  接收数组
----------------------------------------------------------------------------*/
uint16_t lm_prot_get_rx_pkt_pld_len(uint8_t *pu8RxPkt)
{
	//获取接收包长度
    uint16_t len = lm_prot_get_rx_pkt_len(pu8RxPkt);
	//减去头，长度，版本，控制位, 校验位
    len -= 9;   //7改成9	

    //返回接收包包体长度
    return len;
}
/*----------------------------------------------------------------------------
函数名称：*lm_prot_get_rx_pkt_pld
具体描述：获取接收包包体命令字节
输入参数：pu8RxPkt  接收数组
----------------------------------------------------------------------------*/
uint8_t *lm_prot_get_rx_pkt_pld(uint8_t *pu8RxPkt)
{
	//返回包体命令字节
    return &pu8RxPkt[PROT_IDX_PLD];
}
/*----------------------------------------------------------------------------
函数名称：port_checksum
具体描述：计算累加检验和
输入参数：buf 数组 buflen 数据长度
输出参数：校验和
----------------------------------------------------------------------------*/
uint16_t port_checksum(uint8_t * buf, uint32_t buflen)
{
	uint16_t sum = 0;

	if (buf != NULL) 
	{
		while(buflen--)
		{
			sum += *buf++;
		}
	}
	
	return sum;
}
/*----------------------------------------------------------------------------
函数名称：lm_prot_rx_parse_byte
具体描述：接收解析每个字节
输入参数：ptParse 接收解析结果结构体  u8Val 当前接收解析字节数据
----------------------------------------------------------------------------*/
int32_t lm_prot_rx_parse_byte(PROT_RX_UNPACK_T *ptParse, uint8_t u8Val)
{
	
    uint16_t u16Len = 0;								//长度
    uint16_t u16Ver = 0;								//版本	
    uint16_t u16CheckSum = 0;							//校验和
    uint16_t u16Tmp = 0;								//接收的检验和
    uint16_t u16PldLen = 0;								//实际长度
    uint8_t *pu8Buf = NULL;								//解析缓存

    if(ptParse == NULL)									//如果解析结果结构体为空
        return FAIL;									//退出且返回失败

    if(ptParse->ptProtRxParseCallbackFunc == NULL)		//如果解析处理回调为空
        return FAIL;									//退出且返回失败

    pu8Buf = ptParse->pu8Pkt;							//解析缓存数组指针指向传入解析结构体的数组
    if(ptParse->u8Step == 0)							//如果解析第0字节
    {
        if(u8Val != PROT_HEAD_0)						//如果当前解析字节数据不等于协议头0
        {
			//回调：未解析到头的情况下收到非头字段数据，解析缓存，解析第0字节
            (*ptParse->ptProtRxParseCallbackFunc)(PARSE_RESULT_INVALID, pu8Buf, ptParse->u8Step);		
            ptParse->u8Step = 0;						//当前解析字节复位0
            return FAIL;								//退出且返回失败
        }
        else											//如果等于协议头0
        {
            pu8Buf[ptParse->u8Step++] = u8Val;			//解析第1字节，当前字节数据存入数组0
			//回调：正常一条命令解析中(已收到协议头)，但未有整条解析结果，解析缓存，解析第1字节
            (*ptParse->ptProtRxParseCallbackFunc)(PARSE_RESULT_PARSING, pu8Buf, ptParse->u8Step);
        }
    }
    else if(ptParse->u8Step == 1)						//如果解析第1字节
    {
        if(u8Val == PROT_HEAD_1)						//如果当前解析字节数据等于协议头1
        {
            pu8Buf[ptParse->u8Step++] = u8Val;			//解析第2字节，当前字节数据存入数组0
			//回调：正常一条命令解析中(已收到协议头)，但未有整条解析结果，解析缓存，解析第2字节
            (*ptParse->ptProtRxParseCallbackFunc)(PARSE_RESULT_PARSING, pu8Buf, ptParse->u8Step);
        }
        else											//如果当前解析字节数据不等于协议头1
        {
            if(u8Val == PROT_HEAD_0)    				//如果当前解析字节数据等于协议头0
            {
				//回调：未解析到头的情况下收到非头字段数据，解析缓存，解析第1字节
                (*ptParse->ptProtRxParseCallbackFunc)(PARSE_RESULT_INVALID, pu8Buf, ptParse->u8Step);				
                ptParse->u8Step = 0;					//当前解析字节复位0
                pu8Buf[ptParse->u8Step++] = u8Val;		//解析第1字节，当前字节数据存入数组0
				//回调：正常一条命令解析中(已收到协议头)，但未有整条解析结果，解析缓存，解析第1字节
                (*ptParse->ptProtRxParseCallbackFunc)(PARSE_RESULT_PARSING, pu8Buf, ptParse->u8Step);
            }
            else										//如果当前解析字节数据也不等于协议头0
            {
				//回调：未解析到头的情况下收到非头字段数据，解析缓存数组，解析第1字节
                (*ptParse->ptProtRxParseCallbackFunc)(PARSE_RESULT_INVALID, pu8Buf, ptParse->u8Step);
                ptParse->u8Step = 0;					//当前解析字节复位0
                return FAIL;							//退出且返回失败
            }
        }
    }
    else if(ptParse->u8Step < 6)						//如果解析完协议头，解析2~6字节都存入数组
    {
        pu8Buf[ptParse->u8Step++] = u8Val;				//解析下一字节，当前字节存入数组2~6字节
		//回调：正常一条命令解析中(已收到协议头)，但未有整条解析结果，解析缓存，解析当前第2~6字节
        (*ptParse->ptProtRxParseCallbackFunc)(PARSE_RESULT_PARSING, pu8Buf, ptParse->u8Step);
    }
    else if(ptParse->u8Step == 6)						//如果解析完协议头，解析第6字节
    {
        pu8Buf[ptParse->u8Step++] = u8Val;				//解析下一字节，当前字节存入数组第6字节，控制位
        /* 当前步骤解析完帧头2字节，长度2字节，版本2字节，功能控制1字节 */
		//长度
        u16Len = BUILD_UINT16(pu8Buf[PROT_IDX_PKT_LEN+1], pu8Buf[PROT_IDX_PKT_LEN]);
		//版本
        u16Ver = BUILD_UINT16(pu8Buf[PROT_IDX_VER+1],pu8Buf[PROT_IDX_VER]);
        u16Ver = u16Ver;
		if((u16Len <= PROT_HEAD_INFO_LEN + 2))
        {
			//回调：长度字段太小，解析缓存数组，解析第6字节
            (*ptParse->ptProtRxParseCallbackFunc)(PARSE_RESULT_TOO_SHORT, pu8Buf, ptParse->u8Step);
            ptParse->u8Step = 0;						//当前解析字节复位0
            return FAIL;								//退出且返回失败
        }
		//如果接收协议的长度信息大于最大Buf数据长度
        else if(u16Len > ptParse->u16BufLen)
        {
			//回调：长度字段超过最大值，解析缓存数组，解析第6字节
            (*ptParse->ptProtRxParseCallbackFunc)(PARSE_RESULT_TOO_LONG, pu8Buf, ptParse->u8Step);
            ptParse->u8Step = 0;						//当前解析字节复位0
            return FAIL;								//退出且返回失败
        }
    }
    else												//如果解析第6字节之后
    {
        pu8Buf[ptParse->u8Step++] = u8Val;				//除了校验和都为包体数据，存入数组
		//长度
        u16Len = BUILD_UINT16(pu8Buf[PROT_IDX_PKT_LEN+1], pu8Buf[PROT_IDX_PKT_LEN]);
		//如果当前解析的字节数小于协议长度
        if(ptParse->u8Step < u16Len)
        {
			//回调：正常一条命令解析中(已收到协议头)，但未有整条解析结果，解析缓存数组，当前字节
            (*ptParse->ptProtRxParseCallbackFunc)(PARSE_RESULT_PARSING, pu8Buf, ptParse->u8Step);
        }
		//如果当前解析的字节数等于协议长度，则为解析完一帧
        else 
        {
			//计算检验和包体长度：接收的长度信息减去两字节检验和长度
                u16PldLen = u16Len - PROT_HEAD_INFO_LEN- 2;	
				//校验和计算
                u16CheckSum = port_checksum(pu8Buf, u16Len - 2);
				//接收的检验和
                u16Tmp = BUILD_UINT16(pu8Buf[u16Len - 1], pu8Buf[u16Len - 2]);
                //如果计算的校验和和接收的检验和不等
                if(u16CheckSum != u16Tmp)
                {
					//回调：校验位错误，解析缓存数组，当前字节
                    (*ptParse->ptProtRxParseCallbackFunc)(PARSE_RESULT_CHECKSUM_ERROR, pu8Buf, ptParse->u8Step);
                    ptParse->u8Step = 0;				//当前解析字节复位0
                    return FAIL;						//退出且返回失败
                }
            u16PldLen = u16PldLen;
            //回调：正常解析出一条命令，解析缓存数组，当前字节
            (*ptParse->ptProtRxParseCallbackFunc)(PARSE_RESULT_SUCC, pu8Buf, ptParse->u8Step);
            ptParse->u8Step = 0;						//当前解析字节复位0
            return SUCC;								//退出且返回成功
        }
    }
	
    return SUCC;										//返回成功
}

/*----------------------------------------------------------------------------
函数名称：lm_prot_rx_parse
具体描述：按每字节接收解析一整包
输入参数：ptParse 接收解析结果结构体  pu8Data 数组 u16Len 
----------------------------------------------------------------------------*/
int32_t lm_prot_rx_parse(PROT_RX_UNPACK_T *ptRxParse, uint8_t *pu8Data, uint16_t u16Len)
{
    int32_t i = 0;
    
    if(ptRxParse == NULL || pu8Data == NULL)			//如果 解析结果结构体 为空 或者 数组 为空		
        return FAIL;									//返回失败

    for(i=0; i<u16Len; i++)								//按字节接收
        lm_prot_rx_parse_byte(ptRxParse, *(pu8Data+i));	//接收的每字节解析

    return SUCC;										//返回成功
}
/*----------------------------------------------------------------------------
函数名称：lm_prot_rx_parse_step_reset
具体描述：解析数组和长度初始化复位
输入参数：ptParse 解析结果结构体
----------------------------------------------------------------------------*/
int32_t lm_prot_rx_parse_step_reset(PROT_RX_UNPACK_T *ptRxParse)
{
    memset(ptRxParse->pu8Pkt, 0, ptRxParse->u16BufLen);	//清空解析缓存数组，清空解析长度
    ptRxParse->u8Step = 0;								//第0开始接收解析

    return SUCC;										//返回成功
}
/*----------------------------------------------------------------------------
函数名称：lm_prot_tx
具体描述：封装数据并发送
输入参数：ptTx 发送解析结构体，包头到控制  pu8Data  发送数据数组   u16Len  发送数据长度
----------------------------------------------------------------------------*/
int32_t lm_prot_tx(PROT_TX_PACK_T    *ptTx, uint8_t *pu8Data, uint16_t u16Len)
{
    uint16_t len = 0;									//总长度
    uint16_t sum = 0;									//校验和
    uint8_t *pu8Buf = NULL;								//发送封装好的缓存数组
    //如果发送解析结构体为空 或者 发送数组为空 或者 长度为0
    if(ptTx == NULL || pu8Data == NULL || u16Len == 0)	
        return FAIL;									//返回失败
	
    len = PROT_HEAD_INFO_LEN + u16Len + 2 ;  //增加+2 			 //长度等于 发送的数据包长度 + 协议结构，不包含校验和
	
    if(len > ptTx->u16TxBufLen)							//如果实际长度大于发送数据的长度信息
    {
        return FAIL;									//返回失败
    }
    
    pu8Buf = ptTx->pu8TxBuf;							//数组指向传入发送结构体的数组
    *pu8Buf++ = PROT_HEAD_0;							//封装协议头0
    *pu8Buf++ = PROT_HEAD_1;							//封装协议头1
    *pu8Buf++ = LO_UINT16(len);							//封装长度低8位
    *pu8Buf++ = HI_UINT16(len);							//封装长度高8位
    *pu8Buf++ = LO_UINT16(ptTx->u16Ver);				//封装版本低8位
    *pu8Buf++ = HI_UINT16(ptTx->u16Ver);				//封装版本高8位
    *pu8Buf++ = ptTx->u8Opt;							//封装控制位
    if(pu8Data != NULL && u16Len > 0)					//如果发送数组不为空 且 长度大于0
    {
        memcpy(pu8Buf, pu8Data, u16Len);				//发送数据数组存入发送的缓存数组
        pu8Buf += u16Len;								//指针指向最后校验和
    }
    
    sum = port_checksum(ptTx->pu8TxBuf,  len-2);				//检验和计算
    *pu8Buf++ = LO_UINT16(sum);							//封装检验和低8位
    *pu8Buf++ = HI_UINT16(sum);							//封装检验和高8位
    
    if(ptTx->ptProtTxFunc)								//如果发送回调不为空
        (*ptTx->ptProtTxFunc)(ptTx->pu8TxBuf, len);		//发送封装好的数据
        
    return SUCC;										//返回成功
}

/*----------------------------------------------------------------------------
函数名称：lm_prot_tx_cmd_req
具体描述：
输入参数：ptTx 发送解析结构体，包头到控制  u16Cmd 命令类型   pu8Param  发送数据数组  u16ParaLen  发送数据长度
----------------------------------------------------------------------------*/
int32_t lm_prot_tx_cmd_req(PROT_TX_PACK_T    *ptTx, uint16_t u16Cmd, uint8_t *pu8Param, uint16_t u16ParaLen)
{
    uint16_t len = 0;									//总长度
    uint16_t sum = 0;									//校验和
    uint8_t *pu8Buf = NULL;								//封装好的发送缓存数组
    //如果发送解析结构体为空 或者 发送数组为空 或者 长度为0
    if(ptTx == NULL || pu8Param == NULL || u16ParaLen == 0)
        return FAIL;									//返回失败
	//长度等于 发送的数据包长度 + 协议结构
    len = PROT_HEAD_INFO_LEN + 2 + u16ParaLen;    		
    if(GET_BIT(ptTx->u8Opt, PROT_OPTION_BIT_PARITY))	//如果开启校验功能
        len += 2;										//长度再加两位校验位长度
    
    if(len > ptTx->u16TxBufLen)							//如果实际长度大于发送数据的长度信息
    {
        return FAIL;									//返回失败
    }
    
    pu8Buf = ptTx->pu8TxBuf;							//数组指向传入发送结构体的数组
    *pu8Buf++ = PROT_HEAD_0;							//封装协议头0
    *pu8Buf++ = PROT_HEAD_1;							//封装协议头1
    *pu8Buf++ = LO_UINT16(len);							//封装长度低8位
    *pu8Buf++ = HI_UINT16(len);							//封装长度高8位
    *pu8Buf++ = LO_UINT16(ptTx->u16Ver);				//封装版本低8位
    *pu8Buf++ = HI_UINT16(ptTx->u16Ver);				//封装版本高8位
    *pu8Buf++ = ptTx->u8Opt;							//封装控制位
    *pu8Buf++ = LO_UINT16(u16Cmd);						//封装命令类型低8位
    *pu8Buf++ = HI_UINT16(u16Cmd);						//封装命令类型高8位

    if(pu8Param != NULL && u16ParaLen > 0)				//如果发送数组不为空 且 长度大于0
    {
        memcpy(pu8Buf, pu8Param, u16ParaLen);			//发送数据数组存入发送的缓存数组
        pu8Buf+=u16ParaLen;								//指针指向最后校验和
    }
    
    sum = port_checksum(ptTx->pu8TxBuf,  len-2);				//检验和计算
    *pu8Buf++ = LO_UINT16(sum);							//封装检验和低8位
    *pu8Buf++ = HI_UINT16(sum);							//封装检验和高8位

    if(ptTx->ptProtTxFunc)								//如果发送回调不为空
        (*ptTx->ptProtTxFunc)(ptTx->pu8TxBuf, len);		//发送封装好的数据
        
    return SUCC;										//返回成功
}

