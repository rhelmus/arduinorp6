#ifndef WIDGET_H
#define WIDGET_H

#include <stdint.h>

class CWidget
{
    bool dirty;
    CWidget *next;

    virtual void handleKey(uint8_t key) { }
    virtual void reDraw(void) = 0;

protected:
    CWidget(void) : dirty(true), next(0) { }

public:
    void draw(void) { if (dirty) { reDraw(); dirty = false; } }
    void markDirty(void) { dirty = true; }
    CWidget *nextWidget(void) const { return next; }
    void setNextWidget(CWidget *w) { next = w; }
};

#endif // WIDGET_H
