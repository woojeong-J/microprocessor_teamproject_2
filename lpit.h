#ifndef lpit_H
#define lpit_H

#include "regs_config.h"
#include "handler.h"
#include "seven_segment.h"
#include "common.h"
#include "state.h"

void Display_ON();

extern volatile int scan_index;
extern volatile int distance;
extern volatile int input_lock;
extern volatile uint32_t adcResult;

#endif