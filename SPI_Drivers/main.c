#include "SPI_Drivers.h"
void delay(void);

void SPI2_GPIOInits(void)
{
	
	AFIO->MAPR |= (1 << AFIO_MAPR_SPI1_REMAP);
	GPIO_Handel_t SPIPins;
	
	SPIPins.GPIOX = GPIOA;
	SPIPins.PInConfig.outputtype = ALT_OTYPE_PP;
	SPIPins.PInConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	SPIPins.PInConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	SPIPins.PInConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	//SCLK
	SPIPins.PInConfig.pinno = GPIO_PIN_NO_13;
	GPIO_Init(&SPIPins);

	//MOSI
//	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
	//GPIO_Init(&SPIPins);

	//MISO
	SPIPins.PInConfig.pinno = GPIO_PIN_NO_14;
	GPIO_Init(&SPIPins);


	//NSS
	SPIPins.PInConfig.pinno = GPIO_PIN_NO_12;
	GPIO_Init(&SPIPins);

}

void SPI2_Inits(void)
{
	SPI_Handel_t SPI2handel;
	SPI2handel.pSPIX = SPI1;
	SPI2handel.SPICOnfig.SPI_Direction = SPI_BUS_CONFIG_SIMPLEX_RXONLY;
	SPI2handel.SPICOnfig.SPI_Mode = SPI_MODE_MASTER;
	SPI2handel.SPICOnfig.SPI_BaudRatePrescaler = SPI_SCLK_SPEED_DIV8;//generates sclk of 2MHz
	SPI2handel.SPICOnfig.SPI_DataSize = SPI_DFF_8BITS;
	SPI2handel.SPICOnfig.SPI_CLKPolarity = SPI_CPOL_LOW;
	SPI2handel.SPICOnfig.SPI_CLKPhase = SPI_CPHA_LOW;
	SPI_Init(&SPI2handel);
}

void GPIO_ButtonInit(void)
{
	GPIO_Handel_t GPIOBtn,GpioLed;

	//this is btn gpio configuration
	GPIOBtn.GPIOX = GPIOA;
	GPIOBtn.PInConfig.pinno = 0;
	GPIOBtn.PInConfig.pintypewithspeed = GPIO_PIN_MODE_INPUT;
	GPIOBtn.PInConfig.inputtype = GPIO_ITYPE_PUPD;
	GPIO_ClockEnable(GPIOBtn.GPIOX);

	GPIO_Init(&GPIOBtn);

	//this is led gpio configuration
	GpioLed.GPIOX = GPIOD;
	GpioLed.PInConfig.pinno = 12;
	GpioLed.PInConfig.pintypewithspeed = GPIO_PIN_MODE_OUTPUT_2MHZ;
	GpioLed.PInConfig.outputtype = GPIO_OTYPE_PP;

	GPIO_ClockEnable(GpioLed.GPIOX);

	GPIO_Init(&GpioLed);

}


int main(void)
{

	GPIO_ButtonInit();

	//this function is used to initialize the GPIO pins to behave as SPI2 pins
	SPI2_GPIOInits();

	//This function is used to initialize the SPI2 peripheral parameters
	SPI2_Inits();

	/*
	* making SSOE 1 does NSS output enable.
	* The NSS pin is automatically managed by the hardware.
	* i.e when SPE=1 , NSS will be pulled to low
	* and NSS pin will be high when SPE=0
	*/

	SPI_InterruptEN(SPI2_IRQn);

	//wait till button is pressed
	while( ! GPIOReadfrompin(GPIOA,0) );

	//to avoid button de-bouncing related issues 200ms of delay
	delay();


	//enable the SPI2 peripheral
	SPI_PeripheralControl(SPI2,ENABLE);


	//Disable the SPI2 peripheral
	SPI_PeripheralControl(SPI2,DISABLE);

	return 0;

}

//error in this line. Need to paas Handel type variable

void SPI1_IRQHandler(void)
{
	SPI_IRQHandeling();
}

void delay(void)
{
	for(int i =0; i<=500000;i++);
}
