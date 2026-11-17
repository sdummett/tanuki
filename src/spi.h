#ifndef SPI_H
#define SPI_H

#include <stdint.h>

void initSPI1(void); // broches PA5/PA7 + peripherique SPI1
void writeSPI1(uint8_t byte);

#endif // SPI_H
