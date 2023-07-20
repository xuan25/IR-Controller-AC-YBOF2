/**
  ******************************************************************************
  * @file    hpt.h
  * @brief   This file contains all the function prototypes for
  *          the hpt.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 Xuan25.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef __HPT_H__
#define __HPT_H__

#include "stm32f1xx_hal.h"

uint32_t HPT_GetMs();
uint64_t HPT_GetUs();
void HPT_DelayUs(uint64_t us);
void HPT_DelayMs(uint32_t ms);
uint32_t HPT_DeltaMs(uint32_t from, uint32_t to);
uint64_t HPT_DeltaUs(uint64_t from, uint64_t to);

#endif
