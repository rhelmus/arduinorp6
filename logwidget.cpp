#include "logwidget.h"

#include "gui.h"

void CLogWidget::reDraw()
{
    const uint8_t offset = 5;
    const uint8_t totw = MAX_LOG_CHARS * FONT_SMALL_WIDTH + 2 * offset;
    const uint8_t toth = MAX_LOG_ENTRIES * FONT_SMALL_HEIGHT + 2 * offset;
    const uint8_t startx = LCD_STARTX + ((LCD_WIDTH - totw) / 2);
    const uint8_t starty = LCD_STARTY + ((LCD_HEIGHT - toth) / 2);

    CGUI::lcd.setRect(startx, starty, startx + totw, starty + toth, true, BLACK);
    CGUI::lcd.setRect(startx, starty, startx + totw, starty + toth, false, WHITE);

    uint8_t y = starty + offset;
    for (uint8_t i=0; i<lastEntryIndex; ++i)
    {
        uint8_t x = startx + offset;
        /*CGUI::lcd.setCharSmall('<', x, y, ORANGE, BLACK);
        x += FONT_SMALL_WIDTH;

        char tstr[8];
        itoa(logEntries[i].time, tstr, 10);
        CGUI::lcd.setStrSmall(tstr, x, y, ORANGE, BLACK);
        x += (strlen(tstr) * FONT_SMALL_WIDTH);

        CGUI::lcd.setCharSmall('>', x, y, ORANGE, BLACK);
        x += (FONT_SMALL_WIDTH * 2);*/

        CGUI::lcd.setStrSmall(logEntries[i].text, x, y, GRAY, BLACK);

        y += FONT_SMALL_HEIGHT;
    }
}

void CLogWidget::addLogEntry(const char *txt)
{
    if (lastEntryIndex == (MAX_LOG_ENTRIES-1))
    {
        // Log is full, pop first message
        for (uint8_t i=0; i<(MAX_LOG_ENTRIES-1); ++i)
        {
            logEntries[i].time = logEntries[i+1].time;
            strcpy(logEntries[i].text, logEntries[i+1].text);
        }
    }

    logEntries[lastEntryIndex].time = millis() / 1000;
    strncpy(logEntries[lastEntryIndex].text, txt, MAX_LOG_CHARS);
    logEntries[lastEntryIndex].text[MAX_LOG_CHARS-1] = 0;

    if (lastEntryIndex < (MAX_LOG_ENTRIES-1))
        ++lastEntryIndex;

    markDirty();
}
