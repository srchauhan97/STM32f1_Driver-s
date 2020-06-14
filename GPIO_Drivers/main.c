#include "GPIO_Driver.h"

int main()
{
	GPIO_Handel_t myhandel;
	myhandel.GPIOX = GPIO_PORTC;
	myhandel.PInConfig.outputtype = GPIO_OTYPE_PP;
	myhandel.PInConfig.pinno = 13;
	myhandel.PInConfig.pintypewithspeed = GPIO_PIN_MODE_OUTPUT_2MHZ;
	GPIO_Init(&myhandel);
	
	GPIO_INterrupt(PORTB_IT,4,RISING_EDGE);
	GPIO_InterruptEnable(4,EXTI4_IRQn);
	
	while(1);
}

void EXTI4_IRQHandler()
{
	Clear_GPIO_Interrupt(4);
	//extra code can be written here based on applications.
}
