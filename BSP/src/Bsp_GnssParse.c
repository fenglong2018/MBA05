/************************************************************************
*  文件名称:  Bsp_GnssParse.c
*  文件描述:  GNSS数据解析
*  历史记录:  
*  创建时间:  2024-05-10
*  创建人:    江昌钤
*  修改时间:
*  修改人:  
*  描述:
*************************************************************************/

/************************************************************************
头文件引用声明
*************************************************************************/
#include    <string.h>
#include    <stdlib.h>
#include    <stdio.h>
#include    <math.h>

#include    "Bsp_GnssParse.h"
#include    "Bsp_Log.h"
/************************************************************************
宏定义
*************************************************************************/
/* 成功 */
#ifndef SUCC
#define SUCC						0
#endif
/* 失败 */
#ifndef FAIL
#define FAIL						1
#endif
/************************************************************************
内部结构、枚举、公用体等结构声明
*************************************************************************/

/************************************************************************
内部静态变量定义

全局变量定义

extern 外部变量声明

*************************************************************************/
const GNSS_TYPE_T  tGNSSType[GNSS_TYPE_NUM]=
                    {
                       GNSS_TYPE_GPRMC, TYPE_GPRMC_HEAD,        //GPRMC
                       GNSS_TYPE_BDRMC, TYPE_BDRMC_HEAD,        //BDRMC
                       GNSS_TYPE_GNRMC, TYPE_GNRMC_HEAD         //GNRMC
                    };
GNSS_RX_UNPACK_T tGNSSRxCtrl;
RMC_DATA_T tRMCParseData; 
RMC_DATA_CHAR_T tCharRMCdata; 
GNSS_DATA_INF_T tGNSSdataCtr,tGNSSdataTmp; 
uint16_t	u16LocaTempNum = 0,u16LocaValidNum = 0,u16LocaValidNum_B = 0;
uint8_t   Buf_GnssSplit[GNSS_PARSE_BUF_LEN];
/************************************************************************
静态函数声明

extern 外部函数声明
*************************************************************************/

/************************************************************************
函数定义
*************************************************************************/
/*----------------------------------------------------------------------------
函数名称： Gnss_Check_Xor
具体描述： 异或校验
输入参数： *data 校验的数据       
           length 校验长度
----------------------------------------------------------------------------*/
uint8_t Gnss_Check_Xor(const void *data, uint16_t length)
{
  const uint8_t *buf = data;
  uint8_t retval = 0;
  buf++;  
  while(length)
  {
        retval ^= *buf++;
        --length;
  }
  return retval;
}
/*----------------------------------------------------------------------------
函数名称： Bsp_GNSS_Parse_Init
具体描述： GNSS数据清除
----------------------------------------------------------------------------*/
void Bsp_GNSS_Parse_Init(void)
{
    tGNSSRxCtrl.u16BufLen = 0;
    tGNSSRxCtrl.u8Step = 0;
    memset(&tGNSSRxCtrl.u8Pkt[0],0,GNSS_PARSE_BUF_LEN);
    memset(&Buf_GnssSplit[0],0,GNSS_PARSE_BUF_LEN);
}
/*----------------------------------------------------------------------------
函数名称:  Bsp_GNSS_Split_Data_Init
功能描述:  重置gnss解析数据
----------------------------------------------------------------------------*/
void Bsp_GNSS_Split_Data_Init(void)
{
    memset(&tRMCParseData,0,sizeof(RMC_DATA_T));
    memset(&tCharRMCdata,0,sizeof(RMC_DATA_CHAR_T));
    memset(&tGNSSdataCtr,0,sizeof(GNSS_DATA_INF_T));
    memset(&tGNSSdataTmp,0,sizeof(GNSS_DATA_INF_T));
    u16LocaTempNum = 0;
    u16LocaValidNum = 0;
    u16LocaValidNum_B = 0;
}
/*----------------------------------------------------------------------------
函数名称:  Bsp_GNSS_Init
功能描述:  GNSS解析初始化
----------------------------------------------------------------------------*/
void Bsp_GNSS_Init(void)
{
    Bsp_GNSS_Parse_Init();
    Bsp_GNSS_Split_Data_Init();
}
/*----------------------------------------------------------------------
*  函数名称:  Bsp_Gnss_RMC_Data_Split
*  功能描述:  GNSS RMC数据拆解
*  参数说明:  无
*  输入:      *ptData 得到的数据 *pu8Data 接收一包数组
*  输出:      SUCC->成功   FAIL->失败
*  返回值:    无
数据详解：$GPRMC,<0>,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>*hh
<0> UTC 时间，hhmmss(时分秒)格式
<1> 定位状态，A=有效定位，V=无效定位
<2> 纬度ddmm.mmmm(度分)格式(前面的0也将被传输)
<3> 纬度半球N(北半球)或S(南半球)
<4> 经度dddmm.mmmm(度分)格式(前面的0也将被传输)
<5> 经度半球E(东经)或W(西经)
<6> 地面速率(000.0~999.9节，前面的0也将被传输)
<7> 地面航向(000.0~359.9度，以真北为参考基准，前面的0也将被传输)
<8> UTC 日期，ddmmyy(日月年)格式
<9>磁偏角(000.0~180.0度，前面的0也将被传输)
<10>磁偏角方向，E(东)或W(西)
<11>模式指示(仅NMEA01833.00版本输出，A=自主定位，D=差分，E=估算，N=数据无效)
------------------------------------------------------------------------*/
uint8_t Bsp_Gnss_RMC_Data_Split(uint8_t *pu8Data,uint16_t u16Len)
{
  uint8_t u8Num = 0,u8Numnext = 0;
  uint8_t u8Findp = 0;
  uint8_t u8datanum;
  u8Numnext = GNSS_TYPE_SIZE;
  if(u16Len<u8Numnext)
      return FAIL;
  if(pu8Data[GNSS_TYPE_SIZE] != ',')
      return FAIL;
  u8Num = u8Numnext;
  for(u8datanum = 0;u8datanum <= 8;u8datanum++)
  {
    u8Num++;
    u8Findp = 0;
    while(!u8Findp)
    {
      u8Numnext++;
      if(u8Numnext < u16Len)
      {
        if(pu8Data[u8Numnext] == ',' || pu8Data[u8Numnext] == '*')
        {
            u8Findp = 1;
        }
      }
      else
      {
        memset(&tRMCParseData,0,sizeof(RMC_DATA_T));
        u8Findp = 1;
        return FAIL; 
      }   
    }
    switch(u8datanum)
    {
      /*  UTC 时间  */
      case 0:
      {
        memcpy(tCharRMCdata.u8UtcTime,&pu8Data[u8Num],u8Numnext-u8Num);
        tRMCParseData.u32UtcTime = (uint32_t)atof((char*)tCharRMCdata.u8UtcTime);
      }
        break;
      /* 定位状态 */
      case 1:
      {
        memcpy(tCharRMCdata.u8GnssState,&pu8Data[u8Num],u8Numnext-u8Num);
        tRMCParseData.u8GnssState = tCharRMCdata.u8GnssState[0];
      }
        break;
      /* 纬度 */
      case 2:
      {
        memcpy(tCharRMCdata.u8LatitudeValue,&pu8Data[u8Num],u8Numnext-u8Num);
        tRMCParseData.dLatitudeValue = atof((char*)tCharRMCdata.u8LatitudeValue);
      }
        break;
      /* 纬度半球 */
      case 3:
      {
        memcpy(tCharRMCdata.u8Latitude,&pu8Data[u8Num],u8Numnext-u8Num);
        tRMCParseData.u8Latitude = tCharRMCdata.u8Latitude[0];
        if(tRMCParseData.u8Latitude == '\0')
        {
            tRMCParseData.u8Latitude = 'N';
        }
      }
        break;
       /* 经度 */
      case 4:
      {
        memcpy(tCharRMCdata.u8LongtitudeValue,&pu8Data[u8Num],u8Numnext-u8Num);
        tRMCParseData.dLongtitudeValue = atof((char*)tCharRMCdata.u8LongtitudeValue);
      }
        break;
      /* 经度半球 */
      case 5:
      {
        memcpy(tCharRMCdata.u8Longitude,&pu8Data[u8Num],u8Numnext-u8Num);
        tRMCParseData.u8Longitude = tCharRMCdata.u8Longitude[0];
        if(tRMCParseData.u8Longitude == '\0')
        {
            tRMCParseData.u8Longitude = 'E';
        }
      }
        break;
      /* 航速 */
      case 6:
      {
        memcpy(tCharRMCdata.u8Speed,&pu8Data[u8Num],u8Numnext-u8Num);
        tRMCParseData.fSpeed = (float)atof((char*)tCharRMCdata.u8Speed);
      }
        break;
      /* 航向 */
      case 7:
      {
        memcpy(tCharRMCdata.u8Heading,&pu8Data[u8Num],u8Numnext-u8Num);
        tRMCParseData.fHead = (float)atof((char*)tCharRMCdata.u8Heading);
      }
        break;
      /* UTC日期 */
      case 8:
      {
        memcpy(tCharRMCdata.u8UtcDate,&pu8Data[u8Num],u8Numnext-u8Num);
        tRMCParseData.u32UtcDate = (uint32_t)atof((char*)tCharRMCdata.u8UtcDate);
      }
        break;
      default:
      {
      }
        break;
    }
    u8Num = u8Numnext;
  }
  return SUCC;
}
/*----------------------------------------------------------------------
函数名称:  Bsp_Gnss_Loca_If_Vaild
功能描述:  GNSS定位信息是否有效
------------------------------------------------------------------------*/
uint8_t Bsp_Gnss_Loca_If_Vaild(void)
{
    if(tRMCParseData.u32UtcTime == 0 || tRMCParseData.u32UtcDate == 0)
         return FAIL;
    if(tRMCParseData.u8GnssState == 'A')
    {
        return SUCC;
    }
    else
    {
        return FAIL;
    }      
}
/*----------------------------------------------------------------------
函数名称:  Api_Location_Data_Turn
功能描述:  度转分
输入参数:  dLocation      定位
------------------------------------------------------------------------*/
double Api_Location_Data_Turn(double dLocation)
{
    return (double)((uint32_t)(dLocation*100000)%10000000)/100000/60+(double)((uint32_t)dLocation/100);
}
/*----------------------------------------------------------------------
*  函数名称:  Api_Acc_Speed
*  功能描述:  加速度计算
*  输入:      tDisA  后者A点速度，tDisb  前者B点速度
*  输出:      加速度（米/秒2）
------------------------------------------------------------------------*/
uint16_t Api_Acc_Speed(GNSS_DATA_INF_T tDisA,GNSS_DATA_INF_T tDisb)
{
	return (uint16_t)(tDisA.u16Distance-tDisb.u16Distance);
}
/*----------------------------------------------------------------------
*  函数名称:  Api_Loca_Distance
*  功能描述:  坐标距离计算
*  输入:      tPointA  A点经纬度，tPointB  B点经纬度
*  输出:      距离（米）
------------------------------------------------------------------------*/
double Api_Loca_Distance(GNSS_DATA_INF_T tPointA,GNSS_DATA_INF_T tPointB)
{
    double pi = 3.1415926;
    double radian, radius, f, h, k, d, e;
    radian = pi / 180.0;
    radius = 6378137;
	f = tPointA.dLatValue * radian;
	h = tPointB.dLatValue * radian;
    k = 2 * radius;
	d = tPointB.dLongValue * radian - tPointA.dLongValue * radian;
    e = (1 - cos(h - f) + (1 - cos(d)) * cos(f) * cos(h)) / 2;
    return k * asin(sqrt(e));
}
/*----------------------------------------------------------------------
函数名称:  GNSS_Data_Deal
功能描述:  GNSS数据处理
------------------------------------------------------------------------*/
void Bsp_GNSS_Data_Deal(void)
{
  GNSS_DATA_INF_T  tDataTemp = {0};
	if(SUCC == Bsp_Gnss_Loca_If_Vaild())  //数据有效
	{
		//数据分度转换
		tDataTemp.dLongValue = Api_Location_Data_Turn(tRMCParseData.dLongtitudeValue);
		tDataTemp.dLatValue = Api_Location_Data_Turn(tRMCParseData.dLatitudeValue);
    tDataTemp.fSpeed = tRMCParseData.fSpeed;
    tDataTemp.fHead = tRMCParseData.fHead;
    tDataTemp.u32Date = tRMCParseData.u32UtcDate;
    tDataTemp.u32Time = tRMCParseData.u32UtcTime;
    if(tDataTemp.dLongValue == 0 || tDataTemp.dLatValue == 0)
    {
      u16LocaTempNum = 0;
      return;
    }
		switch(u16LocaTempNum)
		{
			//第0个试探定位
			case 0:
			{
				tGNSSdataTmp.dLongValue = tDataTemp.dLongValue;
				tGNSSdataTmp.dLatValue = tDataTemp.dLatValue;
				tGNSSdataTmp.u16Distance = 0;
				u16LocaTempNum++;
			}
				break;
			//第1个试探定位
			case 1:
			{
				tDataTemp.u16Distance = (uint16_t)Api_Loca_Distance(tGNSSdataTmp,tDataTemp);
				if(tDataTemp.u16Distance<35)
				{
					tGNSSdataTmp.dLongValue = tDataTemp.dLongValue;
					tGNSSdataTmp.dLatValue = tDataTemp.dLatValue;
					tGNSSdataTmp.u16Distance = tDataTemp.u16Distance;
					u16LocaTempNum++;
				}
				else
				{
					u16LocaTempNum = 0;
				}
			}
				break;
			//第2个试探定位，第0个有效定位
			case 2:
			{
				tDataTemp.u16Distance = (uint16_t)Api_Loca_Distance(tGNSSdataTmp,tDataTemp);
				if(tDataTemp.u16Distance<35)
				{
					if(Api_Acc_Speed(tDataTemp,tGNSSdataTmp) < 10)
					{
						tGNSSdataCtr.dLongValue = tDataTemp.dLongValue;
						tGNSSdataCtr.dLatValue = tDataTemp.dLatValue;
						tGNSSdataCtr.u16Distance = tDataTemp.u16Distance;
						u16LocaTempNum++;
					}
					else
					{
						u16LocaTempNum = 0;
					}
				}
				else
				{
					u16LocaTempNum = 0;
				}
			}
				break;
			//第1个有效定位
			case 3:
			{
				tDataTemp.u16Distance = (uint16_t)Api_Loca_Distance(tGNSSdataCtr,tDataTemp);
				if(tDataTemp.u16Distance<35)
				{
					if(Api_Acc_Speed(tDataTemp,tGNSSdataCtr) < 10)
					{
						tGNSSdataCtr.dLongValue = tDataTemp.dLongValue;
						tGNSSdataCtr.dLatValue = tDataTemp.dLatValue;
						tGNSSdataCtr.u16Distance = tDataTemp.u16Distance;
            tGNSSdataCtr.fSpeed = tDataTemp.fSpeed;
            tGNSSdataCtr.fHead = tDataTemp.fHead;
            tGNSSdataCtr.u32Time = tDataTemp.u32Time;
            tGNSSdataCtr.u32Date = tDataTemp.u32Date;
						u16LocaValidNum++;
					}
					else
					{
						u16LocaTempNum = 0;
					}
				}
				else
				{
					u16LocaTempNum = 0;
				}
			}
				break;
            default:
            {
                u16LocaTempNum = 0;
            }
            break;
		}
		
	}
	else
	{
		u16LocaTempNum = 0;
	}
}
/*----------------------------------------------------------------------------
函数名称:  Bsp_GNSS_Data_Parse_String
功能描述:  GNSS完整数据解析
输入参数:  *pu8Data    接收一包数组
           u16Len      完整数据长度
----------------------------------------------------------------------------*/
void Bsp_GNSS_Data_Parse_String(uint8_t *pu8Data,uint16_t u16Len)
{
  uint8_t u8i,u8Type = 0;
  for(u8i = 0;u8i < GNSS_TYPE_NUM;u8i++)
  {
    if(memcmp(&pu8Data[0], tGNSSType[u8i].u8GNSSEName, GNSS_TYPE_SIZE) == SUCC)
    {
        u8Type = tGNSSType[u8i].u8TypeHead;
    }
  }
  switch(u8Type)
  {
    case TYPE_NULL:
    {
        
    }
        break;
    case TYPE_GPRMC_HEAD:
    case TYPE_BDRMC_HEAD:
    case TYPE_GNRMC_HEAD:
    {
      #if   LOG
      printf("Receive GNSS RMC data :");
      Log_str_printf(pu8Data,u16Len);
      printf("GNSS RMC data Split\n");
      #endif
      if(SUCC == Bsp_Gnss_RMC_Data_Split(pu8Data,u16Len))
      {
        Bsp_GNSS_Data_Deal();
      }
    }
      break;
    default:
    {
        
    }
      break;
  }
}
/*----------------------------------------------------------------------------
函数名称： Bsp_Gnss_Data_Check
具体描述： GNSS数据检测正常
输入参数： *pu8Data     GNSS数组       
           length       GNSS数组长度
----------------------------------------------------------------------------*/
void Bsp_Gnss_Data_Check(uint8_t *pu8Data, uint16_t u16Len)
{
  uint8_t rxchecksum[2] = {0,0};
	uint8_t u8RxCheckSum = 0;
	uint8_t u8CalCheckSum = 0;
  memset(&Buf_GnssSplit[0],0,GNSS_PARSE_BUF_LEN);
  memcpy(&Buf_GnssSplit[0],&pu8Data[0],u16Len);
  if(u16Len<10)
  {
      return;
  } 
  memcpy(&rxchecksum[0],&Buf_GnssSplit[u16Len-4],2);
	sscanf((char *)rxchecksum,"%hhx", &u8RxCheckSum);
  u8CalCheckSum = Gnss_Check_Xor(&Buf_GnssSplit[0],(u16Len-6));
  if(u8RxCheckSum != u8CalCheckSum)
  {
      Bsp_GNSS_Parse_Init();
      return;
  }
  Bsp_GNSS_Data_Parse_String(&Buf_GnssSplit[0],u16Len);
  Bsp_GNSS_Parse_Init();
}
/*----------------------------------------------------------------------------
函数名称:  Bsp_Gnss_Parse_Byte
功能描述:  接收解析每个字节
输入参数:  u8Val    当前接收解析字节数据   
----------------------------------------------------------------------------*/
uint8_t Bsp_Gnss_Parse_Byte(uint8_t u8Val)    
{
  if(tGNSSRxCtrl.u8Step == 0)							                //如果解析第0字节
  {
    if(u8Val != GNSS_START)						                    //如果当前解析字节数据不等于'$'
    {		
      tGNSSRxCtrl.u8Step = 0;						                  //当前解析字节复位0
      return FAIL;								                        //退出且返回失败
    }
    else											                            //如果等于协议头0
    {
      tGNSSRxCtrl.u8Pkt[tGNSSRxCtrl.u8Step++] = u8Val;		//解析第1字节，当前字节数据存入数组0
    }
  }
  else												
  {
    tGNSSRxCtrl.u8Pkt[tGNSSRxCtrl.u8Step++] = u8Val;				
    if(u8Val != GNSS_STOP)
    {
      if(tGNSSRxCtrl.u8Step > GNSS_MAX_BUF_LEN)
      {
        tGNSSRxCtrl.u8Step = 0;					                  //当前解析字节复位0
        return FAIL;							                        //退出且返回失败
      }
    }
    else
    {
      Bsp_Gnss_Data_Check(tGNSSRxCtrl.u8Pkt, tGNSSRxCtrl.u8Step);
      tGNSSRxCtrl.u8Step = 0;						                  //当前解析字节复位0
      return SUCC;								                        //退出且返回成功
    }
  }
  return SUCC;										                        //返回成功
}

/*----------------------------------------------------------------------------
函数名称:  Bsp_Get_Location
功能描述:  获得定位信息
----------------------------------------------------------------------------*/
uint8_t Bsp_Get_Location(GNSS_DATA_INF_T *pLocaData)
{
	if(u16LocaValidNum_B != u16LocaValidNum)
	{
		u16LocaValidNum_B = u16LocaValidNum;
		pLocaData->dLatValue = tGNSSdataCtr.dLatValue;
		pLocaData->dLongValue = tGNSSdataCtr.dLongValue;
    pLocaData->fSpeed = tGNSSdataCtr.fSpeed;
    pLocaData->fHead = tGNSSdataCtr.fHead;
    pLocaData->u32Time = tGNSSdataCtr.u32Time;
    pLocaData->u32Date = tGNSSdataCtr.u32Date;
    #if   LOG
    printf("Current Time : 20%d-%d-%d %d:%d:%d\n",tGNSSdataCtr.u32Date%100,tGNSSdataCtr.u32Date/100%100,tGNSSdataCtr.u32Date/10000,\
                                                  tGNSSdataCtr.u32Time/10000+8,tGNSSdataCtr.u32Time/100%100,tGNSSdataCtr.u32Time%100);
    printf("Current Location : %f E %f N\n",tGNSSdataCtr.dLongValue,tGNSSdataCtr.dLatValue);
    printf("Current Speed : %.2f Knot,Current Heading : %.2f\n",tGNSSdataCtr.fSpeed,tGNSSdataCtr.fHead);
    #endif
		return SUCC;
	}
	else
	{
		return FAIL;
	}
}

