#ifndef __DHT20_H__
#define __DHT20_H__

#include "stm32f1xx_hal.h"

// AHT20重置寄存器
void AHT20_ResetRegister(uint8_t addr);

// 初始化AHT20
void AHT20_Init();

// 获取温度和湿度
void AHT20_Read(float *Temperature, float *Humidity);

#endif
