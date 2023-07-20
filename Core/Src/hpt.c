/**
  ******************************************************************************
  * @file    hpt.c
  * @brief   This file provides code for High Precision Timing with HAL and SysTick timer
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

#include "hpt.h"

extern __IO uint32_t uwTick;
extern HAL_TickFreqTypeDef uwTickFreq;

/**
 * @brief Get the milliseconds from system initialization to the present moment
 * 
 * @return milliseconds
 * 
*/
uint32_t HPT_GetMs()
{
	return uwTick;
}

/**
 * @brief Get the microseconds from system initialization to the present moment
 * 
 * @return microseconds
 * 
*/
uint64_t HPT_GetUs()
{
	uint32_t tickMs, subTickUs;

  // read both tick and sub-tick part. Re-read if SysTick->VAL reloaded during the process.
	do {
		tickMs = uwTick;
		subTickUs = (1000U * uwTickFreq) * (SysTick->LOAD - SysTick->VAL) / (SysTick->LOAD + 1);
	}
	while (tickMs != uwTick);	

	return tickMs * 1000U + subTickUs;
}

/**
 * @brief Delay by milliseconds
 * 
 * @param us milliseconds
 * 
*/
void HPT_DelayMs(uint32_t ms)
{
	uint64_t startingMs = HPT_GetMs();
	while (HPT_DeltaMs(startingMs, HPT_GetMs()) < ms);
}

/**
 * @brief Delay by microseconds
 * 
 * @param us microseconds
 * 
*/
void HPT_DelayUs(uint64_t us)
{
	uint64_t startingUs = HPT_GetUs();
  while (HPT_DeltaUs(startingUs, HPT_GetUs()) < us);
}

/**
 * @brief Calculate time delta in milliseconds. 
 * Can handle a 32-bit millisecond timer overflow once.
 * 
 * @param from from in milliseconds
 * @param to to in milliseconds
 * 
 * @return time delta in milliseconds
 * 
*/
uint32_t HPT_DeltaMs(uint32_t from, uint32_t to)
{
  if (to >= from) {
    return to - from;
  } else {
    return (UINT32_MAX - from) + to;
  }
}

/**
 * @brief Calculate time delta in microseconds. 
 * Can handle a 32-bit millisecond timer overflow once.
 * 
 * @param from from in microseconds
 * @param to to in microseconds
 * 
 * @return time delta in microseconds
 * 
*/
uint64_t HPT_DeltaUs(uint64_t from, uint64_t to)
{
  if (to >= from) {
    return to - from;
  } else {
    return ((uint64_t)UINT32_MAX * 1000 - from) + to;
  }
}
