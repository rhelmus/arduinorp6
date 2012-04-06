#include "gui.h"
#include "menu.h"

CMenu::CMenu() : firstItem(0), activeItem(0), itemCount(0), maxItemLength(0)
{
}

void CMenu::reDraw()
{
    if (!firstItem)
        return;

    const uint8_t w = (maxItemLength * FONT_WIDTH) + (2 * ITEM_XPAD);
    const uint8_t h = (itemCount * ITEM_YSPACE) + (2 * ITEM_YPAD);
    const uint8_t startx = LCD_STARTX + ((LCD_WIDTH - w) / 2);
    const uint8_t starty = LCD_STARTY + ((LCD_HEIGHT - h) / 2);

    // Background
    CGUI::lcd.setRect(startx, starty, startx + w, starty + h, true, RED);

    // Border
    CGUI::lcd.setRect(startx, starty, startx + w, starty + h, false, YELLOW);

    // Items
    SMenuItem *item = firstItem;
    uint8_t ity = starty + ITEM_YPAD;
    while (item)
    {
        const uint8_t itx = startx + ((w - (FONT_WIDTH * strlen_P(item->text))) / 2);

        if (item == activeItem)
            CGUI::lcd.setStrSmall_P(item->text, itx, ity, YELLOW, BLUE);
        else
            CGUI::lcd.setStrSmall_P(item->text, itx, ity, YELLOW, RED);

        ity += ITEM_YSPACE;

        if (ity > (starty + h))
            break;

        item = item->next;
    }
}

void CMenu::handleKeyRelease(uint8_t key)
{
    Serial.print("key pressed: "); Serial.println(key);

    if (key == 1) // Go up
    {
        if (activeItem == firstItem)
        {
            // Wrap to end
            while (activeItem->next)
                activeItem = activeItem->next;
        }
        else
        {
            SMenuItem *it = firstItem;
            while (it->next && (it->next != activeItem))
                it = it->next;
            if (it)
                activeItem = it;
        }
        markDirty();
    }
    else if (key == 2) // Go down
    {
        activeItem = activeItem->next;
        if (!activeItem)
            activeItem = firstItem;
        markDirty();
    }
    else // Activate
    {
        if (callBack)
            callBack(activeItem);
    }
}

void CMenu::addItem(SMenuItem *item)
{
    if (!firstItem)
        firstItem = activeItem = item;
    else
    {
        SMenuItem *it = firstItem;
        while (it->next)
            it = it->next;
        it->next = item;
    }

    const uint8_t l = strlen_P(item->text);
    if (l > maxItemLength)
        maxItemLength = l;

    ++itemCount;
}

