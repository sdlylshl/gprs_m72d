
#include "msp430x54x.h"

#define SECD_1 1024
#define SECD_2 2048
#define SECD_5 5120


//0x03==������ʾ��;0x0C==����������;0x30==�������ſ�����;0xC0==����GPS,GSMϵͳ����
//unsigned long int UP_All_Bit_Len                                    ;//���������ļ����ֽ���
//char UP_Soft_Flag                                                   ;//����汾�����жϱ�־λ 
//0x11==����汾����;0x00==������汾����;0xXX==δ֪������
extern unsigned char UDP_Built_flag                                            ;//11==���罨����00==����Ͽ�����û�н���
extern unsigned char UP_STR_A10_Type                                           ;//��������ϵͳ���� 
extern unsigned char UP_SPI_Num_Flag                                           ;//11==ARM��Ҫ����(����������,ÿ���ֽ���)
extern unsigned char Ask_index;
extern unsigned int Index;
extern unsigned long int UP_SPI_Sum_Len                                        ;//�ۼ��ѽ����ļ��ֽ���
extern unsigned long int Heart_Beat_Send_Cnt                          ;//�������ʹ�������
extern unsigned long int UP_SPI_NUM_CNT                                       ;//Զ����������
extern unsigned long int UP_Send_Times                                        ;//Զ���������ʹ�������
extern unsigned long int RD_Data_Time                                   ;//���� MUC Ӧ��ʱ�����



extern unsigned int RD_UCA0_Time                                             ;




extern char Sys_NOW_UP_Flag;//ϵͳ����������־ 11==������ 55 δ����


extern char Module_Status[6]                                                   ;//ģ��״̬��ϢGPS��Ϣ+GSM��Ϣ+ϵͳ״̬1��2+����״̬1��2
extern char UP_Root_Stat_BuF[29]                                               ;//������ط�����������ָ��
extern unsigned char UP_Stat_SPI_BuF[18]                                       ;//������ط�����������ָ��
extern unsigned char Soft_VER_memory[32]                                                ;//�洢����汾��
extern char Module_Status[6]                                                   ;//ģ��״̬��ϢGPS��Ϣ+GSM��Ϣ+ϵͳ״̬1��2+����״̬1��2
extern char ANS_Up_Stat_Buf[20]                                                ;//�ظ�ƽ̨��������ָ������
extern char GPS_GSM_ID_Memory[4]                                               ;//һ����豸ID�ն˺�  {0X38,0X1C,0X06,0X00}
//extern char   UP_SPI_Num_BuF[4]                                                ;//��Ű���������,ÿ���ֽ���


#define UP_SYSTEM_C_SIZE 1200
extern unsigned char UP_SYSTEM_C_LANG[UP_SYSTEM_C_SIZE]                         ;//����������
#define AnsUp_SpiNum_BfSz 24
extern char ANS_UP_SPI_Num_BuF[AnsUp_SpiNum_BfSz]                              ;//��ƽ̨��Ҫ�������ݰ�



extern char ANS_UP_STAT_2_NET(void)                                            ;//�ظ���������ָ��ƽ̨����
extern void Delayms(unsigned long int ms)                                      ;//XX*1Ms��ʱ
extern char GSM_SendData(const char * Data,int num)                            ;//���ݷ���ָ��
extern void Tran_ID_CmdNum(char* ID_Addr,char CmdNum)                   ;//ת��ID�ź��������                              
extern unsigned int crc_modbus2(unsigned char *puchMsg, unsigned int usDataLen);//CRC
extern char COMP_Peice_STR(char *Source,char *Target,unsigned int T_Cnt,
                           unsigned int S_Cnt,unsigned int Count )             ;//�Ƚ������ַ����Ƿ���ͬ
extern char Compare_String(char *Source,char *Target,unsigned int Count )      ;//�Ƚ������ַ����Ƿ���ͬ
extern void Flash_WriteChar(unsigned int Addr,unsigned char Data)              ;//��Flash��д��һ���ֽ�
extern void READ_UP_SPI_FLASH(void)                                            ;//ת������汾�ż���������


extern char UP_SPI_Num_Net(void)                                               ;//SPI����������,���͸�ƽ̨������
extern char UP_A2_NUM_CRC(void)                                                ;//��������CRCУ��



//extern void Ce_Shi_SPI_NUM(void)                                               ;//�ظ���������ָ��ƽ̨����
//extern unsigned int  UP_SPI_Ce_SHI                                             ;








































