#include "msp430x54x.h"
#include "GSM.h"

//atoi �ַ���ת int
//itoa int ת�ַ���
//GSM_String
unsigned int GSM_BUF_SIZE;
unsigned int GSM_TimeOut_1ms                                               ;
extern int GSM_strstr (const char * Main_str,const char * search_str)      ;//�ַ�������
extern int GSM_strlen (const char * str)                                   ;//�ַ�������
extern int GSM_strcmp (const char * src,const char * dst)                  ;//�ַ����Ƚ�
extern char * GSM_strsep (char **stringp, const char *delim)               ;//�ַ����ָ�
extern void Delayms(unsigned long int ms)                                  ;//XX*1Ms��ʱ
extern unsigned char GSM_SendCMD(char paktype, const char * s,
                                 unsigned int Len,int ms)                  ;//GSM_CMD
extern char GSM_SendData(const char * Data,int num);

unsigned char GSM_SendCMD(char paktype, const char * s,unsigned int Len,int ms)
{
    int i;
    unsigned char * GSM_RX_Buf;
    unsigned char * REV_OK;
    //������ 0ͻ���� 1 ��ѯ�� 2 ���ð� 4 ���ݰ� 8 ����� 0x10 δ֪��
    packet_type = paktype;

    switch (packet_type) {
    case PACKET_BRUST:
        REV_OK = &M72D_Brust_STATUS;
        GSM_BUF_SIZE = M72D_Brust_RX_BUF_SIZE; //���������С
        GSM_RX_Buf = M72D_Brust_RX_Buf; //���ý�������
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
    GSM_TimeOut_1ms = 0                               ;//ms��ʱ����������
    for (i = 0; i < GSM_BUF_SIZE; i++)                 //��ָ����ջ�����
    {
        GSM_RX_Buf[i] = 0                             ;
    }
    if(Len) 
    {                                                  //����ָ��
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
    UCA0TXBUF = 0x0D                                  ;//���ͽ�����
    while ((UCA0IFG & UCTXIFG) == 0)                  ;
    while ((!(*REV_OK)) && (GSM_TimeOut_1ms < ms))    ;//�ȴ� ERRO �� OK�ķ��� ��ʱ�˳�
    packet_type = 0                                   ;
    M72D_RX_Buf_Cnt = 0                               ;
    return *REV_OK                                    ;
}

//���ݷ���

//���ݷ���ָ��
//char * TCPIP_AT_QISEND = "AT+QISEND=0000000001";

char SendCMD[10+10]= {'A','T','+','Q','I','S','E','N','D','='};

char GSM_SendData(const char * Data,int num)
{
    int nums                                                              ;//���������ݳ��� 0�Զ�����
    if(num)
        nums = num                                                        ;
    else
        nums=GSM_strlen(Data)                                             ;

    //����ת�ַ� void DecToANS(char offset, char*ps,unsigned long src)
    GSM_ltoa(10,SendCMD,nums)                                             ;
    if(GSM_SendCMD(PACKET_NETCONFIG,SendCMD,0,10)=='>') 
    {   
        Delayms(200)                                                      ;//XX*1Ms��ʱ
        if(GSM_SendCMD(PACKET_Query,Data,nums,50)==1)
            return 1;
    } else
        //�ط�
        if(GSM_SendCMD(PACKET_NETCONFIG,SendCMD,0,10)=='>') 
        {
            Delayms(200)                                                  ;//XX*1Ms��ʱ
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
    //������ 0ͻ���� 1 ��ѯ�� 2 ���ð� 4 ���ݰ� 8 ����� 0x10 δ֪��
    packet_type = paktype;

    switch (packet_type) {
    case PACKET_BRUST:
        REV_OK = &M72D_Brust_STATUS;
        GSM_BUF_SIZE = M72D_Brust_RX_BUF_SIZE; //���������С
        GSM_RX_Buf = M72D_Brust_RX_Buf; //���ý�������
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
    GSM_TimeOut_1ms = 0                               ;//ms��ʱ����������
    for (i = 0; i < GSM_BUF_SIZE; i++)                 //��ָ����ջ�����
    {
        GSM_RX_Buf[i] = 0                             ;
    }
    if(Len) 
    {                                                  //����ָ��
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
    UCA0TXBUF = 0x0D                                  ;//���ͽ�����
    while ((UCA0IFG & UCTXIFG) == 0)                  ;
    //Delayms(2)                                        ;//XX*1Ms��ʱ
    while ((!(*REV_OK)) && (GSM_TimeOut_1ms < ms))    ;//�ȴ� ERRO �� OK�ķ��� ��ʱ�˳�
    //Delayms(1)                                        ;//XX*1Ms��ʱ
    packet_type = 0                                   ;
    M72D_RX_Buf_Cnt = 0                               ;
    return *REV_OK                                    ;
}



char GSM_Send_Data(const char * Data,int num)
{
    int nums                                                              ;//���������ݳ��� 0�Զ�����
    if(num)
        nums = num                                                        ;
    else
        nums=GSM_strlen(Data)                                             ;

    //����ת�ַ� void DecToANS(char offset, char*ps,unsigned long src)
    GSM_ltoa(10,SendCMD,nums)                                             ;
    if(GSM_Send_CMD(PACKET_NETCONFIG,SendCMD,0,10)=='>') 
    {   
        Delayms(200)                                                      ;//XX*1Ms��ʱ
        if(GSM_Send_CMD(PACKET_Query,Data,nums,50)==1)
            return 1;
    } else
        //�ط�
        if(GSM_Send_CMD(PACKET_NETCONFIG,SendCMD,0,10)=='>') 
        {
            Delayms(200)                                                  ;//XX*1Ms��ʱ
            if(GSM_Send_CMD(PACKET_Query,Data,nums,50)==1)
                return 1;
        }
    return 0;
}
