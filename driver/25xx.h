#ifndef _25XX_H_
#define	_25XX_H_

/*25xx register*/
#define		REG_READ_COMMAND		0X03
#define		REG_WRITE_COMMAND		0X02
#define		REG_WRITE_ENABLE		0X06
#define		REG_WRITE_DISABLE		0X04
#define		REG_READ_STATUS			0X05
#define		REG_WRITE_STATUS		0X01

/*extern function*/
extern void ee_25xx_init(void);
extern void ee_25xx_write_bytes(uint16_t write_addr, uint8_t *write_buff, uint16_t write_bytes);
extern void ee_25xx_read_bytes(uint16_t read_addr,uint8_t *read_buff,uint16_t read_bytes);
	
#endif
