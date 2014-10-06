
#include "msp430x54x.h"
#include "GSM.h"

#define  GSM_GSP_BUF_SIZE 40
char  GSM_GSP_BUF[GSM_GSP_BUF_SIZE];//GSM接收经纬度时间信息缓存
char * Get_GSM_GSP_AT = "AT+QGSMLOC";
unsigned long GSM_GPS_AT_Cmd_Cnt=0;//发送GSM获取地理信息计数

//AT+QGSMLOC 获取经纬度和时间 6s
//+QGSMLOC: 0,118.441844,34.955625,2013/06/19,02:30:47
//AT+QGSMLOC=1
//+QGSMLOC: 0,118.441844,34.955625,2013/06/19,02:30:55
//OK

/*******************************************************************\
*	      函数名：Send_GSM_GPS_AT_Cmd             
*	      作用域：外部文件调用
*	      功能：  查询经纬度坐标 
*	      参数：  
          格式： 
*	      返回值：  GPS_GSM_System_Stu[0]|=0x0C;//GPS无信号
      Module_Status[0]|=0x0C;
*
*	      修改历史：（每条详述）
\*******************************************************************/
void Send_GSM_GPS_AT_Cmd(void)
{
    if((MSP430_POWER_ON_FLAG==0x11)&&(UDP_Built_flag==0x11)
	   &&((GPS_GSM_System_Stu[0]&GPS_Model_Bug)  //GPS【模块故障】
          ||(GPS_GSM_System_Stu[0]&GPS_No_Signal)//GPS无信号；
          ||(GPS_GSM_System_Stu[0]&GPS_Ant_Open) //GPS【天线剪线/未接】；
          ||((GPS_GSM_System_Stu[0]&GPS_Position)==0x00)//GPS定位标志；
            )&&(GSM_GPS_AT_Cmd_Cnt>SECD_30))
    {
        GSM_GPS_AT_Cmd_Cnt=0;//发送GSM获取地理信息计数
        GSM_SendCMD(PACKET_BRUST, Get_GSM_GSP_AT,0, 10);
    }
}

struct Gsmloc_Time
{
  unsigned char GSM_YEAR;
  unsigned char GSM_Moth;
  unsigned char GSM_dayy;
  unsigned char GSM_Hour;
  unsigned char GSM_Mint;
  unsigned char GSM_Secd;
}LOC;

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
void Do_GSM_GPS_Data(void)//处理GSM接收GPS数据
{

  
    //unsigned char GPS_COUNT;
    unsigned long int GSM_LOC_Weidu=0;//纬度临时存放
    unsigned long int GSM_LOC_Jingdu=0;//经度临时存放
    
    unsigned  long int LOC_Wei_Miao=0;//纬度 秒
    unsigned  long int LOC_Jig_Miao=0;//经度 秒 
    
    unsigned  int LOC_Wei_DU=0;//纬度 分  
    unsigned  int LOC_Jig_DU=0;//经度 分 
    


    LOC_Jig_DU=ASCIITOHEX('0',GSM_GSP_BUF[0]);//经度  7084417
    LOC_Jig_DU=LOC_Jig_DU*10+ASCIITOHEX('0',GSM_GSP_BUF[1]);  
    LOC_Jig_DU=LOC_Jig_DU*10+ASCIITOHEX('0',GSM_GSP_BUF[2]);  
    LOC_Jig_DU=LOC_Jig_DU*6;//*6}
    
    LOC_Jig_Miao=ASCIITOHEX('0',GSM_GSP_BUF[4]);
    LOC_Jig_Miao=LOC_Jig_Miao*10+ASCIITOHEX('0',GSM_GSP_BUF[5]);  
    LOC_Jig_Miao=LOC_Jig_Miao*10+ASCIITOHEX('0',GSM_GSP_BUF[6]); 
    LOC_Jig_Miao=LOC_Jig_Miao*10+ASCIITOHEX('0',GSM_GSP_BUF[7]); 
    LOC_Jig_Miao=LOC_Jig_Miao*10+ASCIITOHEX('0',GSM_GSP_BUF[8]); 
    LOC_Jig_Miao=LOC_Jig_Miao*6/10;
    
    GSM_LOC_Jingdu=LOC_Jig_DU*100000+LOC_Jig_Miao;
    
    
    LOC_Wei_DU=ASCIITOHEX('0',GSM_GSP_BUF[11]);//纬度 204 955439
    LOC_Wei_DU=LOC_Wei_DU*10+ASCIITOHEX('0',GSM_GSP_BUF[12]); 
    LOC_Wei_DU=LOC_Wei_DU*6;
    
    LOC_Wei_Miao=LOC_Wei_Miao*10+ASCIITOHEX('0',GSM_GSP_BUF[14]);  
    LOC_Wei_Miao=LOC_Wei_Miao*10+ASCIITOHEX('0',GSM_GSP_BUF[15]);  
    LOC_Wei_Miao=LOC_Wei_Miao*10+ASCIITOHEX('0',GSM_GSP_BUF[16]);  
    LOC_Wei_Miao=LOC_Wei_Miao*10+ASCIITOHEX('0',GSM_GSP_BUF[17]);  
    LOC_Wei_Miao=LOC_Wei_Miao*10+ASCIITOHEX('0',GSM_GSP_BUF[18]);  
    LOC_Wei_Miao=LOC_Wei_Miao*6/10;
    
    GSM_LOC_Weidu=LOC_Wei_DU*100000+LOC_Wei_Miao;

    
    
    if(GSM_GSP_BUF[21]=='2'&&GSM_GSP_BUF[22]=='0')
    {
        LOC.GSM_YEAR=ASCIITOHEX('0',GSM_GSP_BUF[23]);//年 
        LOC.GSM_YEAR=LOC.GSM_YEAR*10+ASCIITOHEX('0',GSM_GSP_BUF[24]);  
        
        LOC.GSM_Moth=ASCIITOHEX('0',GSM_GSP_BUF[26]);//月 
        LOC.GSM_Moth=LOC.GSM_Moth*10+ASCIITOHEX('0',GSM_GSP_BUF[27]);  
        
        LOC.GSM_dayy=ASCIITOHEX('0',GSM_GSP_BUF[29]);//日 
        LOC.GSM_dayy=LOC.GSM_dayy*10+ASCIITOHEX('0',GSM_GSP_BUF[30]);  
        
        LOC.GSM_Hour=ASCIITOHEX('0',GSM_GSP_BUF[32]);//时 
        LOC.GSM_Hour=LOC.GSM_Hour*10+ASCIITOHEX('0',GSM_GSP_BUF[33]);  
        
        LOC.GSM_Mint=ASCIITOHEX('0',GSM_GSP_BUF[35]);//分 
        LOC.GSM_Mint=LOC.GSM_Mint*10+ASCIITOHEX('0',GSM_GSP_BUF[36]);  
        
        LOC.GSM_Secd=ASCIITOHEX('0',GSM_GSP_BUF[38]);//秒 
        LOC.GSM_Secd=LOC.GSM_Secd*10+ASCIITOHEX('0',GSM_GSP_BUF[39]);  
    }
    
    VmainMon();//检测电瓶及锂电池电压
    GSM_SIM_Iint_Sig_Num=GSM_SIM_Signal();
    
    GPS_Inf[0]=LOC.GSM_YEAR/10*16+LOC.GSM_YEAR%10              ;//UTC时间年
    GPS_Inf[1]=LOC.GSM_Moth/10*16+LOC.GSM_Moth%10              ;//UTC时间月
    GPS_Inf[2]=LOC.GSM_dayy/10*16+LOC.GSM_dayy%10              ;//UTC时间日  
    GPS_Inf[3]=LOC.GSM_Hour/10*16+LOC.GSM_Hour%10              ;//UTC时间小时  
    GPS_Inf[4]=LOC.GSM_Mint/10*16+LOC.GSM_Mint%10              ;//UTC时间分钟  
    //LOC.GSM_Secd=LOC.GSM_Secd+10;
    GPS_Inf[5]=LOC.GSM_Secd/10*16+LOC.GSM_Secd%10              ;//UTC时间秒     
    
    
      GPS_Inf[9]=GSM_LOC_Weidu ;//纬度最终上传信息
      GSM_LOC_Weidu= GSM_LOC_Weidu >>8;
      GPS_Inf[8]=GSM_LOC_Weidu;
      GSM_LOC_Weidu= GSM_LOC_Weidu >>8;  
      GPS_Inf[7]=GSM_LOC_Weidu;
      GSM_LOC_Weidu= GSM_LOC_Weidu >>8;
      GPS_Inf[6]=GSM_LOC_Weidu;
    
      GPS_Inf[13]=GSM_LOC_Jingdu;//经度最终上传信息
      GSM_LOC_Jingdu= GSM_LOC_Jingdu >>8;
      GPS_Inf[12]=GSM_LOC_Jingdu;
      GSM_LOC_Jingdu= GSM_LOC_Jingdu >>8;
      GPS_Inf[11]=GSM_LOC_Jingdu;
      GSM_LOC_Jingdu= GSM_LOC_Jingdu >>8; 
      GPS_Inf[10]=GSM_LOC_Jingdu;
      GPS_Inf[14]=LPM;
      GPS_Inf[15]=0   ;//可用星数

      GPS_Inf[17]=0;//海拔高度
      GPS_Inf[16]=0;

      GPS_Inf[19]=0                                     ;//移动速度
      GPS_Inf[18]=0                                     ;
  
      GPS_Inf[21]=0                             ;//方向角
      GPS_Inf[20]=0                             ;
      
      
      GPS_Inf[22]=0x00 ;//预留 未用
      GPS_Inf[23]=GSM_SIM_Iint_Sig_Num;//信号质量强度
      
      GPS_Inf[25]=0;
      GPS_Inf[24]=BAT_V_main;//电池电压
      
      GPS_Inf[27]=0;
      GPS_Inf[26]=LI_BAT_VOL;//锂电池电压
      
  
     GPS_Data_OK_Flag =0x11;
     ARM_DATA_STU |=0x04   ;//GPS信息定位
     Module_Status[0]=0xC0;//模块状态信息GPS信息+GSM信息+系统状态1、2+升级状态1、2
     GPS_GSM_System_Stu[0]=0xC0;//GPS状态存储    
}















