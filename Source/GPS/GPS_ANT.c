

#include "msp430x54x.h"
#include "GPS.h"
#define GPS_ANT_OK_NUM  100

char GPS_ANT_STATUS_Flag;//AA==���߱պϣ�55==���߶Ͽ�
unsigned long int GPS_ANT_OPEN_CNT; //GPS���߿�·״̬��¼����
unsigned long int GPS_ANT_Close_CNT;//GPS���߱պ�״̬��¼����
unsigned long int GPS_ANT_IO_CNT;//GPS���ߴ���

/*******************************************************************\
*	      ��������GPS_ANT_STATUS             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  GPS���߼��  
*	      ������  
           1��20Ms=���1�Σ�

*	      ����ֵ����     
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
void GPS_ANT_STATUS(void)//GPS���߼��
{
  if(GPS_ANT_IO_CNT>10)
  {
      GPS_ANT_IO_CNT=0;
      if(((P9IN&GPS_ANT)==GPS_ANT))
      {
         if((GPS_ANT_OK_NUM==GPS_ANT_OPEN_CNT++))
         {
             GPS_ANT_OPEN_CNT=0;
             GPS_ANT_Close_CNT=0;
             GPS_ANT_STATUS_Flag=0x55;
             Module_Status[0] |=0x30;//GPS����״̬��·
             GPS_GSM_System_Stu[0]|=0x30;
         }
      }
      
      if(((P9IN&GPS_ANT)==0x00))
      {
        if(GPS_ANT_OK_NUM==GPS_ANT_Close_CNT++)
        {
             GPS_ANT_Close_CNT=0;
             GPS_ANT_OPEN_CNT=0;
             GPS_ANT_STATUS_Flag=0xAA;
             Module_Status[0] &=~0x30;//GPS����״̬�պ�
             GPS_GSM_System_Stu[0]&=~0x30;
        }
      }
  }
}




