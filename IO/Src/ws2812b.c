/**
  ******************************************************************************
  * @file    ws2812b.c
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

#include "ws2812b.h"
#include <math.h>

#if WS2812B_GAMMA_CORRECTION
const uint8_t gamma8[] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
  10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
  17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
  25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
  37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
  51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
  69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
  90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255
};
#endif

RGB Color_EvaluateRGB(Color *color){
  RGB rgb = *color->RGB;
  Filter *filter = color->Filter;
  while (filter != NULL)
  {
    rgb = filter->Function(filter, rgb);
    filter = filter->Next;
  }
  return rgb;
}

void WS2812BSeries_Init(WS2812BSeries *series) {
  series->Internal.CurrentCycle = 0;
  series->Internal.IsUpdating = 0;
  series->Internal.IsDirty = 0;

  uint16_t i;
  for (i = 0; series->Series[i] != NULL; i++) {
    WS2812B *unit = series->Series[i];
    unit->Internal.Parent = series;
  }
  series->Internal.NumUnits = i;
}

void WS2812BSeries_PushUpdate(WS2812BSeries *series) {
  if(series->Internal.IsUpdating) {
    // If a update pushing is ongoing, set the dirty flag and return.
    series->Internal.IsDirty = 1;
    return;
  }
  // start a new update pushing
  series->Internal.CurrentCycle = 0;
  series->Internal.IsUpdating = 1;
  series->Internal.IsDirty = 0;
  HAL_TIM_PWM_Start_DMA(series->TIM, series->TIMChannel, (uint32_t *)series->Internal.DMABuffer, WS2812B_DMA_BUFFER_LENGTH);
}

void WS2812BSeries_FillBuffer(WS2812BSeries *series, uint8_t bufferId) {
  if (series->Internal.CurrentCycle < series->Internal.NumUnits) {
    // fill buffer with a WS2812B unit
    uint16_t unitIdx = series->Internal.CurrentCycle;
    uint32_t arr = series->TIM->Instance->ARR + 1;
    uint32_t pulseHigh = arr * WS2812B_DUTY_ONE / WS2812B_DUTY_BASE;
    uint32_t pulseLow = arr * WS2812B_DUTY_ZERO / WS2812B_DUTY_BASE;
    
    WS2812B *unit = series->Series[unitIdx];
    uint16_t *buffer = &(series->Internal.DMABuffer[bufferId * (WS2812B_DMA_BUFFER_LENGTH / 2)]);
    RGB color = Color_EvaluateRGB(unit->Color);

    uint8_t r = color.R;
    uint8_t g = color.G;
    uint8_t b = color.B;

#if WS2812B_GAMMA_CORRECTION
    r = gamma8[r];
    g = gamma8[g];
    b = gamma8[b];
#endif

    // fill buffer
    for (size_t i = 0; i < 8; i++) {
      buffer[i] =        (g >> (7 - i) & 0b1) ? pulseHigh : pulseLow;
      buffer[8 + i] =    (r >> (7 - i) & 0b1) ? pulseHigh : pulseLow;
      buffer[16 + i] =   (b >> (7 - i) & 0b1) ? pulseHigh : pulseLow;
    }
  } else if (series->Internal.CurrentCycle < (series->Internal.NumUnits + WS2812B_RESET_CYCLES)) {
    // fill reset
    if (series->Internal.CurrentCycle < (series->Internal.NumUnits + WS2812B_RESET_CYCLES + 2)) {
      memset(&series->Internal.DMABuffer[bufferId * (WS2812B_DMA_BUFFER_LENGTH / 2)], 0x00, (sizeof(series->Internal.DMABuffer) / 2));
    }
  } else {
    if(series->Internal.IsDirty) {
      // if the pushing is dirty, start over again
      series->Internal.IsDirty = 0;
      series->Internal.CurrentCycle = 0;
      return;
    }
    // stop DMA and exit
    HAL_TIM_PWM_Stop_DMA(series->TIM, series->TIMChannel);
    series->Internal.IsUpdating = 0;
  }
  
  series->Internal.CurrentCycle++;
}

void WS2812BSeries_OnHT(WS2812BSeries *series) {
  if(series->Internal.IsUpdating) {
    // The first buffer has been transmitted, refill it.
    WS2812BSeries_FillBuffer(series, 0);
  }
}

void WS2812BSeries_OnTC(WS2812BSeries *series) {
  if(series->Internal.IsUpdating) { 
    // The second buffer has been transmitted, refill it.
    WS2812BSeries_FillBuffer(series, 1);
  }
}

RGB HSVToRGB(double h, double s, double v) {
	double r = 0, g = 0, b = 0;

	if (s == 0)
	{
		r = v;
		g = v;
		b = v;
	}
	else
	{
		int i;
		double f, p, q, t;

		if (h == 360)
			h = 0;
		else
			h = h / 60;

		i = (int)trunc(h);
		f = h - i;

		p = v * (1.0 - s);
		q = v * (1.0 - (s * f));
		t = v * (1.0 - (s * (1.0 - f)));

		switch (i)
		{
		case 0:
			r = v;
			g = t;
			b = p;
			break;

		case 1:
			r = q;
			g = v;
			b = p;
			break;

		case 2:
			r = p;
			g = v;
			b = t;
			break;

		case 3:
			r = p;
			g = q;
			b = v;
			break;

		case 4:
			r = t;
			g = p;
			b = v;
			break;

		default:
			r = v;
			g = p;
			b = q;
			break;
		}

	}

  RGB result = {
    .R = r * 0xff,
    .G = g * 0xff,
    .B = b * 0xff,
  };
  return result;

}

RGB AlphaFilter(Filter *filter, RGB rgb) {
  AlphaFilterParams *params = (AlphaFilterParams *)filter->Params;
  RGB result = {
    .R = rgb.R * params->Alpha / ALPHA_RESOLUTION,
    .G = rgb.G * params->Alpha / ALPHA_RESOLUTION,
    .B = rgb.B * params->Alpha / ALPHA_RESOLUTION,
  };
  return result;
}

RGB EasingFilter(Filter *filter, RGB rgb){
  EasingFilterParams *params = (EasingFilterParams *)filter->Params;

  // bypass
  if (params->IsCompleted) {
    return rgb;
  }

  RGB easingFrom = Color_EvaluateRGB(params->EasingFrom);
  
  uint32_t currentTime = HAL_GetTick();
  uint32_t deltaTime = currentTime - params->BeginTime;

  // 0-100
  uint32_t easeRatio = (deltaTime * EASING_RESOLUTION) / params->Duration;

  // update IsCompleted flag and clamping easeRatio
  params->IsCompleted = easeRatio >= EASING_RESOLUTION;
  if(params->IsCompleted) {
    easeRatio = EASING_RESOLUTION;
  }
  if(easeRatio < 0) {
    easeRatio = 0;
  }

  // easing function
  if(params->EasingFunction != NULL) {
    easeRatio = params->EasingFunction(easeRatio);
  }
  
  // mix RGB
  uint32_t easeRatioFlip = EASING_RESOLUTION - easeRatio;
  RGB result = {
    .R = (easeRatio * rgb.R + easeRatioFlip * easingFrom.R) / EASING_RESOLUTION,
    .G = (easeRatio * rgb.G + easeRatioFlip * easingFrom.G) / EASING_RESOLUTION,
    .B = (easeRatio * rgb.B + easeRatioFlip * easingFrom.B) / EASING_RESOLUTION,
  };

  return result;
}
