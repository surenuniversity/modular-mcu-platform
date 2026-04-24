#include "ssd1306.h"
#include "font.h"
#include "string.h"

extern I2C_HandleTypeDef hi2c1;

// Display buffer (128x32 → 512 bytes)
static uint8_t buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

static uint8_t cursorX = 0;
static uint8_t cursorY = 0;

// ================= LOW LEVEL =================

void SSD1306_WriteCommand(uint8_t byte)
{
    uint8_t data[2];
    data[0] = 0x00;
    data[1] = byte;
    HAL_I2C_Master_Transmit(&hi2c1, SSD1306_I2C_ADDR, data, 2, HAL_MAX_DELAY);
}

void SSD1306_WriteData(uint8_t* data, size_t size)
{
    uint8_t temp[size + 1];
    temp[0] = 0x40;
    memcpy(&temp[1], data, size);
    HAL_I2C_Master_Transmit(&hi2c1, SSD1306_I2C_ADDR, temp, size + 1, HAL_MAX_DELAY);
}

// ================= INIT =================

void SSD1306_Init(void)
{
    HAL_Delay(100);

    SSD1306_WriteCommand(0xAE); // Display OFF

    SSD1306_WriteCommand(0x20); // Memory addressing mode
    SSD1306_WriteCommand(0x10);

    SSD1306_WriteCommand(0xB0); // Page start
    SSD1306_WriteCommand(0xC8); // COM scan direction
    SSD1306_WriteCommand(0x00);
    SSD1306_WriteCommand(0x10);

    SSD1306_WriteCommand(0x40); // Start line

    SSD1306_WriteCommand(0x81); // Contrast
    SSD1306_WriteCommand(0xFF);

    SSD1306_WriteCommand(0xA1); // Segment remap
    SSD1306_WriteCommand(0xA6); // Normal display

    SSD1306_WriteCommand(0xA8); // Multiplex
    SSD1306_WriteCommand(0x1F); // 32 rows

    SSD1306_WriteCommand(0xD3); // Offset
    SSD1306_WriteCommand(0x00);

    SSD1306_WriteCommand(0xD5); // Clock
    SSD1306_WriteCommand(0xF0);

    SSD1306_WriteCommand(0xD9); // Pre-charge
    SSD1306_WriteCommand(0x22);

    SSD1306_WriteCommand(0xDA); // COM pins
    SSD1306_WriteCommand(0x02);

    SSD1306_WriteCommand(0xDB); // VCOM detect
    SSD1306_WriteCommand(0x20);

    SSD1306_WriteCommand(0x8D); // Charge pump
    SSD1306_WriteCommand(0x14);

    SSD1306_WriteCommand(0xAF); // Display ON
}

// ================= DISPLAY CONTROL =================

void SSD1306_UpdateScreen(void)
{
    for(uint8_t i = 0; i < 4; i++) // 32px → 4 pages
    {
        SSD1306_WriteCommand(0xB0 + i);
        SSD1306_WriteCommand(0x00);
        SSD1306_WriteCommand(0x10);

        SSD1306_WriteData(&buffer[SSD1306_WIDTH * i], SSD1306_WIDTH);
    }
}

void SSD1306_Clear(void)
{
    memset(buffer, 0, sizeof(buffer));
}

// ================= GRAPHICS =================

void SSD1306_DrawPixel(uint8_t x, uint8_t y, uint8_t color)
{
    if(x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT)
        return;

    if(color)
        buffer[x + (y / 8) * SSD1306_WIDTH] |= (1 << (y % 8));
    else
        buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
}

// ================= TEXT =================

void SSD1306_SetCursor(uint8_t x, uint8_t y)
{
    cursorX = x;
    cursorY = y;
}

void SSD1306_WriteChar(char ch)
{
    const uint8_t *bitmap;

    switch(ch)
    {
        case 'H': bitmap = Font5x7[0]; break;
        case 'E': bitmap = Font5x7[1]; break;
        case 'L': bitmap = Font5x7[2]; break;
        case 'O': bitmap = Font5x7[3]; break;
        default: return;
    }

    for(uint8_t i = 0; i < 5; i++)
    {
        uint8_t line = bitmap[i];

        for(uint8_t j = 0; j < 7; j++)
        {
            if(line & (1 << j))
                SSD1306_DrawPixel(cursorX + i, cursorY + j, 1);
            else
                SSD1306_DrawPixel(cursorX + i, cursorY + j, 0);
        }
    }

    cursorX += 6;
}

void SSD1306_WriteString(char* str)
{
    while(*str)
    {
        SSD1306_WriteChar(*str++);
    }
}
