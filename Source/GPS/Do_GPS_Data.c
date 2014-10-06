

#include "GPS.h"

char GPS_Inf[28]                                                    ;//显示及平台GPS数据缓存

unsigned long int GPS_LOCATE_Time  = 0                              ;//GPS接收时间10S

char LOW_POW_GPS_Flag =0x55                                         ;//先使用GPS数据处理完成标志 00==未完成；11==完成
unsigned char GPS_Data_OK_Flag                                     ;//先使用GPS数据处理完成标志 00==未完成；11==完成
char *GPS_Buffer_RMC                                                ;//GPS_RX_Buffer接收数组首地址
char *GPS_Buffer_GGA                                                ;//GPS_RX_Buffer接收数组首地址
char *GPS_Head_STR                                                  ;//￥GPS头文件指针
char *GPGGA_Save_STR                                                ;//GPGGA缓存指针
char *UTC_Time                                                      ;//UTC时间字符指针  
char *GPS_GPRMC_Data_Use                                            ;//GPRMC数据有效标志位指针
char *GPS_GPGGA_Data_Use                                            ;//GPGGA数据有效标志位指针
char *GPS_Latitude                                                  ;//纬度数据指针
char *GPS_North_South                                               ;//纬度北纬南纬数据
char *GPS_Longitude                                                 ;//经度数据指针
char *GPS_West_East                                                 ;//经度北纬南纬数据
char *GPS_Move_Speed                                                ;//移动速度数据单位m/s
char *GPS_Course_Degree                                             ;//方向角0-360
char *UTC_Time_YMD                                                  ;//UTC时间年月日 
char *GPS_Magnetic_Degree                                           ;//地磁偏角0-360
char *GPS_Magnetic_Degree_Dir                                       ;//地磁偏角方向
char *Position_Output_Mode                                          ;//定位模式输出 
char *GPRMC_Check_Sum                                               ;//GPRMC数据校验和
char *GGA_Sates_Used                                                ;//可用星数
char *GGA_MSL_Altitude                                              ;//海拔高度


unsigned long int GPS_J_Data=0                                      ;//经度最终上传信息
unsigned long int GPS_W_Data=0                                      ;//纬度最终上传信息
int GPS_Speed_Data=0                                                ;//运行速度最终上传信息
unsigned int GPS_Course_Degree_Data=0                               ;//方向角0-360
unsigned char GPRMC_Check_Sum_Data=0                                ;//GPRMC校验和
unsigned char GGA_Sates_Used_Data=0                                 ;//可用星数
int  GGA_MSL_Altitude_Data=0                                        ;//海拔高度
unsigned char UTC_Time_Year=0                                       ;//UTC时间年 
unsigned char UTC_Time_Moth=0                                       ;//UTC时间月
unsigned char UTC_Time_dayy=0                                       ;//UTC时间日 
unsigned char UTC_Time_Hour                                         ;//UTC时间小时 
unsigned char UTC_Time_Mint                                         ;//UTC时间分钟  
unsigned char UTC_Time_Secd                                         ;//UTC时间秒 
extern char ARM_DATA_STU                                            ;
extern unsigned char LPM                                            ;//低功耗模式
/*******************************************************************\
*	      函数名：Do_GPS_Data             
*	      作用域：外部文件调用
*	      功能：  接收、处理GPS数据  
*	      参数：  
           1、通过控制GPS数据类型（00=无响应；11=GPRMC；22=GPGGA）控制步骤
           2、Do_GPS_Mesg_OK_Flag：GPS数据处理完成标志 00==未完成；11==完成
           3、Open_UCA1_UART_Init(void)==用接收完成数据不可用时打开GPS中断
*	      返回值：无     
*
*	      修改历史：（每条详述）
\*******************************************************************/
void Do_GPS_Data(void)                                            //接收、处理GPS数据
{
  
  if(GPS_LOCATE_Time>MSP_A0_Min_10)
  {
      GPS_LOCATE_Time=0;
      GPS_GSM_System_Stu[0]|=0x0C;//GPS无信号
      Module_Status[0]|=0x0C;
  }
  
  GPS_ANT_STATUS();//GPS天线检测
  Send_GSM_GPS_AT_Cmd();
  
  switch (Want_GPS_Data_Type) 
  {
     case 0x01:
      {
         if(GPS_LOCATE_Time>10240)                                   //GPS接收时间10S
         {   
             GPS_LOCATE_Time=0                                      ;
             GPS_UCA1_Init()                                        ;//打开GPS接收初始化
             Want_GPS_Data_Type=0x22                                ;
         }
         break;
      }
    
    case 0x11:
      {
        if(GPS_R_OK_End_Flag==0x11)                                  //GPS中断接收成功标志 00==未接收；0x11==GPRMC；0x22==GPGGA；
          {
            GPS_R_OK_End_Flag  = 0x00                               ;//GPS中断接收成功标志 00==未接收；0x11==GPRMC；0x22==GPGGA；
            Do_GPS_RMC_Data()                                       ;//处理GPRMC数据包UTC时间数据处理
          }
         break                                                      ;
      }
    case 0x22:
      {
         if(GPS_R_OK_End_Flag==0x22)                                 //GPS中断接收成功标志 00==未接收；0x11==GPRMC；0x22==GPGGA；
         {
            GPS_R_OK_End_Flag  = 0x00                               ;//GPS中断接收成功标志 00==未接收；0x11==GPRMC；0x22==GPGGA；
            Do_GPS_GGA_Data()                                       ;//处理GPRMC数据包UTC时间数据处理
         }       
         break;
      }
    case 0x33:
      {   
         GSM_SIM_Iint_Sig_Num=GSM_SIM_Signal();
         if((GSM_SIM_Iint_Sig_Num<32))                               //(GSM_SIM_Iint_Sig_Num>0)&&
         {
            VmainMon();//检测主电源电压及锂电池电压   
            Want_GPS_Data_Type=0x11; 
         }
         Delayms(200);//XX*1Ms延时
         break;
      }
    case 0x44:
    { 
         GSM_GPS_AT_Cmd_Cnt=0;//发送GSM获取地理信息计数
         GPS_Data_OK_Flag =0x11                                     ;//先使用GPS数据处理完成标志 00==未完成；11==完成
         ARM_DATA_STU |=0x04                                        ;//GPS信息定位
         Module_Status[0] =0xC0                                     ;//模块状态信息GPS信息+GSM信息+系统状态1、2+升级状态1、2
         GPS_GSM_System_Stu[0]=0xC0                                 ;//GPS状态存储        
         Want_GPS_Data_Type=0x01                                    ;
         LOW_POW_GPS_Flag =0x11                                     ;//先使用GPS数据处理完成标志 00==未完成；11==完成
         break                                                      ;
    }
    default:
        break;
  }    
}

/*******************************************************************\
*	      函数名：Do_GPS_RMC_Data             
*	      作用域：本地文件调用
*	      功能：  处理GPRMC数据包数据
*	      参数： 
*	      返回值：无     
*
*	      修改历史：（每条详述）
\*****************************************************************/
void Do_GPS_RMC_Data(void)                                         //处理GPRMC数据包UTC时间数据处理
{
    GPS_Buffer_RMC =GPS_RX_Buffer                                   ;//GPS_RX_Buffer接收数组首地址
    GPS_Head_STR=GSM_strsep (&GPS_Buffer_RMC, ",")                  ;//1 读取￥数据包头文件指针
    UTC_Time=GSM_strsep (&GPS_Buffer_RMC, ",")                      ;//2 读取UTC时间数据指针
    GPS_GPRMC_Data_Use=GSM_strsep (&GPS_Buffer_RMC, ",")            ;//3 GPRMC数据包有效标志位

      if(*GPS_GPRMC_Data_Use=='A')
      {
         Want_GPS_Data_Type = 0x44                                  ;//需要GPS数据类型00==无响应；
         GPS_Latitude=GSM_strsep (&GPS_Buffer_RMC, ",")             ;//纬度数据指针
         GPS_North_South=GSM_strsep (&GPS_Buffer_RMC, ",")          ;//纬度北纬南纬数据
         GPS_Longitude=GSM_strsep (&GPS_Buffer_RMC, ",")            ;//经度数据指针
         GPS_West_East=GSM_strsep (&GPS_Buffer_RMC, ",")            ;//经度北纬南纬数据
         GPS_Move_Speed=GSM_strsep (&GPS_Buffer_RMC, ",")           ;//移动速度数据单位m/s
         GPS_Course_Degree=GSM_strsep (&GPS_Buffer_RMC, ",")        ;//方向角0-360
         UTC_Time_YMD=GSM_strsep (&GPS_Buffer_RMC, ",")             ;//UTC时间年月日 
         GPS_Magnetic_Degree=GSM_strsep (&GPS_Buffer_RMC, ",")      ;//地磁偏角0-360
         GPS_Magnetic_Degree_Dir=GSM_strsep (&GPS_Buffer_RMC, ",")  ;//地磁偏角方向
         Position_Output_Mode=GSM_strsep (&GPS_Buffer_RMC, "*")     ;//定位模式输出
         GPRMC_Check_Sum =GPS_Buffer_RMC                            ;//GPRMC数据校验和
         GPS_Post_RMC_Mesg()                                        ;//处理GPRMC数据包定位信息数据处理
      }
      else
      {
         Open_UCA0_UART_Init()                                      ;//打开GPS中断
      }	
}

/*******************************************************************\
*	      函数名：Do_GPS_GGA_Data             
*	      作用域：本地文件调用
*	      功能：  处理GPGGA数据包数据
*	      参数： 
*	      返回值：无     
*
*	      修改历史：（每条详述）
\*****************************************************************/
void Do_GPS_GGA_Data(void)                                          //处理GPGGA数据包数据处理
{
    unsigned int GGA_Numr=0                                         ;//GPGGA数据包数据计数
    GPS_Buffer_GGA  =  GPS_RX_Buffer                                ;//GPS_RX_Buffer接收数组首地址
    for(GGA_Numr=0;GGA_Numr<6;GGA_Numr++)        
    {
       GPGGA_Save_STR=GSM_strsep (&GPS_Buffer_GGA, ",")             ;//GPGGA缓存指针
    } 
    GPS_GPGGA_Data_Use=GSM_strsep (&GPS_Buffer_GGA, ",")            ;//GPGGA数据有效标志位指针
    if(*GPS_GPGGA_Data_Use=='1'||*GPS_GPGGA_Data_Use=='6')
      {
        GGA_Sates_Used=GSM_strsep (&GPS_Buffer_GGA, ",")            ;//可用星数
        GPGGA_Save_STR=GSM_strsep (&GPS_Buffer_GGA, ",")            ;//GPGGA缓存指针
        GGA_MSL_Altitude=GSM_strsep (&GPS_Buffer_GGA, ",")          ;//海拔高度
        Want_GPS_Data_Type = 0x33                                   ;//执行下一步
        GPS_Post_GGA_Mesg()                                         ;//处理GPGGA数据包定位信息数据处理
        Open_UCA0_UART_Init()                                       ;//打开GPS中断
      }
    else
    {
      Open_UCA0_UART_Init()                                         ;//打开GPS中断
    }
}


/*******************************************************************\
*	      函数名：GPS_Post_GGA_Mesg             
*	      作用域：本地文件调用
*	      功能：  
*	      参数： 
*	      返回值：无     
*
*	      修改历史：（每条详述）
\*****************************************************************/
void GPS_Post_GGA_Mesg(void)                                       //处理GPGGA数据包定位信息数据处理
{
  unsigned int GGA_Num=0                                           ;//用于经纬度处理计数
  int GPS_Position_OK_MSL_Altitude=0                               ;//海拔高度
 
  GGA_Sates_Used_Data=ASCIITOHEX('0',*(GGA_Sates_Used++))          ;//可用星数
  GGA_Sates_Used_Data=GGA_Sates_Used_Data*10+ASCIITOHEX('0',*(GGA_Sates_Used++));//可用星数
  for(GGA_Num=0;GGA_Num<4;GGA_Num++)        
  {
     if(*(GGA_MSL_Altitude)!='.')
      GPS_Position_OK_MSL_Altitude=GPS_Position_OK_MSL_Altitude*10+ASCIITOHEX('0',*(GGA_MSL_Altitude++));//可用星数
      else {break;}  
  } 
  GGA_MSL_Altitude_Data=GPS_Position_OK_MSL_Altitude               ;//海拔高度
}


/*******************************************************************\
*	      函数名：打开GPS中断             
*	      作用域：本地文件调用
*	      功能：  将GPS数据存入平台数据缓存，及显示缓存
*	      参数：  无
*	      返回值：无   
*
*	      修改历史：（每条详述）
\*****************************************************************/
void Open_UCA0_UART_Init(void)                                     //打开GPS中断
{
    unsigned char Open_UCA1_Num=0                                 ;//用于经纬度处理计数
    for(Open_UCA1_Num=0;Open_UCA1_Num<10;Open_UCA1_Num++)        
    {
      GPS_RX_Buffer[Open_UCA1_Num]   =  '@'                       ;//缓存清零
    }
   GPS_RX_Buffer_Cnt=0                                            ;//GPS数据缓存计数
   GPS_UCA1_Init()                                                ;//GPS中断初始化   
}
/*******************************************************************\
*	      函数名：Do_GPS_RMC_Data             
*	      作用域：本地文件调用
*	      功能：  
*	      参数： 
*	      返回值：无     
*
*	      修改历史：（每条详述）

\*****************************************************************/
void GPS_Post_RMC_Mesg(void)                                      //处理GPRMC数据包定位信息数据处理
{
    unsigned int Nermber=0                                          ;//用于经纬度处理计数
    unsigned long int GPS_Position_OK_gLat=0                        ;//纬度处理临时存放
    unsigned long int GPS_Position_OK_gLong=0                       ;//经度处理临时存放
    unsigned int GPS_Position_OK_Speed=0                            ;//运行速度处理临时存放
    unsigned int GPS_Position_OK_Degree=0                           ;//方向角处理临时存放
    
    UTC_Time_Hour =ASCIITOHEX('0',*(UTC_Time++))*10                 ;//UTC时间小时 
    UTC_Time_Hour =UTC_Time_Hour+ASCIITOHEX('0',*(UTC_Time++))      ; 
    //UTC_Time_TO_Secd=UTC_Time_Hour*3600                             ;
    UTC_Time_Mint =ASCIITOHEX('0',*(UTC_Time++))*10                 ;//UTC时间分钟   
    UTC_Time_Mint =UTC_Time_Mint+ASCIITOHEX('0',*(UTC_Time++))      ;
    //UTC_Time_TO_Secd=UTC_Time_TO_Secd+UTC_Time_Mint*60              ;
    UTC_Time_Secd =ASCIITOHEX('0',*(UTC_Time++))*10                 ;//UTC时间秒   
    UTC_Time_Secd =UTC_Time_Secd+ASCIITOHEX('0',*(UTC_Time++))      ; 
    //UTC_Time_TO_Secd=UTC_Time_TO_Secd+UTC_Time_Secd                 ;
    
    UTC_Time_dayy =ASCIITOHEX('0',*(UTC_Time_YMD++))*10             ;//日期读取
    UTC_Time_dayy =UTC_Time_dayy+ASCIITOHEX('0',*(UTC_Time_YMD++))  ; 
    UTC_Time_Moth =ASCIITOHEX('0',*(UTC_Time_YMD++))*10             ;//月份读取
    UTC_Time_Moth =UTC_Time_Moth+ASCIITOHEX('0',*(UTC_Time_YMD++))  ;  
    UTC_Time_Year =ASCIITOHEX('0',*(UTC_Time_YMD++))*10             ;//年份读取
    UTC_Time_Year =UTC_Time_Year+ASCIITOHEX('0',*(UTC_Time_YMD++))  ; 
    
    
    
    GPS_Position_OK_gLat=GPS_Position_OK_gLat*10+ASCIITOHEX('0',*(GPS_Latitude++));//纬度
    GPS_Position_OK_gLat=GPS_Position_OK_gLat*10+ASCIITOHEX('0',*(GPS_Latitude++));//纬度
    GPS_Position_OK_gLat=GPS_Position_OK_gLat*6                                   ;
    for(Nermber=0;Nermber<5;Nermber++)        
    {
     GPS_Position_OK_gLat=GPS_Position_OK_gLat*10+ASCIITOHEX('0',*(GPS_Latitude++));//纬度
     if(Nermber == 1){GPS_Latitude++;}
    } 
    //if(*GPS_North_South=='S')
    //{
    //     GPS_Position_OK_gLat=0-GPS_Position_OK_gLat;//有符号32位整型变量，>0 表示北纬；0< 表示南纬
    //}  
    GPS_Position_OK_gLong=GPS_Position_OK_gLong*10+ASCIITOHEX('0',*(GPS_Longitude++));//经度 
    GPS_Position_OK_gLong=GPS_Position_OK_gLong*10+ASCIITOHEX('0',*(GPS_Longitude++));//经度 
    GPS_Position_OK_gLong=GPS_Position_OK_gLong*10+ASCIITOHEX('0',*(GPS_Longitude++));//经度 
    GPS_Position_OK_gLong=GPS_Position_OK_gLong*6                                    ;//经度 
    for(Nermber=0;Nermber<5;Nermber++)        
    {
     GPS_Position_OK_gLong=GPS_Position_OK_gLong*10+ASCIITOHEX('0',*(GPS_Longitude++));//经度 
     if(Nermber == 1){GPS_Longitude++;}
    } 
    //if(*GPS_West_East=='W')
    //{
    //    GPS_Position_OK_gLong=0-GPS_Position_OK_gLong                  ;//有符号32位整型变量，>0 表示东经；0< 表示西经
    //}
    for(Nermber=0;Nermber<4;Nermber++)        
    {
     if(*(GPS_Move_Speed)!='.')
      GPS_Position_OK_Speed=GPS_Position_OK_Speed*10+ASCIITOHEX('0',*(GPS_Move_Speed++));//运行速度
      else {break;}  
    } 
  
    for(Nermber=0;Nermber<4;Nermber++)        
    {
     if(*(GPS_Course_Degree)!='.')
      GPS_Position_OK_Degree=GPS_Position_OK_Degree*10+ASCIITOHEX('0',*(GPS_Course_Degree++));//方向角0-360
      else {break;}  
    } 
    
    GPRMC_Check_Sum_Data=ASCIITOHEX('0',*(GPRMC_Check_Sum++))       ;//GPRMC校验和
    GPRMC_Check_Sum_Data=GPRMC_Check_Sum_Data*10+ASCIITOHEX('0',*(GPRMC_Check_Sum++)); 
    
    
    if(*GPS_North_South=='S')                       //2013.09.05 蒋本庆 修改经纬度处理为南纬,西经最高位为1 8000
    {   
        GPS_W_Data=1;
        GPS_W_Data=GPS_W_Data<<31                   ;//有符号32位整型变量，>0 表示北纬；0< 表示南纬
    }
    else
    {
        GPS_W_Data=0;
    }
    
    if(*GPS_West_East=='W')
    {
        GPS_J_Data=1;
        GPS_J_Data=GPS_J_Data<<31                  ;//有符号32位整型变量，>0 表示东经；0< 表示西经
    }
    else
    {
        GPS_J_Data=0;
    }

    GPS_W_Data=GPS_W_Data+GPS_Position_OK_gLat                      ;//纬度最终上传信息
    GPS_J_Data=GPS_J_Data+GPS_Position_OK_gLong                     ;//经度最终上传信息
    GPS_Speed_Data=GPS_Position_OK_Speed                            ;//运行速度最终上传信息
    GPS_Course_Degree_Data=GPS_Position_OK_Degree                   ;//方向角0-360

    VmainMon()                                                      ;//检测主电源电压及锂电池电压
    JG_OUT_EDGE()                                                   ;//判断越界报警 
    Save_GPS_Inf_Buffer()                                           ;//将GPS数据存入缓存
}


/*******************************************************************\
*	      函数名：Save_Net_Disp_Buffer             
*	      作用域：本地文件调用
*	      功能：  将GPS数据存入平台数据缓存，及显示缓存
*	      参数：  无
*	      返回值：无   
*
*	      修改历史：
                   1.蒋本庆 修改GPS工作模式 低功耗 运输 全速模式
\*****************************************************************/
void Save_GPS_Inf_Buffer(void)                                   //将GPS数据存入缓存
{
  char UTC_TIME_BCD                                              ;
  
  UTC_TIME_BCD=UTC_Time_Year/10*16+UTC_Time_Year%10              ;
  GPS_Inf[0]=UTC_TIME_BCD                                        ;//UTC时间年 
  UTC_TIME_BCD=UTC_Time_Moth/10*16+UTC_Time_Moth%10              ;  
  GPS_Inf[1]=UTC_TIME_BCD                                        ;//UTC时间月
  UTC_TIME_BCD=UTC_Time_dayy/10*16+UTC_Time_dayy%10              ;  
  GPS_Inf[2]=UTC_TIME_BCD                                        ;//UTC时间日 
  
  UTC_TIME_BCD=UTC_Time_Hour/10*16+UTC_Time_Hour%10              ;  
  GPS_Inf[3]=UTC_TIME_BCD                                        ;//UTC时间小时
  UTC_TIME_BCD=UTC_Time_Mint/10*16+UTC_Time_Mint%10              ;  
  GPS_Inf[4]=UTC_TIME_BCD                                        ;//UTC时间分钟  
  UTC_TIME_BCD=UTC_Time_Secd/10*16+UTC_Time_Secd%10              ; 
  GPS_Inf[5]=UTC_TIME_BCD                                        ;//UTC时间秒

  GPS_Inf[9]=GPS_W_Data                                          ;//纬度最终上传信息
  GPS_W_Data= GPS_W_Data >>8                                     ;
  GPS_Inf[8]=GPS_W_Data                                          ;
  GPS_W_Data= GPS_W_Data >>8                                     ;  
  GPS_Inf[7]=GPS_W_Data                                          ;
  GPS_W_Data= GPS_W_Data >>8                                     ;
  GPS_Inf[6]=GPS_W_Data                                          ;

  GPS_Inf[13]=GPS_J_Data                                         ;//经度最终上传信息
  GPS_J_Data= GPS_J_Data >>8                                     ;
  GPS_Inf[12]=GPS_J_Data                                         ;
  GPS_J_Data= GPS_J_Data >>8                                     ;
  GPS_Inf[11]=GPS_J_Data                                         ;
  GPS_J_Data= GPS_J_Data >>8                                     ; 
  GPS_Inf[10]=GPS_J_Data                                         ;

  if(GPS_Speed_Data>40)
      GPS_Inf[14]=0x01                                          ;//GPS模式 01==运输模式
  else
  {
	   GPS_Inf[14]=LPM;
  }
  
  
  GPS_Inf[15]=GGA_Sates_Used_Data                                ;//可用星数
  
  //GGA_MSL_Altitude_Data=GGA_MSL_Altitude_Data+5000               ;
  if(GGA_MSL_Altitude_Data>8000)
     GGA_MSL_Altitude_Data=0;
  GPS_Inf[17]=GGA_MSL_Altitude_Data                              ;//海拔高度
  GGA_MSL_Altitude_Data= GGA_MSL_Altitude_Data >>8               ;    
  GPS_Inf[16]=GGA_MSL_Altitude_Data                              ; 
 
 
  GPS_Inf[19]=GPS_Speed_Data                                     ;//移动速度
  GPS_Speed_Data= GPS_Speed_Data >>8                             ;  
  GPS_Inf[18]=GPS_Speed_Data                                     ;
  
  GPS_Inf[21]=GPS_Course_Degree_Data                             ;//方向角
  GPS_Course_Degree_Data= GPS_Course_Degree_Data >>8             ;  
  GPS_Inf[20]=GPS_Course_Degree_Data                             ;
  
  GPS_Inf[22]=0x00                                               ;//预留 未用
  GPS_Inf[23]=GSM_SIM_Signal_Num                                 ;//信号质量强度
  
  
  GPS_Inf[25]=0                                         ;
  GPS_Inf[24]=BAT_V_main                                         ;//电池电压
  
  GPS_Inf[27]=0                                         ;
  GPS_Inf[26]=LI_BAT_VOL                                         ;//锂电池电压
}














