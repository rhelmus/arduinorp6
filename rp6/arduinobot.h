#ifndef ARDUINOBOT_H
#define ARDUINOBOT_H

#include <stdint.h>
#include <stdlib.h>

typedef enum { TASK_DRIVE, TASK_COLLISION, TASK_SCAN, TASK_ROTATE } ETask;

void setTask(ETask task);
void TWILog(const char *txt);
void TWILog_P(const char *txt);
void setServoPos(uint8_t pos);

inline int getRandom(int min, int max) { return (rand() % (max - min)) + min; }

extern uint8_t sharpIRDistance;

#endif // ARDUINOBOT_H
