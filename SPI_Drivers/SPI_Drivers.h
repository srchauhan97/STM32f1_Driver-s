#ifndef SPI_DRIVER_H
#define SPI_DRIVER_H

#include "stm32f10x.h"
#include "GPIO_Driver.h"


typedef struct
{
  uint32_t SPI_Mode;               /*  Specifies the SPI operating mode. */                        
  uint32_t SPI_Direction;          /*  Specifies the SPI Directional mode state. */
  uint32_t SPI_DataSize;           /*  Specifies the SPI data size. */
  uint32_t SPI_CLKPolarity;        /*  Specifies the serial clock steady state. */                                   
  uint32_t SPI_CLKPhase;           /*  Specifies the clock active edge for the bit capture. */
  uint32_t SPI_SSM;                /*  Specifies whether the NSS signal is managed by
                                    hardware (NSS pin) or by software using the SSI bit. */
    
  uint32_t SPI_BaudRatePrescaler;  /*  Specifies the Baud Rate prescaler value which will be
                                    used to configure the transmit and receive SCK clock. */
}SPI_Config_t;


typedef struct
{
	SPI_TypeDef *pSPIX;
	SPI_Config_t SPICOnfig;
	
	uint8_t *pTxBuffer;          // to store the app tx buffer address
	uint8_t *pRxBuffer;          // to store the app rx buffer address
	uint32_t TxLen;
	uint32_t RxLen;
	uint8_t  TxState;
	uint8_t RxState;
}SPI_Handel_t;

//***************************Macros for SPI configuration************************

//Mode of SPI
#define SPI_MODE_MASTER                           1
#define SPI_MODE_SLAVE                            0

//for SPI direction
#define SPI_BUS_CONFIG_FD                         1
#define SPI_BUS_CONFIG_HD                         2
#define SPI_BUS_CONFIG_SIMPLEX_RXONLY             3

//For data size
#define SPI_DFF_8BITS                             0
#define SPI_DFF_16BITS                            1
 
//For baudrate
#define SPI_SCLK_SPEED_DIV2                       0
#define SPI_SCLK_SPEED_DIV4                       1
#define SPI_SCLK_SPEED_DIV8                       2
#define SPI_SCLK_SPEED_DIV16			                3
#define SPI_SCLK_SPEED_DIV32                      4
#define SPI_SCLK_SPEED_DIV64                      5
#define SPI_SCLK_SPEED_DIV128                     6
#define SPI_SCLK_SPEED_DIV256                     7

//For CPOL
#define SPI_CPOL_HIGH                             1
#define SPI_CPOL_LOW                              0

//For CPHA
#define SPI_CPHA_HIGH                             1
#define SPI_CPHA_LOW                              0

//Other MAcros
#define ENABLE                                    1
#define DISABLE                                   0


//SPI bus state
#define SPI_READY                                 0
#define SPI_BUSY_IN_TX                            1
#define SPI_BUSY_IN_RX                            2

/*********************************************************************************************
*														API prototypes for SPI								
***********************************************************************************************/

//to enable peripheral clock control of SPI
void SPI_PeriCloclControl(SPI_TypeDef *pSPIX, uint8_t ENorDI);

//SPI init and Deinit
void SPI_Init(SPI_Handel_t *pSPIHANDEL);
void SPI_DeInit(SPI_TypeDef *pSPIX);

//SPI perripheral control
void SPI_PeripheralControl(SPI_TypeDef *pSPIX , uint8_t ENorDI);

//spi send and receive data using interrupt
uint8_t SPI_SendDataIT(SPI_Handel_t*pSPIHandel, uint8_t *pTxBuffer, uint32_t len);
uint8_t SPI_ReceiveDataIT(SPI_Handel_t *pSPIHandel, uint8_t *pRxBuffer, uint8_t len);

//SPI interrupt enable and handling
void SPI_InterruptEN(IRQn_Type IRQNumber);
void SPI_IRQHandeling(SPI_Handel_t *pHandel);
#endif
