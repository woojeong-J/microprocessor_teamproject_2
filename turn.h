#ifndef turn_H
#define turn_H

#include "regs_config.h"
#include "common.h"

void PORT_init_turn(void);
void FTM2_CH1_PWM(void);
void Handle_init(void);
void Handle_Steering(void);
void Led_blink(void);

#endif
