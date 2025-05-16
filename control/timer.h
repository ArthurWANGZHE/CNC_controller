#ifndef __TIMER_H
#define __TIMER_H

#include "sys.h"

// 电机控制状态
typedef struct {
    u8 is_running;        // 电机是否在运行
    u8 direction;         // 方向：0=逆时针，1=顺时针
    u32 total_pulses;     // 总脉冲数
    u32 current_pulse;    // 当前已执行脉冲数
    u16 speed;            // 速度
    u16 subdivide;        // 细分值
    u8 motor_id;          // 电机ID：1=电机1，2=电机2
    u8 pulse_state;       // 脉冲状态：0=低电平，1=高电平
    u16 delay_time;       // 当前延时时间
    u8 stage;             // 运动阶段：0=加速，1=匀速，2=减速
    u32 accel_pulses;     // 加速阶段脉冲数
    u32 const_pulses;     // 匀速阶段脉冲数
} MotorStatus;

extern MotorStatus motor1_status;
extern MotorStatus motor2_status;
extern volatile u8 motor_busy;

// 函数声明
void TIM3_Int_Init(u16 arr, u16 psc);
void Motor_Start(u8 motor_id, u8 direction, u32 pulses, u16 speed, u16 subdivide);
void Motor_Stop(u8 motor_id);
void Motor_Stop_All(void);
u8 Is_Motor_Busy(void);

#endif
