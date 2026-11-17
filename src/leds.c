#include "stm32f401xe.h"
#include "leds.h"

void initGPIOLeds(void)
{
    // PA5 (pin 11) = output = onboard green led
    GPIOA->MODER &= ~(GPIO_MODER_MODER5);
    GPIOA->MODER |= (GPIO_MODER_MODER5_0);

    // PB5 (pin 29) -> output -> red led
    GPIOB->MODER &= ~(GPIO_MODER_MODER5);
    GPIOB->MODER |= (GPIO_MODER_MODER5_0);

    // PC3 (pin 37) -> output -> blue led
    GPIOC->MODER &= ~(GPIO_MODER_MODER3);
    GPIOC->MODER |= (GPIO_MODER_MODER3_0);

    // PC11 (pin 2) -> output -> green led
    GPIOC->MODER &= ~(GPIO_MODER_MODER11);
    GPIOC->MODER |= (GPIO_MODER_MODER11_0);
}

void setGreenLedOn(void)
{
    GPIOC->BSRR = GPIO_BSRR_BS11;
}

void setRedLedOn(void)
{
    GPIOB->BSRR = GPIO_BSRR_BS5;
}

void setBlueLedOn(void)
{
    GPIOC->BSRR = GPIO_BSRR_BS3;
}

void setGreenLedOff(void)
{
    GPIOC->BSRR = GPIO_BSRR_BR11;
}

void setRedLedOff(void)
{
    GPIOB->BSRR = GPIO_BSRR_BR5;
}

void setBlueLedOff(void)
{
    GPIOC->BSRR = GPIO_BSRR_BR3;
}

void setLedsOff(void)
{
    GPIOA->BSRR = GPIO_BSRR_BR5; // onboard LD2
    setGreenLedOff();
    setRedLedOff();
    setBlueLedOff();
}

void setLedsOn(void)
{
    GPIOA->BSRR = GPIO_BSRR_BS5; // onboard LD2
    setGreenLedOn();
    setRedLedOn();
    setBlueLedOn();
}
