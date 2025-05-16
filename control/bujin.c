#include "bujin.h"
#include "delay.h"
#include <stdio.h>

//配置GPIO的信息//
void My_Bujin_IO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA,ENABLE);//开时钟总线，添加GPIOA
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;         //推挽输出
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_5 | GPIO_Pin_6 ;  //PB5、PB6作为备用方向引脚
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,  &GPIO_InitStruct);
	
	// 配置GPIOA引脚
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_1 | GPIO_Pin_2;  //PA1作为脉冲引脚(PUL+)，PA2作为方向引脚(DIR+)
	GPIO_Init(GPIOA,  &GPIO_InitStruct);
}

//电机1运动（x轴）
void Bujin1_Move(u16 model,u16 turn,u16 F)   // 1圈=多少mm？？？？？？？？？？（脉冲数未改变情况下，对于x轴电机，即电机1，走1mm要0.1圈->走1圈10mm；对于y轴电机，即电机2，走1mm要0.2圈->1圈5mm）
{
	int delayT= 4000/F;	//设置高低电平的持续时间，通过调节该值可以对电机调速（F=50,delayT=80）
	int i=0;

	if(model==0)  //逆时针旋转
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_2);//方向信号,PA2(DIR+)
		double a=0.2;
		i=0;
		while(i<turn*6400/10*a/2)//使用32细分，每个脉冲转1.8度/32，转一圈所需脉冲数为6400 i=0;i<turn*6400*a;i++
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_1);//拉高PA1(PUL+)
			delay_us(delayT);
			GPIO_ResetBits(GPIOA, GPIO_Pin_1);//拉低PA1(PUL+)
			delay_us(delayT);
			delayT-=1;
			i++;
		}
		
		for(i=0;i<turn*6400/10*a/2 && i<turn*6400/10*(1-a);i++)//使用32细分，每个脉冲转1.8度/32，转一圈所需脉冲数为6400
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_1);//拉高PA1(PUL+)
			delay_us(delayT);
			GPIO_ResetBits(GPIOA, GPIO_Pin_1);//拉低PA1(PUL+)
			delay_us(delayT);
		}
		
		while(i<turn*6400/10*(1-a) && i<turn*6400/10)//使用32细分，每个脉冲转1.8度/32，转一圈所需脉冲数为6400 i=0;i<turn*6400*a;i++
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_1);//拉高PA1(PUL+)
			delay_us(delayT);
			GPIO_ResetBits(GPIOA, GPIO_Pin_1);//拉低PA1(PUL+)
			delay_us(delayT);
			delayT+=1;
			i++;
		}			
	}
	else if(model==1)//顺时针旋转
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_2);//方向信号,PA2(DIR+)
		double a=0.2;
		i=0;
		while(i<turn*6400/10*a/2)//使用32细分，每个脉冲转1.8度/32，转一圈所需脉冲数为6400 i=0;i<turn*6400*a;i++
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_1);//拉高PA1(PUL+)
			delay_us(delayT);
			GPIO_ResetBits(GPIOA, GPIO_Pin_1);//拉低PA1(PUL+)
			delay_us(delayT);
			delayT-=1;
			i++;
		}
		
		for(i=0;i<turn*6400/10*a/2 && i<turn*6400/10*(1-a);i++)//使用32细分，每个脉冲转1.8度/32，转一圈所需脉冲数为6400
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_1);//拉高PA1(PUL+)
			delay_us(delayT);
			GPIO_ResetBits(GPIOA, GPIO_Pin_1);//拉低PA1(PUL+)
			delay_us(delayT);
		}
		
		while(i<turn*6400/10*(1-a) && i<turn*6400/10)//使用32细分，每个脉冲转1.8度/32，转一圈所需脉冲数为6400 i=0;i<turn*6400*a;i++
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_1);//拉高PA1(PUL+)
			delay_us(delayT);
			GPIO_ResetBits(GPIOA, GPIO_Pin_1);//拉低PA1(PUL+)
			delay_us(delayT);
			delayT+=1;
			i++;
		}		
	}
}

//电机2运动（y轴）
void Bujin2_Move(u16 model,u16 turn,u16 F)
{
	int delayT= 4000/F;	//设置高低电平的持续时间，通过调节该值可以对电机调速（F=50,delayT=80）
	int i=0;

	if(model==0)  //逆时针旋转
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_2);//方向信号,PA2(DIR+)
		double a=0.1;//（此时y轴a=0.1才会与x轴等于0.2相匹配）
		i=0;
		while(i<turn*6400*2/10*a/2)//使用32细分，每个脉冲转1.8度/32，转一圈所需脉冲数为6400 i=0;i<turn*6400*a;i++
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_1);//拉高PA1(PUL+)
			delay_us(delayT);
			GPIO_ResetBits(GPIOA, GPIO_Pin_1);//拉低PA1(PUL+)
			delay_us(delayT);
			delayT-=1;//翻转电平所需时间越少，速度越快
			i++;
		}
		
		for(i=0;i<turn*6400*2/10*a/2 && i<turn*6400*2/10*(1-a);i++)//使用32细分，每个脉冲转1.8度/32，转一圈所需脉冲数为6400（实测结果为Y轴转1mm所需脉冲数为6400*2/10）
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_1);//拉高PA1(PUL+)
			delay_us(delayT);
			GPIO_ResetBits(GPIOA, GPIO_Pin_1);//拉低PA1(PUL+)
			delay_us(delayT);
		}
		
		while(i<turn*6400*2/10*(1-a) && i<turn*6400*2/10)//使用32细分，每个脉冲转1.8度/32，转一圈所需脉冲数为6400 i=0;i<turn*6400*a;i++
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_1);//拉高PA1(PUL+)
			delay_us(delayT);
			GPIO_ResetBits(GPIOA, GPIO_Pin_1);//拉低PA1(PUL+)
			delay_us(delayT);
			delayT+=1;//翻转电平所需时间越多，速度越慢
			i++;
		}		
	}
	else if(model==1)//顺时针旋转
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_2);//方向信号,PA2(DIR+)
		double a=0.1;//（此时y轴a=0.1才会与x轴等于0.2相匹配）
		i=0;
		while(i<turn*6400*2/10*a/2)//使用32细分，每个脉冲转1.8度/32，转一圈所需脉冲数为6400 i=0;i<turn*6400*a;i++
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_1);//拉高PA1(PUL+)
			delay_us(delayT);
			GPIO_ResetBits(GPIOA, GPIO_Pin_1);//拉低PA1(PUL+)
			delay_us(delayT);
			delayT-=1;//翻转电平所需时间越少，速度越快
			i++;
		}
		
		for(i=0;i<turn*6400*2/10*a/2 && i<turn*6400*2/10*(1-a);i++)//使用32细分，每个脉冲转1.8度/32，转一圈所需脉冲数为6400（实测结果为Y轴转1mm所需脉冲数为6400*2/10）
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_1);//拉高PA1(PUL+)
			delay_us(delayT);
			GPIO_ResetBits(GPIOA, GPIO_Pin_1);//拉低PA1(PUL+)
			delay_us(delayT);
		}
		
		while(i<turn*6400*2/10*(1-a) && i<turn*6400*2/10)//使用32细分，每个脉冲转1.8度/32，转一圈所需脉冲数为6400 i=0;i<turn*6400*a;i++
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_1);//拉高PA1(PUL+)
			delay_us(delayT);
			GPIO_ResetBits(GPIOA, GPIO_Pin_1);//拉低PA1(PUL+)
			delay_us(delayT);
			delayT+=1;//翻转电平所需时间越多，速度越慢
			i++;
		}	
	}
}

// 电机1运动（x轴）- 支持可变细分值
void Bujin1_Move_Variable(u16 model, u16 pulses, u16 F, u16 subdivide)
{
    // 使用定时器控制电机运动
    Motor_Start(1, model, pulses, F, subdivide);
}

// 电机2运动（y轴）- 支持可变细分值
void Bujin2_Move_Variable(u16 model, u16 pulses, u16 F, u16 subdivide)
{
    // 使用定时器控制电机运动
    Motor_Start(2, model, pulses, F, subdivide);
}
