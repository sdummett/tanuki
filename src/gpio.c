#include "stm32f401xe.h"
#include "gpio.h"

void initGPIOClocks(void)
{
    // Enable GPIOA clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    // Enable GPIOB clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    // Enable GPIOC clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
}
