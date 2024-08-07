/*************************************************************************
*  	�ļ�����:  	Hal_Flash.c
*  	�ļ�����:  	FLASH��ʼ����FLASH����FLASHд
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

#include    "at32f435_437_flash.h"

#include    "Hal_Flash.h"
/************************************************************************
�궨��
*************************************************************************/
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
typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
unsigned int JumpAddress;
/*************************************************************************
��̬��������

extern �ⲿ��������
*************************************************************************/

/*************************************************************************
��������
*************************************************************************/
/*----------------------------------------------------------------------------
�������ƣ� Hal_Flash_Init
���������� FLASH��ʼ��
----------------------------------------------------------------------------*/
void Hal_Flash_Init(void)
{
    flash_unlock();                 //����д
}
/*----------------------------------------------------------------------------
�������ƣ� flash_readwords
���������� FLASH��һ��32�ֽ�
��������� u32ReadAdd  ����ַ
----------------------------------------------------------------------------*/
uint32_t  Hal_Flash_Read_Word(uint32_t u32ReadAdd)
{
    return *(__IO uint32_t *)u32ReadAdd;
}
/*----------------------------------------------------------------------------
�������ƣ� Hal_Flash_Write_Word
���������� FLASHдһ��32�ֽ�
��������� u32WriteAdd  д��ַ
           u32WriteDate д����
----------------------------------------------------------------------------*/
void  Hal_Flash_Write_Word(uint32_t u32WriteAdd,uint32_t u32WriteDate)
{
    Hal_Flash_Init();
    flash_sector_erase(u32WriteAdd);
    flash_word_program(u32WriteAdd,(uint32_t)u32WriteDate);
}
/*----------------------------------------------------------------------------
�������ƣ� Hal_Falsh_Erase_More_Page
���������� ����FLASH���ҳ
���룺     u32EraseAdd ������ַ   
           u8PageNum ����ҳ����
�����     SUCC  �ɹ�    FAIL  ʧ��
----------------------------------------------------------------------------*/
uint8_t Hal_Falsh_Erase_More_Page(uint32_t u32EraseAdd,uint8_t u8PageNum)
{
    uint8_t u8Con = 0;
    if(u32EraseAdd > 0x080FF800 || u32EraseAdd < 0x08000000)
        return FAIL;
    for(u8Con = 0;u8Con < u8PageNum;u8Con++)
    {
        flash_sector_erase(u32EraseAdd); 
        u32EraseAdd += PAGESIZE;
    }
    return SUCC;
}
/*----------------------------------------------------------------------------
�������ƣ� Hal_Flash_Write_More_Word
���������� дFLASH
��������� u32DataAddr   д��ַ    
           Buf           д����   
           u32CharSize   �ֽڳ���
----------------------------------------------------------------------------*/
uint32_t Hal_Flash_Write_More_Word(uint32_t u32DataAddr,uint8_t *Buf,uint32_t u32ByteSize)
{
    uint32_t writeword,n,u32Addr;
    u32Addr = u32DataAddr;
    
    if(!ADDR_IN_FLASH(u32DataAddr))
    {
        return FAIL;
    }
	for (n=0;n<u32ByteSize;n+=4)
	{
        writeword = BUILD_UINT32(Buf[n+3], Buf[n+2], Buf[n+1], Buf[n]);
        flash_word_program(u32Addr,writeword);
		u32Addr += 4;
	}
    return SUCC;
}
/*----------------------------------------------------------------------------
�������ƣ� Hal_Flash_Read_More_Word
���������� ��FLASH���ֽ�
��������� pBuf              �����BUF    
           u32Addr           ����ַ   
           u16WordSize       �ֳ���
----------------------------------------------------------------------------*/
uint8_t Hal_Flash_Read_More_Word(uint32_t *pBuf,uint32_t u32Addr,uint16_t u16WordSize)
{
    uint32_t u32ReadAddr;
    u32ReadAddr = u32Addr;
    if(!ADDR_IN_FLASH(u32ReadAddr))
        return FAIL;
    while(u16WordSize--)
    {
        *pBuf++ = Hal_Flash_Read_Word(u32ReadAddr);
        u32ReadAddr+=4;
    }
    return SUCC;
}
void app_clear_sys_status() 
{ 
 /*Close Peripherals Clock*/ 
 CRM->apb2rst = 0xFFFF; 
 CRM->apb2rst = 0; 
 CRM->apb1rst = 0xFFFF; 
 CRM->apb1rst = 0; 
 CRM->apb1en = 0; 
 CRM->apb2en = 0; 
 /*Close PLL*/ 
 /* Reset SW, AHBDIV, APB1DIV, APB2DIV, ADCDIV and CLKOUT_SEL bits */ 
 CRM->cfg_bit.sclksel = 0; 
 CRM->cfg_bit.ahbdiv = 0; 
 CRM->cfg_bit.apb1div = 0; 
 CRM->cfg_bit.apb2div = 0; 
 CRM->cfg_bit.ertcdiv = 0; 
 CRM->ctrl_bit.hexten = 0; 
 CRM->ctrl_bit.cfden = 0; 
 CRM->ctrl_bit.pllen = 0; 
 /* Disable all interrupts and clear pending bits  */ 
 CRM->clkint_bit.lickstblfc = 0; 
 CRM->clkint_bit.lextstblfc = 0; 
 CRM->clkint_bit.hickstblfc = 0; 
 CRM->clkint_bit.hextstblfc = 0; 
 CRM->clkint_bit.pllstblfc = 0; 
 CRM->clkint_bit.cfdfc = 0; 
 /*Colse Systick*/ 
 SysTick->CTRL = 0; 
   
 /*Disable ALL interrupt && Pending Interrupt Flag*/ 
 /*������Ҫ�����û������������������жϺ͹�����жϱ�־*/ 
 /* 
 user add code... 
 */ 
} 

/*----------------------------------------------------------------------------
�������ƣ� Hal_Falsh_Jump
���������� ��ת��BOOT
----------------------------------------------------------------------------*/
void Hal_Falsh_Jump(void)
{
  if(((*(__IO uint32_t*)CSBOOT_BOOT_BASE) - 0x20000000) <= (SRAM_SIZE * 1024))
  {
    app_clear_sys_status();
    crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK, FALSE);
    crm_periph_clock_enable(CRM_USART2_PERIPH_CLOCK, FALSE);
    crm_periph_clock_enable(CRM_USART3_PERIPH_CLOCK, FALSE);
    crm_periph_clock_enable(CRM_UART4_PERIPH_CLOCK, FALSE);
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, FALSE);
    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, FALSE);
    crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, FALSE);
    crm_periph_clock_enable(CRM_GPIOH_PERIPH_CLOCK, FALSE);
    crm_periph_clock_enable(CRM_SCFG_PERIPH_CLOCK, FALSE);
    
    nvic_irq_disable(USART1_IRQn);
    nvic_irq_disable(USART2_IRQn);
    nvic_irq_disable(USART3_IRQn);
    nvic_irq_disable(UART4_IRQn);
    Jump_To_Application = (pFunction)*(uint32_t*)(CSBOOT_BOOT_BASE+4);
    __set_MSP(*(__IO uint32_t*) CSBOOT_BOOT_BASE);            			
    Jump_To_Application();
  }
}
