/**
  ******************************************************************************
  * @file    key.h
  * @brief   
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

#ifndef __KEY_H
#define __KEY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "gpio_pin.h"

struct Key_Internal;
struct Key;

// Low-pass debouncing filter config for key pins. 
// Should be greater than the half of the bouncing period in microseconds
// ((1 / frequency) / 2)
#define KEY_DEBOUNCE_US 10u

typedef struct Key_Internal {
  void *Parent;                     // Parent
  uint8_t State;                    // State for the key
#if KEY_DEBOUNCE_US > 0
  uint64_t LastLevelChangedUs;      // Time of the last level change
  uint8_t LastChangedLevel;         // Last voltage level
#endif

  /**
   * @brief State changed callback of the key (parent)
   * 
   * @param sender Key that trigger the callback.
   * @param oldState Old state.
   * @param newState New state.
   * @retval None
  */
  void (*OnStateChanged)(struct Key *sender, uint8_t oldState, uint8_t newState);
} Key_Internal;

/**
 * @brief Key Structure definition
 * 
 * @note Used to manage a key (any type) hardware.
*/
typedef struct Key {
  Key_Internal Internal;            // For internal usage

  /**
   * @brief State changed callback of the key
   * 
   * @param sender Key that trigger the callback.
   * @param oldState Old state.
   * @param newState New state.
   * @retval Whether the event has been handled. 
   * Once the event has been handled, it will not been sent to its parent.
  */
  uint8_t (*OnStateChanged)(struct Key *sender, uint8_t oldState, uint8_t newState);
} Key;

/**
 * @brief Initialize a BinaryPushKey
 * 
 * @param key: The key to be initialize
 * @param level: Level of the key
 * @retval None
*/
void Key_Init(Key *key, uint8_t level);

/**
 * @brief Update a Key with a new voltage level
 * 
 * @param key: The key to be updated
 * @param level: Level of the key
 * @retval None
*/
void Key_Update(Key *key, uint8_t level);

#ifdef __cplusplus
}
#endif

#endif /* __KEY_H */
