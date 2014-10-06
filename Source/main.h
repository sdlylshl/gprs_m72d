









//*****************************************************************************//
                        //GSM
//extern unsigned char GSM_STATUS                         ;
//0 正常 0x01关机 0x02休眠 0x04 SIM卡 0x08 GSM网络正常 0x10GPRS网络正常 0x20 TCPIP正常
//extern char *SIM_AT_QCCID_Query                         ;//AT+CPIN  输入 PIN
//extern char GSM_CREG_Init()                             ;//+CREG: 0,1网络初始化
extern void GSM_UCA0_Init(void)                                     ;
extern void LPM_GSM_Globl_IN(void)                                  ;
extern void LPM_GSM_Globl_OUT(void)                                 ;
//*****************************************************************************//
                        //SPI
extern void SPI( void )                                             ;//通过SPI端口发送数据
extern void Init_UCB2SPI(void)                                      ;//设置SPI端口 




//*****************************************************************************//
                        //GPS


extern char LOW_POW_GPS_Flag                            ;//先使用GPS数据处理完成标志 00==未完成；11==完成
extern unsigned char GPS_LOCATE_OK                      ;//GPS定位标志
extern void Init_UCA1UART(void)                         ;//GPS初始化开启
extern void Close_UCA1UART(void)                        ;//GPS初始化关闭

extern void GPS_Start()                                 ;//GPS电源开启
extern void GPS_Stop()                                  ;//GPS电源关闭
extern void SHUT_GPS_UCA1(void)                         ;//关闭GPS中断
extern void Do_GPS_Data(void)                           ;//接收、处理GPS数据
//*****************************************************************************//





//*****************************************************************************//
                        //MSP_430
unsigned char LPM                                       ;//低功耗模式
extern unsigned char RD_FLASH_OK_FLAG                   ;//读取flash标志11==OK；00==fail

void Time_Count_Value_Init(void)                        ;//时间计数清零
extern void Init_CLK(void)                              ;//初始化主时钟: MCLK = XT1
extern void Init_EXTERNAL_HF_OSC_CLK(void)              ;//初始化主时钟: MCLK = XT2  
extern void Init_ADC()                                  ;//初始化ADC12    
extern void Close_ADC()                                 ;//关闭ADC12 
extern void VmainMon()                                  ;//检测主电源电压，需做偏差补偿和平均处理    
extern void Control_Strategy()                          ;//LPM 低功耗转换策略
extern void Init_TimerWDT()                             ;//使能WDT中断
extern void TIME_A0_Init(void)                      ;//设置TimerA0













