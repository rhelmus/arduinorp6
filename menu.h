#ifndef MENU_H
#define MENU_H

#include "gui.h"
#include "widget.h"

#include <stdint.h>

struct SMenuItem
{
    const char *text;
    SMenuItem *next;
    SMenuItem(const char *t) : text(t), next(0) { }
};

class CMenu: public CWidget
{
    enum
    {
        ITEM_XPAD = 5,
        ITEM_YPAD = 5, // No, not iPad!
        ITEM_YSPACE = FONT_HEIGHT + 1
    };

    SMenuItem *firstItem, *activeItem;
    uint8_t itemCount;
    uint8_t maxItemLength;
    
    void reDraw();

public:
    CMenu(void);
    
    void addItem(SMenuItem *item);
};

#endif
