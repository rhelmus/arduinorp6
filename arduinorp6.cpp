#include <Wire.h>

// Need to include this here in order to properly setup include paths...
#include <ColorLCDShield.h>

#include "gui.h"
#include "interface.h"

enum
{
    I2C_CMD_SETLSPEED = 0,
    I2C_CMD_SETRSPEED,
    I2C_CMD_SETBATTERY
};

struct SRobotData
{
    uint8_t lspeed, rspeed;
    uint16_t battery;
    
    SRobotData(void) : lspeed(0), rspeed(0), battery(0) { }
};

const uint8_t sharpIRPin = 0;
SRobotData robotData;
CGUI GUI;

void TWIMasterRequest(void)
{
    
}

void TWIReceived(int bytes)
{
    Serial.print("Received data from TWI master: ");
    Serial.println(bytes);
    
    const uint8_t cmd = Wire.read();   
    
    if (cmd == I2C_CMD_SETLSPEED)
        robotData.lspeed = Wire.read();
    else if (cmd == I2C_CMD_SETRSPEED)
        robotData.rspeed = Wire.read();
    else if (cmd == I2C_CMD_SETBATTERY)
        robotData.battery = word(Wire.read(), Wire.read());
    
    if (Wire.available())
    {
        Serial.print("Extra bytes left: ");
        while (Wire.available())
        {
            Serial.print((char)Wire.read());
            Serial.print(", ");
        }
        Serial.println();
    }
       
#if 0
    Serial.print("Data: ");
    while (bytes)
    {
        Serial.print((char)Wire.read());
        Serial.print(", ");
        --bytes;
    }
    Serial.println("");
#endif
}


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
    Wire.begin(10 >> 1);
    Wire.onRequest(TWIMasterRequest);
    Wire.onReceive(TWIReceived);
    
    GUI.init();
    initInterface();

    Serial.begin(9600);
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
//        drawLCDScreen();
    }
}
