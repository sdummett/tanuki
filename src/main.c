#include "stm32f401xe.h"

static void delay(void)
{
    for (volatile uint32_t i = 0; i < 1000000; i++)
    {
        __NOP();
    }
}

int main(void)
{
    // Enable GPIOA clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // PA5 = output
    GPIOA->MODER &= ~(3U << 10);
    GPIOA->MODER |=  (1U << 10);

    while (1)
    {
        // LD2 ON
        GPIOA->BSRR = GPIO_BSRR_BS5;
        delay();

        // LD2 OFF
        GPIOA->BSRR = GPIO_BSRR_BR5;
        delay();
    }
}
