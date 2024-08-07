/********************************************************************** 
*  文件名称:    Bsp_GnssParse.h
*  文件描述:    GNSS数据解析
*  历史记录:  
*  创建时间:    2023-04-25  
*  创建人:      江昌钤
*  修改时间:
*  修改人:  
*  描述: 	  
***********************************************************************/

#ifndef _BSP_GNSSPARSE_H_
#define _BSP_GNSSPARSE_H_

/********************************************************************** 
头文件引用声明
***********************************************************************/
#include    <stdint.h>

/********************************************************************** 
宏定义
***********************************************************************/
#define     IF_GGA_PARSE            0
#define     IF_RMC_PARSE            1
#define     GNSS_TYPE_GPGGA         "$GPGGA"
#define     GNSS_TYPE_BDGGA         "$BDGGA"
#define     GNSS_TYPE_GNGGA         "$GNGGA"
#define     GNSS_TYPE_GPRMC         "$GPRMC"
#define     GNSS_TYPE_BDRMC         "$BDRMC"
#define     GNSS_TYPE_GNRMC         "$GNRMC"
#define     GNSS_TYPE_XXGGA         "GGA"
#define     GNSS_TYPE_XXRMC         "RMC"
#define     GNSS_TYPE_SIZE          6
#define     GNSS_TYPE_NUM           3

/*  GNSS解析开始符 */
#define     GNSS_START              '$'
/*  GNSS解析校验符 */
#define     GNSS_CHK                '*'
/*  GNSS解析停止符 */
#define     GNSS_STOP               '\n'
/*  GNSS最大解析长度 */
#define     GNSS_MAX_BUF_LEN        100
/*  GNSS最大解析数组长度 */
#define     GNSS_PARSE_BUF_LEN      200
/********************************************************************** 
结构、枚举、公用体等结构定义
***********************************************************************/
typedef struct
{
    uint8_t     u8Pkt[GNSS_PARSE_BUF_LEN];      /* 解析缓存 */
    uint16_t    u16BufLen;                      /* 解析缓存长度 */
    uint8_t     u8Step;
}GNSS_RX_UNPACK_T;

typedef struct
{
	uint8_t	u8GNSSEName[10];
	uint8_t	u8TypeHead;
}GNSS_TYPE_T;

typedef struct
{
	uint16_t	  u16Distance;	  //和上次距离
  double      dLongValue;     //经度
  double      dLatValue;      //纬度
  uint32_t    u32Time;        //时间
  uint32_t    u32Date;        //日期
  float       fSpeed;         //航速
  float       fHead;          //航向
}GNSS_DATA_INF_T;

enum
{
  TYPE_NULL = 0,
	TYPE_GPRMC_HEAD,
  TYPE_BDRMC_HEAD,
	TYPE_GNRMC_HEAD
};

typedef struct
{
  uint8_t     u8PaeseOK;                      // 解析OK
	uint8_t	    u8LocaOK;						            // 当前定位是否有效
	uint32_t	  u32UtcTime;			                // UTC时间
  uint32_t    u32UtcDate;                     // UTC日期
  uint8_t	    u8GnssState;					          // 在接收到有效数据前，这个位是‘V’，后面的数据都为空，接到有效数据后，这个位是‘A’，后面才开始有数据
	double	    dLatitudeValue;				          // 纬度
	uint8_t	    u8Latitude;						          // 纬度半球
	double	    dLongtitudeValue;			          // 经度
	uint8_t	    u8Longitude;					          // 经度半球	
  float       fSpeed;                         // 船速
  float       fHead;                          // 船首向
}RMC_DATA_T;

typedef struct  
{
	uint8_t u8UtcTime[11];		                  //UTC时间
  uint8_t u8UtcDate[7];                       //UTC日期
  uint8_t u8GnssState[2];                     //定位状态V/A
	uint8_t u8LatitudeValue[10];		            //纬度
	uint8_t u8Latitude[2];		                  //纬度半球N/S
	uint8_t u8LongtitudeValue[11];		          //经度
	uint8_t u8Longitude[2];		                  //经度半球	E/W
  uint8_t u8Speed[10];                        //船速
  uint8_t u8Heading[8];                       //船首向
}RMC_DATA_CHAR_T;
/********************************************************************** 
源文件定义变量声明

extern 外部变量声明
***********************************************************************/


/********************************************************************** 
源文件函数声明

extern 外部函数声明
***********************************************************************/
/*----------------------------------------------------------------------------
函数名称:  Bsp_GNSS_Init
功能描述:  GNSS解析初始化
----------------------------------------------------------------------------*/
void Bsp_GNSS_Init(void);
/*----------------------------------------------------------------------------
函数名称:  Bsp_Gnss_Parse_Byte
功能描述:  接收解析每个字节
输入参数:  u8Val    当前接收解析字节数据   
----------------------------------------------------------------------------*/
uint8_t Bsp_Gnss_Parse_Byte(uint8_t u8Val);
/*----------------------------------------------------------------------------
函数名称:  Bsp_Get_Location
功能描述:  获得定位信息
----------------------------------------------------------------------------*/
uint8_t Bsp_Get_Location(GNSS_DATA_INF_T *pLocaData);
/*----------------------------------------------------------------------
函数名称:  Bsp_Gnss_Loca_If_Vaild
功能描述:  GNSS定位信息是否有效
------------------------------------------------------------------------*/
uint8_t Bsp_Gnss_Loca_If_Vaild(void);
#endif
