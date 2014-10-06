

#include "GSM.h"

//#include "E:\工作中\2013\GPS一体机\第一版 -修订2\2013.12.20--00\Source\Global.h"


//0 正常 0x01关机 0x02休眠 0x04 SIM卡 0x08 GSM网络正常 0x10GPRS网络正常 0x20 TCPIP正常   
char GSM_Brust()//突发数据处理
{
      char Brst_Num                                           ;
      switch (M72D_Brust_STATUS)
      {
          case Receive_Gprs_Data:
            {//M72D_Brust_STATUS = 0x01; //+QIRDI: 0,1,0 [GPRS 数据到达 AT+QINDI=1]
                Delayms(5)                                    ;//XX*1Ms延时
                RD_Data_1_Min_Cnt=RD_Data_Time+1              ;//解析 MUC 应答时间计数
                RD_UCA0_Time=10240;
                RecData()                                     ;//解析并将数据转存
                break                                         ;
            }
            
          case 0x02:
            {//+CMTI:<mem>,<index> [短消息到达 AT+CNMI=2,1]
                break                                         ;
            }
            
          case GSM_Get_GPS:
            {//+CMTI:<mem>,<index> [短消息到达 AT+CNMI=2,1]
                Do_GSM_GPS_Data();//处理GSM接收GPS数据
                break;
            } 
            
          case 0x04:
            {//+CPIN: READY [SIM 状态检测 AT+CPIN?]
                Module_Status[2] |= 0XC0;//取卡报警
                GPS_GSM_System_Stu[2] |= 0XC0; 
                Write_No_Chg_Card_Flash();//写换卡无卡Flash 
                CONNECT_FAIL_Flag=0x11;//CONNECT FAIL允许使能
                CONNECT_FAIL_RESET();//处理Conect_Fail链接
                break                                         ;
            }
            
          case 0x08:
            {//M72D_Brust_STATUS = 0x02; //+CREG: 1
                CONNECT_FAIL_Flag     =0x11                   ;//CONNECT FAIL允许使能
                CONNECT_FAIL_RESET()                          ;//处理Conect_Fail链接
                break                                         ;
            }

          case 0x20:
            {//+PDP DEACT [GPRS链路(TCPIP)断开 ]
                CONNECT_FAIL_Flag     =0x11                   ;//CONNECT FAIL允许使能
                CONNECT_FAIL_RESET()                          ;//处理Conect_Fail链接
                break                                         ;
            }
            
          case 0x40:
            {//+CFUN: 1 [全功能工作]

                break                                         ;
            }
            
          case 0x80:
            {//NORMAL_POWER_DOWN
                GSM_NOM_POW_ON()                              ;
                UDP_Built_flag      =0x00                     ;//00==网络断开或者没有建立
                Heart_Beat_Count    =0                        ;//发送心跳1ms时间计数
                if(MCU_Data_Count>(MCU_TimeOut>>2))
                    MCU_Data_Count =   MCU_TimeOut >>  2      ;//1ms 数据包计数
                break                                         ;
            }
            
            
          default:
            {
                break                                         ;
            }
      }
    
     if(M72D_Brust_STATUS!=0)
     {//清缓冲器
           for(Brst_Num=0;Brst_Num<M72D_Brust_RX_BUF_SIZE;Brst_Num++)
        {
            M72D_Brust_RX_Buf[Brst_Num]=0x00                  ;
        }
        M72D_Brust_STATUS=0                                   ;
    }  
    return 0                                                  ;
}


