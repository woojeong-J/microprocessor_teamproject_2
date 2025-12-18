#include "handler.h"

void adc_start(void)
{
	ADC0_SC1A &= ~((0b111111)<<ADCH_BITS);
	ADC0_SC1A |= (ADC0_SE4<<ADCH_BITS);
}

uint32_t read_adc_chx(void)
{
	while((ADC0_SC1A & (1<<COCO_BIT))==0){}

	return ADC0_RA;
}

void LPIT0_init(void)
{
	PCC_LPIT &= ~((0b111)<<PCS_BITS);
	PCC_LPIT |= ((0b110)<<PCS_BITS);
	PCC_LPIT |= (1<<CGC_BIT);

	LPIT_MCR |= (1<<M_CEN_BIT);
    //여기까지 공통
    // ch0
	LPIT_MIER |= (1<<TIE0_BIT); // Enable Timer 0 interrupt

	LPIT_TVAL0 = 400000; // 80MHz / 2 / 400000 = 100Hz -> 10ms

	LPIT_TCTRL0 &= ~((0b11)<<MODE_BITS);
	LPIT_TCTRL0 |= (1<<T_EN_BIT);

    // ch1
    LPIT_MIER |= (1<<TIE1_BIT); // Enable Timer 1 interrupt

    LPIT_TVAL1 = 80000; // 80MHz / 2 / 80000 = 500Hz -> 2ms

	LPIT_TCTRL1 &= ~((0b11)<<MODE_BITS);
	LPIT_TCTRL1 |= (1<<T_EN_BIT);
}

void NVIC_init_IRQs(void)
{
    NVIC_ICPR1 |= (1<<(48 % 32)); // LPIT0 ch 0 IRQ
	NVIC_ISER1 |= (1<<(48 % 32));
	NVIC_IPR48 = 20;

	NVIC_ICPR1 |= (1<<(49 % 32)); // LPIT0 ch 1 IRQ
	NVIC_ISER1 |= (1<<(49 % 32));
	NVIC_IPR48 = 20;

	NVIC_ICPR1 |= (1<<(59 % 32)); // PORTA IRQ
	NVIC_ISER1 |= (1<<(59 % 32));
	NVIC_IPR59 = 10;

	NVIC_ICPR1 |= (1<<(61 % 32)); // PORTC IRQ
	NVIC_ISER1 |= (1<<(61 % 32));
	NVIC_IPR61 = 10;
}


void PORTA_IRQHandler(void)
{
     // 시동 버튼 (PTA13)이 인터럽트를 발생시켰는지 확인

    if (PORTA_PCR13 & (1 << ISF_BIT))
    {
    	if(input_lock ==0)
    	{
        Start_Flag = 1; //시동 on
        input_lock = 30;
        }
        
    	
        PORTA_PCR13 |= (1 << ISF_BIT);
    }

    // 기어 버튼 (PTA12)이 인터럽트를 발생시켰는지 확인

    if (PORTA_PCR12 & (1 << ISF_BIT))
    {
        if(input_lock ==0)
        {
        Gear_Flag = 1;
        input_lock = 30;
        }

        PORTA_PCR12 |= (1 << ISF_BIT);
    }
}


void PORTC_IRQHandler(void)
{
    // C12 눌림 → 오른쪽 방향 모드 토글
    if (PORTC_PCR13 & (1<<ISF_BIT)) 
    {
        if(input_lock == 0)
        {
            if(mode == 1) mode = 0;
            else if(mode == 2) mode = 1;
            else mode = 1;
            input_lock = 30;
        }
        PORTC_PCR13 |= (1<<ISF_BIT);   // ISF 플래그 클리어
    }

    // C13 눌림 → 왼쪽 방향 모드 토글
    if (PORTC_PCR12 & (1<<ISF_BIT)) 
    {
        if(input_lock == 0)
        {
            if(mode == 2) mode = 0;
            else if(mode == 1) mode = 2;
            else mode = 2;
            input_lock = 30;
        }
        PORTC_PCR12 |= (1<<ISF_BIT);   // ISF 플래그 클리어
    }

    // 가속 버튼 (PTC17)이 인터럽트를 발생시켰는지 확인

    if (PORTC_PCR17 & (1 << ISF_BIT))
    {
        if((GPIOC_PDIR & (1 << PTC17))==0) Accel_Flag = 1;
        else Accel_Flag = 0;

        PORTC_PCR17 |= (1 << ISF_BIT);
    }

    // 브레이크 버튼 (PTC16)이 인터럽트를 발생시켰는지 확인

    if (PORTC_PCR16 & (1 << ISF_BIT))
    {
        if((GPIOC_PDIR & (1 << PTC16))==0)
        {
            Brake_Flag = 1;
            mode = 3;
        } 
        else
        {
            Brake_Flag = 0;
            mode = 0;
        }

        PORTC_PCR16 |= (1 << ISF_BIT);
    }
    

    // 오토크루즈 버튼 (PTC15)이 인터럽트를 발생시켰는지 확인

    if (PORTC_PCR15 & (1 << ISF_BIT))
    {
        if(input_lock == 0)
        {
            if(Cruise_Flag == 0) Cruise_Flag = 1;
            else Cruise_Flag = 0;
            input_lock = 30;
        }
        	
        PORTC_PCR15 |= (1 << ISF_BIT);
    }
}
