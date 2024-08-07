/************************************************************************
*  �ļ�����:  Bsp_RdssParse.c
*  �ļ�����:  RDSS���ݽ���
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

#include    "api_common.h"

#include    "Bsp_RdssParse.h"
#include    "Bsp_RdssSend.h"
#include    "Bsp_Config.h"
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
const RDSS_TYPE_T  tRDSSType[RDSS_TYPE_NUM]=
                    {
                       RDSS_TYPE_BDPWI, TYPE_BDPWI_HEAD,        //BDPWI
                       RDSS_TYPE_BDICP, TYPE_BDICP_HEAD         //BDICP
                    };
RDSS_SIGNAL_T		tRdssSignal;
RDSS_RX_UNPACK_T    tRDSSRxCtrl;
uint8_t             Buf_RdssSplit[RDSS_PARSE_BUF_LEN];
/************************************************************************
��̬��������

extern �ⲿ��������
*************************************************************************/

/************************************************************************
��������
*************************************************************************/
/*----------------------------------------------------------------------------
�������ƣ� Rdss_Check_Xor
���������� ���У��
��������� *data У�������       
           length У�鳤��
----------------------------------------------------------------------------*/
uint8_t Rdss_Check_Xor(const void *data, uint16_t length)
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
�������ƣ� Bsp_RDSS_Parse_Init
���������� RDSS�������
----------------------------------------------------------------------------*/
void Bsp_RDSS_Parse_Init(void)
{
  tRDSSRxCtrl.u16BufLen = 0;
  tRDSSRxCtrl.u8Step = 0;
  memset(&tRDSSRxCtrl.u8Pkt[0],0,RDSS_PARSE_BUF_LEN);
  memset(&Buf_RdssSplit[0],0,RDSS_PARSE_BUF_LEN);
}
/*----------------------------------------------------------------------------
��������:  Bsp_RDSS_Split_Data_Init
��������:  ����RDSS��������
----------------------------------------------------------------------------*/
void Bsp_RDSS_Split_Data_Init(void)
{
  memset(&tRdssSignal,0,sizeof(RDSS_SIGNAL_T));
}
/*----------------------------------------------------------------------------
��������:  Bsp_GNSS_Init
��������:  GNSS������ʼ��
----------------------------------------------------------------------------*/
void Bsp_RDSS_Init(void)
{
  Bsp_RDSS_Parse_Init();
  Bsp_RDSS_Split_Data_Init();
}
/*----------------------------------------------------------------------------
��������:  Bsp_Rdss_Signal_Valid
��������:  RDSS�ź��Ƿ���Ч
----------------------------------------------------------------------------*/
uint8_t Bsp_Rdss_Signal_Valid(void)
{
	uint8_t i = 0;
  if((tRdssSignal.u16BeamNum > 1) && (tRdssSignal.dRDSSTime > 20))
  {
    for(i = 0; i<tRdssSignal.u16BeamNum;i++)
    {
      if(tRdssSignal.tWaveBeam[i].u8WES2C_d>45)
      {
        return SUCC;
      }
    }
    return FAIL;
  }
  else
  {
    return FAIL;
  }       
}
/*----------------------------------------------------------------------------
��������:  Bsp_Rdss_Signal_Is_Ok
��������:  RDSS�ź��Ƿ���Ч
----------------------------------------------------------------------------*/
uint8_t Bsp_Rdss_Signal_Is_Ok(void)
{
  if(tRdssSignal.u8IFSignedOK == SUCC)
      return 1;
  else
      return 0;
}
/*----------------------------------------------------------------------
*  ��������:  Bsp_Rdss_PWI_Data_Split
*  ��������:  RDSSPWI���ݲ��
*  ����˵��:  ��
*  ����:      *ptData �õ������� *pu8Data ����һ������
*  ���:      SUCC->�ɹ�   FAIL->ʧ��
*  ����ֵ:    ��
$BDPWI,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,M,<10>,M,<11>,<12>*xx<CR><LF>
<1> ʱ�䣬��ʽΪhhmmss.ss��
<2> δ֪����ʽΪ00��
<3> ������������ʽΪxx��
<4> ����1��ţ���ʽΪxx��
<5> ����1���S2C_d����ȣ���ʽΪxx��
<6> ����1���S2C_p����ȣ���ʽΪxx��
<7> ����������
<n*3+1> ����n��ţ���ʽΪxx��
<n*3+3> ����n���S2C_d����ȣ���ʽΪxx��
<n*3+3> ����n���S2C_p����ȣ���ʽΪxx��
------------------------------------------------------------------------*/
uint8_t Bsp_Rdss_PWI_Data_Split(uint8_t *pu8Data,uint16_t u16Len)
{
  uint8_t u8Num = 0,u8Numnext = 0;
  uint8_t u8Findp = 0;
  uint8_t u8datanum = 0;
	uint8_t u8buftemp[11];
	uint8_t u8bufnum = 0;
	uint8_t u8WBInfoCon = 0,u8WBCon = 0;
	memset(&u8buftemp[0],0,11);
  u8Numnext = RDSS_TYPE_SIZE;
  if(u16Len<u8Numnext)
    return FAIL;
  if(pu8Data[RDSS_TYPE_SIZE] != ',')
    return FAIL;
  u8Num = u8Numnext;
  for(u8datanum = 0;u8datanum <= 2;u8datanum++)
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
        memset(&tRdssSignal,0,sizeof(RDSS_SIGNAL_T));
          return FAIL; 
      }   
    }
    if((u8Numnext-u8Num)>11)
    {
      memset(&tRdssSignal,0,sizeof(RDSS_SIGNAL_T));
      return FAIL;
    }
    switch(u8datanum)
    {
      /*  ʱ��  */
      case 0:
      {
        memcpy(&u8buftemp[0], &pu8Data[u8Num],u8Numnext-u8Num);
        tRdssSignal.dRDSSTime = atof((char*)u8buftemp);
        memset(&u8buftemp[0],0,11);
      }
        break;
      /* ���� */
      case 2:
      {
        memcpy(u8buftemp, &pu8Data[u8Num],u8Numnext-u8Num);
        tRdssSignal.u16BeamNum = (uint8_t)atof((char*)u8buftemp);
        memset(&u8buftemp[0],0,11);
      }
        break;
      default:
      {
      }
        break;
    }
    u8Num = u8Numnext;
  }
  if(tRdssSignal.u16BeamNum > 10)
  {
    memset(&tRdssSignal,0,sizeof(RDSS_SIGNAL_T));
    return FAIL; 
  } 
  if(tRdssSignal.u16BeamNum)
  {
    u8bufnum = tRdssSignal.u16BeamNum * 3;
    for(u8datanum = 0;u8datanum < u8bufnum;u8datanum++)
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
          memset(&tRdssSignal,0,sizeof(RDSS_SIGNAL_T));
          return FAIL; 
        }    
      }
      if((u8Numnext-u8Num)>11)
      {
        memset(&tRdssSignal,0,sizeof(RDSS_SIGNAL_T));
        return FAIL;
      }
      switch(u8WBInfoCon++)
      {
        /*  ���  */
        case 0:
        {
          memcpy(u8buftemp, &pu8Data[u8Num],u8Numnext-u8Num);
          tRdssSignal.tWaveBeam[u8WBCon].u8WBSer = (uint8_t)atof((char*)u8buftemp);
          memset(&u8buftemp[0],0,11);
        }
          break;
        /* d����� */
        case 1:
        {
          memcpy(u8buftemp, &pu8Data[u8Num],u8Numnext-u8Num);
          tRdssSignal.tWaveBeam[u8WBCon].u8WES2C_d = (uint8_t)atof((char*)u8buftemp);
          memset(&u8buftemp[0],0,11);
        }
          break;
        /* p����� */
        case 2:
        {
          memcpy(u8buftemp, &pu8Data[u8Num],u8Numnext-u8Num);
          tRdssSignal.tWaveBeam[u8WBCon].u8WES2C_p = (uint8_t)atof((char*)u8buftemp);
          memset(&u8buftemp[0],0,11);	
          u8WBInfoCon = 0;
          u8WBCon++;						
        }
          break;
        default:
        {
        }
          break;
      }
      u8Num = u8Numnext;
    }
  }
  tRdssSignal.u8IFSignedOK = Bsp_Rdss_Signal_Valid();
  return SUCC;
}

/*----------------------------------------------------------------------
*  ��������:  rdss_icp_data_split
*  ��������:  RDSSICP���ݲ��
*  ����˵��:  ��
*  ����:      *ptData �õ������� *pu8Data ����һ������
*  ���:      SUCC->�ɹ�   FAIL->ʧ��
*  ����ֵ:    ��
$BDICP,<1>,����<n>*xx<CR><LF>
<1> ���ţ�
------------------------------------------------------------------------*/
uint8_t Bsp_Rdss_ICP_Data_Split(uint8_t *pu8Data,uint16_t u16Len)
{
  uint8_t u8Num = 0,u8Numnext = 0;
  uint8_t u8Findp = 0;
  uint8_t u8datanum = 0;
	uint8_t u8buftemp[11];
  uint32_t u32CardID = 0;
  uint8_t  u8CardIDNum = 0;
	memset(&u8buftemp[0],0,11);
  u8Numnext = RDSS_TYPE_SIZE;
  if(u16Len<u8Numnext)
    return FAIL;
  if(pu8Data[RDSS_TYPE_SIZE] != ',')
    return FAIL;
  u8Num = u8Numnext;

  for(u8datanum = 0;u8datanum <= 1;u8datanum++)
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
        u32CardID = 0;
        return FAIL;
      }   
    }
    switch(u8datanum)
    {
      /*  ����  */
      case 0:
      {
        if((u8Numnext-u8Num) > 9)
          return FAIL;
        memcpy(&u8buftemp[0], &pu8Data[u8Num],u8Numnext-u8Num);
        if((u8Numnext-u8Num) < 2)
        {
          u32CardID = 0;
        }
        else
        {
          u32CardID = convert_str_to_uint32(&u8buftemp[0],u8Numnext-u8Num);
          u8CardIDNum = u8Numnext-u8Num;
        }
        memset(&u8buftemp[0],0,11);
      }
        break;
      default:
      {

      }
        break;
    }
    u8Num = u8Numnext;
  }
  #if   LOG
  printf("RDSS Card ID: %d, Card Cont : %d\n",u32CardID,u8CardIDNum);
  #endif
  Bsp_Get_CardID(u32CardID);
  Bsp_Send_Get_CardID(u32CardID,u8CardIDNum);
  return SUCC;
}
/*----------------------------------------------------------------------------
��������:  Bsp_RDSS_Data_Parse_String
��������:  RDSS�������ݽ���
�������:  *pu8Data    ����һ������
           u16Len      �������ݳ���
----------------------------------------------------------------------------*/
void Bsp_RDSS_Data_Parse_String(uint8_t *pu8Data,uint16_t u16Len)
{
  uint8_t u8i,u8Type = 0;
  for(u8i = 1;u8i < RDSS_TYPE_NUM+1;u8i++)
  {
    if(memcmp(&pu8Data[0], tRDSSType[u8i-1].u8RDSSEName, RDSS_TYPE_SIZE) == SUCC)
    {
      u8Type = tRDSSType[u8i-1].u8TypeHead;
    }
  }
  switch(u8Type)
  {
    case TYPE_RDSS_NULL:
    {
        
    }
      break;
    case TYPE_BDPWI_HEAD:
    {
      #if   LOG
      printf("Receive RDSS PWI data :");
      Log_str_printf(pu8Data,u16Len);
      printf("RDSS PWI data Split\n");
      #endif
      Bsp_Rdss_PWI_Data_Split(pu8Data,u16Len);
    }
      break;
    case TYPE_BDICP_HEAD:
    {
      #if   LOG
      printf("Receive RDSS ICP data :");
      Log_str_printf(pu8Data,u16Len);
      printf("RDSS ICP data Split\n");
      #endif
      Bsp_Rdss_ICP_Data_Split(pu8Data,u16Len);
    }
      break;
    default:
    {
        
    }
      break;
  }
}

/*----------------------------------------------------------------------------
�������ƣ� Bsp_Rdss_Data_Check
���������� RDSS���ݼ������
��������� *pu8Data     GNSS����       
           length       GNSS���鳤��
----------------------------------------------------------------------------*/
void Bsp_Rdss_Data_Check(uint8_t *pu8Data, uint16_t u16Len)
{
  uint8_t rxchecksum[2] = {0,0};
	uint8_t u8RxCheckSum = 0;
	uint8_t u8CalCheckSum = 0;
  memset(&Buf_RdssSplit[0],0,RDSS_PARSE_BUF_LEN);
  memcpy(&Buf_RdssSplit[0],&pu8Data[0],u16Len);
  if(u16Len<10)
  {
    return;
  } 
  memcpy(&rxchecksum[0],&Buf_RdssSplit[u16Len-4],2);
  sscanf((char *)rxchecksum,"%hhx", &u8RxCheckSum);
  u8CalCheckSum = Rdss_Check_Xor(&Buf_RdssSplit[0],(u16Len-6));
  if(u8RxCheckSum != u8CalCheckSum)
  {
    Bsp_RDSS_Parse_Init();
    return;
  }
  Bsp_RDSS_Data_Parse_String(&Buf_RdssSplit[0],u16Len);
  Bsp_RDSS_Parse_Init();
}
/*----------------------------------------------------------------------------
��������:  Bsp_Rdss_Parse_Byte
��������:  ���ս���ÿ���ֽ�
�������:  u8Val    ��ǰ���ս����ֽ�����   
----------------------------------------------------------------------------*/
uint8_t Bsp_Rdss_Parse_Byte(uint8_t u8Val)    
{
  if(tRDSSRxCtrl.u8Step == 0)							                //���������0�ֽ�
  {
    if(u8Val != RDSS_START)						                    //�����ǰ�����ֽ����ݲ�����'$'
    {		
      tRDSSRxCtrl.u8Step = 0;						                  //��ǰ�����ֽڸ�λ0
      return FAIL;								                        //�˳��ҷ���ʧ��
    }
    else											                            //�������Э��ͷ0
    {
      tRDSSRxCtrl.u8Pkt[tRDSSRxCtrl.u8Step++] = u8Val;		//������1�ֽڣ���ǰ�ֽ����ݴ�������0
    }
  }
  else												
  {
    tRDSSRxCtrl.u8Pkt[tRDSSRxCtrl.u8Step++] = u8Val;				
    if(u8Val != RDSS_STOP)
    {
      if(tRDSSRxCtrl.u8Step > RDSS_MAX_BUF_LEN)
      {
          tRDSSRxCtrl.u8Step = 0;					                //��ǰ�����ֽڸ�λ0
          return FAIL;							                      //�˳��ҷ���ʧ��
      }
    }
    else
    {
      Bsp_Rdss_Data_Check(tRDSSRxCtrl.u8Pkt, tRDSSRxCtrl.u8Step);
      tRDSSRxCtrl.u8Step = 0;						                  //��ǰ�����ֽڸ�λ0
      return SUCC;								                        //�˳��ҷ��سɹ�
    }
  }
  return SUCC;										                        //���سɹ�
}


