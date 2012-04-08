#ifndef SHARED_H
#define SHARED_H

enum
{
    TWI_CMD_SETDATA = 0
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

    DATA_END
};

#endif // SHARED_H
