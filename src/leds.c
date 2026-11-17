#include "stm32f401xe.h"
#include "leds.h"

void leds_init(void)
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

void led_green_on(void)
{
    GPIOC->BSRR = GPIO_BSRR_BS11;
}

void led_red_on(void)
{
    GPIOB->BSRR = GPIO_BSRR_BS5;
}

void led_blue_on(void)
{
    GPIOC->BSRR = GPIO_BSRR_BS3;
}

void led_green_off(void)
{
    GPIOC->BSRR = GPIO_BSRR_BR11;
}

void led_red_off(void)
{
    GPIOB->BSRR = GPIO_BSRR_BR5;
}

void led_blue_off(void)
{
    GPIOC->BSRR = GPIO_BSRR_BR3;
}

void leds_all_off(void)
{
    GPIOA->BSRR = GPIO_BSRR_BR5; // onboard LD2
    led_green_off();
    led_red_off();
    led_blue_off();
}

void leds_all_on(void)
{
    GPIOA->BSRR = GPIO_BSRR_BS5; // onboard LD2
    led_green_on();
    led_red_on();
    led_blue_on();
}
