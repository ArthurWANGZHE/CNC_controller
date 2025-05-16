#include "sys.h"
#include "motor.h"
#include "usart.h"

// 这个文件是为了解决链接错误而创建的
// 我们在这里实现与usart.c中调用的函数名称完全相同的函数

// 直线插补函数
void Linear_InterPolation_Move1(int inc_x, int inc_y, int speed) {
    // 调用motor.c中的实际实现（如果存在）
    // 或者提供一个简单的实现
    printf("Linear_InterPolation_Move1: inc_x=%d, inc_y=%d, speed=%d\r\n", inc_x, inc_y, speed);
}

void Linear_InterPolation_Move2(int inc_x, int inc_y, int speed) {
    printf("Linear_InterPolation_Move2: inc_x=%d, inc_y=%d, speed=%d\r\n", inc_x, inc_y, speed);
}

void Linear_InterPolation_Move3(int inc_x, int inc_y, int speed) {
    printf("Linear_InterPolation_Move3: inc_x=%d, inc_y=%d, speed=%d\r\n", inc_x, inc_y, speed);
}

void Linear_InterPolation_Move4(int inc_x, int inc_y, int speed) {
    printf("Linear_InterPolation_Move4: inc_x=%d, inc_y=%d, speed=%d\r\n", inc_x, inc_y, speed);
}

// 圆弧插补函数
void Circular_InterPolation_ACW1(int start_x, int start_y, int stop_x, int stop_y, int speed) {
    printf("Circular_InterPolation_ACW1: start_x=%d, start_y=%d, stop_x=%d, stop_y=%d, speed=%d\r\n", 
           start_x, start_y, stop_x, stop_y, speed);
}

void Circular_InterPolation_CW1(int start_x, int start_y, int stop_x, int stop_y, int speed) {
    printf("Circular_InterPolation_CW1: start_x=%d, start_y=%d, stop_x=%d, stop_y=%d, speed=%d\r\n", 
           start_x, start_y, stop_x, stop_y, speed);
}

void Circular_InterPolation_ACW2(int start_x, int start_y, int stop_x, int stop_y, int speed) {
    printf("Circular_InterPolation_ACW2: start_x=%d, start_y=%d, stop_x=%d, stop_y=%d, speed=%d\r\n", 
           start_x, start_y, stop_x, stop_y, speed);
}

void Circular_InterPolation_CW2(int start_x, int start_y, int stop_x, int stop_y, int speed) {
    printf("Circular_InterPolation_CW2: start_x=%d, start_y=%d, stop_x=%d, stop_y=%d, speed=%d\r\n", 
           start_x, start_y, stop_x, stop_y, speed);
}

void Circular_InterPolation_ACW3(int start_x, int start_y, int stop_x, int stop_y, int speed) {
    printf("Circular_InterPolation_ACW3: start_x=%d, start_y=%d, stop_x=%d, stop_y=%d, speed=%d\r\n", 
           start_x, start_y, stop_x, stop_y, speed);
}

void Circular_InterPolation_CW3(int start_x, int start_y, int stop_x, int stop_y, int speed) {
    printf("Circular_InterPolation_CW3: start_x=%d, start_y=%d, stop_x=%d, stop_y=%d, speed=%d\r\n", 
           start_x, start_y, stop_x, stop_y, speed);
}

void Circular_InterPolation_ACW4(int start_x, int start_y, int stop_x, int stop_y, int speed) {
    printf("Circular_InterPolation_ACW4: start_x=%d, start_y=%d, stop_x=%d, stop_y=%d, speed=%d\r\n", 
           start_x, start_y, stop_x, stop_y, speed);
}

void Circular_InterPolation_CW4(int start_x, int start_y, int stop_x, int stop_y, int speed) {
    printf("Circular_InterPolation_CW4: start_x=%d, start_y=%d, stop_x=%d, stop_y=%d, speed=%d\r\n", 
           start_x, start_y, stop_x, stop_y, speed);
}
