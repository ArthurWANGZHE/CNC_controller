#ifndef __BUJIN_NEW_H__
#define __BUJIN_NEW_H__

#include "sys.h"
#include "timer.h"  // 添加定时器头文件

void My_Bujin_IO_Init(void);
void Bujin1_Move_Variable(u16 model, u16 pulses, u16 F, u16 subdivide);
void Bujin2_Move_Variable(u16 model, u16 pulses, u16 F, u16 subdivide);

#endif
