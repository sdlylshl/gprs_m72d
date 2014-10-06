
#include "msp430x54x.h"
#include ".\GPS\GPS.h"
#include ".\GSM\GSM.h"
#include "PortIO.h"

char* Open_Box_Flash=(char *)0x1844;//���б����׵�ַ001844
#define Open_Box_Warn_Times 3
unsigned char Open_Box_Num;

#define Close_Box_Check_Times 5
unsigned char Close_Box_Num;
/*******************************************************************\
*	      ��������Read_Open_Box_Flash             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ���м�ⱨ��
*	      ������  
*	      ����ֵ��  
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
void Read_Open_Box_Flash(void)
{
    if((*Open_Box_Flash==0xAA)&&(*(Open_Box_Flash+1)==0xAA))
    {
       Open_Box_Flag=0xAA;
       Module_Status[2] |=0x0C;
       GPS_GSM_System_Stu[2]|=0x0C;//���Ƴ���908C
       LPM3_Open_Box_Gsm_Flag=0xAA;
    }
    
    if((*Open_Box_Flash==0x55)&&(*(Open_Box_Flash+1)==0x55))
    {
       Open_Box_Flag=0x55;
       Module_Status[2] &=~0x0C;
       GPS_GSM_System_Stu[2]&=~0x0C;
    }
}


/*******************************************************************\
*	      ��������Open_Box_Check             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  ���м�ⱨ��
*	      ������  
*	      ����ֵ��  
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
void Open_Box_Check(void)
{
    //(MSP430_POWER_ON_FLAG==0x55)&&
    if((P2IN&OPEN)&&(Open_Box_Warn_Times<Open_Box_Num++)&&((*Open_Box_Flash!=0xAA)&&(*(Open_Box_Flash+1)!=0xAA)))
    {
       Open_Box_Num=0;//���м��
       Open_Box_Flag=0xAA;
       LPM3_Open_Box_Gsm_Flag=0xAA;
       Write_Open_Box_Flash();//д���б���Flash
    }
    
    if((!(P2IN&OPEN))&&(Close_Box_Check_Times<Close_Box_Num++)&&((*Open_Box_Flash!=0x55)&&(*(Open_Box_Flash+1)!=0x55)))
    {
       Close_Box_Num=0;//�պм��
       Open_Box_Flag=0x55;
       Write_Open_Box_Flash();
    }    
}



/*******************************************************************\
*	      ��������Open_Box_Check             
*	      �������ⲿ�ļ�����
*	      ���ܣ�  
*	      ������  
*	      ����ֵ��  
*
*	      �޸���ʷ����ÿ��������
\*******************************************************************/
void Write_Open_Box_Flash(void)//���м�ⱨ��Flash
{
   if(!((*Open_Box_Flash==0xAA)&&(*(Open_Box_Flash+1)==0xAA))&&(Open_Box_Flag==0xAA))
   {
       Flash_WriteChar(68,0xAA);
       _NOP();_NOP();_NOP();_NOP();_NOP();
       Flash_WriteChar(69,0xAA);
       _NOP();_NOP();_NOP();_NOP();_NOP();
   }

   if((!((*Open_Box_Flash==0x55)&&(*(Open_Box_Flash+1)==0x55)))&&(Open_Box_Flag==0x55))
   {
       Flash_WriteChar(68,0x55);
       _NOP();_NOP();_NOP();_NOP();_NOP();
       Flash_WriteChar(69,0x55);
       _NOP();_NOP();_NOP();_NOP();_NOP();
   }
   Read_Open_Box_Flash();
}













