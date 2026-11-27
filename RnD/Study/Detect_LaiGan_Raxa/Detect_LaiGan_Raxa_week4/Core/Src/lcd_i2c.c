#include "lcd_i2c.h"
#include <stdint.h>

#define LCD_I2C_ADDR  (0x27 << 1)  // PCF8574 address
#define LCD_RS        0x01
#define LCD_RW        0x02
#define LCD_EN        0x04
#define LCD_BL        0x08          // Backlight ON

static I2C_HandleTypeDef *lcd_i2c;

/* Send 1 byte to PCF8574 */
static void LCD_I2C_Write(uint8_t data)
{
    HAL_I2C_Master_Transmit(
        lcd_i2c,
        LCD_I2C_ADDR,
        &data,
        1,
        100
    );
}

/* Pulse Enable pin */
static void LCD_EnablePulse(uint8_t data)
{
    LCD_I2C_Write(data | LCD_EN);
    HAL_Delay(1);

    LCD_I2C_Write(data & ~LCD_EN);
    HAL_Delay(1);
}

/* Send 4 bits to LCD */
static void LCD_Send4Bits(uint8_t nibble, uint8_t mode)
{
    uint8_t data = (nibble & 0xF0) | LCD_BL | mode;

    LCD_I2C_Write(data);
    LCD_EnablePulse(data);
}

/* Send command */
static void LCD_SendCommand(uint8_t cmd)
{
    LCD_Send4Bits(cmd & 0xF0, 0);
    LCD_Send4Bits((cmd << 4) & 0xF0, 0);

    if (cmd == 0x01 || cmd == 0x02)
        HAL_Delay(2);
}

/* Send data */
static void LCD_SendData(uint8_t data)
{
    LCD_Send4Bits(data & 0xF0, LCD_RS);
    LCD_Send4Bits((data << 4) & 0xF0, LCD_RS);
}

/* Initialize LCD */
void LCD_Init(I2C_HandleTypeDef *hi2c)
{
    lcd_i2c = hi2c;

    HAL_Delay(50);

    // HD44780 initialization sequence
    LCD_Send4Bits(0x30, 0);
    HAL_Delay(5);

    LCD_Send4Bits(0x30, 0);
    HAL_Delay(1);

    LCD_Send4Bits(0x30, 0);
    HAL_Delay(10);

    LCD_Send4Bits(0x20, 0);  // Enter 4-bit mode
    HAL_Delay(10);

    LCD_SendCommand(0x28);   // 4-bit, 2-line, 5x8 font
    LCD_SendCommand(0x0C);   // Display ON, cursor OFF
    LCD_SendCommand(0x06);   // Entry mode
    LCD_SendCommand(0x01);   // Clear display

    HAL_Delay(2);
}

/* Clear LCD */
void LCD_Clear(void)
{
    LCD_SendCommand(0x01);
}

/* Set cursor position */
void LCD_SetCursor(uint8_t col, uint8_t row)
{
    if (row > 1) row = 1;
    if (col > 15) col = 15;

    uint8_t address;

    if (row == 0)
        address = 0x00 + col;
    else
        address = 0x40 + col;

    LCD_SendCommand(0x80 | address);
}

/* Send one character */
void LCD_SendChar(char ch)
{
    LCD_SendData((uint8_t)ch);
}

/* Send string */
void LCD_SendString(char *str)
{
    while (*str != '\0')
    {
        LCD_SendChar(*str++);
    }
}
