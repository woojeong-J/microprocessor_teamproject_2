#include "state.h"

void Process_State_Transition(void)
{
    // [1] 시동 버튼이 눌렸다면? (Event 처리)
    if (Start_Flag == 1)
    {
        // 1-1. 지금 꺼져 있으면 -> 켠다 (P모드)
        if (current_state == STATE_OFF)
        {
            current_state = STATE_P;
        }
        // 1-2. 지금 켜져 있으면(P단이고 멈춰있을 때) -> 끈다 (OFF)
        else if (current_state == STATE_P)
        {
            current_state = STATE_OFF;
        }
        
        // [핵심] 처리가 끝났으니 깃발 내리기! (다른 깃발들과 통일)
        Start_Flag = 0; 
    }

    // -----------------------------------------------------
    // [2] 기어 변환 (시동 켜져 있을 때만 동작)
    // -----------------------------------------------------
    // Start_Flag는 이제 0이 되어버리므로, current_state로 시동 여부 판단!
    if (current_state != STATE_OFF) 
    {
        if (Gear_Flag == 1 && current_speed == 0)
        {
            switch (current_state)
            {
                case STATE_P: current_state = STATE_D; break;
                case STATE_D: current_state = STATE_R; break;
                case STATE_R: current_state = STATE_P; break;
                default: break;
            }
            Gear_Flag = 0; // 처리 완료
        }

        // [3] 크루즈 로직
        if (Cruise_Flag == 1) 
        {
            if (current_state == STATE_D) current_state = STATE_CRUISE;
            else if (current_state == STATE_CRUISE) current_state = STATE_D;
            Cruise_Flag = 0; 
        }
        
        if (current_state == STATE_CRUISE && Brake_Flag == 1) 
        {
            current_state = STATE_D;
            Cruise_Flag = 0;
        }
    }
}

void Process_State_Action(void)
{
    switch (current_state)
    {
        case STATE_OFF:
            Velocity_init();        // 정지
            break;

        case STATE_P:
            gear = 0;              // 디스플레이용 기어 정보 갱신
            Velocity_init(); // 모터 힘 0 (확실하게 잠금)
            Handle_init();   // 핸들 정렬
            break;

        case STATE_D:
            gear = 1;              // 디스플레이용 기어 정보 갱신
            if (Brake_Flag) DRV_Brake_Control(); // 브레이크 우선
            else if (Accel_Flag) DRV_Control();  // 가속 허용
            else DRV_Coasting_Control();         // 관성 주행
            
            Handle_Steering(); // 조향 허용
            Led_blink();
            break;

        case STATE_R:
            gear = 2;              // 디스플레이용 기어 정보 갱신
            if (Brake_Flag) DRV_Brake_Control(); // 브레이크 우선
            else if (Accel_Flag) DRV_Reverse_Control();  // 가속 허용
            else DRV_Coasting_Control();         // 관성 주행
            
            Handle_Steering(); // 조향 허용
            Led_blink();
            break;

        case STATE_CRUISE:
            gear = 3;              // 디스플레이용 기어 정보 갱신
            // [할 수 있는 것] 정속 주행, 조향, 브레이크(누르면 해제)
            DRV_Control();           // 항상 accel 유지
            Handle_Steering();     // 조향 허용
            Led_blink();
            break;
    }
}