
/********************************************************************** 
*  文件名称: api_common.h
*  文件描述: 一些常用的操作、算法等公用函数模块
*  历史记录:  
*  创建时间:  2022-05-7
*  创建人:    江昌钤
*  修改时间:
*  修改人:  
*  描述: 	  
***********************************************************************/
#ifndef _API_COMMON_H_
#define _API_COMMON_H_
/********************************************************************** 
头文件引用声明
***********************************************************************/
#include <stdint.h>
/********************************************************************** 
宏定义
***********************************************************************/
/* 硬件版本 */
#define HW_DEMO_BOARD              	0
#define HW_SCH_LM_WIFI_R8710_V10    1

/* 成功 */
#ifndef SUCC
#define SUCC						0
#endif
/* 失败 */
#ifndef FAIL
#define FAIL						1
#endif
/* 正确 */
#ifndef TRUE
#define TRUE						1
#endif
/* 错误 */
#ifndef FALSE
#define FALSE 						0
#endif
/* 设置 */
#ifndef SET
#define SET						    1
#endif
/* 复位 */
#ifndef RESET
#define RESET 						0
#endif
/* BIT */
#ifndef BIT
#define BIT(n)      				(1 << (n))
#endif
/* 取小值 */
#ifndef MIN
#define MIN(n,m)   					(((n) < (m)) ? (n) : (m))
#endif
/* 取大值 */
#ifndef MAX
#define MAX(n,m)   					(((n) < (m)) ? (m) : (n))
#endif
/* 加符号 */
#ifndef ABS
#define ABS(n)     					(((n) < 0) ? -(n) : (n))
#endif
/* 取BIT */
#define GET_BIT(data, bit)			((data) & BIT(bit))
/* 取地址值 */
#define REG_VAL(a)					(*((volatile uint32_t *)(a)))

/* 从32位数中取一字节 : var - 待取uint32数,  ByteNum - 第 (0 - 3)字节 */
#define BREAK_UINT32( var, ByteNum ) \
          							(uint8_t)((uint32_t)(((var) >>((ByteNum) * 8)) & 0x00FF))

/* 4字节合并成uint32数 : Byte0~Byte3 对应 低位开始的0-3字节*/
#define BUILD_UINT32(Byte3, Byte2, Byte1, Byte0) \
          							((uint32_t)((uint32_t)((Byte0) & 0x00FF) \
							          + ((uint32_t)((Byte1) & 0x00FF) << 8) \
							          + ((uint32_t)((Byte2) & 0x00FF) << 16) \
							          + ((uint32_t)((Byte3) & 0x00FF) << 24)))

/* 2字节合并成uint16数 : loByte 低字节  hiByte 高字节*/
#define BUILD_UINT16(hiByte, loByte) \
         							 ((uint16_t)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))

/* 取16位中的高8位 */
#define HI_UINT16(a) 				(((a) >> 8) & 0xFF)
/* 取16位中的低8位 */
#define LO_UINT16(a) 				((a) & 0xFF)

/* 16位大小端反序 */
#define SWAP16(a) 					(BUILD_UINT16(LO_UINT16(a), HI_UINT16(a)))

/* 8位反序 */
#define BUILD_UINT8(hiByte, loByte) \
          							((uint8_t)(((loByte) & 0x0F) + (((hiByte) & 0x0F) << 4)))

/* 取一字节中的高4位 */
#define HI_UINT8(a) 				(((a) >> 4) & 0x0F)
/* 取一字节中的高4位 */
#define LO_UINT8(a) 				((a) & 0x0F)

/* 是否闰年 */
#define	IS_LEAP_YEAR(yr)			(!((yr) % 400) || (((yr) % 100) && !((yr) % 4)))
/* 年天数 */
#define	YEAR_LENTH(yr)				(IsLeapYear(yr) ? 366 : 365)

/********************************************************************** 
结构、枚举、公用体等结构定义
***********************************************************************/

/********************************************************************** 
源文件定义变量声明

extern 外部变量声明
***********************************************************************/


/********************************************************************** 
源文件函数声明

extern 外部函数声明
***********************************************************************/
/********************************************************************** 
*  函数名称: be_big_endian
*  功能描述: 判断内存是否是大端对齐
*  参数说明: 
*  输入:	 无
*  返回值:	1：大端；0:小端；	  
***********************************************************************/
uint8_t be_big_endian(void);


/********************************************************************** 
*  函数名称: calc_parity
*  功能描述: 计算奇偶校验位
*  参数说明:  
*  输入:	 buf: 待计算uint8_t数据源地址
*			 buflen:待计算数据长度
*  输出:    无
*  返回值:  void,  成功:指向被分配内存的指针   失败:NULL   	  
***********************************************************************/
uint8_t calc_parity(uint8_t * buf, uint32_t buflen);
/* 计算和校验 */
uint16_t checksum(uint8_t * buf, uint32_t buflen);

/* 打印多个1字节HEX数 */
void print_multi_hex_8(uint8_t *pData, uint32_t len);

/* 打印1字节HEX数 */
void print_single_hex_8(uint8_t pData);

/* 打印多个2字节HEX数 */
void print_multi_hex_16(uint16_t *pData, uint32_t len);

/* 打印2字节HEX数 */
void print_single_hex_16(uint16_t pData);

/* 打印多个4字节HEX数 */
void print_multi_hex_32(uint32_t *pData, uint32_t len);

/* 打印4字节HEX数 */
void print_single_hex_32(uint32_t pData);
/*
 *计算二进制数中1的个数
 *运算次数与输入n的大小无关，只与n中1的个数有关。如果n的二进制表示中有k个1，
 *那么这个方法只需要循环k次即可。其原理是不断清除n的二进制表示中最右边的1，
 *同时累加计数器，直至n为0
 */
uint8_t bit_count(uint16_t n);
/*----------------------------------------------------------------------
//  函数名称:  convert_str_to_hex
//  功能描述:  ASCII转HEX字符
//  参数说明:  无
//  输入:      *str  ASCII数组，   str_len  ASCII数组长度，*out_str HEX字符输出数组  out_len  输出长度
//  输出:      无
//  返回值:    无
//---------------------------------------------------------------------*/
int convert_str_to_hex(uint8_t *str, uint32_t str_len, uint8_t *out_str, uint32_t out_len);
uint32_t convert_str_to_uint32(uint8_t *str,uint8_t str_len);
uint8_t convert_str_to_uint8(uint8_t *str,uint8_t str_len);
/*----------------------------------------------------------------------------
函数名称： Fundtoa
具体描述： 整数转字符串
----------------------------------------------------------------------------*/
uint8_t Fundtoa(uint8_t *pOut,uint32_t u32num,uint8_t u8Bit);
uint32_t TimeTurn(uint32_t u32Time,uint32_t u32Date);
#endif


