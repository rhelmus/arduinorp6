#ifndef MAINSTATWIDGET_H
#define MAINSTATWIDGET_H

#include "interface.h"
#include "widget.h"

class CMainStatWidget : public CWidget
{
    bool fullUpdate;

    static const char *dataLabels[DATA_END];

    void reDraw(void);

public:
    CMainStatWidget(void);

    void handleKeyRelease(uint8_t key);
    void onActivate(void) { fullUpdate = true; }
};

#endif // MAINSTATWIDGET_H
