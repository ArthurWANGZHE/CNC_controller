#ifndef __MOTOR_H__
#define __MOTOR_H__
#include "sys.h"

typedef struct{
  __IO int endpoint_x;           //�յ�����Xe
  __IO int endpoint_y;           //�յ�����Ye
	__IO int startpoint_x;           
  __IO int startpoint_y;
  __IO int endpoint_pulse;       //�����յ�λ����Ҫ��������F
  __IO int active_axis;          //��ǰ�˶�����
  __IO int deviation;             //ƫ�����
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
