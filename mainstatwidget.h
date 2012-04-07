#ifndef MAINSTATWIDGET_H
#define MAINSTATWIDGET_H

#include "widget.h"

class CMainStatWidget : public CWidget
{
    void reDraw(void);

public:
    CMainStatWidget(void);

    void handleKeyRelease(uint8_t key);
};

#endif // MAINSTATWIDGET_H
