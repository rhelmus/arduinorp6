#include "arduinobot.h"
#include "drivetask.h"

#include "RP6RobotBaseLib.h"

static struct
{
    uint8_t servoPos;
    uint16_t delay, moveDelay;
    uint8_t checkLeft;
    uint8_t measurements;
    uint16_t summedDist;
} taskState;

static uint8_t leftCollision(void)
{
    return (obstacle_left && isStopwatch3Running() && (getStopwatch3() >= 50));
}

static uint8_t rightCollision(void)
{
    return (obstacle_right && isStopwatch4Running() && (getStopwatch4() >= 50));
}

static void moveRobot(int8_t extral, int8_t extrar)
{
    // HACK: slightly higher right speed to compensate inequal motor speeds
    moveAtSpeed(90 + extral, 95 + extrar);
}

void initDriveTask()
{
    taskState.servoPos = 90;
    taskState.delay = 2000;
    taskState.checkLeft = getRandom(0, 1);
    taskState.measurements = 0;
    taskState.summedDist = 0;
    taskState.moveDelay = 0;

    changeDirection(FWD);
    moveRobot(0, 0);

    setStopwatch5(0);
    startStopwatch5();
}

void doDriveTask()
{
    // Check bumpers
    if (bumper_left || bumper_right || leftCollision() || rightCollision())
    {
        TWILog_P(PSTR("Bumper/ACS hit"));
        stopStopwatch5();
        setTask(TASK_COLLISION);
    }
    else if ((taskState.servoPos == 90) && (sharpIRDistance > 20) && (sharpIRDistance < 40) &&
             (!taskState.moveDelay || (getStopwatch5() > 500)))
    {
        TWILog_P(PSTR("Close sharp IR"));
        stopStopwatch5();
        setTask(TASK_SCAN);
    }
    else if (getStopwatch5() > taskState.delay)
    {
        if (taskState.servoPos == 90)
        {
            if  (sharpIRDistance > 60)
            {
                if (taskState.checkLeft)
                    taskState.servoPos = getRandom(0, 60);
                else
                    taskState.servoPos = getRandom(120, 180);

                setServoPos(taskState.servoPos);
                taskState.checkLeft = !taskState.checkLeft;
            }

            taskState.delay = 1000;
        }
        else
        {
            if (sharpIRDistance <= 20)
                taskState.summedDist += 160;
            else
                taskState.summedDist += sharpIRDistance;

            ++taskState.measurements;

            if (taskState.measurements == 3)
            {
                const uint8_t avgdist = taskState.summedDist / taskState.measurements;
                if ((avgdist > 20) && (avgdist <= 50))
                {
                    if (taskState.servoPos < 90)
                        moveRobot(10, -10);
                    else
                        moveRobot(-10, 10);

                    taskState.moveDelay = getRandom(1000, 2000);
                }

                taskState.measurements = 0;
                taskState.summedDist = 0;

                taskState.servoPos = 90;
                setServoPos(90);
                taskState.delay = 500;
            }
            else
                taskState.delay = 50;
        }

        if (taskState.moveDelay)
        {
            if (taskState.moveDelay < getStopwatch5())
            {
                moveRobot(0, 0);
                taskState.moveDelay = 0;
            }
            else
                taskState.moveDelay -= getStopwatch5();
        }

        setStopwatch5(0);
    }
}
