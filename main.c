/*
 * main.c
 * author: MK, TA, KS
 */

#include "stm32g0xx.h"

#define PWMPERIOD 10000
#define SAMPLE 10000
volatile uint16_t X = 0;
volatile uint8_t state =0;
void mts_led_init(void)
{
	RCC->IOPENR |= (1U << 2); // PORTC CLOCK ENABLE
	GPIOC->MODER &= ~(3U << 2*6); // MODE REG. RESET FOR PiN6
	GPIOC->MODER |= (1U <<2*6); // PiN6 OUTPUT
	GPIOC->BRR |= (1U << 6);  // BiT RESET
	GPIOC->ODR |= (1U << 6);
}
void led_set(void){	GPIOC->ODR |= (1U << 6); /* PiN6 ON */ }
void led_clear(void) {	GPIOC->BRR |= (1U << 6); /*TO RESET WRiTE 1 BiT RESET REG */ }
void led_toggle(void){	 GPIOC->ODR ^= (1U << 6);	; }

void TIM3_IRQHandler(void)
{
     // clear interrupt status
    if (TIM3->DIER & 0x01) {
        if (TIM3->SR & 0x01) {
            TIM3->SR &= ~(1U << 0); //interrupt clear
        }
    }
    TIM3->CCR3 = X;
    if (!state)	X+=1;
    else 		X-=1;

    if (X > 1900){ 	state = 1;   TIM3->CNT = 0;   }
    if( X < 30)  {  state=0;	 TIM3->CNT = 0;   }


}

void mts_TIM3_init(void)
{
	RCC->IOPENR |= (1U << 1);
	RCC->APBENR1 |= RCC_APBENR1_TIM3EN ;

	GPIOB->MODER &= ~(3U << 2*0	);
	GPIOB->MODER |= (2U << 2*0	);

	GPIOB->AFR[0] |= (1U << 4*0	); //Bpin 5 alternate func 1

	TIM3->CR1 = 0;
	// CLOCK / PSC+1 = CNT   16 000 000/15
	// ARR = CNT / ARR+1
	TIM3->PSC = 15 ;
	TIM3->ARR = 2000 ;
	TIM3->CNT = 0 ;

    // reset duty cycle on channel 2
    TIM3->CCR3 = 0 ;

    // enable channel 2 in capture/compare register
    // set output cap comp 2 mode as pwm (0b110 or 0x6 in bits 6-4) preload arr enable
    TIM3->CCMR2 |= 0x68U;
    // Preload Enable
    //  TIM3->CCMR1 |= ( 0x1U << 11 )	;
    // enable capture/compare ch3 enable reg
    TIM3->CCER |= (	0x100U )	 	;
    // enable update interrupt
    TIM3->DIER |= (1 << 0) ;

    NVIC_SetPriority(TIM3_IRQn, 1) ;
    // enable TIM3 IRQ from NVIC
    NVIC_EnableIRQ(TIM3_IRQn) ;

    // Enable Timer 3 module (CEN, bit0)
    TIM3->CR1 |= 1U ;
}

int main(void) {

	mts_TIM3_init();
	mts_led_init();
	led_set();

    while(1) {


    }
    return 0;
}

