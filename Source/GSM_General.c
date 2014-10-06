#include "msp430x54x.h"
#include ".\GPS\GPS.h"
#include ".\GSM\GSM.h"
#include ".\GSM\UP\Root_Up.h"

char Open_Box_Flag;
extern void Time_Count_Value_Init(void)                          ;//ʱ���������
/**********************************************************************\
*	      ��������LPM_GSM_Globl_Init             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ����IP��ַ\Port��  
*	      ������  
          ��ʽ�� ID(4)+�������0x87(2)+IP��ַ(4)+Port��(2)
*	      ����ֵ������IP��ַ\Port��   
*
*	      �޸���ʷ����ÿ��������
\**********************************************************************/
void LPM_GSM_Globl_IN(void)
{
    MCU_Data_TimeOut    =   SECD_30;
    MCU_Data_Count      =   0;
	
    HeartBeat_TimeOut   =   20480 ;//20S
    
    THR_Mint_Time_Cnt   =   0;
    CONNECT_FAIL_Flag   =   0;
    RD_Data_1_Min_Cnt   =   0;//���� MUC Ӧ��ʱ�����
    Soft_Vers_Cnt       =   0;//��������汾��ʱ���ʱ
    Heart_Beat_Send_Cnt =   0;//�������ʹ�������
    Time_Count_Value_Init();
    Sys_NOW_UP_Flag=0x66;//�͹���ģʽ
    CREG_Zhu_Ce_Cnt=0 ;//ע�������ѯע�����
    One_JI_Warn_Cnt=0;//һ����������
    Two_JI_Warn_Cnt=0;//������������
    GSM_INIT_TIME_CNT=SECD_10;//GSM��ʼ����ʱ��
}






/**********************************************************************\
*	      ��������LPM_GSM_Globl_Init             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ����IP��ַ\Port��  
*	      ������  
          ��ʽ�� ID(4)+�������0x87(2)+IP��ַ(4)+Port��(2)
*	      ����ֵ������IP��ַ\Port��   
*
*	      �޸���ʷ����ÿ��������
\**********************************************************************/
void LPM_GSM_Globl_OUT(void)
{
    RD_MCU_TIME_FLASH();        
    MCU_Data_TimeOut    =   MSP_A0_Min_2            ;
    MCU_ACK_TimeOut     =   MSP_A0_Min_1          ;
    HeartBeat_TimeOut   =   MSP_A0_Min_1            ;
    UDP_Built_flag      =   0x00                    ;//11==���罨����00==����Ͽ�����û�н���
    Heart_Beat_Send_Cnt =0                          ;//�������ʹ�������
    Time_Count_Value_Init();
    Sys_NOW_UP_Flag=0x55;//δ����
    One_JI_Warn_Cnt=0;//һ����������
    Two_JI_Warn_Cnt=0;//������������
    CREG_Zhu_Ce_Cnt=0 ;//ע�������ѯע�����
    Module_Status[0] &=~0xC0;
    GPS_GSM_System_Stu[0]&=~0xC0;
    GSM_INIT_TIME_CNT=0;//GSM��ʼ����ʱ��
}






