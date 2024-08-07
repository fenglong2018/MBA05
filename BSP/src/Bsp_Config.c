/*************************************************************************
*  	文件名称:  	Bsp_Config.c
*  	文件描述:  	串口协议收发和解析、ID烧写、FLASH跳转
*  	历史记录:
*  	创建时间:  	2024-05-10
*  	创建人:  	江昌钤
*  	修改时间:
*	修改人:
*   描述:
*************************************************************************/

/*************************************************************************
头文件引用声明
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
宏定义
*************************************************************************/
/* 取16位中的高8位 */
#define HI_UINT16(a) 				(((a) >> 8) & 0xFF)
/* 取16位中的低8位 */
#define LO_UINT16(a) 				((a) & 0xFF)
/* 从32位数中取一字节 : var - 待取uint32数,  ByteNum - 第 (0 - 3)字节 */
#define BREAK_UINT32( var, ByteNum ) \
          							(uint8_t)((uint32_t)(((var) >>((ByteNum) * 8)) & 0x00FF))
                                    /* 4字节合并成uint32数 : Byte0~Byte3 对应 低位开始的0-3字节*/
#define BUILD_UINT32(Byte3, Byte2, Byte1, Byte0) \
          							((uint32_t)((uint32_t)((Byte0) & 0x00FF) \
							          + ((uint32_t)((Byte1) & 0x00FF) << 8) \
							          + ((uint32_t)((Byte2) & 0x00FF) << 16) \
							          + ((uint32_t)((Byte3) & 0x00FF) << 24)))
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
/* 协议接收结构体 */
PORT_RX_UNPACK_T tPortRxCtrl;

/* 一整包协议发送缓存数组 */
uint8_t u8TxPORTBuf[PORT_TX_LEN] = {0};
//--------------------------------设备信息查询———————————————//
INFOR_GET_T         tInforGet = {0};

uint8_t             u8UartMode = 0;
/*************************************************************************
函数定义
*************************************************************************/
/*----------------------------------------------------------------------------
函数名称： Port_Checksum
具体描述： 计算累加检验和
输入参数： buf 数组 buflen 数据长度
输出参数： 校验和
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
函数名称： Port_Tx_Cmd_Pack
具体描述： 封装协议与发送
输入参数： u8Cmd 命令  txbuf 发送数组   u8buflen 数据长度
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
函数名称： Bsp_Write_Boot_Flag
具体描述： 写BOOT标志位
----------------------------------------------------------------------------*/
void Bsp_Write_Boot_Flag(void)
{
    Hal_Flash_Write_Word(APPFLAG_ADD,FLAG_BOOT);
}

/*----------------------------------------------------------------------------
函数名称： write_id
具体描述： 写ID
----------------------------------------------------------------------------*/
void Bsp_Write_ID(uint32_t u32ID)
{
    Hal_Flash_Write_Word(ID_ADDR,u32ID);
}

/*----------------------------------------------------------------------------
函数名称： Bsp_Read_ID
具体描述： 读ID
----------------------------------------------------------------------------*/
uint32_t Bsp_Read_ID(void)
{
    return Hal_Flash_Read_Word(ID_ADDR);
}
/*----------------------------------------------------------------------------
函数名称：ota_start_rep
具体描述：OTA启动包回复
输入参数： rep ->0X01/成功   0X00/失败
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
	Port_Tx_Cmd_Pack(PORT_CMD_OTA_START,u8RepBuf,1);						//封装协议发送
}
/*----------------------------------------------------------------------------
函数名称： versions_rep
具体描述： 查询回复
----------------------------------------------------------------------------*/
void versions_rep(void)
{
    uint8_t     u8RepBuf[2];
    u8RepBuf[0] = SOFT_VER;
    u8RepBuf[1] = HARD_VER;
    Port_Tx_Cmd_Pack(PORT_CMD_VERSIONS,u8RepBuf,2);						//封装协议发送
}
/*----------------------------------------------------------------------------
函数名称： open_closs_gnss
具体描述： GNSS开关
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
    Port_Tx_Cmd_Pack(PORT_CMD_GNSS,u8RepBuf,1);						//封装协议发送
}
/*----------------------------------------------------------------------------
函数名称： open_closs_rdss
具体描述： RDSS开关
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
    Port_Tx_Cmd_Pack(PORT_CMD_RDSS,u8RepBuf,1);						//封装协议发送
}
/*----------------------------------------------------------------------------
函数名称： Bsp_Get_Uart_Mode
具体描述： 获取串口模式
----------------------------------------------------------------------------*/
uint8_t Bsp_Get_Uart_Mode(void)
{
    return u8UartMode;
}
/*----------------------------------------------------------------------------
函数名称： infor_get
具体描述： 设备信息查询
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
函数名称： infor_rep
具体描述： 设备信息回复
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
    Port_Tx_Cmd_Pack(PORT_CMD_INFOR,u8RepBuf,20);						//封装协议发送
}
/*----------------------------------------------------------------------------
函数名称： Bsp_Get_CardID
具体描述： 获取北斗卡号
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
函数名称： Bsp_Port_InforGet_Ctrl
具体描述： 设备信息查询控制/100s时基
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
函数名称： id_write
具体描述： ID烧写
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
            Port_Tx_Cmd_Pack(PORT_CMD_IDWRITE,u8RepBuf,5);					//封装协议发送
        }
        else
        {
            u8RepBuf[0] = BREAK_UINT32(u32ReadID,0);
            u8RepBuf[1] = BREAK_UINT32(u32ReadID,1);
            u8RepBuf[2] = BREAK_UINT32(u32ReadID,2);
            u8RepBuf[3] = BREAK_UINT32(u32ReadID,3);
            u8RepBuf[4] = 0x02;                                                 //ID烧录失败，NG
            Port_Tx_Cmd_Pack(PORT_CMD_IDWRITE,u8RepBuf,5);						//封装协议发送
        }
    }
    else
    {
        u8RepBuf[4] = 0x02;                                                     //ID为0，NG
        Port_Tx_Cmd_Pack(PORT_CMD_IDWRITE,u8RepBuf,5);						    //封装协议发送
    }
}
/*----------------------------------------------------------------------------
函数名称： Port_Rxdata_deal
具体描述： 接收数据处理
输入参数： pu8Data 数组 u8Len 数据长度
----------------------------------------------------------------------------*/
void Port_Rxdata_deal(uint8_t *pu8Data,uint8_t u8Len)
{

    uint8_t u8Cmd = 0;
    uint8_t u8BufLen = 0;
	uint8_t buf[PORT_MAX_LEN] = {0};
    u8Cmd = pu8Data[PORT_IDX_PLD];		                //命令赋值给u8Cmd
    u8BufLen = u8Len - 4;
	memcpy(buf,&pu8Data[PORT_IDX_PLD+1],u8BufLen);	
    switch(u8Cmd)								        //命令类型
    {
		/* OTA启动命令 */
        case PORT_CMD_OTA_START:
        {		
            ota_start_rep(buf);
        }
        break;
        /* 查询 */
        case PORT_CMD_INFOR:
        {		
            #if   LOG
            printf("Infor Get\n");
            #endif
            infor_get();		
        }
        break;
        /* ID烧写 */
        case PORT_CMD_IDWRITE:
        {	
            #if   LOG
            printf("ID Write\n");
            #endif
            id_write(buf);
        }
        break;
        /* 版本号查询 */
        case PORT_CMD_VERSIONS:
        {	
            versions_rep();
        }
        break;
        /* GNSS透传 */
        case PORT_CMD_GNSS:
        {	
            open_closs_gnss(buf);
        }
        break;
        /* RDSS透传 */
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
函数名称： Bsp_Port_Parse_Byte
具体描述： 按字节解析
----------------------------------------------------------------------------*/
void Bsp_Port_Parse_Byte(uint8_t u8Val)
{
    uint8_t u8BufLen = 0;                                       //包体总长度
    uint16_t u16CheckSum = 0;						            //16位校验和
    uint8_t u8CheckSumTmp = 0;						            //接收的检验和
    
    if(tPortRxCtrl.u8Step == 0)							        //如果解析第0字节
    {
        if(u8Val != PORT_HEAD_0)						        //如果当前解析字节数据不等于协议头0
        {
            tPortRxCtrl.u8Step = 0;						        //当前解析字节复位0
        }
        else											        //如果等于协议头0
        {
            tPortRxCtrl.pu8Pkt[tPortRxCtrl.u8Step++] = u8Val;	//解析第1字节，当前字节数据存入数组0
        }
    }
    else if(tPortRxCtrl.u8Step == 1)
    {
        if(u8Val<3 || u8Val > PORT_MAX_LEN)
        {
            tPortRxCtrl.u8Step = 0;						        //当前解析字节复位0
        }
        else
        {
            tPortRxCtrl.pu8Pkt[tPortRxCtrl.u8Step++] = u8Val;	//解析第1字节，当前字节数据存入数组0
        }
    }
    else 						                                //如果解析完协议头，解析1~7字节都存入数组
    {
        tPortRxCtrl.pu8Pkt[tPortRxCtrl.u8Step++] = u8Val;		//除了校验和都为包体数据，存入数组
        if(tPortRxCtrl.u8Step == tPortRxCtrl.pu8Pkt[1])
        {
            u8BufLen = tPortRxCtrl.pu8Pkt[1];
			u16CheckSum = Port_Checksum(&tPortRxCtrl.pu8Pkt[1], u8BufLen - 2);
            u8CheckSumTmp = tPortRxCtrl.pu8Pkt[u8BufLen-1];
            if(LO_UINT16(u16CheckSum) != u8CheckSumTmp)
            {
                tPortRxCtrl.u8Step = 0;				        //当前解析字节复位0
            }
            Port_Rxdata_deal(&tPortRxCtrl.pu8Pkt[0],u8BufLen);
            tPortRxCtrl.u8Step = 0;						    //当前解析字节复位0
        }
    } 
}
/*----------------------------------------------------------------------------
函数名称： Bsp_Port_Init
具体描述： 协议通信初始化
----------------------------------------------------------------------------*/
void Bsp_Port_Init(void)
{
    memset(&tPortRxCtrl, 0, sizeof(PORT_RX_UNPACK_T));						//清空接收结构体
}

