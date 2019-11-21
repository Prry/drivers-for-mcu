/*
 * File      : i2c_core.c
 * 
 * This file is part of i2c device
 * COPYRIGHT (C) 2016
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-01-10     Acuity      first version.
 */
 
#include "i2c_core.h"

int i2c_bus_xfer(struct i2c_dev_device *dev,struct i2c_dev_message msgs[],unsigned int num)
{
    int size;
	
    size = dev->xfer(dev,msgs,num);
    return size;
}
