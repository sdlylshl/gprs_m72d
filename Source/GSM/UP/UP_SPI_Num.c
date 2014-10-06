

#include "Root_Up.h"

#define ROOT_UP_OUT_TIME 61440

//unsigned int  UP_SPI_NUM_ADD_1   =0                                 ;//Զ�̷��ʹ�������
unsigned char SD_EROR_UP_NUM                                        ;//11==����ʧ�ܣ�
unsigned char UP_SPI_Num_Flag    =0x55                              ;//11==ARM��Ҫ����(����������,ÿ���ֽ���)
unsigned long int UP_SPI_Sum_Len =0xffffffff                                ;//�ۼ��ѽ����ļ��ֽ���
unsigned long int UP_SPI_NUM_CNT                                    ;//Զ����������
unsigned long int UP_Send_Times                                        ;//Զ���������ʹ�������


#define AnsUp_SpiNum_BfSz 24
//char   UP_SPI_Num_BuF[4]                                            ;//��Ű���������,ÿ���ֽ���
char   ANS_UP_SPI_Num_BuF[AnsUp_SpiNum_BfSz];//��ƽ̨��Ҫ�������ݰ�
/*******************************************************************\
*	      ��������UP_SPI_Num_Net             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  SPI����������,���͸�ƽ̨������
*	      ������  
          ��ʽ��  ID(4)+Cmd(2)+ָ����ˮ��(2)+���ݰ�����(2)
                  +������������(1)+���������ļ����ֽ���(2)
                  +�����ļ��汾��(8)+CRCУ��(2))
*	      ����ֵ��    
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
char UP_SPI_Num_Net(void)                                            //SPI����������,���͸�ƽ̨������
{
    unsigned int  UP_SPI_CRC                                        ;
    static   unsigned long int UP_SPI_Sum                                    ;//�ۼ��ѽ����ļ��ֽ���

    if((UDP_Built_flag==0x11)&&(UP_SPI_Num_Flag==0x11)
       &&(UP_SPI_NUM_CNT>128) &&(Heart_Beat_Send_Cnt>1))     //         
    {
        UP_Send_Times++;
        UP_SPI_NUM_CNT      =   0                                   ;//Զ����������
        UP_SPI_Num_Flag=0x00;
     

        Tran_ID_CmdNum(ANS_UP_SPI_Num_BuF,0xA2)                   ;//ת��ID�ź��������                              
        
        ANS_UP_SPI_Num_BuF[6]   =   0x00                            ;//��ָ����ֽ��ܳ��� 
        ANS_UP_SPI_Num_BuF[7]   =   0x18                            ; 
        
        ANS_UP_SPI_Num_BuF[8]   =   UP_STR_A10_Type                 ;//��������ϵͳ���� 
        
        ANS_UP_SPI_Num_BuF[9]   =   0x01                            ;//������־:00==��������0x01==���� 
        

        UP_SPI_Sum_Len  =   UP_SPI_Sum_Len  <<  10                   ;//1024   *512  //�޸ĳ�256
        
        UP_SPI_Sum      =UP_SPI_Sum_Len;
        ANS_UP_SPI_Num_BuF[13]  =   UP_SPI_Sum                  ;
        UP_SPI_Sum  =   UP_SPI_Sum  >>  8                   ;
        ANS_UP_SPI_Num_BuF[12]  =   UP_SPI_Sum                  ;
        UP_SPI_Sum  =   UP_SPI_Sum  >>  8                   ;
        ANS_UP_SPI_Num_BuF[11]  =   UP_SPI_Sum                  ;
        UP_SPI_Sum  =   UP_SPI_Sum  >>  8                   ;
        ANS_UP_SPI_Num_BuF[10]  =   UP_SPI_Sum                  ;
        
        
        UP_SPI_CRC=crc_modbus2((unsigned char *)ANS_UP_SPI_Num_BuF,
                               (AnsUp_SpiNum_BfSz-2))               ;
        ANS_UP_SPI_Num_BuF[22]    =   (char)(UP_SPI_CRC&0xFF)       ;
        ANS_UP_SPI_Num_BuF[23]    =   (char)((UP_SPI_CRC>>8)&0xFF)  ;
        
        SD_EROR_UP_NUM    =   0x11                                  ;

        if(GSM_SendData(ANS_UP_SPI_Num_BuF,AnsUp_SpiNum_BfSz))       //���ݷ���
        {
            SD_EROR_UP_NUM    =0                                    ;//�޸ı�־λ
            return 1                                                ;
        }
        if(SD_EROR_UP_NUM == 0x11)                                   //ʧ���ط�
        {
            Delayms(200)                                            ;//XX*1Ms��ʱ
            if(GSM_SendData(ANS_UP_SPI_Num_BuF,AnsUp_SpiNum_BfSz))
            {          
               SD_EROR_UP_NUM    =    0                             ;
               return 1                                             ;
            }
            SD_EROR_UP_NUM    =    0                                ;
        }
    }
    return 0                                                        ;
}



/*******************************************************************\
*	      ��������Root_UP_Net_Begin             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ���ط�����������ָ��
*	      ������  
          ��ʽ��  ID(4)+Cmd(2)+ָ����ˮ��(2)+���ݰ�����(2)
                  +������������(1)+���������ļ����ֽ���(2)
                  +�����ļ��汾��(8)+CRCУ��(2))
*	      ����ֵ��    
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
void ROOT_UP_TIME_FLASH(void)                                        //���ط�����������ָ��
{

    if((UP_SPI_Num_Flag==0x11)&&(UP_SPI_NUM_CNT>ROOT_UP_OUT_TIME))     //         
    {
      
      
    }
}






