
#include "msp430x54x.h"
#include ".\GPS\GPS.h"
#include ".\GSM\GSM.h"
#include "PortIO.h"
//看门狗定时唤醒 唤醒周期1S
//看门狗循环计数
unsigned long int WDTnum;
//看门狗计数溢出标志
unsigned long int WDT_EXITLPM;

unsigned int adc_num,adc_num1,adc_num2;
float     Vmain = 0;
float     Vbat = 0;

unsigned char GPS_Vmain_Bat;
unsigned int  BAT_V_main;//蓄电池电压 电池
unsigned int  LI_BAT_VOL;//锂电池电压 电池

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

//聊城测试数据
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
extern unsigned char LPM_GPS_LOCATE_OK;//低功耗模式下,GPS定位成功

extern void Init_ADC();
extern void Close_ADC();
extern void VmainMon();

extern void Init_TimerWDT();
extern void Control_Strategy();
extern void Delayms(unsigned long int ms)                           ;//XX*1Ms延时
extern void Open_Box_Check(void);

void Init_TimerWDT()
{
    // 使能WDT中断
    SFRIE1  |= WDTIE;

    //WDTHOLD  Watchdog timer Start.
    //WDTSSEL0 Watchdog timer clock source select ACLK
    //WDTTMSEL 设置WDT为Interval timer mode  定时模式
    //WDTCNTCL Watchdog timer counter clear.

    //计时模式
    //WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0;                   // 18'12'16'' 2^31 2G/32K     = 65536s by 32.768K
    //WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS0;            //  1'08'16'' 2^27 128M/32K   = 4096s by 32.768K
    //WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS1;            //     4'16'' 2^23 8192k/32K  = 256s by 32.768K
    //WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS1+WDTIS0;     //       16'' 2^19 512K/32K   = 16s by 32.768K

    WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K
    //WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2+WDTIS0;           // 2^13   8192/32K  = 250ms by 32.768K
    //WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2+WDTIS1;           // 2^9    512/32K   = 15.6ms by 32.768K
    //WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2+WDTIS1+WDTIS0;    // 2^6    64/32K    = 1.95ms by 32.768K
    //看门狗模式
    //WDTCTL = WDTPW+WDTCNTCL;  //看门够清除
    //WDTCTL = WDTPW+WDTCNTCL+WDTSSEL0+WDTIS2
    
}


//***************************************************************************//
//                                                                           //
//函数：void Init_ADC(void)                                                  //
//说明：初始化ADC12                                                          //
//P6.3电瓶电压 ADC12MEM0                                                                        //
//P6.4锂电池电压
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
    //采样保持1 SHT1
    //多次采样/转换位 MSC
    //SHP = 1  MSC = 0 每次转换需要SHI信号的上升沿触发采集定时器
    //CONSE!=0 MSC = 1 仅首次转换同上，而后采样转换将在前一次转换完成之后立即进行
    //内部参考电压 REF2_5V 0 1.5V 1 2.5V
    //参考电压控制 REFON 0 关闭 1打开
    //ADC12内核控制位 ADC12ON 0 关闭内核 1 打开内核
    //溢出中断允许 ADC12OVIE 0 无溢出 1 溢出
    //转换时间溢出 ADC12TVIE 0 无溢出 1 溢出
    //转换允许位 ENC 0 ADC12 处于初始模式 1 启动
    //采样转换控制位 ADC12SC
    //ENC = 1 SHP = 1  (1) ADC12SC 由0变为1 启动ADC转换 (2)转换完成后ADC12SC自动复位
    //ISSH = 0SHP = 0 (1) ADC12SC 保持高电平时采集  (2) ADC12SC 复位时启动一次转换
    //单次转换
    //ADC12CTL0   = ADC12ON+ADC12SHT02+ADC12REFON+ADC12REF2_5V; // 打开ADC12, 采样时钟源为内部发生器，参考源为内部2.5V
    //序列转换
    ADC12CTL0   = ADC12ON+ADC12SHT02+ADC12REFON+ADC12REF2_5V+ADC12MSC; // 打开ADC12, 采样时钟源为内部发生器，参考源为内部2.5V

    //转换存储地址定义
    //采样输入信号选择 SHSx 00 ADC12SC 01 Timer_A.OUT1 10 Timer_B.OUT0 11 Timer_B.OUT1
    //采样信号控制位选择 SHP 0 SAMPCON直接源自采样输入信号 1 SAMPCON 直接源自采样定时器，由采样输入信号的上升沿触发采样定时器
    //采样输入信号方向 ISSH  0 采样输入信号 同向输入  1 采样输入信号 反向输入
    //ADC12时钟源分频 ADC12DIV 000 1/1 ... 111 1/8
    //ADC12时钟源选择 ADC12SEL 00 内部时钟 01 ACLK 10 MCLK 11 SMCLK
    //转换模式选择 CONSEQ 00单通道单次 01 序列通道单次 10 单通道多次 11 序列通道多次
    //ADC12忙标志 ADC12BUSY 0 无活动 1 正处于 采样、转换、序列转换
    //单次转换
    //ADC12CTL1   = ADC12SHP                                  ; // 打开内部采样时钟
    //序列转换
    ADC12CTL1   = ADC12SHP+ADC12CONSEQ0                       ; // 打开内部采样时钟

    //序列结束控制 EOS 0序列没有结束 1此序列最后一次转换
    //参考电压源选择 SREFx 0
    //选择模拟输入通道 INCHx 0~7  8 Veref+ 9 Vref- 10 片内温度传感器 11~15 (AVcc-AVss)/2

    ADC12MCTL0  = ADC12SREF_1+ ADC12INCH_3                 ; // Vr+=Vref+ and Vr-=AVss  P6.3电瓶电压
    ADC12MCTL1  = ADC12SREF_1+ ADC12INCH_4+ADC12EOS        ; // Vr+=Vref+ and Vr-=AVss 最后一个通道  P6.4锂电池电压

    __delay_cycles(20000)                                ; // Delay for reference start-up

    //ADC12IE =ADC12IE3 ;                                     //ADC12IFG.x 中断使能
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
//函数：void VmainMon(void)                                                  //
//说明：检测主电源电压，需做偏差补偿和平均处理                               //
//
//变量说明: ArrDataBuffer[AD_CNT],放采集的数据
//
//   13.08.26                                                                //
//***************************************************************************//
void VmainMon(void)                                             //检测主电源电压及锂电池电压   
{
	unsigned char i,j,k,l;
	float sum=0,sum_Vbat=0;
	float ArrDataBuffer[AD_CNT],ArrDataBuffer_Vbat[AD_CNT],Temp;
	
	
    ADC12CTL0   |= ADC12SC;//ADC12SC由0-->1启动转换；转换完成后ADC12SC自动复位
    while (!(ADC12IFG & BIT0));

	//测锂电池电压
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
	
	
	for(j=0;j<AD_CNT-1;j++)//冒泡排序
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
	
	for(l=1;l<AD_CNT-1;l++)//去掉最大值和最小值后的和
	{
		sum_Vbat+=ArrDataBuffer_Vbat[l];
	}
	
	Vbat=sum_Vbat/(AD_CNT-2);
	
	
	//测电瓶电压
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
	
    
	for(j=0;j<AD_CNT-1;j++)//冒泡排序
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

    for(l=1;l<AD_CNT-1;l++)//去掉最大值和最小值后的和
	{
		sum+=ArrDataBuffer[l];
	}
	
	Vmain=sum/(AD_CNT-2);
	
	GPS_Vmain_Bat=(int)Vmain;
	
    BAT_V_main=(int)(Vmain*6.667);
    LI_BAT_VOL=(int)(Vbat*50.0);
}





//------------------------------------------------------------------------------
//定时唤醒
//24V使用
//------------------------------------------------------------------------------
//24V
/*void Control_Strategy()
{
    //LPM 低功耗转换策略
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
        //GPS_Inf[14]|=0x03                                              ;//GPS模式 0x03 =休眠
        //WDT_EXITLPM=MIN15;
		LPM = 3;
		WDT_EXITLPM=MIN90;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K

    } 
	else if(Vmain>21) 
    {
        //BATMODE =3;
        //GPS_Inf[14]|=0x03                                              ;//GPS模式 0x03 =休眠
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
        //关闭电池电源
         //GPS_Inf[14]|=0x04                                              ;//GPS模式 0x04=深度睡眠
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
//定时唤醒
//12V使用
//------------------------------------------------------------------------------
//12V
/*void Control_Strategy()
{
    //LPM 低功耗转换策略
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
        //GPS_Inf[14]|=0x03                                              ;//GPS模式 0x03 =休眠
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
        //关闭电池电源
         //GPS_Inf[14]|=0x04                                              ;//GPS模式 0x04=深度睡眠
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
//定时唤醒
//兼容程序
//------------------------------------------------------------------------------
//兼容程序
void Control_Strategy()
{
    //LPM 低功耗转换策略
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
        //GPS_Inf[14]|=0x03                                              ;//GPS模式 0x03 =休眠
        //WDT_EXITLPM=MIN15;
		LPM = 3;
		WDT_EXITLPM=HOUR24;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K

    } 
	else if(Vmain>LOW_POW.MODE_4) 
    {
        //BATMODE =3;
        //GPS_Inf[14]|=0x03                                              ;//GPS模式 0x03 =休眠
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
        //关闭电池电源
         //GPS_Inf[14]|=0x04                                              ;//GPS模式 0x04=深度睡眠
        //BATMODE =7;
		 //WDT_EXITLPM=HOUR48;
        //WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0;                   // 18'12'16'' 2^31 2G/32K     = 65536s by 32.768K
		//WDT_EXITLPM=MIN25;
        LPM = 4;
		WDT_EXITLPM=HOUR120;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K
    }
}
//兼容程序
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//定时唤醒
//测试使用
//------------------------------------------------------------------------------
//测试使用
/*
void Control_Strategy()
{
    //LPM 低功耗转换策略
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
        //GPS_Inf[14]|=0x03                                              ;//GPS模式 0x03 =休眠
        //WDT_EXITLPM=MIN15;
		LPM = 3;
		WDT_EXITLPM=MIN90;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K

    } 
	else if(Vmain>21) 
    {
        //BATMODE =3;
        //GPS_Inf[14]|=0x03                                              ;//GPS模式 0x03 =休眠
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
        //关闭电池电源
         //GPS_Inf[14]|=0x04                                              ;//GPS模式 0x04=深度睡眠
        //BATMODE =7;
		 //WDT_EXITLPM=HOUR48;
        //WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0;                   // 18'12'16'' 2^31 2G/32K     = 65536s by 32.768K
		//WDT_EXITLPM=MIN25;
        LPM = 4;
		WDT_EXITLPM=MIN3;
		WDTCTL   =WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL0+WDTIS2;                  // 1 2^15 32/32K    = 1s by 32.768K
    }
}*/
//测试使用
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer (void)
{
    //喂狗
    //P1OUT  = ~P1OUT                               ; //  P1.0 Reset

    if(WDT_EXITLPM==WDTnum++) {
        WDTnum=0;
        //这里的逻辑是 定时唤醒 (唤醒时间由策略进行转换)
        //每次唤醒如果是低功耗模式 则退出
        //如果是AM模式则切换指示灯状态指示
        if(LPM) 
        {
            if(((P2IN&PWRON)) && LPM) 
            {
                //退出低功耗模式LPM = 0; 蒋本庆 08.28修改测试低功耗
                LPM = 0;
                //关闭 上电、震动、开盒
            }
            LPM3_EXIT; // 退出低功耗模式
            
            
			//GPS_WDT_CNT=0;
        } 
        else 
        {
            //运行指示灯
            //P1OUT  = ~P1OUT                               ; //  P1.0 Reset
        }
    }
	

    Open_Box_Check();
	if(GPS_Start_FLG)
      
	{
		if(GPS_Out_Time==GPS_WDT_CNT++)
		{
			GPS_WDT_CNT=0;
			LPM3_EXIT                                     ; // 退出低功耗模式
			LPM_GPS_Stop=1;
			GPS_Start_FLG=0;
			LPM_GPS_LOCATE_OK=1;
		}
	}
	
	Shake_Time_CNT++;
	if(Shake_Time_CNT==1800)//30分钟
	{
		Shake_Time_CNT=0;
		Init_Shake();
		LPM_SHAKE_CNT=0;
	}
    
    
    
    
    
	/*if(LPM_GPS_LOCATE_OK==1)//李凯9.12添加，
	{
		LPM3_EXIT                                     ; // 退出低功耗模式
	}*/
}









