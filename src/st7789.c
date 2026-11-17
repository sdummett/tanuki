#include "stm32f401xe.h"
#include <stdint.h>
#include "st7789.h"
#include "spi.h"
#include "systick.h"

#define CS_LOW()    (GPIOB->BSRR = GPIO_BSRR_BR_6)
#define CS_HIGH()   (GPIOB->BSRR = GPIO_BSRR_BS_6)

#define DC_LOW()    (GPIOA->BSRR = GPIO_BSRR_BR_9)
#define DC_HIGH()   (GPIOA->BSRR = GPIO_BSRR_BS_9)

#define RST_LOW()   (GPIOC->BSRR = GPIO_BSRR_BR_7)
#define RST_HIGH()  (GPIOC->BSRR = GPIO_BSRR_BS_7)

#define ST7789_CMD_SWRESET 0x01U
#define ST7789_CMD_SLPOUT  0x11U
#define ST7789_CMD_COLMOD  0x3AU
#define ST7789_CMD_MADCTL  0x36U
#define ST7789_CMD_INVON   0x21U
#define ST7789_CMD_NORON   0x13U
#define ST7789_CMD_DISPON  0x29U
#define ST7789_CMD_CASET   0x2AU
#define ST7789_CMD_RASET   0x2BU
#define ST7789_CMD_RAMWR   0x2CU

#define ST7789_MADCTL_MY      0x80U
#define ST7789_MADCTL_MX      0x40U
#define ST7789_MADCTL_MV      0x20U
#define ST7789_MADCTL_RGB     0x00U
#define ST7789_MADCTL_NORMAL  0x00U

#define TFT_ROWSTART 80U

typedef enum {
    TFT_ROTATION_0   = 0U,
    TFT_ROTATION_90  = 1U,
    TFT_ROTATION_180 = 2U,
    TFT_ROTATION_270 = 3U,
} st7789_rotation_t;

static st7789_rotation_t st7789_rotation = TFT_ROTATION_270;

void st7789_gpio_init(void)
{
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
}

static void st7789_write_command(uint8_t cmd)
{
    CS_LOW();
    DC_LOW();
    spi1_write_byte(cmd);
    CS_HIGH();
}

static void st7789_write_command_data(uint8_t cmd, const uint8_t *data, uint32_t length)
{
    CS_LOW();
    DC_LOW();
    spi1_write_byte(cmd);

    DC_HIGH();
    for (uint32_t i = 0U; i < length; i++) {
        spi1_write_byte(data[i]);
    }

    CS_HIGH();
}

static uint8_t st7789_madctl_for_rotation(st7789_rotation_t rotation)
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

void st7789_init(void)
{
    const uint8_t pixel_format = 0x55U; // RGB565
    const uint8_t madctl = st7789_madctl_for_rotation(st7789_rotation) | ST7789_MADCTL_RGB;

    RST_LOW();
    systick_delay_ms(10U);
    RST_HIGH();
    systick_delay_ms(120U);

    st7789_write_command(ST7789_CMD_SWRESET);
    systick_delay_ms(150U);

    st7789_write_command(ST7789_CMD_SLPOUT);
    systick_delay_ms(120U);

    st7789_write_command_data(ST7789_CMD_COLMOD, &pixel_format, 1U);
    systick_delay_ms(10U);

    st7789_write_command_data(ST7789_CMD_MADCTL, &madctl, 1U);
    st7789_write_command(ST7789_CMD_INVON);
    st7789_write_command(ST7789_CMD_NORON);

    st7789_write_command(ST7789_CMD_DISPON);
    systick_delay_ms(20U);
}

static void st7789_set_window(void)
{
    uint16_t x_start = 0U;
    uint16_t y_start = 0U;

    switch (st7789_rotation) {
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

    const uint16_t x_end = (uint16_t)(x_start + TFT_WIDTH - 1U);
    const uint16_t y_end = (uint16_t)(y_start + TFT_HEIGHT - 1U);

    const uint8_t columns[4] = {
        (uint8_t)(x_start >> 8), (uint8_t)x_start,
        (uint8_t)(x_end >> 8),   (uint8_t)x_end
    };

    const uint8_t rows[4] = {
        (uint8_t)(y_start >> 8), (uint8_t)y_start,
        (uint8_t)(y_end >> 8),   (uint8_t)y_end
    };

    st7789_write_command_data(ST7789_CMD_CASET, columns, sizeof(columns));
    st7789_write_command_data(ST7789_CMD_RASET, rows, sizeof(rows));
}

void st7789_draw_image(const uint16_t *pixels)
{
    st7789_set_window();

    CS_LOW();
    DC_LOW();
    spi1_write_byte(ST7789_CMD_RAMWR);

    DC_HIGH();
    for (uint32_t i = 0U; i < TFT_WIDTH * TFT_HEIGHT; i++) {
        uint16_t pixel = pixels[i];
        spi1_write_byte((uint8_t)(pixel >> 8)); // Octet fort
        spi1_write_byte((uint8_t)pixel); // Octet faible
    }

    CS_HIGH();
}
