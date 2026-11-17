
#include "systick.h"
#include "gpio.h"
#include "leds.h"
#include "switches.h"
#include "spi.h"
#include "st7789.h"

#include "tanuki.h"

int main(void)
{
    systick_init();
    initGPIOClocks();
    initGPIOLeds();
    initGPIOSwitch();
    initTftGPIO();
    initSPI1();
    initST7789();
    tftDrawImage(image_data);

    while (1) ;
}
