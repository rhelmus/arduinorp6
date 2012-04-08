#include "gui.h"
#include "interface.h"
#include "mainstatwidget.h"
#include "menu.h"

#include <Wire.h>

uint16_t robotData[DATA_END];
CGUI GUI;

CMainStatWidget mainStatWidget;

#define MAKE_MENUITEM(v, s) \
    const char v##_txt[] PROGMEM = s; \
    SMenuItem v(v##_txt)


CMenu mainMenu;
MAKE_MENUITEM(mainStatItem, "Status");
MAKE_MENUITEM(secondItem, "Second item");
MAKE_MENUITEM(thirdItem, "Third item");


#undef MAKE_MENUITEM

struct SButton
{
    uint8_t pin;
    uint8_t lastState, currentState;
    uint32_t lastDebounceTime;
} LCDShieldSwitches[3] = { { 3, HIGH, HIGH, 0 }, { 4, HIGH, HIGH, 0 }, { 5, HIGH, HIGH, 0 } };


extern int __bss_end;
extern void *__brkval;
int memFree()
{
    int fv;
    if ((int)__brkval == 0)
        return ((int)&fv) - ((int)&__bss_end);
    return ((int)&fv) - ((int)&__brkval);
}

void TWIMasterRequest(void)
{

}

void TWIReceived(int bytes)
{
    const uint8_t cmd = Wire.read();

    if (cmd == TWI_CMD_SETDATA)
    {
        const uint8_t type = Wire.read();

        if ((type == MOTOR_LSPEED) || (type == MOTOR_RSPEED))
            robotData[type] = Wire.read();
        else
            robotData[type] = word(Wire.read(), Wire.read());

        mainStatWidget.markDirty();
    }

    if (Wire.available())
    {
        Wire.read();
//        Serial.print(F("Extra bytes left: "));
//        while (Wire.available())
//        {
//            Serial.print((char)Wire.read());
//            Serial.print(", ");
//        }
//        Serial.println();
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

void menuCallBack(SMenuItem *item)
{
    Serial.print("CallBack: ");

    char c;
    const char *str = item->text;
    while ((c = pgm_read_byte(str++)))
        Serial.print(c);

    Serial.println("");

    if (item == &mainStatItem)
        GUI.setActiveWidget(&mainStatWidget);
}

void widgetCloseCB(CWidget *w)
{
    GUI.setActiveWidget(&mainMenu);
}

void initInterface()
{
    for (uint8_t i=0; i<3; ++i)
    {
        pinMode(LCDShieldSwitches[i].pin, INPUT);
        digitalWrite(LCDShieldSwitches[i].pin, HIGH);
    }

    Wire.begin(10 >> 1);
    Wire.onRequest(TWIMasterRequest);
    Wire.onReceive(TWIReceived);

    GUI.init();
    GUI.addWidget(&mainStatWidget);
    GUI.addWidget(&mainMenu);
    GUI.setActiveWidget(&mainMenu);

    mainMenu.addItem(&mainStatItem);
    mainMenu.addItem(&secondItem);
    mainMenu.addItem(&thirdItem);
    mainMenu.setCallback(menuCallBack);

    mainStatWidget.setClosedCB(widgetCloseCB);
}

void updateInterface()
{
    const uint32_t curtime = millis();
    for (uint8_t i=0; i<3; ++i)
    {
        const uint8_t state = digitalRead(LCDShieldSwitches[i].pin);
        if (state != LCDShieldSwitches[i].lastState)
        {
            LCDShieldSwitches[i].lastDebounceTime = curtime;
            LCDShieldSwitches[i].lastState = state;
        }
        else if ((curtime - LCDShieldSwitches[i].lastDebounceTime) > 50)
            LCDShieldSwitches[i].currentState = state;
    }

    GUI.run(!LCDShieldSwitches[0].currentState | !LCDShieldSwitches[1].currentState << 1 |
            !LCDShieldSwitches[2].currentState << 2);
}
