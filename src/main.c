#include "stm32f401xe.h"
#include <stdint.h>
#include <stdbool.h>

static void delay(void)
{
    for (volatile uint32_t i = 0; i < 500000; i++)
    {
        __NOP();
    }
}

void initGPIOClocks ()
{
    // Enable GPIOA clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    // Enable GPIOB clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    // Enable GPIOC clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
}

void initGPIOLeds ()
{
    // PA5 (pin 11) = output = onboard green led
    GPIOA->MODER &= ~(GPIO_MODER_MODE5);
    GPIOA->MODER |= (GPIO_MODER_MODE5_0);

    // PB5 (pin 29) -> output -> red led
    GPIOB->MODER &= ~(GPIO_MODER_MODE5);
    GPIOB->MODER |= (GPIO_MODER_MODE5_0);

    // PC3 (pin 37) -> output -> blue ed
    GPIOC->MODER &= ~(GPIO_MODER_MODE3);
    GPIOC->MODER |= (GPIO_MODER_MODE3_0);

    // PC11 (pin 2) -> output -> green led
    GPIOC->MODER &= ~(GPIO_MODER_MODE11);
    GPIOC->MODER |= (GPIO_MODER_MODE11_0);
}

void initGPIOSwitch ()
{
    // PC4 (pin 34) -> input -> right switch
    GPIOC->MODER &= ~(GPIO_MODER_MODE4);
    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD4; // clear les bits du pull
    GPIOC->PUPDR |= GPIO_PUPDR_PUPD4_0; // pull-up (01

    // PB14 (pin 28) -> input -> middle switch
    GPIOB->MODER &= ~(GPIO_MODER_MODE14);
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD14; // clear les bits du pull
    GPIOB->PUPDR |= GPIO_PUPDR_PUPD14_0; // pull-up (01)

    // PB13 (pin 30) -> input -> left switch
    GPIOB->MODER &= ~(GPIO_MODER_MODE13);
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD13; // clear les bits du pull
    GPIOB->PUPDR |= GPIO_PUPDR_PUPD13_0; // pull-up (01)
}

void init ()
{
    initGPIOClocks ();
    initGPIOLeds();
    initGPIOSwitch();
}

void setGreenLedOn ()
{
    GPIOC->BSRR = GPIO_BSRR_BS11;
}
void setRedLedOn ()
{
    GPIOB->BSRR = GPIO_BSRR_BS5;
}
void setBlueLedOn ()
{
    GPIOC->BSRR = GPIO_BSRR_BS3;
}

void setGreenLedOff ()
{
    GPIOC->BSRR = GPIO_BSRR_BR11;
}
void setRedLedOff ()
{
    GPIOB->BSRR = GPIO_BSRR_BR5;
}
void setBlueLedOff ()
{
    GPIOC->BSRR = GPIO_BSRR_BR3;
}

bool rightSwitchIsPressed(void)
{
    return (GPIOC->IDR & GPIO_IDR_ID4) == 0U;
}

bool middleSwitchIsPressed(void)
{
    return (GPIOB->IDR & GPIO_IDR_ID14) == 0U;
}

bool leftSwitchIsPressed(void)
{
    return (GPIOB->IDR & GPIO_IDR_ID13) == 0U;
}

void setLedsOff ()
{
    GPIOA->BSRR = GPIO_BSRR_BR5; // onboard LD2
    setGreenLedOff();
    setRedLedOff();
    setBlueLedOff();
}
void setLedsOn () {
    GPIOA->BSRR = GPIO_BSRR_BS5; // onboard LD2
    setGreenLedOn();
    setRedLedOn();
    setBlueLedOn();
}

int main(void)
{
    init();

    while (1)
    {
        // volatile uint32_t pc4Raw = (GPIOC->IDR >> 4U) & 1U;
        if (leftSwitchIsPressed())
        {
            setGreenLedOn();
            delay();
        }
        else if (middleSwitchIsPressed())
        {
            setRedLedOn();
            delay();
        }
        else if (rightSwitchIsPressed())
        {
            setBlueLedOn();
            delay();
        }
        else
        {
            setLedsOff();
        }
    }
}
