/**********************************************************************
*  文件名称: lm_prot.h
*  文件描述: 通讯协议解析和封装
*  历史记录:  
*  创建时间: 
*  创建人:   林开洲
*  修改时间: 20220225
*  修改人:  
*  描述:
*  基本使用流程参考
*  1、定义发送缓存及发送封装实体PROT_TX_PACK_T，定义接收缓存及接收解析实体PROT_RX_UNPACK_T；
*  2、定义底层发送函数及接收解析结果处理回调函数；
*  3、初始化各参数，如协议版本，是否开始校验位，缓存地址及长度关联；
*  4、把待解析数据传入lm_prot_rx_parse；
*  5、调用lm_prot_tx_cmd_req/调用lm_prot_tx_cmd_req发送数据；
***********************************************************************/

#ifndef _LM_PROT_H_
#define _LM_PROT_H_

/********************************************************************** 
头文件引用声明
***********************************************************************/
#include "stdio.h"

//#include "lm_includes.h"

/********************************************************************** 
宏定义
***********************************************************************/
#if 1
#ifndef SUCC
#define SUCC                            0
#endif
#ifndef FAIL
#define FAIL                            -1
#endif
    
#ifndef BIT
#define BIT(n)                          (1 << (n))
#endif
        
#define GET_BIT(data, bit)              ((data) & BIT(bit))
        
#define BREAK_UINT32( var, ByteNum )    \
                                            (uint8_t)((uint32_t)(((var) >>((ByteNum) * 8)) & 0x00FF))
        
#define BUILD_UINT32(Byte3, Byte2, Byte1, Byte0) \
                                            ((uint32_t)((uint32_t)((Byte0) & 0x00FF) \
                                              + ((uint32_t)((Byte1) & 0x00FF) << 8) \
                                              + ((uint32_t)((Byte2) & 0x00FF) << 16) \
                                              + ((uint32_t)((Byte3) & 0x00FF) << 24)))
        
#define BUILD_UINT16(hiByte, loByte)    ((uint16_t)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))
        
#define HI_UINT16(a)                    (((a) >> 8) & 0xFF)
#define LO_UINT16(a)                    ((a) & 0xFF)
        
#define SWAP16(a)                       (BUILD_UINT16(LO_UINT16(a), HI_UINT16(a)))
        
#define BUILD_UINT8(hiByte, loByte)     ((uint8_t)(((loByte) & 0x0F) + (((hiByte) & 0x0F) << 4)))
        
#define HI_UINT8(a)                     (((a) >> 4) & 0x0F)
#define LO_UINT8(a)                     ((a) & 0x0F)
    
#if (1)
#define log_trace(x, ...)               do{printf("T:"x, ##__VA_ARGS__);}while(0)
#define log_error(x, ...)               do{printf("E:"x, ##__VA_ARGS__);}while(0)
#define log_print                       printf
#else
#define log_trace(x, ...)
#define log_error(x, ...)
#define log_print
#endif

#endif


#define LM_STD_PROT_TEST                            1
/* 
    帧格式：2字节头(0xaa, 0x55)，2字节长度，2字节协议版本， 1字节功能控制位，n字节负载，2字节可选校验位
    小端对方模式；
*/
#define PROT_IDX_HEAD                           0
#define PROT_IDX_PKT_LEN                        2                   //帧头到校验码的所有字节数
#define PROT_IDX_VER                            4                   //协议版本
#define PROT_IDX_OPTION                         6                   //功能控制位
#define PROT_IDX_PLD                            7                   //命令2字节+可选参数

#define PROT_HEAD_INFO_LEN                      7                   /* 解析完帧头2字节，长度2字节，版本2字节，功能控制1字节 */

#define PROT_HEAD_0                             0XAA
#define PROT_HEAD_1                             0X55

#define PROT_ERR_TYPE_NONE                      (0X00)              //没有错误
#define PROT_ERR_TYPE_LEN                       (0X01)              //长度不对
#define PROT_ERR_TYPE_PARITY                    (0X02)              //校验位错误
#define PROT_ERR_TYPE_NOT_SUPPORT               (0X03)              //无此命令
#define PROT_ERR_TYPE_PARA                      (0X04)              //参数不对
#define PROT_ERR_TYPE_CONDITION                 (0X05)              //当前状态不支持
#define PROT_ERR_TYPE_OTHER                     (0XFF)              //未知其他错误

#define PROT_OPTION_BIT_PARITY                  0                   //是否开启校验功能，暂时不考虑
#define PROT_OPTION_BIT_MORE                    1                   //一条控制指令是否未接收完，暂时没用
#define PROT_OPTION_BIT_REQUIRE_ACK             2                   //是否需要对方回复执行结果，暂时没用

/********************************************************************** 
结构、枚举、公用体等结构定义
***********************************************************************/
#if 1
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

typedef signed int int32_t;
typedef signed short int16_t;
typedef signed char int8_t;

#endif

typedef enum
{
    PARSE_RESULT_INVALID = 0,       /* 未解析到头的情况下收到非头字段数据 */
    PARSE_RESULT_PARSING,           /* 正常一条命令解析中(已收到协议头)，但未有整条解析结果 */
    PARSE_RESULT_TOO_SHORT,         /* 长度字段太小 */
    PARSE_RESULT_TOO_LONG,          /* 长度字段超过最大值 */
    PARSE_RESULT_CHECKSUM_ERROR,    /* 校验位错误 */
    PARSE_RESULT_SUCC               /* 正常解析出一条命令 */
}PARSE_RESULT_E;

/* 接收数据按字节解析，结果回调函数，其中返回成功或者校验错误时pu8Data和u16Len才有效， */
typedef void (*prot_tx_io_func)(uint8_t *pu8Data, uint16_t u16Len);
typedef void (*prot_rx_parse_cb_func)(PARSE_RESULT_E eResult, uint8_t *pu8Data, uint16_t u16Len);

typedef struct
{
    prot_rx_parse_cb_func ptProtRxParseCallbackFunc;    /* 接收解析处理回调接口 */

    uint8_t *pu8Pkt;        /* 解析缓存 */
    uint16_t u16BufLen;      /* 解析缓存长度 */
    uint8_t u8Step;
}PROT_RX_UNPACK_T;

typedef struct
{
    prot_tx_io_func ptProtTxFunc;   /* 底层发送回调接口 */
    
    uint16_t u16Ver;        /* 版本 */
    uint8_t u8Opt;          /* 控制配置信息 */
    uint8_t *pu8TxBuf;      /* 发送缓存 */
    uint16_t u16TxBufLen;   /* 发送缓存长度 */
}PROT_TX_PACK_T;

/********************************************************************** 
源文件定义变量声明

extern 外部变量声明
***********************************************************************/

/********************************************************************** 
源文件函数声明

extern 外部函数声明
***********************************************************************/
/* 指定字节流数据数据解析*/
int32_t lm_prot_rx_parse_byte(PROT_RX_UNPACK_T *ptParse, uint8_t u8Val);

/* 指定长度字节流数据数据解析*/
int32_t lm_prot_rx_parse(PROT_RX_UNPACK_T *ptRxParse, uint8_t *pu8Data, uint16_t u16Len);

/* 复位解析步骤，从头开始解析数据,超时没有再收到数据，则丢弃之前数据并复位解析流程 */
int32_t lm_prot_rx_parse_step_reset(PROT_RX_UNPACK_T *ptRxParse);

/* 获取接收包体长度 */
uint16_t lm_prot_get_rx_pkt_len(uint8_t *pu8RxPkt);

/* 获取接收包体协议版本号 */
uint16_t lm_prot_get_rx_pkt_ver(uint8_t *pu8RxPkt);

/* 获取接收包体功能配置字段 */
uint8_t lm_prot_get_rx_pkt_option(uint8_t *pu8RxPkt);

/* 获取接收包体校验位 */
uint16_t lm_prot_get_rx_pkt_checksum(uint8_t *pu8RxPkt);

/* 获取接收包体负载数据(应用层数据)长度 */
uint16_t lm_prot_get_rx_pkt_pld_len(uint8_t *pu8RxPkt);

/* 获取接收包体负载数据(应用层数据)，返回地址 */
uint8_t *lm_prot_get_rx_pkt_pld(uint8_t *pu8RxPkt);


/* 发送应用层原始数据 */
int32_t lm_prot_tx(PROT_TX_PACK_T *ptTx, uint8_t *pu8Data, uint16_t u16Len);

/* 发送应用层请求命令，格式：命令 + 参数   */
int32_t lm_prot_tx_cmd_req(PROT_TX_PACK_T    *ptTx, uint16_t u16Cmd, uint8_t *pu8Param, uint16_t u16ParaLen);

/* 发送应用层响应数据，格式：命令 + 错误码 + 参数   */
int32_t lm_prot_tx_cmd_rep(PROT_TX_PACK_T *ptTx, uint16_t u16Cmd, uint8_t u8ErrType, uint8_t *pu8Param, uint16_t u16ParaLen);

#if LM_STD_PROT_TEST
int32_t lm_prot_demo(void);


#endif

#endif
