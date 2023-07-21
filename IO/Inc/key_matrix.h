/**
  ******************************************************************************
  * @file    key_matrix.h
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

#ifndef __KEY_MATRIX_H
#define __KEY_MATRIX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "gpio_pin.h"
#include "binary_push_key.h"

struct MatrixKey_Internal;
struct MatrixKey;
struct KeyMatrix_Internal;
struct KeyMatrix;

typedef struct MatrixKey_Internal {
  void *Parent;      // Parent

  /**
   * @brief State changed callback of the MatrixKey (parent)
   * 
   * @param sender MatrixKey that trigger the callback.
   * @param state state.
   * @retval None
  */
  void (*OnStateChanged)(struct MatrixKey *sender, BinaryPushKeyState state);
} MatrixKey_Internal;

/**
 * @brief MatrixKey Structure definition
 * 
 * @note Used to managed a key hardware in a key matrix.
*/
typedef struct MatrixKey {
  MatrixKey_Internal Internal;
  Key *Key;           // Key structure
  uint16_t X;         // X coordinate of the key
  uint16_t Y;         // Y coordinate of the key

  /**
   * @brief State changed callback of the MatrixKey
   * 
   * @param sender MatrixKey that trigger the callback.
   * @param state state.
   * @retval Whether the event has been handled. 
   * Once the event has been handled, it will not been sent to its parent.
  */
  uint8_t (*OnStateChanged)(struct MatrixKey *sender, BinaryPushKeyState state);
} MatrixKey;

typedef struct KeyMatrix_Internal {
  void *Parent;                 // Parent
  uint16_t Stride;              // Lookup stride
  Key **Keys;                   // Keys for matrix coordinates
  uint8_t *EnabledFlags;        // Enabled flags for matrix coordinates

  /**
   * @brief State changed callback of the KeyMatrix (parent)
   * 
   * @param sender KeyMatrix that trigger the callback.
   * @param matrixKey MatrixKey which state has been changed.
   * @param state state.
   * @retval None
  */
  void (*OnStateChanged)(struct KeyMatrix *sender, struct MatrixKey *matrixKey, BinaryPushKeyState state);
} KeyMatrix_Internal;

/**
 * @brief KeyMatrix Structure definition
 * 
 * @note Used to managed a key matrix.
*/
typedef struct KeyMatrix {
  KeyMatrix_Internal Internal;  // For internal usage
  MatrixKey **MatrixKeys;       // A null-terminated array of defined MatrixKeys
  GPIO_Pin **Rows;              // A null-terminated array of row pins
  GPIO_Pin **Cols;              // A null-terminated array of column pins
  GPIO_PinState ReleasedLevel;  // GPIO state when key released

  /**
   * @brief State changed callback of the KeyMatrix
   * 
   * @param sender KeyMatrix that trigger the callback.
   * @param matrixKey MatrixKey which state has been changed.
   * @param state state.
   * @retval Whether the event has been handled. 
   * Once the event has been handled, it will not been sent to its parent.
  */
  uint8_t (*OnStateChanged)(struct KeyMatrix *sender, struct MatrixKey *matrixKey, BinaryPushKeyState state);
} KeyMatrix;

/**
 * @brief Initialize the Key matrix
 * 
 * @param keyMatrix The KeyMatrix to be initialized
 * @retval None
*/
void KeyMatrix_Init(KeyMatrix *keyMatrix);

/**
 * @brief De-initialize the Key matrix
 * 
 * @param keyMatrix The KeyMatrix to be de-initialize
 * @retval None
*/
void KeyMatrix_DeInit(KeyMatrix *keyMatrix);

/**
 * @brief Scan an Key matrix hardware to update its state and potentially 
 * trigger callbacks.
 * 
 * @param keyMatrix The KeyMatrix to be scanned
 * @retval None
*/
void KeyMatrix_Scan(KeyMatrix *keyMatrix);

#ifdef __cplusplus
}
#endif

#endif /* __KEY_MATRIX_H */
