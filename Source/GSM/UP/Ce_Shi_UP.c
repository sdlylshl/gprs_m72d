
#include "msp430x54x.h"
#include "Root_Up.h"



//unsigned int  UP_SPI_Ce_SHI                                       ;

/*******************************************************************\
*	      ��������ANS_UP_STAT_2_NET             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  �ظ���������ָ��ƽ̨����  
*	      ������ 0x01==������0x00==������ 
          ��ʽ�� ID(4)+�������0x69(2)+����ָ��(2)
*	      ����ֵ��������������ָ��״̬ 
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
/*
void Ce_Shi_SPI_NUM(void)                                          //�ظ���������ָ��ƽ̨����
{
    unsigned int  UP_SPI_Ce_CNT                                   ; 
    
    if(UP_SPI_Num_Flag == 0x00)
    {
        UP_SPI_Num_Flag  =0x11                                    ;//11==ARM��Ҫ����(����������,ÿ���ֽ���)
        UP_SPI_Ce_CNT   =   UP_SPI_Ce_SHI                         ;
        
        UP_SPI_Num_BuF[1]   =   UP_SPI_Ce_CNT                     ;
        UP_SPI_Ce_CNT       =   UP_SPI_Ce_CNT   >>  8             ;
        UP_SPI_Num_BuF[0]   =   UP_SPI_Ce_CNT                     ;
    
        UP_SPI_Num_BuF[2]=0x02                                    ;
        UP_SPI_Num_BuF[3]=0x00                                    ;
        
        UP_SPI_Ce_SHI++                                           ;
    }
}


*/























