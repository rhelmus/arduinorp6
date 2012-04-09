#ifndef SHARED_H
#define SHARED_H

enum
{
    TWI_CMD_SETDATA = 0,
    TWI_CMD_REQDATA,
    TWI_CMD_LOG,
    TWI_CMD_SETSERVO
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
    SHARPIR,
    SERVOPOS,

    DATA_END
};

enum EReqData
{
    REQ_NONE = 0,
    REQ_PING,
    REQ_SHARPIR
};

#endif // SHARED_H
