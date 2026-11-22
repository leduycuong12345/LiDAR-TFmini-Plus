#include "lcd_i2c.h"
#include <stdint.h>

#define LCD_ADDR (0x27 << 1)

#define LCD_RS  0x01
#define LCD_EN  0x04
#define LCD_BL  0x08

static I2C_HandleTypeDef *lcd_i2c;

static void LCD_Write4Bits(uint8_t data);
static void LCD_Send(uint8_t data, uint8_t mode);
static void LCD_Cmd(uint8_t cmd);

static void LCD_Write4Bits(uint8_t data)
{
    uint8_t value = data | LCD_BL;

    HAL_I2C_Master_Transmit(lcd_i2c,
                           LCD_ADDR,
                           &value,
                           1,
                           HAL_MAX_DELAY);

    value |= LCD_EN;

    HAL_I2C_Master_Transmit(lcd_i2c,
                           LCD_ADDR,
                           &value,
                           1,
                           HAL_MAX_DELAY);

    HAL_Delay(1);

    value &= ~LCD_EN;

    HAL_I2C_Master_Transmit(lcd_i2c,
                           LCD_ADDR,
                           &value,
                           1,
                           HAL_MAX_DELAY);
}

static void LCD_Send(uint8_t data, uint8_t mode)
{
    LCD_Write4Bits((data & 0xF0) | mode);
    LCD_Write4Bits(((data << 4) & 0xF0) | mode);
}

static void LCD_Cmd(uint8_t cmd)
{
    LCD_Send(cmd, 0);

    if (cmd == 0x01 || cmd == 0x02)
        HAL_Delay(2);
}

void LCD_Init(I2C_HandleTypeDef *hi2c)
{
    lcd_i2c = hi2c;

    HAL_Delay(50);

    // Initialize LCD in 4-bit mode
    LCD_Write4Bits(0x30);
    HAL_Delay(5);

    LCD_Write4Bits(0x30);
    HAL_Delay(1);

    LCD_Write4Bits(0x30);
    HAL_Delay(1);

    LCD_Write4Bits(0x20);
    HAL_Delay(1);

    // Function set: 4-bit, 2-line, 5x8 font
    LCD_Cmd(0x28);

    // Display off
    LCD_Cmd(0x08);

    // Clear display
    LCD_Cmd(0x01);

    // Entry mode: cursor increments
    LCD_Cmd(0x06);

    // Display on, cursor off, blink off
    LCD_Cmd(0x0C);
}

void LCD_Clear(void)
{
    LCD_Cmd(0x01);
}

void LCD_SetCursor(uint8_t col, uint8_t row)
{
    uint8_t row_offset[] = {0x00, 0x40, 0x14, 0x54};

    if (row > 3)
        row = 3;

    LCD_Cmd(0x80 | (row_offset[row] + col));
}

void LCD_SendChar(char ch)
{
    LCD_Send((uint8_t)ch, LCD_RS);
}

void LCD_SendString(char *str)
{
    while (*str)
    {
        LCD_SendChar(*str++);
    }
}