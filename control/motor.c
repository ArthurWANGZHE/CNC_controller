#include "motor.h"
#include "usart.h"
#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "stm32f10x.h"  // ??????????

#define clockwise 0		//顺时针定义
#define anti_clockwise 1		//逆时针定义

static int label = 0;

LinearInterpolation_TypeDef interpolation_para = {0};		//插补参数初始化
Axis_TypeDef axis;		//定义坐标轴

void TIM8_PWM_Init(u16 arr)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
  TIM_OCInitTypeDef TIM_OCInitSturcture;

  // 使能A端口时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  // X轴电机控制引脚配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;  // A1(PUL+), A2(DIR+)
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOA, GPIO_Pin_1 | GPIO_Pin_2); // 初始状态设为低电平

  // Y轴电机控制引脚配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;  // A3(PUL+), A4(DIR+)
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOA, GPIO_Pin_3 | GPIO_Pin_4); // 初始状态设为低电平

	/*设置定时器8中断触发的中断优先级*/
  NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  TIM_TimeBaseInitStructure.TIM_Period = arr;
  TIM_TimeBaseInitStructure.TIM_Prescaler = 71;
  TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0x00;
  TIM_TimeBaseInit(TIM8, &TIM_TimeBaseInitStructure);

  TIM_OCInitSturcture.TIM_OCMode = TIM_OCMode_PWM2;		//配置PWM模式为模式2
  TIM_OCInitSturcture.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitSturcture.TIM_Pulse = 0;
  TIM_OCInitSturcture.TIM_OCPolarity = TIM_OCPolarity_High;		//当计数器值小于比较值时设置为高电平
	TIM_OCInitSturcture.TIM_OCNPolarity = TIM_OCPolarity_High;	//设置互补通道输出的极性
	TIM_OCInitSturcture.TIM_OCIdleState = TIM_OCIdleState_Reset;		//配置空闲电平
	TIM_OCInitSturcture.TIM_OCNIdleState = TIM_OCIdleState_Reset;		//互补通道设置
  TIM_OC2Init(TIM8, &TIM_OCInitSturcture);		//通道2初始化
	TIM_OC3Init(TIM8, &TIM_OCInitSturcture);		//通道3初始化

  TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);
  TIM_CtrlPWMOutputs(TIM8, ENABLE);	//主输出使能
  TIM_ARRPreloadConfig(TIM8, ENABLE);

  TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM8, ENABLE);				//使能计数器
  TIM_SetCompare2(TIM8, arr/2);		//配置通道2的PWM的占空比为50%
	TIM_SetCompare3(TIM8, arr/2);		//配置通道3的PWM的占空比为50%
	TIM_CCxCmd(TIM8, TIM_Channel_2, TIM_CCx_Enable);		//开启通道2的PWM输出
	TIM_CCxCmd(TIM8, TIM_Channel_3, TIM_CCx_Enable);		//开启通道3的PWM输出
}

void Direction(u16 dir)
{
	if(dir == 0){ // 顺时针
		if(axis == x_axis){
			// X轴方向控制，DIR+ 拉高表示顺时针
			GPIO_SetBits(GPIOA, GPIO_Pin_2);
		}else{
			// Y轴方向控制，DIR+ 拉高表示顺时针
			GPIO_SetBits(GPIOA, GPIO_Pin_4);
		}
	}else if(dir == 1){ // 逆时针
		if(axis == x_axis){
			// X轴方向控制，DIR+ 拉低表示逆时针
			GPIO_ResetBits(GPIOA, GPIO_Pin_2);
		}else{
			// Y轴方向控制，DIR+ 拉低表示逆时针
			GPIO_ResetBits(GPIOA, GPIO_Pin_4);
		}
	}
}

void Linear_InterPolation_Move1(int inc_x, int inc_y, int speed)
{
	interpolation_para.deviation = 0;					//配置初始偏差值为0
	interpolation_para.endpoint_x = inc_x;		//配置X轴需要插补的脉冲数
	interpolation_para.endpoint_y = inc_y;		//配置Y轴需要插补的脉冲数
	interpolation_para.endpoint_pulse = inc_x + inc_y;		//总插补脉冲数等于X轴需要插补的脉冲数加上Y轴需要插补的脉冲数
	
	interpolation_para.active_axis = x_axis;		//配置第一次插补为X轴插补
	interpolation_para.deviation -= interpolation_para.endpoint_y; 		//计算第一次插补后的偏差值
	
	Direction(clockwise);
	printf("0");
	label = 1;
	TIM8_PWM_Init(speed);		//开启脉冲输出
}

void Linear_InterPolation_Move2(int inc_x, int inc_y, int speed)
{
	interpolation_para.deviation = 0;					//配置初始偏差值为0
	interpolation_para.endpoint_x = inc_x;		//配置X轴需要插补的脉冲数
	interpolation_para.endpoint_y = inc_y;		//配置Y轴需要插补的脉冲数
	interpolation_para.endpoint_pulse = inc_x + inc_y;		//总插补脉冲数等于X轴需要插补的脉冲数加上Y轴需要插补的脉冲数
	
	interpolation_para.active_axis = x_axis;		//配置第一次插补为X轴插补
	interpolation_para.deviation -= interpolation_para.endpoint_y; 		//计算第一次插补后的偏差值
	
	Direction(anti_clockwise);
	printf("2");
	label = 2;
	TIM8_PWM_Init(speed);		//开启脉冲输出
}

void Linear_InterPolation_Move3(int inc_x, int inc_y, int speed)
{
	interpolation_para.deviation = 0;					//配置初始偏差值为0
	interpolation_para.endpoint_x = inc_x;		//配置X轴需要插补的脉冲数
	interpolation_para.endpoint_y = inc_y;		//配置Y轴需要插补的脉冲数
	interpolation_para.endpoint_pulse = inc_x + inc_y;		//总插补脉冲数等于X轴需要插补的脉冲数加上Y轴需要插补的脉冲数
	
	interpolation_para.active_axis = x_axis;		//配置第一次插补为X轴插补
	interpolation_para.deviation -= interpolation_para.endpoint_y; 		//计算第一次插补后的偏差值
	
	Direction(anti_clockwise);
	printf("2");
	label = 3;
	TIM8_PWM_Init(speed);		//开启脉冲输出
}

void Linear_InterPolation_Move4(int inc_x, int inc_y, int speed)
{
	interpolation_para.deviation = 0;					//配置初始偏差值为0
	interpolation_para.endpoint_x = inc_x;		//配置X轴需要插补的脉冲数
	interpolation_para.endpoint_y = inc_y;		//配置Y轴需要插补的脉冲数
	interpolation_para.endpoint_pulse = inc_x + inc_y;		//总插补脉冲数等于X轴需要插补的脉冲数加上Y轴需要插补的脉冲数
	
	interpolation_para.active_axis = x_axis;		//配置第一次插补为X轴插补
	interpolation_para.deviation -= interpolation_para.endpoint_y; 		//计算第一次插补后的偏差值
	
	Direction(clockwise);
	printf("0");
	label = 4;
	TIM8_PWM_Init(speed);		//开启脉冲输出
}

void Circular_InterPolation_ACW1(int start_x, int start_y, int stop_x, int stop_y, int speed)
{
	int delta_x;
	int delta_y;
	
	delta_x = stop_x - start_x;
	delta_y = stop_y - start_y;
	
	/*判断起始点与终止点是否在同一个圆上，若不是则停止执行函数，返回错误提示*/
	if(((start_x * start_x) + (start_y * start_y)) != ((stop_x * stop_x) + (stop_y * stop_y ))){
		printf("The data is not available, please enter a new");
		return;
	}
	
	interpolation_para.deviation = 0;
	interpolation_para.startpoint_x = start_x;
	interpolation_para.startpoint_y = start_y;
	interpolation_para.endpoint_x = stop_x;
	interpolation_para.endpoint_y = stop_y;
	interpolation_para.endpoint_pulse = abs(delta_x) + abs(delta_y);

	interpolation_para.active_axis = x_axis;
	interpolation_para.deviation -= (2*interpolation_para.startpoint_x - 1);
	interpolation_para.startpoint_x --;
	
	Direction(anti_clockwise);
	printf("2");
	label = 5;
	TIM8_PWM_Init(speed);
}

void Circular_InterPolation_CW1(int start_x, int start_y, int stop_x, int stop_y, int speed)
{
	int delta_x;
	int delta_y;
	
	delta_x = stop_x - start_x;
	delta_y = stop_y - start_y;
	
	/*判断起始点与终止点是否在同一个圆上，若不是则停止执行函数，返回错误提示*/
	if(((start_x * start_x) + (start_y * start_y)) != ((stop_x * stop_x) + (stop_y * stop_y ))){
		printf("The data is not available, please enter a new");
		return;
	}
	
	interpolation_para.deviation = 0;
	interpolation_para.startpoint_x = start_x;
	interpolation_para.startpoint_y = start_y;
	interpolation_para.endpoint_x = stop_x;
	interpolation_para.endpoint_y = stop_y;
	interpolation_para.endpoint_pulse = abs(delta_x) + abs(delta_y);

	interpolation_para.active_axis = x_axis;
	interpolation_para.deviation -= (2*interpolation_para.startpoint_y - 1);
	interpolation_para.startpoint_x --;
	
	Direction(clockwise);
	printf("0");
	label = 6;
	TIM8_PWM_Init(speed);
}

void Circular_InterPolation_ACW2(int start_x, int start_y, int stop_x, int stop_y, int speed)
{
	int delta_x;
	int delta_y;
	
	delta_x = stop_x - start_x;
	delta_y = stop_y - start_y;
	
	/*判断起始点与终止点是否在同一个圆上，若不是则停止执行函数，返回错误提示*/
	if(((start_x * start_x) + (start_y * start_y)) != ((stop_x * stop_x) + (stop_y * stop_y ))){
		printf("The data is not available, please enter a new");
		return;
	}
	
	interpolation_para.deviation = 0;
	interpolation_para.startpoint_x = start_x;
	interpolation_para.startpoint_y = start_y;
	interpolation_para.endpoint_x = stop_x;
	interpolation_para.endpoint_y = stop_y;
	interpolation_para.endpoint_pulse = abs(delta_x) + abs(delta_y);

	interpolation_para.active_axis = x_axis;
	interpolation_para.deviation -= (2*interpolation_para.startpoint_y - 1);
	interpolation_para.startpoint_x --;
	
	Direction(anti_clockwise);
	printf("2");
	label = 7;
	TIM8_PWM_Init(speed);
}

void Circular_InterPolation_CW2(int start_x, int start_y, int stop_x, int stop_y, int speed)
{
	int delta_x;
	int delta_y;
	
	delta_x = stop_x - start_x;
	delta_y = stop_y - start_y;
	
	/*判断起始点与终止点是否在同一个圆上，若不是则停止执行函数，返回错误提示*/
	if(((start_x * start_x) + (start_y * start_y)) != ((stop_x * stop_x) + (stop_y * stop_y ))){
		printf("The data is not available, please enter a new");
		return;
	}
	
	interpolation_para.deviation = 0;
	interpolation_para.startpoint_x = start_x;
	interpolation_para.startpoint_y = start_y;
	interpolation_para.endpoint_x = stop_x;
	interpolation_para.endpoint_y = stop_y;
	interpolation_para.endpoint_pulse = abs(delta_x) + abs(delta_y);

	interpolation_para.active_axis = x_axis;
	interpolation_para.deviation += (2*interpolation_para.startpoint_x + 1);
	interpolation_para.startpoint_x ++;
	
	Direction(clockwise);
	printf("0");
	label = 8;
	TIM8_PWM_Init(speed);
}

void Circular_InterPolation_ACW3(int start_x, int start_y, int stop_x, int stop_y, int speed)
{
	int delta_x;
	int delta_y;
	
	delta_x = stop_x - start_x;
	delta_y = stop_y - start_y;
	
	/*判断起始点与终止点是否在同一个圆上，若不是则停止执行函数，返回错误提示*/
	if(((start_x * start_x) + (start_y * start_y)) != ((stop_x * stop_x) + (stop_y * stop_y ))){
		printf("The data is not available, please enter a new");
		return;
	}
	
	interpolation_para.deviation = 0;
	interpolation_para.startpoint_x = start_x;
	interpolation_para.startpoint_y = start_y;
	interpolation_para.endpoint_x = stop_x;
	interpolation_para.endpoint_y = stop_y;
	interpolation_para.endpoint_pulse = abs(delta_x) + abs(delta_y);

	interpolation_para.active_axis = x_axis;
	interpolation_para.deviation += (2*interpolation_para.startpoint_x + 1);
	interpolation_para.startpoint_x ++;
	
	Direction(clockwise);
	printf("0");
	label = 9;
	TIM8_PWM_Init(speed);
}

void Circular_InterPolation_CW3(int start_x, int start_y, int stop_x, int stop_y, int speed)
{
	int delta_x;
	int delta_y;
	
	delta_x = stop_x - start_x;
	delta_y = stop_y - start_y;
	
	/*判断起始点与终止点是否在同一个圆上，若不是则停止执行函数，返回错误提示*/
	if(((start_x * start_x) + (start_y * start_y)) != ((stop_x * stop_x) + (stop_y * stop_y ))){
		printf("The data is not available, please enter a new");
		return;
	}
	
	interpolation_para.deviation = 0;
	interpolation_para.startpoint_x = start_x;
	interpolation_para.startpoint_y = start_y;
	interpolation_para.endpoint_x = stop_x;
	interpolation_para.endpoint_y = stop_y;
	interpolation_para.endpoint_pulse = abs(delta_x) + abs(delta_y);

	interpolation_para.active_axis = x_axis;
	interpolation_para.deviation -= (2*interpolation_para.startpoint_x - 1);
	interpolation_para.startpoint_x --;
	
	Direction(anti_clockwise);
	printf("2");
	label = 10;
	TIM8_PWM_Init(speed);	
}

void Circular_InterPolation_ACW4(int start_x, int start_y, int stop_x, int stop_y, int speed)
{
	int delta_x;
	int delta_y;
	
	delta_x = stop_x - start_x;
	delta_y = stop_y - start_y;
	
	/*判断起始点与终止点是否在同一个圆上，若不是则停止执行函数，返回错误提示*/
	if(((start_x * start_x) + (start_y * start_y)) != ((stop_x * stop_x) + (stop_y * stop_y ))){
		printf("The data is not available, please enter a new");
		return;
	}
	
	interpolation_para.deviation = 0;
	interpolation_para.startpoint_x = start_x;
	interpolation_para.startpoint_y = start_y;
	interpolation_para.endpoint_x = stop_x;
	interpolation_para.endpoint_y = stop_y;
	interpolation_para.endpoint_pulse = abs(delta_x) + abs(delta_y);

	interpolation_para.active_axis = x_axis;
	interpolation_para.deviation -= (2*interpolation_para.startpoint_x - 1);
	interpolation_para.startpoint_x --;
	
	
	Direction(clockwise);
	printf("0");
	label = 11;
	TIM8_PWM_Init(speed);
}

void Circular_InterPolation_CW4(int start_x, int start_y, int stop_x, int stop_y, int speed)
{
	int delta_x;
	int delta_y;
	
	delta_x = stop_x - start_x;
	delta_y = stop_y - start_y;
	
	/*判断起始点与终止点是否在同一个圆上，若不是则停止执行函数，返回错误提示*/
	if(((start_x * start_x) + (start_y * start_y)) != ((stop_x * stop_x) + (stop_y * stop_y ))){
		printf("The data is not available, please enter a new");
		return;
	}
	
	interpolation_para.deviation = 0;
	interpolation_para.startpoint_x = start_x;
	interpolation_para.startpoint_y = start_y;
	interpolation_para.endpoint_x = stop_x;
	interpolation_para.endpoint_y = stop_y;
	interpolation_para.endpoint_pulse = abs(delta_x) + abs(delta_y);

	interpolation_para.active_axis = x_axis;
	interpolation_para.deviation -= (2*interpolation_para.startpoint_x - 1);
	interpolation_para.startpoint_x --;
	
	Direction(anti_clockwise);
	printf("2");
	label = 12;
	TIM8_PWM_Init(speed);
}

void TIM8_UP_IRQHandler(void)
{
	u32 last_axis = 0;
	uart_init(9600);
	last_axis = interpolation_para.active_axis;		//记录上一次的进给活动轴
	/*若label=1则进行第一象限的直线插补程序*/
	if(label == 1){
		if(TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
		{
			TIM_ClearITPendingBit(TIM8, TIM_IT_Update );
			/*根据上一步的偏差判断进给方向，并计算下一步的偏差*/
			if(interpolation_para.deviation >= 0){		
				interpolation_para.active_axis = x_axis;
				printf("0");
				interpolation_para.deviation -= interpolation_para.endpoint_y;		//若偏差大于0，说明在直线上方，此时应向x轴进给，并计算下一次的偏差
			}else{
				interpolation_para.active_axis = y_axis;
				printf("1");
				interpolation_para.deviation += interpolation_para.endpoint_x;		//若偏差小于0，说明在直线下方，此时应向y轴进给，并计算下一次的偏差
			}
			/*当下一步的进给轴与上一步的不一致时，需换轴*/
			if(last_axis != interpolation_para.active_axis){		
				if(last_axis == x_axis){
					Direction(clockwise);
					TIM_CCxCmd(TIM8, TIM_Channel_2, TIM_CCx_Disable);
					TIM_CCxCmd(TIM8, TIM_Channel_3, TIM_CCx_Enable);
				}else{
					Direction(clockwise);
					TIM_CCxCmd(TIM8, TIM_Channel_2, TIM_CCx_Enable);
					TIM_CCxCmd(TIM8, TIM_Channel_3, TIM_CCx_Disable);
				}
			}
			interpolation_para.endpoint_pulse --;		//每次判断完后总脉冲数减1
			if(interpolation_para.endpoint_pulse == 0){
				TIM_Cmd(TIM8, DISABLE);		//完成所有插补后关闭定时器8
			}
		}
	}
	/*若label=2则进行第二象限的直线插补程序*/
	if(label == 2){
		if(TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
		{
			TIM_ClearITPendingBit(TIM8, TIM_IT_Update );
			/*根据上一步的偏差判断进给方向，并计算下一步的偏差*/
			if(interpolation_para.deviation >= 0){		
				interpolation_para.active_axis = x_axis;
				printf("2");
				interpolation_para.deviation -= interpolation_para.endpoint_y;		//若偏差大于0，说明在直线上方，此时应向x轴进给，并计算下一次的偏差
			}else{
				interpolation_para.active_axis = y_axis;
				printf("1");
				interpolation_para.deviation += interpolation_para.endpoint_x;		//若偏差小于0，说明在直线下方，此时应向y轴进给，并计算下一次的偏差
			}
			/*当下一步的进给轴与上一步的不一致时，需换轴*/
			if(last_axis != interpolation_para.active_axis){		
				if(last_axis == x_axis){
					Direction(clockwise);
					TIM_CCxCmd(TIM8, TIM_Channel_2, TIM_CCx_Disable);
					TIM_CCxCmd(TIM8, TIM_Channel_3, TIM_CCx_Enable);
				}else{
					Direction(anti_clockwise);
					TIM_CCxCmd(TIM8, TIM_Channel_2, TIM_CCx_Enable);
					TIM_CCxCmd(TIM8, TIM_Channel_3, TIM_CCx_Disable);	
				}
			}
			interpolation_para.endpoint_pulse --;		//每次判断完后总脉冲数减1
			if(interpolation_para.endpoint_pulse == 0){
				TIM_Cmd(TIM8, DISABLE);		//完成所有插补后关闭定时器8
			}
		}
	}
	/*若label=3则进行第三象限的直线插补程序*/
	if(label == 3){
		if(TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
		{
			TIM_ClearITPendingBit(TIM8, TIM_IT_Update );
			/*根据上一步的偏差判断进给方向，并计算下一步的偏差*/
			if(interpolation_para.deviation >= 0){		
				interpolation_para.active_axis = x_axis;
				printf("2");
				interpolation_para.deviation -= interpolation_para.endpoint_y;		//若偏差大于0，说明在直线上方，此时应向x轴进给，并计算下一次的偏差
			}else{
				interpolation_para.active_axis = y_axis;
				printf("3");
				interpolation_para.deviation += interpolation_para.endpoint_x;		//若偏差小于0，说明在直线下方，此时应向y轴进给，并计算下一次的偏差
			}
			/*当下一步的进给轴与上一步的不一致时，需换轴*/
			if(last_axis != interpolation_para.active_axis){		
				if(last_axis == x_axis){
					Direction(anti_clockwise);
					TIM_CCxCmd(TIM8, TIM_Channel_2, TIM_CCx_Disable);
					TIM_CCxCmd(TIM8, TIM_Channel_3, TIM_CCx_Enable);	
				}else{
					Direction(anti_clockwise);
					TIM_CCxCmd(TIM8, TIM_Channel_2, TIM_CCx_Enable);
					TIM_CCxCmd(TIM8, TIM_Channel_3, TIM_CCx_Disable);		
				}
			}
			interpolation_para.endpoint_pulse --;		//每次判断完后总脉冲数减1
			if(interpolation_para.endpoint_pulse == 0){
				TIM_Cmd(TIM8, DISABLE);		//完成所有插补后关闭定时器8
			}
		}
	}
	/*若label=4则进行第四象限的直线插补程序*/
	if(label == 4){
		if(TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
		{
			TIM_ClearITPendingBit(TIM8, TIM_IT_Update );
			/*根据上一步的偏差判断进给方向，并计算下一步的偏差*/
			if(interpolation_para.deviation >= 0){		
				interpolation_para.active_axis = x_axis;
				printf("0");
				interpolation_para.deviation -= interpolation_para.endpoint_y;		//若偏差大于0，说明在直线上方，此时应向x轴进给，并计算下一次的偏差
			}else{
				interpolation_para.active_axis = y_axis;
				printf("3");
				interpolation_para.deviation += interpolation_para.endpoint_x;		//若偏差小于0，说明在直线下方，此时应向y轴进给，并计算下一次的偏差
			}
			/*当下一步的进给轴与上一步的不一致时，需换轴*/
			if(last_axis != interpolation_para.active_axis){		
				if(last_axis == x_axis){
					Direction(anti_clockwise);
					TIM_CCxCmd(TIM8, TIM_Channel_2, TIM_CCx_Disable);
					TIM_CCxCmd(TIM8, TIM_Channel_3, TIM_CCx_Enable);
				}else{
					Direction(clockwise);
					TIM_CCxCmd(TIM8, TIM_Channel_2, TIM_CCx_Enable);
					TIM_CCxCmd(TIM8, TIM_Channel_3, TIM_CCx_Disable);
				}
			}
			interpolation_para.endpoint_pulse --;		//每次判断完后总脉冲数减1
			if(interpolation_para.endpoint_pulse == 0){
				TIM_Cmd(TIM8, DISABLE);		//完成所有插补后关闭定时器8
			}
		}
	}
	/*若label=5则进行第一象限的逆圆弧插补程序*/
	if(label == 5){
		if(TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
		{
			TIM_ClearITPendingBit(TIM8, TIM_IT_Update );
			if(interpolation_para.deviation >= 0){
				interpolation_para.active_axis = x_axis;
				printf("2");
				interpolation_para.deviation -= (2*interpolation_para.startpoint_x - 1);
				interpolation_para.startpoint_x --;
			}else{
				interpolation_para.active_axis = y_axis;
				printf("1");
				interpolation_para.deviation += (2*interpolation_para.startpoint_y + 1);
				interpolation_para.startpoint_y ++;
			}
			/*当下一步的进给轴与上一步的不一致时，需换轴*/
			if(last_axis != interpolation_para.active_axis){		
				if(last_axis == x_axis){
					Direction(clockwise);
					TIM_CCxCmd(TIM8, TIM_Channel_2, TIM_CCx_Disable);
					TIM_CCxCmd(TIM8, TIM_Channel_3, TIM_CCx_Enable);	
				}else{
					Direction(anti_clockwise);
					TIM_CCxCmd(TIM8, TIM_Channel_2, TIM_CCx_Enable);
					TIM_CCxCmd(TIM8, TIM_Channel_3, TIM_CCx_Disable);	
				}
			}
			interpolation_para.endpoint_pulse --;		//每次判断完后总脉冲数减1
			if(interpolation_para.endpoint_pulse == 0){
				TIM_Cmd(TIM8, DISABLE);		//完成所有插补后关闭定时器8
			}
		}
	}
	/*若label=6则进行第一象限的顺圆弧插补程序*/
	if(label == 6){
		if(TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
		{
			TIM_ClearITPendingBit(TIM8, TIM_IT_Update );
			if(interpolation_para.deviation >= 0){
				interpolation_para.active_axis = y_axis;
				printf("3");
				interpolation_para.deviation -= (2*interpolation_para.startpoint_y - 1);
				interpolation_para.startpoint_y --;
			}else{
				interpolation_para.active_axis = x_axis;
				printf("0");
				interpolation_para.deviation += (2*interpolation_para.startpoint_x + 1);
				interpolation_para.startpoint_x ++;
			}
			/*当下一步的进给轴与上一步的不一致时，需换轴*/
			if(last_axis != interpolation_para.active_axis){		
				if(last_axis == x_axis){
					Direction(anti_clockwise);
					TIM_CCxCmd(TIM8, TIM_Channel_2, TIM_CCx_Disable);
					TIM_CCxCmd(TIM8, TIM_Channel_3, TIM_CCx_Enable);
				}else{
					Direction(clockwise);
					TIM_CCxCmd(TIM8, TIM_Channel_2, TIM_CCx_Enable);
					TIM_CCxCmd(TIM8, TIM_Channel_3, TIM_CCx_Disable);
				}
			}
			interpolation_para.endpoint_pulse --;		//每次判断完后总脉冲数减1
			if(interpolation_para.endpoint_pulse == 0){
				TIM_Cmd(TIM8, DISABLE);		//完成所有插补后关闭定时器8
			}
		}
	}
	/*若label=7则进行第二象限的逆圆弧插补程序*/
	if(label == 7){
		if(TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
		{
			TIM_ClearITPendingBit(TIM8, TIM_IT_Update );
			if(interpolation_para.deviation >= 0){
				interpolation_para.active_axis = y_axis;
				printf("3");
				interpolation_para.deviation -= (2*interpolation_para.startpoint_y - 1);
				interpolation_para.startpoint_y --;
			}else{
				interpolation_para.active_axis = x_axis;
				printf("2");
				interpolation_para.deviation += (2*interpolation_para.startpoint_x + 1);
				interpolation_para.startpoint_x ++;
			}
			/*当下一步的进给轴与上一步的不一致时，需换轴*/
			if(last_axis != interpolation_para.active_axis){		
				if(last_axis == x_axis){
					Direction(anti_clockwise);
					TIM_CCxCmd(TIM8, TIM_Channel_2, TIM_CCx_Disable);
					TIM_CCxCmd(TIM8, TIM_Channel_3, TIM_CCx_Enable);
				}else{
					Direction(anti_clockwise);
					TIM_CCxCmd(TIM8, TIM_Channel_2, TIM_CCx_Enable);
					TIM_CCxCmd(TIM8, TIM_Channel_3, TIM_CCx_Disable);
				}
			}
			interpolation_para.endpoint_pulse --;		//每次判断完后总脉冲数减1
			if(interpolation_para.endpoint_pulse == 0){
				TIM_Cmd(TIM8, DISABLE);		//完成所有插补后关闭定时器8
			}
		}
	}
	/*若label=8则进行第二象限的顺圆弧插补程序*/
	if(label == 8){
		if(TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
		{
			TIM_ClearITPendingBit(TIM8, TIM_IT_Update );
			if(interpolation_para.deviation >= 0){
				interpolation_para.active_axis = x_axis;
				printf("0");
				interpolation_para.deviation += (2*interpolation_para.startpoint_x + 1);
				interpolation_para.startpoint_x ++;
			}else{
				interpolation_para.active_axis = y_axis;
				printf("1");
				interpolation_para.deviation -= (2*interpolation_para.startpoint_y - 1);
				interpolation_para.startpoint_y --;
			}
			/*当下一步的进给轴与上一步的不一致时，需换轴*/
			if(last_axis != interpolation_para.active_axis){		
				if(last_axis == x_axis){
					Direction(clockwise);
					TIM_CCxCmd(TIM8, TIM_Channel_2, TIM_CCx_Disable);
					TIM_CCxCmd(TIM8, TIM_Channel_3, TIM_CCx_Enable);
				}else{
					Direction(clockwise);
					TIM_CCxCmd(TIM8, TIM_Channel_2, TIM_CCx_Enable);
					TIM_CCxCmd(TIM8, TIM_Channel_3, TIM_CCx_Disable);
				}
			}
			interpolation_para.endpoint_pulse --;		//每次判断完后总脉冲数减1
			if(interpolation_para.endpoint_pulse == 0){
				TIM_Cmd(TIM8, DISABLE);		//完成所有插补后关闭定时器8
			}
		}
	}
	/*若label=9则进行第三象限的逆圆弧插补程序*/
	if(label == 9){
		if(TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
		{
			TIM_ClearITPendingBit(TIM8, TIM_IT_Update );
			if(interpolation_para.deviation >= 0){
				interpolation_para.active_axis = x_axis;
				printf("0");
				interpolation_para.deviation += (2*interpolation_para.startpoint_x + 1);
				interpolation_para.startpoint_x ++;
			}else{
				interpolation_para.active_axis = y_axis;
				printf("3");
				interpolation_para.deviation -= (2*interpolation_para.startpoint_y - 1);
				interpolation_para.startpoint_y --;
			}
			/*当下一步的进给轴与上一步的不一致时，需换轴*/
			if(last_axis != interpolation_para.active_axis){		
				if(last_axis == x_axis){
					Direction(clockwise);
					TIM_CCxCmd(TIM8, TIM_Channel_2, TIM_CCx_Disable);
					TIM_CCxCmd(TIM8, TIM_Channel_3, TIM_CCx_Enable);
				}else{
					Direction(anti_clockwise);
					TIM_CCxCmd(TIM8, TIM_Channel_2, TIM_CCx_Enable);
					TIM_CCxCmd(TIM8, TIM_Channel_3, TIM_CCx_Disable);
				}
			}
			interpolation_para.endpoint_pulse --;		//每次判断完后总脉冲数减1
			if(interpolation_para.endpoint_pulse == 0){
				TIM_Cmd(TIM8, DISABLE);		//完成所有插补后关闭定时器8
			}
		}
	}
	/*若label=10则进行第三象限的顺圆弧插补程序*/
	if(label == 10){
		if(TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
		{
			TIM_ClearITPendingBit(TIM8, TIM_IT_Update );
			if(interpolation_para.deviation >= 0){
				interpolation_para.active_axis = y_axis;
				printf("1");
				interpolation_para.deviation += (2*interpolation_para.startpoint_y + 1);
				interpolation_para.startpoint_y ++;
			}else{
				interpolation_para.active_axis = x_axis;
				printf("2");
				interpolation_para.deviation -= (2*interpolation_para.startpoint_x - 1);
				interpolation_para.startpoint_x --;
			}
			/*当下一步的进给轴与上一步的不一致时，需换轴*/
			if(last_axis != interpolation_para.active_axis){		
				if(last_axis == x_axis){
					Direction(anti_clockwise);
					TIM_CCxCmd(TIM8, TIM_Channel_2, TIM_CCx_Disable);
					TIM_CCxCmd(TIM8, TIM_Channel_3, TIM_CCx_Enable);
				}else{
					Direction(clockwise);
					TIM_CCxCmd(TIM8, TIM_Channel_2, TIM_CCx_Enable);
					TIM_CCxCmd(TIM8, TIM_Channel_3, TIM_CCx_Disable);
				}
			}
			interpolation_para.endpoint_pulse --;		//每次判断完后总脉冲数减1
			if(interpolation_para.endpoint_pulse == 0){
				TIM_Cmd(TIM8, DISABLE);		//完成所有插补后关闭定时器8
			}
		}
	}
	/*若label=11则进行第四象限的逆圆弧插补程序*/
	if(label == 11){
		if(TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
		{
			TIM_ClearITPendingBit(TIM8, TIM_IT_Update );
			if(interpolation_para.deviation >= 0){
				interpolation_para.active_axis = y_axis;
				printf("1");
				interpolation_para.deviation += (2*interpolation_para.startpoint_y + 1);
				interpolation_para.startpoint_y ++;
			}else{
				interpolation_para.active_axis = x_axis;
				printf("0");
				interpolation_para.deviation -= (2*interpolation_para.startpoint_x - 1);
				interpolation_para.startpoint_x --;
			}
			/*当下一步的进给轴与上一步的不一致时，需换轴*/
			if(last_axis != interpolation_para.active_axis){		
				if(last_axis == x_axis){
					Direction(clockwise);
					TIM_CCxCmd(TIM8, TIM_Channel_2, TIM_CCx_Disable);
					TIM_CCxCmd(TIM8, TIM_Channel_3, TIM_CCx_Enable);
				}else{
					Direction(clockwise);
					TIM_CCxCmd(TIM8, TIM_Channel_2, TIM_CCx_Enable);
					TIM_CCxCmd(TIM8, TIM_Channel_3, TIM_CCx_Disable);
				}
			}
			interpolation_para.endpoint_pulse --;		//每次判断完后总脉冲数减1
			if(interpolation_para.endpoint_pulse == 0){
				TIM_Cmd(TIM8, DISABLE);		//完成所有插补后关闭定时器8
			}
		}
	}
	/*若label=12则进行第四象限的顺圆弧插补程序*/
	if(label == 12){
		if(TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
		{
			TIM_ClearITPendingBit(TIM8, TIM_IT_Update );
			if(interpolation_para.deviation >= 0){
				interpolation_para.active_axis = x_axis;
				printf("2");
				interpolation_para.deviation -= (2*interpolation_para.startpoint_x - 1);
				interpolation_para.startpoint_x --;
			}else{
				interpolation_para.active_axis = y_axis;
				printf("3");
				interpolation_para.deviation += (2*interpolation_para.startpoint_y + 1);
				interpolation_para.startpoint_y ++;
			}
			/*当下一步的进给轴与上一步的不一致时，需换轴*/
			if(last_axis != interpolation_para.active_axis){		
				if(last_axis == x_axis){
					Direction(anti_clockwise);
					TIM_CCxCmd(TIM8, TIM_Channel_2, TIM_CCx_Disable);
					TIM_CCxCmd(TIM8, TIM_Channel_3, TIM_CCx_Enable);
				}else{
					Direction(anti_clockwise);
					TIM_CCxCmd(TIM8, TIM_Channel_2, TIM_CCx_Enable);
					TIM_CCxCmd(TIM8, TIM_Channel_3, TIM_CCx_Disable);
				}
			}
			interpolation_para.endpoint_pulse --;		//每次判断完后总脉冲数减1
			if(interpolation_para.endpoint_pulse == 0){
				TIM_Cmd(TIM8, DISABLE);		//完成所有插补后关闭定时器8
			}
		}
	}
}

