
#include "msp430x54x.h"
#include "GSM.h"

char Module_Status[6]={0X00,0X00,0X00,0x03}                       ;//模块状态信息GPS信息+GSM信息+系统状态1、2+升级状态1、2
char GPS_GSM_System_Stu[4]                                        ;//GSM\GPS状态
char GPS_GSM_ID_Memory[4]                                         ;//一体机设备ID终端号  {0X38,0X1C,0X06,0X00}={0X02,0XB4,0XC4,0x04}  

unsigned long int MCU_TimeOut   =   MSP_A0_Min_30                 ;//数据包发送时间 1843000
unsigned long int MCU_Data_TimeOut=   MSP_A0_Min_1                ;//配置数据包发送时间      
unsigned long int MCU_Data_Count                                  ;//1ms 数据包计数
unsigned long int MCU_ACK_TimeOut                                 ;//1min 查询应答包 无应答则重发
unsigned long int MCU_ACK_Count=0                                 ;//应答包计数

unsigned char Pow3_Send_Times                                     ;//发送次数

char LOW_POW_GSM_Flag =0x55                                       ;//先使用GPS数据处理完成标志 00==未完成；11==完成
char Send_MCU_ERR                                                 ;//发送失败标志 重发
char MCU_Data_ACK                                                 ;//请求发送数据标志位
char GSM_Send_MCU_Buf[106]                                        ;//上报MCU数据命令(回复)

/*******************************************************************\
*	      函数名：GSM_Send_MCU             GPS_ANT_STATUS
*	      作用域：外部文件调用
*	      功能：  发送MCU数据 
*	      参数：  
ID(4)+命令码(2)+GPS信息(4+28)+MCU标志(1)+[MCU数据(62)]+Modbus校验(2)+[结束符(2)]
*	      返回值：无     
*
*	      修改历史：（每条详述）
           1//GSM_MCU[MCU_CNT]=0;去掉此处 2013.12.13 防止出现0数据
\*******************************************************************/
char GSM_Send_MCU(void)
{
    unsigned int tmp;
    unsigned char MCU_CNT;  
    static   unsigned int len;
 
    if((MCU_Data_Count>MCU_Data_TimeOut)&&(UDP_Built_flag==0x11)
       &&(Heart_Beat_Count>10200)&&(Heart_Beat_Count<30715))
    {
        
        MCU_Data_Count=0;
        
        if((MCU_Data_TimeOut<MSP_A0_Min_5)&&(P2IN&PWRON))
        {
            MCU_Data_TimeOut    =   MCU_TimeOut;//配置数据包发送时间
            MCU_ACK_TimeOut     =   MSP_A0_Min_1;
        }
        RD_MCU_TIME_FLASH();//读取时间设置FLASH     
  
        if(LPM3_Open_Box_Gsm_Flag==0xAA)
        {
            GPS_GSM_System_Stu[2]|=0x0C;
            if(LPM3_Open_Send_Time==Pow3_Send_Times++)
            {
                Pow3_Send_Times=0;
                LPM3_Open_Box_Gsm_Flag=0x55;
                GPS_GSM_System_Stu[2]&=~0x0C;
            }
        }
        
        for(MCU_CNT=0;MCU_CNT<4;MCU_CNT++)//GPS 状态转存  
        {
           GSM_Send_MCU_Buf[6+MCU_CNT]=GPS_GSM_System_Stu[MCU_CNT] ; 
        }
    
        /*ARM_DATA_STU &= ~0x04;//GPS数据有效  
        for(MCU_CNT=0;MCU_CNT<28;MCU_CNT++)//GPS数据转存
        {
           GSM_Send_MCU_Buf[6+4+MCU_CNT]=GPS_Inf[MCU_CNT];
        }*///直接发送GPS数据
        
        
        if((ARM_DATA_STU&0x04)==0x04)
        {//GPS数据有效      
            ARM_DATA_STU &= ~0x04;
            for(MCU_CNT=0;MCU_CNT<28;MCU_CNT++)//GPS数据转存
            {
               GSM_Send_MCU_Buf[6+4+MCU_CNT]=GPS_Inf[MCU_CNT];
            }
        } 
        else
        {//没有GPS定位数据
            for(MCU_CNT=0;MCU_CNT<28;MCU_CNT++)                     
            {
               GSM_Send_MCU_Buf[6+4+MCU_CNT]=0x00                  ;
            } 
        }
        
        
        if(((ARM_DATA_STU&0x01)==0x01)&&(LPM==0x00))  
        {   //MCU 数据有效
            ARM_DATA_STU &= ~0x01;
            GSM_Send_MCU_Buf[38]=0x13; 
            for(MCU_CNT=0;MCU_CNT<62;MCU_CNT++)
            {
               GSM_Send_MCU_Buf[11+28+MCU_CNT]=GSM_MCU[MCU_CNT]; 
            }
            len = 105;             
            tmp=crc_modbus2((unsigned char *)GSM_Send_MCU_Buf,(len-2));
            GSM_Send_MCU_Buf[103]=(char)(tmp&0xFF);
            GSM_Send_MCU_Buf[104]=(char)((tmp>>8)&0xFF);
        }
        else
        {//MCU 数据无效 
            GSM_Send_MCU_Buf[38]=0x00;
            len = 43;            
            tmp=crc_modbus2((unsigned char *)GSM_Send_MCU_Buf,(len-2));
            GSM_Send_MCU_Buf[41]=(char)(tmp&0xFF);
            GSM_Send_MCU_Buf[42]=(char)((tmp>>8)&0xFF);
        }
        GSM_Send_MCU_Buf[105]=0;
        Send_MCU_ERR=1;
        MCU_Data_ACK=1;
        if(GSM_SendData(GSM_Send_MCU_Buf,len))
        {//数据发送
            MCU_ACK_Count  =  0;//重置响应超时
            Send_MCU_ERR   =  0; 
			//LOW_POW_GSM_Flag=0x11;
            return 1;
        }
    }        
 
    if(MCU_Data_ACK && (MCU_ACK_Count>MCU_ACK_TimeOut)
       &&(Heart_Beat_Count<10199)&&(Heart_Beat_Count>7679)
       &&(UDP_Built_flag==0x11))                                   
    { //7.5--10S超时无响应重发
        Send_MCU_ERR=1;
        MCU_ACK_Count  = 0;
        if(GSM_SendData(GSM_Send_MCU_Buf,len))
        { 
            MCU_Data_Count = 0;
            Send_MCU_ERR   = 0;
            MCU_Data_ACK   = 0;
            //LOW_POW_GSM_Flag = 0x11;
            return 1;
        }
        MCU_Data_ACK=0;
    }
    if(Send_MCU_ERR&&(UDP_Built_flag==0x11)
       &&(Heart_Beat_Count<7678)&&(Heart_Beat_Count>5120))
    { //5--7.5S失败重发 
        if(GSM_SendData(GSM_Send_MCU_Buf,len))
        { 
            MCU_ACK_Count  =  0;
            Send_MCU_ERR   =  0;
            //LOW_POW_GSM_Flag=0x11;
            return 1;
        } 
        Send_MCU_ERR       =  0;
    }
    return 0;
}



unsigned long int One_JI_Warn_Cnt=0;//一级报警计数
unsigned long int Two_JI_Warn_Cnt=0;//二级报警计数


unsigned long int One_Degree_Alert_Time=0;//1级报警计数
/**********************************************************************\
*	      函数名：CHECK_MCU_WARN             
*	      作用域：内部文件调用
*	      功能：  检测MCU报警  
*	      参数：  GSM_MCU[1] 水温101度
          格式： 
*	      返回值：AA==有报警；00==无报警   
*
*	      修改历史：（每条详述）MSP430_POWER_ON_FLAG=0x11;//
\**********************************************************************/
char CHECK_MCU_WARN(void)//检测MCU报警 
{
    if((MSP430_POWER_ON_FLAG==0x11)//11=上电；55=断电
      &&(UDP_Built_flag==0x11)&&((ARM_DATA_STU&0x01)==0x01))
    {
        if((One_Degree_Alert_Time!=0)&&(GSM_MCU[10]==0x00)&&(GSM_MCU[11]==0x00)&&(GSM_MCU[12]==0x00)&&((GSM_MCU[13]&0x0F)==0x00)
           &&(One_JI_Warn_Cnt>MSP_A0_Min_1))
           One_Degree_Alert_Time=0;
      
        if((((GSM_MCU[1]>142)||((GSM_MCU[10]&0x03)==0x03))      //水温高报警
            ||(((GSM_MCU[10]&0x0C)==0x0C))                      //机油压力低
            //||(((GSM_MCU[11]&0x0C)==0x0C))                    //发动机机油温度高报警
              ||((GSM_MCU[12]&0x30)==0x30)||((GSM_MCU[12]&0xC0)==0xC0))//主泵压力P1 P2  2014.02.12 修改
              //&&(UDP_B||((GSM_MCU[6]>195)||(GSM_MCU[7]>195))) //主泵压力P1 P2uilt_flag==0x11)&&((ARM_DATA_STU&0x01)==0x01)                      
              &&(One_JI_Warn_Cnt>One_Degree_Alert_Time))                 //一级报警计数
        {
            One_JI_Warn_Cnt=0;
            
            if(One_Degree_Alert_Time>MSP_A0_Min_10)
              One_Degree_Alert_Time   =  0;
                One_Degree_Alert_Time   +=  MSP_A0_Min_1;
              
            MCU_Data_Count=MCU_Data_TimeOut+1;
            Heart_Beat_Count=10200+1; 
            if(GSM_Send_MCU())
              return 1;
            GSM_Send_MCU(); 
        }

          if((((GSM_MCU[11]&0x30)==0x30)            //冷却液液位低报警
                  ||((GSM_MCU[11]&0xC0)==0xC0)      //空滤堵塞报警
                   ||((GSM_MCU[12]&0x03)==0x03)     //液压油油滤堵塞报警
                    ||((GSM_MCU[12]&0x0C)==0x0C))
                    &&(Two_JI_Warn_Cnt>MSP_A0_Min_10)
             )  // 油水分离报警  
        {
            Two_JI_Warn_Cnt=0; //二级报警计数
            
            MCU_Data_Count=MCU_Data_TimeOut+1;
            Heart_Beat_Count=10200+1; 
            if(GSM_Send_MCU())
              return 1;
            GSM_Send_MCU();
        }

    }
    
    return 0;
}

unsigned long int ExitLMP_Time_Cnt=0;//退出低功耗时计数
/**********************************************************************\
*	      函数名：ExitLMP_SendMCU             
*	      作用域：外部文件调用
*	      功能：  当退出低功耗时，发送一次MCU数据  
*	      参数：  1 P2IN&PWRON 
                  2 UDP_Built_flag=0x11
                  3 ExitLMP_Time_Cnt低功耗计时大于3Min
          格式： 
*	      返回值：AA==有报警；00==无报警   
*
*	      修改历史：（每条详述）
\**********************************************************************/
void ExitLMP_SendMCU(void)
{
    if(((P2IN&PWRON)==0)&&(MSP430_POWER_ON_FLAG==0x55)&&(UDP_Built_flag==0x11)
       &&((ARM_DATA_STU&0x01)==0x01)&&(ExitLMP_Time_Cnt>MSP_A0_Min_5))//低功耗发送MCU数据  
    {
        ExitLMP_Time_Cnt=0;
        MCU_Data_Count=MCU_Data_TimeOut+1;
        Heart_Beat_Count=10200+1; 
        
        GPS_Inf[14]=LPM;
        GSM_Send_MCU();
    }
}













