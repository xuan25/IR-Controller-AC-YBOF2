/**
  ******************************************************************************
  * @file    ws2812b.h
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

#ifndef __WS2812B_H
#define __WS2812B_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include <stdlib.h>
#include <string.h>

#define WS2812B_RESET_CYCLES 2u
#define WS2812B_DUTY_BASE	      3
#define WS2812B_DUTY_ONE	      2
#define WS2812B_DUTY_ZERO	      1

#define WS2812B_BYTES_PER_UNIT 3

#define WS2812B_DMA_BUFFER_LENGTH   2 * WS2812B_BYTES_PER_UNIT * 8

#define WS2812B_GAMMA_CORRECTION   1

/**
 * @brief RGB Structure definition
 * 
 * @note Used to manage a set of RGB value.
*/
typedef struct RGB {
  uint8_t R;
  uint8_t G;
  uint8_t B;
} RGB;

/**
 * @brief Filter Structure definition
 * 
 * @note Used to manage a color filter.
*/
typedef struct Filter {
  void *Params;                                       // Filter parameters
  RGB (*Function)(struct Filter *filter, RGB rgb);    // Filter function
  struct Filter *Next;                                // Next filter in the chain
} Filter;

/**
 * @brief Color Structure definition
 * 
 * @note Used to manage a Color.
*/
typedef struct Color {
  RGB *RGB;                // RGB value
  Filter *Filter;          // A linked-list of filter chain
} Color;

/**
 * @brief Evaluate the RGB value of the Color
 * 
 * @param series The Color to be evaluate
 * @retval None
*/
RGB Color_EvaluateRGB(Color *color);

typedef struct WS2812B_Internal {
  void *Parent;       // Parent
} WS2812B_Internal;

/**
 * @brief WS2812B Structure definition
 * 
 * @note Used to managed a unit of WS2812B.
*/
typedef struct WS2812B {
  WS2812B_Internal Internal;  // For internal usage
  Color *Color;               // Color of the WS2812B
} WS2812B;

typedef struct WS2812BSeries_Internal {
  void *Parent;                                     // Parent
  uint16_t NumUnits;                                // Number of Units
  uint8_t CurrentCycle;                             // Current update pushing cycle
  uint8_t IsUpdating;                               // Whether an updating push is ongoing 
  uint16_t DMABuffer[WS2812B_DMA_BUFFER_LENGTH];    // Double-buffer for DMA
  uint8_t IsDirty;                                  // Whether have changes that need to be pushed
} WS2812BSeries_Internal;

/**
 * @brief WS2812BSeries Structure definition
 * 
 * @note Used to managed a Series of WS2812B units.
*/
typedef struct WS2812BSeries {
  WS2812BSeries_Internal Internal;    // For internal usage
  WS2812B **Series;                   // A null-terminated array of WS2812B units
  TIM_HandleTypeDef *TIM;             // Timer handle
  uint32_t TIMChannel;                // Timer channel
} WS2812BSeries;

/**
 * @brief Initialize the WS2812BSeries
 * 
 * @param series The WS2812BSeries to be initialized
 * @retval None
*/
void WS2812BSeries_Init(WS2812BSeries *series);

/**
 * @brief Push Update form WS2812BSeries structure to hardware
 * 
 * @param series The WS2812BSeries to be pushed
 * @retval None
*/
void WS2812BSeries_PushUpdate(WS2812BSeries *series);

/**
 * @brief Fill DMA buffer
 * 
 * @param series The WS2812BSeries to be processed
 * @param bufferId The ID of the buffer to be filled
 * @retval None
*/
void WS2812BSeries_FillBuffer(WS2812BSeries *series, uint8_t bufferId);

/**
 * @brief Handle HT event of DMA
 * 
 * @param series The WS2812BSeries that associated to the event
 * @retval None
*/
void WS2812BSeries_OnHT(WS2812BSeries *series);

/**
 * @brief Handle TC event of DMA
 * 
 * @param series The WS2812BSeries that associated to the event
 * @retval None
*/
void WS2812BSeries_OnTC(WS2812BSeries *series);

/**
 * @brief Convert HSV to RGB
 * 
 * @param h H
 * @param s S
 * @param v V
 * @retval RGB value
*/
RGB HSVToRGB(double h, double s, double v);

// Resolution on alpha
#define ALPHA_RESOLUTION 200
// Max value for Alpha
#define ALPHA_MAX ALPHA_RESOLUTION

/**
 * @brief AlphaFilterParams Structure definition
 * 
 * @note The parameters of a AlphaFilter
*/
typedef struct AlphaFilterParams {
  uint32_t Alpha;
} AlphaFilterParams;

/**
 * @brief Alpha Filter Function
 * 
 * @param filter A Filter structure that contains AlphaFilterParams
 * @param rgb Input RGB value
 * @retval Filtered RGB value
*/
RGB AlphaFilter(Filter *filter, RGB rgb);

// Resolution on easing timing
#define EASING_RESOLUTION 200

/**
 * @brief EasingFilterParams Structure definition
 * 
 * @note The parameters of a EasingFilter
*/
typedef struct EasingFilterParams {
  uint32_t BeginTime;   // Begin time of the easing
  uint32_t Duration;    // Duration of the easing
  uint8_t IsCompleted;  // Whether the easing is completed
  Color *EasingFrom;    // Easing from color

  /**
   * @brief Easing function that mapping time ratio to easing ratio.
   * Keep null for linear easing
   * 
   * @param ratio Time ratio
   * @retval Easing ratio
  */
  uint16_t (*EasingFunction)(uint16_t ratio);
} EasingFilterParams;

/**
 * @brief Easing Filter Function
 * 
 * @param filter A Filter structure that contains EasingFilterParams
 * @param rgb Input RGB value
 * @retval Filtered RGB value
*/
RGB EasingFilter(Filter *filter, RGB rgb);


#ifdef __cplusplus
}
#endif

#endif /* __WS2812B_H */
