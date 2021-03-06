//===========================================================================//
//                                                                           //
// 文件：  MAIN.C                                                            //
//                                                                           //
//===========================================================================//
/******************************************************************************
*名    称：USCI_B2_ISR()

*功    能：1.接收ARM传来的数据并进行解析处理
		   2.在中断中给ARM回应数据

*入口参数：无

*出口参数：无

*说    明：无

*修改历史:无
******************************************************************************/

/********************************************************\
*	 函数名：Delayms
     作用域：外部文件调用
*	 功能：  延时1ms
*	 参数：  
*    返回值：无
*	 修改历史：（每条详述）
\********************************************************/
#include "msp430x54x.h"




void Delayms(unsigned long int ms)
{
    unsigned int n;                               //10 Cylce
    for(n=0; n<ms; n++)                           //(N+1)Cylce
        __delay_cycles(15996);                    //(n+3)Cylce
}


//修改时间 蒋本庆 2014.01.06 

//------------------------------------------------------------------------------
//		主函数
//------------------------------------------------------------------------------
void main( void ) 
{
  
    P3DIR |= BIT4 + BIT5; // TX设置为输出

    while(1)
    {
        //Delayms(10)      ;
        P3OUT |= BIT4 + BIT5;  
        //Delayms(10)      ;
        P3OUT &=~BIT4 + BIT5;//GSMPWR L
       // Delayms(10)      ;
    
    }
}




