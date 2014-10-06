

#include "msp430x54x.h"
#include "Root_Up.h"

char Sys_NOW_UP_Flag;//ϵͳ����������־ 11==������ 55 δ����  �������ʱ��Ҫ��λ 88  �͹���ģʽʱΪ66
#define UP_SYSTEM_C_SIZE 1200
unsigned char UP_SYSTEM_C_LANG[UP_SYSTEM_C_SIZE]                         ;//����������
extern unsigned char Data_Bag_TX_Finish;
extern unsigned int      SPI_UP_BIT_SUM                                    ;//�ۼ��ѽ����ļ��ֽ���

/*******************************************************************\
*	      ��������DO_ROOT_UP             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ����Զ����������
*	      ������  
          ��ʽ��  ID(4)+Cmd(2)+ָ����ˮ��(2)+���ݰ�����(2)
                  +������������(1)+���������ļ����ֽ���(2)
                  +�����ļ��汾��(8)+CRCУ��(2))
*	      ����ֵ��    
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
void DO_ROOT_UP(void)                                               //����Զ����������
{
  
    static unsigned long int ROOT_NUM                             ;//�ۼ��ѽ����ļ��ֽ���

    if(Ask_index==1)
    {
        Ask_index=0;
        ROOT_NUM=Index;
        ROOT_NUM=ROOT_NUM<<10;
        if(UP_SPI_Sum_Len!=ROOT_NUM)//�ѽ��յ����ݰ���������
        {
             UP_SPI_Num_Flag=0x11;
             UP_SPI_Sum_Len=Index;
             UP_Send_Times=0;
             Sys_NOW_UP_Flag=0x11;
             
            //��ƽ̨Ҫ���ݰ�;
            //��־��1;		
        }
        
        
        if((UP_SPI_Sum_Len==SPI_UP_BIT_SUM)&&(!Data_Bag_TX_Finish))
        //else
        {
             //UP_SPI_Num_Flag=0x00;
             //Module_Status[4]       = 0x00                                ;//������Ҫ��
             //��־��1;
             Module_Status[5]|=0x0c;//��ÿ��������ɵı�־  
        }
        
        if((UP_SPI_Sum_Len==ROOT_NUM)&&(UP_SPI_NUM_CNT>SECD_5*2))//if((SPI_UP_BIT_SUM!=ROOT_NUM)&&(UP_SPI_NUM_CNT>SECD_5*2))
        {
            if(UP_Send_Times>5)
            {
                UP_SPI_Num_Flag=0x00;
                Sys_NOW_UP_Flag=0x55;
            }
            else
            {
                UP_SPI_Num_Flag=0x11;
				UP_SPI_Sum_Len=Index;
                Sys_NOW_UP_Flag=0x11;
            }
        }
    }
    
    if(Sys_NOW_UP_Flag==0x11)//�ӿ������ٶ�
    {
        RD_Data_Time=128;
        RD_UCA0_Time=10240;
    }
    else
    {
        RD_Data_Time=5120;
        RD_UCA0_Time=512;
    }
    
    UP_SPI_Num_Net()                                               ;//SPI����������,���͸�ƽ̨������
    //Ce_Shi_SPI_NUM()                                               ;//�ظ���������ָ��ƽ̨����
}




/**********************************************************************\
*	      ��������UP_VAR_Init             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ����IP��ַ\Port��  
*	      ������  
          ��ʽ�� ID(4)+�������0x87(2)+IP��ַ(4)+Port��(2)
*	      ����ֵ������IP��ַ\Port��   
*
*	      �޸���ʷ����ÿ��������
\**********************************************************************/
void UP_VAR_Init(void)
{

}
















