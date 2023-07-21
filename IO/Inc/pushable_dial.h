/**
  ******************************************************************************
  * @file    pushable_dial.h
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

#ifndef __PUSHABLE_DIAL_H
#define __PUSHABLE_DIAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "dial.h"
#include "binary_push_key.h"

enum PushableDialLastUpdatedItem;
struct PushableDial_Internal;
struct PushableDial;

typedef enum PushableDialLastUpdatedItem {
  ReleasedDial = 1,
  PressedDial = 2,
  PushKey = 3,
} PushableDialLastUpdatedItem;

typedef struct PushableDial_Internal {
  void *Parent;                                   // Parent
  PushableDialLastUpdatedItem LastUpdatedItem;    // Last updated item

  /**
   * @brief Tick callback of the dial for released mode (parent)
   * 
   * @param sender PushableDial that trigger the callback.
   * @param direction Tick direction. Positive for CW. Negative for CCW.
   * @retval None
  */
  void (*OnReleasedDialTicked)(struct PushableDial *sender, int8_t direction);
  
  /**
   * @brief Tick callback of the dial for pressed mode (parent)
   * 
   * @param sender PushableDial that trigger the callback.
   * @param direction Tick direction. Positive for CW. Negative for CCW.
   * @retval None
  */
  void (*OnPressedDialTicked)(struct PushableDial *sender, int8_t direction);

  /**
   * @brief State changed callback of the key (parent)
   * 
   * @param sender PushableDial that trigger the callback.
   * @param state state
   * @param isDialTicked Whether the dial has ticked after the last key state change.
   * @retval None
  */
  void (*OnPushKeyStateChanged)(struct PushableDial *sender, BinaryPushKeyState state, uint8_t isDialTicked);
} PushableDial_Internal;

/**
 * @brief PushableDial Structure definition
 * 
 * @note Used to managed a pushable dial.
*/
typedef struct PushableDial {
  PushableDial_Internal Internal;                 // For internal usage
  Dial *ReleasedDial;                             // Dial for released mode
  Dial *PressedDial;                              // Dial for pressed mode
  BinaryPushKey *PushKey;                         // PushKey to press/release
  
  /**
   * @brief Tick callback of the dial for released mode
   * 
   * @param sender PushableDial that trigger the callback.
   * @param direction Tick direction. Positive for CW. Negative for CCW.
   * @retval Whether the event has been handled. 
   * Once the event has been handled, it will not been sent to its parent.
  */
  uint8_t (*OnReleasedDialTicked)(struct PushableDial *sender, int8_t direction);
  
  /**
   * @brief Tick callback of the dial for pressed mode
   * 
   * @param sender PushableDial that trigger the callback.
   * @param direction Tick direction. Positive for CW. Negative for CCW.
   * @retval Whether the event has been handled. 
   * Once the event has been handled, it will not been sent to its parent.
  */
  uint8_t (*OnPressedDialTicked)(struct PushableDial *sender, int8_t direction);

  /**
   * @brief State changed callback of the key
   * 
   * @param sender PushableDial that trigger the callback.
   * @param state state
   * @param isDialTicked Whether the dial has ticked after the last key state change.
   * @retval Whether the event has been handled. 
   * Once the event has been handled, it will not been sent to its parent.
  */
  uint8_t (*OnPushKeyStateChanged)(struct PushableDial *sender, BinaryPushKeyState state, uint8_t isDialTicked);
} PushableDial;

/**
 * @brief Initialize an PushableDial
 * 
 * @param pushableDial The dial to be initialized
 * @retval None
*/
void PushableDial_Init(PushableDial *pushableDial);

/**
 * @brief Scan an PushableDial to update its state and potentially trigger a callback.
 * 
 * @param pushableDial The dial to be scanned
 * @retval None
*/
void PushableDial_Scan(PushableDial *pushableDial);

#ifdef __cplusplus
}
#endif

#endif /* __PUSHABLE_Dial_H */
