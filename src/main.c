#include "stm32f401xe.h"
#include <stdint.h>
#include <stdbool.h>

#define CS_LOW()    (GPIOB->BSRR = GPIO_BSRR_BR_6)
#define CS_HIGH()   (GPIOB->BSRR = GPIO_BSRR_BS_6)

#define DC_LOW()    (GPIOA->BSRR = GPIO_BSRR_BR_9)
#define DC_HIGH()   (GPIOA->BSRR = GPIO_BSRR_BS_9)

#define RST_LOW()   (GPIOC->BSRR = GPIO_BSRR_BR_7)
#define RST_HIGH()  (GPIOC->BSRR = GPIO_BSRR_BS_7)

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

void initGPIO ()
{
    initGPIOClocks ();
    initGPIOLeds();
    initGPIOSwitch();

    // Mettre les sorties CS/DC/RST a leur etat inactif avant de les configurer.
    // Cela evitera de possibles glitchs.
    CS_HIGH();
    DC_HIGH();
    RST_HIGH();

    // PB6=CS, PC7=RST et PA9=DC en sortie : MODER = 01
    GPIOB->MODER &= ~GPIO_MODER_MODER6;
    GPIOB->MODER |=  GPIO_MODER_MODER6_0;

    GPIOC->MODER &= ~GPIO_MODER_MODER7;
    GPIOC->MODER |=  GPIO_MODER_MODER7_0;

    GPIOA->MODER &= ~GPIO_MODER_MODER9;
    GPIOA->MODER |=  GPIO_MODER_MODER9_0;

    // PA5=SCK et PA7=MOSI en fonction alternative : MODER = 10
    GPIOA->MODER &= ~(GPIO_MODER_MODER5 | GPIO_MODER_MODER7);
    GPIOA->MODER |=  (GPIO_MODER_MODER5_1 | GPIO_MODER_MODER7_1);

    // AF5 sur PA5 et PA7 : AF5 = 0101
    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL5 | GPIO_AFRL_AFSEL7);
    GPIOA->AFR[0] |=  GPIO_AFRL_AFSEL5_0 | GPIO_AFRL_AFSEL5_2
                    | GPIO_AFRL_AFSEL7_0 | GPIO_AFRL_AFSEL7_2;
}

static void initSPI1(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // Maitre, mode 0, 8 bits, NSS logiciel, frequence PCLK2/16.
    SPI1->CR1 = SPI_CR1_MSTR
              | SPI_CR1_SSM
              | SPI_CR1_SSI
              | SPI_CR1_BR_0
              | SPI_CR1_BR_1;
    SPI1->CR1 |= SPI_CR1_SPE;
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
    initGPIO();
    initSPI1();

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
