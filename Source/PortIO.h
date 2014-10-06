#include "msp430x54x.h"

extern char Open_Box_Flag;

//上电检测 p1.0  P2.0   IO中断
#define PWRON BIT0
//中断功能
//李凯：上升沿触发       （李守磊：选择下降沿触发）
//打开中断允许
//PxIES的切换可能使P1IFG置位，需清除
#define Init_PWRON()    P2DIR &=  ~PWRON;\
                        P2SEL &=  ~PWRON;\
                        P2IES &=  ~PWRON;\
                        P2IE  |=  PWRON;\
                        P2IFG=0

//作为普通IO 输入 无上拉
#define Close_PWRON()   P2SEL &=  ~PWRON;\
                        P2IE  &=  ~PWRON


//震动唤醒      P2.1        IO中断
#define SHAKE BIT1
//中断功能
//选择下降沿触发
//打开中断允许
//PxIES的切换可能使P1IFG置位，需清除
#define Init_Shake()    P2DIR &=  ~SHAKE;\
                        P2SEL &=  ~SHAKE;\
                        P2IES |=  SHAKE;\
                        P2IE  |=  SHAKE;\
                        P2IFG=0

//作为普通IO 输入 无上拉
#define Close_Shake()   P2SEL &=  ~SHAKE;\
                        P2IE  &=  ~SHAKE


//开盒唤醒      P2.3        IO中断
#define OPEN BIT3
//中断功能
//选择下降沿触发
//打开中断允许
//PxIES的切换可能使P1IFG置位，需清除
#define Init_Open_Up()  P2DIR &=  ~OPEN;\
                        P2SEL &=  ~OPEN;\
                        P2IES &=  ~OPEN;\
                        P2IE  |=   OPEN;\
                        P2IFG=0
/* 闭盒报警屏蔽
#define Init_Open_Down()P2DIR &=  ~OPEN;\
                        P2SEL &=  ~OPEN;\
                        P2IES |=   OPEN;\
                        P2IE  |=   OPEN;\
                        P2IFG=0
*/

//作为普通IO 输入 无上拉
#define Close_Open()    P2SEL &=  ~OPEN;\
                        P2IE  &=  ~OPEN

//来电唤醒 p2.0  P2.6       IO中断
#define RING BIT6
//中断功能
//选择下降沿触发
//打开中断允许
//PxIES的切换可能使P1IFG置位，需清除
#define Init_Ring()     P2DIR &=  ~RING;\
                        P2SEL &=  ~RING;\
                        P2IES |=  RING;\
                        P2IE  |=  RING;\
                        P2IFG=0

//作为普通IO 输入 无上拉
#define Close_Ring()    P2SEL &=  ~RING;\
                        P2IE  &=  ~RING

//GPS定位唤醒   P2.7      IO中断
#define LOCATE BIT7
//中断功能
//选择下降沿触发
//打开中断允许
//PxIES的切换可能使P1IFG置位，需清除
#define Init_Locate()   P2DIR &=  ~LOCATE;\
                        P2SEL &=  ~LOCATE;\
                        P2IES |=  LOCATE;\
                        P2IE  |=  LOCATE;\
                        P2IFG=0

//作为普通IO 输入 无上拉
#define Close_Locate()  P2SEL &=  ~LOCATE;\
                        P2IE  &=  ~LOCATE





//2013.9.4 添加  
//初始化未使用的IO
//未使用的IO设置为普通IO,输出，在PCB上不连接
//P1.1, P1.2, P1.3, P1.4, P1.5, P1.6, P1.7
//P2.2, P2.4, P2.5
//P3.0, P3.1, P3.2, P3.3, P3.6, P3.7
//P4.1, P4.3, P4.4, P4.6
//P5.1, P5.5
//P6.0, P6.1, P6.2, P6.3, P6.5, P6.6, P6.7
//P7.2, P7.5, P7.7, 
//P8.2, P8.3, P8.4, P8.5, P8.6, P8.7
//P9.6, P9.7
//P10.0, P10.3, P10.4, P10.5, P10.6, P10.7
//P11.0, P11.1, P11.2, 
#define Init_NC_IO()   P1DIR|=BIT1+BIT2+BIT3+BIT4+BIT5+BIT6+BIT7;\
					   P2DIR|=BIT2+BIT4+BIT5;\
					   P3DIR|=BIT0+BIT1+BIT2+BIT3+BIT6+BIT7;\
					   P4DIR|=BIT1+BIT3+BIT4+BIT6;\
					   P5DIR|=BIT0+BIT1+BIT5;\
                       P6DIR|=BIT0+BIT1+BIT2+BIT5+BIT6+BIT7;\
					   P7DIR|=BIT2+BIT4+BIT5+BIT6+BIT7;\
					   P8DIR|=BIT2+BIT3+BIT4+BIT5+BIT6+BIT7;\
					   P9DIR|=BIT4+BIT5+BIT6+BIT7;\
					   P10DIR|=BIT0+BIT1+BIT2+BIT3+BIT4+BIT5+BIT6+BIT7;\
					   P11DIR|=BIT0+BIT1+BIT2;\
					   P10OUT&=~BIT6

#define Close_A0()     TA0CTL=0x00;

#define Close_UCA1()   P5SEL&=~(BIT6+BIT7);\
                       P5DIR|=(BIT6+BIT7);\
                       P5OUT&=~(BIT6+BIT7)



#define Close_UCA0()   P3SEL&=~(BIT4+BIT5);\
                       P3DIR|=(BIT4+BIT5);\
                       P3OUT&=~(BIT4+BIT5)