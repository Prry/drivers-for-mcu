#ifndef _SPI_BITOPS_H_
#include "stm32f10x.h"                  
#include "spi_core.h"

/* 模拟spi 结构体 */
struct ops_spi_bus_device
{
	void (*sdo)(int8_t state);
	int8_t (*sdi)(void);
	void (*clk)(int8_t state);
	void (*delayus)(uint32_t us); 
};

/* 模拟spi收发函数 */
extern int ops_spi_bus_xfer(struct spi_dev_device *spi_bus,struct spi_dev_message *msg);

/* stm32f1 模拟spi */
extern void st_ops_spi0_init(struct ops_spi_bus_device *ops_spi_bus);
extern void st_ops_spi1_init(struct ops_spi_bus_device *ops_spi_bus);

#endif
