#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);
SoftwareSerial stm32(10, 11); 
// RX = 10, TX = 11

void setup()
{
    Serial.begin(115200);
    stm32.begin(115200);
    
    Serial.println("Arduino ready");
    lcd.init();
    lcd.backlight();

    lcd.setCursor(0, 0);
    lcd.print("Hello Arduino!");
}

void loop()
{
    if (stm32.available())
    {
        /*lcd.clear();
        char c = stm32.read();
        Serial.write(c);
        lcd.print(c);*/
        char c = stm32.read();

        if (c == '\n')
        {
            Serial.println();
            lcd.clear();
            lcd.setCursor(0, 0); // Move LCD to second row

        }
        else if (c != '\r')
        {
            Serial.write(c);
            lcd.print(c);
        }
    }
}