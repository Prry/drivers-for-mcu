#ifndef			_SPI_H_
#define			_SPI_H_
#include "stm32f10x.h"

//#define USE_SPI_BUS_BITOPS
//#define USE_SPI0_BITOPS
//#define USE_SPI1_BITOPS

/*spi device struct*/
struct spi_dev_message
{
    const void 		*send_buf;
    void 			*recv_buf;
    int 			length;
	unsigned char 	cs_take    : 1;
    unsigned char	cs_release : 1;
};

struct spi_dev_device
{	
	void 	 				(*spi_cs)(unsigned char state);	
	struct spi_bus_device	*spi_bus;	
};
struct spi_bus_device
{	
	int 			(*spi_bus_xfer)(struct spi_dev_device *spi_bus,struct spi_dev_message *msg);
	void			*spi_phy;
	unsigned char 	data_width;
};

/*extern function*/
extern int spi_send_then_recv(struct spi_dev_device *spi_dev,const void *send_buff,unsigned short send_size,void *recv_buff,unsigned short recv_size);
extern int spi_send_then_send(struct spi_dev_device *spi_dev,const void *send_buff1,unsigned short send_size1,const void *send_buff2,unsigned short send_size2);
extern int spi_send_recv(struct spi_dev_device *spi_dev,const void *send_buff,void *recv_buff,unsigned short data_size);
extern int spi_send(struct spi_dev_device *spi_dev,const void *send_buff,unsigned short send_size);

#endif 
