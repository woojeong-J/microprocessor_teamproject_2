#include <stdint.h>
#include "handler.h"
#include "regs_config.h"
#include "Motor.h"
#include "turn.h"
#include "common.h"
#include "state.h"
#include "clocks_and_modes.h"
#include "seven_segment.h"
#include "lpit.h"

volatile int input_lock = 0;
volatile uint32_t adcResult;
volatile CarState_t current_state = STATE_OFF;
volatile int Start_Flag = 0;
volatile int Gear_Flag = 0;
volatile int Cruise_Flag = 0;
volatile int mode = 0;   // 0=정지(center), 1=왼쪽(C13), 2=오른쪽(C12)
volatile int current_speed = 0;
volatile int Accel_Flag = 0;
volatile int Brake_Flag = 0;
volatile int gear = 0; // 디스플레이용 변수 (0:P, 1:D, 2:R, 3:C)
volatile int distance = 0;

int main(void)
{
	SOSC_init_8MHz();
	SPLL_init_160MHz();
	NormalRUNmode_80MHz();
	PORT_init_Motor();
	PORT_init_Segment();
	PORT_init_turn();
	FTM2_CH0_PWM();
	FTM2_CH1_PWM();
	ADC0_init();
	NVIC_init_IRQs();
	LPIT0_init();


	for(;;)
	{


	}
}
