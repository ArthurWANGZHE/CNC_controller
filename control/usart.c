#include "sys.h"
#include "usart.h"	  
#include "stdio.h"
#include "motor.h"
#include "string.h"

#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif

#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
char USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  
  
void uart_init(u32 bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
  //USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART1, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                    //使能串口1 

}

// 全局变量定义
u8 rxBuffer[100]; // 接收缓冲区
u8 rxIndex = 0;   // 当前接收索引
u8 frameState = 0; // 帧解析状态

// 计算校验和
u8 CalculateChecksum(u8* data, u16 length) {
    u8 sum = 0;
    for (u16 i = 0; i < length; i++) {
        sum += data[i];
    }
    return sum;
}

// 解析接收到的数据
void ProcessReceivedData(u8* data, u16 length) {
    // 检查数据长度是否足够
    if (length < 35) { // 至少需要帧头(2) + 命令码(1) + 长度(1) + 数据(31) + 校验和(1)
        return;
    }
    
    // 检查帧头
    if (data[0] != FRAME_HEADER1 || data[1] != FRAME_HEADER2) {
        return;
    }
    
    // 检查数据长度
    if (data[3] != 0x1F) { // 数据长度应为31字节
        return;
    }
    
    // 检查校验和
    u8 calculatedChecksum = 0;
    for (u16 i = 0; i < 34; i++) { // 帧头+命令码+长度+数据 = 34字节
        calculatedChecksum += data[i];
    }
    
    if (calculatedChecksum != data[34]) {
        // 校验和错误
        return;
    }
    
    // 解析数据
    ProtocolData_TypeDef protocolData;
    protocolData.algorithm = data[4];      // 算法类型
    protocolData.quadrant = data[5];      // 象限编号
    protocolData.direction = data[6];     // 方向
    
    // 解析坐标（大端格式）
    protocolData.start_x = (data[7] << 24) | (data[8] << 16) | (data[9] << 8) | data[10];
    protocolData.start_y = (data[11] << 24) | (data[12] << 16) | (data[13] << 8) | data[14];
    protocolData.end_x = (data[15] << 24) | (data[16] << 16) | (data[17] << 8) | data[18];
    protocolData.end_y = (data[19] << 24) | (data[20] << 16) | (data[21] << 8) | data[22];
    protocolData.center_x = (data[23] << 24) | (data[24] << 16) | (data[25] << 8) | data[26];
    protocolData.center_y = (data[27] << 24) | (data[28] << 16) | (data[29] << 8) | data[30];
    
    // 解析速度和细分值
    protocolData.speed = (data[31] << 8) | data[32];
    protocolData.subdivision = data[33];
    
    // 根据命令码执行相应操作
    int inc_x, inc_y;
    
    switch (data[2]) { // 命令码
        case CMD_LINEAR: // 直线插补
            // 计算直线增量
            inc_x = protocolData.end_x - protocolData.start_x;
            inc_y = protocolData.end_y - protocolData.start_y;
            
            // 打印接收到的命令信息
            printf("\r\n收到直线插补命令: 象限=%d, 增量X=%d, 增量Y=%d, 速度=%d\r\n", 
                   protocolData.quadrant, inc_x, inc_y, protocolData.speed);
            
            // 调用相应的插补函数
            switch (protocolData.quadrant) {
                case 1: // 第一象限
                    printf("\r\n调用Linear_InterPolation_Move1\r\n");
                    Linear_InterPolation_Move1(inc_x, inc_y, protocolData.speed);
                    // 发送确认消息
                    printf("\r\n直线插补第一象限执行完成\r\n");
                    USART_SendData(USART1, 0xAA); // 发送确认字节
                    break;
                case 2: // 第二象限
                    printf("\r\n调用Linear_InterPolation_Move2\r\n");
                    Linear_InterPolation_Move2(inc_x, inc_y, protocolData.speed);
                    // 发送确认消息
                    printf("\r\n直线插补第二象限执行完成\r\n");
                    USART_SendData(USART1, 0xAA); // 发送确认字节
                    break;
                case 3: // 第三象限
                    printf("\r\n调用Linear_InterPolation_Move3\r\n");
                    Linear_InterPolation_Move3(inc_x, inc_y, protocolData.speed);
                    // 发送确认消息
                    printf("\r\n直线插补第三象限执行完成\r\n");
                    USART_SendData(USART1, 0xAA); // 发送确认字节
                    break;
                case 4: // 第四象限
                    printf("\r\n调用Linear_InterPolation_Move4\r\n");
                    Linear_InterPolation_Move4(inc_x, inc_y, protocolData.speed);
                    // 发送确认消息
                    printf("\r\n直线插补第四象限执行完成\r\n");
                    USART_SendData(USART1, 0xAA); // 发送确认字节
                    break;
                default:
                    printf("\r\n无效的象限值: %d\r\n", protocolData.quadrant);
                    USART_SendData(USART1, 0xFF); // 发送错误字节
                    break;
            }
            break;
            
        case CMD_CIRCULAR: // 圆弧插补
            // 打印接收到的命令信息
            printf("\r\n收到圆弧插补命令: 象限=%d, 方向=%d, 起点=(%d,%d), 终点=(%d,%d), 圆心=(%d,%d), 速度=%d\r\n", 
                   protocolData.quadrant, protocolData.direction, 
                   protocolData.start_x, protocolData.start_y, 
                   protocolData.end_x, protocolData.end_y, 
                   protocolData.center_x, protocolData.center_y, 
                   protocolData.speed);
            
            // 调用相应的插补函数
            if (protocolData.direction == DIR_ACW) { // 逆时针
                switch (protocolData.quadrant) {
                    case 1: // 第一象限
                        printf("\r\n调用Circular_InterPolation_ACW1\r\n");
                        Circular_InterPolation_ACW1(protocolData.start_x, protocolData.start_y, 
                                                protocolData.end_x, protocolData.end_y, protocolData.speed);
                        // 发送确认消息
                        printf("\r\n圆弧插补第一象限（逆时针）执行完成\r\n");
                        USART_SendData(USART1, 0xAA); // 发送确认字节
                        break;
                    case 2: // 第二象限
                        printf("\r\n调用Circular_InterPolation_ACW2\r\n");
                        Circular_InterPolation_ACW2(protocolData.start_x, protocolData.start_y, 
                                                protocolData.end_x, protocolData.end_y, protocolData.speed);
                        // 发送确认消息
                        printf("\r\n圆弧插补第二象限（逆时针）执行完成\r\n");
                        USART_SendData(USART1, 0xAA); // 发送确认字节
                        break;
                    case 3: // 第三象限
                        printf("\r\n调用Circular_InterPolation_ACW3\r\n");
                        Circular_InterPolation_ACW3(protocolData.start_x, protocolData.start_y, 
                                                protocolData.end_x, protocolData.end_y, protocolData.speed);
                        // 发送确认消息
                        printf("\r\n圆弧插补第三象限（逆时针）执行完成\r\n");
                        USART_SendData(USART1, 0xAA); // 发送确认字节
                        break;
                    case 4: // 第四象限
                        printf("\r\n调用Circular_InterPolation_ACW4\r\n");
                        Circular_InterPolation_ACW4(protocolData.start_x, protocolData.start_y, 
                                                protocolData.end_x, protocolData.end_y, protocolData.speed);
                        // 发送确认消息
                        printf("\r\n圆弧插补第四象限（逆时针）执行完成\r\n");
                        USART_SendData(USART1, 0xAA); // 发送确认字节
                        break;
                    default:
                        printf("\r\n无效的象限值: %d\r\n", protocolData.quadrant);
                        break;
                }
            } else { // 顺时针
                switch (protocolData.quadrant) {
                    case 1: // 第一象限
                        printf("\r\n调用Circular_InterPolation_CW1\r\n");
                        Circular_InterPolation_CW1(protocolData.start_x, protocolData.start_y, 
                                                protocolData.end_x, protocolData.end_y, protocolData.speed);
                        // 发送确认消息
                        printf("\r\n圆弧插补第一象限（顺时针）执行完成\r\n");
                        USART_SendData(USART1, 0xAA); // 发送确认字节
                        break;
                    case 2: // 第二象限
                        printf("\r\n调用Circular_InterPolation_CW2\r\n");
                        Circular_InterPolation_CW2(protocolData.start_x, protocolData.start_y, 
                                                protocolData.end_x, protocolData.end_y, protocolData.speed);
                        // 发送确认消息
                        printf("\r\n圆弧插补第二象限（顺时针）执行完成\r\n");
                        USART_SendData(USART1, 0xAA); // 发送确认字节
                        break;
                    case 3: // 第三象限
                        printf("\r\n调用Circular_InterPolation_CW3\r\n");
                        Circular_InterPolation_CW3(protocolData.start_x, protocolData.start_y, 
                                                protocolData.end_x, protocolData.end_y, protocolData.speed);
                        // 发送确认消息
                        printf("\r\n圆弧插补第三象限（顺时针）执行完成\r\n");
                        USART_SendData(USART1, 0xAA); // 发送确认字节
                        break;
                    case 4: // 第四象限
                        printf("\r\n调用Circular_InterPolation_CW4\r\n");
                        Circular_InterPolation_CW4(protocolData.start_x, protocolData.start_y, 
                                                protocolData.end_x, protocolData.end_y, protocolData.speed);
                        // 发送确认消息
                        printf("\r\n圆弧插补第四象限（顺时针）执行完成\r\n");
                        USART_SendData(USART1, 0xAA); // 发送确认字节
                        break;
                    default:
                        printf("\r\n无效的象限值: %d\r\n", protocolData.quadrant);
                        break;
                }
            }
            break;
    }
    
    // 发送响应给上位机（可选）
    printf("Command received and executed\r\n");
}

void USART1_IRQHandler(void)                	//串口1中断服务程序
	{
	char Res;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
		{
		Res = USART_ReceiveData(USART1);	//读取接收到的数据
		
		// 使用状态机来解析协议帧
		switch(frameState) {
			case 0: // 等待帧头1
				if(Res == FRAME_HEADER1) {
					rxBuffer[0] = Res;
					frameState = 1;
					rxIndex = 1;
				}
				break;
				
			case 1: // 等待帧头2
				if(Res == FRAME_HEADER2) {
					rxBuffer[1] = Res;
					frameState = 2;
					rxIndex = 2;
				} else {
					frameState = 0; // 重置状态
				}
				break;
				
			case 2: // 接收命令码
				rxBuffer[2] = Res;
				frameState = 3;
				rxIndex = 3;
				break;
				
			case 3: // 接收数据长度
				rxBuffer[3] = Res;
				frameState = 4;
				rxIndex = 4;
				break;
				
			case 4: // 接收数据和校验和
				rxBuffer[rxIndex++] = Res;
				
				// 当接收到所有数据和校验和后处理
				if(rxIndex >= 35) { // 帧头(2) + 命令码(1) + 长度(1) + 数据(31) + 校验和(1)
					ProcessReceivedData(rxBuffer, rxIndex);
					frameState = 0; // 重置状态机
				}
				break;
		}
     } 
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
} 
#endif	

