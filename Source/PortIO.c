#include "msp430x54x.h"
#include "PortIO.h"
#include ".\GPS\GPS.h"
#include ".\GSM\GSM.h"

unsigned char LPM_GPS_CNT; 
unsigned long LPM_SHAKE_CNT;

extern unsigned char LPM;
char LPM3_Open_Box_Gsm_Flag;
extern unsigned char GSM_STATUS;

extern void Init_LPM_IO();

extern unsigned char LPM_GPS_LOCATE_OK;//�͹���ģʽ��,GPS��λ�ɹ�
unsigned char Power_down2on;
/*
void Init_LPM_IO()
{
    //ͨ��PxREN��������������ʹ�ܣ�ͨ��PxOUT����������������
    //���������� 30K~50K
    //�������� ��������Ϊ2mA ��������Ϊ 5mA
    //����I/O�����ܺͲ�����48mA
    P1OUT    = 0x00;//���Ϊ0
    P1DIR    = 0x00;//����
    P1REN    = 0X00;//���õ���
    P1DS     = 0X00;//����������
    P1SEL    = 0X00;//��ͨIO
    P1IES    = 0X00;//�����ش���
    P1IE     = 0X00;//�����ж�
    P1IFG    = 0X00;//���жϴ���

    P2OUT    = 0x00;//���Ϊ0
    P2DIR    = 0x00;//����
    P2REN    = 0X00;//���õ���
    P2DS     = 0X00;//����������
    P2SEL    = 0X00;//��ͨIO
    P2IES    = 0X00;//�����ش���
    P2IE     = 0X00;//�����ж�
    P2IFG    = 0X00;//���жϴ���

    P3OUT    = 0x00;//���Ϊ0
    P3DIR    = 0x00;//����
    P3REN    = 0X00;//���õ���
    P3DS     = 0X00;//����������
    P3SEL    = 0X00;//��ͨIO

    P4OUT    = 0x00;//���Ϊ0
    P4DIR    = 0x00;//����
    P4REN    = 0X00;//���õ���
    P4DS     = 0X00;//����������
    P4SEL    = 0X00;//��ͨIO

    P5OUT    = 0x00;//���Ϊ0
    P5DIR    = 0x00;//����
    P5REN    = 0X00;//���õ���
    P5DS     = 0X00;//����������
    P5SEL    = 0X00;//��ͨIO

    P6OUT    = 0x00;//���Ϊ0
    P6DIR    = 0x00;//����
    P6REN    = 0X00;//���õ���
    P6DS     = 0X00;//����������
    P6SEL    = 0X00;//��ͨIO

    P7OUT    = 0x00;//���Ϊ0
    P7DIR    = 0x00;//����
    P7REN    = 0X00;//���õ���
    P7DS     = 0X00;//����������
    P7SEL    = 0X00;//��ͨIO

    P8OUT    = 0x00;//���Ϊ0
    P8DIR    = 0x00;//����
    P8REN    = 0X00;//���õ���
    P8DS     = 0X00;//����������
    P8SEL    = 0X00;//��ͨIO


    P9OUT    = 0x00;//���Ϊ0
    P9DIR    = 0x00;//����
    P9REN    = 0X00;//���õ���
    P9DS     = 0X00;//����������
    P9SEL    = 0X00;//��ͨIO

    P10OUT    = 0x00;//���Ϊ0
    P10DIR    = 0x00;//����
    P10REN    = 0X00;//���õ���
    P10DS     = 0X00;//����������
    P10SEL    = 0X00;//��ͨIO

    P11OUT    = 0x00;//���Ϊ0
    P11DIR    = 0x00;//����
    P11REN    = 0X00;//���õ���
    P11DS     = 0X00;//����������
    P11SEL    = 0X00;//��ͨIO
}

*/
void GPS_Start()
{
    P5DIR   |=  BIT4                               ; //  P5.4 outputs
    P5OUT   |=  BIT4                               ; // GPSPWR ON
    //P5OUT   &=  ~BIT4                               ; // GPSPWR OFF  
}
void GPS_Stop()
{
    P5DIR   |=  BIT4                               ; //  P5.4 outputs
    P5OUT   &=  ~BIT4                               ; // GPSPWR ON
    //P5OUT   &=  ~BIT4                               ; // GPSPWR OFF  
}


#pragma vector=PORT2_VECTOR
__interrupt void Port2(void)
{
	
	__delay_cycles(1000);
    //������
    //P1OUT  ^= BIT0;//P1.0״̬ȡ��,����LED

    //�ϵ��� p1.0  P2.0   IO�ж�
    //�ϵ��Ǹߵ�ƽ
    if((P2IFG&PWRON)) {//P2.0�����ش���
        //�˳��͹���ģʽ
        LPM = 0;
        //�ر� �ϵ硢�𶯡�����
        Power_down2on=1;
		LPM3_EXIT;

    } else if(P2IFG&SHAKE) {
        //�𶯻���      P2.1        IO�ж�
        //���ж�
		LPM_SHAKE_CNT++;
		if(LPM_SHAKE_CNT==1000)
		{
			Close_Shake();
        	//LPM = 2;
			LPM3_EXIT;
		}
        
    } else if(P2IFG&OPEN) {
        //���л���      P2.3        IO�ж�
      //LPM = 5;//08.29DIGH
      /*if((P2IES&OPEN)==0x00)
      {
        LPM3_Open_Box_Gsm_Flag=0xAA;
        Open_Box_Flag=0xAA;
        Close_Open();
      //Init_Open_Down();
      }*/
      /*else
      {
          if((P2IES&OPEN)==OPEN)
          {
              Open_Box_Flag=0x55;
              Close_Open();
              Init_Open_Up();
              
          }
      }*/
      LPM3_Open_Box_Gsm_Flag=0xAA;
      LPM3_EXIT;
      
    } else if(P2IFG&RING) {
        //���绽�� p2.0  P2.6       IO�ж�
        //LPM = 6;
		LPM3_EXIT;
    }
	
    
    
    
    
	if(P2IFG&LOCATE)//ʹ��P2IFG�ȽϺ�
	{
	    LPM_GPS_CNT++;
		if(LPM_GPS_CNT==5)
		{
			LPM_GPS_CNT=0;
			LPM_GPS_LOCATE_OK=1;
			Close_Locate();
			LPM3_EXIT;
		}
			
	}
    P2IFG=0;//���жϱ�־
    //LPM3_EXIT;
}





      




