#ifndef GUI_H
#define GUI_H

#include <ColorLCDShield.h>

#include "widget.h"

enum
{
    LCD_STARTX = 2,
    LCD_WIDTH = 128,
    LCD_STARTY = 2,
    LCD_HEIGHT = 128,

    FONT_WIDTH = 8,
    FONT_HEIGHT = 16,

    FONT_SMALL_WIDTH = 6,
    FONT_SMALL_HEIGHT = 8
};

class CGUI
{
public:
    typedef void (*TDrawStatsCB)(void);

private:
    CWidget *firstWidget, *activeWidget;
    uint8_t lastSwState;
    uint32_t lastStatsDraw;
    TDrawStatsCB drawStatsCB;

public:
    CGUI(void);

    void init(void);
    void setDrawStatsCB(TDrawStatsCB cb) { drawStatsCB = cb; }
    void addWidget(CWidget *w);
    void setActiveWidget(CWidget *w);
    void run(uint8_t swstate);

    static LCDShield lcd;
};

#endif // GUI_H
