#include "RP6RobotBaseLib.h"
#include "RP6I2CmasterTWI.h"

enum
{
    TWI_CMD_SETLSPEED = 0,
    TWI_CMD_SETRSPEED,
    TWI_CMD_SETBATTERY,
};

const uint8_t I2CSlaveAddress = 10;

void I2C_transmissionError(uint8_t errorState)
{
    writeString_P("\nI2C ERROR - TWI STATE: 0x");
    writeInteger(errorState, HEX);
    writeChar('\n');
}

int main(void)
{
    initRobotBase();
    
    I2CTWI_initMaster(100);  
    I2CTWI_setTransmissionErrorHandler(I2C_transmissionError);
    
    setLEDs(0b111111);
    mSleep(500);
    setLEDs(0b000000);
    
    powerON();
    
    startStopwatch1();
    
    while(true)  
    {
        if (getStopwatch1() > 1000)
        {
            I2CTWI_transmit2Bytes(I2CSlaveAddress, TWI_CMD_SETLSPEED, 10);
            I2CTWI_transmit2Bytes(I2CSlaveAddress, TWI_CMD_SETRSPEED, 15);           
            I2CTWI_transmit3Bytes(I2CSlaveAddress, TWI_CMD_SETBATTERY, (adcBat >> 8), adcBat);
            setStopwatch1(0);
        }
        task_I2CTWI();
        task_RP6System();
    }
    return 0;
}