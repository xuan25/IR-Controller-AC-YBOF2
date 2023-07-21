/**
  ******************************************************************************
  * @file    dial.h
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

#ifndef __DIAL_H
#define __DIAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "encoder.h"

struct Dial_Internal;
struct Dial;

typedef struct Dial_Internal {
  void *Parent;                   // Parent
  uint32_t LastInputTickMs;       // Time of the last hardware tick
  int16_t InputState;             // Internal dial state tracing

  /**
   * @brief Tick callback of the dial (parent)
   * 
   * @param sender Dial that trigger the callback.
   * @param direction Tick direction. Positive for CW. Negative for CCW.
   * @retval None
  */
  void (*OnTicked)(struct Dial *sender, int8_t direction);
} Dial_Internal;

/**
 * @brief Dial Structure definition
 * 
 * @note Used to managed a encoder hardware that act like a dial.
*/
typedef struct Dial {
  Dial_Internal Internal;         // For internal usage
  Encoder *Encoder;               // Encoder struct
  uint16_t TickInterval;          // Specify the ratio between hardware ticks and a dial tick
  uint16_t ResetDelayMs;          // Specify the time for reset delay that auto align the dial to a dial tick
  Encoder_Edge IgnoreInputEdge;   // Specify the pin to ignore its rising/falling edges
  
  /**
   * @brief Tick callback of the dial
   * 
   * @param sender Dial that trigger the callback.
   * @param direction Tick direction. Positive for CW. Negative for CCW.
   * @retval Whether the event has been handled. 
   * Once the event has been handled, it will not been sent to its parent.
  */
  uint8_t (*OnTicked)(struct Dial *sender, int8_t direction);
} Dial;

/**
 * @brief Initialize an dial
 * 
 * @param dial The dial to be initialized
 * @retval None
*/
void Dial_Init(Dial *dial);

/**
 * @brief Scan an dial to update its state and potentially trigger a callback.
 * 
 * @param dial The dial to be scanned
 * @retval None
*/
void Dial_Scan(Dial *dial);

#ifdef __cplusplus
}
#endif

#endif /* __DIAL_H */
