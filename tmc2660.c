#include  "tmc2660.h"

#define USE_TMC2660 1
#if USE_TMC2660

//电机spi控制设备
static struct spi_dev_device	tmc2660_spi_dev[1];
static struct spi_bus_device 	spi_bus0;

//默认配置
#define		SCG_DEFAULT			0x10000

static void spi0_cs(unsigned char state)
{
    if (state)
        GPIO_SetBits(TMC2660_PORTY_CS, TMC2660_GPIOY_CS);
    else
        GPIO_ResetBits(TMC2660_PORTY_CS, TMC2660_GPIOY_CS);
}

//tmc2660 spi
static u32 tmc2660_spi_xfer(u8 spi_no,u32 write_data)
{
    u8 send_buff[3],recv_buff[3];
    u32 recv_data= 0;
	
    send_buff[0] = (write_data>>16)&0xff;
    send_buff[1] = (write_data>>8)&0xff;
    send_buff[2] = (write_data&0xff);
    spi_send_recv(struct spi_dev_device * spi_dev, const void * send_buff, void * recv_buff, unsigned short data_size)(&tmc2660_spi_dev[spi_no],send_buff,recv_buff,3);
    recv_data = recv_buff[0]<<16 | recv_buff[1]<<8 | recv_buff[2];
		
    return (recv_data&0x0fffff);
}

//tmc2660 init
void tmc2660_init(module_param_t *motor_param,u8 motor_index)
{		
	GPIO_InitTypeDef GPIO_InitStructure;
	
	if(motor_index == YMOTOR)
	{
		//spi io
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC,ENABLE);
		//spi cs PA4
		GPIO_InitStructure.GPIO_Pin = TMC2660_GPIOY_CS;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(TMC2660_PORTY_CS, &GPIO_InitStructure);
		//device init
		stm32f1xx_spi_init(&spi_bus0,8,0,0);   //调用spi_core.h函数
		tmc2660_spi_dev[0].spi_cs = spi0_cs;
		tmc2660_spi_dev[0].spi_bus = &spi_bus0;
	
		//enable 
		GPIO_InitStructure.GPIO_Pin = TMC2660_GPIOY_EN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     				
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		    		
		GPIO_Init(TMC2660_PORTY_EN,&GPIO_InitStructure);
		//step	
		GPIO_InitStructure.GPIO_Pin = TMC2660_GPIOY_STEP;										
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     			//推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		    		
		GPIO_Init(TMC2660_PORTY_STEP,&GPIO_InitStructure);
		//dir  
		GPIO_InitStructure.GPIO_Pin = TMC2660_GPIOY_DIR;										
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     				
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		    		
		GPIO_Init(TMC2660_PORTY_DIR,&GPIO_InitStructure);
						
		//原点光电开关 PB11
		GPIO_InitStructure.GPIO_Pin =  SenseY_GPIO;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;							//上拉输入
		GPIO_Init(SenseY_PORT, &GPIO_InitStructure);
				
		TMC2660_OUTY_CS 	= 1;
		TMC2660_OUTY_EN 	= 0;																		//enable
		TMC2660_OUTY_DIR 	= 0;																		//正反转控制
				
		if(motor_param->max_cs)
			tmc2660_spi_xfer(YMOTOR,REG_DRVCONF | 0X0010); 
		else
			tmc2660_spi_xfer(YMOTOR,REG_DRVCONF | 0X0050); //0X0010->305mV  0X0050->165mV 与电机电流相关
		tmc2660_spi_xfer(YMOTOR,REG_DRVCTRL | MICROSTEP_16);
		tmc2660_spi_xfer(YMOTOR,0x901b4);	//0x94557
		tmc2660_spi_xfer(YMOTOR,0xa0202);	//0xa0202->1/2CS 0xa8202->1/4CS 
		tmc2660_spi_xfer(YMOTOR,REG_SGCSCONF | SCG_DEFAULT | 0x00);		//后5位为电流大小
		tmc2660_set_force(motor_param->freeforce_group[0],YMOTOR);
		tmc2660_set_subdivide(motor_param->subdivide,YMOTOR);
	}
	else if(motor_index == ZMOTOR)
	{	
		
	}
}

/***********************************************************
 * 函数名：tmc2660_move_onestep
 * 功能  ：电机控制端口初始化		  
 * 输入  : 无
 * 输出  ：无    
 ***********************************************************/
void tmc2660_move_onestep(u8 direct,u8 motor_index)
{
	if(motor_index == YMOTOR)
	{
		TMC2660_OUTY_DIR = direct;
	}
	else if(motor_index == ZMOTOR)
	{
		TMC2660_MOTORZ_DIR = direct;
	}
}

//设定电机力矩
void tmc2660_set_force(u8 force,u8 motor_index)
{
	u8 temp;
	int cmd = 0;
	
	temp = force/8;
	cmd = REG_SGCSCONF | SCG_DEFAULT | temp;
	tmc2660_spi_xfer(motor_index,cmd);
}

//获取原点位
char tmc2660_light_sw_state(u8 motor_index)
{
	u8 status = 0;
	if(motor_index == YMOTOR) 
	{
		status = Y_SENSE;
	}
	else if(motor_index == ZMOTOR)
	{	
			
	}
	return status;
}

//设置tmc2660细分
void tmc2660_set_subdivide(char mode,u8 motor_index)
{
	u8 	step = 0;
	u32 cmd = 0;
	
	switch(mode)
	{
		case 0:
			step = MICROSTEP_1;
		break;
		case 1:
			step = MICROSTEP_2;
		break;
		case 2:
			step = MICROSTEP_4;
		break;
		case 3:
			step = MICROSTEP_8;
		break;
		case 4:
			step = MICROSTEP_16;
		break;
		case 5:
			step = MICROSTEP_32;
		break;
		case 6:
			step = MICROSTEP_64;
		break;
		case 7:
			step = MICROSTEP_128;
		break;
		case 8:
			step = MICROSTEP_256;
		break;
		default:
			step = MICROSTEP_16;
		break;
	}
	cmd = REG_DRVCTRL | step;
	tmc2660_spi_xfer(motor_index,cmd);
}
	
//获取tmc2660状态信息
u32 tmc2660_read_state(u8 motor_index)
{
	tmc2660_spi_xfer(motor_index,0x00);
	return 0;
}

#endif
