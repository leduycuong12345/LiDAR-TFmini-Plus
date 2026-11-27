#ifndef LCD_I2C_H
#define LCD_I2C_H

#include "stm32f1xx_hal_i2c.h"
#include <stdint.h>

void LCD_Init(I2C_HandleTypeDef *hi2c);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t col, uint8_t row);
void LCD_SendChar(char ch);
void LCD_SendString(char *str);

#endif