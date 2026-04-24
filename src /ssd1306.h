#ifndef __SSD1306_H
#define __SSD1306_H

#include "stm32f4xx_hal.h"

// OLED size
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 32

// I2C address
#define SSD1306_I2C_ADDR (0x3C << 1)

// Functions
void SSD1306_Init(void);
void SSD1306_Clear(void);
void SSD1306_UpdateScreen(void);
void SSD1306_DrawPixel(uint8_t x, uint8_t y, uint8_t color);
void SSD1306_SetCursor(uint8_t x, uint8_t y);
void SSD1306_WriteChar(char ch);
void SSD1306_WriteString(char* str);

#endif
