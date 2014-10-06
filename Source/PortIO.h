#include "msp430x54x.h"

extern char Open_Box_Flag;

//�ϵ��� p1.0  P2.0   IO�ж�
#define PWRON BIT0
//�жϹ���
//��������ش���       �������ڣ�ѡ���½��ش�����
//���ж�����
//PxIES���л�����ʹP1IFG��λ�������
#define Init_PWRON()    P2DIR &=  ~PWRON;\
                        P2SEL &=  ~PWRON;\
                        P2IES &=  ~PWRON;\
                        P2IE  |=  PWRON;\
                        P2IFG=0

//��Ϊ��ͨIO ���� ������
#define Close_PWRON()   P2SEL &=  ~PWRON;\
                        P2IE  &=  ~PWRON


//�𶯻���      P2.1        IO�ж�
#define SHAKE BIT1
//�жϹ���
//ѡ���½��ش���
//���ж�����
//PxIES���л�����ʹP1IFG��λ�������
#define Init_Shake()    P2DIR &=  ~SHAKE;\
                        P2SEL &=  ~SHAKE;\
                        P2IES |=  SHAKE;\
                        P2IE  |=  SHAKE;\
                        P2IFG=0

//��Ϊ��ͨIO ���� ������
#define Close_Shake()   P2SEL &=  ~SHAKE;\
                        P2IE  &=  ~SHAKE


//���л���      P2.3        IO�ж�
#define OPEN BIT3
//�жϹ���
//ѡ���½��ش���
//���ж�����
//PxIES���л�����ʹP1IFG��λ�������
#define Init_Open_Up()  P2DIR &=  ~OPEN;\
                        P2SEL &=  ~OPEN;\
                        P2IES &=  ~OPEN;\
                        P2IE  |=   OPEN;\
                        P2IFG=0
/* �պб�������
#define Init_Open_Down()P2DIR &=  ~OPEN;\
                        P2SEL &=  ~OPEN;\
                        P2IES |=   OPEN;\
                        P2IE  |=   OPEN;\
                        P2IFG=0
*/

//��Ϊ��ͨIO ���� ������
#define Close_Open()    P2SEL &=  ~OPEN;\
                        P2IE  &=  ~OPEN

//���绽�� p2.0  P2.6       IO�ж�
#define RING BIT6
//�жϹ���
//ѡ���½��ش���
//���ж�����
//PxIES���л�����ʹP1IFG��λ�������
#define Init_Ring()     P2DIR &=  ~RING;\
                        P2SEL &=  ~RING;\
                        P2IES |=  RING;\
                        P2IE  |=  RING;\
                        P2IFG=0

//��Ϊ��ͨIO ���� ������
#define Close_Ring()    P2SEL &=  ~RING;\
                        P2IE  &=  ~RING

//GPS��λ����   P2.7      IO�ж�
#define LOCATE BIT7
//�жϹ���
//ѡ���½��ش���
//���ж�����
//PxIES���л�����ʹP1IFG��λ�������
#define Init_Locate()   P2DIR &=  ~LOCATE;\
                        P2SEL &=  ~LOCATE;\
                        P2IES |=  LOCATE;\
                        P2IE  |=  LOCATE;\
                        P2IFG=0

//��Ϊ��ͨIO ���� ������
#define Close_Locate()  P2SEL &=  ~LOCATE;\
                        P2IE  &=  ~LOCATE





//2013.9.4 ���  
//��ʼ��δʹ�õ�IO
//δʹ�õ�IO����Ϊ��ͨIO,�������PCB�ϲ�����
//P1.1, P1.2, P1.3, P1.4, P1.5, P1.6, P1.7
//P2.2, P2.4, P2.5
//P3.0, P3.1, P3.2, P3.3, P3.6, P3.7
//P4.1, P4.3, P4.4, P4.6
//P5.1, P5.5
//P6.0, P6.1, P6.2, P6.3, P6.5, P6.6, P6.7
//P7.2, P7.5, P7.7, 
//P8.2, P8.3, P8.4, P8.5, P8.6, P8.7
//P9.6, P9.7
//P10.0, P10.3, P10.4, P10.5, P10.6, P10.7
//P11.0, P11.1, P11.2, 
#define Init_NC_IO()   P1DIR|=BIT1+BIT2+BIT3+BIT4+BIT5+BIT6+BIT7;\
					   P2DIR|=BIT2+BIT4+BIT5;\
					   P3DIR|=BIT0+BIT1+BIT2+BIT3+BIT6+BIT7;\
					   P4DIR|=BIT1+BIT3+BIT4+BIT6;\
					   P5DIR|=BIT0+BIT1+BIT5;\
                       P6DIR|=BIT0+BIT1+BIT2+BIT5+BIT6+BIT7;\
					   P7DIR|=BIT2+BIT4+BIT5+BIT6+BIT7;\
					   P8DIR|=BIT2+BIT3+BIT4+BIT5+BIT6+BIT7;\
					   P9DIR|=BIT4+BIT5+BIT6+BIT7;\
					   P10DIR|=BIT0+BIT1+BIT2+BIT3+BIT4+BIT5+BIT6+BIT7;\
					   P11DIR|=BIT0+BIT1+BIT2;\
					   P10OUT&=~BIT6

#define Close_A0()     TA0CTL=0x00;

#define Close_UCA1()   P5SEL&=~(BIT6+BIT7);\
                       P5DIR|=(BIT6+BIT7);\
                       P5OUT&=~(BIT6+BIT7)



#define Close_UCA0()   P3SEL&=~(BIT4+BIT5);\
                       P3DIR|=(BIT4+BIT5);\
                       P3OUT&=~(BIT4+BIT5)