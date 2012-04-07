#include "gui.h"
#include "interface.h"
#include "mainstatwidget.h"

extern SRobotData robotData;

CMainStatWidget::CMainStatWidget()
{
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

    uint8_t x = startx + 5, y = endy - 5 - FONT_SMALL_HEIGHT;
    CGUI::lcd.setStrSmall_P(PSTR("Bat"), x, y, BLUE, WHITE);

    x += (strlen_P(PSTR("Bat")) * FONT_SMALL_WIDTH) + 5;
    CGUI::lcd.setStrSmall_P(PSTR("L spd"), x, y, BLUE, WHITE);

    x += (strlen_P(PSTR("L spd")) * FONT_SMALL_WIDTH) + 5;
    CGUI::lcd.setStrSmall_P(PSTR("R spd"), x, y, BLUE, WHITE);
}

void CMainStatWidget::handleKeyRelease(uint8_t key)
{

}
