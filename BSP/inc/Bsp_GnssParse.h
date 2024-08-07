/********************************************************************** 
*  �ļ�����:    Bsp_GnssParse.h
*  �ļ�����:    GNSS���ݽ���
*  ��ʷ��¼:  
*  ����ʱ��:    2023-04-25  
*  ������:      ������
*  �޸�ʱ��:
*  �޸���:  
*  ����: 	  
***********************************************************************/

#ifndef _BSP_GNSSPARSE_H_
#define _BSP_GNSSPARSE_H_

/********************************************************************** 
ͷ�ļ���������
***********************************************************************/
#include    <stdint.h>

/********************************************************************** 
�궨��
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

/*  GNSS������ʼ�� */
#define     GNSS_START              '$'
/*  GNSS����У��� */
#define     GNSS_CHK                '*'
/*  GNSS����ֹͣ�� */
#define     GNSS_STOP               '\n'
/*  GNSS���������� */
#define     GNSS_MAX_BUF_LEN        100
/*  GNSS���������鳤�� */
#define     GNSS_PARSE_BUF_LEN      200
/********************************************************************** 
�ṹ��ö�١�������Ƚṹ����
***********************************************************************/
typedef struct
{
    uint8_t     u8Pkt[GNSS_PARSE_BUF_LEN];      /* �������� */
    uint16_t    u16BufLen;                      /* �������泤�� */
    uint8_t     u8Step;
}GNSS_RX_UNPACK_T;

typedef struct
{
	uint8_t	u8GNSSEName[10];
	uint8_t	u8TypeHead;
}GNSS_TYPE_T;

typedef struct
{
	uint16_t	  u16Distance;	  //���ϴξ���
  double      dLongValue;     //����
  double      dLatValue;      //γ��
  uint32_t    u32Time;        //ʱ��
  uint32_t    u32Date;        //����
  float       fSpeed;         //����
  float       fHead;          //����
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
  uint8_t     u8PaeseOK;                      // ����OK
	uint8_t	    u8LocaOK;						            // ��ǰ��λ�Ƿ���Ч
	uint32_t	  u32UtcTime;			                // UTCʱ��
  uint32_t    u32UtcDate;                     // UTC����
  uint8_t	    u8GnssState;					          // �ڽ��յ���Ч����ǰ�����λ�ǡ�V������������ݶ�Ϊ�գ��ӵ���Ч���ݺ����λ�ǡ�A��������ſ�ʼ������
	double	    dLatitudeValue;				          // γ��
	uint8_t	    u8Latitude;						          // γ�Ȱ���
	double	    dLongtitudeValue;			          // ����
	uint8_t	    u8Longitude;					          // ���Ȱ���	
  float       fSpeed;                         // ����
  float       fHead;                          // ������
}RMC_DATA_T;

typedef struct  
{
	uint8_t u8UtcTime[11];		                  //UTCʱ��
  uint8_t u8UtcDate[7];                       //UTC����
  uint8_t u8GnssState[2];                     //��λ״̬V/A
	uint8_t u8LatitudeValue[10];		            //γ��
	uint8_t u8Latitude[2];		                  //γ�Ȱ���N/S
	uint8_t u8LongtitudeValue[11];		          //����
	uint8_t u8Longitude[2];		                  //���Ȱ���	E/W
  uint8_t u8Speed[10];                        //����
  uint8_t u8Heading[8];                       //������
}RMC_DATA_CHAR_T;
/********************************************************************** 
Դ�ļ������������

extern �ⲿ��������
***********************************************************************/


/********************************************************************** 
Դ�ļ���������

extern �ⲿ��������
***********************************************************************/
/*----------------------------------------------------------------------------
��������:  Bsp_GNSS_Init
��������:  GNSS������ʼ��
----------------------------------------------------------------------------*/
void Bsp_GNSS_Init(void);
/*----------------------------------------------------------------------------
��������:  Bsp_Gnss_Parse_Byte
��������:  ���ս���ÿ���ֽ�
�������:  u8Val    ��ǰ���ս����ֽ�����   
----------------------------------------------------------------------------*/
uint8_t Bsp_Gnss_Parse_Byte(uint8_t u8Val);
/*----------------------------------------------------------------------------
��������:  Bsp_Get_Location
��������:  ��ö�λ��Ϣ
----------------------------------------------------------------------------*/
uint8_t Bsp_Get_Location(GNSS_DATA_INF_T *pLocaData);
/*----------------------------------------------------------------------
��������:  Bsp_Gnss_Loca_If_Vaild
��������:  GNSS��λ��Ϣ�Ƿ���Ч
------------------------------------------------------------------------*/
uint8_t Bsp_Gnss_Loca_If_Vaild(void);
#endif
