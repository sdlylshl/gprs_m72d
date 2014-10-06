
#ifndef __GPS_H__
#define __GPS_H__














#define SECD_1    1024
#define SECD_2    2048
#define SECD_3    3072
#define SECD_5    5120
#define SECD_10   10240
#define SECD_20   20480
#define SECD_30   30720
#define SECD_50   51200

#define MSP_A0_Min_1    61440
#define MSP_A0_Min_2    122880
#define MSP_A0_Min_3    184320
#define MSP_A0_Min_5    307200
#define MSP_A0_Min_10   614400
#define MSP_A0_Min_20   1228800
#define MSP_A0_Min_30   1843200


#define MSP_Hour_1   3686400
#define MSP_Hour_2   7372800
#define MSP_Hour_3   11059200


#define GPS_ANT BIT7
#define GPS_ANT_IO_Init()  P9SEL &=  ~GPS_ANT;\
                           P9DIR &=  ~GPS_ANT;\
                           P9REN &=  ~GPS_ANT;

extern unsigned long int GPS_LOCATE_Time                            ;//GPS接收时间10S


extern void GPS_Start()                                             ;
extern void GPS_Stop()                                              ;
extern void Close_UCA1UART(void)                                    ;
extern void Delayms(unsigned long int ms)                           ;//XX*1Ms延时

#define GPS_RX_Buffer_Size 100

void Do_GPS_Data(void)                                        ;//分组GPS处理数据
void Do_GPS_RMC_Data(void)                                    ;//处理GPRMC数据包UTC时间数据处理
void Do_GPS_GGA_Data(void)                                    ;//处理GPGGA数据包数据处理
void Open_UCA0_UART_Init(void)                                ;//打开GPS中断
void GPS_Post_GGA_Mesg(void)                                  ;//处理GPGGA数据包定位信息数据处理
void GPS_Post_RMC_Mesg(void)                                  ;//处理GPRMC数据包定位信息数据处理
void Save_GPS_Inf_Buffer(void)                                ;//将GPS数据存入缓存
unsigned char ASCIITOHEX( unsigned char Input_H,
                         unsigned char Input_L )              ;//将ASCII字符转换为16进制的数
extern void SHUT_GPS_UCA1(void)                              ;//关闭GPS中断


extern char GSM_SIM_Iint_Sig_Num;
extern unsigned char GSM_SIM_Signal_Num                       ;//信号质量强度
extern unsigned int  BAT_V_main                               ;//蓄电池电压 电池
extern unsigned int  LI_BAT_VOL                               ;//锂电池电压 电池
extern unsigned long GSM_GPS_AT_Cmd_Cnt;//发送GSM获取地理信息计数

extern void VmainMon(void)                                    ;//检测主电源电压及锂电池电压
extern char GPS_GSM_System_Stu[4]                             ;//一体机状态存储
extern char Module_Status[6]                                  ;//模块状态信息GPS信息+GSM信息+系统状态1、2+升级状态1、2
extern char * GSM_strsep (char **stringp, const char *delim)  ;//字符串分割
extern char  GPS_RX_Buffer[GPS_RX_Buffer_Size]                ;//GPS数据缓存100
extern unsigned int GPS_RX_Buffer_Cnt                         ;//GPS数据缓存计数
extern void GPS_UCA1_Init(void)                               ;//GPS中断初始化
extern unsigned char Want_GPS_Data_Type                       ;//需要GPS数据类型00==无响应；0x11==GPRMC；0x22==GPGGA； 
extern unsigned char GPS_R_OK_End_Flag                        ;//GPS中断接收成功标志 00==未接收；0x11==GPRMC；0x22==GPGGA；
extern unsigned char GSM_SIM_Signal()                                  ;//信号质量强度
extern void JG_OUT_EDGE(void)                                 ;//判断越界报警 
extern void VmainMon(void)                                    ;//检测主电源电压及锂电池电压   
extern void GPS_ANT_STATUS(void)                              ;//GPS天线检测
extern void Send_GSM_GPS_AT_Cmd(void);








#endif