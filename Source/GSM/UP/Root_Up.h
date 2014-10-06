
#include "msp430x54x.h"

#define SECD_1 1024
#define SECD_2 2048
#define SECD_5 5120


//0x03==升级显示器;0x0C==升级控制器;0x30==升级油门控制器;0xC0==升级GPS,GSM系统类型
//unsigned long int UP_All_Bit_Len                                    ;//升级程序文件总字节数
//char UP_Soft_Flag                                                   ;//软件版本升级判断标志位 
//0x11==软件版本升级;0x00==不软件版本升级;0xXX==未知不处理
extern unsigned char UDP_Built_flag                                            ;//11==网络建立，00==网络断开或者没有建立
extern unsigned char UP_STR_A10_Type                                           ;//升级控制系统类型 
extern unsigned char UP_SPI_Num_Flag                                           ;//11==ARM索要数据(包数索引号,每包字节数)
extern unsigned char Ask_index;
extern unsigned int Index;
extern unsigned long int UP_SPI_Sum_Len                                        ;//累计已接收文件字节数
extern unsigned long int Heart_Beat_Send_Cnt                          ;//心跳发送次数计数
extern unsigned long int UP_SPI_NUM_CNT                                       ;//远程升级计数
extern unsigned long int UP_Send_Times                                        ;//远程升级发送次数计数
extern unsigned long int RD_Data_Time                                   ;//解析 MUC 应答时间计数



extern unsigned int RD_UCA0_Time                                             ;




extern char Sys_NOW_UP_Flag;//系统正在升级标志 11==在升级 55 未升级


extern char Module_Status[6]                                                   ;//模块状态信息GPS信息+GSM信息+系统状态1、2+升级状态1、2
extern char UP_Root_Stat_BuF[29]                                               ;//存放网关发送升级启动指令
extern unsigned char UP_Stat_SPI_BuF[18]                                       ;//存放网关发送升级启动指令
extern unsigned char Soft_VER_memory[32]                                                ;//存储软件版本号
extern char Module_Status[6]                                                   ;//模块状态信息GPS信息+GSM信息+系统状态1、2+升级状态1、2
extern char ANS_Up_Stat_Buf[20]                                                ;//回复平台升级启动指令数组
extern char GPS_GSM_ID_Memory[4]                                               ;//一体机设备ID终端号  {0X38,0X1C,0X06,0X00}
//extern char   UP_SPI_Num_BuF[4]                                                ;//存放包数索引号,每包字节数


#define UP_SYSTEM_C_SIZE 1200
extern unsigned char UP_SYSTEM_C_LANG[UP_SYSTEM_C_SIZE]                         ;//升级包数据
#define AnsUp_SpiNum_BfSz 24
extern char ANS_UP_SPI_Num_BuF[AnsUp_SpiNum_BfSz]                              ;//向平台索要升级数据包



extern char ANS_UP_STAT_2_NET(void)                                            ;//回复升级启动指令平台数据
extern void Delayms(unsigned long int ms)                                      ;//XX*1Ms延时
extern char GSM_SendData(const char * Data,int num)                            ;//数据发送指令
extern void Tran_ID_CmdNum(char* ID_Addr,char CmdNum)                   ;//转存ID号和命令编码                              
extern unsigned int crc_modbus2(unsigned char *puchMsg, unsigned int usDataLen);//CRC
extern char COMP_Peice_STR(char *Source,char *Target,unsigned int T_Cnt,
                           unsigned int S_Cnt,unsigned int Count )             ;//比较两个字符串是否相同
extern char Compare_String(char *Source,char *Target,unsigned int Count )      ;//比较两个字符串是否相同
extern void Flash_WriteChar(unsigned int Addr,unsigned char Data)              ;//向Flash中写入一个字节
extern void READ_UP_SPI_FLASH(void)                                            ;//转移软件版本号及升级类型


extern char UP_SPI_Num_Net(void)                                               ;//SPI包数索引号,发送给平台索引包
extern char UP_A2_NUM_CRC(void)                                                ;//升级程序CRC校验



//extern void Ce_Shi_SPI_NUM(void)                                               ;//回复升级启动指令平台数据
//extern unsigned int  UP_SPI_Ce_SHI                                             ;








































