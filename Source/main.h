









//*****************************************************************************//
                        //GSM
//extern unsigned char GSM_STATUS                         ;
//0 ���� 0x01�ػ� 0x02���� 0x04 SIM�� 0x08 GSM�������� 0x10GPRS�������� 0x20 TCPIP����
//extern char *SIM_AT_QCCID_Query                         ;//AT+CPIN  ���� PIN
//extern char GSM_CREG_Init()                             ;//+CREG: 0,1�����ʼ��
extern void GSM_UCA0_Init(void)                                     ;
extern void LPM_GSM_Globl_IN(void)                                  ;
extern void LPM_GSM_Globl_OUT(void)                                 ;
//*****************************************************************************//
                        //SPI
extern void SPI( void )                                             ;//ͨ��SPI�˿ڷ�������
extern void Init_UCB2SPI(void)                                      ;//����SPI�˿� 




//*****************************************************************************//
                        //GPS


extern char LOW_POW_GPS_Flag                            ;//��ʹ��GPS���ݴ�����ɱ�־ 00==δ��ɣ�11==���
extern unsigned char GPS_LOCATE_OK                      ;//GPS��λ��־
extern void Init_UCA1UART(void)                         ;//GPS��ʼ������
extern void Close_UCA1UART(void)                        ;//GPS��ʼ���ر�

extern void GPS_Start()                                 ;//GPS��Դ����
extern void GPS_Stop()                                  ;//GPS��Դ�ر�
extern void SHUT_GPS_UCA1(void)                         ;//�ر�GPS�ж�
extern void Do_GPS_Data(void)                           ;//���ա�����GPS����
//*****************************************************************************//





//*****************************************************************************//
                        //MSP_430
unsigned char LPM                                       ;//�͹���ģʽ
extern unsigned char RD_FLASH_OK_FLAG                   ;//��ȡflash��־11==OK��00==fail

void Time_Count_Value_Init(void)                        ;//ʱ���������
extern void Init_CLK(void)                              ;//��ʼ����ʱ��: MCLK = XT1
extern void Init_EXTERNAL_HF_OSC_CLK(void)              ;//��ʼ����ʱ��: MCLK = XT2  
extern void Init_ADC()                                  ;//��ʼ��ADC12    
extern void Close_ADC()                                 ;//�ر�ADC12 
extern void VmainMon()                                  ;//�������Դ��ѹ������ƫ�����ƽ������    
extern void Control_Strategy()                          ;//LPM �͹���ת������
extern void Init_TimerWDT()                             ;//ʹ��WDT�ж�
extern void TIME_A0_Init(void)                      ;//����TimerA0













