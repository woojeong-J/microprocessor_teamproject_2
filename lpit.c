#include "lpit.h"


void LPIT0_Ch0_IRQHandler(void) //10ms rtos
{
    Process_State_Transition();
    Process_State_Action();

    Distance();
    Fuel_Warning();

    adc_start();
	adcResult = read_adc_chx(); //가변 저항 값 0~ 4095

    if (input_lock > 0)
	{
	    input_lock--; // 10ms마다 1씩 감소
	        // 0이 되면 자동으로 잠금 해제됨
	}
    

    LPIT_MSR |= (1<<TIF0_BIT);
}

void LPIT0_Ch1_IRQHandler(void) // 2ms (화면 제어)
{
    displayDigitClear();
    
    // [수정] Start_Flag 대신 current_state로 판단
    if(current_state != STATE_OFF) 
    {
        Display_ON();
    }

    LPIT_MSR |= (1 << TIF1_BIT);
}