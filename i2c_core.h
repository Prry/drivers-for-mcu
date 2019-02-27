#ifndef _I2C_CORE_H_
#define	_I2C_CORE_H_

/*i2c bus operate flag*/
#define I2C_BUS_WR            0x0000
#define I2C_BUS_RD            (1u << 0)		
#define I2C_BUS_ADDR_10BIT    (1u << 2)		/* this is a ten bit chip address */
#define I2C_BUS_NO_START      (1u << 4)
#define I2C_BUS_IGNORE_NACK   (1u << 5)
#define I2C_BUS_NO_READ_ACK   (1u << 6)		/* when I2C reading, we do not ACK */

/* the message of i2c device */ 
struct i2c_dev_message
{
    unsigned short  addr;
    unsigned short  flags;
    unsigned short  size;
    unsigned char   *buff;
    unsigned char   retries;
    undinged short  speed;
};

/* i2c device */
struct i2c_dev_device
{
    int (*xfer)(struct i2c_dev_device *dev,struct i2c_dev_message msgs[],unsigned int num);
    void *i2c_phy;
};

/* extern function */
int i2c_bus_xfer(struct i2c_dev_device *dev,struct i2c_dev_message msgs[],unsigned int num); 

#endif
