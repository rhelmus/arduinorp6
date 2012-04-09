#include "gui.h"
#include "interface.h"
#include "logwidget.h"
#include "mainstatwidget.h"
#include "menu.h"

#include <Servo.h>
#include <Wire.h>

uint16_t robotData[DATA_END];
CGUI GUI;
Servo servo;

namespace {

CMainStatWidget mainStatWidget;
CLogWidget logWidget;

#define MAKE_MENUITEM(v, s) \
    const char v##_txt[] PROGMEM = s; \
    SMenuItem v(v##_txt)


CMenu mainMenu;
MAKE_MENUITEM(mainStatItem, "Status");
MAKE_MENUITEM(logItem, "Log");
MAKE_MENUITEM(thirdItem, "Third item");


#undef MAKE_MENUITEM

uint32_t connectedTime = 0, lastPing = 0, sharpIRUptime = 0;
bool connected = false;
EReqData requestedData = REQ_NONE;

struct SButton
{
    uint8_t pin;
    uint8_t lastState, currentState;
    uint32_t lastDebounceTime;
} LCDShieldSwitches[3] = { { 3, HIGH, HIGH, 0 }, { 4, HIGH, HIGH, 0 }, { 5, HIGH, HIGH, 0 } };


void setServoPos(uint8_t pos)
{
    servo.write(pos);
    robotData[SERVOPOS] = pos;
    mainStatWidget.markDirty();
}

uint8_t getSharpIRDistance()
{
    const uint8_t sharpIRPin = 0;
    uint16_t adc = analogRead(sharpIRPin);

    // UNDONE: Avoid float calculations?

    // ADC to volt (assuming 5v supply))
    float volt = (float)adc * 5.0 / 1023.0;

    // From http://www.robotshop.ca/PDF/Sharp_GP2Y0A02YK_Ranger.pdf
    const float A = 0.008271, B = 939.6, C = -3.398, D = 17.339;

    return (uint8_t)((A + B * volt) / (1.0 + C * volt + D * volt * volt));
}

void drawStats()
{
    // top and bottom bars
    const uint8_t barh = 10;
    CGUI::lcd.setRect(LCD_STARTX, LCD_STARTY, LCD_STARTX + LCD_WIDTH,
                      LCD_STARTY + barh, true, CYAN);
    CGUI::lcd.setRect(LCD_STARTX, LCD_STARTY + LCD_HEIGHT - barh, LCD_STARTX + LCD_WIDTH,
                      LCD_STARTY + LCD_HEIGHT, true, CYAN);

    // Battery indicator
    const uint8_t batw = 18, batx = LCD_STARTX + LCD_WIDTH - batw - 2;
    const uint8_t baty = LCD_STARTY + 2, bath = 6;
    const uint16_t batv = (robotData[BATTERY] < 600) ? 600 : robotData[BATTERY];
    CGUI::lcd.setRect(batx, baty, batx + batw, baty + bath, false, GRAY);
    CGUI::lcd.setRect(batx, baty, batx + map(batv, 600, 820, 0, batw),
                      baty + bath, true, GRAY);

    // Run time
    if (!connected)
        CGUI::lcd.setStrSmall_P(PSTR("no con"), LCD_STARTX + LCD_WIDTH - 40,
                              LCD_STARTY + LCD_HEIGHT - FONT_SMALL_HEIGHT - 1, GRAY, CYAN);
    else
    {
        const uint8_t rtime = (millis() - connectedTime) / 1000;
        char rtimestr[8];
        itoa(rtime, rtimestr, 10);
        CGUI::lcd.setStrSmall(rtimestr, LCD_STARTX + LCD_WIDTH - (strlen(rtimestr) * FONT_SMALL_WIDTH) - 2,
                              LCD_STARTY + LCD_HEIGHT - FONT_SMALL_HEIGHT - 1, GRAY, CYAN);
    }
}

void TWIMasterRequest(void)
{
    if (requestedData == REQ_PING)
    {
        if (!connected)
        {
            connected = true;
            connectedTime = millis();
        }
        lastPing = millis();
        Wire.write(10); // Dummy value
    }
    else if (requestedData == REQ_SHARPIR)
        Wire.write(getSharpIRDistance());
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
    else if (cmd == TWI_CMD_REQDATA)
        requestedData = static_cast<EReqData>(Wire.read());
    else if (cmd == TWI_CMD_LOG)
    {
        char buf[CLogWidget::MAX_LOG_CHARS];
        uint8_t i = 0;
        while (Wire.available())
            buf[i++] = Wire.read();
        logWidget.addLogEntry(buf);
    }
    else if (cmd == TWI_CMD_SETSERVO)
        setServoPos(Wire.read());

    if (Wire.available())
    {
        while (Wire.available())
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
    char c;
    const char *str = item->text;
    while ((c = pgm_read_byte(str++)))
        Serial.print(c);

    Serial.println("");

    if (item == &mainStatItem)
        GUI.setActiveWidget(&mainStatWidget);
    else if (item == &logItem)
        GUI.setActiveWidget(&logWidget);
}

void widgetCloseCB(CWidget *w)
{
    GUI.setActiveWidget(&mainMenu);
}


}

void initInterface()
{
    for (uint8_t i=0; i<3; ++i)
    {
        pinMode(LCDShieldSwitches[i].pin, INPUT);
        digitalWrite(LCDShieldSwitches[i].pin, HIGH);
    }

    servo.attach(3);

    Wire.begin(10 >> 1);
    Wire.onRequest(TWIMasterRequest);
    Wire.onReceive(TWIReceived);

    GUI.init();
    GUI.setDrawStatsCB(drawStats);
    GUI.addWidget(&mainStatWidget);
    GUI.addWidget(&logWidget);
    GUI.addWidget(&mainMenu);
    GUI.setActiveWidget(&mainMenu);

    mainMenu.addItem(&mainStatItem);
    mainMenu.addItem(&logItem);
    mainMenu.addItem(&thirdItem);
    mainMenu.setCallback(menuCallBack);

    mainStatWidget.setClosedCB(widgetCloseCB);
    logWidget.setClosedCB(widgetCloseCB);

    logWidget.addLogEntry("Hello!");
    logWidget.addLogEntry("Stuff");
    logWidget.addLogEntry("Stuff");
    logWidget.addLogEntry("Stuff");
    logWidget.addLogEntry("End!");

    setServoPos(90);
}

void updateInterface()
{
    const uint32_t curtime = millis();

    if ((curtime - lastPing) > 1000)
        connected = false;

    if (curtime > sharpIRUptime)
    {
        sharpIRUptime = curtime + 500;
        robotData[SHARPIR] = getSharpIRDistance();
        if (robotData[SHARPIR] < 20)
            robotData[SHARPIR] = 20;
        else if (robotData[SHARPIR] > 150)
            robotData[SHARPIR] = 150;
        mainStatWidget.markDirty();
    }

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
