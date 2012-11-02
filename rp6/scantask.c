#include "arduinobot.h"
#include "rotatetask.h"
#include "scantask.h"

#include "RP6RobotBaseLib.h"

enum
{
    SCAN_ANGLE_INTERVAL = 30,
    SCAN_POSITIONS = 180 / SCAN_ANGLE_INTERVAL
};

static struct
{
    uint8_t servoPos;
    uint16_t idleWaitTime;
    uint8_t measurements; // Measurements from current angle
    uint16_t summedDist; // Summed distance from current angle
    uint8_t scanResults[SCAN_POSITIONS];

    uint16_t furthestSummedDist;
    uint8_t bestServoPos;
} taskState;

static uint16_t getBestAngle(void)
{
    uint8_t freelist[SCAN_POSITIONS];
    uint8_t freepositions = 0;
    uint8_t furthestdist = 0, furthestservopos;

    memset(freelist, 0, sizeof(freelist));

    for (uint8_t i=0; i<SCAN_POSITIONS; ++i)
    {
        if ((taskState.scanResults[i] <= 20) || (taskState.scanResults[i] >= 150))
        {
            freelist[freepositions] = SCAN_ANGLE_INTERVAL * i;
            ++freepositions;
        }
        else if (furthestdist < taskState.scanResults[i])
        {
            furthestdist = taskState.scanResults[i];
            furthestservopos = SCAN_ANGLE_INTERVAL * i;
        }
    }

    uint8_t idealspos;

    if (freepositions > 0)
        idealspos = freelist[getRandom(0, freepositions-1)];
    else if (furthestdist > 40)
        idealspos = furthestservopos;
    else
        return 180; // Nothing interesting here, turn

    if (idealspos < 90)
        return 360 - (90 - idealspos);
    else
        return idealspos - 90;
}

void initScanTask()
{
    stop();
    setServoPos(0);
    taskState.servoPos = 0;
    taskState.idleWaitTime = 1500;
    taskState.measurements = 0;
    memset(taskState.scanResults, 0, sizeof(taskState.scanResults));
    taskState.summedDist = 0;
    taskState.furthestSummedDist = 0;
    setStopwatch7(0);
    startStopwatch7();
}

void doScanTask()
{
    // HACK: sometimes robot doesn't stop moving
    if (getLeftSpeed() || getRightSpeed())
        stop();

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
        if (sharpIRDistance <= 20)
            taskState.summedDist += 160;
        else
            taskState.summedDist += sharpIRDistance;

        ++taskState.measurements;

        if (taskState.measurements == 3)
        {
            taskState.scanResults[taskState.servoPos / SCAN_ANGLE_INTERVAL] =
                    taskState.summedDist / taskState.measurements;

            if (taskState.furthestSummedDist < taskState.summedDist)
            {
                taskState.furthestSummedDist = taskState.summedDist;
                taskState.bestServoPos = taskState.servoPos;
            }

            taskState.servoPos += 30;
            if (taskState.servoPos > 180)
            {
                stopStopwatch7();
                setServoPos(90);

#if 0
                if (taskState.furthestSummedDist <= 70) // Nothing interesting here, turn
                    setRotateTaskTarget(180);
                else if (taskState.bestServoPos < 90)
                    setRotateTaskTarget(360 - (90 - taskState.bestServoPos));
                else
                    setRotateTaskTarget(taskState.bestServoPos - 90);
#endif
                setRotateTaskTarget(getBestAngle());
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
