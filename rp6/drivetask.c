#include "arduinobot.h"
#include "drivetask.h"

#include "RP6RobotBaseLib.h"

static uint8_t leftCollision(void)
{
    return (obstacle_left && isStopwatch3Running() && (getStopwatch3() >= 50));
}

static uint8_t rightCollision(void)
{
    return (obstacle_right && isStopwatch4Running() && (getStopwatch4() >= 50));
}

void initDriveTask()
{
    changeDirection(FWD);
    moveAtSpeed(80, 80);
}

void doDriveTask()
{
    // Check bumpers
    if (bumper_left || bumper_right || leftCollision() || rightCollision() ||
        ((sharpIRDistance >= 20) && (sharpIRDistance < 50)))
    {
        TWILog_P(PSTR("Bumper/ACS hit"));
        setTask(TASK_COLLISION);
        return;
    }
}
