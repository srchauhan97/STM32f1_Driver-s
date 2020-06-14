#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

#include "stm32f10x.h"



#define ENABLE      1
#define DISABLE     0

/*
 * @I2C_SCLSpeed
 */
#define I2C_SCLSPEED_SM                                  100000
#define I2C_SCLSPEED_FM4K                                400000
#define I2C_SCLSPEED_FM2K                                200000


/*
*	@I2C_dutyCycle
*/
#define I2C_FM_DUTY_2                                     0
#define I2C_FM_DUTY_16_9                                  1



typedef struct
{
	uint32_t Mode;
	uint32_t Speed;
	uint32_t Ackcontrol;
	uint32_t FMDutycycle;
}I2C_Config_t;

typedef struct
{
	I2C_TypeDef *pI2CX;
	I2C_Config_t *I2Cconfig;

}I2C_Handle_t;



void I2C_PeripheralClockControl(I2C_TypeDef *pI2CX,uint8_t ENorDI);

void I2CInit(I2C_Handle_t *pI2Cx);
void I2CDeinit(I2C_Handle_t *pI2Cx);

void I2CSendData(I2C_Handle_t *pI2CHandel, uint8_t *pTxBuffer,uint32_t len,	uint8_t Slaveaddr);
void I2CReceiv(I2C_Handle_t *pI2CHandel, uint8_t *RxBuffer, uint8_t len, uint8_t SlaveAddr);

#endif
