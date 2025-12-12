#include "lpit.h"


void LPIT0_Ch0_IRQHandler(void) //10ms rtos
{
    Process_State_Transition();
    Process_State_Action();

    Distance();

    adc_start();
	adcResult = read_adc_chx(); //가변 저항 값 0~ 4095

    if (input_lock > 0)
	{
	    input_lock--; // 10ms마다 1씩 감소
	        // 0이 되면 자동으로 잠금 해제됨
	}
    

    LPIT_MSR |= (1<<TIF0_BIT);
}

void LPIT0_Ch1_IRQHandler(void) //0.05ms
{
    displayDigitClear();
    if(Start_Flag == 1) Display_ON();

    LPIT_MSR |= (1 << TIF1_BIT);
}