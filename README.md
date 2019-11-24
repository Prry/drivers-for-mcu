MCU常用外设驱动程序
=

1.测试硬件平台
-
STM32F1

2.编译器
-
Keil、IAR

3.源码
-
3.1 接口总线

总线 | 类型 | 备注
-- | --- | ---
I2C |	模拟I2C | 7bit地址
SPI |	硬件SPI/模拟SPI |	支持4种类型/非标SPI
Onewire | 模拟单总线 | IO模拟

3.2 外设驱动  


外设 | 型号 | 原厂 | 接口 | 备注  
--- | --- | --- | --- | --- 
温度传感器 | LM75A	| NXP	| I2C	  
LED恒流驱动 | LP55231 | TI | I2C  	
EEPROM/FRAM | AT24Cxx | ATMEL | I2C  	
EEPROM | ST25AAXX | ST | SPI  	
ADC | AD7684 | ADI | SPI | 3线SPI    
步进电机驱动 | TMC2660 | TRINAMIC | SPI
温度传感器 | DS18B20 | MAXIM | Onewire|    
DAC | AD5290 | ADI | SPI | 3线SPI   	

4.更新
-
不定期更新！
