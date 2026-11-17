#include "tanuki.h"

#include "systick.h"
#include "gpio.h"
#include "leds.h"
#include "switches.h"
#include "spi.h"
#include "st7789.h"

int main(void)
{
    systick_init();
    gpio_clocks_enable();
    leds_init();
    switches_init();
    st7789_gpio_init();
    spi1_init();
    st7789_init();
    st7789_draw_image(image_data);

    while (1) ;
}
