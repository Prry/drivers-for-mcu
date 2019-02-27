/*
 * File      : spi_hw.c
 * 
 * This file is part of spi for stm32f1xx
 * COPYRIGHT (C) 2016-
 *
 * Change Logs:
 * Date           Author       Notes
 * 2016-12-22     Acuity      first version.
 * 2017-01-06     Acuity      Add spi bus init function.
 */
#include "spi_hw.h"

/********************eg: stm32f1 spi start*******************/
/**
  * @brief  spi xfer.
  * @param  spi device,spi message.
  * @retval None
  */
static int stm32_spi_bus_xfer(struct spi_dev_device *spi_dev,struct spi_dev_message *msg)
{
	int size;
	SPI_TypeDef		*SPI_NO;
	
	SPI_NO = (SPI_TypeDef	*)spi_dev->spi_bus->spi_phy;
	size = msg->length;
	if(msg->cs_take)
	{// take CS 
		spi_dev->spi_cs(0);
	}
	if(spi_dev->spi_bus->data_width <=8)
	{//8bit
		const unsigned char *send_ptr = msg->send_buf;
        unsigned char *recv_ptr = msg->recv_buf;
				
        while(size--)
        { 
             unsigned char data = 0xFF;
             if(send_ptr != 0)
             {
                  data = *send_ptr++;
             }
             while (SPI_I2S_GetFlagStatus(SPI_NO, SPI_I2S_FLAG_TXE) == RESET);//Wait until the transmit buffer is empty
             SPI_I2S_SendData(SPI_NO, data); // Send the byte
             while (SPI_I2S_GetFlagStatus(SPI_NO, SPI_I2S_FLAG_RXNE) == RESET);//Wait until a data is received
             data = SPI_I2S_ReceiveData(SPI_NO); // Get the received data

             if(recv_ptr != 0)
             {
                *recv_ptr++ = data;
             }
        }	
	}
	else if(spi_dev->spi_bus->data_width <=16)
	{//16bit
		const unsigned short * send_ptr = msg->send_buf;
		unsigned short *recv_ptr =	msg->recv_buf;
			
		while(size--)
		{
			unsigned short data = 0xFF;

			if(send_ptr != 0)
			{
				data = *send_ptr++;
			}
			while (SPI_I2S_GetFlagStatus(SPI_NO, SPI_I2S_FLAG_TXE) == RESET); 
					SPI_I2S_SendData(SPI_NO, data);
			while (SPI_I2S_GetFlagStatus(SPI_NO, SPI_I2S_FLAG_RXNE) == RESET); 
					data = SPI_I2S_ReceiveData(SPI_NO); 
			if(recv_ptr != 0)
			{
				*recv_ptr++ = data;
			}
		}
	}
	if(msg->cs_release)
	{// release CS 
		spi_dev->spi_cs(1);
	}
	return msg->length;
}	

/**
  * @brief  stm32f1xx_spi_init.
  * @param  \spi_num the spi bus channel, @arg 1 or 2
	*					\speed spi bus transfer speed, @arg SPI_BaudRatePrescaler_2,SPI_BaudRatePrescaler_4
	*					\data_widt the data width of spi bus transfer, @arg SPI_DataSize_8b,SPI_DataSize_16b 
  * @retval None
  */
void stm32f1xx_spi_init(struct spi_bus_device *spi0,unsigned char byte_size0,
												struct spi_bus_device *spi1,unsigned char byte_size1)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;	
	
	if(spi0)
	{//SPI1配置
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
		if(byte_size0 <= 8)
			SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		
		else
			SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;						
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;							//软件控制NSS
		SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;					//高位在前
		SPI_InitStructure.SPI_CRCPolynomial = 7;
		SPI_Init(SPI1, &SPI_InitStructure);
		SPI_Cmd(SPI1, ENABLE);	
		//spi io
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO,ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		spi0->data_width 	= byte_size0;
		spi0->spi_bus_xfer 	= stm32_spi_bus_xfer;
		spi0->spi_phy		= SPI1;
	}
	if(spi1)
	{//SPI2配置
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//全双工
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;						//主模式
		if(byte_size1 <= 8)
			SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		
		else
			SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;				  							
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;					  						
		SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; //波特率
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;					//高位在前
		SPI_InitStructure.SPI_CRCPolynomial = 7;
		SPI_Init(SPI2, &SPI_InitStructure);								
		SPI_Cmd(SPI2, ENABLE);	
		//spi io
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		spi1->data_width 	= byte_size1;
		spi1->spi_bus_xfer 	= stm32_spi_bus_xfer;
		spi1->spi_phy 		= SPI2;
	}
}
/********************eg: stm32f1 spi end*******************/
