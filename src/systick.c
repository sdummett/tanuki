#include "stm32f401xe.h"
#include "systick.h"

static volatile uint32_t ms_ticks = 0;

void SysTick_Handler(void)
{
    ms_ticks++;
}

void systick_init(void)
{
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000);
}

void delayMs(uint32_t ms)
{
    uint32_t start = ms_ticks;
    while ((ms_ticks - start) < ms);
}
