/*
 * File      : i2c_bitops.c
 * 
 * This file is part of simulative i2c
 * COPYRIGHT (C) 2014 
 *
 * Change Logs:
 * Date           Author       Notes
 * 2016-06-10     Acuity	first version.
 * 2017-02-20     Acuity	add i2c_bus device.
 */
#include "i2c_bitops.h"

/********************simulate i2c start*******************/
//产生i2c起始信号
static void i2c_bitops_start(struct ops_i2c_dev *i2c_bus)
{
    i2c_bus->set_sda(0); 					//start:when CLK is high,DATA change form high to low 
    i2c_bus->delayus(3);
    i2c_bus->set_scl(0);						
}	 

//重复产生起始信号，一般是连续读写中
static void i2c_bitops_restart(struct ops_i2c_dev *i2c_bus)
{
    i2c_bus->set_sda(1);
    i2c_bus->set_scl(1);
    i2c_bus->delayus(3);
    i2c_bus->set_sda(0);					//restart:when CLK is high,DATA change form high to low 
	i2c_bus->delayus(3);
	i2c_bus->set_scl(0);						
}	

//产生IIC停止信号
static void i2c_bitops_stop(struct ops_i2c_dev *i2c_bus)
{
		i2c_bus->set_sda(0);				//stop:when CLK is high DATA change form low to high
		//i2c_bus->delayus(3);
		i2c_bus->set_scl(1);
		i2c_bus->delayus(3);
		i2c_bus->set_sda(1);								
		i2c_bus->delayus(5);							   	
}

//等待应答信号到来 1，接收应答失败    0，接收应答成功
static char i2c_bitops_wait_ack(struct ops_i2c_dev *i2c_bus)
{
	char  ack = 1;
	
	i2c_bus->set_sda(1);
	i2c_bus->delayus(3);   
	i2c_bus->set_scl(1);
	i2c_bus->delayus(3);		
	ack = i2c_bus->get_sda();
	i2c_bus->set_scl(0);	 
		
	return ack;  
} 

//产生ACK应答
static void i2c_bitops_create_ack(struct ops_i2c_dev *i2c_bus)
{
	i2c_bus->set_sda(0);
	i2c_bus->delayus(2);
	i2c_bus->set_scl(1);
	i2c_bus->delayus(3);
	i2c_bus->set_scl(0);
}

//不产生ACK应答		    
static void i2c_bitops_not_ack(struct ops_i2c_dev *i2c_bus)
{
	i2c_bus->set_sda(1);
	i2c_bus->delayus(2);
	i2c_bus->set_scl(1);
	i2c_bus->delayus(2);
	i2c_bus->set_scl(0);
}	

//IIC发送一个字节 1，有应答；0，无应答			  
static int i2c_bitops_send_byte(struct ops_i2c_dev *i2c_bus,unsigned char data)
{                        
    signed char i; 
     
    for(i=7;i >= 0;i--)
    {        
		i2c_bus->set_scl(0);
		i2c_bus->set_sda((data >> i) & 0x01);
		i2c_bus->delayus(5);
		i2c_bus->set_scl(1);
    }	
	i2c_bus->set_scl(0);
	i2c_bus->delayus(3);
	return (i2c_bitops_wait_ack(i2c_bus));
} 

//读1个字节 
static signed char i2c_bitops_recv_byte(struct ops_i2c_dev *i2c_bus)
{
	signed char i,receive=0;
	
	i2c_bus->set_scl(0);
	i2c_bus->set_sda(1);
	i2c_bus->delayus(3);
    for(i = 0;i < 8;i++ )
	{
		receive <<= 1;
		i2c_bus->set_scl(1);
		i2c_bus->delayus(3);    
      	if(i2c_bus->get_sda())
			receive |= 0x01; 
		i2c_bus->set_scl(0);
		i2c_bus->delayus(5); 
    }					 

    return receive;
} 

//发送从地址，启动i2c总线，可重复
static int i2c_send_address(struct ops_i2c_dev *i2c_bus,unsigned short addr,unsigned char retries)
{
    int ret = 0;
		unsigned char i;
	
    for (i = 0; i <= retries; i++)
    {//如果i2c总线未响应可以重复启动
        ret = i2c_bitops_send_byte(i2c_bus,addr);
        if (ret == 0 || i == retries)
            break;
        i2c_bitops_stop(i2c_bus);
        i2c_bus->delayus(5);
		i2c_bitops_start(i2c_bus);
    }

    return ret;
}

//发送i2c从地址
static int i2c_bitops_send_address(struct ops_i2c_dev *i2c_bus,struct i2c_dev_message  *msg)
{
    unsigned short flags = msg->flags;
    unsigned short ignore_nack = msg->flags & I2C_BUS_IGNORE_NACK;
    unsigned char addr1,addr2;
    int ret;

    if (flags & I2C_BUS_ADDR_10BIT)
    {// 10-bit addr，todo:暂未验证
				addr1 = 0xf0 | ((msg->addr >> 7) & 0x06);
        addr2 = msg->addr & 0xff;

        //ret = i2c_bitops_send_byte(addr1);
				ret = i2c_send_address(i2c_bus,addr1,msg->retries);
        if ((ret != 0) && !ignore_nack)
        {
            return -1;
        }

        //ret = i2c_bitops_send_byte(addr2);
				ret = i2c_send_address(i2c_bus,addr2,msg->retries);
        if ((ret != 0) && !ignore_nack)
        {
            return -1;
        }
        if (flags & I2C_BUS_RD)
        {
            i2c_bitops_start(i2c_bus);
            addr1 |= 0x01;
            //ret = i2c_bitops_send_byte(addr1);
						ret = i2c_send_address(i2c_bus,addr1,msg->retries);
            if ((ret != 0) && !ignore_nack)
            {
                return -1;
            }
        }
    }
    else
    {// 7-bit addr    
        addr1 = msg->addr << 1; //最低位为读写位
        if (flags & I2C_BUS_RD)
            addr1 |= 1;
        //ret = i2c_bitops_send_byte(addr1);
		ret = i2c_send_address(i2c_bus,addr1,msg->retries);
		if ((ret != 0) && !ignore_nack)
			return -1;
    }
    return ret;
}

//i2c bus w
static unsigned long i2c_bitops_bus_write(struct ops_i2c_dev *i2c_bus,struct i2c_dev_message *msg)
{
	unsigned long count = 0,bytes = 0;
	unsigned short ignore_nack;
	unsigned char *ptr;
	signed char data;
	
	count = msg->size;
	ptr   = msg->buff;
	ignore_nack = msg->flags & I2C_BUS_IGNORE_NACK;
	while(count > 0)
	{
		data = i2c_bitops_send_byte(i2c_bus,*ptr);
		if(data == 0 || (ignore_nack && data))
		{
			ptr++;
			bytes++;
			count--;
		}
		else 
			return 0;
	}
	return bytes;		//return write bytes
}

//i2c bus R
static unsigned long i2c_bitops_bus_read(struct ops_i2c_dev *i2c_bus,struct i2c_dev_message *msg)
{
	unsigned long count = 0,bytes = 0;
	unsigned short flags;
	
	unsigned char *ptr,data;
	count = msg->size;
	ptr   = msg->buff;
		
	while(count > 0)
	{
		data = i2c_bitops_recv_byte(i2c_bus);
		//if(data >= 0)
		{
			*ptr = data;
			bytes++;
		}
		//else
		//		break;
		ptr++;
		count--;
		if (!(flags & I2C_BUS_NO_READ_ACK) && count != 0)
			i2c_bitops_create_ack(i2c_bus); //发送ACK
		else if(count ==0)
			i2c_bitops_not_ack(i2c_bus);	//读完必须发送nACK！！！此问题找了很久20160616
	}
	return bytes;	//成功返回读字节数
}

//外部操作函数
int i2c_bitops_bus_xfer(struct ops_i2c_dev *i2c_bus,struct i2c_dev_message msgs[],unsigned long num)
{
	struct i2c_dev_message *msg;
	unsigned long i;
	unsigned short ignore_nack;
	int ret;
	
	ignore_nack = msg->flags & I2C_BUS_IGNORE_NACK;
	i2c_bitops_start(i2c_bus);							//启动信号
	for (i = 0; i < num; i++)
    {
		msg = &msgs[i];
        if (!(msg->flags & I2C_BUS_NO_START))
        {//再次启动i2c总线
            if (i)
            {
                i2c_bitops_restart(i2c_bus);	
            }
			ret = i2c_bitops_send_address(i2c_bus,msg);
			if ((ret != 0) && !ignore_nack)
				goto out;
		}
		if (msg->flags & I2C_BUS_RD)
        {//read
			ret = i2c_bitops_bus_read(i2c_bus,msg);
			if(ret < msg->size)
			{
				ret = -1;
				goto out;
			}
		}
		else
		{//write
			ret = i2c_bitops_bus_write(i2c_bus,msg);
			if(ret < msg->size)
			{
				ret = -1;
				goto out;
			}
		}
	}
	ret = i;
out:
	i2c_bitops_stop(i2c_bus);
	return ret;
}
/********************simulate i2c end*******************/
