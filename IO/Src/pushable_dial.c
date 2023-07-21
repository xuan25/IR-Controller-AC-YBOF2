/**
  ******************************************************************************
  * @file    pushable_dial.c
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

#include "pushable_dial.h"

void PushableDial_OnReleasedDialTicked(Dial *sender, int8_t direction);
void PushableDial_OnPressedDialTicked(Dial *sender, int8_t direction);
void PushableDial_OnPushKeyStateChanged(BinaryPushKey *sender, BinaryPushKeyState state);

void PushableDial_Init(PushableDial *pushableDial) {
  if(pushableDial->ReleasedDial != NULL) {
    pushableDial->ReleasedDial->Internal.Parent = pushableDial;
    pushableDial->ReleasedDial->Internal.OnTicked = PushableDial_OnReleasedDialTicked;
    Dial_Init(pushableDial->ReleasedDial);
  }
  
  if(pushableDial->ReleasedDial != NULL) {
    pushableDial->PressedDial->Internal.Parent = pushableDial;
    pushableDial->PressedDial->Internal.OnTicked = PushableDial_OnPressedDialTicked;
    Dial_Init(pushableDial->PressedDial);
  }

  pushableDial->PushKey->Internal.Parent = pushableDial;
  pushableDial->PushKey->Internal.OnStateChanged = PushableDial_OnPushKeyStateChanged;
  BinaryPushKey_Init(pushableDial->PushKey);
}

void PushableDial_Scan(PushableDial *pushableDial) {
  Dial_Scan(pushableDial->PressedDial);
  Dial_Scan(pushableDial->ReleasedDial);
  BinaryPushKey_Scan(pushableDial->PushKey);
}

void PushableDial_OnReleasedDialTicked(Dial *sender, int8_t direction) {
  PushableDial *pushableDial = (PushableDial *)sender->Internal.Parent;
  if(pushableDial->PushKey->Key->Internal.State == pushableDial->PushKey->ReleasedLevel) {
    pushableDial->Internal.LastUpdatedItem = ReleasedDial;
    if(pushableDial->OnReleasedDialTicked == NULL || !pushableDial->OnReleasedDialTicked(pushableDial, direction)) {
      if(pushableDial->Internal.OnReleasedDialTicked != NULL) {
        pushableDial->Internal.OnReleasedDialTicked(pushableDial, direction);
      }
    }
  }
}

void PushableDial_OnPressedDialTicked(Dial *sender, int8_t direction){
  PushableDial *pushableDial = (PushableDial *)sender->Internal.Parent;
  if(pushableDial->PushKey->Key->Internal.State != pushableDial->PushKey->ReleasedLevel) {
    pushableDial->Internal.LastUpdatedItem = PressedDial;
    if(pushableDial->OnPressedDialTicked == NULL || !pushableDial->OnPressedDialTicked(pushableDial, direction)) {
      if(pushableDial->Internal.OnPressedDialTicked != NULL) {
        pushableDial->Internal.OnPressedDialTicked(pushableDial, direction);
      }
    }
  }
}

void PushableDial_OnPushKeyStateChanged(BinaryPushKey *sender, BinaryPushKeyState state) {
  PushableDial *pushableDial = (PushableDial *)sender->Internal.Parent;
  PushableDialLastUpdatedItem lastUpdatedItem = pushableDial->Internal.LastUpdatedItem;
  pushableDial->Internal.LastUpdatedItem = PushKey;
  if(pushableDial->OnPushKeyStateChanged == NULL || !pushableDial->OnPushKeyStateChanged(pushableDial, state, lastUpdatedItem != PushKey)) {
    if (pushableDial->Internal.OnPushKeyStateChanged != NULL) {
      pushableDial->Internal.OnPushKeyStateChanged(pushableDial, state, lastUpdatedItem != PushKey);
    }
  }
}
