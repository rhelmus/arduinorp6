#ifndef SHARED_H
#define SHARED_H

enum
{
    TWI_CMD_SETDATA = 0,
    TWI_CMD_REQDATA,
    TWI_CMD_LOG
};

enum
{
    BATTERY = 0,
    MOTOR_LSPEED,
    MOTOR_RSPEED,
    MOTOR_LCURRENT,
    MOTOR_RCURRENT,
    LIGHT_L,
    LIGHT_R,
    SHARP_IR,
    SERVOPOS,

    DATA_END
};

enum EReqData
{
    REQ_NONE = 0,
    REQ_PING
};

#endif // SHARED_H
