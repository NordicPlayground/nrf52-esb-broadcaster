#ifndef __PWM_H
#define __PWM_H

#include "nrf.h"

void timer_init(void);

void timer_start(void);

void pwm0_init(uint32_t pinselect);
    
void pwm0_set_duty_cycle(uint32_t value);

#endif
