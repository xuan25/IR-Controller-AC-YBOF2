/**
  ******************************************************************************
  * @file    dial.c
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

#include "dial.h"

void Dial_OnEncoderTick(struct Encoder *sender, int8_t direction, Encoder_Edge edge);
void Dial_InputTick(Dial *dial, int8_t direction, Encoder_Edge edge);

void Dial_Init(Dial *dial) {
  uint32_t tickMs = HAL_GetTick();
  dial->Internal.LastInputTickMs = tickMs;
  dial->Internal.InputState = 0;
  dial->Encoder->Internal.Parent = dial;
  dial->Encoder->Internal.OnTicked = Dial_OnEncoderTick;
  Encoder_Init(dial->Encoder);
}

void Dial_Scan(Dial *dial) {
  Encoder_Scan(dial->Encoder);
}

void Dial_OnEncoderTick(struct Encoder *sender, int8_t direction, Encoder_Edge edge) {
  Dial *dial = (Dial *)sender->Internal.Parent;
  Dial_InputTick(dial, direction, edge);
}

void Dial_InputTick(Dial *dial, int8_t direction, Encoder_Edge edge) {
  // Check for reset delay 
  uint32_t currentTick = HAL_GetTick();
  if(dial->ResetDelayMs > 0 && currentTick - dial->Internal.LastInputTickMs > dial->ResetDelayMs){
    dial->Internal.InputState = 0;
  }
  dial->Internal.LastInputTickMs = currentTick;

  // Check for tick interval
  if (edge & dial->IgnoreInputEdge) {
    return;
  }
  dial->Internal.InputState += direction;
  if (dial->Internal.InputState >= dial->TickInterval || dial->Internal.InputState <= -dial->TickInterval) {
    if(dial->OnTicked == NULL || !dial->OnTicked(dial, dial->Internal.InputState)) {
      if(dial->Internal.OnTicked != NULL) {
        dial->Internal.OnTicked(dial, dial->Internal.InputState);
      }
    }
    dial->Internal.InputState = 0;
  }
}
