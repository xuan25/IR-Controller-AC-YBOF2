/**
  ******************************************************************************
  * @file    key_matrix.c
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

#include "key_matrix.h"
#include <stdlib.h>
#include "gpio_conf.h"
#include "hpt.h"

void MatrixKey_OnKeyStateChanged(Key *sender, uint8_t oldState, uint8_t newState);
void KeyMatrix_OnMatrixKeyStateChanged(MatrixKey *sender, BinaryPushKeyState state);

void KeyMatrix_Init(KeyMatrix *keyMatrix) {

  uint16_t numRows = 0, numCols = 0;
  while (keyMatrix->Rows[numRows] != NULL)
  {
    numRows++;
  }
  while (keyMatrix->Cols[numCols] != NULL)
  {
    numCols++;
  }

  uint16_t numItemsMax = numRows * numCols;
  keyMatrix->Internal.Stride = numCols;

  // Init lookup
  keyMatrix->Internal.Keys = (Key **)malloc(numItemsMax * sizeof(Key *));
  keyMatrix->Internal.EnabledFlags = (uint8_t *)malloc(numItemsMax * sizeof(uint8_t));
  for (int i = 0; i < numItemsMax; i++) {
    keyMatrix->Internal.Keys[i] = NULL;
    keyMatrix->Internal.EnabledFlags[i] = 0;
  }

  // Init relations
  for (uint16_t i = 0; keyMatrix->MatrixKeys[i] != NULL; i++) {
    MatrixKey *matrixKey = keyMatrix->MatrixKeys[i];
    matrixKey->Key->Internal.Parent = matrixKey;
    matrixKey->Key->Internal.OnStateChanged = MatrixKey_OnKeyStateChanged;
    matrixKey->Internal.Parent = keyMatrix;
    matrixKey->Internal.OnStateChanged = KeyMatrix_OnMatrixKeyStateChanged;

    uint8_t idx = matrixKey->Y * keyMatrix->Internal.Stride + matrixKey->X;
    keyMatrix->Internal.Keys[idx] = matrixKey->Key;
    keyMatrix->Internal.EnabledFlags[idx] = 1;
  }
  
  // Init state
  uint64_t tickUs = HPT_GetUs();
  for (uint16_t x = 0; keyMatrix->Rows[x] != NULL; x++) {
    GPIO_Pin *row = keyMatrix->Rows[x];
    HAL_GPIO_WritePin(row->GPIOx, row->GPIO_Pin, keyMatrix->ReleasedLevel);
  }
  for (uint16_t x = 0; keyMatrix->Rows[x] != NULL; x++){
    GPIO_Pin *row = keyMatrix->Rows[x];
    HAL_GPIO_WritePin(row->GPIOx, row->GPIO_Pin, ~keyMatrix->ReleasedLevel & 0b1);
    for (uint16_t y = 0; keyMatrix->Cols[y] != NULL; y++) {
      uint8_t idx = x * keyMatrix->Internal.Stride + y;
      if (!keyMatrix->Internal.EnabledFlags[idx]) {
        continue;
      }
      Key *key = keyMatrix->Internal.Keys[idx];

      GPIO_Pin *col = keyMatrix->Cols[y];
      GPIO_PinState level = HAL_GPIO_ReadPin(col->GPIOx, col->GPIO_Pin);

      key->Internal.LastChangedLevel = level;
      key->Internal.LastLevelChangedUs = tickUs;
    }
    HAL_GPIO_WritePin(row->GPIOx, row->GPIO_Pin, keyMatrix->ReleasedLevel);
  }
}

void KeyMatrix_DeInit(KeyMatrix *keyMatrix) {
  free(keyMatrix->Internal.Keys);
  free(keyMatrix->Internal.EnabledFlags);
}

void KeyMatrix_Scan(KeyMatrix *keyMatrix) {
  for (uint16_t r = 0; keyMatrix->Rows[r] != NULL; r++) {
    GPIO_Pin *row = keyMatrix->Rows[r];
    HAL_GPIO_WritePin(row->GPIOx, row->GPIO_Pin, ~keyMatrix->ReleasedLevel & 0b1);
#if GPIO_GENERIC_DELAY_US > 0
    HPT_DelayUs(GPIO_GENERIC_DELAY_US);
#endif
    for (uint16_t c = 0; keyMatrix->Cols[c] != NULL; c++) {
      uint8_t idx = r * keyMatrix->Internal.Stride + c;
      if (!keyMatrix->Internal.EnabledFlags[idx]) {
        continue;
      }
      Key *key = keyMatrix->Internal.Keys[idx];

      GPIO_Pin *col = keyMatrix->Cols[c];
      GPIO_PinState level = HAL_GPIO_ReadPin(col->GPIOx, col->GPIO_Pin);

      Key_Update(key, level);
    }
    HAL_GPIO_WritePin(row->GPIOx, row->GPIO_Pin, keyMatrix->ReleasedLevel);
  }
}

void MatrixKey_OnKeyStateChanged(Key *sender, uint8_t oldState, uint8_t newState) {
  MatrixKey *matrixKey = (MatrixKey *)sender->Internal.Parent;
  KeyMatrix *keyMatrix = (KeyMatrix *)matrixKey->Internal.Parent;

  BinaryPushKeyState state;
  if (newState == keyMatrix->ReleasedLevel) {
    state = PushKeyReleased;
  } else {
    state = PushKeyPressed;
  }
  if (matrixKey->OnStateChanged == NULL || !matrixKey->OnStateChanged(matrixKey, state)) {
    if(matrixKey->Internal.OnStateChanged != NULL) {
      matrixKey->Internal.OnStateChanged(matrixKey, state);
    }
  }
}

void KeyMatrix_OnMatrixKeyStateChanged(MatrixKey *sender, BinaryPushKeyState state) {
  KeyMatrix *keyMatrix = (KeyMatrix *)sender->Internal.Parent;
  if (keyMatrix->OnStateChanged == NULL || !keyMatrix->OnStateChanged(keyMatrix, sender, state)) {
    if(keyMatrix->Internal.OnStateChanged != NULL) {
      keyMatrix->Internal.OnStateChanged(keyMatrix, sender, state);
    }
  }
}