#ifndef SWITCHES_H
#define SWITCHES_H

#include <stdbool.h>

void initGPIOSwitch(void);

bool rightSwitchIsPressed(void);
bool middleSwitchIsPressed(void);
bool leftSwitchIsPressed(void);

#endif // SWITCHES_H
