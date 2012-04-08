#include <Wire.h>

// Need to include this here in order to properly setup include paths...
#include <ColorLCDShield.h>

#include "gui.h"
#include "interface.h"

const uint8_t sharpIRPin = 0;

uint8_t getSharpIRDistance()
{
    uint16_t adc = analogRead(sharpIRPin);
    
    // UNDONE: Avoid float calculations?
    
    // ADC to volt (assuming 5v supply))
    float volt = (float)adc * 5.0 / 1023.0;
    
    // From http://www.robotshop.ca/PDF/Sharp_GP2Y0A02YK_Ranger.pdf
    const float A = 0.008271, B = 939.6, C = -3.398, D = 17.339;
    
    return (uint8_t)((A + B * volt) / (1.0 + C * volt + D * volt * volt));
}

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
