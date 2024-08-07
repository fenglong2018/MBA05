/**********************************************************************
*  	文件名称:  api_common.c
*  	文件描述:  一些常用的操作、算法等公用函数模块
*
*  	历史记录:
*  	创建时间:  2019-04-12
*   创建人:    江昌钤
*  	修改时间:
*	修改人:
*   描述:
***********************************************************************/

/*************************************************************************
头文件引用声明
*************************************************************************/
#include    <string.h>
#include    <stdio.h>
#include    <time.h>

#include    "api_common.h"
/********************************************************************** 
宏定义
***********************************************************************/

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
/** 返回0表示小端， 返回1表示大端**/
uint8_t be_big_endian(void)
{
	unsigned short value = 0x1234;
	return (*(unsigned char*)(&value) == 0x12);
}


/* 计算奇偶校验位*/
uint8_t calc_parity(uint8_t * buf, uint32_t buflen)
{
	uint8_t lrc = 0;
	if (buf != NULL) {
		while(buflen--) {
			lrc ^= *buf++;
		}
	}
	
	return lrc;
}

/*----------------------------------------------------------------------------
函数名称：checksum
具体描述：计算累加检验和
输入参数：buf 数组 buflen 数据长度
输出参数：校验和
----------------------------------------------------------------------------*/
uint16_t checksum(uint8_t * buf, uint32_t buflen)
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
函数名称：print_multi_hex_8
具体描述：打印一串一字节HEX数
输入参数：pData 需要打印的HEX数 len 数据长度
----------------------------------------------------------------------------*/
void print_multi_hex_8(uint8_t *pData, uint32_t len)
{
	uint32_t i=0;

	for(i=0; i<len; i++)
	{
		printf("%02X ", *(pData+i));
	}
    
	printf("\r\n");
}
/*----------------------------------------------------------------------------
函数名称：print_single_hex_8
具体描述：打印一字节HEX数
输入参数：pData 需要打印的HEX数
----------------------------------------------------------------------------*/
void print_single_hex_8(uint8_t pData)
{
	printf("%02X ", pData);
	printf("\r\n");
}
/*----------------------------------------------------------------------------
函数名称：print_multi_hex_16
具体描述：打印一串两字节HEX数
输入参数：pData 需要打印的HEX数 len 数据长度
----------------------------------------------------------------------------*/
void print_multi_hex_16(uint16_t *pData, uint32_t len)
{
	uint32_t i=0;

	for(i=0; i<len; i++)
		printf("%04X ", *(pData+i));

	printf("\r\n");
}
/*----------------------------------------------------------------------------
函数名称：print_single_hex_16
具体描述：打印一个两字节HEX数
输入参数：pData 需要打印的HEX数
----------------------------------------------------------------------------*/
void print_single_hex_16(uint16_t pData)
{
	printf("%04X ", pData);
	printf("\r\n");
}
/*----------------------------------------------------------------------------
*  函数名称：print_multi_hex_32
*  具体描述：打印一串四字节HEX数
*  输入：pData 需要打印的HEX数 len 数据长度
*  输出:      无
----------------------------------------------------------------------------*/
void print_multi_hex_32(uint32_t *pData, uint32_t len)
{
	uint32_t i=0;

	for(i=0; i<len; i++)
		printf("%08X ", *(pData+i));

	printf("\r\n");
}

/*----------------------------------------------------------------------------
函数名称：print_single_hex_32
具体描述：打印一个四字节HEX数
输入参数：pData 需要打印的HEX数
----------------------------------------------------------------------------*/
void print_single_hex_32(uint32_t pData)
{
	printf("%08X ", pData);
	printf("\r\n");
}

/*----------------------------------------------------------------------------
 *计算二进制数中1的个数
 *运算次数与输入n的大小无关，只与n中1的个数有关。如果n的二进制表示中有k个1，
 *那么这个方法只需要循环k次即可。其原理是不断清除n的二进制表示中最右边的1，
 *同时累加计数器，直至n为0
----------------------------------------------------------------------------*/
uint8_t bit_count(uint16_t n)
{
	uint8_t c =0;

	for (c =0; n; ++c)
	{
		/* 清除最低位的1 */
		n &= (n -1);
	}

	return c ;
}
/*----------------------------------------------------------------------
*  函数名称:  convert_str_to_hex
*  功能描述:  ASCII转HEX字符
*  输入:      *str  ASCII数组，   str_len  ASCII数组长度，*out_str HEX字符输出数组  out_len  输出长度
*  输出:      FAIL 失败     FAIL 成功
//---------------------------------------------------------------------*/
int convert_str_to_hex(uint8_t *str, uint32_t str_len, uint8_t *out_str, uint32_t out_len)
{
    uint16_t i = 0;
    uint8_t ch1 = 0, ch2 = 0;
    if(str_len * 2 > out_len)
    {
        return FAIL;
    }
    
    for(i = 0; i < str_len; i++)
    {
        ch1 = (str[i] & 0xf0) >> 4;
        ch2 = (str[i] & 0x0f);

        ch1 += ((ch1 > 9) ? 0x57 : 0x30);
        ch2 += ((ch2 > 9) ? 0x57 : 0x30);

        out_str[2*i + 0] = ch1;
        out_str[2*i + 1] = ch2;
    }

    out_str[str_len * 2 ] = 0;
    out_len = str_len * 2;
    return SUCC;
}

/*----------------------------------------------------------------------
*  函数名称:  convert_str_to_uint32
*  功能描述:  字符串转无符号整型
*  输入:      *str  字符串，   str_len  字符串长度
*  输出:      uint32数
//---------------------------------------------------------------------*/
uint32_t convert_str_to_uint32(uint8_t *str,uint8_t str_len)
{
    uint32_t u32Num = 0;
    uint8_t u8Len = 0;
    if (NULL == *str)	//字符串为空
		return 0;
    while (u8Len<str_len)
    {
        if (str[u8Len]<'0' || str[u8Len]>'9')		//只要当前字符不在数字字符的范围内，直接跳出循环
            break;
        else
            u32Num = u32Num * 10 + (str[u8Len] - '0');		//括号里面的是二者字符对应ASCII值的差值，即为当前位置的字符对应的整型值
        u8Len++;
    }
	return u32Num;
}

/*----------------------------------------------------------------------
*  函数名称:  convert_str_to_uint8
*  功能描述:  字符串转无符号整型
*  输入:      *str  字符串，   str_len  字符串长度
*  输出:      uint8数
//---------------------------------------------------------------------*/
uint8_t convert_str_to_uint8(uint8_t *str,uint8_t str_len)
{
    uint8_t u8Num = 0;
    uint8_t u8Len = 0;
    if (NULL == *str)	//字符串为空
		return 0;
    while (u8Len<str_len)
    {
        if (str[u8Len]<'0' || str[u8Len]>'9')		//只要当前字符不在数字字符的范围内，直接跳出循环
            break;
        else
            u8Num = u8Num * 10 + (str[u8Len] - '0');		//括号里面的是二者字符对应ASCII值的差值，即为当前位置的字符对应的整型值
        u8Len++;
    }
	return u8Num;
}
/*----------------------------------------------------------------------------
函数名称： Fundtoa
具体描述： 整数转字符串
----------------------------------------------------------------------------*/
uint8_t Fundtoa(uint8_t *pOut,uint32_t u32num,uint8_t u8Bit)
{
    uint8_t k;
    uint32_t u32intNum;
    u32intNum = u32num;
    for(k = u8Bit;k>0;k--)
    {
        pOut[k-1] = u32intNum%10 + '0';
        u32intNum = u32intNum/10;
    }
    return SUCC;
}
/*----------------------------------------------------------------------------
函数名称： TimeTurn
具体描述： 时间戳转换
输入参数： u32Time 时间 hhmmss    u32Date 日期 ddmmyy
----------------------------------------------------------------------------*/
uint32_t TimeTurn(uint32_t u32Time,uint32_t u32Date)
{
    uint32_t     u32Timestamp = 0;
    uint8_t year = 0,month = 0,date = 0,hours = 0,minutes = 0,seconds = 0;
    struct tm local_time; 
    if(u32Time > 999999 || u32Date > 999999)
        return 0;
    if(u32Time == 0 || u32Date == 0)
        return 0;
    year = u32Date%100;
    month = (u32Date/100)%100;
    date = u32Date/10000;
    
    seconds = u32Time%100;
    minutes = (u32Time/100)%100;
    hours = u32Time/10000;
    /*调用系统的时间结构体，并填充数据 */
    local_time.tm_year = year+2000-1900; // 注意：这边加上2000是因为我读出来的年是22，
   														//	不是2022。后面减去1900,是看到调用mktime接口必须要减的。
   														//  原因你们可以自己查一下
   local_time.tm_mon  = month-1; //月份要减一，这个你可以自己验证下不减，时间戳转换出来会多一个月
   local_time.tm_mday  = date;
   local_time.tm_hour  = hours; //这个-8好像是因为时区的原因
   local_time.tm_min  = minutes;
   local_time.tm_sec  = seconds;
   u32Timestamp = mktime(&local_time);
   //printf("Timestamp = %d\n",u32Timestamp);
#if LOG
   printf("TimeStamp %d",u32Timestamp);
   printf("Time : %0.2d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d\n",
           year,
           month,
           date,
           hours,
           minutes,
           seconds);
#endif
    return  u32Timestamp;
}
