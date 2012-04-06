#include "gui.h"

LCDShield CGUI::lcd;

CGUI::CGUI() : firstWidget(0), activeWidget(0), lastSwState(0)
{
}

void CGUI::init()
{
    lcd.init(PHILLIPS, LCDShield::NORTH);
    lcd.clear(GRAY);
}

void CGUI::addWidget(CWidget *w)
{
    if (!firstWidget)
        firstWidget = w;
    else
    {
        CWidget *it = firstWidget;
        while (it->nextWidget())
            it = it->nextWidget();
        it->setNextWidget(w);
    }
}

void CGUI::setActiveWidget(CWidget *w)
{
    activeWidget = w;
    lcd.clear(GRAY);
    w->markDirty();
}

void CGUI::run(uint8_t swstate)
{
    if (activeWidget)
    {
        if ((lastSwState & (1 << 0)) && !(swstate & (1 << 0)))
            activeWidget->handleKeyRelease(1);
        if ((lastSwState & (1 << 1)) && !(swstate & (1 << 1)))
            activeWidget->handleKeyRelease(2);
        if ((lastSwState & (1 << 2)) && !(swstate & (1 << 2)))
            activeWidget->handleKeyRelease(3);

        activeWidget->draw();
    }

    lastSwState = swstate;
}
