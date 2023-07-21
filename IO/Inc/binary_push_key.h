/**
  ******************************************************************************
  * @file    binary_push_key.h
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

#ifndef __BINARY_PUSH_KEY_H
#define __BINARY_PUSH_KEY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "key.h"

enum BinaryPushKeyState;
struct BinaryPushKey_Internal;
struct BinaryPushKey;

typedef enum BinaryPushKeyState {
  PushKeyReleased = 0,
  PushKeyPressed = 1,
} BinaryPushKeyState;

typedef struct BinaryPushKey_Internal {
  void *Parent;                 // Parent

  /**
   * @brief State changed callback of the key (parent)
   * 
   * @param sender Key that trigger the callback.
   * @param state state.
   * @retval None
  */
  void (*OnStateChanged)(struct BinaryPushKey *sender, BinaryPushKeyState state);
} BinaryPushKey_Internal;

/**
 * @brief BinaryPushKey Structure definition
 * 
 * @note Used to managed a key hardware that directed connect to a GPIO.
*/
typedef struct BinaryPushKey {
  BinaryPushKey_Internal Internal;  // For internal usage
  Key *Key;                         // Key structure
  GPIO_Pin *Pin;                    // GPIO Pin
  GPIO_PinState ReleasedLevel;      // GPIO state when key released

  /**
   * @brief State changed callback of the key
   * 
   * @param sender Key that trigger the callback.
   * @param state state.
   * @retval Whether the event has been handled. 
   * Once the event has been handled, it will not been sent to its parent.
  */
  uint8_t (*OnStateChanged)(struct BinaryPushKey *sender, BinaryPushKeyState state);
} BinaryPushKey;

/**
 * @brief Initialize a BinaryPushKey
 * 
 * @param binaryPushKey The BinaryPushKey to be initialized
 * @retval None
*/
void BinaryPushKey_Init(BinaryPushKey *binaryPushKey);

/**
 * @brief Scan an BinaryPushKey hardware to update its state and potentially 
 * trigger a callback.
 * 
 * @param binaryPushKey The BinaryPushKey to be scanned
 * @retval None
*/
void BinaryPushKey_Scan(BinaryPushKey *binaryPushKey);

#ifdef __cplusplus
}
#endif

#endif /* __BINARY_PUSH_KEY_H */
