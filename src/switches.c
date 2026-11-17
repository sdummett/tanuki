#include "stm32f401xe.h"
#include "switches.h"

void switches_init(void)
{
    // PC4 (pin 34) -> input -> right switch
    GPIOC->MODER &= ~(GPIO_MODER_MODER4);
    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD4;  // clear les bits du pull
    GPIOC->PUPDR |= GPIO_PUPDR_PUPD4_0; // pull-up (01)

    // PB14 (pin 28) -> input -> middle switch
    GPIOB->MODER &= ~(GPIO_MODER_MODER14);
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD14;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPD14_0;

    // PB13 (pin 30) -> input -> left switch
    GPIOB->MODER &= ~(GPIO_MODER_MODER13);
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD13;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPD13_0;
}

bool switch_right_is_pressed(void)
{
    return (GPIOC->IDR & GPIO_IDR_ID4) == 0U;
}

bool switch_middle_is_pressed(void)
{
    return (GPIOB->IDR & GPIO_IDR_ID14) == 0U;
}

bool switch_left_is_pressed(void)
{
    return (GPIOB->IDR & GPIO_IDR_ID13) == 0U;
}
