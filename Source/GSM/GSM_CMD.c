#include "msp430x54x.h"
#include "GSM.h"

//atoi 字符串转 int
//itoa int 转字符串
//GSM_String
unsigned int GSM_BUF_SIZE;
unsigned int GSM_TimeOut_1ms                                               ;
extern int GSM_strstr (const char * Main_str,const char * search_str)      ;//字符串查找
extern int GSM_strlen (const char * str)                                   ;//字符串长度
extern int GSM_strcmp (const char * src,const char * dst)                  ;//字符串比较
extern char * GSM_strsep (char **stringp, const char *delim)               ;//字符串分割
extern void Delayms(unsigned long int ms)                                  ;//XX*1Ms延时
extern unsigned char GSM_SendCMD(char paktype, const char * s,
                                 unsigned int Len,int ms)                  ;//GSM_CMD
extern char GSM_SendData(const char * Data,int num);

unsigned char GSM_SendCMD(char paktype, const char * s,unsigned int Len,int ms)
{
    int i;
    unsigned char * GSM_RX_Buf;
    unsigned char * REV_OK;
    //包类型 0突发包 1 查询包 2 配置包 4 数据包 8 网络包 0x10 未知包
    packet_type = paktype;

    switch (packet_type) {
    case PACKET_BRUST:
        REV_OK = &M72D_Brust_STATUS;
        GSM_BUF_SIZE = M72D_Brust_RX_BUF_SIZE; //配置数组大小
        GSM_RX_Buf = M72D_Brust_RX_Buf; //配置接收数组
        break;

    case PACKET_Query:
        REV_OK = &M72D_Query_STATUS;
        GSM_BUF_SIZE = M72D_Query_RX_BUF_SIZE;
        GSM_RX_Buf = M72D_Query_RX_Buf;
        break;

    case PACKET_CONFIG:
        REV_OK = &M72D_SetConfig_STATUS;
        GSM_BUF_SIZE = M72D_SetConfig_RX_BUF_SIZE;
        GSM_RX_Buf = M72D_SetConfig_RX_Buf;
        break;

    case PAKET_DATA:
        REV_OK = &M72D_ServerData_STATUS;
        GSM_BUF_SIZE = M72D_ServerData_RX_BUF_SIZE;
        GSM_RX_Buf = M72D_ServerData_RX_Buf;
        break;

    case PACKET_NETCONFIG:
        REV_OK = &M72D_NetConfig_STATUS;
        GSM_BUF_SIZE = M72D_NetConfig_RX_BUF_SIZE;
        GSM_RX_Buf = M72D_NetConfig_RX_Buf;
        break;

    case PCKET_OTHER:
        REV_OK = &M72D_Other_STATUS;
        GSM_BUF_SIZE = M72D_Other_RX_BUF_SIZE;
        GSM_RX_Buf = M72D_Other_RX_Buf;
        break;

    default:
        break;
    }

    *REV_OK = 0                                       ;
    M72D_RX_Buf_Cnt = 0;
    GSM_TimeOut_1ms = 0                               ;//ms定时计数器清零
    for (i = 0; i < GSM_BUF_SIZE; i++)                 //清指令接收缓冲器
    {
        GSM_RX_Buf[i] = 0                             ;
    }
    if(Len) 
    {                                                  //发送指令
        while (Len--) 
        {
            UCA0TXBUF = *s++                          ;
            while ((UCA0IFG & UCTXIFG) == 0)          ;
        }
    } 
    else 
    {
        while (UCA0TXBUF = *s++) 
        {
            while ((UCA0IFG & UCTXIFG) == 0)          ;
        }
    }
    UCA0TXBUF = 0x0D                                  ;//发送结束符
    while ((UCA0IFG & UCTXIFG) == 0)                  ;
    while ((!(*REV_OK)) && (GSM_TimeOut_1ms < ms))    ;//等待 ERRO 或 OK的返回 超时退出
    packet_type = 0                                   ;
    M72D_RX_Buf_Cnt = 0                               ;
    return *REV_OK                                    ;
}

//数据发送

//数据发送指令
//char * TCPIP_AT_QISEND = "AT+QISEND=0000000001";

char SendCMD[10+10]= {'A','T','+','Q','I','S','E','N','D','='};

char GSM_SendData(const char * Data,int num)
{
    int nums                                                              ;//待发送数据长度 0自动计算
    if(num)
        nums = num                                                        ;
    else
        nums=GSM_strlen(Data)                                             ;

    //数字转字符 void DecToANS(char offset, char*ps,unsigned long src)
    GSM_ltoa(10,SendCMD,nums)                                             ;
    if(GSM_SendCMD(PACKET_NETCONFIG,SendCMD,0,10)=='>') 
    {   
        Delayms(200)                                                      ;//XX*1Ms延时
        if(GSM_SendCMD(PACKET_Query,Data,nums,50)==1)
            return 1;
    } else
        //重发
        if(GSM_SendCMD(PACKET_NETCONFIG,SendCMD,0,10)=='>') 
        {
            Delayms(200)                                                  ;//XX*1Ms延时
            if(GSM_SendCMD(PACKET_Query,Data,nums,50)==1)
                return 1;
        }
    return 0;
}











unsigned char GSM_Send_CMD(char paktype, const char * s,unsigned int Len,int ms)
{
    int i;
    unsigned char * GSM_RX_Buf;
    unsigned char * REV_OK;
    //包类型 0突发包 1 查询包 2 配置包 4 数据包 8 网络包 0x10 未知包
    packet_type = paktype;

    switch (packet_type) {
    case PACKET_BRUST:
        REV_OK = &M72D_Brust_STATUS;
        GSM_BUF_SIZE = M72D_Brust_RX_BUF_SIZE; //配置数组大小
        GSM_RX_Buf = M72D_Brust_RX_Buf; //配置接收数组
        break;

    case PACKET_Query:
        REV_OK = &M72D_Query_STATUS;
        GSM_BUF_SIZE = M72D_Query_RX_BUF_SIZE;
        GSM_RX_Buf = M72D_Query_RX_Buf;
        break;

    case PACKET_CONFIG:
        REV_OK = &M72D_SetConfig_STATUS;
        GSM_BUF_SIZE = M72D_SetConfig_RX_BUF_SIZE;
        GSM_RX_Buf = M72D_SetConfig_RX_Buf;
        break;

    case PAKET_DATA:
        REV_OK = &M72D_ServerData_STATUS;
        GSM_BUF_SIZE = M72D_ServerData_RX_BUF_SIZE;
        GSM_RX_Buf = M72D_ServerData_RX_Buf;
        break;

    case PACKET_NETCONFIG:
        REV_OK = &M72D_NetConfig_STATUS;
        GSM_BUF_SIZE = M72D_NetConfig_RX_BUF_SIZE;
        GSM_RX_Buf = M72D_NetConfig_RX_Buf;
        break;

    case PCKET_OTHER:
        REV_OK = &M72D_Other_STATUS;
        GSM_BUF_SIZE = M72D_Other_RX_BUF_SIZE;
        GSM_RX_Buf = M72D_Other_RX_Buf;
        break;

    default:
        break;
    }

    *REV_OK = 0                                       ;
    M72D_RX_Buf_Cnt = 0;
    GSM_TimeOut_1ms = 0                               ;//ms定时计数器清零
    for (i = 0; i < GSM_BUF_SIZE; i++)                 //清指令接收缓冲器
    {
        GSM_RX_Buf[i] = 0                             ;
    }
    if(Len) 
    {                                                  //发送指令
        while (Len--) 
        {
            UCA0TXBUF = *s++                          ;
            while ((UCA0IFG & UCTXIFG) == 0)          ;
        }
    } 
    else 
    {
        while (UCA0TXBUF = *s++) 
        {
            while ((UCA0IFG & UCTXIFG) == 0)          ;
        }
    }
    UCA0TXBUF = 0x0D                                  ;//发送结束符
    while ((UCA0IFG & UCTXIFG) == 0)                  ;
    //Delayms(2)                                        ;//XX*1Ms延时
    while ((!(*REV_OK)) && (GSM_TimeOut_1ms < ms))    ;//等待 ERRO 或 OK的返回 超时退出
    //Delayms(1)                                        ;//XX*1Ms延时
    packet_type = 0                                   ;
    M72D_RX_Buf_Cnt = 0                               ;
    return *REV_OK                                    ;
}



char GSM_Send_Data(const char * Data,int num)
{
    int nums                                                              ;//待发送数据长度 0自动计算
    if(num)
        nums = num                                                        ;
    else
        nums=GSM_strlen(Data)                                             ;

    //数字转字符 void DecToANS(char offset, char*ps,unsigned long src)
    GSM_ltoa(10,SendCMD,nums)                                             ;
    if(GSM_Send_CMD(PACKET_NETCONFIG,SendCMD,0,10)=='>') 
    {   
        Delayms(200)                                                      ;//XX*1Ms延时
        if(GSM_Send_CMD(PACKET_Query,Data,nums,50)==1)
            return 1;
    } else
        //重发
        if(GSM_Send_CMD(PACKET_NETCONFIG,SendCMD,0,10)=='>') 
        {
            Delayms(200)                                                  ;//XX*1Ms延时
            if(GSM_Send_CMD(PACKET_Query,Data,nums,50)==1)
                return 1;
        }
    return 0;
}
