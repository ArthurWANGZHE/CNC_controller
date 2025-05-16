#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收

// 协议相关定义
#define FRAME_HEADER1    0xAA    // 帧头1
#define FRAME_HEADER2    0x55    // 帧头2

// 命令码定义
#define CMD_LINEAR       0x01    // 直线插补
#define CMD_CIRCULAR     0x02    // 圆弧插补

// 算法类型
#define ALGO_POINT       0x01    // 逐点比较法
#define ALGO_DDA         0x02    // 数字积分法（DDA）
#define ALGO_BRESENHAM   0x03    // Bresenham算法

// 方向定义
#define DIR_CW           0x00    // 顺时针
#define DIR_ACW          0x01    // 逆时针

// 协议数据结构
typedef struct {
    u8 algorithm;        // 算法类型
    u8 quadrant;         // 象限编号
    u8 direction;        // 方向
    int start_x;         // 起点X坐标
    int start_y;         // 起点Y坐标
    int end_x;           // 终点X坐标
    int end_y;           // 终点Y坐标
    int center_x;        // 圆心X坐标
    int center_y;        // 圆心Y坐标
    u16 speed;           // 速度
    u8 subdivision;      // 细分值
} ProtocolData_TypeDef;

extern char  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	

void uart_init(u32 bound);
void ProcessReceivedData(u8* data, u16 length);
u8 CalculateChecksum(u8* data, u16 length);
#endif


