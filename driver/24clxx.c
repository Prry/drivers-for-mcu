/*
 * File      : 24clxx.c
 * 
 * This file is part of eeprom driver for 24clxx,such as at24c02/at24c16.
 * COPYRIGHT (C) 2016
 *
 * Change Logs:
 * Date           Author       Notes
 * 2016-10-22     Acuity	first version.
 * 2017-03-27     Acuity	Add 16bit address for eeprom,to test at24c64. 
 * 2017-05-09     Acuity	修改页写算法.
 *
 * Depend on:
 * i2c_core.c,i2c_hw.c
 *
 * Note:
 * 24c04 24c08 24c16 地址为8位，超过0xff地址后，注意页的选择读写
 * 24c32 及以上地址为16位
 * eeprm 页写需要作应答查询或作短暂延时以等待数据写入完成；fm24CLXX则不需要，因为写入速度快
 */
 
#define USE_24CLXX_DEBUG 	0

#include <string.h>
#include "i2c_hw.h"
#include "24clxx.h"	

/* eeprom/fram 参数 */
#define	EEPROM_TYPE			(0)		/* 0->EEPROM 1->FRAM */
#define	EEPROM_MODEL		(16u)	/* EEPROM类型:24c16 */
#define EE24CLXX_SLAVE_ADDR (0x50)	/* 注意读写位，实际地址为0x50 */
#define EE24CLXX_PAGESIZE   (16u)	/* at24C16每页有16个字节 */   

/* i2c bus device */
struct i2c_dev_device *ee_i2c_dev = NULL;

/* 页写延时，fram不用 */
static void i2c_24clxx_wait(void) 
{
#if EEPROM_TYPE
	
#else
	uint16_t i = 0xffff;
	
	while (i--);
#endif
}

/**
  * @brief  写多字节，确保当前写地址+写入数据长度不能超过eeprom的一页
  * @param  
  * @retval none
  */
static char i2c_24clxx_write(uint16_t write_addr, char *pbuff, uint16_t write_size)
{
	struct i2c_dev_message ee24_msg[2];
	uint8_t	buf[2] = {0};
	uint8_t slave_addr = 0;

	if(write_addr+write_size > EE24CLXX_PAGESIZE)
	{
		return -1;
	}
	if (EEPROM_MODEL > 16)
	{/* 容量大于2k字节时，为16位寻址;24c32 - 24c1024 */
		slave_addr = EE24CLXX_SLAVE_ADDR;
		buf[0] = (write_addr >>8)& 0xff;
		buf[1] = write_addr & 0xff;
		ee24_msg[0].size  = 2;
	}
	else
	{/* 小容量;24c01 - 24c16 */
		slave_addr = EE24CLXX_SLAVE_ADDR | (write_addr>>8);
		buf[0] = write_addr & 0xff;
		ee24_msg[0].size  = 1;
	}
	ee24_msg[0].addr = slave_addr;
	ee24_msg[0].flags = I2C_BUS_WR;
	ee24_msg[0].buff  = buf;
	ee24_msg[1].addr = slave_addr;
	ee24_msg[1].flags = I2C_BUS_WR  | I2C_BUS_NO_START;
	ee24_msg[1].buff  = (uint8_t*)pbuff;
	ee24_msg[1].size  = write_size;
	i2c_bus_xfer(ee_i2c_dev, ee24_msg, 2);
	
	return write_size;
}

/**
  * @brief 初始化
  * @param i2c总线设备
  * @retval none
  */
int8_t ee_24clxx_init(struct i2c_dev_device *i2c_bus)
{
	if (NULL == i2c_bus)	
	{
		return -1;
	}
	ee_i2c_dev = i2c_bus;
	
	return 0;
}

/**
  * @brief 写1字节
  * @param  
  * @retval none
  */
char ee_24clxx_writebyte(uint16_t write_addr, uint8_t data)
{
	struct i2c_dev_message ee24_msg[1];
	uint8_t buf[3] = {0};
	uint8_t slave_addr = 0;
	
	if(EEPROM_MODEL > 16)
	{
		slave_addr = EE24CLXX_SLAVE_ADDR;
		buf[0] = (write_addr >>8)& 0xff;   /* 高位地址在前 */
		buf[1] = write_addr & 0xff;
		buf[2] = data;
		ee24_msg[0].size  = 3;
	}
	else
	{
		slave_addr = EE24CLXX_SLAVE_ADDR | (write_addr>>8);
		buf[0] = write_addr & 0xff;
		buf[1] = data;
		ee24_msg[0].size  = 2;
	}
	ee24_msg[0].addr = slave_addr;
	ee24_msg[0].flags = I2C_BUS_WR;
	ee24_msg[0].buff  = buf;
	i2c_bus_xfer(ee_i2c_dev,ee24_msg,1);	
	
	return 0;
}

/**
  * @brief 任意地址开始写多字节，带页写算法
  * @param  
  * @retval none
  */
char ee_24clxx_writebytes(uint16_t write_addr, char *pbuff, uint16_t write_size)
{
	char error = 0;
	uint8_t write_len = 0;
	uint8_t page_offset = 0;
	uint16_t check_writes = 0;
	uint16_t check_addr = 0;
	char *check_buf;
	
	check_writes= write_size;
	check_addr  = write_addr;
	check_buf   = pbuff;
	while(write_size > 0)
	{
		page_offset = EE24CLXX_PAGESIZE - (write_addr & (EE24CLXX_PAGESIZE-1));//write_current_addr%EE24CLXX_PageSize
		write_len = write_size > page_offset ? page_offset : write_size;
		i2c_24clxx_write(write_addr, pbuff, write_len);
		write_size   = write_size - write_len;
		if(write_size > 0)
		{
			pbuff = pbuff + write_len;
			write_addr  = write_addr + write_len;
			i2c_24clxx_wait();
		}
	}
	i2c_24clxx_wait();
	{/* 校验数据(非必要) */
		int i;
		char checkdata;
			
		for(i = 0;i < check_writes;i++)
		{
			ee_24clxx_readbytes(check_addr+i,&checkdata,1);	
			if(checkdata != check_buf[i])
			{
				error = -1;
				break;
			}
		}
	}
	
	return error;
}

/**
  * @brief 任意地址开始读多字节
  * @param  
  * @retval none
  */
void ee_24clxx_readbytes(uint16_t read_addr, char* pbuff, uint16_t read_size)
{  
	struct i2c_dev_message ee24_msg[2];
	uint8_t	buf[2] = {0};
	uint8_t slave_addr = 0;
	
	if(EEPROM_MODEL > 16)
	{
		slave_addr = EE24CLXX_SLAVE_ADDR;
		buf[0] = (read_addr >>8)& 0xff;
		buf[1] = read_addr & 0xff;
		ee24_msg[0].size  = 2;
	}
	else
	{
		slave_addr = EE24CLXX_SLAVE_ADDR | (read_addr>>8);
		buf[0] = read_addr & 0xff;
		ee24_msg[0].size  = 1;
	}
	ee24_msg[0].buff  = buf;
	ee24_msg[0].addr  = slave_addr;
	ee24_msg[0].flags = I2C_BUS_WR;
	ee24_msg[1].addr  = slave_addr;
	ee24_msg[1].flags = I2C_BUS_RD;
	ee24_msg[1].buff  = (uint8_t*)pbuff;
	ee24_msg[1].size  = read_size;
	i2c_bus_xfer(ee_i2c_dev,ee24_msg,2);
}

/**
  * @brief 擦除eeprom
  * @param  
  * @retval none
  */
char ee_24clxx_erasebytes(uint16_t erase_addr, char erase_data, uint16_t erase_size)
{
	char error = 0;
	uint16_t i = 0;
#if 1
	char buff[2048]; /* 测试用，实际使用注意堆栈溢出，建议用堆内存 */
#else
	char *buff = NULL;
#endif

#if 0	
	buff = (char*)malloc(2048);
#endif
	for(i = 0;i < erase_size;i++)
	{
		buff[i] = erase_size;
	}
	error = ee_24clxx_writebytes(erase_addr, buff, erase_size);	
#if 0		
	free(buff);
#endif
	return error;
}

/* eeprom test */
#if USE_24CLXX_DEBUG
#define	 WR_SIZE   2048
#define  WR_ADDR   0
char w_buff[WR_SIZE];
char r_buff[WR_SIZE];
void ee24clxx_test(void)
{
	uint16_t i = 0;
	char error = 0;
			
	for (i=0; i<WR_SIZE; i++ ) /* 填充缓冲 */
	{   
		w_buff[i] = i;   
	}
	ee_24clxx_readbytes(WR_ADDR, r_buff, WR_SIZE); 
	ee_24clxx_readbytes(WR_ADDR, r_buff, WR_SIZE);
	error = ee_24clxx_writebytes(WR_ADDR, w_buff, WR_SIZE);	 
	
	ee_24clxx_readbytes(WR_ADDR, r_buff, WR_SIZE); 

	for ( i=0; i<WR_SIZE; i++ )  
	{
		w_buff[i] = 0;   
	}
	error = ee_24clxx_writebytes(WR_ADDR, w_buff, WR_SIZE);
	
	ee_24clxx_readbytes(WR_ADDR, r_buff, WR_SIZE); 
}
#endif
