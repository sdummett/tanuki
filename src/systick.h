#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

void systick_init(void);
void delayMs(uint32_t ms);

#endif // SYSTICK_H
