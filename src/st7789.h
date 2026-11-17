#ifndef ST7789_H
#define ST7789_H

#include <stdint.h>

#define TFT_WIDTH   240U
#define TFT_HEIGHT  240U

void st7789_gpio_init(void); // CS / DC / RST
void st7789_init(void); // sequence d'initialisation de l'ecran
void st7789_draw_image(const uint16_t *pixels);

#endif // ST7789_H
