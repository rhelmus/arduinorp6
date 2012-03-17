#include "gui.h"
#include "interface.h"
#include "menu.h"

extern CGUI GUI;

CMenu mainMenu;

#define MAKE_MENUITEM(v, s) \
    const char v##_txt[] PROGMEM = s; \
    SMenuItem v(v##_txt)

MAKE_MENUITEM(firstItem, "First item");
MAKE_MENUITEM(secondItem, "Second item");
MAKE_MENUITEM(thirdItem, "Third item");

#undef MAKE_MENUITEM

void initInterface()
{
    pinMode(PIN_BUTTON1, INPUT);
    digitalWrite(PIN_BUTTON1, HIGH);
    pinMode(PIN_BUTTON2, INPUT);
    digitalWrite(PIN_BUTTON2, HIGH);
    pinMode(PIN_BUTTON3, INPUT);
    digitalWrite(PIN_BUTTON3, HIGH);

    GUI.addWidget(&mainMenu);
    GUI.setActiveWidget(&mainMenu);

    mainMenu.addItem(&firstItem);
    mainMenu.addItem(&secondItem);
    mainMenu.addItem(&thirdItem);
}

void updateInterface()
{
    GUI.run();
}
