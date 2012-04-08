#include "../shared/shared.h"

#include "RP6RobotBaseLib.h"
#include "RP6I2CmasterTWI.h"

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
        task_I2CTWI();
        task_RP6System();
    }
    return 0;
}
