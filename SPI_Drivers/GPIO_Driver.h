#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

#include "stm32f10x.h" 


/******************************************************************************************
*  											1. Macros used for GPIO pin initilization
*******************************************************************************************/

//*************************** MODE of GPIO pin( input,output,alt)
#define GPIO_PIN_MODE_INPUT               (uint32_t)0x00
#define GPIO_PIN_MODE_OUTPUT_10MHZ        (uint32_t)0x01
#define GPIO_PIN_MODE_OUTPUT_2MHZ         (uint32_t)0x02
#define GPIO_PIN_MODE_OUTPUT_50MHZ        (uint32_t)0x03

//**************************** Input mode of GPIO pin*****************************************
#define GPIO_ITYPE_ANALOG                 (uint32_t)0x00
#define GPIO_ITYPE_FLOATING               (uint32_t)0x01
#define GPIO_ITYPE_PUPD                   (uint32_t)0x02

//*************************** Output mode of GPIO pin*****************************************
#define GPIO_OTYPE_PP                     (uint32_t)0x00
#define GPIO_OTYPE_OD                     (uint32_t)0x01
#define ALT_OTYPE_PP                      (uint32_t)0x02	
#define ALT_OTYPE_OD                      (uint32_t)0x03

/********************************************************************************************
*															GPIO port address
*********************************************************************************************/
#define GPIO_PORTA                        GPIOA
#define GPIO_PORTB                        GPIOB
#define GPIO_PORTC                        GPIOC
#define GPIO_PORTD                        GPIOD
#define GPIO_PORTE                        GPIOE

/**********************************************************************************************
*														CLock enable macros 
***********************************************************************************************/

#define GPIOA_CLOCK_EN                    (RCC->APB2ENR |= (1 << 2))
#define GPIOB_CLOCK_EN                    (RCC->APB2ENR |= (1 << 3))
#define GPIOC_CLOCK_EN                    (RCC->APB2ENR |= (1 << 4))
#define GPIOD_CLOCK_EN                    (RCC->APB2ENR |= (1 << 5))
#define GPIOE_CLOCK_EN                    (RCC->APB2ENR |= (1 << 6))

/**********************************************************************************************
*														CLock disable macros 
***********************************************************************************************/
#define GPIOA_CLOCK_DI                    (RCC->APB2ENR &=~ (1 << 2))
#define GPIOB_CLOCK_DI                    (RCC->APB2ENR &=~ (1 << 3))
#define GPIOC_CLOCK_DI                    (RCC->APB2ENR &=~ (1 << 4))
#define GPIOD_CLOCK_DI                    (RCC->APB2ENR &=~ (1 << 5))
#define GPIOE_CLOCK_DI                    (RCC->APB2ENR &=~ (1 << 6))

/**********************************************************************************************
*														Port Interrupt Macros
***********************************************************************************************/
#define PORTA_IT                          (uint8_t)0
#define PORTB_IT                          (uint8_t)1
#define PORTC_IT                          (uint8_t)2
#define PORTD_IT                          (uint8_t)3
#define PORTE_IT                          (uint8_t)4

/**********************************************************************************************
*														Structures For GPIO pin initialization
***********************************************************************************************/

typedef struct
{
	uint8_t pinno;
	uint32_t pintypewithspeed;
	uint32_t inputtype;
	uint32_t outputtype;
	
}GPIO_Pinconfig_t;

typedef struct
{
	GPIO_TypeDef *GPIOX;
	GPIO_Pinconfig_t PInConfig;
	
	

}GPIO_Handel_t;


typedef enum
{
	RISING_EDGE,
	FALLING_EDGE,
	RISING_FALLING_EDGE
}edge_select;

/**********************************************************************************************
*														API's for driver
***********************************************************************************************/

void GPIO_Init(GPIO_Handel_t *pGPIOX);


void GPIO_ClockEnable(GPIO_TypeDef *pGPIOX);

uint8_t GPIOReadfrompin(GPIO_TypeDef *pGPIOX, uint8_t PinNo);

void GPIOWritetopin(GPIO_TypeDef *pGPIOX, uint8_t PinNo, uint8_t Data);


void GPIO_INterrupt(uint8_t ITPort, uint8_t PinNo, edge_select select);
void GPIO_InterruptEnable(uint8_t PinNo, IRQn_Type IRQNumber);
void Clear_GPIO_Interrupt(uint8_t PinNo);


//***********************************Extra macros*********************************************

#define PIN_SET                           (uint8_t)1
#define PIN_RESET                         (uint8_t)0


#endif
