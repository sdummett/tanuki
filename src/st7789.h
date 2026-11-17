#ifndef ST7789_H
#define ST7789_H

#include <stdint.h>

#define TFT_WIDTH   240U
#define TFT_HEIGHT  240U

void initTftGPIO(void); // CS / DC / RST
void initST7789(void); // sequence d'initialisation de l'ecran
void tftDrawImage(const uint16_t *pixels);

#endif // ST7789_H
