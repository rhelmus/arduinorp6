#include "../shared/shared.h"

#include "RP6RobotBaseLib.h"
#include "RP6I2CmasterTWI.h"

const uint8_t I2CSlaveAddress = 10;

typedef enum { ACS_LOW=0, ACS_MED, ACS_HIGH } EPowerState;
typedef enum { NAV_CRUISE, NAV_TURN, NAV_FULLSTOP } ENavState;

typedef struct
{
    uint8_t left:1, right:1;
    EPowerState powerState:2;
} SACSCollisionState;

SACSCollisionState ACSCollisionState;
ENavState navState;
uint8_t sharpIRDistance;

void TWIDataReqHandler(uint8_t id)
{
    if (id == REQ_PING)
        ; // Nothing
    else if (id == REQ_SHARPIR)
    {
        I2CTWI_getReceivedData(&sharpIRDistance, 1);
        writeString_P("Received Sharp IR: "); writeInteger(sharpIRDistance, DEC); writeChar('\n');
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
    I2CTWI_transmit3Bytes(I2CSlaveAddress, TWI_CMD_SETSERVO, pos);
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
    ACSCollisionState.left = obstacle_left;
    ACSCollisionState.right = obstacle_right;

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

    statusLEDs.byte = 0;

    switch (ACSCollisionState.powerState)
    {
    case ACS_LOW:
        statusLEDs.LED3 = obstacle_right;
        statusLEDs.LED6 = obstacle_left;
        break;
    case ACS_MED:
        statusLEDs.LED2 = obstacle_right;
        statusLEDs.LED5 = obstacle_left;
        break;
    case ACS_HIGH:
        statusLEDs.LED1 = obstacle_right;
        statusLEDs.LED4 = obstacle_left;
        break;
    }

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

void updateACSPower(void)
{
    ACSCollisionState.left = false;
    ACSCollisionState.right = false;
    stopStopwatch3();
    setStopwatch3(0);
    stopStopwatch4();
    setStopwatch4(0);

    switch (ACSCollisionState.powerState)
    {
        case ACS_LOW: setACSPwrLow(); break;
        case ACS_MED: setACSPwrMed(); break;
        case ACS_HIGH: setACSPwrHigh(); break;
    }
}

void setNavState(ENavState state)
{
    navState = state;
    switch (state)
    {
        case NAV_CRUISE: changeDirection(FWD); moveAtSpeed(80, 80); break;
        case NAV_TURN: break; // Set somewhere else
        case NAV_FULLSTOP: stop(); break;
    }
}

uint8_t leftCollision(void)
{
    return (ACSCollisionState.left && isStopwatch3Running() &&
            getStopwatch3() >= 50);
}

uint8_t rightCollision(void)
{
    return (ACSCollisionState.right && isStopwatch4Running() &&
            getStopwatch4() >= 50);
}

void navigate(void)
{
    if (navState == NAV_CRUISE)
    {
        // Check bumpers
        if (bumper_left || bumper_right)
        {
            TWILog_P(PSTR("Bumper hit"));

            move(70, BWD, DIST_MM(100), true); // Move away

            if (bumper_left && bumper_right)
                rotate(60, RIGHT, 180, false);
            else if (bumper_left)
                rotate(60, RIGHT, 60, false);
            else // right
                rotate(60, LEFT, 60, false);

            setNavState(NAV_TURN);
            return;
        }

        // Check ACS
        const uint8_t cleft = leftCollision();
        const uint8_t cright = rightCollision();

        if (cleft || cright)
        {
            TWILog_P(PSTR("ACS hit"));

            if (ACSCollisionState.powerState == ACS_LOW)
            {
                move(70, BWD, DIST_MM(60), true); // Move away

                if (cleft && cright)
                    rotate(60, RIGHT, 180, false); // UNDONE: randomize
                else if (cleft)
                    rotate(60, RIGHT, 60, false);
                else // right
                    rotate(60, LEFT, 60, false);

                setNavState(NAV_TURN);
            }
            else
            {
                uint8_t left = 80, right = 80;

                if (cleft && cright)
                {
                    if (ACSCollisionState.powerState == ACS_MED)
                        left = right = 60; // UNDONE
                }
                else if (cleft)
                {
                    left = 100;
                    if (ACSCollisionState.powerState == ACS_MED)
                        right = 0;
                    else
                        right = 50;
                }
                else // right
                {
                    right = 100;
                    if (ACSCollisionState.powerState == ACS_MED)
                        left = 0;
                    else
                        left = 50;
                }

                if (!isStopwatch5Running())
                    startStopwatch5();

                setStopwatch5(0); // Update

                moveAtSpeed(left, right);
            }
        }
        else
        {
            if (isStopwatch5Running() && (getStopwatch5() > 1000))
            {
                // Didn't update stopwatch for a while,
                // revert to normal speed
                moveAtSpeed(80, 80);
                stopStopwatch5();
                setStopwatch5(0);
            }
        }
    }
    else if (navState == NAV_TURN)
    {
        if (bumper_left || bumper_right)
            setNavState(NAV_FULLSTOP); // Emergency stop
        else if (isMovementComplete())
            setNavState(NAV_CRUISE);
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

    ACSCollisionState.powerState = ACS_LOW;
    updateACSPower();
//    setNavState(NAV_CRUISE);
    
    startStopwatch1(); // TWI status update
    startStopwatch2(); // TWI ping
    // Stopwatch3: continueous left ACS collision time
    // Stopwatch4: continueous right ACS collision time
    // Stopwatch5: drive with altered speed (e.g. to avoid collisions)
    startStopwatch6(); // update Sharp IR

    TWILog_P(PSTR("RP6 init"));
    
    while(true)  
    {
        updateTWI();
        updateACSState();
        updateSharpIR();
        navigate();
        task_I2CTWI();
        task_RP6System();
    }

    return 0;
}
