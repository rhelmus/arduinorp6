#include <Servo.h>
#include <Wire.h>

// Need to include this here in order to properly setup include paths...
#include <ColorLCDShield.h>

#include "gui.h"
#include "interface.h"

void drawLCDScreen(void)
{
#if 0
//     lcd.clear(GREEN);
    
    const uint8_t wsx = LCD_STARTX + 10, wsy = LCD_STARTY + 10;
    const uint8_t wex = (LCD_STARTX + LCD_WIDTH) - 10;
    const uint8_t wey = (LCD_STARTY + LCD_HEIGHT) - 20;
    lcd.setRect(wsx, wsy, wex, /*wey*/wsy+3, true, BLUE);
    
    char strval[5];
    const uint8_t tx = wsx + 5;
    uint8_t ty = wsy + 5;
    
    lcd.setStr("Left speed", tx, ty, BLACK, BLUE);
    itoa(robotData.lspeed, strval, 10);
    lcd.setStr(strval, (wex - 5 - (8 * strlen(strval))), ty, BLACK, BLUE);
    
    ty += 17;
    lcd.setStr("Right speed", tx, ty, BLACK, BLUE);
    itoa(robotData.rspeed, strval, 10);    
    lcd.setStr(strval, (wex - 5 - (8 * strlen(strval))), ty, BLACK, BLUE);

    ty += 17;
    lcd.setStr("Battery", tx, ty, BLACK, BLUE);
    itoa(robotData.battery, strval, 10);    
    lcd.setStr(strval, (wex - 5 - (8 * strlen(strval))), ty, BLACK, BLUE);
#endif
}

void setup()  
{
    Serial.begin(9600);
    initInterface();
    Serial.println("Arduino started!");
}

void loop()
{
    static uint32_t updelay = 0;
    
    const uint32_t curtime = millis();
    if (updelay < curtime)
    {
        updelay = curtime + 25;
        updateInterface();
    }
}
