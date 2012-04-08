#include "gui.h"
#include "interface.h"
#include "mainstatwidget.h"

extern uint16_t robotData[DATA_END];

namespace {

uint16_t getDataMax(uint8_t type)
{
    if ((type == MOTOR_LSPEED) || (type == MOTOR_RSPEED))
        return 210;
    else
        return 1023;
}

}

const char *CMainStatWidget::dataLabels[DATA_END];

CMainStatWidget::CMainStatWidget() : fullUpdate(true)
{
    dataLabels[BATTERY] = PSTR("Battery");
    dataLabels[MOTOR_LSPEED] = PSTR("L spd");
    dataLabels[MOTOR_RSPEED] = PSTR("R spd");
    dataLabels[MOTOR_LCURRENT] = PSTR("L cur");
    dataLabels[MOTOR_RCURRENT] = PSTR("R cur");
    dataLabels[LIGHT_L] = PSTR("Light L");
    dataLabels[LIGHT_R] = PSTR("Light R");
}

void CMainStatWidget::reDraw()
{
    const uint8_t offset = 10;
    const uint8_t startx = LCD_STARTX + offset;
    const uint8_t endx = startx + LCD_WIDTH - (2 * offset);
    const uint8_t labelsx = startx + 5;
    const uint8_t graphsx = labelsx + 45, graphex = endx - 5;
    const uint8_t rowh = 10, rowspacing = 2;
    const uint8_t labelyoffset = (rowh - FONT_SMALL_HEIGHT) / 2;

    const uint8_t totheight = DATA_END * (rowh + rowspacing);
    const uint8_t starty = (LCD_HEIGHT - totheight) / 2;

    uint8_t y = starty + rowspacing;

    if (fullUpdate)
    {
        // Background
        CGUI::lcd.setRect(startx, starty, endx, starty + totheight, true, BROWN);

        // Border
        CGUI::lcd.setRect(startx, starty, endx, starty + totheight, false, BLUE);
    }
    else
    {
        // Partially clear widget
        CGUI::lcd.setRect(graphsx, y, graphex, y + (DATA_END*(rowh+rowspacing))-rowspacing,
                          true, BROWN);
    }

    for (uint8_t i=0; i<DATA_END; ++i)
    {
        CGUI::lcd.setStrSmall_P(dataLabels[i], labelsx, y + labelyoffset, YELLOW, BROWN);

        CGUI::lcd.setRect(graphsx, y, map(robotData[i], 0, getDataMax(i), graphsx, graphex),
                          y + rowh, true, BLUE);

        char strval[5]; // Assume no more than 4 numbers!
        itoa(robotData[i], strval, 10);
        const uint8_t vw = strlen(strval) * FONT_SMALL_WIDTH;
        CGUI::lcd.setStrSmall(strval, graphsx + ((graphex - graphsx - vw) / 2),
                              y + labelyoffset, RED, BROWN);

        y += rowh + rowspacing;
    }

    fullUpdate = false;
}

void CMainStatWidget::handleKeyRelease(uint8_t key)
{
    close();
}
