#ifndef SYSTEM_DATA_H
#define SYSTEM_DATA_H
#include <stdint.h>

#define SERVO_LEFT   1200
#define SERVO_RIGHT  5200
#define SERVO_CENTER 3200

// 다른 파일들이 갖다 쓸 수 있게 'extern' 선언
extern volatile int Start_Flag;
extern volatile int Cruise_Flag;
extern volatile int Accel_Flag;
extern volatile int Brake_Flag;
extern volatile int Gear_Flag;
extern volatile CarState_t current_state;
extern volatile uint32_t adcResult;
extern volatile int input_lock;
extern volatile int mode;
extern volatile int current_speed;
extern volatile int gear;
extern volatile int distance;


// 상태 정의도 여기서 하면 좋음
typedef enum {
    STATE_OFF,      // 시동 꺼짐
    STATE_P,        // 주차 (Parking)
    STATE_D,        // 주행 (Drive)
    STATE_R,        // 후진 (Reverse)
    STATE_CRUISE    // 크루즈 모드
} CarState_t;

#endif