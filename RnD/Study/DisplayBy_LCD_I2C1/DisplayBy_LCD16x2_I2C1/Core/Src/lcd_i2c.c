
#include "lcd_i2c.h"

#define LCD_ADDR (0x27 << 1)

#define LCD_BACKLIGHT 0x08
#define LCD_ENABLE    0x04
#define LCD_RS        0x01

static I2C_HandleTypeDef *lcd_i2c;

static void LCD_ExpanderWrite(uint8_t data)
{
    HAL_I2C_Master_Transmit(
        lcd_i2c,
        LCD_ADDR,
        &data,
        1,
        HAL_MAX_DELAY
    );
}

static void LCD_PulseEnable(uint8_t data)
{
    LCD_ExpanderWrite(data | LCD_ENABLE);
    HAL_Delay(1);

    LCD_ExpanderWrite(data & ~LCD_ENABLE);
    HAL_Delay(1);
}

static void LCD_Send4Bits(uint8_t data)
{
    data |= LCD_BACKLIGHT;

    LCD_ExpanderWrite(data);
    LCD_PulseEnable(data);
}

static void LCD_Send(uint8_t data, uint8_t rs)
{
    uint8_t high = data & 0xF0;
    uint8_t low  = (data << 4) & 0xF0;

    LCD_Send4Bits(high | rs);
    LCD_Send4Bits(low | rs);
}

static void LCD_SendCommand(uint8_t cmd)
{
    LCD_Send(cmd, 0);
}

static void LCD_SendData(uint8_t data)
{
    LCD_Send(data, LCD_RS);
}

void LCD_Init(I2C_HandleTypeDef *hi2c)
{
    lcd_i2c = hi2c;

    HAL_Delay(50);

    LCD_Send4Bits(0x30);
    HAL_Delay(5);

    LCD_Send4Bits(0x30);
    HAL_Delay(1);

    LCD_Send4Bits(0x30);
    HAL_Delay(1);

    LCD_Send4Bits(0x20);

    LCD_SendCommand(0x28); // 4-bit, 2-line mode
    LCD_SendCommand(0x0C); // Display ON
    LCD_SendCommand(0x06); // Entry mode
    LCD_Clear();
}

void LCD_Clear(void)
{
    LCD_SendCommand(0x01);
    HAL_Delay(2);
}

void LCD_SetCursor(uint8_t row, uint8_t col)
{
    static const uint8_t row_offsets[] = {
        0x00, 0x40, 0x14, 0x54
    };

    if (row > 3 || col > 19)
        return;

    LCD_SendCommand(0x80 | (row_offsets[row] + col));
}

void LCD_SendString(char *str)
{
    while (*str)
    {
        LCD_SendData((uint8_t)*str++);
    }
}
