#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include "chabu.h"
#include "bujin.h"
#include <stdbool.h>
#include <stdint.h>
/******************************************** *************/
/*函数 名: InsertPoint*/
/*功能:控制机床各轴进给并将进给结果写入文件中*/

/*说明:与机床硬件关联，每产生-一个点调用一次*/
/********************
********************************** *** ***********/

/**************************************************/
/*函数 名: Judge_ Quadrant*/
/*功能:判断参数坐标的所在象限并返回相应象限值*/
/*参数: double x mm*/
/*double  y mm*/
/*****************************************************/
int Judge_Quadrant(int x, int y)
{
    if (x>=0)
    {//象限判断
        if (y>=0)
        {
            return 1;
        }
        else
        {
            return 4;
        }
    }

    else
    {
        if (y>=0)
        {
            return 2;
        }
        else
        {
            return 3;
        }
    }
}
/****************************************/
/* 函数名: PBP_Line*/
/*功能:逐点比较法直线插补*/
/*参数: double step 步长 mm */
/*double XEnd, double YEnd 插补终点 mm*/
/****************************************************/
 void PBP_Line(int step,int XEnd,int YEnd,int F)
{
     int Fi = F;
		 int IDevVal;//偏差值
     int xCurVal=0,yCurVal=0;//当前点的实际位置
     int StepMount;//插补总步数
     int StepCount=0;//插补次数计数器
     XEnd=abs(XEnd);
     YEnd=abs(YEnd);

     StepMount=(XEnd+YEnd)/step; //计算插补总步数
     IDevVal=yCurVal*XEnd- xCurVal*YEnd;//偏差计算
     while (StepCount<StepMount)//终点判别
     {
         if (IDevVal>=0) //偏差>=0
         {
             int nDir=Judge_Quadrant(XEnd,YEnd); //象限判断
             switch(nDir)
             {
             case 1:
                 xCurVal+=step;
                 Bujin1_Move(1,1,Fi);
								 printf("0");
                 break;
             case 2:
                 xCurVal-=step;
                 Bujin1_Move(0,1,Fi);
								 printf("2");
                 break;
             case 3:
                 xCurVal-=step;
                 Bujin1_Move(0,1,Fi);
								 printf("2");
                 break;
             case 4:
                 xCurVal+=step;
                 Bujin1_Move(1,1,Fi);
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
								 Bujin2_Move(1,1,Fi);
								 printf("1");

								 break;
						 case 2:
								 yCurVal+=step;
								 Bujin2_Move(1,1,Fi);
								 printf("1");

								 break;
						 case 3:
								 yCurVal-=step;
								 Bujin2_Move(0,1,Fi);
								 printf("3");
								 break;
						 case 4:
								 yCurVal-=step;
								 Bujin2_Move(0,1,Fi);
								 printf("3");

								 break;
						 }
						 IDevVal+=XEnd;
				}
				StepCount++;
    }
 }

/******************************************************/
/*函数名: PBP_ _Circle*/
/*功能:逐点比较法圆弧插补*/

/* 参数: double step  步长  mm */
/*double XStart, double YStart 插补起点 mm */
/* double XEnd, double YEnd 插补终点 mm */
/* double radius圆弧半径 mm */
/* bool blsCW 圆弧插补方向 0或1 */
/*********************************************************/
 
void PBP_Circle(u8 simbol, int XStart, int YStart, int XEnd, int YEnd, int radius, int F)
{
    double IDevVal,goallDevVal;//当前误差和与终点的差值
    double xCurVal=XStart,yCurVal=YStart;//当前坐标
    u16 Fi = F;
		u8 sim = simbol;
    long stepcount=0;//步数
    IDevVal=xCurVal*xCurVal+yCurVal*yCurVal-radius*radius;

    goallDevVal=(xCurVal-XEnd)*(xCurVal-XEnd)+(yCurVal-YEnd)*(yCurVal-YEnd);
    while(goallDevVal>=(1*1)||stepcount==0)
    {
        if(IDevVal>=0)//误差大于等于零
        {
            if(sim == 'N')//逆圆插补
            {
                int nDir=Judge_Quadrant(xCurVal,yCurVal);
                switch(nDir)
                {
                case 1:
                    xCurVal-=1;
                    Bujin1_Move(0,1,Fi);
										printf("2");

                    break;
                case 2:
                    yCurVal-=1;
                    Bujin2_Move(0,1,Fi);
										printf("3");

                    break;
                case 3:
                    xCurVal+=1;
                    Bujin1_Move(1,1,Fi);
										printf("0");

                    break;
                case 4:
                    yCurVal+=1;
                    Bujin2_Move(1,1,Fi);
										printf("1");

                    break;
                }
            }
            else//顺圆插补
            {
                int nDir=Judge_Quadrant(xCurVal,yCurVal);
                switch(nDir)
                {
                case 1:
                    yCurVal-=1;
                    Bujin2_Move(0,1,Fi);
										printf("3");
                    break;
                case 2:
                    xCurVal+=1;
                    Bujin1_Move(1,1,Fi);
										printf("0");
                    break;
                case 3:
                    yCurVal+=1;
                    Bujin2_Move(1,1,Fi);
										printf("1");
                    break;
                case 4:
                    xCurVal-=1;
                    Bujin1_Move(0,1,Fi);
										printf("2");
                    break;

                }
            }
        }
       else//误差小于零
				{if(sim == 'N')//逆圆插补
					{
						int nDir=Judge_Quadrant(xCurVal,yCurVal);
						switch(nDir)
						{
							case 1:
									yCurVal+=1;
									Bujin2_Move(1,1,Fi);
									printf("1");
									break;
							case 2:
									xCurVal-=1;
									Bujin1_Move(0,1,Fi);
									printf("2");
									break;
							case 3:
									yCurVal-=1;
									Bujin2_Move(0,1,Fi);
									printf("3");
									break;
							case 4:
									xCurVal+=1;
									Bujin1_Move(1,1,Fi);
									printf("0");
									break;
							}
					}

					else//顺圆插补
					{
							int nDir=Judge_Quadrant(xCurVal,yCurVal);
							switch(nDir)
							{
							case 1:
									xCurVal+=1;
									Bujin1_Move(1,1,Fi);
									printf("0");
									break;
							case 2:
									yCurVal+=1;
									Bujin2_Move(1,1,Fi);
									printf("1");
									break;
							case 3:
									xCurVal-=1;
									Bujin1_Move(0,1,Fi);
									printf("2");
									break;
							case 4:
									yCurVal-=1;
									Bujin2_Move(0,1,Fi);
									printf("3");
									break;

							}
					}
        }
        IDevVal = xCurVal* xCurVal+yCurVal*yCurVal-radius* radius;
        goallDevVal = (xCurVal-XEnd)*(xCurVal-XEnd)+(yCurVal-YEnd)*(yCurVal-YEnd);
        stepcount++;
      }
      }

      //FILE *fp;//文件指针

			
		