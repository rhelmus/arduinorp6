#include "arduinobot.h"
#include "collisiontask.h"

#include "RP6RobotBaseLib.h"

void initCollisionTask()
{
    move(70, BWD, DIST_MM(100), false); // Move away
}

void doCollisionTask()
{
    if (isMovementComplete())
        setTask(TASK_SCAN);
}
