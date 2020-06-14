#include "SPI_Drivers.h"
#include <stddef.h>

static void SPI_TXE_Interrupt_handel(SPI_Handel_t *SPIHandel);
static void SPI_RNXE_Interrupt_handel(SPI_Handel_t *SPIHandel);

/********************************************************************************************
										peripheral clock enable and disable api
*********************************************************************************************/
void SPI_PeriCloclControl(SPI_TypeDef *pSPIX, uint8_t ENorDI)
{
	if(ENorDI == ENABLE)
	{
		if(pSPIX == SPI1)
		{
			RCC->APB2ENR |= (1 << 12 ); // 12 is bit position of SPI in APB2ENR register
		}
		else if(pSPIX == SPI2)
		{
			RCC->APB1ENR |= (1 << 14);
		}
		else if(pSPIX == SPI3)
		{
			RCC->APB1RSTR |= (1 << 15 );
		}
	}
	else
	{
		if(pSPIX == SPI1)
		{
			RCC->APB2ENR &=~ (1 << 12 ); // 12 is bit position of SPI in APB2ENR register
		}
		else if(pSPIX == SPI2)
		{
			RCC->APB1ENR &=~ (1 << 14);
		}
		else if(pSPIX == SPI3)
		{
			RCC->APB1ENR &=~ (1 << 15 );
		}	
	}
}


void SPI_PeripheralControl(SPI_TypeDef *pSPIX , uint8_t ENorDI)
{
	if(ENorDI == ENABLE)
	{
		pSPIX->CR1 |= (1 << 6);
	}else
	{
		pSPIX->CR1 &= ~(1 << 6);
	}
}
/*************************************************************************************************
										SPI initialization and de initialization
*************************************************************************************************/
void SPI_Init(SPI_Handel_t *pSPIHandel)
{
	SPI_PeriCloclControl(pSPIHandel->pSPIX, ENABLE);
	/********* 1.MODE********/
	//mode of Device is provided by user through handel type varialbe.
	//we just have to configure MSTR i.e third bit of SPI_CR1
	pSPIHandel->pSPIX->CR1 |= (pSPIHandel->SPICOnfig.SPI_Mode << 2);
	
	//*************2.Direction of SPI 
	if(pSPIHandel->SPICOnfig.SPI_Direction == SPI_BUS_CONFIG_FD)
	{
		//cleare BIDIMODE to enable 2line unidirectional communication
		pSPIHandel->pSPIX->CR1 &=~ (1 << 15); // 15 is bit pos of BIDI mode
	}
	else if(pSPIHandel->SPICOnfig.SPI_Direction == SPI_BUS_CONFIG_HD)
	{
		//Set BIDIMODE to enable 1line bidirectional communication
		pSPIHandel->pSPIX->CR1 |= (1 << 15); // 15 is bit pos of BIDI mode
	}
	else if(pSPIHandel->SPICOnfig.SPI_Direction == SPI_BUS_CONFIG_SIMPLEX_RXONLY)
	{
		//Set BIDIMODE to enable 1line bidirectional communication
		pSPIHandel->pSPIX->CR1 |= (1 << 15); // 15 is bit pos of BIDI mode in SPI_CR1 register
		
		//set RXONLY Bit in SPI_CR1
		pSPIHandel->pSPIX->CR1 |= (1 << 10);// 10 is bit pos of RXONLY in SPI_CR1 register
	}
	
	//***************3.Data frame formate of SPI
	pSPIHandel->pSPIX->CR1 |= (pSPIHandel->SPICOnfig.SPI_DataSize << 11); // 11 is bit pos of DFF in SPI_CR2 register
	
	//***************4 Clock phase and polarity 
	pSPIHandel->pSPIX->CR1 |= (pSPIHandel->SPICOnfig.SPI_CLKPhase << 0);
	pSPIHandel->pSPIX->CR1 |= (pSPIHandel->SPICOnfig.SPI_CLKPolarity << 1);
	
	//****************5. Slave select mode.
	
	pSPIHandel->pSPIX->CR1 |= ( pSPIHandel->SPICOnfig.SPI_SSM << 9);
}



void SPI_DeInit(SPI_TypeDef *pSPIX)
{
	if(pSPIX == SPI1)
	{
		RCC->APB2RSTR |= (1 << 12 ); // 12 is bit position of SPI in APB2ENR register
	}
	else if(pSPIX == SPI2)
	{
		RCC->APB1RSTR |= (1 << 14);
	}
	else if(pSPIX == SPI3)
	{
		RCC->APB1RSTR |= (1 << 15 );
	}	

}

/*********************************************************************************************
*										SPI send data and receive data using interrupt
*********************************************************************************************/

uint8_t SPI_SendDataIT(SPI_Handel_t*pSPIHandel, uint8_t *pTxBuffer, uint32_t len)
{

	uint8_t state = pSPIHandel->TxState;
	
	if(state != SPI_BUSY_IN_TX)
	{
		//Store TxBuffer address in handel register
		pSPIHandel->pTxBuffer = pTxBuffer;
		pSPIHandel->TxLen = len;
		
		//change state to buzy
		pSPIHandel->TxState = SPI_BUSY_IN_TX;
		
		//enable TXEIE in CR2 register to get TXE interupt
		pSPIHandel->pSPIX->CR2 |= (1 << 7);
		
		//call IRQ tx handling
		SPI_IRQHandeling(pSPIHandel);
	}
	return state;
}

uint8_t SPI_ReceiveDataIT(SPI_Handel_t *pSPIHandel, uint8_t *pRxBuffer, uint8_t len)
{
	uint8_t state = pSPIHandel->RxState;
	
	if(state != SPI_BUSY_IN_RX)
	{
		//Store TxBuffer address in handel register
		pSPIHandel->pTxBuffer = pRxBuffer;
		pSPIHandel->RxLen = len;
		
		//change state to buzy
		pSPIHandel->RxState = SPI_BUSY_IN_RX;
		
		//enable RXEIE in CR2 register to get TXE interupt
		pSPIHandel->pSPIX->CR2 |= (1 << 6);
		
		//call IRQ tx handling
		SPI_IRQHandeling(pSPIHandel);
	}
	return state;
}

/***************************************************************************************
																					SPI interrupt enable
*****************************************************************************************/

void SPI_InterruptEN(IRQn_Type IRQNumber)
{
	NVIC_EnableIRQ(IRQNumber);
}


void SPI_IRQHandeling(SPI_Handel_t *pHandel)
{
	uint8_t temp1, temp2;
	
	// 1. Checking for TX interrupt enable and TXE bit
	
	temp1 = (pHandel->pSPIX->CR2 &( 1 << 7)); //7 is the bit position of TXEIE
	temp2 = (pHandel->pSPIX->SR &(1 << 1)); // 1 is the bit position of TXE
	if( temp1 & temp2)
	{
	
	SPI_TXE_Interrupt_handel(pHandel);
	
	}
	
	// 2.Checing for RX interrupt enable and RNXE bit
	
	temp1 = (pHandel->pSPIX->CR2 & (1 << 6)); // 6 is bit position for RXNEIE flag
	temp2 = (pHandel->pSPIX->SR & ( 1 << 0)); // 0 is bit position for RXNE flag
	
	if(temp1 & temp2)
	{
	SPI_RNXE_Interrupt_handel(pHandel);
	}
}

static void SPI_TXE_Interrupt_handel(SPI_Handel_t *SPIHandel)
{
	//in this i want to send data.
	//so first check if DFF is 1 or 0 i.e data is 8bit or 16bit
	
	if(SPIHandel->pSPIX->CR1 & (1 << 11))
	{
		SPIHandel->pSPIX->DR |= *((uint16_t*)SPIHandel->pTxBuffer);
		SPIHandel->TxLen--;
		SPIHandel->TxLen--;
	}
	else
	{
		SPIHandel->pSPIX->DR |= *(SPIHandel->pTxBuffer);
		SPIHandel->TxLen--;	
	}
	
	if(!SPIHandel->TxLen)
	{
		SPIHandel->pSPIX->CR2 &=~ (1 << 7);
		SPIHandel->TxState = SPI_READY;
		SPIHandel->TxLen = 0;
		SPIHandel->pTxBuffer = NULL;
	}
}


static void SPI_RNXE_Interrupt_handel(SPI_Handel_t *SPIHandel)
{
	//in this i want to send data.
	//so first check if DFF is 1 or 0 i.e data is 8bit or 16bit
	
	if(SPIHandel->pSPIX->CR1 & (1 << 11))
	{
		 *((uint16_t*)SPIHandel->pTxBuffer)  =SPIHandel->pSPIX->DR;
		SPIHandel->RxLen -= 2;
		SPIHandel->pRxBuffer--;
		SPIHandel->pRxBuffer--;
	}
	else
	{
		*(SPIHandel->pTxBuffer) = (uint8_t)SPIHandel->pSPIX->DR;
		SPIHandel->RxLen--;
		SPIHandel->pRxBuffer--;
	}
	
	if(!SPIHandel->RxLen)
	{
		SPIHandel->pSPIX->CR2 &=~ (1 << 6);
		SPIHandel->RxState = SPI_READY;
		SPIHandel->RxLen = 0;
		SPIHandel->pRxBuffer = NULL;
	
	}
}
