/**
  ******************************************************************************
  * @file    encoder.c
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

#include "encoder.h"
#include "hpt.h"

void Encoder_Init(Encoder *encoder) {
  uint8_t levelA = HAL_GPIO_ReadPin(encoder->PinA->GPIOx, encoder->PinA->GPIO_Pin);
  uint8_t levelB = HAL_GPIO_ReadPin(encoder->PinB->GPIOx, encoder->PinB->GPIO_Pin);
  encoder->Internal.LastStateA = levelA;
  encoder->Internal.LastStateB = levelB;

#if ENCODER_DEBOUNCE_US > 0
  uint64_t tickUs = HPT_GetUs();

  encoder->Internal.LastLevelChangedUs = tickUs;
  encoder->Internal.LastChangedLevelA = levelA;
  encoder->Internal.LastChangedLevelB = levelB;
#endif
}

void Encoder_Scan(Encoder *encoder) {
  uint8_t levelA = HAL_GPIO_ReadPin(encoder->PinA->GPIOx, encoder->PinA->GPIO_Pin);
  uint8_t levelB = HAL_GPIO_ReadPin(encoder->PinB->GPIOx, encoder->PinB->GPIO_Pin);

#if ENCODER_DEBOUNCE_US > 0
  uint64_t tickUs = HPT_GetUs();

  // Level update
  if(encoder->Internal.LastChangedLevelA != levelA) {
    encoder->Internal.LastChangedLevelA = levelA;
    encoder->Internal.LastLevelChangedUs = tickUs;
  }
  if(encoder->Internal.LastChangedLevelB != levelB) {
    encoder->Internal.LastChangedLevelB = levelB;
    encoder->Internal.LastLevelChangedUs = tickUs;
  }

  // State update
  uint8_t stateA = encoder->Internal.LastStateA;
  uint8_t stateB = encoder->Internal.LastStateB;
  if(encoder->Internal.LastStateA != levelA && HPT_DeltaUs(encoder->Internal.LastLevelChangedUs, tickUs) > ENCODER_DEBOUNCE_US) {
    stateA = levelA;
  }
  if(encoder->Internal.LastStateB != levelB && HPT_DeltaUs(encoder->Internal.LastLevelChangedUs, tickUs) > ENCODER_DEBOUNCE_US) {
    stateB = levelB;
  }
#else
  uint8_t stateA = levelA;
  uint8_t stateB = levelB;
#endif

  // Tick check
  int8_t dir = 0, edge = 0;
  if (stateA != encoder->Internal.LastStateA) {
    edge = ENCODER_EDGE_A;
    if (stateA != encoder->OffLevel) {
      if (stateB != encoder->OffLevel) {
        dir--;
      }
      else {
        dir++;
      }
    }
    else {
      if (stateB != encoder->OffLevel) {
        dir++;
      }
      else {
        dir--;
      }
    }
  }
  else if (stateB != encoder->Internal.LastStateB) {
    edge = ENCODER_EDGE_B;
    if (stateB != encoder->OffLevel) {
      if (stateA != encoder->OffLevel) {
        dir++;
      }
      else {
        dir--;
      }
    }
    else{
      if (stateA != encoder->OffLevel) {
        dir--;
      }
      else {
        dir++;
      }
    }
  }
  
  // Struct update
  encoder->Internal.LastStateA = stateA;
  encoder->Internal.LastStateB = stateB;
  
  // callback
  if (dir != 0) {
    if(encoder->OnTicked == NULL || !encoder->OnTicked(encoder, dir, edge)) {
      if(encoder->Internal.OnTicked != NULL) {
        encoder->Internal.OnTicked(encoder, dir, edge);
      }
    }
    
  }
}
