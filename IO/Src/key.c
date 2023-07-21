/**
  ******************************************************************************
  * @file    key.c
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

#include "key.h"
#include "hpt.h"

void Key_Init(Key *key, uint8_t level) {
  key->Internal.LastChangedLevel = level;
  #if KEY_DEBOUNCE_US > 0
    uint64_t tickUs = HPT_GetUs();
    key->Internal.LastLevelChangedUs = tickUs;
  #endif
}

void Key_Update(Key *key, uint8_t level) {
#if KEY_DEBOUNCE_US > 0
  uint64_t tickUs = HPT_GetUs();
  // Level update
  if(key->Internal.LastChangedLevel != level) {
    key->Internal.LastChangedLevel = level;
    key->Internal.LastLevelChangedUs = tickUs;
  }
  // State update
  if(key->Internal.State != level && HPT_DeltaUs(key->Internal.LastLevelChangedUs, tickUs) > KEY_DEBOUNCE_US) {
    uint8_t oldState = key->Internal.State;
    key->Internal.State = level;
    if(key->OnStateChanged == NULL || !key->OnStateChanged(key, oldState, level)) {
      if(key->Internal.OnStateChanged != NULL) {
        key->Internal.OnStateChanged(key, oldState, level);
      }
    }
  }
#else
  // State update
  if(key->Internal.State != level) {
    uint8_t oldState = key->Internal.State;
    key->Internal.State = level;
    if(key->OnStateChanged == NULL || !key->OnStateChanged(key, oldState, level)) {
      if(key->Internal.OnStateChanged != NULL) {
        key->Internal.OnStateChanged(key, oldState, level);
      }
    }
  }
#endif
}
