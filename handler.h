#ifndef handler_h
#define handler_h

#include <stdint.h>
#include "regs_config.h"
#include "common.h"

void adc_start(void);
uint32_t read_adc_chx(void);
void LPIT0_init(void);
void NVIC_init_IRQs(void);
void PORTA_IRQHandler(void);
void PORTC_IRQHandler(void);

#endif
