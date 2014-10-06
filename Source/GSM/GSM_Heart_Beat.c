
#include "msp430x54x.h"
#include "GSM.h"

unsigned long int Heart_Beat_Send_Cnt                          ;//�������ʹ�������
unsigned long int HeartBeat_TimeOut=MSP_A0_Min_1               ;//��������������ʱ��  61425
unsigned long int Heart_Beat_Count=0                           ;//��������1msʱ�����
unsigned long int Heart_Beat_Num=0                             ;//����������  
char Heart_Beat_SendERROR                                      ;//����ʧ�ܱ�־
char Heart_Beat[10]                                            ;//�������ݰ�

/*******************************************************************\
*	      ��������GSM_Heart_Beat             
*	      �������ⲿ�ļ�����
*	      ���ܣ�   PING���
*	      ������  �������˵���� GPS�ն˵�¼������ƶ���������
          GPS�ն˵�IP��ַ�Ͷ˿ں��Ƕ�̬�ģ�GPS�ն��������֮�����
          UDP���䷽ʽ�����GPS����һ���Ӳ���������������ݣ��ƶ���
          �罫���ջ�GPS�ն˵�ǰ��IP�Ͷ˿ںţ���GPS�ն���һ���ٷ���
          ����ʱ�����ȡ�µ�IP�Ͷ˿ںš�����Ϊ��ά��������·��GPS��
          �˵�¼�����ÿһ���ӷ���һ��PING��������������
          ��ʽ�� 
*	      ����ֵ��  �������Ҫ�������ظ�
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/

char GSM_Heart_Beat()
{
    unsigned long int Heart_Beat_Cnt                           ;//1msʱ�����    
    if((Heart_Beat_Count>HeartBeat_TimeOut)&&(UDP_Built_flag==0x11))
    {
        Heart_Beat_Count=0                                     ;//��������1msʱ���������
        Heart_Beat_Num++                                       ;//�����ۼӼ���
        Heart_Beat_Send_Cnt++;
        Heart_Beat_SendERROR=1                                 ;//��λ����ʧ�ܱ�־
       
        //UP_SPI_Num_Flag=0x11;
        Heart_Beat_Cnt=Heart_Beat_Num                          ;//����������ֵ
        Heart_Beat[9]=Heart_Beat_Cnt                           ;
        Heart_Beat_Cnt= Heart_Beat_Cnt >>8                     ;
        Heart_Beat[8]=Heart_Beat_Cnt                           ;
        Heart_Beat_Cnt= Heart_Beat_Cnt >>8                     ;
        Heart_Beat[7]=Heart_Beat_Cnt                           ;
        Heart_Beat_Cnt= Heart_Beat_Cnt >>8                     ;
        Heart_Beat[6]=Heart_Beat_Cnt                           ;
            
        if(GSM_SendData(Heart_Beat,sizeof(Heart_Beat)))         //������������
        {
           THR_Mint_Time_Cnt    =0                             ;//3.5����ʱ����������ڲ��ϴ����ݼ��
           Heart_Beat_SendERROR=0                              ;
           return 1                                            ;
        }  
    }
    
    if((Heart_Beat_SendERROR)&&(UDP_Built_flag==0x11)
       &&(Heart_Beat_Count<5119)&&(Heart_Beat_Count>1023))     //1-5Sʧ���ط�
    {
        Delayms(200);//XX*1Ms��ʱ
        if(GSM_SendData(Heart_Beat,sizeof(Heart_Beat)))
        {
            THR_Mint_Time_Cnt   =0                             ;//3.5����ʱ����������ڲ��ϴ����ݼ��
            Heart_Beat_SendERROR=0                             ;
            return 1                                           ;
        }
        Heart_Beat_SendERROR=0                                 ;
    }
    
  return 0                                                     ;
}

