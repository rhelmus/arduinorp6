#include "gui.h"
#include "interface.h"
#include "mainstatwidget.h"

extern uint16_t robotData[DATA_END];

namespace {

uint16_t getDataMax(uint8_t type)
{
    if (type == BATTERY)
        return 1023;
    else
        return 255;
}

}

CMainStatWidget::CMainStatWidget()
{
    dataLabels[BATTERY] = PSTR("Bat");
    dataLabels[MOTOR_LSPEED] = PSTR("L spd");
    dataLabels[MOTOR_RSPEED] = PSTR("R spd");
}

void CMainStatWidget::reDraw()
{
    const uint8_t offset = 10;
    const uint8_t startx = LCD_STARTX + offset, starty = LCD_STARTY + offset;
    const uint8_t endx = startx + LCD_WIDTH - (2 * offset);
    const uint8_t endy = starty + LCD_HEIGHT - (2 * offset);

    // Background
    CGUI::lcd.setRect(startx, starty, endx, endy, true, WHITE);

    // Border
    CGUI::lcd.setRect(startx, starty, endx, endy, false, BLUE);

    const uint8_t vy = starty + 5;
    const uint8_t ty = endy - 5 - FONT_SMALL_HEIGHT;
    const uint8_t gsy = vy + FONT_SMALL_HEIGHT + 2, gw = 4 * FONT_SMALL_WIDTH;
    const uint8_t gh = (ty - gsy - 5);

    uint8_t x = startx + 5;
    for (uint8_t i=0; i<DATA_END; ++i)
    {
        char strval[5];
        itoa(robotData[i], strval, 10);

        const uint8_t tw = strlen_P(dataLabels[i]) * FONT_SMALL_WIDTH;
        const uint8_t vw = strlen(strval) * FONT_SMALL_WIDTH;

        const uint8_t gy = gsy + (gh - map(robotData[i], 0, getDataMax(i), 0, gh));
        if (tw > gw) // text width longer than graph width
        {
            CGUI::lcd.setStrSmall_P(dataLabels[i], x, ty, BLUE, WHITE);

            const uint8_t gx = x + ((tw - gw) / 2);
            CGUI::lcd.setRect(gx, gy, gx + gw, gsy + gh, true, GREEN);
            CGUI::lcd.setStrSmall(strval, x + ((tw - vw) / 2), vy, RED, WHITE);

            x += tw + 7;
        }
        else // text width shorter than graph width
        {
            CGUI::lcd.setStrSmall_P(dataLabels[i], x + (gw - tw), ty, BLUE, WHITE);
            CGUI::lcd.setRect(x, gy, x + gw, gsy + gh, true, GREEN);
            CGUI::lcd.setStrSmall(strval, x + ((gw - vw) / 2), vy, RED, WHITE);
            x += gw + 7;
        }
    }
}

void CMainStatWidget::handleKeyRelease(uint8_t key)
{

}
