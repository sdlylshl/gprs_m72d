

#include "msp430x54x.h"
#include "GPS.h"
#define GPS_ANT_OK_NUM  100

char GPS_ANT_STATUS_Flag;//AA==天线闭合；55==天线断开
unsigned long int GPS_ANT_OPEN_CNT; //GPS天线开路状态记录次数
unsigned long int GPS_ANT_Close_CNT;//GPS天线闭合状态记录次数
unsigned long int GPS_ANT_IO_CNT;//GPS天线次数

/*******************************************************************\
*	      函数名：GPS_ANT_STATUS             
*	      作用域：外部文件调用
*	      功能：  GPS天线检测  
*	      参数：  
           1、20Ms=检测1次；

*	      返回值：无     
*
*	      修改历史：（每条详述）
\*******************************************************************/
void GPS_ANT_STATUS(void)//GPS天线检测
{
  if(GPS_ANT_IO_CNT>10)
  {
      GPS_ANT_IO_CNT=0;
      if(((P9IN&GPS_ANT)==GPS_ANT))
      {
         if((GPS_ANT_OK_NUM==GPS_ANT_OPEN_CNT++))
         {
             GPS_ANT_OPEN_CNT=0;
             GPS_ANT_Close_CNT=0;
             GPS_ANT_STATUS_Flag=0x55;
             Module_Status[0] |=0x30;//GPS天线状态开路
             GPS_GSM_System_Stu[0]|=0x30;
         }
      }
      
      if(((P9IN&GPS_ANT)==0x00))
      {
        if(GPS_ANT_OK_NUM==GPS_ANT_Close_CNT++)
        {
             GPS_ANT_Close_CNT=0;
             GPS_ANT_OPEN_CNT=0;
             GPS_ANT_STATUS_Flag=0xAA;
             Module_Status[0] &=~0x30;//GPS天线状态闭合
             GPS_GSM_System_Stu[0]&=~0x30;
        }
      }
  }
}




