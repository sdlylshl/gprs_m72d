
#include "msp430x54x.h"


unsigned char RD_FLASH_OK_FLAG                                      ;//��ȡflash��־11==OK��00==fail


extern char Module_Status[6]                                        ;//ģ��״̬��ϢGPS��Ϣ+GSM��Ϣ+ϵͳ״̬1��2+����״̬1��2
extern char GPS_GSM_System_Stu[4]                                   ;//GSM\GPS״̬
extern char Heart_Beat[10]                                          ;//�������ݰ�
extern char GPS_GSM_ID_Memory[4]                                    ;//һ����豸ID�ն˺�
extern char GSM_Send_MCU_Buf[106]                                        ;//�ϱ�MCU��������(�ظ�)


extern void Delayms(unsigned long int ms)                           ;//XX*1Ms��ʱ
extern void Tran_ID_CmdNum(char* ID_Addr,char CmdNum)                   ;//ת��ID�ź��������                              


extern void Read_No_Chg_Card_Flash(void);//�������޿�Flash 
extern void READ_SIM_NUM_FH(void)                                   ;//�洢�ֻ����� 
extern void ReaD_IP_PORT_FLASH(void)                                ;//��ȡIP��ַ\Port�� 
extern char RD_OUT_EDGE_FH(void)                                    ;//��ȡԽ�籨�� 
extern void READ_UP_SPI_FLASH(void)                                 ;//ת������汾�ż���������
extern void RD_LOCK_BUS_FLASH(void)                                 ;//��ȡ����״̬
extern void Read_Open_Box_Flash(void);
/********************************************************\
*	 ��������Delayms
     �������ⲿ�ļ�����
*	 ���ܣ�  ��ʱ1ms
*	 ������  
*    ����ֵ����
*	 �޸���ʷ����ÿ��������
\********************************************************/
void Delayms(unsigned long int ms)
{
    unsigned int n;                               //10 Cylce
    for(n=0; n<ms; n++)                           //(N+1)Cylce
        __delay_cycles(15996);                        //(n+3)Cylce
}



//*******************************************************************//
//                                                                   //
//      ������Read_Flash_Data                                        //
//      ˵������ȡFLASH��ȡ��������������                            //
//      ��������                                                     //
//                                                                   //
//*******************************************************************//
void Read_Flash_Data(void)
{     
    unsigned char ID_NUM                                            ;

    char* RD_FH_HD_AD   =  (char *)0x1800                           ;//flash��ȡ�׵�ַ
    char* RD_ID_Addr    =  (char *)0x1801                           ;//ID�׵�ַ
    RD_FLASH_OK_FLAG     = 0x00                                     ;//��λ��ȡflash��־
    
    if(*(RD_FH_HD_AD)==0x01)
    {
        RD_FLASH_OK_FLAG     = 0x11                                 ;//��ȡflash OK��
        for(ID_NUM=0;ID_NUM<4;ID_NUM++)                              //ID
        {   
            GPS_GSM_ID_Memory[ID_NUM]  = *(RD_ID_Addr)++            ; 
        }
        
        ReaD_IP_PORT_FLASH()                                        ;//��ȡIP��ַ\Port��
        
        Tran_ID_CmdNum(Heart_Beat,0xB2)                             ;//ת��ID�ź�������� 
        Tran_ID_CmdNum(GSM_Send_MCU_Buf,0x81)                        ;//ת��ID�ź��������

   }    
        
        RD_LOCK_BUS_FLASH()                                         ;//��ȡ����״̬
        READ_SIM_NUM_FH()                                           ;//�洢�ֻ�����
        RD_OUT_EDGE_FH()                                            ;//��ȡԽ�籨�� 
        READ_UP_SPI_FLASH()                                         ;//ת������汾�ż���������
        Read_Open_Box_Flash();
        Read_No_Chg_Card_Flash();//�������޿�Flash 
}


/*******************************************************************\
*	      ��������Do_GPS_Data             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ���ա�����GPS����  
*	      ������  
           1��ͨ������GPS�������ͣ�00=����Ӧ��11=GPRMC��22=GPGGA�����Ʋ���
           2��Do_GPS_Mesg_OK_Flag��GPS���ݴ�����ɱ�־ 00==δ��ɣ�11==���
           3��Open_UCA1_UART_Init(void)==�ý���������ݲ�����ʱ��GPS�ж�
*	      ����ֵ����     
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
void Tran_ID_CmdNum(char* ID_Addr,char CmdNum)                       //ת��ID�ź��������
{
    char ID_Num                                                     ;//
    char* ID_CmdNum_Addr;
    
    ID_CmdNum_Addr=ID_Addr;
    for(ID_Num=0;ID_Num<4;ID_Num++,ID_CmdNum_Addr++)               
    {
        *ID_CmdNum_Addr    =     GPS_GSM_ID_Memory[ID_Num]                 ;
    }
    
    *(ID_CmdNum_Addr++)    =     CmdNum                 ;
    *(ID_CmdNum_Addr++)    =     CmdNum                 ;
}


/********************************************************\
*	��������ASCIITOHEX
    �������ⲿ�ļ�����
*	���ܣ�   ��ASCII�ַ�ת��Ϊ16���Ƶ���
             ����0x31,0x32ת����󷵻صĽ��Ϊ12
*	������  Input_H==ASCII��λ
            Input_L==ASCII��λ
*	����ֵ��Input_H*16+Input_L
            
*	�޸���ʷ����ÿ��������
\********************************************************/
unsigned char ASCIITOHEX( unsigned char Input_H,unsigned char Input_L )//��ASCII�ַ�ת��Ϊ16���Ƶ���
{
    if( Input_H>0x39 ) Input_H=Input_H-0x37;
    else Input_H=Input_H-0x30;
    if( Input_L>0x39 ) Input_L=Input_L-0x37;
    else Input_L=Input_L-0x30;
    return( Input_H*16+Input_L );
}


/********************************************************\
*	 ��������HEX_TO_ASCII
     �������ⲿ�ļ�����
*	 ���ܣ�  ��һ��16�����ַ�ת��ΪASCII
*	 ������  Input_H��Input_L
*        ����ֵ��One_Char һ��ASCII��
*	 �޸���ʷ����ÿ��������
\********************************************************/
unsigned char HEX_TO_ASCII(char One_Char)                //16�����ַ�ת��ASCII
{
    if(((One_Char>0)&&(One_Char<=9))||(One_Char==0))
        One_Char += 0x30;
    else One_Char = 0xff;
    return One_Char; 
}



/********************************************************\
*	��������Compare_String
   1    �������ⲿ�ļ�����
*	���ܣ�  �Ƚ������ַ����Ƿ���ͬ
*	������  *Source ��Ҫ�Ƚϵ��ַ����ĵ�ͷָ��
                *Target �������Ƚϱ�׼���ַ���ͷָ��
                 Count  ��Ҫ�Ƚϵĳ�

*	����ֵ��0   ��ʾ�ȽϽ������ͬ
                1   ��ʾ�ȽϽ����ͬ
*	�޸���ʷ����ÿ��������
\********************************************************/
char Compare_String(char *Source,
                             char *Target,
                             unsigned int Count )         // 1 �Ƚ������ַ����Ƿ���ͬ
{
    while(Count>0)
    {
        if((*Source)!=(*Target)) return(0);
        Source++;
        Target++;
        Count--;
    }
    return(1);
}


/********************************************************\
*	��������Compare_String
   1    �������ⲿ�ļ�����
*	���ܣ�  �Ƚ������ַ����Ƿ���ͬ
*	������  *Source ��Ҫ�Ƚϵ��ַ����ĵ�ͷָ��
                *Target �������Ƚϱ�׼���ַ���ͷָ��
                 Count  ��Ҫ�Ƚϵĳ�

*	����ֵ��0   ��ʾ�ȽϽ������ͬ
                1   ��ʾ�ȽϽ����ͬ
*	�޸���ʷ����ÿ��������
\********************************************************/
char COMP_Peice_STR(char *Source,char *Target,unsigned int T_Cnt,unsigned int S_Cnt,
                             unsigned int Count )         // 1 �Ƚ������ַ����Ƿ���ͬ
{
    Source=Source+S_Cnt;
    Target=Target+T_Cnt;
    while(Count>0)
    {
        if((*Source)!=(*Target)) return(0);
        Source++;
        Target++;
        Count--;
    }
    return(1);
}
