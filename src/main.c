#include "stm32f401xe.h"
#include <stdint.h>
#include <stdbool.h>

volatile uint32_t ms_ticks = 0;

void SysTick_Handler(void) {
    ms_ticks++;
}

void systick_init(void) {
    SysTick_Config(SystemCoreClock / 1000);
}

static void delayMs(uint32_t ms)
{
    uint32_t start = ms_ticks;
    while ((ms_ticks - start) < ms);
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
    GPIOA->MODER &= ~(GPIO_MODER_MODER5);
    GPIOA->MODER |= (GPIO_MODER_MODER5_0);

    // PB5 (pin 29) -> output -> red led
    GPIOB->MODER &= ~(GPIO_MODER_MODER5);
    GPIOB->MODER |= (GPIO_MODER_MODER5_0);

    // PC3 (pin 37) -> output -> blue ed
    GPIOC->MODER &= ~(GPIO_MODER_MODER3);
    GPIOC->MODER |= (GPIO_MODER_MODER3_0);

    // PC11 (pin 2) -> output -> green led
    GPIOC->MODER &= ~(GPIO_MODER_MODER11);
    GPIOC->MODER |= (GPIO_MODER_MODER11_0);
}

void initGPIOSwitch ()
{
    // PC4 (pin 34) -> input -> right switch
    GPIOC->MODER &= ~(GPIO_MODER_MODER4);
    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD4; // clear les bits du pull
    GPIOC->PUPDR |= GPIO_PUPDR_PUPD4_0; // pull-up (01

    // PB14 (pin 28) -> input -> middle switch
    GPIOB->MODER &= ~(GPIO_MODER_MODER14);
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD14; // clear les bits du pull
    GPIOB->PUPDR |= GPIO_PUPDR_PUPD14_0; // pull-up (01)

    // PB13 (pin 30) -> input -> left switch
    GPIOB->MODER &= ~(GPIO_MODER_MODER13);
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
    SystemCoreClockUpdate();
    systick_init();
    init();

    uint16_t delay = 500;
    while (1)
    {
        setGreenLedOn();
        delayMs(delay);
        setRedLedOn();
        delayMs(delay);
        setBlueLedOn();
        delayMs(delay);
        setLedsOff();
        delayMs(delay);
    }
}
