/************************************************************************
*  �ļ�����:  Bsp_GnssParse.c
*  �ļ�����:  GNSS���ݽ���
*  ��ʷ��¼:  
*  ����ʱ��:  2024-05-10
*  ������:    ������
*  �޸�ʱ��:
*  �޸���:  
*  ����:
*************************************************************************/

/************************************************************************
ͷ�ļ���������
*************************************************************************/
#include    <string.h>
#include    <stdlib.h>
#include    <stdio.h>
#include    <math.h>

#include    "Bsp_GnssParse.h"
#include    "Bsp_Log.h"
/************************************************************************
�궨��
*************************************************************************/
/* �ɹ� */
#ifndef SUCC
#define SUCC						0
#endif
/* ʧ�� */
#ifndef FAIL
#define FAIL						1
#endif
/************************************************************************
�ڲ��ṹ��ö�١�������Ƚṹ����
*************************************************************************/

/************************************************************************
�ڲ���̬��������

ȫ�ֱ�������

extern �ⲿ��������

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
��̬��������

extern �ⲿ��������
*************************************************************************/

/************************************************************************
��������
*************************************************************************/
/*----------------------------------------------------------------------------
�������ƣ� Gnss_Check_Xor
���������� ���У��
��������� *data У�������       
           length У�鳤��
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
�������ƣ� Bsp_GNSS_Parse_Init
���������� GNSS�������
----------------------------------------------------------------------------*/
void Bsp_GNSS_Parse_Init(void)
{
    tGNSSRxCtrl.u16BufLen = 0;
    tGNSSRxCtrl.u8Step = 0;
    memset(&tGNSSRxCtrl.u8Pkt[0],0,GNSS_PARSE_BUF_LEN);
    memset(&Buf_GnssSplit[0],0,GNSS_PARSE_BUF_LEN);
}
/*----------------------------------------------------------------------------
��������:  Bsp_GNSS_Split_Data_Init
��������:  ����gnss��������
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
��������:  Bsp_GNSS_Init
��������:  GNSS������ʼ��
----------------------------------------------------------------------------*/
void Bsp_GNSS_Init(void)
{
    Bsp_GNSS_Parse_Init();
    Bsp_GNSS_Split_Data_Init();
}
/*----------------------------------------------------------------------
*  ��������:  Bsp_Gnss_RMC_Data_Split
*  ��������:  GNSS RMC���ݲ��
*  ����˵��:  ��
*  ����:      *ptData �õ������� *pu8Data ����һ������
*  ���:      SUCC->�ɹ�   FAIL->ʧ��
*  ����ֵ:    ��
������⣺$GPRMC,<0>,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>*hh
<0> UTC ʱ�䣬hhmmss(ʱ����)��ʽ
<1> ��λ״̬��A=��Ч��λ��V=��Ч��λ
<2> γ��ddmm.mmmm(�ȷ�)��ʽ(ǰ���0Ҳ��������)
<3> γ�Ȱ���N(������)��S(�ϰ���)
<4> ����dddmm.mmmm(�ȷ�)��ʽ(ǰ���0Ҳ��������)
<5> ���Ȱ���E(����)��W(����)
<6> ��������(000.0~999.9�ڣ�ǰ���0Ҳ��������)
<7> ���溽��(000.0~359.9�ȣ����汱Ϊ�ο���׼��ǰ���0Ҳ��������)
<8> UTC ���ڣ�ddmmyy(������)��ʽ
<9>��ƫ��(000.0~180.0�ȣ�ǰ���0Ҳ��������)
<10>��ƫ�Ƿ���E(��)��W(��)
<11>ģʽָʾ(��NMEA01833.00�汾�����A=������λ��D=��֣�E=���㣬N=������Ч)
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
      /*  UTC ʱ��  */
      case 0:
      {
        memcpy(tCharRMCdata.u8UtcTime,&pu8Data[u8Num],u8Numnext-u8Num);
        tRMCParseData.u32UtcTime = (uint32_t)atof((char*)tCharRMCdata.u8UtcTime);
      }
        break;
      /* ��λ״̬ */
      case 1:
      {
        memcpy(tCharRMCdata.u8GnssState,&pu8Data[u8Num],u8Numnext-u8Num);
        tRMCParseData.u8GnssState = tCharRMCdata.u8GnssState[0];
      }
        break;
      /* γ�� */
      case 2:
      {
        memcpy(tCharRMCdata.u8LatitudeValue,&pu8Data[u8Num],u8Numnext-u8Num);
        tRMCParseData.dLatitudeValue = atof((char*)tCharRMCdata.u8LatitudeValue);
      }
        break;
      /* γ�Ȱ��� */
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
       /* ���� */
      case 4:
      {
        memcpy(tCharRMCdata.u8LongtitudeValue,&pu8Data[u8Num],u8Numnext-u8Num);
        tRMCParseData.dLongtitudeValue = atof((char*)tCharRMCdata.u8LongtitudeValue);
      }
        break;
      /* ���Ȱ��� */
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
      /* ���� */
      case 6:
      {
        memcpy(tCharRMCdata.u8Speed,&pu8Data[u8Num],u8Numnext-u8Num);
        tRMCParseData.fSpeed = (float)atof((char*)tCharRMCdata.u8Speed);
      }
        break;
      /* ���� */
      case 7:
      {
        memcpy(tCharRMCdata.u8Heading,&pu8Data[u8Num],u8Numnext-u8Num);
        tRMCParseData.fHead = (float)atof((char*)tCharRMCdata.u8Heading);
      }
        break;
      /* UTC���� */
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
��������:  Bsp_Gnss_Loca_If_Vaild
��������:  GNSS��λ��Ϣ�Ƿ���Ч
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
��������:  Api_Location_Data_Turn
��������:  ��ת��
�������:  dLocation      ��λ
------------------------------------------------------------------------*/
double Api_Location_Data_Turn(double dLocation)
{
    return (double)((uint32_t)(dLocation*100000)%10000000)/100000/60+(double)((uint32_t)dLocation/100);
}
/*----------------------------------------------------------------------
*  ��������:  Api_Acc_Speed
*  ��������:  ���ٶȼ���
*  ����:      tDisA  ����A���ٶȣ�tDisb  ǰ��B���ٶ�
*  ���:      ���ٶȣ���/��2��
------------------------------------------------------------------------*/
uint16_t Api_Acc_Speed(GNSS_DATA_INF_T tDisA,GNSS_DATA_INF_T tDisb)
{
	return (uint16_t)(tDisA.u16Distance-tDisb.u16Distance);
}
/*----------------------------------------------------------------------
*  ��������:  Api_Loca_Distance
*  ��������:  ����������
*  ����:      tPointA  A�㾭γ�ȣ�tPointB  B�㾭γ��
*  ���:      ���루�ף�
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
��������:  GNSS_Data_Deal
��������:  GNSS���ݴ���
------------------------------------------------------------------------*/
void Bsp_GNSS_Data_Deal(void)
{
  GNSS_DATA_INF_T  tDataTemp = {0};
	if(SUCC == Bsp_Gnss_Loca_If_Vaild())  //������Ч
	{
		//���ݷֶ�ת��
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
			//��0����̽��λ
			case 0:
			{
				tGNSSdataTmp.dLongValue = tDataTemp.dLongValue;
				tGNSSdataTmp.dLatValue = tDataTemp.dLatValue;
				tGNSSdataTmp.u16Distance = 0;
				u16LocaTempNum++;
			}
				break;
			//��1����̽��λ
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
			//��2����̽��λ����0����Ч��λ
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
			//��1����Ч��λ
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
��������:  Bsp_GNSS_Data_Parse_String
��������:  GNSS�������ݽ���
�������:  *pu8Data    ����һ������
           u16Len      �������ݳ���
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
�������ƣ� Bsp_Gnss_Data_Check
���������� GNSS���ݼ������
��������� *pu8Data     GNSS����       
           length       GNSS���鳤��
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
��������:  Bsp_Gnss_Parse_Byte
��������:  ���ս���ÿ���ֽ�
�������:  u8Val    ��ǰ���ս����ֽ�����   
----------------------------------------------------------------------------*/
uint8_t Bsp_Gnss_Parse_Byte(uint8_t u8Val)    
{
  if(tGNSSRxCtrl.u8Step == 0)							                //���������0�ֽ�
  {
    if(u8Val != GNSS_START)						                    //�����ǰ�����ֽ����ݲ�����'$'
    {		
      tGNSSRxCtrl.u8Step = 0;						                  //��ǰ�����ֽڸ�λ0
      return FAIL;								                        //�˳��ҷ���ʧ��
    }
    else											                            //�������Э��ͷ0
    {
      tGNSSRxCtrl.u8Pkt[tGNSSRxCtrl.u8Step++] = u8Val;		//������1�ֽڣ���ǰ�ֽ����ݴ�������0
    }
  }
  else												
  {
    tGNSSRxCtrl.u8Pkt[tGNSSRxCtrl.u8Step++] = u8Val;				
    if(u8Val != GNSS_STOP)
    {
      if(tGNSSRxCtrl.u8Step > GNSS_MAX_BUF_LEN)
      {
        tGNSSRxCtrl.u8Step = 0;					                  //��ǰ�����ֽڸ�λ0
        return FAIL;							                        //�˳��ҷ���ʧ��
      }
    }
    else
    {
      Bsp_Gnss_Data_Check(tGNSSRxCtrl.u8Pkt, tGNSSRxCtrl.u8Step);
      tGNSSRxCtrl.u8Step = 0;						                  //��ǰ�����ֽڸ�λ0
      return SUCC;								                        //�˳��ҷ��سɹ�
    }
  }
  return SUCC;										                        //���سɹ�
}

/*----------------------------------------------------------------------------
��������:  Bsp_Get_Location
��������:  ��ö�λ��Ϣ
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

