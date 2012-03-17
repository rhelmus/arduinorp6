#include "gui.h"

LCDShield CGUI::lcd;

CGUI::CGUI() : firstWidget(0), activeWidget(0)
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
    w->markDirty();
}

void CGUI::run()
{
    if (activeWidget)
    {
        // UNDONE: Check key press

        activeWidget->draw();
    }
}
