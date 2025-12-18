#ifndef Motor_H
#define Motor_H

#include "regs_config.h"
#include <stdint.h>
#include "common.h"
#include "clocks_and_modes.h"

void PORT_init_Motor(void);
void ADC0_init(void);
void FTM2_CH0_PWM(void);

void DRV_Control(void);
void DRV_Brake_Control(void);
void DRV_Coasting_Control(void);
void DRV_Reverse_Control(void);
void Distance(void);
void Velocity_init(void);
void Fuel_Warning(void);

#endif