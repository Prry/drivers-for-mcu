/*
 * File      : 25xx.c
 * 
 * This file is part of eeprom driver for  25xx,such as 25aa256
 * COPYRIGHT (C) 2016-
 *
 * Change Logs:
 * Date           Author       Notes
 * 2016-12-26     Acuity      first version.
 *
 * Depend on:
 * spi_core.c & spi_hw.c or spi_bitops.h
 *
 * note:
 *
 */
#include  "25xx.h"

#define USE_25XX_DEBUG 	0

/*module variable*/
#define	   EE25XX_PAGE_SIZE	    64 /* 25aa256 */

/* 25xx register */
#define    REG_READ_COMMAND    0x03
#define	   REG_WRITE_COMMAND   0x02
#define	   REG_WRITE_ENABLE    0x06
#define	   REG_WRITE_DISABLE   0x04
#define	   REG_READ_STATUS     0x05
#define	   REG_WRITE_STATUS    0x01

/* 25xx spi device */
struct 	spi_dev_device	ee_25xx_spi_dev;

/* module function */
static u8 ee_25xx_wait_busy(void);

static void spi1_cs(unsigned char state)
{
	if (state)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_12);
	}
   	else
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	}
}

void ee_25xx_init(struct spi_bus_device *spi_bus)
{
	GPIO_InitTypeDef GPIO_InitStructure;
		
	/* spi cs */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;										
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
			
	/* device init */
	ee_25xx_spi_dev.spi_cs 	= spi1_cs;
	ee_25xx_spi_dev.spi_bus = spi_bus;			
}

void ee_25xx_write_enable(uint8_t select)
{
	spi_send(&ee_25xx_spi_dev,&select,1);
}

void ee_25xx_write_byte(uint16_t write_addr,uint8_t write_data)
{
	uint8_t send_buff[3];
	
	ee_25xx_write_enable(REG_WRITE_ENABLE);
	send_buff[0] = REG_WRITE_COMMAND;
	send_buff[1] = (write_addr>>8)&0xff;
	send_buff[2] = write_addr&0xff;
	spi_send_then_send(&ee_25xx_spi_dev,send_buff,3,&write_data,1);
	ee_25xx_write_enable(REG_WRITE_DISABLE);
}

/**
  * @brief  25xx write page
  * @param  address,buff,size.write_size is not more than device page size,such as 64 bytes for 25aa256. 
  * @retval None
  */
void ee_25xx_write_page(uint16_t write_addr,uint8_t *write_page_buff,uint8_t write_size)
{
	uint8_t send_buff[3];
	
	ee_25xx_write_enable(REG_WRITE_ENABLE);
	send_buff[0] = REG_WRITE_COMMAND;
	send_buff[1] = (write_addr>>8)&0xff;
	send_buff[2] = write_addr&0xff;
	spi_send_then_send(&ee_25xx_spi_dev,send_buff,3,write_page_buff,write_size);
	ee_25xx_write_enable(REG_WRITE_DISABLE);
}

void ee_25xx_write_bytes(uint16_t write_addr, uint8_t *write_buff, uint16_t write_bytes)
{
	uint8_t  write_current_bytes,page_offset;
	uint16_t write_remain,write_current_addr;
	uint8_t *pbuff;
	
	//ee_25xx_write_enable(REG_WRITE_ENABLE);
	pbuff = write_buff;			
	write_remain = write_bytes;
	write_current_addr = write_addr;
	while(write_remain > 0)
	{
		page_offset = write_current_addr % EE25XX_PAGE_SIZE;		//连续页写大小，25aa256为64字节
		write_current_bytes = write_remain > (EE25XX_PAGE_SIZE - page_offset) ? (EE25XX_PAGE_SIZE - page_offset) : write_remain;
		ee_25xx_write_page(write_current_addr,pbuff, write_current_bytes);
		write_remain  = write_remain - write_current_bytes;
		if(write_remain > 0)
		{
			pbuff = pbuff + EE25XX_PAGE_SIZE - page_offset;
			write_current_addr  =  write_current_addr + EE25XX_PAGE_SIZE - page_offset;
			//Delayms(5);
			ee_25xx_wait_busy();//wait free,根据cpu时钟计算时间
		}
	}
	//ee_25xx_write_enable(REG_WRITE_DISABLE);
}

void ee_25xx_read_bytes(uint16_t read_addr,uint8_t *read_buff,uint16_t read_bytes)
{
	uint8_t send_buff[3];
	
	send_buff[0] = REG_READ_COMMAND;
	send_buff[1] = (read_addr>>8)&0xff;
	send_buff[2] = read_addr&0xff;
	spi_send_then_recv(&ee_25xx_spi_dev,send_buff,3,read_buff,read_bytes);
}

void ee_25xx_write_status(uint8_t write_data)
{
	uint8_t send_buff[2];
	
	send_buff[0] = REG_WRITE_STATUS;
	send_buff[1] = write_data;
	spi_send(&ee_25xx_spi_dev,send_buff,2);
}

uint8_t ee_25xx_read_status(void)
{
	uint8_t read_status = 0,send_buff[1];
	
	send_buff[0] = REG_READ_STATUS;
	spi_send_then_recv(&ee_25xx_spi_dev,send_buff,1,&read_status,1);
	
	return read_status;
}

static uint8_t ee_25xx_wait_busy(void) 
{
	uint32_t i;
	
	i = 0xFFFFF;
	while (i--);
		
	return 0;
}

/*test*/
#if USE_25XX_DEBUG
void ee_25xx_test(void)
{
	uint8_t write_buff[255] = {0},read_buff[255] = {0},i;
		
	for(i = 0;i < 0xff;i++)
		write_buff[i] = i;
	ee_25xx_read_bytes(255,read_buff,255);
	ee_25xx_write_bytes(255,write_buff,255);
	memset(read_buff,0,255);
	ee_25xx_read_bytes(255,read_buff,255);
	for(i = 0;i < 0xff;i++)
		write_buff[i] = 255-i;
	//i=ee_25xx_read_status();
	ee_25xx_write_bytes(255,write_buff,255);
	ee_25xx_read_bytes(255,read_buff,255);
		
}
#endif
