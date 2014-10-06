#include "msp430x54x.h"
#include "PortIO.h"
#include ".\GPS\GPS.h"
#include ".\GSM\GSM.h"

unsigned char LPM_GPS_CNT; 
unsigned long LPM_SHAKE_CNT;

extern unsigned char LPM;
char LPM3_Open_Box_Gsm_Flag;
extern unsigned char GSM_STATUS;

extern void Init_LPM_IO();

extern unsigned char LPM_GPS_LOCATE_OK;//低功耗模式下,GPS定位成功
unsigned char Power_down2on;
/*
void Init_LPM_IO()
{
    //通过PxREN控制上下拉电阻使能，通过PxOUT决定上拉还是下拉
    //上下拉电阻 30K~50K
    //驱动能力 低驱电流为2mA 高驱电流为 5mA
    //所有I/O电流总和不超过48mA
    P1OUT    = 0x00;//输出为0
    P1DIR    = 0x00;//输入
    P1REN    = 0X00;//禁用电阻
    P1DS     = 0X00;//低驱动能力
    P1SEL    = 0X00;//普通IO
    P1IES    = 0X00;//上升沿触发
    P1IE     = 0X00;//禁用中断
    P1IFG    = 0X00;//无中断触发

    P2OUT    = 0x00;//输出为0
    P2DIR    = 0x00;//输入
    P2REN    = 0X00;//禁用电阻
    P2DS     = 0X00;//低驱动能力
    P2SEL    = 0X00;//普通IO
    P2IES    = 0X00;//上升沿触发
    P2IE     = 0X00;//禁用中断
    P2IFG    = 0X00;//无中断触发

    P3OUT    = 0x00;//输出为0
    P3DIR    = 0x00;//输入
    P3REN    = 0X00;//禁用电阻
    P3DS     = 0X00;//低驱动能力
    P3SEL    = 0X00;//普通IO

    P4OUT    = 0x00;//输出为0
    P4DIR    = 0x00;//输入
    P4REN    = 0X00;//禁用电阻
    P4DS     = 0X00;//低驱动能力
    P4SEL    = 0X00;//普通IO

    P5OUT    = 0x00;//输出为0
    P5DIR    = 0x00;//输入
    P5REN    = 0X00;//禁用电阻
    P5DS     = 0X00;//低驱动能力
    P5SEL    = 0X00;//普通IO

    P6OUT    = 0x00;//输出为0
    P6DIR    = 0x00;//输入
    P6REN    = 0X00;//禁用电阻
    P6DS     = 0X00;//低驱动能力
    P6SEL    = 0X00;//普通IO

    P7OUT    = 0x00;//输出为0
    P7DIR    = 0x00;//输入
    P7REN    = 0X00;//禁用电阻
    P7DS     = 0X00;//低驱动能力
    P7SEL    = 0X00;//普通IO

    P8OUT    = 0x00;//输出为0
    P8DIR    = 0x00;//输入
    P8REN    = 0X00;//禁用电阻
    P8DS     = 0X00;//低驱动能力
    P8SEL    = 0X00;//普通IO


    P9OUT    = 0x00;//输出为0
    P9DIR    = 0x00;//输入
    P9REN    = 0X00;//禁用电阻
    P9DS     = 0X00;//低驱动能力
    P9SEL    = 0X00;//普通IO

    P10OUT    = 0x00;//输出为0
    P10DIR    = 0x00;//输入
    P10REN    = 0X00;//禁用电阻
    P10DS     = 0X00;//低驱动能力
    P10SEL    = 0X00;//普通IO

    P11OUT    = 0x00;//输出为0
    P11DIR    = 0x00;//输入
    P11REN    = 0X00;//禁用电阻
    P11DS     = 0X00;//低驱动能力
    P11SEL    = 0X00;//普通IO
}

*/
void GPS_Start()
{
    P5DIR   |=  BIT4                               ; //  P5.4 outputs
    P5OUT   |=  BIT4                               ; // GPSPWR ON
    //P5OUT   &=  ~BIT4                               ; // GPSPWR OFF  
}
void GPS_Stop()
{
    P5DIR   |=  BIT4                               ; //  P5.4 outputs
    P5OUT   &=  ~BIT4                               ; // GPSPWR ON
    //P5OUT   &=  ~BIT4                               ; // GPSPWR OFF  
}


#pragma vector=PORT2_VECTOR
__interrupt void Port2(void)
{
	
	__delay_cycles(1000);
    //测试用
    //P1OUT  ^= BIT0;//P1.0状态取反,控制LED

    //上电检测 p1.0  P2.0   IO中断
    //上电是高电平
    if((P2IFG&PWRON)) {//P2.0上升沿触发
        //退出低功耗模式
        LPM = 0;
        //关闭 上电、震动、开盒
        Power_down2on=1;
		LPM3_EXIT;

    } else if(P2IFG&SHAKE) {
        //震动唤醒      P2.1        IO中断
        //关中断
		LPM_SHAKE_CNT++;
		if(LPM_SHAKE_CNT==1000)
		{
			Close_Shake();
        	//LPM = 2;
			LPM3_EXIT;
		}
        
    } else if(P2IFG&OPEN) {
        //开盒唤醒      P2.3        IO中断
      //LPM = 5;//08.29DIGH
      /*if((P2IES&OPEN)==0x00)
      {
        LPM3_Open_Box_Gsm_Flag=0xAA;
        Open_Box_Flag=0xAA;
        Close_Open();
      //Init_Open_Down();
      }*/
      /*else
      {
          if((P2IES&OPEN)==OPEN)
          {
              Open_Box_Flag=0x55;
              Close_Open();
              Init_Open_Up();
              
          }
      }*/
      LPM3_Open_Box_Gsm_Flag=0xAA;
      LPM3_EXIT;
      
    } else if(P2IFG&RING) {
        //来电唤醒 p2.0  P2.6       IO中断
        //LPM = 6;
		LPM3_EXIT;
    }
	
    
    
    
    
	if(P2IFG&LOCATE)//使用P2IFG比较好
	{
	    LPM_GPS_CNT++;
		if(LPM_GPS_CNT==5)
		{
			LPM_GPS_CNT=0;
			LPM_GPS_LOCATE_OK=1;
			Close_Locate();
			LPM3_EXIT;
		}
			
	}
    P2IFG=0;//清中断标志
    //LPM3_EXIT;
}





      




