#include "stm32f401xe.h"
#include <stdint.h>
#include <stdbool.h>
#include "tanuki.h"

#define CS_LOW()    (GPIOB->BSRR = GPIO_BSRR_BR_6)
#define CS_HIGH()   (GPIOB->BSRR = GPIO_BSRR_BS_6)

#define DC_LOW()    (GPIOA->BSRR = GPIO_BSRR_BR_9)
#define DC_HIGH()   (GPIOA->BSRR = GPIO_BSRR_BS_9)

#define RST_LOW()   (GPIOC->BSRR = GPIO_BSRR_BR_7)
#define RST_HIGH()  (GPIOC->BSRR = GPIO_BSRR_BS_7)

#define TFT_WIDTH       240U
#define TFT_HEIGHT      240U

#define ST7789_CMD_SWRESET 0x01U
#define ST7789_CMD_SLPOUT 0x11U
#define ST7789_CMD_COLMOD 0x3AU
#define ST7789_CMD_MADCTL 0x36U
#define ST7789_CMD_INVON 0x21U
#define ST7789_CMD_NORON 0x13U
#define ST7789_CMD_DISPON 0x29U
#define ST7789_CMD_CASET 0x2AU
#define ST7789_CMD_RASET 0x2BU
#define ST7789_CMD_RAMWR 0x2CU

#define ST7789_MADCTL_MY        0x80U
#define ST7789_MADCTL_MX        0x40U
#define ST7789_MADCTL_MV        0x20U
#define ST7789_MADCTL_RGB       0x00U
#define ST7789_MADCTL_NORMAL    0x00U
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

static void writeSPI1(uint8_t byte)
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

static void tftCommand(uint8_t cmd)
{
    CS_LOW();
    DC_LOW();
    writeSPI1(cmd);
    CS_HIGH();
}

static void tftCommandData(uint8_t cmd, const uint8_t *data, uint32_t length)
{
    CS_LOW();
    DC_LOW();
    writeSPI1(cmd);

    DC_HIGH();
    for (uint32_t i = 0U; i < length; i++) {
        writeSPI1(data[i]);
    }

    CS_HIGH();
}

#define TFT_ROWSTART 80U

typedef enum {
    TFT_ROTATION_0   = 0U,
    TFT_ROTATION_90  = 1U,
    TFT_ROTATION_180 = 2U,
    TFT_ROTATION_270 = 3U,
} TftRotation_t;

static TftRotation_t tftRotation = TFT_ROTATION_270;

static uint8_t tftMadctl(TftRotation_t rotation)
{
    switch (rotation) {
    case TFT_ROTATION_90:
        return ST7789_MADCTL_MY | ST7789_MADCTL_MV;
    case TFT_ROTATION_270:
        return ST7789_MADCTL_MX | ST7789_MADCTL_MV;
    case TFT_ROTATION_180:
        return ST7789_MADCTL_NORMAL;
    case TFT_ROTATION_0:
    default:
        return ST7789_MADCTL_MY | ST7789_MADCTL_MX;
    }
}

static void initST7789(void)
{
    const uint8_t pixel_format = 0x55U; // RGB565
    const uint8_t madctl = tftMadctl(tftRotation) | ST7789_MADCTL_RGB;

    RST_LOW();
    delayMs(10U);
    RST_HIGH();
    delayMs(120U);

    tftCommand(ST7789_CMD_SWRESET);
    delayMs(150U);

    tftCommand(ST7789_CMD_SLPOUT);
    delayMs(120U);

    tftCommandData(ST7789_CMD_COLMOD, &pixel_format, 1U);
    delayMs(10U);

    tftCommandData(ST7789_CMD_MADCTL, &madctl, 1U);
    tftCommand(ST7789_CMD_INVON);
    tftCommand(ST7789_CMD_NORON);

    tftCommand(ST7789_CMD_DISPON);
    delayMs(20U);
}

static void tftSetWindow(void)
{
    uint16_t x_start = 0U;
    uint16_t y_start = 0U;

    switch (tftRotation) {
    case TFT_ROTATION_90:
        x_start = TFT_ROWSTART;
        break;

    case TFT_ROTATION_270:
        // offsets nuls
        break;

    case TFT_ROTATION_0:
    case TFT_ROTATION_180:
    default:
        y_start = TFT_ROWSTART;
        break;
    }

    const uint16_t x_end =
        (uint16_t)(x_start + TFT_WIDTH - 1U);
    const uint16_t y_end =
        (uint16_t)(y_start + TFT_HEIGHT - 1U);

    const uint8_t columns[4] = {
        (uint8_t)(x_start >> 8),
        (uint8_t)x_start,
        (uint8_t)(x_end >> 8),
        (uint8_t)x_end
    };

    const uint8_t rows[4] = {
        (uint8_t)(y_start >> 8),
        (uint8_t)y_start,
        (uint8_t)(y_end >> 8),
        (uint8_t)y_end
    };

    tftCommandData(ST7789_CMD_CASET, columns, sizeof(columns));
    tftCommandData(ST7789_CMD_RASET, rows, sizeof(rows));
}

static void tftDrawImage(const uint16_t *pixels)
{
    tftSetWindow();

    CS_LOW();
    DC_LOW();
    writeSPI1(ST7789_CMD_RAMWR);

    DC_HIGH();
    for (uint32_t i = 0U; i < TFT_WIDTH * TFT_HEIGHT; i++) {
        uint16_t pixel = pixels[i];
        writeSPI1((uint8_t)(pixel >> 8)); //Octet fort
        writeSPI1((uint8_t)pixel); // Octet faible
    }

    CS_HIGH();
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
    initST7789();
    tftDrawImage(image_data);
}
