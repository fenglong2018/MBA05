/************************************************************************
*  文件名称: uart_send.c
*  文件描述: 串口数据发送
*  历史记录:  
*  创建时间: 2023-04-28
*  创建人:   江昌钤
*  修改时间:
*  修改人:  
*  描述:
*************************************************************************/

/************************************************************************
头文件引用声明
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
NORMALSEND_CTRL_T          tSendCtrl;                       //正常和告警发送控制结构体 
uint8_t                 SendBuf[TXBUF_LEN];                 //发送数组
/************************************************************************
静态函数声明

extern 外部函数声明
*************************************************************************/

/************************************************************************
函数定义
*************************************************************************/
/*----------------------------------------------------------------------------
函数名称： API_Check_Xor
具体描述： 异或校验
输入参数： *data 校验的数据       
           length 校验长度
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
函数名称： API_Hex8_To_String
具体描述： 8位HEX转字符串
输入参数： hex 8位16进制数      *string 待转换字符串
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
函数名称： API_Funftoa
具体描述： 浮点数转字符串
输入参数： *pOut -> 字符串数组  fnum -> 浮点数  u8intcon -> 整数位数
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
	
	/*   取整数部分  */
	u32intNum=fnum;
	/*   取小数部分  */
	u32decnum=(uint32_t)((fnum-(double)u32intNum)*10000000);
	//整数部分
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
	//小数部分
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
函数名称： Bsp_Float_to_Buf
具体描述： 浮点数转字符串
输入参数： *pOut -> 字符串数组  fnum -> 浮点数
----------------------------------------------------------------------------*/
int Bsp_Float_to_Buf(uint8_t *pOut,double fnum)
{
    return sprintf((char *)pOut,"%0.1f",fnum);
}
/*----------------------------------------------------------------------------
函数名称： Bsp_Send_Get_CardID
具体描述： 获取北斗卡号
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
函数名称： Bsp_Add_Loca_Queue
具体描述： 添加定位点
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
    tSendCtrl.dLatValue = tLocaData.dLatValue;                        //保存到拆除发送定位里
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
    /* 消息头 */
    memcpy(&tSendCtrl.Buf_SendData[len],u8BufHead,strlen((const char*)u8BufHead));
    len += strlen((const char*)u8BufHead);
    /* 消息编号 */
    memcpy(&tSendCtrl.Buf_SendData[len],u8MsgNum,strlen((const char*)u8MsgNum));
    len += strlen((const char*)u8MsgNum);
    /* 设备ID */
    memcpy(&tSendCtrl.Buf_SendData[len], &u8ID,10);
    len += 10;
    tSendCtrl.Buf_SendData[len++] = ',';
    /* 发送方式 */
    memcpy(&tSendCtrl.Buf_SendData[len],u8SendType,strlen((const char*)u8SendType));
    len += strlen((const char*)u8SendType);
    /* 卡号 */
    memcpy(&tSendCtrl.Buf_SendData[len], &u8CardID,tSendCtrl.u8CardIDNum);
    len += tSendCtrl.u8CardIDNum;
    tSendCtrl.Buf_SendData[len++] = ',';
    /* 告警模式 */
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
    /* 电量 */
    memcpy(&tSendCtrl.Buf_SendData[len], &u8Temp[0],i16TempNum);
    len += i16TempNum;
    tSendCtrl.Buf_SendData[len++] = ',';
    /* 定位点数量 */
    tSendCtrl.Buf_SendData[len++] = '1';
    tSendCtrl.Buf_SendData[len++] = ',';
    /* 编号 */
    tSendCtrl.Buf_SendData[len++] = '1';
    tSendCtrl.Buf_SendData[len++] = ',';
    /* 时间戳 */
    memcpy(&tSendCtrl.Buf_SendData[len], &u8Time,10);
    len += 10;
    tSendCtrl.Buf_SendData[len++] = ',';
    /* 经度 */
		memcpy(&tSendCtrl.Buf_SendData[len], Longtitude,11);
		len += 11;
    tSendCtrl.Buf_SendData[len++] = ',';
		tSendCtrl.Buf_SendData[len++] = 'E';
    tSendCtrl.Buf_SendData[len++] = ',';
    /* 纬度 */
		memcpy(&tSendCtrl.Buf_SendData[len], Latitude,10);
		len += 10; 
    tSendCtrl.Buf_SendData[len++] = ',';
		tSendCtrl.Buf_SendData[len++] = 'N';
    tSendCtrl.Buf_SendData[len++] = ',';
    /* 航向 */
    memcpy(&tSendCtrl.Buf_SendData[len], &u8Head,i16HeadNum);
    len += i16HeadNum;
    tSendCtrl.Buf_SendData[len++] = ',';
    /* 航速 */
    memcpy(&tSendCtrl.Buf_SendData[len], &u8Speed,i16SpeedNum);
    len += i16SpeedNum;
    tSendCtrl.Buf_SendData[len++] = ',';
    /* 太阳能电压 */
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
      /* 消息头 */
      memcpy(&tSendCtrl.Buf_SendData[len],u8BufHead,strlen((const char*)u8BufHead));
      len += strlen((const char*)u8BufHead);
      /* 消息编号 */
      memcpy(&tSendCtrl.Buf_SendData[len],u8MsgNum,strlen((const char*)u8MsgNum));
      len += strlen((const char*)u8MsgNum);
      /* 设备ID */
      memcpy(&tSendCtrl.Buf_SendData[len], &u8ID,10);
      len += 10;
      tSendCtrl.Buf_SendData[len++] = ',';
      /* 发送方式 */
      memcpy(&tSendCtrl.Buf_SendData[len],u8SendType,strlen((const char*)u8SendType));
      len += strlen((const char*)u8SendType);
      /* 卡号 */
      memcpy(&tSendCtrl.Buf_SendData[len], &u8CardID,tSendCtrl.u8CardIDNum);
      len += tSendCtrl.u8CardIDNum;
      tSendCtrl.Buf_SendData[len++] = ',';
      /* 告警模式 */
      tSendCtrl.Buf_SendData[len++] = 'D';
      tSendCtrl.Buf_SendData[len++] = ',';
      /* 电量 */
      memcpy(&tSendCtrl.Buf_SendData[len], &u8Temp[0],i16TempNum);
      len += i16TempNum;
      tSendCtrl.Buf_SendData[len++] = ',';
      /* 定位点数量 */
      tSendCtrl.Buf_SendData[len++] = '1';
      tSendCtrl.Buf_SendData[len++] = ',';
      /* 编号 */
      tSendCtrl.Buf_SendData[len++] = '1';
      tSendCtrl.Buf_SendData[len++] = ',';
      /* 时间戳 */
      memcpy(&tSendCtrl.Buf_SendData[len], &u8Time,10);
      len += 10;
      tSendCtrl.Buf_SendData[len++] = ',';
      /* 经度 */
      memcpy(&tSendCtrl.Buf_SendData[len], Longtitude,11);
      len += 11;
      tSendCtrl.Buf_SendData[len++] = ',';
      tSendCtrl.Buf_SendData[len++] = 'E';
      tSendCtrl.Buf_SendData[len++] = ',';
      /* 纬度 */
      memcpy(&tSendCtrl.Buf_SendData[len], Latitude,10);
      len += 10; 
      tSendCtrl.Buf_SendData[len++] = ',';
      tSendCtrl.Buf_SendData[len++] = 'N';
      tSendCtrl.Buf_SendData[len++] = ',';
      /* 航向 */
      memcpy(&tSendCtrl.Buf_SendData[len], &u8Head,i16HeadNum);
      len += i16HeadNum;
      tSendCtrl.Buf_SendData[len++] = ',';
      /* 航速 */
      memcpy(&tSendCtrl.Buf_SendData[len], &u8Speed,i16SpeedNum);
      len += i16SpeedNum;
      tSendCtrl.Buf_SendData[len++] = ',';
      /* 太阳能电压 */
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
函数名称： Rdss_Tx_CCICR
具体描述： RDSS发送CCICR语句
----------------------------------------------------------------------------*/
void Bsp_Rdss_Tx_CCICR(void)
{
  uint8_t TxCCICR[] = "$CCICR,0,00*68\r\n";
  uart4_tx_string(&TxCCICR[0],16);    
}
/*----------------------------------------------------------------------------
函数名称： Rdss_Tx_CCTCQ
具体描述： RDSS发送CCTCQ语句
输入参数： pu8Param  发送数据数组  u16ParaLen  发送数据长度
----------------------------------------------------------------------------*/
int32_t Bsp_Rdss_Tx_CCTCQ(uint8_t *pu8Param, uint16_t u16ParaLen)
{
  uint16_t len = 0,chklen = 0;						            //长度
  uint8_t sum = 0;									                  //校验和
  uint8_t hextochk[2];
  uint8_t u8Type[] = RDSS_TYPE_CCTCQ;                 //"CCTCQ,"
  uint8_t u8RecvID[] = RDSS_RECVID;                   //
  uint8_t u8FrePoint[] = RDSS_FREPOINT;
  uint8_t u8SendFre[] = RDSS_SENDFRE;
  uint8_t u8InbApp[] = RDSS_INBAPP;
  uint8_t u8TxtType[] = RDSS_TXTTYPE;
  //如果发送解析结构体为空 或者 发送数组为空 或者 长度为0
  if(pu8Param == NULL || u16ParaLen == 0)
    return FAIL;									                    //返回失败
  memset(&SendBuf[0],0,TXBUF_LEN);
  //不包含$ + * + sum(2) + \r + \n
  chklen = u16ParaLen+23;                             
  //长度等于 发送的数据包长度 + $ + * + sum(2) + \r + \n   		
  SendBuf[len++] = RDSS_START;                        //封装协议头'$'			    
  /* CCTCQ */
  memcpy(&SendBuf[len],u8Type,strlen((const char*)u8Type));
  len+=strlen((const char*)u8Type);
  /* ID */
  memcpy(&SendBuf[len], u8RecvID, strlen((const char*)u8RecvID));
  len+= strlen((const char*)u8RecvID); 
  /* 频点 */    
  memcpy(&SendBuf[len], u8FrePoint, strlen((const char*)u8FrePoint));
  len+= strlen((const char*)u8FrePoint); 
  /* 申请确认 */
  memcpy(&SendBuf[len], u8InbApp, strlen((const char*)u8InbApp));
  len+= strlen((const char*)u8InbApp);
  /* 编码类型 */
  memcpy(&SendBuf[len], u8TxtType, strlen((const char*)u8TxtType));
  len+= strlen((const char*)u8TxtType);	
  /* 通信数据 */
  memcpy(&SendBuf[len], pu8Param, u16ParaLen);			
  len+=u16ParaLen;								
  SendBuf[len++] = ',';							    
  /* 通信频度 */
  memcpy(&SendBuf[len], u8SendFre, strlen((const char*)u8SendFre));
  len+= strlen((const char*)u8SendFre);	
  SendBuf[len++] = RDSS_CHK;
  sum = API_Check_Xor(&SendBuf[0],chklen);				    //检验和计算
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
  return SUCC;										//返回成功
}

/*----------------------------------------------------------------------------
*  函数名称:  Sos_Data_Send
*  功能描述:  SOS短报文数据发送
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
函数名称： Bsp_Into_Sleep
具体描述： 关闭其他设备后进入睡眠模式
----------------------------------------------------------------------------*/
void Bsp_Into_Sleep(uint16_t u16SleepTime)
{
    Bsp_RDSS_Init();
    Bsp_GNSS_Init();
    Hal_Get_Into_Sleep(u16SleepTime);
}
#if 1
/*----------------------------------------------------------------------------
函数名称： Bsp_Nor_SOS_SendCtrl
具体描述： 发送控制/1秒时基
----------------------------------------------------------------------------*/
uint8_t Bsp_Nor_SOS_SendCtrl(void)
{
  if((Bsp_Get_Sys_State() == STATE_NORMAL) || (Bsp_Get_Sys_State() == STATE_SOS))                          //确认处于正常发送模式
  {
    switch(tSendCtrl.u8SendState)
    {
      /*  正常和告警发送 开启 */
      case NORMAL_START:
      {
        #if   LOG
        printf("Normal Mode :Open RDSS and Get Into Step NORMAL_GETID\n");
        #endif
        //如果已经有北斗
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
      /*  获取ID */
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
      /*  开GNSS和电量检测 */
      case NORMAL_OPEN_GNSS:
      {
        #if   LOG
        printf("Normal Mode:Start Updata BatVol Percent,Open Gnss and Get Into Step NORMAL_OPEN_RDSS\n");
        #endif
        Bps_Start_Once_Time_Updata_BatVol_Percent();            //电量检测
        GNSS_POW_EN();                                          //开GNSS电源和串口
        Hal_Uart1_Init();
        tSendCtrl.u32Count = 0;
        tSendCtrl.u8SendState = NORMAL_OPEN_RDSS;
        break;
      }
      /*  开RDSS */
      case NORMAL_OPEN_RDSS:
      {
        if(tSendCtrl.u32Count++ >48)      //等待48秒后开启RDSS
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
      /*  第四步：添加定位 */
      case NORMAL_ADDLOCA:
      {
        if(tSendCtrl.u32Count++ <10)      //10秒内如果发送包准备完成，进入发送
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
        else                              //10秒后如果发送包未准备完成，进入发送
        {
          #if   LOG
          printf("Normal Mode:Add Loca Data Fail and Get Into Step NORMAL_SEND\n");
          #endif
          tSendCtrl.u32Count = 0;
          tSendCtrl.u8SendState = NORMAL_SEND;
        }
        break;
      }
      /*  发送 */
      case NORMAL_SEND:
      {
        tSendCtrl.u32Count++;
        //10-20秒，如果RD信号条件好发送
        if(tSendCtrl.u32Count > 10 && tSendCtrl.u32Count <= 20)
        {                                                                                                                                                                                                                                                                                
          if(Bsp_Rdss_Signal_Is_Ok())                                 //信号条件判断
          {
            #if   LOG
            printf("Normal Mode Step 6:Rdss Signal is Fine and Send Data,Get Into Step NORMAL_WAIT\n");
            #endif
            tSendCtrl.u32Count = 0;
            Bsp_BDS_Data_Send();
            tSendCtrl.u8SendState = NORMAL_WAIT;
          }
        }
        //如果大于20秒直接发送
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
      /*  等待下次发送 */
      case NORMAL_WAIT:
      {
        tSendCtrl.u32Count++;
        if(tSendCtrl.u32Count ==2)          //发送2秒后关闭RDSS
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
        if(Bsp_Get_Sys_State() == STATE_SOS)        //SOS模式
        {
          if(tSendCtrl.u32Count > SLEEP_TIME_SOS)   //SOS模式等待时间到
          {
            tSendCtrl.u32Count = 0;
            //Bps_Start_Once_Time_Updata_BatVol_Percent();            //电量检测
            tSendCtrl.u8SendState = NORMAL_OPEN_GNSS;
          }
        }
        else                                                        //正常模式
        {
          if(tSendCtrl.u32Count > SLEEP_TIME_NORMAL)                //正常模式等待时间到
          {
            tSendCtrl.u32Count = 0;
            //Bps_Start_Once_Time_Updata_BatVol_Percent();            //电量检测
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
函数名称： Bsp_Pull_SendCtrl
具体描述： 发送控制/1秒时基
----------------------------------------------------------------------------*/
uint8_t Bsp_Pull_SendCtrl(void)
{
  if((Bsp_Get_Sys_State() == STATE_PULL) || (Bsp_Get_Sys_State() == STATE_MOVE))                          //确认处于拆除发送模式
  {
    switch(tSendCtrl.u8SendState)
    {
      /*  发送 开启 */
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
      /*  第一步：获取ID */
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
      /*  第二步：开GNSS */
      case NORMAL_OPEN_GNSS:
      {
        #if   LOG
        printf("Pull Mode Step 3:Start Updata BatVol Percent,Open Gnss and Get Into Step NORMAL_OPEN_RDSS\n");
        #endif
        Bps_Start_Once_Time_Updata_BatVol_Percent();            //电量检测
        GNSS_POW_EN();                                          //开GNSS电源和串口
        Hal_Uart1_Init();
        tSendCtrl.u32Count = 0;
        tSendCtrl.u8SendState = NORMAL_OPEN_RDSS;
        break;
      }
      /*  第三步：开RDSS */
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
      /*  第四步：添加定位 */
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
      /*  第五步：发送 */
      case NORMAL_SEND:
      {
        tSendCtrl.u32Count++;
        //10-20秒，如果RD信号条件好发送
        if(tSendCtrl.u32Count < 20)
        {                                                                                                                                                                                                                                                                                
          if(Bsp_Rdss_Signal_Is_Ok())                                 //信号条件判断
          {
            #if   LOG
            printf("Pull Mode Step 6:Rdss Signal is Fine and Send Data,Get Into Step NORMAL_SLEEP\n");
            #endif
            tSendCtrl.u32Count = 0;
            Bsp_BDS_Data_Send();
            tSendCtrl.u8SendState = NORMAL_WAIT;
          }
        }
        //如果大于20秒直接发送                                                                                                                                                                                                                                                                   
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
        if(tSendCtrl.u32Count ==2)          //发送2秒后关闭RDSS
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
        if(tSendCtrl.u32Count > SLEEP_TIME_PULL)   //PULL模式等待时间到
        {
          tSendCtrl.u32Count = 0;
          //Bps_Start_Once_Time_Updata_BatVol_Percent();            //电量检测
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
函数名称： Bsp_Nor_SOS_SendCtrl
具体描述： 发送控制/1秒时基
----------------------------------------------------------------------------*/
uint8_t Bsp_Nor_SOS_SendCtrl(void)
{
  if((Bsp_Get_Sys_State() == STATE_NORMAL) || (Bsp_Get_Sys_State() == STATE_SOS))                          //确认处于正常发送模式
  {
    switch(tSendCtrl.u8SendState)
    {
      /*  发送 开启 */
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
      /*  第一步：获取ID */
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
      /*  第二步：开GNSS */
      case NORMAL_OPEN_GNSS:
      {
        #if   LOG
        printf("Normal Mode Step 3:Start Updata BatVol Percent,Open Gnss and Get Into Step 4\n");
        #endif
        Bps_Start_Once_Time_Updata_BatVol_Percent();            //电量检测
        GNSS_POW_EN();                                          //开GNSS电源和串口
        Hal_Uart1_Init();
        tSendCtrl.u32Count = 0;
        tSendCtrl.u8SendState = NORMAL_OPEN_RDSS;
        break;
      }
      /*  第三步：开RDSS */
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
      /*  第四步：添加定位 */
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
      /*  第五步：发送 */
      case NORMAL_SEND:
      {
        tSendCtrl.u32Count++;
        //10-20秒，如果RD信号条件好发送
        if(tSendCtrl.u32Count > 10 && tSendCtrl.u32Count <= 20)
        {                                                                                                                                                                                                                                                                                
          if(Bsp_Rdss_Signal_Is_Ok())                                 //信号条件判断
          {
            #if   LOG
            printf("Normal Mode Step 6:Rdss Signal is Fine and Send Data,Get Into Step 7\n");
            #endif
            tSendCtrl.u32Count = 0;
            Bsp_BDS_Data_Send();
            tSendCtrl.u8SendState = NORMAL_WAIT;
          }
        }
        //如果大于20秒直接发送
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
      /*  第五步：休眠 */
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
函数名称： Bsp_Pull_SendCtrl
具体描述： 发送控制/1秒时基
----------------------------------------------------------------------------*/
uint8_t Bsp_Pull_SendCtrl(void)
{
  if((Bsp_Get_Sys_State() == STATE_PULL) || (Bsp_Get_Sys_State() == STATE_MOVE))                          //确认处于拆除发送模式
  {
    switch(tSendCtrl.u8SendState)
    {
      /*  发送 开启 */
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
      /*  第一步：获取ID */
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
      /*  第二步：开GNSS */
      case NORMAL_OPEN_GNSS:
      {
        #if   LOG
        printf("Pull Mode Step 3:Start Updata BatVol Percent,Open Gnss and Get Into Step NORMAL_OPEN_RDSS\n");
        #endif
        Bps_Start_Once_Time_Updata_BatVol_Percent();            //电量检测
        GNSS_POW_EN();                                          //开GNSS电源和串口
        Hal_Uart1_Init();
        tSendCtrl.u32Count = 0;
        tSendCtrl.u8SendState = NORMAL_OPEN_RDSS;
        break;
      }
      /*  第三步：开RDSS */
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
      /*  第四步：添加定位 */
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
      /*  第五步：发送 */
      case NORMAL_SEND:
      {
        tSendCtrl.u32Count++;
        //10-20秒，如果RD信号条件好发送
        if(tSendCtrl.u32Count < 20)
        {                                                                                                                                                                                                                                                                                
          if(Bsp_Rdss_Signal_Is_Ok())                                 //信号条件判断
          {
            #if   LOG
            printf("Pull Mode Step 6:Rdss Signal is Fine and Send Data,Get Into Step NORMAL_SLEEP\n");
            #endif
            tSendCtrl.u32Count = 0;
            Bsp_BDS_Data_Send();
            tSendCtrl.u8SendState = NORMAL_WAIT;
          }
        }
        //如果大于20秒直接发送·                                                                                                                                                                                                                                                                     
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
      /*  第五步：休眠 */
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
          Bps_Start_Once_Time_Updata_BatVol_Percent();            //电量检测
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
函数名称： Bsp_Start_Normal_Mode
具体描述： 开始正常发送模式
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
函数名称： Bsp_Start_PULL_Mode
具体描述： 开始拆除发送模式
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
函数名称： Bsp_Start_PULL_Mode
具体描述： 开始拆除发送模式
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
函数名称： Bsp_Start_SOS_Mode
具体描述： 开始SOS发送模式
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
