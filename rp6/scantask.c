#include "arduinobot.h"
#include "rotatetask.h"
#include "scantask.h"

#include "RP6RobotBaseLib.h"

static struct
{
    uint8_t servoPos;
    uint16_t idleWaitTime;
    uint8_t measurements; // Measurements from current angle
    uint16_t summedDist; // Summed distance from current angle
    uint16_t furthestSummedDist;
    uint8_t bestServoPos;
} taskState;

enum { NO_DIST = 200 };

void initScanTask()
{
    stop();
    setServoPos(0);
    taskState.servoPos = 0;
    taskState.idleWaitTime = 1500;
    taskState.measurements = 0;
    taskState.summedDist = 0;
    taskState.furthestSummedDist = NO_DIST;
    setStopwatch7(0);
    startStopwatch7();
}

void doScanTask()
{
    if (taskState.idleWaitTime)
    {
        if (getStopwatch7() >= taskState.idleWaitTime)
        {
            taskState.idleWaitTime = 0;
            setStopwatch7(0);
        }
    }
    else if (getStopwatch7() > 50)
    {
        taskState.summedDist += sharpIRDistance;
        ++taskState.measurements;

        if (taskState.measurements == 3)
        {
            if ((taskState.furthestSummedDist == NO_DIST) || (taskState.furthestSummedDist < taskState.summedDist))
            {
                taskState.furthestSummedDist = taskState.summedDist;
                taskState.bestServoPos = taskState.servoPos;
            }

            taskState.servoPos += 30;
            if (taskState.servoPos > 180)
            {
                stopStopwatch7();
                setServoPos(90);

                if (taskState.furthestSummedDist <= 50) // Nothing interesting here, turn
                    setRotateTaskTarget(180);
                else if (taskState.bestServoPos < 90)
                    setRotateTaskTarget(360 - taskState.bestServoPos);
                else
                    setRotateTaskTarget(taskState.bestServoPos - 90);

                setTask(TASK_ROTATE);
            }
            else
            {
                setServoPos(taskState.servoPos);
                taskState.measurements = 0;
                taskState.summedDist = 0;
                taskState.idleWaitTime = 750;
                setStopwatch7(0);
            }
        }
    }
}
