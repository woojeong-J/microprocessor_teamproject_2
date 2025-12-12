#include "state.h"

void Process_State_Transition(void)
{
    // 1. 시동 버튼 처리 (최우선 순위)
    if (Start_Flag == 1)
    {
         if (Gear_Flag == 1 && current_speed == 0)
         {
             switch (current_state)
             {
                 case STATE_P:
                     current_state = STATE_D; // P -> D
                     break;
                 case STATE_D:
                     current_state = STATE_R; // D -> R
                     break;
                 case STATE_R:
                     current_state = STATE_P; // R -> P
                     break;
                default:
                     break;
                 // ...
             }
             Gear_Flag = 0; // 처리 했으니 깃발 내림
         }

         // 3. 크루즈 진입/해제 로직
         if (Cruise_Flag == 1) 
         {
             if (current_state == STATE_D) current_state = STATE_CRUISE;
             else if (current_state == STATE_CRUISE) current_state = STATE_D;
             Cruise_Flag = 0; // 처리 했으니 깃발 내림
         }
    }
    else // 시동 꺼짐 (Start_Flag == 0)
    {
        current_state = STATE_OFF;
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
            else if (Accel_Flag) DRV_Control();  // 가속 허용
            else DRV_Coasting_Control();         // 관성 주행
            
            Handle_Steering(); // 조향 허용
            Led_blink();
            break;

        case STATE_CRUISE:
            gear = 3;              // 디스플레이용 기어 정보 갱신
            // [할 수 있는 것] 정속 주행, 조향, 브레이크(누르면 해제)
            // [제약] 가속 페달 무시 (이미 정속이니까)
            Keep_Constant_Speed(); // 크루즈 전용 함수
            Handle_Steering();     // 조향 허용
            Led_blink();
            break;
    }
}