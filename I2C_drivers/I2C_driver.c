#include "I2C_driver.h"


void I2C_PeripheralClockControl(I2C_TypeDef *pI2CX,uint8_t ENorDI)
{
	if(ENorDI == ENABLE)
	{
		if(pI2CX == I2C1)
		{
			RCC->APB1ENR |= (1 << 21);
			//21 is bit definition of I2C1 in APB1ENR register
		}
		else if(pI2CX == I2C2)
		{
			RCC->APB1ENR |= (1 << 22);
			//22 is bit definition of I2C2 in APB1ENR register
		}
	}
	else
	{
		if(pI2CX == I2C1)
		{
			RCC->APB1ENR &=~ (1 << 21);
			//21 is bit definition of I2C1 in APB1ENR register
		}
		else if(pI2CX == I2C2)
		{
			RCC->APB1ENR &=~ (1 << 22);
			//22 is bit definition of I2C2 in APB1ENR register
		}
	}
}



void I2CInit(I2C_Handle_t *pI2Cx)
{
	/////////////////CR1 Register configurations/////////////////
	//1.Enable peripheral clock of the given peripheral
	I2C_PeripheralClockControl(pI2Cx->pI2CX,ENABLE);
	
	//2.Enable acking
	pI2Cx->pI2CX->CR1 |= (1 << 10);
	//10 is bit position for ACK
	
	/////////////////CR2 Register configurations///////////////////
	//3. Configure FREQ feild
	//since we are using HSI , our peripheral clock freq is 16Mhz, so we need to devide with 1000000 so to get 16

	pI2Cx->pI2CX->CR2 |= ((8) << 0); //since i am considering internal HSI clock..so its value is 8MHz
	
	
	//4.Now configure CCR register of I2C
	if(pI2Cx->I2Cconfig->Speed <= I2C_SCLSPEED_SM)
	{
		//since it is SM then we dont need to set mode bit in CCR as its already 0

		
		//we have hsi clock as 8Mhz , for SM Duty cycle is 50%
		//so  from reference manual formula is      Th=CCR*Tpclk
		//so for 100000 hz freq, time period is 10us
		//so Th=Tl= (5/1000000)
		//so from formula, CCR= Th/Tpclk
		//so CCR = (5/100000)/(1/8,000000)
		// CCR = 5*16 = 80
		pI2Cx->pI2CX->CCR |= 40; 
	}
	else
	{
	//since this is FM, so we have 2 duty cycles
	//first of all enable Fast mode in CCR reg by setting bit15 as 1
		pI2Cx->pI2CX->CCR |= (1 << 15);
		
		//we need to set duty cycle also for FM mode
		pI2Cx->pI2CX->CCR |= (pI2Cx->I2Cconfig->FMDutycycle << 14);
		//now for duty cycle also we have 2 values
		if(pI2Cx->I2Cconfig->FMDutycycle == 0)
		{
			//Th = CCR*Tpclk
			//Tl = 2 * CCR * Tpclk which is nothing but Tl = 2*(Th)
			//Th + Tl = 3Th which is 3*CCR*Tpclk
			//5us = 3*CCR *1/16000000
			//(5*16)/3 = CCR
			pI2Cx->pI2CX->CCR |= ((5*8)/3);
		}
		else
		{
			//Th = 9*CCR*Tpclk
			//Tl = 16*CCR*Tpclk
			//Th + Tl = 25*CCR*Tpclk
			//so desired freq is 400Khz, so time period is 2.5us
			//5*8/25 = CCR
			pI2Cx->pI2CX->CCR |= ((5*8)/25);
		
		}
	}

}
void I2CDeinit(I2C_Handle_t *pI2Cx)
{
	if(pI2Cx->pI2CX == I2C1)
	{
		RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
	
	}
	else if(pI2Cx->pI2CX == I2C2)
	{
		RCC->APB1RSTR |= RCC_APB1RSTR_I2C2RST;
	}
}

void I2CSendData(I2C_Handle_t *pI2CHandel, uint8_t *pTxBuffer,uint32_t len,	uint8_t Slaveaddr)
{
	//generate the start bit
	pI2CHandel->pI2CX->CR1 |= (1 << 8);
	
	//now once start condition is initiated, then SB bit is set. so check until SB bit is set
	while(!((pI2CHandel->pI2CX->SR1) & 0x01));
	//now SB bit is cleared by reading SR register and writing data to DR register.
	//so writing Slave address to DR
	//now this address phase has 7ad bits and in read write bit.
	//Since we are writing so 8th bit should be 0
	Slaveaddr |= Slaveaddr << 1;
	Slaveaddr &=~(1);
	pI2CHandel->pI2CX->DR = Slaveaddr;
	
	//now once add is sent, ADDR event is set.
	while(!((pI2CHandel->pI2CX->SR1 >> 1) & 0x01));
	//so we need to clear it by reading SR1 followed by reading SR2
	uint8_t tempreg;
	tempreg = pI2CHandel->pI2CX->SR1;
	tempreg = pI2CHandel->pI2CX->SR2;
	
	//now as we clear the ADDR, we get other event EV8_1 i.e TxE = 1;
	//meaning data register is empty and shift register is empty
	//so we need to write data in data register.
	while(len > 0)
	{
		//check TxE is set or not, 
		while(!((pI2CHandel->pI2CX->SR1 >> 7) & 0x01));
		
		//now transfer data into data register
		pI2CHandel->pI2CX->DR = *pTxBuffer;
		
		pTxBuffer++;
		len--;
	}
	
	//.When len becomes zero wait for TXE=1 and BTF=1 before generating tstop condition
	//Note: When TXE=1 and BTF=1, means both SR and DR is empty and next transmission should begin
	//When BTF=1 SCL will be stretched
	while(!((pI2CHandel->pI2CX->SR1 >> 2) & 0x01));
	while(!((pI2CHandel->pI2CX->SR1 >> 7) & 0x01));
	
	//Now generate the stop condition and master need to wait on completion of stop condition
	pI2CHandel->pI2CX->CR1 |= (1 << 8);
	
}

void I2CReceiv(I2C_Handle_t *pI2CHandel, uint8_t *RxBuffer, uint8_t len, uint8_t Slaveaddr)
{
	///generate the start bit
	pI2CHandel->pI2CX->CR1 |= (1 << 8);
	//2.now once start condition is initiated, then SB bit is set. so check until SB bit is set
	//Note: until SB is cleared, and DR is written with data SR will be set
	while(!((pI2CHandel->pI2CX->SR1) & 0x01));

	//now SB bit is cleared by reading SR register and writing data to DR register.
	//so writing Slave address to DR
	//now this address phase has 7ad bits and in read write bit.
	//Since we are writing so 8th bit should be 0
	Slaveaddr |= Slaveaddr << 1;
	Slaveaddr |= (1);
	pI2CHandel->pI2CX->DR = Slaveaddr;
	//now once add is sent, ADDR event is set.
	while(!((pI2CHandel->pI2CX->SR1 >> 1) & 0x01));
	//so we need to clear it by reading SR1 followed by reading SR2
	uint8_t tempreg;
	//procedure to read only 1byte from slave
	if(len == 1)
	{
		//Disable acking
			pI2CHandel->pI2CX->CR1 &=~ (1 << 10);
		//clear ADDR flag
		tempreg = pI2CHandel->pI2CX->SR1;
		tempreg = pI2CHandel->pI2CX->SR2;
		//wait until RXNE = 1
		while(!((pI2CHandel->pI2CX->SR1 >> 6) & 0x01 ));
		//Generate stop condition
		pI2CHandel->pI2CX->CR1 |= (1 << 9);
		//read data from DR
		*RxBuffer = pI2CHandel->pI2CX->DR;
	}
	if(len > 1)
	{

		// Clear the ADDR flag
		tempreg = pI2CHandel->pI2CX->SR1;
		tempreg = pI2CHandel->pI2CX->SR2;
		//Read data until len becomes zero

		for(uint32_t i = len; i> 0; i--)
		{
			//wait until RXNE becomes 1
			while(!((pI2CHandel->pI2CX->SR1 >> 6) & 0x01 ));
			if(i == 2)
			{
				// clear ack bit
				pI2CHandel->pI2CX->CR1 &=~ (1 << 10);
				//generate stop condition
				pI2CHandel->pI2CX->CR1 |= (1 << 9);
			}
			//read the data from data register
			*RxBuffer = pI2CHandel->pI2CX->DR;
			//increment buffer address
			RxBuffer++;
		}
	}

	//re enable acking
	pI2CHandel->pI2CX->CR1 |= (1 << 10);
}
