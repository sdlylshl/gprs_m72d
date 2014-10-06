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



extern unsigned char GSM_SIM_Signal_Num                       ;//信号质量强度
extern unsigned int  BAT_V_main                               ;//蓄电池电压 电池
extern unsigned int  LI_BAT_VOL                               ;//锂电池电压 电池


extern void VmainMon(void)                                    ;//检测主电源电压及锂电池电压
extern char GPS_GSM_System_Stu[4]                             ;//一体机状态存储
extern char Module_Status[6]                                  ;//模块状态信息GPS信息+GSM信息+系统状态1、2+升级状态1、2
extern char * GSM_strsep (char **stringp, const char *delim)  ;//字符串分割
extern char  GPS_RX_Buffer[GPS_RX_Buffer_Size]                ;//GPS数据缓存100
extern unsigned int GPS_RX_Buffer_Cnt                         ;//GPS数据缓存计数
extern void GPS_UCA1_Init(void)                               ;//GPS中断初始化
extern unsigned char Want_GPS_Data_Type                       ;//需要GPS数据类型00==无响应；0x11==GPRMC；0x22==GPGGA； 
extern unsigned char GPS_R_OK_End_Flag                        ;//GPS中断接收成功标志 00==未接收；0x11==GPRMC；0x22==GPGGA；
extern char GSM_SIM_Signal()                                  ;//信号质量强度
extern void JG_OUT_EDGE(void)                                 ;//判断越界报警 
extern void VmainMon(void)                                    ;//检测主电源电压及锂电池电压   




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



extern unsigned char GSM_SIM_Signal_Num                       ;//信号质量强度
extern unsigned int  BAT_V_main                               ;//蓄电池电压 电池
extern unsigned int  LI_BAT_VOL                               ;//锂电池电压 电池


extern void VmainMon(void)                                    ;//检测主电源电压及锂电池电压
extern char GPS_GSM_System_Stu[4]                             ;//一体机状态存储
extern char Module_Status[6]                                  ;//模块状态信息GPS信息+GSM信息+系统状态1、2+升级状态1、2
extern char * GSM_strsep (char **stringp, const char *delim)  ;//字符串分割
extern char  GPS_RX_Buffer[GPS_RX_Buffer_Size]                ;//GPS数据缓存100
extern unsigned int GPS_RX_Buffer_Cnt                         ;//GPS数据缓存计数
extern void GPS_UCA1_Init(void)                               ;//GPS中断初始化
extern unsigned char Want_GPS_Data_Type                       ;//需要GPS数据类型00==无响应；0x11==GPRMC；0x22==GPGGA； 
extern unsigned char GPS_R_OK_End_Flag                        ;//GPS中断接收成功标志 00==未接收；0x11==GPRMC；0x22==GPGGA；
extern char GSM_SIM_Signal()                                  ;//信号质量强度
extern void JG_OUT_EDGE(void)                                 ;//判断越界报警 
extern void VmainMon(void)                                    ;//检测主电源电压及锂电池电压   














