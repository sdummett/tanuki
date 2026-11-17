#include "stm32f401xe.h"
#include "spi.h"

static void initSPI1Pins(void)
{
    // PA5=SCK et PA7=MOSI en fonction alternative : MODER = 10
    GPIOA->MODER &= ~(GPIO_MODER_MODER5 | GPIO_MODER_MODER7);
    GPIOA->MODER |=  (GPIO_MODER_MODER5_1 | GPIO_MODER_MODER7_1);

    // AF5 sur PA5 et PA7 : AF5 = 0101
    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL5 | GPIO_AFRL_AFSEL7);
    GPIOA->AFR[0] |=  GPIO_AFRL_AFSEL5_0 | GPIO_AFRL_AFSEL5_2
                    | GPIO_AFRL_AFSEL7_0 | GPIO_AFRL_AFSEL7_2;
}

void initSPI1(void)
{
    initSPI1Pins();

    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // Maitre, mode 0, 8 bits, NSS logiciel, frequence PCLK2/16.
    SPI1->CR1 = SPI_CR1_MSTR
              | SPI_CR1_SSM
              | SPI_CR1_SSI
              | SPI_CR1_BR_0
              | SPI_CR1_BR_1;
    SPI1->CR1 |= SPI_CR1_SPE;
}

void writeSPI1(uint8_t byte)
{
    // Attente de la disponibilité du registre d'émission
    while ((SPI1->SR & SPI_SR_TXE) == 0U) {
    }

    // Écriture de l'octet en accès 8 bits pour éviter un envoi sur 16 bits
    *(__IO uint8_t *)&SPI1->DR = byte;

    // Attente de l'octet reçu pendant l'émission (full-duplex)
    while ((SPI1->SR & SPI_SR_RXNE) == 0U) {
    }

    // Lecture factice de DR pour effacer RXNE et éviter un OVR
    (void)*(__IO uint8_t *)&SPI1->DR;
}
