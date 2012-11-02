#include "../shared/shared.h"
#include "arduinobot.h"
#include "collisiontask.h"
#include "drivetask.h"
#include "rotatetask.h"
#include "scantask.h"

#include "RP6RobotBaseLib.h"
#include "RP6I2CmasterTWI.h"

static const uint8_t I2CSlaveAddress = 10;
static ETask currentTask;
uint8_t sharpIRDistance;

void TWIDataReqHandler(uint8_t id)
{
    if (id == REQ_PING)
        ; // Nothing
    else if (id == REQ_SHARPIR)
    {
        I2CTWI_getReceivedData(&sharpIRDistance, 1);
        writeString_P("Received Sharp IR: "); writeInteger(sharpIRDistance, DEC); writeChar('\n');
        statusLEDs.LED1 = statusLEDs.LED4 = (sharpIRDistance < 40);
        updateStatusLEDs();
    }
}

void TWIErrorHandler(uint8_t errorState)
{
    writeString_P("\nI2C ERROR - TWI STATE: 0x");
    writeInteger(errorState, HEX);
    writeChar('\n');
}

void TWILog(const char *txt)
{
#define MAX_LOG_CHARS 15
    static char msgbuf[MAX_LOG_CHARS + 1];
    msgbuf[0] = TWI_CMD_LOG;
    strncpy(&msgbuf[1], txt, MAX_LOG_CHARS);
    I2CTWI_transmitBytes(I2CSlaveAddress, msgbuf, MAX_LOG_CHARS + 1);
#undef MAX_LOG_CHARS
}

void TWILog_P(const char *txt)
{
#define MAX_LOG_CHARS 15
    static char msgbuf[MAX_LOG_CHARS + 1];
    msgbuf[0] = TWI_CMD_LOG;
    strncpy_P(&msgbuf[1], txt, MAX_LOG_CHARS);
    I2CTWI_transmitBytes(I2CSlaveAddress, msgbuf, MAX_LOG_CHARS + 1);
#undef MAX_LOG_CHARS
}

void setServoPos(uint8_t pos)
{
    I2CTWI_transmit2Bytes(I2CSlaveAddress, TWI_CMD_SETSERVO, pos);
}

void updateTWI(void)
{
    if (!I2CTWI_isBusy())
    {
        if (getStopwatch1() > 1000)
        {
            I2CTWI_transmit4Bytes(I2CSlaveAddress, TWI_CMD_SETDATA, BATTERY,
                                  (adcBat >> 8), adcBat);

            I2CTWI_transmit3Bytes(I2CSlaveAddress, TWI_CMD_SETDATA, MOTOR_LSPEED,
                                  getLeftSpeed());
            I2CTWI_transmit3Bytes(I2CSlaveAddress, TWI_CMD_SETDATA, MOTOR_RSPEED,
                                  getRightSpeed());

            I2CTWI_transmit4Bytes(I2CSlaveAddress, TWI_CMD_SETDATA, MOTOR_LCURRENT,
                                  (adcMotorCurrentLeft >> 8), adcMotorCurrentLeft);
            I2CTWI_transmit4Bytes(I2CSlaveAddress, TWI_CMD_SETDATA, MOTOR_RCURRENT,
                                  (adcMotorCurrentRight >> 8), adcMotorCurrentRight);

            I2CTWI_transmit4Bytes(I2CSlaveAddress, TWI_CMD_SETDATA, LIGHT_L,
                                  (adcLSL >> 8), adcLSL);
            I2CTWI_transmit4Bytes(I2CSlaveAddress, TWI_CMD_SETDATA, LIGHT_R,
                                  (adcLSR >> 8), adcLSR);

            setStopwatch1(0);
        }

        if (getStopwatch2() > 50)
        {
            // Ping
            I2CTWI_transmit2Bytes(I2CSlaveAddress, TWI_CMD_REQDATA, REQ_PING);
            I2CTWI_requestDataFromDevice(I2CSlaveAddress, REQ_PING, 1);
            setStopwatch2(0);
        }
    }
}

void updateACSState(void)
{
    if (obstacle_left && !isStopwatch3Running())
        startStopwatch3();
    else if (!obstacle_left && isStopwatch3Running())
    {
        stopStopwatch3();
        setStopwatch3(0);
    }

    if (obstacle_right && !isStopwatch4Running())
        startStopwatch4();
    else if (!obstacle_right && isStopwatch4Running())
    {
        stopStopwatch4();
        setStopwatch4(0);
    }

    statusLEDs.LED2 = obstacle_right;
    statusLEDs.LED5 = obstacle_left;

    updateStatusLEDs();
}

void updateSharpIR(void)
{
    if (getStopwatch6() > 45)
    {
        I2CTWI_transmit2Bytes(I2CSlaveAddress, TWI_CMD_REQDATA, REQ_SHARPIR);
        I2CTWI_requestDataFromDevice(I2CSlaveAddress, REQ_SHARPIR, 1);
        setStopwatch6(0);
    }
}

void setTask(ETask task)
{
    currentTask = task;
    switch (task)
    {
    case TASK_DRIVE: initDriveTask(); break;
    case TASK_COLLISION: initCollisionTask(); break;
    case TASK_SCAN: initScanTask(); break;
    case TASK_ROTATE: initRotateTask(); break;
    }
}

void doTask(void)
{
    switch (currentTask)
    {
    case TASK_DRIVE: doDriveTask(); break;
    case TASK_COLLISION: doCollisionTask(); break;
    case TASK_SCAN: doScanTask(); break;
    case TASK_ROTATE: doRotateTask(); break;
    }
}

int main(void)
{
    initRobotBase();
    
    I2CTWI_initMaster(100);
    I2CTWI_setRequestedDataReadyHandler(TWIDataReqHandler);
    I2CTWI_setTransmissionErrorHandler(TWIErrorHandler);
    
    setLEDs(0b111111);
    mSleep(500);
    setLEDs(0b000000);
    
    powerON();
    setACSPwrLow();

    srand(readADC(ADC_LS_L) + readADC(ADC_LS_R)); // Should be fairly random

    setTask(TASK_DRIVE);
    
    startStopwatch1(); // TWI status update
    startStopwatch2(); // TWI ping
    // Stopwatch3: continueous left ACS collision time
    // Stopwatch4: continueous right ACS collision time
    // Stopwatch5: used by drive task
    startStopwatch6(); // update Sharp IR
    // Stopwatch7: used by scan task

    setServoPos(90);

    TWILog_P(PSTR("RP6 init"));

    while(true)  
    {
        updateTWI();
        updateACSState();
        updateSharpIR();
        doTask();
        task_I2CTWI();
        task_RP6System();
    }

    return 0;
}
