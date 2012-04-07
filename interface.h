#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdint.h>

enum
{
    TWI_CMD_SETLSPEED = 0,
    TWI_CMD_SETRSPEED,
    TWI_CMD_SETBATTERY
};

struct SRobotData
{
    uint8_t lspeed, rspeed;
    uint16_t battery;

    SRobotData(void) : lspeed(0), rspeed(0), battery(0) { }
};

void initInterface(void);
void updateInterface(void);

#endif // INTERFACE_H
