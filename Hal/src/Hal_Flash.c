/*************************************************************************
*  	文件名称:  	Hal_Flash.c
*  	文件描述:  	FLASH初始化、FLASH读、FLASH写
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

#include    "at32f435_437_flash.h"

#include    "Hal_Flash.h"
/************************************************************************
宏定义
*************************************************************************/
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
typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
unsigned int JumpAddress;
/*************************************************************************
静态函数声明

extern 外部函数声明
*************************************************************************/

/*************************************************************************
函数定义
*************************************************************************/
/*----------------------------------------------------------------------------
函数名称： Hal_Flash_Init
具体描述： FLASH初始化
----------------------------------------------------------------------------*/
void Hal_Flash_Init(void)
{
    flash_unlock();                 //解锁写
}
/*----------------------------------------------------------------------------
函数名称： flash_readwords
具体描述： FLASH读一个32字节
输入参数： u32ReadAdd  读地址
----------------------------------------------------------------------------*/
uint32_t  Hal_Flash_Read_Word(uint32_t u32ReadAdd)
{
    return *(__IO uint32_t *)u32ReadAdd;
}
/*----------------------------------------------------------------------------
函数名称： Hal_Flash_Write_Word
具体描述： FLASH写一个32字节
输入参数： u32WriteAdd  写地址
           u32WriteDate 写数据
----------------------------------------------------------------------------*/
void  Hal_Flash_Write_Word(uint32_t u32WriteAdd,uint32_t u32WriteDate)
{
    Hal_Flash_Init();
    flash_sector_erase(u32WriteAdd);
    flash_word_program(u32WriteAdd,(uint32_t)u32WriteDate);
}
/*----------------------------------------------------------------------------
函数名称： Hal_Falsh_Erase_More_Page
具体描述： 擦除FLASH多个页
输入：     u32EraseAdd 擦除地址   
           u8PageNum 擦除页数量
输出：     SUCC  成功    FAIL  失败
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
函数名称： Hal_Flash_Write_More_Word
具体描述： 写FLASH
输入参数： u32DataAddr   写地址    
           Buf           写数据   
           u32CharSize   字节长度
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
函数名称： Hal_Flash_Read_More_Word
具体描述： 读FLASH多字节
输入参数： pBuf              保存的BUF    
           u32Addr           读地址   
           u16WordSize       字长度
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
 /*这里需要根据用户开启的外设进行清除中断和挂起的中断标志*/ 
 /* 
 user add code... 
 */ 
} 

/*----------------------------------------------------------------------------
函数名称： Hal_Falsh_Jump
具体描述： 跳转到BOOT
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
