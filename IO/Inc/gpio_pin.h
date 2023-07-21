/**
  ******************************************************************************
  * @file    gpio_pin.h
  * @brief   This file contains the structure definition of GPIO_Pin
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

#ifndef __GPIO_PIN_H
#define __GPIO_PIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

/**
 * @brief GPIO_Pin Structure definition
 * 
 * @note Used to define a GPIO pin
*/
typedef struct GPIO_Pin {
  GPIO_TypeDef *GPIOx;
  uint16_t GPIO_Pin;
} GPIO_Pin;

#ifdef __cplusplus
}
#endif

#endif /* __GPIO_PIN_H */
