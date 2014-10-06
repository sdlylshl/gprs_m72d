#ifndef __GSM_H__
#define __GSM_H__


#define LPM3_Open_Send_Time    5
#define  PWRON BIT0


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


#define MSP_Hour_10  36864000
#define MSP_Hour_20  73728000
#define MSP_Hour_30  110592000  

#define GPS_Model_Bug  0x03   //11 GPS【模块故障】提示；
#define GPS_No_Signal  0x0C   //11 GPS无信号；
#define GPS_Ant_Open   0x30   //11 GPS【天线剪线/未接】；
#define GPS_Position   0xC0   //11 GPS定位标志；
/*******************************************************************\
*	      文件名：GSM_Brust.c      //突发数据处理
\*******************************************************************/
#define Receive_Gprs_Data    0x01
#define GSM_Get_GPS    0x03

#define  NET_Up_Speed_BUF_SIZE 10
extern char REC_Up_Speed_BUF[NET_Up_Speed_BUF_SIZE];//接收允许换卡 无卡升速
extern char LPM3_Open_Box_Gsm_Flag;

extern char* Low_Pow_Time_Addr   ;//低功耗时间间隔设置标志地址



extern unsigned char GPS_Vmain_Bat;//蓄电池电压 电池
extern unsigned char LPM                                       ;//低功耗模式
extern unsigned char GSM_STATUS                                         ;
extern unsigned int M72D_RX_Buf_Cnt                                     ;                             
extern unsigned char packet_type                                        ;
#define PACKET_BRUST 0x00
#define M72D_Brust_RX_BUF_SIZE 80                         
extern unsigned char M72D_Brust_RX_Buf[M72D_Brust_RX_BUF_SIZE]          ;
extern unsigned char M72D_Brust_STATUS                                  ; 
#define PACKET_Query 0x01
#define M72D_Query_RX_BUF_SIZE 50                                       //数据传输最大字节数
extern unsigned char M72D_Query_RX_Buf[M72D_Query_RX_BUF_SIZE]          ;
extern unsigned char M72D_Query_STATUS                                  ;//配置包接收完成 0 无响应 0x01 OK 0x02 ERRO 0x04 数据溢出
#define PACKET_CONFIG 0x02
#define M72D_SetConfig_RX_BUF_SIZE 50                         
extern unsigned char M72D_SetConfig_RX_Buf[M72D_SetConfig_RX_BUF_SIZE]  ;
extern unsigned char M72D_SetConfig_STATUS                              ; 
#define PAKET_DATA 0x04
#define M72D_ServerData_RX_BUF_SIZE 1200                         
extern unsigned char M72D_ServerData_RX_Buf[M72D_ServerData_RX_BUF_SIZE];
extern unsigned char M72D_ServerData_STATUS                             ; 
#define PACKET_NETCONFIG 0x08
#define M72D_NetConfig_RX_BUF_SIZE 100                        
extern unsigned char M72D_NetConfig_RX_Buf[M72D_NetConfig_RX_BUF_SIZE]  ;
extern unsigned char M72D_NetConfig_STATUS                              ;//网络包接收完成 0 无响应 0x01 OK 0x02 ERRO 0x04 数据溢出
#define PCKET_OTHER 0x10
#define M72D_Other_RX_BUF_SIZE 100                         
extern unsigned char M72D_Other_RX_Buf[M72D_Other_RX_BUF_SIZE]          ;
extern unsigned char M72D_Other_STATUS                                  ; 


#define OUT_EDGE_BUF_SZ 25                                               //越界报警命令设置缓存大小
extern char NET_OUT_EDGE_BUF[OUT_EDGE_BUF_SZ]                           ;//平台下发越界报警命令
#define  MCU_TIME_BUF_SZ 12
extern char REC_MCU_TIME_BUF[MCU_TIME_BUF_SZ]                           ;//接收MCU数据上传时间间隔设置
#define  WORK_MODE_BUF_SZ 29
extern char REC_WORK_MODE_BUF[WORK_MODE_BUF_SZ]                         ;//接收MCU数据上传时间间隔设置
#define  NET_IP_PORT_BUF_SIZE 14
extern char REC_IP_PORT_BUF[NET_IP_PORT_BUF_SIZE]                       ;//接收IP端口号
#define  NET_LOOK_M_BUF_SIZE 10
extern char REC_LOOK_M_BUF[NET_LOOK_M_BUF_SIZE]                         ;//接收 GPS终端锁车/解锁命令
#define  CALL_MCU_BUF_SIZE 12
extern char  REC_CALL_MCU_BUF[CALL_MCU_BUF_SIZE]                        ;//接收MCU数据上传时间间隔设置

extern char UDP_Built_STR[41]                                           ;//山重建机GPS管理平台端口
extern char GSM_MCU[62]                                                 ;//显示器MCU数据缓存
extern char GPS_Inf[28]                                                 ;//显示及平台GPS数据缓存
extern char GSM_Send_MCU_Buf[106]                                        ;//上报MCU数据命令(回复)

extern char UP_Root_Stat_BuF[29]                                        ;//存放网关发送升级启动指令
extern char Module_Status[6]                                            ;//模块状态信息GPS信息+GSM信息+系统状态1、2+升级状态1、2
extern char GPS_GSM_System_Stu[4]                                       ;//一体机状态存储
extern char GPS_GSM_ID_Memory[4]                                        ;//一体机设备ID终端号
#define UP_SYSTEM_C_SIZE 1200
extern unsigned char UP_SYSTEM_C_LANG[UP_SYSTEM_C_SIZE]                         ;//升级包数据
#define  GSM_GSP_BUF_SIZE 40
extern char  GSM_GSP_BUF[GSM_GSP_BUF_SIZE];//GSM接收经纬度时间信息缓存



//****************************************************************************//
extern int GSM_strstr (const char * Main_str,const char * search_str)   ;//GSM_String//字符串查找
extern int GSM_strlen (const char * str)                                ;//字符串长度
extern int GSM_strcmp (const char * src,const char * dst)               ;//字符串比较
extern char * GSM_strsep (char **stringp, const char *delim)            ;//字符串分割
extern void GSM_ltoa(char offset, char*ps,unsigned long src)            ;//数字转字符串
extern long GSM_atol( const char * str )                                ;//字符串转数字
extern unsigned char GSM_SendCMD(char paktype, const char * s,
                                 unsigned int Len,int ms)               ;//GSM_CMD
extern char GSM_SendData(const char * Data,int num)                     ;
extern unsigned char ASCIITOHEX( unsigned char Input_H,
                                unsigned char Input_L );//将ASCII字符转换为16进制的数
extern unsigned int crc_modbus2(unsigned char *puchMsg, 
                                unsigned int usDataLen)                 ;//CRC
extern char Compare_String(char *Source,char *Target,unsigned int Count);//比较两个字符串是否相同
extern void Flash_WriteChar(unsigned int Addr,unsigned char Data)       ;//向Flash中写入一个字节
extern void Tran_ID_CmdNum(char* ID_Addr,char CmdNum)                   ;//转存ID号和命令编码                              
extern void Delayms(unsigned long int ms)                               ;//XX*1Ms延时
extern unsigned char HEX_TO_ASCII(char One_Char)                        ;//16进制字符转换ASCII
extern void Tran_ID_CmdNum(char* ID_Addr,char CmdNum)                   ;//转存ID号和命令编码                              
extern char SAVE_SIM_NUM_FH(unsigned char CMD_LEN)                      ;//存储手机卡号 
extern void SIM_CHG_WARN(void)                        ;//更换手机卡报警 
extern void Do_GSM_GPS_Data(void);//处理GSM接收GPS数据

extern char GSM_Lock_Monitor()                                          ;//0x03==锁车；0x00解锁 
extern char IP_Port_Reset()                                             ;//处理IP地址\Port号  
extern char Work_Mode_Set()                                             ;//系统运行工作模式参数
extern char Root_UP_Net_Begin(void)                                     ;//网关发送升级启动指令
extern char TCPIP_BASIC_SET(void)                                       ;
extern char SIM_NUM_CHECK(void)                                         ;
extern char GSM_CREG_Init()                                             ;
extern char GSM_GPRS_Init()                                             ;
extern char GSM_TCPIP_Init()                                            ;
extern void GSM_NOM_POW_ON(void)                                            ;//GSM_State
extern void GSM_Sleep()                                                 ;
extern void GSM_Stop()                                                  ;
extern void GSM_State()                                                 ;
extern void DO_ROOT_UP(void)                                            ;//处理远程升级程序
extern char UP_A2_NUM_CRC(void)                                         ;//升级程序CRC校验
extern void GSM()                                                       ;
extern char GSM_Heart_Beat()                                            ;//GSM_HEART_BEAT.C
extern char GSM_Send_MCU(void)                                              ;//GSM_Send_MCU_Data.c
extern void RecData()                                                   ;
extern char MCU_Time_Set()                                              ;//MCU数据上传时间间隔设置 
extern char Call_MCU_Data(void)                                         ;//呼叫GPS终端上报定位数据命令
extern void SHUT_GPS_UCA1(void)                                         ;//关闭GPS中断
extern void Init_UCA0UART(void)                                         ;//GPS中断初始化
extern void RE_START_GSM(void)                                          ;//重启系统
extern char CONNECT_FAIL_RESET(void)                                    ;//处理Conect_Fail链接
extern char OUT_EDGE_SET(void)                                          ;//越界报警命令设置
extern char ANSW_CALL_MCU(void)                                         ;//回复上报定位数据命令
extern void RD_MCU_TIME_FLASH(void)                                     ;//读取时间设置FLASH
extern void GSM_NOM_POW_OFF(void)                                       ;
extern void SHUT_GSM_UCA0(void)                                         ;
extern void GPS_Stop()                                                  ;
extern char GSM_Brust()                                                     ;//突发数据处理
extern void Open_Box_Check(void);
extern void Write_Open_Box_Flash(void);//开盒检测报警Flash
extern void Send_SIM_Card_NUM(void);//开机发送手机卡号
extern void Change_SIM_Card_NUM(void);//允许更换手机卡号
extern void Send_GSM_GPS_AT_Cmd(void);
extern void VmainMon(void);//检测主电源电压及锂电池电压   
extern unsigned char GSM_SIM_Signal();//信号质量强度
extern void ExitLMP_SendMCU(void);//退出低功耗发送MCU数据 
extern char CHECK_MCU_WARN(void);//检测MCU报警 
extern char Allow_Up_Speed(void);//处理允许换卡 无卡升速
extern void READ_UP_SPI_FLASH(void); //转移软件版本号及升级类型


extern unsigned int RD_UCA0_Time                                             ;
//extern void GSM_Value_Init(void);


extern unsigned long int Soft_Vers_Cnt                                  ;//发送软件版本号时间计时
extern unsigned long int RD_Data_1_Min_Cnt                              ;//解析 MUC 应答时间计数
extern unsigned long int HeartBeat_TimeOut                              ;//配置心跳包发送时间  61425
extern unsigned long int MCU_Data_TimeOut                               ;//配置数据包发送时间   
extern unsigned long int MCU_ACK_TimeOut                                ;//1min 查询应答包 无应答则重发
extern unsigned long int MCU_TimeOut                                    ;//数据包发送时间 
extern unsigned long int Heart_Beat_Send_Cnt                          ;//心跳发送次数计数
extern unsigned long int THR_Mint_Time_Cnt                              ;//3.5分钟时间计数，用于不上传数据检测
extern unsigned int  GSM_TimeOut_1ms                                    ;//Timer
extern unsigned long int Heart_Beat_Count                               ;//发送心跳1ms时间计数
extern unsigned long int MCU_Data_Count                                 ;//1ms 数据包计数
extern unsigned long int CALL_MCU_TIME_CNT                              ;//时间计数
extern unsigned long int GSM_INIT_TIME_CNT                              ;//GSM初始化计时用
extern unsigned int SIM_CARD_TIME_OUT                              ;//SIM卡初始化计时用
extern unsigned long int RD_Data_Time                                   ;//解析 MUC 应答时间计数
extern unsigned long int RD_Data_1_Min_Cnt                                        ;//解析 MUC 应答时间计数
extern unsigned long int SIM_Card_Work_Cnt;//发发送手机卡号时间计时
extern unsigned long int No_SIM_CARD_Cnt;//检测不到SIM卡初始化计数
extern unsigned long GSM_GPS_AT_Cmd_Cnt;//发送GSM获取地理信息计数
extern unsigned long int ExitLMP_Time_Cnt;//退出低功耗时计数
extern unsigned long int One_JI_Warn_Cnt;//一级报警计数
extern unsigned long int Two_JI_Warn_Cnt;//二级报警计数
extern unsigned long int One_Degree_Alert_Time;//1级报警计数


extern char MSP430_POWER_ON_FLAG;//11=上电；55=断电
extern unsigned char GPS_Data_OK_Flag                                     ;//先使用GPS数据处理完成标志 00==未完成；11==完成
extern unsigned char Soft_VER_OK                                        ;//软件版本号(显示器，控制器，油门电机软件版本号)
extern unsigned char UP_SPI_Num_Flag                                           ;//11==ARM索要数据(包数索引号,每包字节数)
extern unsigned char CONNECT_FAIL_Flag                                  ;//CONNECT FAIL允许使能
extern unsigned char POWER_ON_FLAG                                      ;//11=关机；55=开机
extern unsigned char UDP_Built_flag                                     ;//11==网络建立，00==网络断开或者没有建立
extern unsigned char UDP_Built_flag                                     ;//11==网络建立，00==网络断开或者没有建立
extern unsigned char GSM_ECHO                                           ;//配置打开回显 0x80
extern char GSM_SIM_Iint_Sig_Num                                    ;



extern unsigned int  BAT_V_main                               ;//蓄电池电压 电池
extern unsigned int  LI_BAT_VOL                               ;//锂电池电压 电池


extern char SIM_Card_ER_Flag;//发送手机卡号失败标志 重发
extern char SD_SOFT_V_ER_Flag                                           ;//发送软件版本号失败标志 重发
extern char ARM_DATA_STU                                                ;//数据有效标志
extern char MCU_Data_ACK                                                ;//请求发送数据标志位  
extern char LOW_POW_GSM_Flag                                            ;//先使用GPS数据处理完成标志 00==未完成；11==完成
extern char JG_OUT_EDGE_Flag                                            ;//越界报警判断标志位 11=可以判断
extern char Sys_NOW_UP_Flag;//系统正在升级标志 11==在升级 55 未升级
extern char Open_Box_Flag;
extern char SIM_Card_Work_Flag;//上电手机卡号标志
extern char Change_SIM_Card_Flag;//上电更换手机卡号标志
extern void Read_No_Chg_Card_Flash(void);//读换卡无卡Flash 
extern void Write_No_Chg_Card_Flash(void);//写换卡无卡Flash 
extern void RD_Up_Speed_FLASH(void);//处理允许换卡 无卡升速
extern char Send_Soft_Version(void)                                 ;//发送软件版本号

extern unsigned int CREG_Zhu_Ce_Cnt ;//注册网络查询注册次数


/**********************************************************************\
    extern void Test_IP_Port(void);测试IP地址\Port号  
\**********************************************************************/
extern void Ceshi_Allow_Up_Speed(void);//测试处理允许换卡 无卡升速

#endif