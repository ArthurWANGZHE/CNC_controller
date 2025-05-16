#ifndef __MOTOR_H__
#define __MOTOR_H__
#include "sys.h"

typedef struct{
  __IO int endpoint_x;           //终点坐标Xe
  __IO int endpoint_y;           //终点坐标Ye
	__IO int startpoint_x;           
  __IO int startpoint_y;
  __IO int endpoint_pulse;       //到达终点位置需要的脉冲数F
  __IO int active_axis;          //当前运动的轴
  __IO int deviation;             //偏差参数
}LinearInterpolation_TypeDef;

typedef enum{
  x_axis = 0U,
  y_axis
}Axis_TypeDef;

void TIM8_PWM_Init(u16 arr);
void Direction(u16 dir);
void Linear_InterPolation_Move1(int inc_x, int inc_y, int speed);
void Linear_InterPolation_Move2(int inc_x, int inc_y, int speed);
void Linear_InterPolation_Move3(int inc_x, int inc_y, int speed);
void Linear_InterPolation_Move4(int inc_x, int inc_y, int speed);
void Circular_InterPolation_ACW1(int start_x, int start_y, int stop_x, int stop_y, int speed);
void Circular_InterPolation_CW1(int start_x, int start_y, int stop_x, int stop_y, int speed);
void Circular_InterPolation_ACW2(int start_x, int start_y, int stop_x, int stop_y, int speed);
void Circular_InterPolation_CW2(int start_x, int start_y, int stop_x, int stop_y, int speed);
void Circular_InterPolation_ACW3(int start_x, int start_y, int stop_x, int stop_y, int speed);
void Circular_InterPolation_CW3(int start_x, int start_y, int stop_x, int stop_y, int speed);
void Circular_InterPolation_ACW4(int start_x, int start_y, int stop_x, int stop_y, int speed);
void Circular_InterPolation_CW4(int start_x, int start_y, int stop_x, int stop_y, int speed);

#endif
