#ifndef __OLED_H__
#define __OLED_H__

#include "stm32f1xx_hal.h"

#define OLED_ADDRESS 0x7A // OLED I2C address

typedef enum OLED_TransmitMode {
    OLED_TRANSMIT_CTRL = 0x00,
    OLED_TRANSMIT_DATA = 0x40,
} OLED_TransmitMode;

typedef enum OLED_FillingMode {
    OLED_FILLING_NORMAL = 0,
    OLED_FILLING_REVERSED = 1,
} OLED_FillingMode;

typedef enum OLED_OrientationMode {
    OLED_ORIENTATION_NORMAL = 0,
    OLED_ORIENTATION_REVERSED = 1,
} OLED_OrientationMode;

typedef enum OLED_PlottingMode {
    OLED_PLOTTING_CLEAR = 0,
    OLED_PLOTTING_FILL = 1,
} OLED_PlottingMode;

typedef enum OLED_BackgroundMode {
    OLED_BACKGROUND_TRANSPARENT = 0,
    OLED_BACKGROUND_FILL = 1,
} OLED_BackgroundMode;

typedef enum OLED_Font {
    OLED_FONT_0806 = 8,
    OLED_FONT_1206 = 12,
    OLED_FONT_1608 = 16,
    OLED_FONT_2412 = 24,
} OLED_Font;

void OLED_TransmitByte(uint8_t data, OLED_TransmitMode mode);
void OLED_Init();
void OLED_Deinit();
void OLED_EnterSleep();
void OLED_ExitSleep();
void OLED_SetFillingMode(OLED_FillingMode mode);
void OLED_SetOrientation(OLED_OrientationMode mode);
void OLED_Flush();
void OLED_ClearBuffer();
void OLED_Plot(uint8_t x, uint8_t y, OLED_PlottingMode mode);
void OLED_PlotChar(uint8_t x, uint8_t y, char c, OLED_Font font, OLED_PlottingMode foreground, OLED_BackgroundMode background);
void OLED_PlotString(uint8_t x, uint8_t y, char *s, OLED_Font font, OLED_PlottingMode foreground, OLED_BackgroundMode background);

#endif
