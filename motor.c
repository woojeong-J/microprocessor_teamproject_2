#include "Motor.h"

volatile int max_speed = 200000;
volatile int limited_speed = 0;
volatile int speed_time = 10;
static uint32_t dist_buffer = 0;
volatile int distance = 0;

#define DIST_THRESHOLD 5000000
// [튜닝 포인트] 이 숫자로 거리 올라가는 속도를 조절합니다.
// 숫자가 클수록 거리가 천천히 올라갑니다.
// 200,000 (200km/h) 속도일 때 10ms마다 더해지므로, 1초에 20,000,000이 쌓입니다.
// 아래 값으로 설정하면 200km/h일 때 1초에 숫자가 약 2~4 정도 올라갑니다. (적당한 데모 속도)

void PORT_init_Motor(void)
{
	PCC_PORTD |= (1<<CGC_BIT);
	PORTD_PCR10 &= ~((0b111)<<MUX_BITS);
	PORTD_PCR10 |= ((0b010)<<MUX_BITS);    //Gpio alternative function for FTM2_CH0


	PCC_PORTB |= (1<<CGC_BIT); // ADC용 포트
	PORTB_PCR0 &= ~((0b111)<<MUX_BITS);


	PCC_PORTC |= (1<<CGC_BIT);
	// 브레이크 버튼
	PORTC_PCR16 &= ~((0b111)<<MUX_BITS);
	PORTC_PCR16 |= (1<<MUX_BITS);  // GPIO
	PORTC_PCR16 &= ~((0b1111)<<IRQC_BITS);
	PORTC_PCR16 |= (0b1011<<IRQC_BITS); //either rising or falling edge
	PORTC_PCR16 |= (1<<1) | (1<<0);
	GPIOC_PDDR &= ~(1<<PTC16); // Input
	// 가속 버튼
	PORTC_PCR17 &= ~((0b111)<<MUX_BITS);
	PORTC_PCR17 |= (1<<MUX_BITS);  // GPIO
	PORTC_PCR17 &= ~((0b1111)<<IRQC_BITS);
	PORTC_PCR17 |= (0b1011<<IRQC_BITS); //either rising or falling edge
	PORTC_PCR17 |= (1<<1) | (1<<0);
	GPIOC_PDDR &= ~(1<<PTC17); // Input
	// 오토크루즈 버튼
	PORTC_PCR15 &= ~((0b111)<<MUX_BITS);
	PORTC_PCR15 |= (1<<MUX_BITS);  // GPIO
	PORTC_PCR15 &= ~((0b1111)<<IRQC_BITS);
	PORTC_PCR15 |= (0b1000<<IRQC_BITS); //logic 0
	PORTC_PCR15 |= (1<<1) | (1<<0);
	GPIOC_PDDR &= ~(1<<PTC15); // Input

	PCC_PORTA |= (1<<CGC_BIT);
	// 기어버튼
	PORTA_PCR12 &= ~((0b111)<<MUX_BITS);
	PORTA_PCR12 |= (1<<MUX_BITS);  // GPIO
	PORTA_PCR12 &= ~((0b1111)<<IRQC_BITS);
	PORTA_PCR12 |= (0b1000<<IRQC_BITS); //logic 0
	PORTA_PCR12 |= (1<<1) | (1<<0);
	GPIOA_PDDR &= ~(1<<PTA12); // Input
	// 시동버튼
	PORTA_PCR13 &= ~((0b111)<<MUX_BITS);
	PORTA_PCR13 |= (1<<MUX_BITS);  // GPIO
	PORTA_PCR13 &= ~((0b1111)<<IRQC_BITS);
	PORTA_PCR13 |= (0b1000<<IRQC_BITS); //logic 0
	PORTA_PCR13 |= (1<<1) | (1<<0);
	GPIOA_PDDR &= ~(1<<PTA13); // Input
}

void ADC0_init(void)
{
    /* ADC0 Clocking */
    PCC_ADC0 &= ~(1<<CGC_BIT);                  /* Disable clock to change PCS */
    PCC_ADC0 &= ~((0b111)<<PCS_BITS);           /* PCS=0b001: Select SOSCDIV2 */
    PCC_ADC0 |= ((0b001)<<PCS_BITS);
    PCC_ADC0 |= (1<<CGC_BIT);                   /* Enable bus clock in ADC0 */

    /* ADC0 Initialization */
    ADC0_SC1A |= ((0b111111)<<ADCH_BITS);        /* ADCH=1F: Module is disabled for conversion */

    ADC0_CFG1 &= ~((0b11)<<ADIV_BITS);          /* The divide ratio is 1 */
    ADC0_CFG1 &= ~((0b11)<<MODE_BITS);          /* MODE=1: 12-bit conversion */
    ADC0_CFG1 |= ((0b01)<<MODE_BITS);

    ADC0_CFG2 &= ~(255<<SMPLTS_BITS);           /* SMPLTS=12(default): sample time is 13 ADC clks */
    ADC0_CFG2 |= (12<<SMPLTS_BITS);

    ADC0_SC2 &= ~(1<<ADTRG_BIT);                /* ADTRG=0: SW trigger */
}

void FTM2_CH0_PWM(void) // DC motor PWM (FTM2 CH0)
{
	PCC_FTM2 &= ~(1<<CGC_BIT);
	PCC_FTM2 &= ~((0b111)<<PCS_BITS);
	PCC_FTM2 |= ((0b010)<<PCS_BITS);
	PCC_FTM2 |= (1<<CGC_BIT);

	FTM2_SC |= (1<<PWMEN0_BIT);
	FTM2_SC &= ~((0b111)<<PS_BITS);
	FTM2_SC |= ((0b010)<<PS_BITS); // 8MHZ / 4 = 2MHZ

	FTM2_MOD   = 40000 - 1; //2MHz / 40000 = 50Hz (20ms 필수)
	FTM2_CNTIN = 0;

	FTM2_C0SC |= (1<<MSB_BIT);
	FTM2_C0SC |= (1<<ELSB_BIT);
	FTM2_C0SC &= ~(1<<ELSA_BIT);

	FTM2_SC |= ((0b11)<<CLKS_BITS);
}

void DRV_Control()
{
	//DC motor control code

	limited_speed = (max_speed * adcResult) / 4095; // 최고 한계 속도 설정
	if(current_speed <= limited_speed)
	{
		current_speed = current_speed + 200; // 가속도(s단위) 에 따라 속도 증가

	}
	else if(current_speed > limited_speed)
	{
		current_speed = limited_speed; // 최대 속도 넘어가면 최대 속도로 고정
	}
	FTM2_C0V = ((uint64_t)current_speed * FTM2_MOD) / max_speed;
}

void DRV_Brake_Control()
{
	//급제동시 DC motor 제어 코드
	if(current_speed > 0)
	{
		current_speed = current_speed - 400; // 5초만에 정지
		if(current_speed < 0)
			current_speed = 0;
	}
	FTM2_C0V = ((uint64_t)current_speed * FTM2_MOD) / max_speed; // PWM duty 변경
}

void DRV_Coasting_Control()
{
	//타력 주행시 DC motor 제어 코드
	if(current_speed > 0)
	{
		current_speed = current_speed - 100; // 20초만에 정지
		if(current_speed < 0)
			current_speed = 0;
	}
	FTM2_C0V = ((uint64_t)current_speed * FTM2_MOD) / max_speed;
}

void Keep_Constant_Speed()
{
	//크루즈 모드시 일정 속도 유지 코드
	FTM2_C0V = ((uint64_t)current_speed * FTM2_MOD) / max_speed;
}

void Distance()
{
	dist_buffer += current_speed;

    // 2. 일정 임계값을 넘으면 거리 1 증가 및 버퍼 초기화
    if (dist_buffer >= DIST_THRESHOLD)
    {
        distance++;       // 거리 1 증가
        dist_buffer = 0;  // 버퍼 초기화

        if(distance > 99) distance = 0; // 0~99 반복
    }
}

void Velocity_init()
{
	current_speed = 0;
}