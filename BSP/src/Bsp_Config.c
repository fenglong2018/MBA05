/*************************************************************************
*  	�ļ�����:  	Bsp_Config.c
*  	�ļ�����:  	����Э���շ��ͽ�����ID��д��FLASH��ת
*  	��ʷ��¼:
*  	����ʱ��:  	2024-05-10
*  	������:  	������
*  	�޸�ʱ��:
*	�޸���:
*   ����:
*************************************************************************/

/*************************************************************************
ͷ�ļ���������
*************************************************************************/
#include    <string.h>
#include    <stdio.h>

#include    "at32f435_437_gpio.h"

#include    "Hal_Flash.h"
#include    "Hal_Uart.h"
#include    "Hal_PowManage.h"

#include    "Bsp_Config.h"
#include    "Bsp_RdssSend.h"
#include    "Bsp_KeyLed.h"
#include    "Bsp_BatVol.h"
#include    "Bsp_Log.h"
/************************************************************************
�궨��
*************************************************************************/
/* ȡ16λ�еĸ�8λ */
#define HI_UINT16(a) 				(((a) >> 8) & 0xFF)
/* ȡ16λ�еĵ�8λ */
#define LO_UINT16(a) 				((a) & 0xFF)
/* ��32λ����ȡһ�ֽ� : var - ��ȡuint32��,  ByteNum - �� (0 - 3)�ֽ� */
#define BREAK_UINT32( var, ByteNum ) \
          							(uint8_t)((uint32_t)(((var) >>((ByteNum) * 8)) & 0x00FF))
                                    /* 4�ֽںϲ���uint32�� : Byte0~Byte3 ��Ӧ ��λ��ʼ��0-3�ֽ�*/
#define BUILD_UINT32(Byte3, Byte2, Byte1, Byte0) \
          							((uint32_t)((uint32_t)((Byte0) & 0x00FF) \
							          + ((uint32_t)((Byte1) & 0x00FF) << 8) \
							          + ((uint32_t)((Byte2) & 0x00FF) << 16) \
							          + ((uint32_t)((Byte3) & 0x00FF) << 24)))
/*************************************************************************
�ڲ��ṹ��ö�١�������Ƚṹ����
*************************************************************************/

/*************************************************************************
�ڲ���̬��������

ȫ�ֱ�������

extern �ⲿ��������

*************************************************************************/

/*************************************************************************
��̬��������

extern �ⲿ��������
*************************************************************************/
/* Э����սṹ�� */
PORT_RX_UNPACK_T tPortRxCtrl;

/* һ����Э�鷢�ͻ������� */
uint8_t u8TxPORTBuf[PORT_TX_LEN] = {0};
//--------------------------------�豸��Ϣ��ѯ������������������������������//
INFOR_GET_T         tInforGet = {0};

uint8_t             u8UartMode = 0;
/*************************************************************************
��������
*************************************************************************/
/*----------------------------------------------------------------------------
�������ƣ� Port_Checksum
���������� �����ۼӼ����
��������� buf ���� buflen ���ݳ���
��������� У���
----------------------------------------------------------------------------*/
uint16_t Port_Checksum(uint8_t *buf, uint8_t buflen)
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
�������ƣ� Port_Tx_Cmd_Pack
���������� ��װЭ���뷢��
��������� u8Cmd ����  txbuf ��������   u8buflen ���ݳ���
----------------------------------------------------------------------------*/
void Port_Tx_Cmd_Pack(uint8_t u8Cmd,uint8_t *txbuf,uint8_t u8buflen)
{
    uint8_t u8length = 0;
    uint16_t u16CheckSum;
    u8length = u8buflen + 4;
    memset(&u8TxPORTBuf[0],0,PORT_TX_LEN);
    u8TxPORTBuf[0] = PORT_HEAD_0;
    u8TxPORTBuf[1] = u8length;
    u8TxPORTBuf[2] = u8Cmd;
    memcpy(&u8TxPORTBuf[3],txbuf,u8buflen);
    u16CheckSum = Port_Checksum(&u8TxPORTBuf[1],u8length-2);
    u8TxPORTBuf[u8length-1] = LO_UINT16(u16CheckSum);
    uart3_tx_string(u8TxPORTBuf,u8length);
}
/*----------------------------------------------------------------------------
�������ƣ� Bsp_Write_Boot_Flag
���������� дBOOT��־λ
----------------------------------------------------------------------------*/
void Bsp_Write_Boot_Flag(void)
{
    Hal_Flash_Write_Word(APPFLAG_ADD,FLAG_BOOT);
}

/*----------------------------------------------------------------------------
�������ƣ� write_id
���������� дID
----------------------------------------------------------------------------*/
void Bsp_Write_ID(uint32_t u32ID)
{
    Hal_Flash_Write_Word(ID_ADDR,u32ID);
}

/*----------------------------------------------------------------------------
�������ƣ� Bsp_Read_ID
���������� ��ID
----------------------------------------------------------------------------*/
uint32_t Bsp_Read_ID(void)
{
    return Hal_Flash_Read_Word(ID_ADDR);
}
/*----------------------------------------------------------------------------
�������ƣ�ota_start_rep
����������OTA�������ظ�
��������� rep ->0X01/�ɹ�   0X00/ʧ��
----------------------------------------------------------------------------*/
void ota_start_rep(uint8_t *rxbuf)
{
    uint8_t u8RepBuf[1];
    if(rxbuf[0])
    {
        Bsp_Write_Boot_Flag();
        GNSS_POW_DIS();
        RDSS_POW_DIS();
        RS422_POW_DIS();
        Hal_Falsh_Jump();
        u8RepBuf[0] = 0x0;
    }
    else
    {
        u8RepBuf[0] = 0x0;
    }
	Port_Tx_Cmd_Pack(PORT_CMD_OTA_START,u8RepBuf,1);						//��װЭ�鷢��
}
/*----------------------------------------------------------------------------
�������ƣ� versions_rep
���������� ��ѯ�ظ�
----------------------------------------------------------------------------*/
void versions_rep(void)
{
    uint8_t     u8RepBuf[2];
    u8RepBuf[0] = SOFT_VER;
    u8RepBuf[1] = HARD_VER;
    Port_Tx_Cmd_Pack(PORT_CMD_VERSIONS,u8RepBuf,2);						//��װЭ�鷢��
}
/*----------------------------------------------------------------------------
�������ƣ� open_closs_gnss
���������� GNSS����
----------------------------------------------------------------------------*/
void open_closs_gnss(uint8_t *rxbuf)
{
    uint8_t     u8RepBuf[1];
    if(Bsp_Get_Pow_State() == POWOFF)
    {
        if(rxbuf[0])
        {
            if(u8UartMode != MODE_GNSS)
            {
                if(u8UartMode == MODE_RDSS)
                {
                    Hal_Uart4_Deinit();
                    RDSS_POW_DIS();
                    GNSS_POW_EN();
                    Hal_Uart1_Init();
                    u8UartMode = MODE_GNSS;
                }
                else
                {
                    GNSS_POW_EN();
                    Hal_Uart1_Init();
                    u8UartMode = MODE_GNSS;
                }
            }
        }
        else
        {
            Hal_Uart1_Deinit();
            GNSS_POW_DIS();
            u8UartMode = MODE_COMMOND;
        }
    }
    u8RepBuf[0] = 1;
    Port_Tx_Cmd_Pack(PORT_CMD_GNSS,u8RepBuf,1);						//��װЭ�鷢��
}
/*----------------------------------------------------------------------------
�������ƣ� open_closs_rdss
���������� RDSS����
----------------------------------------------------------------------------*/
void open_closs_rdss(uint8_t *rxbuf)
{
    uint8_t     u8RepBuf[1];
    if(Bsp_Get_Sys_State() == POWOFF)
    {
        if(rxbuf[0])
        {
            if(u8UartMode != MODE_RDSS)
            {
                if(u8UartMode == MODE_GNSS)
                {
                    Hal_Uart1_Deinit();
                    GNSS_POW_DIS();
                    RDSS_POW_EN();
                    Hal_Uart4_Init();
                    u8UartMode = MODE_RDSS;
                }
                else
                {
                    RDSS_POW_EN();
                    Hal_Uart4_Init();
                    u8UartMode = MODE_RDSS;
                }
            }
        }
        else
        {
            Hal_Uart4_Deinit();
            RDSS_POW_DIS();
            u8UartMode = MODE_COMMOND;
        }
    }
    u8RepBuf[0] = 1;
    Port_Tx_Cmd_Pack(PORT_CMD_RDSS,u8RepBuf,1);						//��װЭ�鷢��
}
/*----------------------------------------------------------------------------
�������ƣ� Bsp_Get_Uart_Mode
���������� ��ȡ����ģʽ
----------------------------------------------------------------------------*/
uint8_t Bsp_Get_Uart_Mode(void)
{
    return u8UartMode;
}
/*----------------------------------------------------------------------------
�������ƣ� infor_get
���������� �豸��Ϣ��ѯ
----------------------------------------------------------------------------*/
void infor_get(void)
{
    if(!tInforGet.u8InforGetEn)
    {
        tInforGet.u8InforGetEn = 1;
        tInforGet.eInforGetState = INFORGET_START;
    }
}
/*----------------------------------------------------------------------------
�������ƣ� infor_rep
���������� �豸��Ϣ�ظ�
----------------------------------------------------------------------------*/
void infor_rep(void)
{
    uint8_t     u8RepBuf[20];
    uint32_t    u32ReadID = 0;
    memset(&u8RepBuf[0],0,20);
    u32ReadID = Bsp_Read_ID();
    tInforGet.u8GetPercent = Bsp_Get_BatPercent();
    u8RepBuf[0] = BREAK_UINT32(u32ReadID,0);
    u8RepBuf[1] = BREAK_UINT32(u32ReadID,1);
    u8RepBuf[2] = BREAK_UINT32(u32ReadID,2);
    u8RepBuf[3] = BREAK_UINT32(u32ReadID,3);
    u8RepBuf[4] = BREAK_UINT32(tInforGet.u32GetCardID,0);
    u8RepBuf[5] = BREAK_UINT32(tInforGet.u32GetCardID,1);
    u8RepBuf[6] = BREAK_UINT32(tInforGet.u32GetCardID,2);
    u8RepBuf[7] = BREAK_UINT32(tInforGet.u32GetCardID,3);
    u8RepBuf[8] = tInforGet.u8GetPercent;
    u8RepBuf[9] = SOFT_VER;  
    Port_Tx_Cmd_Pack(PORT_CMD_INFOR,u8RepBuf,20);						//��װЭ�鷢��
}
/*----------------------------------------------------------------------------
�������ƣ� Bsp_Get_CardID
���������� ��ȡ��������
----------------------------------------------------------------------------*/
void Bsp_Get_CardID(uint32_t u32InCardID)
{
    if(tInforGet.u8CardIDGet)
    {
        tInforGet.u8CardIDGet = 0;
        tInforGet.u32GetCardID =u32InCardID;
    }
}
/*----------------------------------------------------------------------------
�������ƣ� Bsp_Port_InforGet_Ctrl
���������� �豸��Ϣ��ѯ����/100sʱ��
----------------------------------------------------------------------------*/
void Bsp_Port_InforGet_Ctrl(void)
{
    if(tInforGet.u8InforGetEn)
    {
        switch(tInforGet.eInforGetState)
        {
            case INFORGET_START:
            {
                tInforGet.u16Cont = 0;
                tInforGet.u8TxCont = 0;
                tInforGet.u8GetPercent = 0;
                tInforGet.u8CardIDGet = 1;
                Bps_Start_Once_Time_Updata_BatVol_Percent();
                RDSS_POW_EN();
                Hal_Uart4_Init();
                tInforGet.eInforGetState = INFORGET_WAIT;
                break;
            }
            case INFORGET_WAIT:
            {
                if(tInforGet.u8CardIDGet)
                {
                    if(tInforGet.u16Cont++ > 9)
                    {
                        tInforGet.u16Cont = 0;
                        if(tInforGet.u8TxCont++ > 3)
                        {
                            if(!Bsp_Get_Updata_Bat_Voltage_Once_State())
                            {
                                tInforGet.u8TxCont = 0;
                                tInforGet.u8CardIDGet = 0;
                                tInforGet.u32GetCardID = 0;
                                tInforGet.eInforGetState = INFORGET_FINISH;
                            }
                        }
                        else
                        {
                            Bsp_Rdss_Tx_CCICR();
                        }
                    }
                }
                else
                {
                    if(!Bsp_Get_Updata_Bat_Voltage_Once_State())
                    {
                        tInforGet.u16Cont = 0;
                        tInforGet.u16Cont = 0;
                        tInforGet.eInforGetState = INFORGET_FINISH;
                    }
                }
                break;
            }
            case INFORGET_FINISH:
            {
                infor_rep();
                RDSS_POW_DIS();
                Hal_Uart4_Deinit();
                tInforGet.u8InforGetEn = 0;
                break;
            }
            default:
                break;
        }
    }
}
/*----------------------------------------------------------------------------
�������ƣ� id_write
���������� ID��д
----------------------------------------------------------------------------*/
void id_write(uint8_t *rxbuf)
{
    uint32_t u32RxID = 0;
    uint32_t u32ReadID = 0;
    uint8_t u8RepBuf[5];
    memset(&u8RepBuf[0],0,5);
    u32RxID = BUILD_UINT32(rxbuf[3],rxbuf[2],rxbuf[1],rxbuf[0]);
    if(u32RxID)
    {
        Bsp_Write_ID(u32RxID);
        u32ReadID = Bsp_Read_ID();
        if(u32ReadID == u32RxID)
        {
            memcpy(&u8RepBuf[0],&rxbuf[0],4);
            u8RepBuf[4] = 0x01;                                             //OK
            Port_Tx_Cmd_Pack(PORT_CMD_IDWRITE,u8RepBuf,5);					//��װЭ�鷢��
        }
        else
        {
            u8RepBuf[0] = BREAK_UINT32(u32ReadID,0);
            u8RepBuf[1] = BREAK_UINT32(u32ReadID,1);
            u8RepBuf[2] = BREAK_UINT32(u32ReadID,2);
            u8RepBuf[3] = BREAK_UINT32(u32ReadID,3);
            u8RepBuf[4] = 0x02;                                                 //ID��¼ʧ�ܣ�NG
            Port_Tx_Cmd_Pack(PORT_CMD_IDWRITE,u8RepBuf,5);						//��װЭ�鷢��
        }
    }
    else
    {
        u8RepBuf[4] = 0x02;                                                     //IDΪ0��NG
        Port_Tx_Cmd_Pack(PORT_CMD_IDWRITE,u8RepBuf,5);						    //��װЭ�鷢��
    }
}
/*----------------------------------------------------------------------------
�������ƣ� Port_Rxdata_deal
���������� �������ݴ���
��������� pu8Data ���� u8Len ���ݳ���
----------------------------------------------------------------------------*/
void Port_Rxdata_deal(uint8_t *pu8Data,uint8_t u8Len)
{

    uint8_t u8Cmd = 0;
    uint8_t u8BufLen = 0;
	uint8_t buf[PORT_MAX_LEN] = {0};
    u8Cmd = pu8Data[PORT_IDX_PLD];		                //���ֵ��u8Cmd
    u8BufLen = u8Len - 4;
	memcpy(buf,&pu8Data[PORT_IDX_PLD+1],u8BufLen);	
    switch(u8Cmd)								        //��������
    {
		/* OTA�������� */
        case PORT_CMD_OTA_START:
        {		
            ota_start_rep(buf);
        }
        break;
        /* ��ѯ */
        case PORT_CMD_INFOR:
        {		
            #if   LOG
            printf("Infor Get\n");
            #endif
            infor_get();		
        }
        break;
        /* ID��д */
        case PORT_CMD_IDWRITE:
        {	
            #if   LOG
            printf("ID Write\n");
            #endif
            id_write(buf);
        }
        break;
        /* �汾�Ų�ѯ */
        case PORT_CMD_VERSIONS:
        {	
            versions_rep();
        }
        break;
        /* GNSS͸�� */
        case PORT_CMD_GNSS:
        {	
            open_closs_gnss(buf);
        }
        break;
        /* RDSS͸�� */
        case PORT_CMD_RDSS:
        {	
            open_closs_rdss(buf);
        }
        break;
        default:
        {

        }
        break;
    }
}
/*----------------------------------------------------------------------------
�������ƣ� Bsp_Port_Parse_Byte
���������� ���ֽڽ���
----------------------------------------------------------------------------*/
void Bsp_Port_Parse_Byte(uint8_t u8Val)
{
    uint8_t u8BufLen = 0;                                       //�����ܳ���
    uint16_t u16CheckSum = 0;						            //16λУ���
    uint8_t u8CheckSumTmp = 0;						            //���յļ����
    
    if(tPortRxCtrl.u8Step == 0)							        //���������0�ֽ�
    {
        if(u8Val != PORT_HEAD_0)						        //�����ǰ�����ֽ����ݲ�����Э��ͷ0
        {
            tPortRxCtrl.u8Step = 0;						        //��ǰ�����ֽڸ�λ0
        }
        else											        //�������Э��ͷ0
        {
            tPortRxCtrl.pu8Pkt[tPortRxCtrl.u8Step++] = u8Val;	//������1�ֽڣ���ǰ�ֽ����ݴ�������0
        }
    }
    else if(tPortRxCtrl.u8Step == 1)
    {
        if(u8Val<3 || u8Val > PORT_MAX_LEN)
        {
            tPortRxCtrl.u8Step = 0;						        //��ǰ�����ֽڸ�λ0
        }
        else
        {
            tPortRxCtrl.pu8Pkt[tPortRxCtrl.u8Step++] = u8Val;	//������1�ֽڣ���ǰ�ֽ����ݴ�������0
        }
    }
    else 						                                //���������Э��ͷ������1~7�ֽڶ���������
    {
        tPortRxCtrl.pu8Pkt[tPortRxCtrl.u8Step++] = u8Val;		//����У��Ͷ�Ϊ�������ݣ���������
        if(tPortRxCtrl.u8Step == tPortRxCtrl.pu8Pkt[1])
        {
            u8BufLen = tPortRxCtrl.pu8Pkt[1];
			u16CheckSum = Port_Checksum(&tPortRxCtrl.pu8Pkt[1], u8BufLen - 2);
            u8CheckSumTmp = tPortRxCtrl.pu8Pkt[u8BufLen-1];
            if(LO_UINT16(u16CheckSum) != u8CheckSumTmp)
            {
                tPortRxCtrl.u8Step = 0;				        //��ǰ�����ֽڸ�λ0
            }
            Port_Rxdata_deal(&tPortRxCtrl.pu8Pkt[0],u8BufLen);
            tPortRxCtrl.u8Step = 0;						    //��ǰ�����ֽڸ�λ0
        }
    } 
}
/*----------------------------------------------------------------------------
�������ƣ� Bsp_Port_Init
���������� Э��ͨ�ų�ʼ��
----------------------------------------------------------------------------*/
void Bsp_Port_Init(void)
{
    memset(&tPortRxCtrl, 0, sizeof(PORT_RX_UNPACK_T));						//��ս��սṹ��
}

