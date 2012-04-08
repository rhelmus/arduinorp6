#ifndef WIDGET_H
#define WIDGET_H

#include <stdint.h>

class CWidget
{
public:
    typedef void (*TClosedCB)(CWidget *);

private:
    bool dirty;
    CWidget *next;
    TClosedCB closedCB;

    virtual void reDraw(void) = 0;

protected:
    CWidget(void) : dirty(true), next(0), closedCB(0) { }

    void close(void) { closedCB(this); }

public:
    virtual void handleKeyRelease(uint8_t key) { }
    virtual void onActivate(void) { }

    void draw(void) { if (dirty) { reDraw(); dirty = false; } }
    void markDirty(void) { dirty = true; }
    void setClosedCB(TClosedCB cb) { closedCB = cb; }
    CWidget *nextWidget(void) const { return next; }
    void setNextWidget(CWidget *w) { next = w; }
};

#endif // WIDGET_H
