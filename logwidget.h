#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include "widget.h"

class CLogWidget : public CWidget
{
public:
    enum { MAX_LOG_ENTRIES = 12, MAX_LOG_CHARS = 15 };

private:
    struct SLogEntry
    {
        char text[MAX_LOG_CHARS];
        uint32_t time;
    };

    void reDraw(void);

    SLogEntry logEntries[MAX_LOG_ENTRIES];
    uint8_t lastEntryIndex;

public:
    CLogWidget(void) : lastEntryIndex(0) { }

    void handleKeyRelease(uint8_t key) { close(); }

    void addLogEntry(const char *txt);
    void clear(void) { lastEntryIndex = 0; markDirty(); }
};

#endif // LOGWIDGET_H
