/*
 * File      : spi_bitops.c
 * 
 * This file is part of spi bus for simulation.
 * COPYRIGHT (C) 
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-09-15	  Acuity       first version.
 */

#include "spi_bitops.h"

#ifdef USE_SPI_BUS_BITOPS

/********************simulate spi start*******************/
int8_t ops_spi_send_byte(struct ops_spi_bus_device *spi_bus,int8_t data)
{/* 8bit */
	char i,recv=0;

	for(i = 0x80;i != 0;i >>= 1)
	{     
		spi_bus->clk(0);	
		spi_bus->sdo((data&i) ? 1 : 0);
		//spi_bus->delayus(1);
		spi_bus->clk(1); 
		//spi_bus->delayus(1);
		if(spi_bus->sdi())
			recv |= i;
	}
	spi_bus->clk(0);
	
	return recv;
}

int16_t ops_spi_send_2byte(struct ops_spi_bus_device*spi_bus,int16_t data)
{/* 16bit todo 未测试 */
	int16_t i,recv=0;

	for(i = 0x8000;i != 0;i >>= 1)
	{     
		spi_bus->clk(0);	
		spi_bus->sdo((data&i) ? 1 : 0);
		//spi_bus->delayus(1);
		spi_bus->clk(1); 
		//spi_bus->delayus(1);
		if(spi_bus->sdi())
			recv |= i;
	}
	spi_bus->clk(0);	
	
	return recv;
}

int ops_spi_bus_xfer(struct spi_dev_device *spi_dev,struct spi_dev_message *msg)
{
	int size;

	size = msg->length;
	if(msg->cs_take)
	{/* take CS */
		spi_dev->spi_cs(0);
	}
	if(spi_dev->spi_bus->data_width <=8)
	{/* 8bit */
		const unsigned char *send_ptr = msg->send_buf;
		unsigned char *recv_ptr = msg->recv_buf;

		while(size--)
		{ 
			unsigned char data = 0xFF;
			if(send_ptr != 0)
			{
				data = *send_ptr++;
			}
			data = ops_spi_send_byte((struct ops_spi_bus_device*)spi_dev->spi_bus->spi_phy,data);
			if(recv_ptr != 0)
			{
				*recv_ptr++ = data;
			}
		}	
	}
	else if(spi_dev->spi_bus->data_width <=16)
	{/* 16bit */
		const unsigned short * send_ptr = msg->send_buf;
		unsigned short *recv_ptr =	msg->recv_buf;

		while(size--)
		{
			unsigned short data = 0xFF;

			if(send_ptr != 0)
			{
				data = *send_ptr++;
			}
			data = ops_spi_send_2byte((struct ops_spi_bus_device*)spi_dev->spi_bus->spi_phy,data);
			if(recv_ptr != 0)
			{
				*recv_ptr++ = data;
			}
		}
	}
	if(msg->cs_release)
	{/* release CS */ 
		spi_dev->spi_cs(1);
	}
	
	return msg->length;
}
/********************simulate spi end*******************/

#endif
