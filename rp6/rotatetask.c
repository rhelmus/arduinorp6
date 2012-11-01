#include "arduinobot.h"
#include "rotatetask.h"

#include "RP6RobotBaseLib.h"

static uint16_t targetAngle;

void initRotateTask()
{
    if (targetAngle > 180)
        rotate(60, LEFT, 360 - targetAngle, false);
    else
        rotate(60, RIGHT, targetAngle, false);
}

void doRotateTask()
{
    if (isMovementComplete())
        setTask(TASK_DRIVE);
    else
    {
        // UNDONE: check bumpers
    }
}

void setRotateTaskTarget(uint16_t angle)
{
    targetAngle = angle;
}
