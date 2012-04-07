#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdint.h>

enum
{
    TWI_CMD_SETDATA = 0
};

enum
{
    BATTERY = 0,
    MOTOR_LSPEED,
    MOTOR_RSPEED,

    DATA_END
};

void initInterface(void);
void updateInterface(void);

#endif // INTERFACE_H
