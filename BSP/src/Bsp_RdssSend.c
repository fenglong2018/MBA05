/************************************************************************
*  �ļ�����: uart_send.c
*  �ļ�����: �������ݷ���
*  ��ʷ��¼:  
*  ����ʱ��: 2023-04-28
*  ������:   ������
*  �޸�ʱ��:
*  �޸���:  
*  ����:
*************************************************************************/

/************************************************************************
ͷ�ļ���������
*************************************************************************/
#include    <stdlib.h>
#include    <string.h>
#include    <stdio.h>

#include    "at32f435_437_gpio.h"

#include    "api_fifo.h"
#include    "api_common.h"

#include    "Hal_Uart.h"
#include    "Hal_PowManage.h"
#include    "Hal_Sleep.h"

#include    "Bsp_RdssSend.h"
#include    "Bsp_GnssParse.h"
#include    "Bsp_RdssParse.h"
#include    "Bsp_Config.h"
#include    "Bsp_BatVol.h"
#include    "Bsp_KeyLed.h"
#include    "Bsp_Log.h"
/************************************************************************
�궨��
*************************************************************************/

/************************************************************************
�ڲ��ṹ��ö�١�������Ƚṹ����
*************************************************************************/

/************************************************************************
�ڲ���̬��������

ȫ�ֱ�������

extern �ⲿ��������

*************************************************************************/
NORMALSEND_CTRL_T          tSendCtrl;                       //�����͸澯���Ϳ��ƽṹ�� 
uint8_t                 SendBuf[TXBUF_LEN];                 //��������
/************************************************************************
��̬��������

extern �ⲿ��������
*************************************************************************/

/************************************************************************
��������
*************************************************************************/
/*----------------------------------------------------------------------------
�������ƣ� API_Check_Xor
���������� ���У��
��������� *data У�������       
           length У�鳤��
----------------------------------------------------------------------------*/
uint8_t API_Check_Xor(const void *data, uint16_t length)
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
�������ƣ� API_Hex8_To_String
���������� 8λHEXת�ַ���
��������� hex 8λ16������      *string ��ת���ַ���
----------------------------------------------------------------------------*/
void API_Hex8_To_String(uint8_t hex,uint8_t *string)
{
  uint8_t u8Hex[2];
  uint8_t u8i;
  u8Hex[1] = LO_UINT8(hex);
  u8Hex[0] = HI_UINT8(hex);
  for(u8i = 0;u8i < 2;u8i++)
  {
    if(u8Hex[u8i] < 10)
    {
      string[u8i] = u8Hex[u8i] + 48;
    }
    else
    {
      string[u8i] = u8Hex[u8i] + 55;
    }
  }
}
/*----------------------------------------------------------------------------
�������ƣ� API_Funftoa
���������� ������ת�ַ���
��������� *pOut -> �ַ�������  fnum -> ������  u8intcon -> ����λ��
----------------------------------------------------------------------------*/
uint8_t API_Funftoa(uint8_t *pOut,double fnum,uint8_t u8intcon)
{
	uint32_t u32intNum,u32decnum;
	uint8_t k = 0;
	if(pOut==NULL)
	{
		return FAIL;
	}
  if(fnum<0)
    return FAIL;
	if(u8intcon == 2)
	{
		if(fnum>90)
		{
			return FAIL;
		}
	}
	else if(u8intcon == 3)
	{
		if(fnum>180)
		{
			return FAIL;
		}
	}
	else
		return FAIL;
	memset(pOut,0,u8intcon+8);
	
	/*   ȡ��������  */
	u32intNum=fnum;
	/*   ȡС������  */
	u32decnum=(uint32_t)((fnum-(double)u32intNum)*10000000);
	//��������
	if(u8intcon == 2)
	{
		pOut[0] = (u32intNum/10)+'0';
		pOut[1] = u32intNum%10+'0';
		pOut[2] = '.';
	}
	if(u8intcon == 3)
	{
		pOut[0] = u32intNum/100+'0';
		pOut[1] = (u32intNum%100)/10+'0';
		pOut[2] = u32intNum%10+'0';
		pOut[3] = '.';
	}
	//С������
	if(u8intcon == 2)
	{
		for(k = 9;k>2;k--)
		{
			pOut[k] = u32decnum%10+'0';
			u32decnum = u32decnum/10;
		}
	}
	if(u8intcon == 3)
	{
		for(k = 10;k>3;k--)
		{
			pOut[k] = u32decnum%10+'0';
			u32decnum = u32decnum/10;
		}
	}
	return SUCC;
}
/*----------------------------------------------------------------------------
�������ƣ� Bsp_Float_to_Buf
���������� ������ת�ַ���
��������� *pOut -> �ַ�������  fnum -> ������
----------------------------------------------------------------------------*/
int Bsp_Float_to_Buf(uint8_t *pOut,double fnum)
{
    return sprintf((char *)pOut,"%0.1f",fnum);
}
/*----------------------------------------------------------------------------
�������ƣ� Bsp_Send_Get_CardID
���������� ��ȡ��������
----------------------------------------------------------------------------*/
void Bsp_Send_Get_CardID(uint32_t u32InCardID,uint8_t u8InIDNum)
{
  if(tSendCtrl.u8IFGetCard)
  {
    if(u32InCardID)
    {
      tSendCtrl.u8IFGetCard = 0;
      tSendCtrl.u32CardID =u32InCardID;
      tSendCtrl.u8CardIDNum = u8InIDNum;
    }
  }
}

/*----------------------------------------------------------------------------
�������ƣ� Bsp_Add_Loca_Queue
���������� ��Ӷ�λ��
----------------------------------------------------------------------------*/
uint8_t Bsp_Add_Loca_Queue(void)
{
	GNSS_DATA_INF_T tLocaData;
  uint32_t    Timestamp = 0;
  uint8_t     u8GetPercent = 0;
  float       fSloarVol = 0;
  double       fTemp = 0;
  uint8_t     Longtitude[11],Latitude[11];
	uint8_t		  u8Time[10];
  uint8_t     u8ID[10];
  uint8_t     u8CardID[10];
  uint8_t     u8Percent[2];
  uint8_t     u8Speed[10];
  uint8_t     u8Head[10];
  uint8_t     u8Solat[10];
  uint8_t     u8Temp[10];
  int         i16SpeedNum;
  int         i16HeadNum;
  int         i16TempNum;
  uint8_t     u8BufHead[] = RDSS_TYPE_MBA05;
  uint8_t     u8MsgNum[] = RDSS_MSG_NUM;
  uint8_t     u8SendType[] = RDSS_SEND_TYPE;
  uint8_t     len = 0;
  memset(&Longtitude[0],0,11);
  memset(&Latitude[0],0,11);
	memset(&u8Time[0],0,10);
  memset(&u8ID,0,10);
  memset(&u8CardID,0,10);
  memset(&u8Percent,0,2);
  memset(&u8Speed,0,10);
  memset(&u8Head,0,10);
  memset(&u8Solat,0,10);
  memset(&u8Temp,0,10);
  memset(&tSendCtrl.Buf_SendData[0],0,SENDBUFLEN);
  memset(&tSendCtrl.Buf_SendStr[0],0,SENDCHARBUFLEN);
  
  tSendCtrl.u8BufLen = 0;
  u8GetPercent = Bsp_Get_BatPercent();
  fSloarVol = Bsp_Get_SloarVolFloat();
  fTemp = Bsp_Get_Temp();
  if(SUCC == Bsp_Get_Location(&tLocaData))
	{
    tSendCtrl.dLatValue = tLocaData.dLatValue;                        //���浽������Ͷ�λ��
    tSendCtrl.dLongValue = tLocaData.dLongValue;
    tSendCtrl.u32Time = tLocaData.u32Time;
    tSendCtrl.u32Date = tLocaData.u32Date;
		if(FAIL == API_Funftoa(&Longtitude[0],tLocaData.dLongValue,3))
      return FAIL;
		if(FAIL == API_Funftoa(&Latitude[0],tLocaData.dLatValue,2))
      return FAIL;
    Timestamp = TimeTurn(tLocaData.u32Time,tLocaData.u32Date);
    i16SpeedNum = Bsp_Float_to_Buf(&u8Speed[0],tLocaData.fSpeed);
    i16HeadNum = Bsp_Float_to_Buf(&u8Head[0],tLocaData.fHead);
    if(fTemp>0)
    {
      i16TempNum = Bsp_Float_to_Buf(&u8Temp[0],fTemp);
      }
    Bsp_Float_to_Buf(&u8Solat[0],fSloarVol);
    if(!Timestamp)
      return FAIL;
    if(i16SpeedNum<0)
      return FAIL;
    if(i16HeadNum<0)
      return FAIL;
		if(FAIL == Fundtoa(&u8Time[0],Timestamp,10))
      return FAIL;
    if(FAIL == Fundtoa(&u8ID[0],tSendCtrl.u32ID,10))
      return FAIL;
    if(FAIL == Fundtoa(&u8CardID[0],tSendCtrl.u32CardID,tSendCtrl.u8CardIDNum))
      return FAIL;
    if(FAIL == Fundtoa(&u8Percent[0],(uint32_t)u8GetPercent,2))
      return FAIL;
    if(tLocaData.dLongValue == 0 || tLocaData.dLatValue == 0)
      return FAIL;
    /* ��Ϣͷ */
    memcpy(&tSendCtrl.Buf_SendData[len],u8BufHead,strlen((const char*)u8BufHead));
    len += strlen((const char*)u8BufHead);
    /* ��Ϣ��� */
    memcpy(&tSendCtrl.Buf_SendData[len],u8MsgNum,strlen((const char*)u8MsgNum));
    len += strlen((const char*)u8MsgNum);
    /* �豸ID */
    memcpy(&tSendCtrl.Buf_SendData[len], &u8ID,10);
    len += 10;
    tSendCtrl.Buf_SendData[len++] = ',';
    /* ���ͷ�ʽ */
    memcpy(&tSendCtrl.Buf_SendData[len],u8SendType,strlen((const char*)u8SendType));
    len += strlen((const char*)u8SendType);
    /* ���� */
    memcpy(&tSendCtrl.Buf_SendData[len], &u8CardID,tSendCtrl.u8CardIDNum);
    len += tSendCtrl.u8CardIDNum;
    tSendCtrl.Buf_SendData[len++] = ',';
    /* �澯ģʽ */
    if(Bsp_Get_Sys_State() == STATE_SOS)
    {
      tSendCtrl.Buf_SendData[len++] = 'C';
      tSendCtrl.Buf_SendData[len++] = ',';
    }
    else if(Bsp_Get_Sys_State() == STATE_PULL || Bsp_Get_Sys_State() == STATE_MOVE)
    {
      tSendCtrl.Buf_SendData[len++] = 'D';
      tSendCtrl.Buf_SendData[len++] = ',';
    }
    else
    {
      tSendCtrl.Buf_SendData[len++] = 'A';
      tSendCtrl.Buf_SendData[len++] = ',';
    }
    /* ���� */
    memcpy(&tSendCtrl.Buf_SendData[len], &u8Temp[0],i16TempNum);
    len += i16TempNum;
    tSendCtrl.Buf_SendData[len++] = ',';
    /* ��λ������ */
    tSendCtrl.Buf_SendData[len++] = '1';
    tSendCtrl.Buf_SendData[len++] = ',';
    /* ��� */
    tSendCtrl.Buf_SendData[len++] = '1';
    tSendCtrl.Buf_SendData[len++] = ',';
    /* ʱ��� */
    memcpy(&tSendCtrl.Buf_SendData[len], &u8Time,10);
    len += 10;
    tSendCtrl.Buf_SendData[len++] = ',';
    /* ���� */
		memcpy(&tSendCtrl.Buf_SendData[len], Longtitude,11);
		len += 11;
    tSendCtrl.Buf_SendData[len++] = ',';
		tSendCtrl.Buf_SendData[len++] = 'E';
    tSendCtrl.Buf_SendData[len++] = ',';
    /* γ�� */
		memcpy(&tSendCtrl.Buf_SendData[len], Latitude,10);
		len += 10; 
    tSendCtrl.Buf_SendData[len++] = ',';
		tSendCtrl.Buf_SendData[len++] = 'N';
    tSendCtrl.Buf_SendData[len++] = ',';
    /* ���� */
    memcpy(&tSendCtrl.Buf_SendData[len], &u8Head,i16HeadNum);
    len += i16HeadNum;
    tSendCtrl.Buf_SendData[len++] = ',';
    /* ���� */
    memcpy(&tSendCtrl.Buf_SendData[len], &u8Speed,i16SpeedNum);
    len += i16SpeedNum;
    tSendCtrl.Buf_SendData[len++] = ',';
    /* ̫���ܵ�ѹ */
    memcpy(&tSendCtrl.Buf_SendData[len], &u8Solat,3);
    len += 3;
    if(len < 120)
    {
      tSendCtrl.u8BufLen = len;
      return SUCC; 
    } 
    else
      return FAIL; 
	}
  else
  {
    if((Bsp_Get_Sys_State() == STATE_PULL) || (Bsp_Get_Sys_State() == STATE_MOVE))
    {
      API_Funftoa(&Longtitude[0],tSendCtrl.dLongValue,3);
      API_Funftoa(&Latitude[0],tSendCtrl.dLatValue,2);
      Timestamp = TimeTurn(tSendCtrl.u32Time,tSendCtrl.u32Date);
      i16SpeedNum = Bsp_Float_to_Buf(&u8Speed[0],0);
      i16HeadNum = Bsp_Float_to_Buf(&u8Head[0],0);
      i16TempNum = Bsp_Float_to_Buf(&u8Temp[0],fTemp);
      Bsp_Float_to_Buf(&u8Solat[0],fSloarVol);
      if(i16SpeedNum<0)
        i16SpeedNum = 0;
      if(i16HeadNum<0)
        i16HeadNum = 0;
      if(FAIL == Fundtoa(&u8Time[0],Timestamp,10))
        memset(&u8Time[0],0,10);
      if(FAIL == Fundtoa(&u8ID[0],tSendCtrl.u32ID,10))
        memset(&u8ID[0],0,10);
      Fundtoa(&u8CardID[0],tSendCtrl.u32CardID,tSendCtrl.u8CardIDNum);
      Fundtoa(&u8Percent[0],(uint32_t)u8GetPercent,2);
      /* ��Ϣͷ */
      memcpy(&tSendCtrl.Buf_SendData[len],u8BufHead,strlen((const char*)u8BufHead));
      len += strlen((const char*)u8BufHead);
      /* ��Ϣ��� */
      memcpy(&tSendCtrl.Buf_SendData[len],u8MsgNum,strlen((const char*)u8MsgNum));
      len += strlen((const char*)u8MsgNum);
      /* �豸ID */
      memcpy(&tSendCtrl.Buf_SendData[len], &u8ID,10);
      len += 10;
      tSendCtrl.Buf_SendData[len++] = ',';
      /* ���ͷ�ʽ */
      memcpy(&tSendCtrl.Buf_SendData[len],u8SendType,strlen((const char*)u8SendType));
      len += strlen((const char*)u8SendType);
      /* ���� */
      memcpy(&tSendCtrl.Buf_SendData[len], &u8CardID,tSendCtrl.u8CardIDNum);
      len += tSendCtrl.u8CardIDNum;
      tSendCtrl.Buf_SendData[len++] = ',';
      /* �澯ģʽ */
      tSendCtrl.Buf_SendData[len++] = 'D';
      tSendCtrl.Buf_SendData[len++] = ',';
      /* ���� */
      memcpy(&tSendCtrl.Buf_SendData[len], &u8Temp[0],i16TempNum);
      len += i16TempNum;
      tSendCtrl.Buf_SendData[len++] = ',';
      /* ��λ������ */
      tSendCtrl.Buf_SendData[len++] = '1';
      tSendCtrl.Buf_SendData[len++] = ',';
      /* ��� */
      tSendCtrl.Buf_SendData[len++] = '1';
      tSendCtrl.Buf_SendData[len++] = ',';
      /* ʱ��� */
      memcpy(&tSendCtrl.Buf_SendData[len], &u8Time,10);
      len += 10;
      tSendCtrl.Buf_SendData[len++] = ',';
      /* ���� */
      memcpy(&tSendCtrl.Buf_SendData[len], Longtitude,11);
      len += 11;
      tSendCtrl.Buf_SendData[len++] = ',';
      tSendCtrl.Buf_SendData[len++] = 'E';
      tSendCtrl.Buf_SendData[len++] = ',';
      /* γ�� */
      memcpy(&tSendCtrl.Buf_SendData[len], Latitude,10);
      len += 10; 
      tSendCtrl.Buf_SendData[len++] = ',';
      tSendCtrl.Buf_SendData[len++] = 'N';
      tSendCtrl.Buf_SendData[len++] = ',';
      /* ���� */
      memcpy(&tSendCtrl.Buf_SendData[len], &u8Head,i16HeadNum);
      len += i16HeadNum;
      tSendCtrl.Buf_SendData[len++] = ',';
      /* ���� */
      memcpy(&tSendCtrl.Buf_SendData[len], &u8Speed,i16SpeedNum);
      len += i16SpeedNum;
      tSendCtrl.Buf_SendData[len++] = ',';
      /* ̫���ܵ�ѹ */
      memcpy(&tSendCtrl.Buf_SendData[len], &u8Solat,3);
      len += 3;
      tSendCtrl.u8BufLen = len;
      return SUCC;
    }
    else
      return FAIL;
  }
}
/*----------------------------------------------------------------------------
�������ƣ� Rdss_Tx_CCICR
���������� RDSS����CCICR���
----------------------------------------------------------------------------*/
void Bsp_Rdss_Tx_CCICR(void)
{
  uint8_t TxCCICR[] = "$CCICR,0,00*68\r\n";
  uart4_tx_string(&TxCCICR[0],16);    
}
/*----------------------------------------------------------------------------
�������ƣ� Rdss_Tx_CCTCQ
���������� RDSS����CCTCQ���
��������� pu8Param  ������������  u16ParaLen  �������ݳ���
----------------------------------------------------------------------------*/
int32_t Bsp_Rdss_Tx_CCTCQ(uint8_t *pu8Param, uint16_t u16ParaLen)
{
  uint16_t len = 0,chklen = 0;						            //����
  uint8_t sum = 0;									                  //У���
  uint8_t hextochk[2];
  uint8_t u8Type[] = RDSS_TYPE_CCTCQ;                 //"CCTCQ,"
  uint8_t u8RecvID[] = RDSS_RECVID;                   //
  uint8_t u8FrePoint[] = RDSS_FREPOINT;
  uint8_t u8SendFre[] = RDSS_SENDFRE;
  uint8_t u8InbApp[] = RDSS_INBAPP;
  uint8_t u8TxtType[] = RDSS_TXTTYPE;
  //������ͽ����ṹ��Ϊ�� ���� ��������Ϊ�� ���� ����Ϊ0
  if(pu8Param == NULL || u16ParaLen == 0)
    return FAIL;									                    //����ʧ��
  memset(&SendBuf[0],0,TXBUF_LEN);
  //������$ + * + sum(2) + \r + \n
  chklen = u16ParaLen+23;                             
  //���ȵ��� ���͵����ݰ����� + $ + * + sum(2) + \r + \n   		
  SendBuf[len++] = RDSS_START;                        //��װЭ��ͷ'$'			    
  /* CCTCQ */
  memcpy(&SendBuf[len],u8Type,strlen((const char*)u8Type));
  len+=strlen((const char*)u8Type);
  /* ID */
  memcpy(&SendBuf[len], u8RecvID, strlen((const char*)u8RecvID));
  len+= strlen((const char*)u8RecvID); 
  /* Ƶ�� */    
  memcpy(&SendBuf[len], u8FrePoint, strlen((const char*)u8FrePoint));
  len+= strlen((const char*)u8FrePoint); 
  /* ����ȷ�� */
  memcpy(&SendBuf[len], u8InbApp, strlen((const char*)u8InbApp));
  len+= strlen((const char*)u8InbApp);
  /* �������� */
  memcpy(&SendBuf[len], u8TxtType, strlen((const char*)u8TxtType));
  len+= strlen((const char*)u8TxtType);	
  /* ͨ������ */
  memcpy(&SendBuf[len], pu8Param, u16ParaLen);			
  len+=u16ParaLen;								
  SendBuf[len++] = ',';							    
  /* ͨ��Ƶ�� */
  memcpy(&SendBuf[len], u8SendFre, strlen((const char*)u8SendFre));
  len+= strlen((const char*)u8SendFre);	
  SendBuf[len++] = RDSS_CHK;
  sum = API_Check_Xor(&SendBuf[0],chklen);				    //����ͼ���
  API_Hex8_To_String(sum,&hextochk[0]);
  memcpy(&SendBuf[len], hextochk, 2);			       
  len+=2;
  SendBuf[len++] = RDSS_CR;
  SendBuf[len++] = RDSS_LF;
  uart4_tx_string(&SendBuf[0],len);  
  #if   LOG
  printf("BDS Send :");
  Log_str_printf(SendBuf,len);
  #endif  
  return SUCC;										//���سɹ�
}

/*----------------------------------------------------------------------------
*  ��������:  Sos_Data_Send
*  ��������:  SOS�̱������ݷ���
----------------------------------------------------------------------------*/
uint8_t Bsp_BDS_Data_Send(void)
{
  if(!tSendCtrl.u8BufLen)
    return FAIL;
  #if   LOG
  printf("BDS Send :");
  Log_str_printf(tSendCtrl.Buf_SendData,tSendCtrl.u8BufLen);
  #endif  
  convert_str_to_hex(&tSendCtrl.Buf_SendData[0],tSendCtrl.u8BufLen,&tSendCtrl.Buf_SendStr[0],tSendCtrl.u8BufLen*2);
  Bsp_Rdss_Tx_CCTCQ(&tSendCtrl.Buf_SendStr[0],tSendCtrl.u8BufLen*2);
  return SUCC;
}

/*----------------------------------------------------------------------------
�������ƣ� Bsp_Into_Sleep
���������� �ر������豸�����˯��ģʽ
----------------------------------------------------------------------------*/
void Bsp_Into_Sleep(uint16_t u16SleepTime)
{
    Bsp_RDSS_Init();
    Bsp_GNSS_Init();
    Hal_Get_Into_Sleep(u16SleepTime);
}
#if 1
/*----------------------------------------------------------------------------
�������ƣ� Bsp_Nor_SOS_SendCtrl
���������� ���Ϳ���/1��ʱ��
----------------------------------------------------------------------------*/
uint8_t Bsp_Nor_SOS_SendCtrl(void)
{
  if((Bsp_Get_Sys_State() == STATE_NORMAL) || (Bsp_Get_Sys_State() == STATE_SOS))                          //ȷ�ϴ�����������ģʽ
  {
    switch(tSendCtrl.u8SendState)
    {
      /*  �����͸澯���� ���� */
      case NORMAL_START:
      {
        #if   LOG
        printf("Normal Mode :Open RDSS and Get Into Step NORMAL_GETID\n");
        #endif
        //����Ѿ��б���
        tSendCtrl.u32Count = 0;
        if((tSendCtrl.u32CardID == 0) || (tSendCtrl.u32ID != Bsp_Read_ID()))
        {
          tSendCtrl.u8IFGetCard = 1;
          tSendCtrl.u32ID = Bsp_Read_ID();
          RDSS_POW_EN();
          Hal_Uart4_Init();
          tSendCtrl.u8SendState = NORMAL_GETID;
        }
        else
        {
          Hal_Uart4_Deinit(); 
          RDSS_POW_DIS();
          tSendCtrl.u8SendState = NORMAL_OPEN_GNSS;
        }
        break;
      }
      /*  ��ȡID */
      case NORMAL_GETID:
      {
        if(tSendCtrl.u8IFGetCard)
        {
          Bsp_Rdss_Tx_CCICR();
          if(tSendCtrl.u32Count++ > 10)
          {
            #if   LOG
            printf("Normal Mode:Get Card ID Fail,Close RDSS Pow and Get Into Step NORMAL_OPEN_GNSS\n");
            #endif
            tSendCtrl.u32Count = 0;
            tSendCtrl.u8IFGetCard = 0;
            Hal_Uart4_Deinit(); 
            RDSS_POW_DIS();
            tSendCtrl.u8SendState = NORMAL_OPEN_GNSS;
          }
        }
        else
        {
          #if   LOG
          printf("Normal Mode:Get Card ID Succ and Into Step NORMAL_OPEN_GNSS\n");
          #endif
          tSendCtrl.u32Count = 0;
          Hal_Uart4_Deinit(); 
          RDSS_POW_DIS();
          tSendCtrl.u8SendState = NORMAL_OPEN_GNSS;
        }
        break;
      }
      /*  ��GNSS�͵������ */
      case NORMAL_OPEN_GNSS:
      {
        #if   LOG
        printf("Normal Mode:Start Updata BatVol Percent,Open Gnss and Get Into Step NORMAL_OPEN_RDSS\n");
        #endif
        Bps_Start_Once_Time_Updata_BatVol_Percent();            //�������
        GNSS_POW_EN();                                          //��GNSS��Դ�ʹ���
        Hal_Uart1_Init();
        tSendCtrl.u32Count = 0;
        tSendCtrl.u8SendState = NORMAL_OPEN_RDSS;
        break;
      }
      /*  ��RDSS */
      case NORMAL_OPEN_RDSS:
      {
        if(tSendCtrl.u32Count++ >48)      //�ȴ�48�����RDSS
        {
          #if   LOG
          printf("Normal Mode:Open Rdss and Get Into Step NORMAL_ADDLOCA\n");
          #endif
          tSendCtrl.u32Count = 0;
          RDSS_POW_EN();
          Hal_Uart4_Init();
          tSendCtrl.u8SendState = NORMAL_ADDLOCA;
        }
          break;
        }
      /*  ���Ĳ�����Ӷ�λ */
      case NORMAL_ADDLOCA:
      {
        if(tSendCtrl.u32Count++ <10)      //10����������Ͱ�׼����ɣ����뷢��
        {   
          if(SUCC == Bsp_Add_Loca_Queue())
          {
            #if   LOG
            printf("Normal Mode:Add Loca Data Succ and Get Into Step 6\n");
            #endif
            tSendCtrl.u32Count = 0;
            tSendCtrl.u8SendState = NORMAL_SEND;
          }
        }
        else                              //10���������Ͱ�δ׼����ɣ����뷢��
        {
          #if   LOG
          printf("Normal Mode:Add Loca Data Fail and Get Into Step NORMAL_SEND\n");
          #endif
          tSendCtrl.u32Count = 0;
          tSendCtrl.u8SendState = NORMAL_SEND;
        }
        break;
      }
      /*  ���� */
      case NORMAL_SEND:
      {
        tSendCtrl.u32Count++;
        //10-20�룬���RD�ź������÷���
        if(tSendCtrl.u32Count > 10 && tSendCtrl.u32Count <= 20)
        {                                                                                                                                                                                                                                                                                
          if(Bsp_Rdss_Signal_Is_Ok())                                 //�ź������ж�
          {
            #if   LOG
            printf("Normal Mode Step 6:Rdss Signal is Fine and Send Data,Get Into Step NORMAL_WAIT\n");
            #endif
            tSendCtrl.u32Count = 0;
            Bsp_BDS_Data_Send();
            tSendCtrl.u8SendState = NORMAL_WAIT;
          }
        }
        //�������20��ֱ�ӷ���
        else if(tSendCtrl.u32Count > 20)
        {
          #if   LOG
          printf("Normal Mode:Rdss Signal is Bad and Send Data,Get Into Step NORMAL_WAIT\n");
          #endif
          tSendCtrl.u32Count = 0;
          Bsp_BDS_Data_Send();
          tSendCtrl.u8SendState = NORMAL_WAIT;
        }
        break;
      }
      /*  �ȴ��´η��� */
      case NORMAL_WAIT:
      {
        tSendCtrl.u32Count++;
        if(tSendCtrl.u32Count ==2)          //����2���ر�RDSS
        {
          #if   LOG
          RTC_Printf();
          printf("Close Rdss\n");
          #endif
          Hal_Uart4_Deinit(); 
          Hal_Uart1_Deinit(); 
          RDSS_POW_DIS();
          GNSS_POW_DIS();
        }
        if(Bsp_Get_Sys_State() == STATE_SOS)        //SOSģʽ
        {
          if(tSendCtrl.u32Count > SLEEP_TIME_SOS)   //SOSģʽ�ȴ�ʱ�䵽
          {
            tSendCtrl.u32Count = 0;
            //Bps_Start_Once_Time_Updata_BatVol_Percent();            //�������
            tSendCtrl.u8SendState = NORMAL_OPEN_GNSS;
          }
        }
        else                                                        //����ģʽ
        {
          if(tSendCtrl.u32Count > SLEEP_TIME_NORMAL)                //����ģʽ�ȴ�ʱ�䵽
          {
            tSendCtrl.u32Count = 0;
            //Bps_Start_Once_Time_Updata_BatVol_Percent();            //�������
            tSendCtrl.u8SendState = NORMAL_OPEN_GNSS;
          }
        }
        break;
      }
      default:
        break;
    }
  }
  return SUCC;
}
/*----------------------------------------------------------------------------
�������ƣ� Bsp_Pull_SendCtrl
���������� ���Ϳ���/1��ʱ��
----------------------------------------------------------------------------*/
uint8_t Bsp_Pull_SendCtrl(void)
{
  if((Bsp_Get_Sys_State() == STATE_PULL) || (Bsp_Get_Sys_State() == STATE_MOVE))                          //ȷ�ϴ��ڲ������ģʽ
  {
    switch(tSendCtrl.u8SendState)
    {
      /*  ���� ���� */
      case NORMAL_START:
      {
        #if   LOG
        printf("Pull Mode Step 1:Open RDSS and Get Into Step NORMAL_GETID\n");
        #endif
        tSendCtrl.u32Count = 0;
        if((tSendCtrl.u32CardID == 0) || (tSendCtrl.u32ID != Bsp_Read_ID()))
        {
          tSendCtrl.u32ID = Bsp_Read_ID();
          RDSS_POW_EN();
          Hal_Uart4_Init();
          tSendCtrl.u8IFGetCard = 1;
          tSendCtrl.u8SendState = NORMAL_GETID;
        }
        else
        {
          #if   LOG
          printf("Pull Mode Step 1:Get Into Step NORMAL_OPEN_GNSS\n");
          #endif
          tSendCtrl.u8IFGetCard = 0;
          tSendCtrl.u8SendState = NORMAL_OPEN_GNSS;
        }
        break;
      }
      /*  ��һ������ȡID */
      case NORMAL_GETID:
      {
        if(tSendCtrl.u8IFGetCard)
        {
          Bsp_Rdss_Tx_CCICR();
          if(tSendCtrl.u32Count++ > 10)
          {
            #if   LOG
            printf("Pull Mode Step 2:Get Card ID Fail,Close RDSS Pow and Get Into Step NORMAL_OPEN_GNSS\n");
            #endif
            tSendCtrl.u32Count = 0;
            tSendCtrl.u8IFGetCard = 0;
            Hal_Uart4_Deinit(); 
            RDSS_POW_DIS();
            tSendCtrl.u8SendState = NORMAL_OPEN_GNSS;
          }
        }
        else
        {
          #if   LOG
          printf("Pull Mode Step 2:Get Card ID Succ and Into Step NORMAL_OPEN_GNSS\n");
          #endif
          tSendCtrl.u32Count = 0;
          Hal_Uart4_Deinit(); 
          RDSS_POW_DIS();
          tSendCtrl.u8SendState = NORMAL_OPEN_GNSS;
        }
        break;
      }
      /*  �ڶ�������GNSS */
      case NORMAL_OPEN_GNSS:
      {
        #if   LOG
        printf("Pull Mode Step 3:Start Updata BatVol Percent,Open Gnss and Get Into Step NORMAL_OPEN_RDSS\n");
        #endif
        Bps_Start_Once_Time_Updata_BatVol_Percent();            //�������
        GNSS_POW_EN();                                          //��GNSS��Դ�ʹ���
        Hal_Uart1_Init();
        tSendCtrl.u32Count = 0;
        tSendCtrl.u8SendState = NORMAL_OPEN_RDSS;
        break;
      }
      /*  ����������RDSS */
      case NORMAL_OPEN_RDSS:
      {
          #if   LOG
          printf("Pull Mode Step 4:Open Rdss and Get Into Step NORMAL_ADDLOCA\n");
          #endif
          tSendCtrl.u32Count = 0;
          RDSS_POW_EN();
          Hal_Uart4_Init();
          tSendCtrl.u8SendState = NORMAL_ADDLOCA;
          break;
      }
      /*  ���Ĳ�����Ӷ�λ */
      case NORMAL_ADDLOCA:
      {
        if(SUCC == Bsp_Add_Loca_Queue())
        {
          #if   LOG
          printf("Pull Mode Step 5:Add Loca Data Succ and Get Into Step NORMAL_SEND\n");
          #endif
          tSendCtrl.u32Count = 0;
          tSendCtrl.u8SendState = NORMAL_SEND;
        }
        else
        {
          #if   LOG
          printf("Pull Mode Step 5:Add Loca Data Fail and Get Into Step NORMAL_SEND\n");
          #endif
          tSendCtrl.u32Count = 0;
          tSendCtrl.u8SendState = NORMAL_SEND;
        }
        break;
      }
      /*  ���岽������ */
      case NORMAL_SEND:
      {
        tSendCtrl.u32Count++;
        //10-20�룬���RD�ź������÷���
        if(tSendCtrl.u32Count < 20)
        {                                                                                                                                                                                                                                                                                
          if(Bsp_Rdss_Signal_Is_Ok())                                 //�ź������ж�
          {
            #if   LOG
            printf("Pull Mode Step 6:Rdss Signal is Fine and Send Data,Get Into Step NORMAL_SLEEP\n");
            #endif
            tSendCtrl.u32Count = 0;
            Bsp_BDS_Data_Send();
            tSendCtrl.u8SendState = NORMAL_WAIT;
          }
        }
        //�������20��ֱ�ӷ���                                                                                                                                                                                                                                                                   
        else
        {
          #if   LOG
          printf("Pull Mode Step 6:Rdss Signal is Bad and Send Data,Get Into Step NORMAL_SLEEP\n");
          #endif
          tSendCtrl.u32Count = 0;
          Bsp_BDS_Data_Send();
          tSendCtrl.u8SendState = NORMAL_WAIT;
        }
        break;
      }
      case NORMAL_WAIT:
      {
        tSendCtrl.u32Count++;
        if(tSendCtrl.u32Count ==2)          //����2���ر�RDSS
        {
          #if   LOG
          RTC_Printf();
          printf("Close Rdss\n");
          #endif
          Hal_Uart4_Deinit(); 
          Hal_Uart1_Deinit(); 
          RDSS_POW_DIS();
          GNSS_POW_DIS();
        }
        if(tSendCtrl.u32Count > SLEEP_TIME_PULL)   //PULLģʽ�ȴ�ʱ�䵽
        {
          tSendCtrl.u32Count = 0;
          //Bps_Start_Once_Time_Updata_BatVol_Percent();            //�������
          tSendCtrl.u8SendState = NORMAL_OPEN_GNSS;
        }
        break;
      }
    }
  }
  return SUCC;
}
#else
/*----------------------------------------------------------------------------
�������ƣ� Bsp_Nor_SOS_SendCtrl
���������� ���Ϳ���/1��ʱ��
----------------------------------------------------------------------------*/
uint8_t Bsp_Nor_SOS_SendCtrl(void)
{
  if((Bsp_Get_Sys_State() == STATE_NORMAL) || (Bsp_Get_Sys_State() == STATE_SOS))                          //ȷ�ϴ�����������ģʽ
  {
    switch(tSendCtrl.u8SendState)
    {
      /*  ���� ���� */
      case NORMAL_START:
      {
        #if   LOG
        printf("Normal Mode Step 1:Open RDSS and Get Into Step 2\n");
        #endif
        if(tSendCtrl.u32CardID == 0)
        {
          tSendCtrl.u8IFGetCard = 1;
          tSendCtrl.u32Count = 0;
          tSendCtrl.u32ID = Bsp_Read_ID();
          RDSS_POW_EN();
          Hal_Uart4_Init();
          tSendCtrl.u8SendState = NORMAL_GETID;
        }
        else
        {
          Hal_Uart4_Deinit(); 
          RDSS_POW_DIS();
          tSendCtrl.u8SendState = NORMAL_OPEN_GNSS;
        }
        break;
      }
      /*  ��һ������ȡID */
      case NORMAL_GETID:
      {
        if(tSendCtrl.u8IFGetCard)
        {
          Bsp_Rdss_Tx_CCICR();
          if(tSendCtrl.u32Count++ > 10)
          {
            #if   LOG
            printf("Normal Mode Step 2:Get Card ID Fail,Close RDSS Pow and Get Into Step 3\n");
            #endif
            tSendCtrl.u32Count = 0;
            tSendCtrl.u8IFGetCard = 0;
            Hal_Uart4_Deinit(); 
            RDSS_POW_DIS();
            tSendCtrl.u8SendState = NORMAL_OPEN_GNSS;
          }
        }
        else
        {
          #if   LOG
          printf("Normal Mode Step 2:Get Card ID Succ and Into Step 3\n");
          #endif
          tSendCtrl.u32Count = 0;
          Hal_Uart4_Deinit(); 
          RDSS_POW_DIS();
          tSendCtrl.u8SendState = NORMAL_OPEN_GNSS;
        }
        break;
      }
      /*  �ڶ�������GNSS */
      case NORMAL_OPEN_GNSS:
      {
        #if   LOG
        printf("Normal Mode Step 3:Start Updata BatVol Percent,Open Gnss and Get Into Step 4\n");
        #endif
        Bps_Start_Once_Time_Updata_BatVol_Percent();            //�������
        GNSS_POW_EN();                                          //��GNSS��Դ�ʹ���
        Hal_Uart1_Init();
        tSendCtrl.u32Count = 0;
        tSendCtrl.u8SendState = NORMAL_OPEN_RDSS;
        break;
      }
      /*  ����������RDSS */
      case NORMAL_OPEN_RDSS:
      {
        if(tSendCtrl.u32Count++ >48)
        {
          #if   LOG
          printf("Normal Mode Step 4:Open Rdss and Get Into Step 5\n");
          #endif
          tSendCtrl.u32Count = 0;
          RDSS_POW_EN();
          Hal_Uart4_Init();
          tSendCtrl.u8SendState = NORMAL_ADDLOCA;
        }
          break;
        }
      /*  ���Ĳ�����Ӷ�λ */
      case NORMAL_ADDLOCA:
      {
        if(tSendCtrl.u32Count++ <10)
        {   
          if(SUCC == Bsp_Add_Loca_Queue())
          {
            #if   LOG
            printf("Normal Mode Step 5:Add Loca Data Succ and Get Into Step 6\n");
            #endif
            tSendCtrl.u32Count = 0;
            tSendCtrl.u8SendState = NORMAL_SEND;
          }
        }
        else
        {
          #if   LOG
          printf("Normal Mode Step 5:Add Loca Data Fail and Get Into Step 6\n");
          #endif
          tSendCtrl.u32Count = 0;
          tSendCtrl.u8SendState = NORMAL_SEND;
        }
        break;
      }
      /*  ���岽������ */
      case NORMAL_SEND:
      {
        tSendCtrl.u32Count++;
        //10-20�룬���RD�ź������÷���
        if(tSendCtrl.u32Count > 10 && tSendCtrl.u32Count <= 20)
        {                                                                                                                                                                                                                                                                                
          if(Bsp_Rdss_Signal_Is_Ok())                                 //�ź������ж�
          {
            #if   LOG
            printf("Normal Mode Step 6:Rdss Signal is Fine and Send Data,Get Into Step 7\n");
            #endif
            tSendCtrl.u32Count = 0;
            Bsp_BDS_Data_Send();
            tSendCtrl.u8SendState = NORMAL_WAIT;
          }
        }
        //�������20��ֱ�ӷ���
        else if(tSendCtrl.u32Count > 20)
        {
          #if   LOG
          printf("Normal Mode Step 6:Rdss Signal is Bad and Send Data,Get Into Step 7\n");
          #endif
          tSendCtrl.u32Count = 0;
          Bsp_BDS_Data_Send();
          tSendCtrl.u8SendState = NORMAL_WAIT;
        }
        break;
      }
      /*  ���岽������ */
      case NORMAL_WAIT:
      {
        if(tSendCtrl.u32Count++ >2)
        {
          #if   LOG
          RTC_Printf();
          printf("Normal Mode Step 7:Get Into Sleep\n");
          #endif
          tSendCtrl.u32Count = 0;
          Hal_Uart1_Deinit(); 
          Hal_Uart4_Deinit();
          GNSS_POW_DIS();
          RDSS_POW_DIS();
          tSendCtrl.u8SendState = NORMAL_OPEN_GNSS;
          if(Bsp_Get_Sys_State() == STATE_NORMAL)
            Bsp_Into_Sleep(SLEEP_TIME_NORMAL);
          else
            Bsp_Into_Sleep(SLEEP_TIME_SOS);
        }
          break;
      }
      default:
        break;
    }
  }
  return SUCC;
}
/*----------------------------------------------------------------------------
�������ƣ� Bsp_Pull_SendCtrl
���������� ���Ϳ���/1��ʱ��
----------------------------------------------------------------------------*/
uint8_t Bsp_Pull_SendCtrl(void)
{
  if((Bsp_Get_Sys_State() == STATE_PULL) || (Bsp_Get_Sys_State() == STATE_MOVE))                          //ȷ�ϴ��ڲ������ģʽ
  {
    switch(tSendCtrl.u8SendState)
    {
      /*  ���� ���� */
      case NORMAL_START:
      {
        #if   LOG
        printf("Pull Mode Step 1:Open RDSS and Get Into Step NORMAL_GETID\n");
        #endif
        tSendCtrl.u32Count = 0;
        if((tSendCtrl.u32CardID == 0) || (tSendCtrl.u32ID != Bsp_Read_ID()))
        {
          tSendCtrl.u32ID = Bsp_Read_ID();
          RDSS_POW_EN();
          Hal_Uart4_Init();
          tSendCtrl.u8IFGetCard = 1;
          tSendCtrl.u8SendState = NORMAL_GETID;
        }
        else
        {
          #if   LOG
          printf("Pull Mode Step 1:Get Into Step NORMAL_OPEN_GNSS\n");
          #endif
          tSendCtrl.u8IFGetCard = 0;
          tSendCtrl.u8SendState = NORMAL_OPEN_GNSS;
        }
        break;
      }
      /*  ��һ������ȡID */
      case NORMAL_GETID:
      {
        if(tSendCtrl.u8IFGetCard)
        {
          Bsp_Rdss_Tx_CCICR();
          if(tSendCtrl.u32Count++ > 10)
          {
            #if   LOG
            printf("Pull Mode Step 2:Get Card ID Fail,Close RDSS Pow and Get Into Step NORMAL_OPEN_GNSS\n");
            #endif
            tSendCtrl.u32Count = 0;
            tSendCtrl.u8IFGetCard = 0;
            Hal_Uart4_Deinit(); 
            RDSS_POW_DIS();
            tSendCtrl.u8SendState = NORMAL_OPEN_GNSS;
          }
        }
        else
        {
          #if   LOG
          printf("Pull Mode Step 2:Get Card ID Succ and Into Step NORMAL_OPEN_GNSS\n");
          #endif
          tSendCtrl.u32Count = 0;
          Hal_Uart4_Deinit(); 
          RDSS_POW_DIS();
          tSendCtrl.u8SendState = NORMAL_OPEN_GNSS;
        }
        break;
      }
      /*  �ڶ�������GNSS */
      case NORMAL_OPEN_GNSS:
      {
        #if   LOG
        printf("Pull Mode Step 3:Start Updata BatVol Percent,Open Gnss and Get Into Step NORMAL_OPEN_RDSS\n");
        #endif
        Bps_Start_Once_Time_Updata_BatVol_Percent();            //�������
        GNSS_POW_EN();                                          //��GNSS��Դ�ʹ���
        Hal_Uart1_Init();
        tSendCtrl.u32Count = 0;
        tSendCtrl.u8SendState = NORMAL_OPEN_RDSS;
        break;
      }
      /*  ����������RDSS */
      case NORMAL_OPEN_RDSS:
      {
          #if   LOG
          printf("Pull Mode Step 4:Open Rdss and Get Into Step NORMAL_ADDLOCA\n");
          #endif
          tSendCtrl.u32Count = 0;
          RDSS_POW_EN();
          Hal_Uart4_Init();
          tSendCtrl.u8SendState = NORMAL_ADDLOCA;
          break;
      }
      /*  ���Ĳ�����Ӷ�λ */
      case NORMAL_ADDLOCA:
      {
        if(SUCC == Bsp_Add_Loca_Queue())
        {
          #if   LOG
          printf("Pull Mode Step 5:Add Loca Data Succ and Get Into Step NORMAL_SEND\n");
          #endif
          tSendCtrl.u32Count = 0;
          tSendCtrl.u8SendState = NORMAL_SEND;
        }
        else
        {
          #if   LOG
          printf("Pull Mode Step 5:Add Loca Data Fail and Get Into Step NORMAL_SEND\n");
          #endif
          tSendCtrl.u32Count = 0;
          tSendCtrl.u8SendState = NORMAL_SEND;
        }
        break;
      }
      /*  ���岽������ */
      case NORMAL_SEND:
      {
        tSendCtrl.u32Count++;
        //10-20�룬���RD�ź������÷���
        if(tSendCtrl.u32Count < 20)
        {                                                                                                                                                                                                                                                                                
          if(Bsp_Rdss_Signal_Is_Ok())                                 //�ź������ж�
          {
            #if   LOG
            printf("Pull Mode Step 6:Rdss Signal is Fine and Send Data,Get Into Step NORMAL_SLEEP\n");
            #endif
            tSendCtrl.u32Count = 0;
            Bsp_BDS_Data_Send();
            tSendCtrl.u8SendState = NORMAL_WAIT;
          }
        }
        //�������20��ֱ�ӷ��͡�                                                                                                                                                                                                                                                                     
        else
        {
          #if   LOG
          printf("Pull Mode Step 6:Rdss Signal is Bad and Send Data,Get Into Step NORMAL_SLEEP\n");
          #endif
          tSendCtrl.u32Count = 0;
          Bsp_BDS_Data_Send();
          tSendCtrl.u8SendState = NORMAL_WAIT;
        }
        break;
      }
      /*  ���岽������ */
      case NORMAL_WAIT:
      {
        tSendCtrl.u32Count++;
        if(tSendCtrl.u32Count == 2)
        {
          #if   LOG
          RTC_Printf();
          printf("Pull Mode Step 7:Close Rdss and Get Into Delay\n");
          #endif
          Hal_Uart4_Deinit(); 
          RDSS_POW_DIS();
        }
        else if(tSendCtrl.u32Count == 100)
        {
          #if   LOG
          RTC_Printf();
          printf("Pull Mode Step 7:BatVol Dete\n");
          #endif
          Bps_Start_Once_Time_Updata_BatVol_Percent();            //�������
        }
        else if(tSendCtrl.u32Count == 110)
        {
          #if   LOG
          RTC_Printf();
          printf("Pull Mode Step 7:Get Into Step NORMAL_OPEN_RDSS\n");
          #endif
          tSendCtrl.u32Count = 0;
          tSendCtrl.u8SendState = NORMAL_OPEN_RDSS;
        }
          break;
      }
      default:
        break;
    }
  }
  return SUCC;
}
#endif
/*----------------------------------------------------------------------------
�������ƣ� Bsp_Start_Normal_Mode
���������� ��ʼ��������ģʽ
----------------------------------------------------------------------------*/
void Bsp_Start_Normal_Mode(void)
{
    tSendCtrl.u8SendState = NORMAL_START;
    Bsp_RDSS_Init();
    Bsp_GNSS_Init();
    Hal_Uart1_Deinit(); 
    Hal_Uart4_Deinit();
    GNSS_POW_DIS();
    RDSS_POW_DIS();
}

/*----------------------------------------------------------------------------
�������ƣ� Bsp_Start_PULL_Mode
���������� ��ʼ�������ģʽ
----------------------------------------------------------------------------*/
void Bsp_Start_PULL_Mode(void)
{
    tSendCtrl.u8SendState = NORMAL_START;
    Bsp_RDSS_Init();
    Bsp_GNSS_Init();
    Hal_Uart1_Deinit(); 
    Hal_Uart4_Deinit();
    GNSS_POW_DIS();
    RDSS_POW_DIS();
}
/*----------------------------------------------------------------------------
�������ƣ� Bsp_Start_PULL_Mode
���������� ��ʼ�������ģʽ
----------------------------------------------------------------------------*/
void Bsp_Start_Move_Mode(void)
{
    tSendCtrl.u8SendState = NORMAL_START;
    Bsp_RDSS_Init();
    Bsp_GNSS_Init();
    Hal_Uart1_Deinit(); 
    Hal_Uart4_Deinit();
    GNSS_POW_DIS();
    RDSS_POW_DIS();
}
/*----------------------------------------------------------------------------
�������ƣ� Bsp_Start_SOS_Mode
���������� ��ʼSOS����ģʽ
----------------------------------------------------------------------------*/
void Bsp_Start_SOS_Mode(void)
{
    tSendCtrl.u8SendState = NORMAL_START;
    Bsp_RDSS_Init();
    Bsp_GNSS_Init();
    Hal_Uart1_Deinit(); 
    Hal_Uart4_Deinit();
    GNSS_POW_DIS();
    RDSS_POW_DIS();
}
