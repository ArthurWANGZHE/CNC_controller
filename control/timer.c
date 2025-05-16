#include "timer.h"
#include "bujin.h"
#include "delay.h"
#include <stdio.h>

// 电机状态变量
MotorStatus motor1_status = {0};
MotorStatus motor2_status = {0};
volatile u8 motor_busy = 0;

// 初始化定时器3
void TIM3_Int_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    // 配置定时器基本参数
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // 使能定时器中断
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    // 配置中断优先级
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 启动定时器
    TIM_Cmd(TIM3, ENABLE);
    
    printf("定时器3初始化完成\r\n");
}

// 启动电机
void Motor_Start(u8 motor_id, u8 direction, u32 pulses, u16 speed, u16 subdivide)
{
    if (motor_busy) {
        printf("电机忙，无法启动新的运动\r\n");
        return;
    }
    
    // 确保速度在1-50范围内
    if (speed < 1) speed = 1;
    if (speed > 50) speed = 50;
    
    // 将0-50的速度值映射到实际的电机转速（10-300 RPM）
    u16 actual_speed = 10 + (speed - 1) * 490 / 49;  // 映射公式：10 + (speed-1)*(300-10)/(50-1)
    
    // 设置电机方向
    if (direction == 0) {
        GPIO_ResetBits(GPIOA, GPIO_Pin_2);  // 逆时针
    } else {
        GPIO_SetBits(GPIOA, GPIO_Pin_2);    // 顺时针
    }
    
    // 配置电机状态
    MotorStatus *motor = (motor_id == 1) ? &motor1_status : &motor2_status;
    
    motor->is_running = 1;
    motor->direction = direction;
    motor->total_pulses = pulses;
    motor->current_pulse = 0;
    motor->speed = speed;  // 保存原始速度值
    motor->subdivide = subdivide;
    motor->motor_id = motor_id;
    motor->pulse_state = 0;
    motor->delay_time = 4000 / actual_speed;  // 使用映射后的速度计算延时
    motor->stage = 0;  // 加速阶段
    
    // 计算加速和匀速阶段的脉冲数
    double accel_ratio = 0.2;  // 加速比例
    motor->accel_pulses = pulses * accel_ratio / 2;
    motor->const_pulses = pulses * (1 - accel_ratio);
    
    printf("电机%d开始运动: 方向=%d, 脉冲数=%d, 速度=%d(映射为%d RPM), 细分=%d\r\n", 
           motor_id, direction, pulses, speed, actual_speed, subdivide);
    
    // 设置全局忙标志
    motor_busy = 1;
    
    // 根据映射后的速度计算定时器频率
    // 定时器频率 = 72MHz / (psc+1) / (arr+1)
    u16 psc = 71;  // 预分频值固定为72
    
    // 根据映射后的速度调整arr值
    u16 arr = 10000 / actual_speed;
    
    // 限制arr的范围，防止频率过高或过低
    if (arr < 10) arr = 10;     // 防止频率过高
    if (arr > 1000) arr = 1000; // 防止频率过低
    
    printf("设置定时器: 速度=%d(映射为%d RPM), arr=%d, psc=%d\r\n", 
           speed, actual_speed, arr, psc);
    
    // 初始化定时器
    TIM3_Int_Init(arr, psc);
}

// 停止指定电机
void Motor_Stop(u8 motor_id)
{
    if (motor_id == 1) {
        motor1_status.is_running = 0;
    } else if (motor_id == 2) {
        motor2_status.is_running = 0;
    }
    
    // 如果两个电机都停止了，关闭定时器
    if (!motor1_status.is_running && !motor2_status.is_running) {
        TIM_Cmd(TIM3, DISABLE);
        motor_busy = 0;
        printf("所有电机停止运行\r\n");
    }
}

// 停止所有电机
void Motor_Stop_All(void)
{
    motor1_status.is_running = 0;
    motor2_status.is_running = 0;
    TIM_Cmd(TIM3, DISABLE);
    motor_busy = 0;
    printf("强制停止所有电机\r\n");
}

// 检查电机是否忙
u8 Is_Motor_Busy(void)
{
    return motor_busy;
}

// 定时器3中断服务函数
void TIM3_IRQHandler(void)
{
    static u32 pulse_counter = 0;  // 脉冲计数器
    
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
        
        pulse_counter++;  // 计数器增加
        
        // 处理电机1
        if (motor1_status.is_running) {
            // 根据速度调整脉冲频率
            // 速度越快，脉冲间隔越短
            u32 pulse_interval = 100 / motor1_status.speed;  // 简化计算
            if (pulse_interval < 1) pulse_interval = 1;
            
            // 只有当计数器达到间隔时才生成脉冲
            if (pulse_counter % pulse_interval == 0) {
                if (motor1_status.pulse_state == 0) {
                    // 输出高电平
                    GPIO_SetBits(GPIOA, GPIO_Pin_1);
                    motor1_status.pulse_state = 1;
                } else {
                    // 输出低电平，并计数
                    GPIO_ResetBits(GPIOA, GPIO_Pin_1);
                    motor1_status.pulse_state = 0;
                    
                    // 脉冲计数增加
                    motor1_status.current_pulse++;
                    
                    // 根据阶段调整延时
                    if (motor1_status.stage == 0) {  // 加速阶段
                        if (motor1_status.current_pulse < motor1_status.accel_pulses) {
                            if (motor1_status.delay_time > 20) {  // 防止速度过快
                                motor1_status.delay_time -= 1;
                            }
                        } else {
                            motor1_status.stage = 1;  // 进入匀速阶段
                        }
                    } else if (motor1_status.stage == 1) {  // 匀速阶段
                        if (motor1_status.current_pulse >= motor1_status.accel_pulses + motor1_status.const_pulses) {
                            motor1_status.stage = 2;  // 进入减速阶段
                        }
                    } else if (motor1_status.stage == 2) {  // 减速阶段
                        motor1_status.delay_time += 1;
                    }
                    
                    // 检查是否完成
                    if (motor1_status.current_pulse >= motor1_status.total_pulses) {
                        Motor_Stop(1);
                        printf("电机1运动完成，共输出%d个脉冲\r\n", motor1_status.current_pulse);
                    }
                }
            }
        }
        
        // 处理电机2 (与电机1类似)
        if (motor2_status.is_running) {
            // 根据速度调整脉冲频率
            u32 pulse_interval = 100 / motor2_status.speed;
            if (pulse_interval < 1) pulse_interval = 1;
            
            if (pulse_counter % pulse_interval == 0) {
                if (motor2_status.pulse_state == 0) {
                    // 输出高电平
                    GPIO_SetBits(GPIOA, GPIO_Pin_1);
                    motor2_status.pulse_state = 1;
                } else {
                    // 输出低电平，并计数
                    GPIO_ResetBits(GPIOA, GPIO_Pin_1);
                    motor2_status.pulse_state = 0;
                    
                    // 脉冲计数增加
                    motor2_status.current_pulse++;
                    
                    // 根据阶段调整延时
                    if (motor2_status.stage == 0) {  // 加速阶段
                        if (motor2_status.current_pulse < motor2_status.accel_pulses) {
                            if (motor2_status.delay_time > 20) {  // 防止速度过快
                                motor2_status.delay_time -= 1;
                            }
                        } else {
                            motor2_status.stage = 1;  // 进入匀速阶段
                        }
                    } else if (motor2_status.stage == 1) {  // 匀速阶段
                        if (motor2_status.current_pulse >= motor2_status.accel_pulses + motor2_status.const_pulses) {
                            motor2_status.stage = 2;  // 进入减速阶段
                        }
                    } else if (motor2_status.stage == 2) {  // 减速阶段
                        motor2_status.delay_time += 1;
                    }
                    
                    // 检查是否完成
                    if (motor2_status.current_pulse >= motor2_status.total_pulses) {
                        Motor_Stop(2);
                        printf("电机2运动完成，共输出%d个脉冲\r\n", motor2_status.current_pulse);
                    }
                }
            }
        }
    }
}
