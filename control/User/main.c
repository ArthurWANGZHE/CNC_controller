#include "stm32f10x.h"
#include "../delay.h"
#include "../usart.h"
#include "../bujin.h"
#include "../chabu.h"
#include "../timer.h"  // 添加定时器头文件
#include <string.h>
#include <stdlib.h>  // 添加stdlib.h头文件，提供abs函数

// 添加命令处理状态标志
volatile u8 cmd_processing = 0;  // 0表示空闲，1表示正在处理命令
volatile u8 cmd_received = 0;    // 新增：标记是否收到新命令
volatile u8 cmd_interrupt = 0;   // 新增：中断命令标志

// 添加函数声明
void PBP_Line_Variable(int step, int XEnd, int YEnd, int F, int subdivide);

// 16进制字符转整数
int hex_to_int(char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    return 0;
}

int main(void)
{	
	u16 s1=0,s2=0,s3=0;
	u8 Simbol;
	int pulses = 0;  // 脉冲数
	int F = 0;       // 速度
	int dir = 0;     // 方向
	int subdivide = 200;  // 默认细分值
	u32 timeout_counter = 0;
	
	//Set NVIC interrupt group 2: 2-bit preemptive priority, 2-bit response priority
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	uart_init(115200);	 //Initialize serial port to 115200
	delay_init();
	My_Bujin_IO_Init();
	USART_RX_STA=0;
	printf("系统初始化完成\r\n");	
	printf("等待命令...\r\n");
	
	// 初始化定时器
	TIM3_Int_Init(7199, 9999);  // 72MHz/10000/7200 = 1Hz
	
	while(1)
	{
		// 手动重置接收状态，确保能接收新命令
		if(!(USART_RX_STA & 0x8000) && cmd_received)
		{
			cmd_received = 0;
			printf("重置接收状态，准备接收新命令\r\n");
		}
		
		// 如果有命令正在处理，检查超时
		if(cmd_processing)
		{
			timeout_counter++;
			if(timeout_counter > 1000000)  // 减小超时值
			{
				cmd_processing = 0;
				timeout_counter = 0;
				printf("命令处理超时，重置系统\r\n");
				printf("等待新命令...\r\n");
				// 强制清除接收状态
				USART_RX_STA = 0;
				memset(USART_RX_BUF, '\0', sizeof(USART_RX_BUF));
			}
			// 添加一个短暂延时，避免CPU占用过高
			delay_us(10);
			continue;  // 跳过本次循环，等待当前命令处理完成
		}
		
		if(USART_RX_STA & 0x8000) //Check if data is received
		{					   
			cmd_processing = 1;  // 设置命令处理标志
			cmd_received = 1;    // 设置命令接收标志
			timeout_counter = 0;  // 重置超时计数器
			
			printf("接收状态: 0x%04X\r\n", USART_RX_STA); // 打印接收状态
			
			Simbol = USART_RX_BUF[0];//Get the first character from the buffer
			//Used to determine the interpolation mode (line, circle)
			printf("收到命令: %c\r\n", Simbol); // 添加调试输出
			
			// 打印接收到的完整数据（十六进制）
			printf("接收数据(HEX): ");
			for(int i=0; i<(USART_RX_STA&0x3FFF); i++)
			{
				printf("%02X ", USART_RX_BUF[i]);
			}
			printf("\r\n");
			
			// 打印接收到的完整数据（ASCII）
			printf("接收数据(ASCII): ");
			for(int i=0; i<(USART_RX_STA&0x3FFF); i++)
			{
				printf("%c", USART_RX_BUF[i]);
			}
			printf("\r\n");
			
			switch(Simbol)
			{
				case 'L':  // 线性插补命令
				printf("命令: 线性插补\r\n");
				
				// 检查数据长度是否足够
				if((USART_RX_STA&0x3FFF) >= 19) {
					// 解析16进制脉冲数 (XXXX)
					pulses = 0;
					for(int i=1; i<=4; i++) {
						pulses = pulses * 16 + hex_to_int(USART_RX_BUF[i]);
					}
					
					// 解析速度值 (FFF)
					F = 0;
					for(int i=6; i<=8; i++) {
						if(USART_RX_BUF[i] >= '0' && USART_RX_BUF[i] <= '9') {
							F = F * 10 + (USART_RX_BUF[i] - '0');
						}
					}
					
					// 解析方向 (DD)
					dir = 0;
					if(USART_RX_BUF[10] == '0' && USART_RX_BUF[11] == '0') {
						dir = 1;  // 正转
					} else if(USART_RX_BUF[10] == '0' && USART_RX_BUF[11] == '1') {
						dir = 0;  // 反转
					}
					
					// 解析细分值 (SSS)
					subdivide = 0;
					for(int i=13; i<=15; i++) {
						if(USART_RX_BUF[i] >= '0' && USART_RX_BUF[i] <= '9') {
							subdivide = subdivide * 10 + (USART_RX_BUF[i] - '0');
						}
					}
					
					// 检查校验结尾 (ed)
					if(USART_RX_BUF[17] == 'e' && USART_RX_BUF[18] == 'd') {
						printf("校验通过\r\n");
						
						// 打印解析结果
						printf("解析结果: 脉冲数=%d, 速度=%d, 方向=%d, 细分=%d\r\n", 
							pulses, F, dir, subdivide);
						
						// 检查是否有电机正在运行
						if (Is_Motor_Busy()) {
							printf("电机忙，停止当前运动并开始新的运动\r\n");
							Motor_Stop_All();  // 停止当前运动
							delay_ms(100);    // 等待电机完全停止
						}
						
						printf("开始执行电机运动\r\n");
						
						// 使用非阻塞方式控制电机
						Bujin1_Move_Variable(dir, pulses, F, subdivide);
						
						// 立即释放命令处理标志，允许接收新命令
						cmd_processing = 0;
					} else {
						printf("校验失败\r\n");
					}
				} else {
					printf("数据长度不足\r\n");
				}
				break;
				
				case 'S':  // 停止命令
				printf("命令: 停止电机\r\n");
				Motor_Stop_All();  // 停止所有电机
				printf("已停止所有电机\r\n");
				break;
				
				case 'T':  // 测试命令
				printf("命令: 测试\r\n");
				
				// 立即发送确认
				printf("test_ack\r\n");
				
				// 直接使用USART_SendData发送确认
				char *test_confirm = "test_ack\r\n";
				for(int i=0; test_confirm[i] != '\0'; i++) {
					USART_SendData(USART1, test_confirm[i]);
					// 等待发送完成
					while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
				}
				
				// 测试电机 - 使用非阻塞方式输出100个脉冲
				printf("测试电机 - 输出100个脉冲\r\n");
				Bujin1_Move_Variable(1, 100, 50, 200);
				printf("测试命令已发送\r\n");
				break;
				
				default:
				printf("未知命令: %c\r\n", Simbol);
				break;
			}
			
			// 清空接收缓冲区
			memset(USART_RX_BUF, '\0', sizeof(USART_RX_BUF));
			// 重置接收状态
			USART_RX_STA = 0;
			
			printf("命令处理完成\r\n");
			printf("等待新命令...\r\n");
			
			cmd_processing = 0;  // 清除命令处理标志
		}	
		
		// 添加短暂延时，避免CPU占用过高
		delay_us(10);
	} 
}

// 支持可变细分值的线性插补函数
void PBP_Line_Variable(int step, int XEnd, int YEnd, int F, int subdivide)
{
     int Fi = F;
     int IDevVal;//偏差值
     int xCurVal=0, yCurVal=0;//当前点的实际位置
     int StepMount;//插补总步数
     int StepCount=0;//插补次数计数器
     XEnd=abs(XEnd);
     YEnd=abs(YEnd);

     StepMount=(XEnd+YEnd)/step; //计算插补总步数
     IDevVal=yCurVal*XEnd - xCurVal*YEnd;//偏差计算
     
     printf("开始线性插补: XEnd=%d, YEnd=%d, 总步数=%d, 细分=%d\r\n", XEnd, YEnd, StepMount, subdivide);
     
     while (StepCount<StepMount)//终点判别
     {
         if (IDevVal>=0) //偏差>=0
         {
             int nDir=Judge_Quadrant(XEnd,YEnd); //象限判断
             switch(nDir)
             {
             case 1:
                 xCurVal+=step;
                 Bujin1_Move_Variable(1, 1, Fi, subdivide);
                 printf("0");
                 break;
             case 2:
                 xCurVal-=step;
                 Bujin1_Move_Variable(0, 1, Fi, subdivide);
                 printf("2");
                 break;
             case 3:
                 xCurVal-=step;
                 Bujin1_Move_Variable(0, 1, Fi, subdivide);
                 printf("2");
                 break;
             case 4:
                 xCurVal+=step;
                 Bujin1_Move_Variable(1, 1, Fi, subdivide);
                 printf("0");
                 break;
             }
             IDevVal-=YEnd;
         }
         else//偏差<0
         { 
             int nDir=Judge_Quadrant(XEnd,YEnd); //象限判断
             switch(nDir) 
             {
             case 1:
                 yCurVal+=step;
                 Bujin2_Move_Variable(1, 1, Fi, subdivide);
                 printf("1");
                 break;
             case 2:
                 yCurVal+=step;
                 Bujin2_Move_Variable(1, 1, Fi, subdivide);
                 printf("1");
                 break;
             case 3:
                 yCurVal-=step;
                 Bujin2_Move_Variable(0, 1, Fi, subdivide);
                 printf("3");
                 break;
             case 4:
                 yCurVal-=step;
                 Bujin2_Move_Variable(0, 1, Fi, subdivide);
                 printf("3");
                 break;
             }
             IDevVal+=XEnd;
         }
         StepCount++;
     }
     
     printf("\r\n线性插补完成，共执行%d步\r\n", StepCount);
}
