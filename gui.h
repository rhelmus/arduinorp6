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
    FONT_HEIGHT = 16
};

class CGUI
{
    CWidget *firstWidget, *activeWidget;

public:
    CGUI(void);

    void init(void);
    void addWidget(CWidget *w);
    void setActiveWidget(CWidget *w);
    void run(void);

    static LCDShield lcd;
};

#endif // GUI_H
