#ifndef __DHT20_H__
#define __DHT20_H__

#include "stm32f1xx_hal.h"

void AHT20_ResetRegister(uint8_t addr);

void AHT20_Init();

void AHT20_Measure();

uint8_t AHT20_Read(float *temperature, float *humidity);

#endif
