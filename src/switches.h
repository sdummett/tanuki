#ifndef SWITCHES_H
#define SWITCHES_H

#include <stdbool.h>

void switches_init(void);

bool switch_right_is_pressed(void);
bool switch_middle_is_pressed(void);
bool switch_left_is_pressed(void);

#endif // SWITCHES_H
