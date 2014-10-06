
#include "msp430x54x.h"
#include ".\GPS\GPS.h"
#include ".\GSM\GSM.h"
#include "PortIO.h"
//���Ź���ʱ���� ��������1S
//���Ź�ѭ������
unsigned long int WDTnum;
//���Ź����������־
unsigned long int WDT_EXITLPM;

unsigned int adc_num,adc_num1,adc_num2;
float     Vmain = 0;
float     Vbat = 0;

unsigned char GPS_Vmain_Bat;
unsigned int  BAT_V_main;//���ص�ѹ ���
unsigned int  LI_BAT_VOL;//﮵�ص�ѹ ���

#define  PWRON BIT0
/*#define MIN5 300
#define MIN10 600
#define MIN30 7
#define HOUR2 28
#define HOUR24 2
#define HOUR48 4*/

#define AD_CNT 12
#define GPS_Out_Time 300
extern unsigned char GPS_Start_FLG;
unsigned char LPM_GPS_Stop;
unsigned int GPS_WDT_CNT;
unsigned int Shake_Time_CNT;
extern unsigned char Shake_Open_FLG;
extern unsigned long LPM_SHAKE_CNT;

//�ĳǲ�������
#define S30 30
#define MIN1 60
#define MIN2 120
#define MIN3 180
#define MIN5 300
#define MIN10 600
#define MIN15 900
#define MIN20 1200
#define MIN25 1500
#define MIN30 1800
#define MIN90 5400
#define HOUR1 3600
#define HOUR2 7200
#define HOUR3 10800
#define HOUR4 14400
#define HOUR6 21600
#define HOUR8 28800
#define HOUR12 43200
#define HOUR24 86400
#define HOUR48 172800
#define HOUR72 259200
#define HOUR96 345600
#define HOUR120 432000

extern unsigned char LPM;
extern unsigned char LPM_GPS_LOCATE_OK;//�͹���ģʽ��,GPS��λ�ɹ�

extern void Init_ADC();
extern void Close_ADC();
extern void VmainMon();

extern void Init_TimerWDT();
extern void Control_Strategy();
extern void Delayms(unsigned long int ms)                           ;//XX*1Ms��ʱ
extern void Open_Box_Check(void);

void Init_TimerWDT()
{
    // ʹ��WDT�ж�
    SFRIE1  |= WDTIE;

    //WDTHOLD  Watchdog timer Start.
    //WDTSSEL0 Watchdog timer clock source select ACLK
    //WDTTMSEL ����WDTΪInterval timer mode  ��ʱģʽ
    //WDTCNTCL Watchdog timer counter clear.

    //��ʱģʽ
    //WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0;                   // 18'12'16'' 2^31 2G/32K     = 65536s by 32.768K
    //WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS0;            //  1'08'16'' 2^27 128M/32K   = 4096s by 32.768K
    //WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS1;            //     4'16'' 2^23 8192k/32K  = 256s by 32.768K
    //WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS1+WDTIS0;     //       16'' 2^19 512K/32K   = 16s by 32.768K

    WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K
    //WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2+WDTIS0;           // 2^13   8192/32K  = 250ms by 32.768K
    //WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2+WDTIS1;           // 2^9    512/32K   = 15.6ms by 32.768K
    //WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2+WDTIS1+WDTIS0;    // 2^6    64/32K    = 1.95ms by 32.768K
    //���Ź�ģʽ
    //WDTCTL = WDTPW+WDTCNTCL;  //���Ź����
    //WDTCTL = WDTPW+WDTCNTCL+WDTSSEL0+WDTIS2
    
}


//***************************************************************************//
//                                                                           //
//������void Init_ADC(void)                                                  //
//˵������ʼ��ADC12                                                          //
//P6.3��ƿ��ѹ ADC12MEM0                                                                        //
//P6.4﮵�ص�ѹ
//***************************************************************************//
void Init_ADC()
{
   //Vbat
    //VbatCON P6.3
    //P6DIR   |=  BIT3                               ; //  P6.3 outputs
    //P6OUT   |=  BIT3                               ; //  P6.3 set
    //P6OUT   &= ~BIT3                             ; //  P6.3 reset
    //Vbat P6.4
    P6DIR   &= ~BIT3                               ; // P6.4 Input
    P6SEL   |=  BIT3                               ; // Enable A/D channel A0
    P6DIR   &= ~BIT4                               ; // P6.4 Input
    P6SEL   |=  BIT4                               ; // Enable A/D channel A0

    //P7SEL      |= TVBTM                                     ; // Enable A/D channel A0
    //��������1 SHT1
    //��β���/ת��λ MSC
    //SHP = 1  MSC = 0 ÿ��ת����ҪSHI�źŵ������ش����ɼ���ʱ��
    //CONSE!=0 MSC = 1 ���״�ת��ͬ�ϣ��������ת������ǰһ��ת�����֮����������
    //�ڲ��ο���ѹ REF2_5V 0 1.5V 1 2.5V
    //�ο���ѹ���� REFON 0 �ر� 1��
    //ADC12�ں˿���λ ADC12ON 0 �ر��ں� 1 ���ں�
    //����ж����� ADC12OVIE 0 ����� 1 ���
    //ת��ʱ����� ADC12TVIE 0 ����� 1 ���
    //ת������λ ENC 0 ADC12 ���ڳ�ʼģʽ 1 ����
    //����ת������λ ADC12SC
    //ENC = 1 SHP = 1  (1) ADC12SC ��0��Ϊ1 ����ADCת�� (2)ת����ɺ�ADC12SC�Զ���λ
    //ISSH = 0SHP = 0 (1) ADC12SC ���ָߵ�ƽʱ�ɼ�  (2) ADC12SC ��λʱ����һ��ת��
    //����ת��
    //ADC12CTL0   = ADC12ON+ADC12SHT02+ADC12REFON+ADC12REF2_5V; // ��ADC12, ����ʱ��ԴΪ�ڲ����������ο�ԴΪ�ڲ�2.5V
    //����ת��
    ADC12CTL0   = ADC12ON+ADC12SHT02+ADC12REFON+ADC12REF2_5V+ADC12MSC; // ��ADC12, ����ʱ��ԴΪ�ڲ����������ο�ԴΪ�ڲ�2.5V

    //ת���洢��ַ����
    //���������ź�ѡ�� SHSx 00 ADC12SC 01 Timer_A.OUT1 10 Timer_B.OUT0 11 Timer_B.OUT1
    //�����źſ���λѡ�� SHP 0 SAMPCONֱ��Դ�Բ��������ź� 1 SAMPCON ֱ��Դ�Բ�����ʱ�����ɲ��������źŵ������ش���������ʱ��
    //���������źŷ��� ISSH  0 ���������ź� ͬ������  1 ���������ź� ��������
    //ADC12ʱ��Դ��Ƶ ADC12DIV 000 1/1 ... 111 1/8
    //ADC12ʱ��Դѡ�� ADC12SEL 00 �ڲ�ʱ�� 01 ACLK 10 MCLK 11 SMCLK
    //ת��ģʽѡ�� CONSEQ 00��ͨ������ 01 ����ͨ������ 10 ��ͨ����� 11 ����ͨ�����
    //ADC12æ��־ ADC12BUSY 0 �޻ 1 ������ ������ת��������ת��
    //����ת��
    //ADC12CTL1   = ADC12SHP                                  ; // ���ڲ�����ʱ��
    //����ת��
    ADC12CTL1   = ADC12SHP+ADC12CONSEQ0                       ; // ���ڲ�����ʱ��

    //���н������� EOS 0����û�н��� 1���������һ��ת��
    //�ο���ѹԴѡ�� SREFx 0
    //ѡ��ģ������ͨ�� INCHx 0~7  8 Veref+ 9 Vref- 10 Ƭ���¶ȴ����� 11~15 (AVcc-AVss)/2

    ADC12MCTL0  = ADC12SREF_1+ ADC12INCH_3                 ; // Vr+=Vref+ and Vr-=AVss  P6.3��ƿ��ѹ
    ADC12MCTL1  = ADC12SREF_1+ ADC12INCH_4+ADC12EOS        ; // Vr+=Vref+ and Vr-=AVss ���һ��ͨ��  P6.4﮵�ص�ѹ

    __delay_cycles(20000)                                ; // Delay for reference start-up

    //ADC12IE =ADC12IE3 ;                                     //ADC12IFG.x �ж�ʹ��
    ADC12CTL0  |= ADC12ENC                                  ; // Enable conversions
}
void Close_ADC()
{
	ADC12CTL0 = 0x00;
    P6SEL   &= ~BIT4                                        ; // Disable A/D channel A0
	P6SEL   &= ~BIT3                                        ; // Disable A/D channel A0
	P6DIR|=(BIT3+BIT4);
	P6OUT&=~(BIT3+BIT4);
    //

}
//***************************************************************************//
//                                                                           //
//������void VmainMon(void)                                                  //
//˵�����������Դ��ѹ������ƫ�����ƽ������                               //
//
//����˵��: ArrDataBuffer[AD_CNT],�Ųɼ�������
//
//   13.08.26                                                                //
//***************************************************************************//
void VmainMon(void)                                             //�������Դ��ѹ��﮵�ص�ѹ   
{
	unsigned char i,j,k,l;
	float sum=0,sum_Vbat=0;
	float ArrDataBuffer[AD_CNT],ArrDataBuffer_Vbat[AD_CNT],Temp;
	
	
    ADC12CTL0   |= ADC12SC;//ADC12SC��0-->1����ת����ת����ɺ�ADC12SC�Զ���λ
    while (!(ADC12IFG & BIT0));

	//��﮵�ص�ѹ
	for(i=0;i<AD_CNT;i++)
	{
		//ArrDataBuffer_Vbat[i]=((ADC12MEM1*5.0/4095)+0.1);
		//ArrDataBuffer_Vbat[i]=((ADC12MEM1*5.0/4095)+2.42);
		ArrDataBuffer_Vbat[i]=((ADC12MEM1*5.0/4095)+0.44);
		Delayms(1);
		ADC12CTL0   |= ADC12SC;
    	while (!(ADC12IFG & BIT0));
	}
	
	for(i=0;i<AD_CNT;i++)
	{
		//ArrDataBuffer_Vbat[i]=((ADC12MEM1*5.0/4095)+0.1);
		//ArrDataBuffer_Vbat[i]=((ADC12MEM1*5.0/4095)+2.42);
		ArrDataBuffer_Vbat[i]=((ADC12MEM1*5.0/4095)+0.44);
		Delayms(1);
		ADC12CTL0   |= ADC12SC;
    	while (!(ADC12IFG & BIT0));
	}
	
	
	for(j=0;j<AD_CNT-1;j++)//ð������
	{
		for(k=0;k<AD_CNT-j-1;j++)
		{
			if(ArrDataBuffer_Vbat[k]>ArrDataBuffer_Vbat[k+1])	
			{
				Temp=ArrDataBuffer_Vbat[k];
				ArrDataBuffer_Vbat[k]=ArrDataBuffer_Vbat[k+1];
				ArrDataBuffer_Vbat[k+1]=Temp;
			}
		}
	}
	
	for(l=1;l<AD_CNT-1;l++)//ȥ�����ֵ����Сֵ��ĺ�
	{
		sum_Vbat+=ArrDataBuffer_Vbat[l];
	}
	
	Vbat=sum_Vbat/(AD_CNT-2);
	
	
	//���ƿ��ѹ
	for(i=0;i<AD_CNT;i++)
	{
		//ArrDataBuffer[i]=((ADC12MEM0*0.01350655501994680851063829787234)-0.03);
		ArrDataBuffer[i]=((ADC12MEM0*0.01350655501994680851063829787234)-0.4);
		Delayms(1);
		ADC12CTL0   |= ADC12SC;
    	while (!(ADC12IFG & BIT0));
		//Vmain = ((ADC12MEM0*0.01350655501994680851063829787234)-0.03);
	}
	
	for(i=0;i<AD_CNT;i++)
	{
		//ArrDataBuffer[i]=((ADC12MEM0*0.01350655501994680851063829787234)-0.03);
		ArrDataBuffer[i]=((ADC12MEM0*0.01350655501994680851063829787234)-0.4);
		Delayms(1);
		ADC12CTL0   |= ADC12SC;
    	while (!(ADC12IFG & BIT0));
		//Vmain = ((ADC12MEM0*0.01350655501994680851063829787234)-0.03);
	}
	
    
	for(j=0;j<AD_CNT-1;j++)//ð������
	{
		for(k=0;k<AD_CNT-j-1;j++)
		{
			if(ArrDataBuffer[k]>ArrDataBuffer[k+1])	
			{
				Temp=ArrDataBuffer[k];
				ArrDataBuffer[k]=ArrDataBuffer[k+1];
				ArrDataBuffer[k+1]=Temp;
			}
		}
	}

    for(l=1;l<AD_CNT-1;l++)//ȥ�����ֵ����Сֵ��ĺ�
	{
		sum+=ArrDataBuffer[l];
	}
	
	Vmain=sum/(AD_CNT-2);
	
	GPS_Vmain_Bat=(int)Vmain;
	
    BAT_V_main=(int)(Vmain*6.667);
    LI_BAT_VOL=(int)(Vbat*50.0);
}





//------------------------------------------------------------------------------
//��ʱ����
//24Vʹ��
//------------------------------------------------------------------------------
//24V
/*void Control_Strategy()
{
    //LPM �͹���ת������
	VmainMon();
    if(Vmain>24) {
        //BATMODE =1;

        WDT_EXITLPM=MIN30;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K
    } 
	else if(Vmain>23) {
        //BATMODE =2;

        //WDT_EXITLPM=MIN10;
		WDT_EXITLPM=HOUR1;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K

    } 
	else if(Vmain>22) 
    {
        //BATMODE =3;
        //GPS_Inf[14]|=0x03                                              ;//GPSģʽ 0x03 =����
        //WDT_EXITLPM=MIN15;
		LPM = 3;
		WDT_EXITLPM=MIN90;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K

    } 
	else if(Vmain>21) 
    {
        //BATMODE =3;
        //GPS_Inf[14]|=0x03                                              ;//GPSģʽ 0x03 =����
        //WDT_EXITLPM=MIN15;
		LPM = 3;
		WDT_EXITLPM=HOUR4;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K

    }
	 else if(Vbat>4) {

        //BATMODE =6;
        //WDT_EXITLPM=HOUR24;
        //WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0;                   // 18'12'16'' 2^31 2G/32K     = 65536s by 32.768K

		//WDT_EXITLPM=MIN20;
        LPM = 3;
		WDT_EXITLPM=HOUR12;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K
    } 
	 else if(Vbat>3.9) {

        //BATMODE =6;
        //WDT_EXITLPM=HOUR24;
        //WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0;                   // 18'12'16'' 2^31 2G/32K     = 65536s by 32.768K

		//WDT_EXITLPM=MIN20;
        LPM = 4;
		WDT_EXITLPM=HOUR24;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K
    } 
	 else {
        //�رյ�ص�Դ
         //GPS_Inf[14]|=0x04                                              ;//GPSģʽ 0x04=���˯��
        //BATMODE =7;
		 //WDT_EXITLPM=HOUR48;
        //WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0;                   // 18'12'16'' 2^31 2G/32K     = 65536s by 32.768K
		//WDT_EXITLPM=MIN25;
        LPM = 4;
		WDT_EXITLPM=HOUR48;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K
    }
}*/
//24V
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////




//------------------------------------------------------------------------------
//��ʱ����
//12Vʹ��
//------------------------------------------------------------------------------
//12V
/*void Control_Strategy()
{
    //LPM �͹���ת������
	VmainMon();
    if(Vmain>12) {
        //BATMODE =1;

        WDT_EXITLPM=MIN30;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K
    } 
	else if(Vmain>11) {
        //BATMODE =2;

        //WDT_EXITLPM=MIN10;
		WDT_EXITLPM=HOUR1;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K

    } 
	else if(Vmain>10) 
    {
        //BATMODE =3;
        //GPS_Inf[14]|=0x03                                              ;//GPSģʽ 0x03 =����
        //WDT_EXITLPM=MIN15;
		LPM = 3;
		WDT_EXITLPM=MIN90;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K

    } 
	 else if(Vbat>4) {

        //BATMODE =6;
        //WDT_EXITLPM=HOUR24;
        //WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0;                   // 18'12'16'' 2^31 2G/32K     = 65536s by 32.768K

		//WDT_EXITLPM=MIN20;
        LPM = 3;
		WDT_EXITLPM=HOUR12;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K
    } 
	 else if(Vbat>3.9) {

        //BATMODE =6;
        //WDT_EXITLPM=HOUR24;
        //WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0;                   // 18'12'16'' 2^31 2G/32K     = 65536s by 32.768K

		//WDT_EXITLPM=MIN20;
        LPM = 4;
		WDT_EXITLPM=HOUR24;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K
    } 
	 else {
        //�رյ�ص�Դ
         //GPS_Inf[14]|=0x04                                              ;//GPSģʽ 0x04=���˯��
        //BATMODE =7;
		 //WDT_EXITLPM=HOUR48;
        //WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0;                   // 18'12'16'' 2^31 2G/32K     = 65536s by 32.768K
		//WDT_EXITLPM=MIN25;
        LPM = 4;
		WDT_EXITLPM=HOUR48;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K
    }
}*/
//12V
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


struct Low_Pow_Mode
{
  unsigned char MODE_1;
  unsigned char MODE_2;
  unsigned char MODE_3;
  unsigned char MODE_4;
  float         MODE_5;
  float         MODE_6;
}LOW_POW;
//------------------------------------------------------------------------------
//��ʱ����
//���ݳ���
//------------------------------------------------------------------------------
//���ݳ���
void Control_Strategy()
{
    //LPM �͹���ת������
	VmainMon();
    
    if((Vmain<36)&&((Vmain>16))||((Vmain==16)))
    {
        LOW_POW.MODE_1=24;
        LOW_POW.MODE_2=23;
        LOW_POW.MODE_3=22;
        LOW_POW.MODE_4=21;
        LOW_POW.MODE_5=4.0;
        LOW_POW.MODE_6=3.9;
    }
    else
    {
        if((Vmain<16)&&(Vmain>4))
        {
            LOW_POW.MODE_1=12;
            LOW_POW.MODE_2=11;
            LOW_POW.MODE_3=10;
            LOW_POW.MODE_4=9;
            LOW_POW.MODE_5=4.0;
            LOW_POW.MODE_6=3.9;
        }
        else
        {
            LOW_POW.MODE_1=32;
            LOW_POW.MODE_2=32;
            LOW_POW.MODE_3=32;
            LOW_POW.MODE_4=32;
            LOW_POW.MODE_5=4.0;
            LOW_POW.MODE_6=3.9;
        }
    }
    
    if(Vmain>LOW_POW.MODE_1) {
        //BATMODE =1;

        WDT_EXITLPM=HOUR3;//MIN10
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K
    } 
	else if(Vmain>LOW_POW.MODE_2) {
        //BATMODE =2;

        //WDT_EXITLPM=MIN10;HOUR1
		WDT_EXITLPM=HOUR12;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K

    } 
	else if(Vmain>LOW_POW.MODE_3) 
    {
        //BATMODE =3;
        //GPS_Inf[14]|=0x03                                              ;//GPSģʽ 0x03 =����
        //WDT_EXITLPM=MIN15;
		LPM = 3;
		WDT_EXITLPM=HOUR24;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K

    } 
	else if(Vmain>LOW_POW.MODE_4) 
    {
        //BATMODE =3;
        //GPS_Inf[14]|=0x03                                              ;//GPSģʽ 0x03 =����
        //WDT_EXITLPM=MIN15;
		LPM = 3;
		WDT_EXITLPM=HOUR48;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K

    }
	 else if(Vbat>LOW_POW.MODE_5) {

        //BATMODE =6;
        //WDT_EXITLPM=HOUR24;
        //WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0;                   // 18'12'16'' 2^31 2G/32K     = 65536s by 32.768K

		//WDT_EXITLPM=MIN20;
        LPM = 3;
		WDT_EXITLPM=HOUR72;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K
    } 
	 else if(Vbat>LOW_POW.MODE_6) {

        //BATMODE =6;
        //WDT_EXITLPM=HOUR24;
        //WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0;                   // 18'12'16'' 2^31 2G/32K     = 65536s by 32.768K

		//WDT_EXITLPM=MIN20;
        LPM = 4;
		WDT_EXITLPM=HOUR96;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K
    } 
	 else {
        //�رյ�ص�Դ
         //GPS_Inf[14]|=0x04                                              ;//GPSģʽ 0x04=���˯��
        //BATMODE =7;
		 //WDT_EXITLPM=HOUR48;
        //WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0;                   // 18'12'16'' 2^31 2G/32K     = 65536s by 32.768K
		//WDT_EXITLPM=MIN25;
        LPM = 4;
		WDT_EXITLPM=HOUR120;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K
    }
}
//���ݳ���
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//��ʱ����
//����ʹ��
//------------------------------------------------------------------------------
//����ʹ��
/*
void Control_Strategy()
{
    //LPM �͹���ת������
	VmainMon();
    if(Vmain>24) {
        //BATMODE =1;

        WDT_EXITLPM=MIN30;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K
    } 
	else if(Vmain>23) {
        //BATMODE =2;

        //WDT_EXITLPM=MIN10;
		WDT_EXITLPM=HOUR1;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K

    } 
	else if(Vmain>22) 
    {
        //BATMODE =3;
        //GPS_Inf[14]|=0x03                                              ;//GPSģʽ 0x03 =����
        //WDT_EXITLPM=MIN15;
		LPM = 3;
		WDT_EXITLPM=MIN90;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K

    } 
	else if(Vmain>21) 
    {
        //BATMODE =3;
        //GPS_Inf[14]|=0x03                                              ;//GPSģʽ 0x03 =����
        //WDT_EXITLPM=MIN15;
		LPM = 3;
		WDT_EXITLPM=HOUR4;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K

    }
	 else if(Vbat>4) {

        //BATMODE =6;
        //WDT_EXITLPM=HOUR24;
        //WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0;                   // 18'12'16'' 2^31 2G/32K     = 65536s by 32.768K

		//WDT_EXITLPM=MIN20;
        LPM = 3;
		WDT_EXITLPM=MIN1;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K
    } 
	 else if(Vbat>3.9) {

        //BATMODE =6;
        //WDT_EXITLPM=HOUR24;
        //WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0;                   // 18'12'16'' 2^31 2G/32K     = 65536s by 32.768K

		//WDT_EXITLPM=MIN20;
        LPM = 4;
		WDT_EXITLPM=MIN3;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K
    } 
	 else {
        //�رյ�ص�Դ
         //GPS_Inf[14]|=0x04                                              ;//GPSģʽ 0x04=���˯��
        //BATMODE =7;
		 //WDT_EXITLPM=HOUR48;
        //WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0;                   // 18'12'16'' 2^31 2G/32K     = 65536s by 32.768K
		//WDT_EXITLPM=MIN25;
        LPM = 4;
		WDT_EXITLPM=MIN3;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K
    }
}*/
//����ʹ��
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer (void)
{
    //ι��
    //P1OUT  = ~P1OUT                               ; //  P1.0 Reset

    if(WDT_EXITLPM==WDTnum++) {
        WDTnum=0;
        //������߼��� ��ʱ���� (����ʱ���ɲ��Խ���ת��)
        //ÿ�λ�������ǵ͹���ģʽ ���˳�
        //�����AMģʽ���л�ָʾ��״ָ̬ʾ
        if(LPM) 
        {
            if(((P2IN&PWRON)) && LPM) 
            {
                //�˳��͹���ģʽLPM = 0; ������ 08.28�޸Ĳ��Ե͹���
                LPM = 0;
                //�ر� �ϵ硢�𶯡�����
            }
            LPM3_EXIT; // �˳��͹���ģʽ
            
            
			//GPS_WDT_CNT=0;
        } 
        else 
        {
            //����ָʾ��
            //P1OUT  = ~P1OUT                               ; //  P1.0 Reset
        }
    }
	

    Open_Box_Check();
	if(GPS_Start_FLG)
      
	{
		if(GPS_Out_Time==GPS_WDT_CNT++)
		{
			GPS_WDT_CNT=0;
			LPM3_EXIT                                     ; // �˳��͹���ģʽ
			LPM_GPS_Stop=1;
			GPS_Start_FLG=0;
			LPM_GPS_LOCATE_OK=1;
		}
	}
	
	Shake_Time_CNT++;
	if(Shake_Time_CNT==1800)//30����
	{
		Shake_Time_CNT=0;
		Init_Shake();
		LPM_SHAKE_CNT=0;
	}
    
    
    
    
    
	/*if(LPM_GPS_LOCATE_OK==1)//�9.12��ӣ�
	{
		LPM3_EXIT                                     ; // �˳��͹���ģʽ
	}*/
}









