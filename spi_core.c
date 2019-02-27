/*
 * File: spi_core.c
 * 
 * This file is part of spi core
 * COPYRIGHT (C) 
 *
 * Change Logs:
 * Date           Author       Notes
 * 2016-12-22     Acuity        first version.
 * 2017-09-25     Acuity        修改spi设备,片选(cs)改为函数控制.
 */
 
#define USE_SPI_EN 	1
#if USE_SPI_EN

#include "spi_core.h"

/********************spi extern fun start*******************/
/**
  * @brief  spi send and then recvice.
  * @param  spi device,send data and data size,recvice data and data size.
  * @retval None
  */
int spi_send_then_recv(struct spi_dev_device *spi_dev,const void *send_buff,unsigned short send_size,void *recv_buff,unsigned short recv_size)
{
struct spi_dev_message message;
	
    message.length      = send_size;
    message.send_buf    = send_buff;
    message.recv_buf    = 0;
    message.cs_take    	= 1;
    message.cs_release 	= 0;
    spi_dev->spi_bus->spi_bus_xfer(spi_dev,&message);
	
    message.length      = recv_size;
    message.send_buf    = 0;
    message.recv_buf    = recv_buff;
    message.cs_take    	= 0;
    message.cs_release 	= 1;
    spi_dev->spi_bus->spi_bus_xfer(spi_dev,&message);
	return 0;
}

/**
  * @brief  spi send and then send
  * @param  spi device,send data and data size.
  * @retval None
  */
int spi_send_then_send(struct spi_dev_device *spi_dev,const void *send_buff1,unsigned short send_size1,const void *send_buff2,unsigned short send_size2)
{
	struct spi_dev_message message;
	
    message.length 		= send_size1;
    message.send_buf    = send_buff1;
    message.recv_buf    = 0;
    message.cs_take    	= 1;
    message.cs_release 	= 0;
    spi_dev->spi_bus->spi_bus_xfer(spi_dev,&message);

    message.length 		= send_size2;
    message.send_buf    = send_buff2;
    message.recv_buf    = 0;
    message.cs_take    	= 0;
    message.cs_release 	= 1;
    spi_dev->spi_bus->spi_bus_xfer(spi_dev,&message);
    return 0;
}

/**
  * @brief  spi send and receive immediately.
  * @param  spi device,send data and data size.
  * @retval None
  */
int spi_send_recv(struct spi_dev_device *spi_dev,const void *send_buff,void *recv_buff,unsigned short data_size)
{
    struct spi_dev_message message;
	
    message.length 		= data_size;
    message.send_buf    = send_buff;
    message.recv_buf    = recv_buff;
    message.cs_take    	= 1;
    message.cs_release 	= 1;
    spi_dev->spi_bus->spi_bus_xfer(spi_dev,&message);
    return 0;
}

/**
  * @brief  spi send with null
  * @param  spi device,send data and data size.
  * @retval None
  */
int spi_send(struct spi_dev_device *spi_dev,const void *send_buff,unsigned short send_size)
{
    struct spi_dev_message message;
	
    message.length 		= send_size;
    message.send_buf    = send_buff;
    message.recv_buf    = 0;
    message.cs_take    	= 1;
    message.cs_release 	= 1;
    spi_dev->spi_bus->spi_bus_xfer(spi_dev,&message);
    return 0;
}
/********************spi extern fun end*******************/

#endif
