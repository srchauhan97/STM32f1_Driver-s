#include "GPIO_Driver.h"


void GPIO_Init(GPIO_Handel_t *pGPIOX)
{
	
	//GPIO Clock enable
	GPIO_ClockEnable(pGPIOX->GPIOX);
	
	//locate the pin number of the GPIO pin
	uint8_t temp1 = 0;
	uint8_t temp2 = 0;
	
	temp1 = pGPIOX->PInConfig.pinno %4;
	temp2 = pGPIOX->PInConfig.pinno /4;
	
	
	//1. configure mode of the pin with speed
	if(temp2 <= 1)
	{
		if(temp2 == 0)
		{
			pGPIOX->GPIOX->CRL |= (pGPIOX->PInConfig.pintypewithspeed << temp1*4);
		}else
		{
			pGPIOX->GPIOX->CRL |= (pGPIOX->PInConfig.pintypewithspeed << ((temp1*4)+16));
		}
	}else
	{
		if(temp2 == 2)
		{
			pGPIOX->GPIOX->CRH |= (pGPIOX->PInConfig.pintypewithspeed << (temp1*4));
		}else
		{
			pGPIOX->GPIOX->CRH |=(pGPIOX->PInConfig.pintypewithspeed << ((temp1*4)+16));
		}
	}
	
	
	//2.Configuration bits setting
	if(pGPIOX->PInConfig.pintypewithspeed == GPIO_PIN_MODE_INPUT)
	{
		//configurations for input
		if(temp2 <= 1)
		{
			if(temp2 == 0)
			{
				pGPIOX->GPIOX->CRL |= (pGPIOX->PInConfig.inputtype << ((temp1*4)+2));
			}else
			{
				pGPIOX->GPIOX->CRL |= (pGPIOX->PInConfig.inputtype << ((temp1*4)+18));
			}
		}else
		{
			if(temp2 == 2)
			{
				pGPIOX->GPIOX->CRH |= (pGPIOX->PInConfig.inputtype << ((temp1*4)+2));
			}else
			{
				pGPIOX->GPIOX->CRH |=(pGPIOX->PInConfig.inputtype << ((temp1*4)+16));
			}
		}
		
	}else
	{
		//configurations for output
		if(temp2 <= 1)
		{
			if(temp2 == 0)
			{
				pGPIOX->GPIOX->CRL |= (pGPIOX->PInConfig.outputtype << ((temp1*4)+2));
			}else
			{
				pGPIOX->GPIOX->CRL |= (pGPIOX->PInConfig.outputtype << ((temp1*4)+18));
			}
		}else
		{
			if(temp2 == 2)
			{
				pGPIOX->GPIOX->CRH |= (pGPIOX->PInConfig.outputtype << ((temp1*4)+2));
			}else
			{
				pGPIOX->GPIOX->CRH |=(pGPIOX->PInConfig.outputtype << ((temp1*4)+16));
			}
		}
	}
}

void GPIO_ClockEnable(GPIO_TypeDef *pGPIOX)
{
	if(pGPIOX  == GPIOA)
	{
		GPIOA_CLOCK_EN;
	}else if(pGPIOX == GPIOB)
	{
		GPIOB_CLOCK_EN;
	}else if(pGPIOX == GPIOC)
	{
		GPIOC_CLOCK_EN;
	}else if(pGPIOX == GPIOD)
	{
		GPIOD_CLOCK_EN;
	}else if(pGPIOX == GPIOE)
	{
		GPIOE_CLOCK_EN;
	}
}


//************************************Read from GPIO pin*****************************

uint8_t GPIOReadfrompin(GPIO_TypeDef *pGPIOX, uint8_t PinNo)
{
	uint8_t value=0;
	value = (uint8_t)((pGPIOX->IDR >> PinNo) & 0x00000001 );
	return value;
}


//************************************Write to GPIO pin*******************************
void GPIOWritetopin(GPIO_TypeDef *pGPIOX, uint8_t PinNo, uint8_t Data)
{
	if(Data == PIN_SET)
	{
		pGPIOX->ODR |= (1 << PinNo);
	}
	else
	{
		pGPIOX->ODR &=~ (1 << PinNo);
	}
}
/*************************************************************************************
*													API for GPIO Interrupts
*************************************************************************************/
/*
ITPORT VALUES ARE

PORTA_IT            0
PORTB_IT            1
PORTC_IT            2
PORTD_IT            3
PORTE_IT            4
*/
void GPIO_INterrupt(uint8_t ITPort	, uint8_t PinNo, edge_select edge)
{
	//enabling AFIO register
	RCC->APB2ENR |= (1 <<0x00);
	
	uint8_t temp = 0;
	if(PinNo <= 3)
	{
		temp = PinNo % 4;
		AFIO->EXTICR[0] |= ITPort << (temp*4);
	}
	if(PinNo >3 && PinNo <=7)
	{
		temp = PinNo % 4;
		AFIO->EXTICR[1] |= ITPort << (temp*4);
	}
	if(PinNo >7 && PinNo <=11)
	{
		temp = PinNo % 4;
		AFIO->EXTICR[2] |= ITPort << (temp*4);	
	}
	if (PinNo >11 && PinNo <= 15)
	{
		temp = PinNo % 4;
		AFIO->EXTICR[3] |= ITPort << (temp*4);
	}
	//edge configuration of interrupt
	if(edge == RISING_EDGE)
	{
		EXTI->RTSR |= (1 << PinNo);
	}
	if(edge == FALLING_EDGE)
	{
		EXTI->FTSR |= (1 << PinNo);
	}
	if(edge == RISING_FALLING_EDGE)
	{
		EXTI->RTSR |= (1 << PinNo);
		EXTI->FTSR |= (1 << PinNo);
	}
	
	
	
}

void GPIO_InterruptEnable(uint8_t PinNo, IRQn_Type IRQNumber)
{
	//mask EXTI interrupt to NVIC
	EXTI->IMR |= (1 << PinNo);
	
	//enable interrupt in NVIC
	NVIC_EnableIRQ(IRQNumber);
	
	
}

void Clear_GPIO_Interrupt(uint8_t PinNo)
{

	//clear pending register
	EXTI->PR |= (1 << PinNo);
}



