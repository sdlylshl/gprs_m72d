#include "msp430x54x.h"

// ��ʱ������ѡ���ѡ���ڲ�DCO���ⲿ��Ƶ����

#define  FLL_FACTOR         749
// FLL_FACTOR: DCO��Ƶϵ��  Ĭ��31 ���1023
//32.014336 DCORSEL_ 6 7
#define  FLL_FACTOR_32M     976
//25.001984 DCORSEL_ 5 6 7
#define  FLL_FACTOR_25M     762
//24.018944 DCORSEL_ 5 6 7
#define  FLL_FACTOR_24M     732
//20.021248 DCORSEL_ 5 6 7
#define  FLL_FACTOR_20M     610
//16.023552 DCORSEL_ 4 5 6
#define  FLL_FACTOR_16M     488
//12.025856 DCORSEL_ 4 5 6
#define  FLL_FACTOR_12M     366
//8.02816   DCORSEL_ 3 4 5
#define  FLL_FACTOR_8M      244
//4.030464  DCORSEL_ 2 3 4
#define  FLL_FACTOR_4M      122
//2.031616  DCORSEL_ 1 2 3
#define  FLL_FACTOR_2M      61
//1.015808  DCORSEL_ 0 1 2
#define  FLL_FACTOR_1M      30

//Clock
extern void Init_CLK(void);
extern void Init_EXTERNAL_HF_OSC_CLK(void);

//***************************************************************************//
//                                                                           //
//                 ��ʼ����ʱ��: MCLK = XT1��(FLL_FACTOR+1)     16Mhz             //
//                                                                           //
//***************************************************************************//
void Init_CLK(void)
{
    WDTCTL     = WDTPW + WDTHOLD                            ; // �ؿ��Ź�
    P7SEL     |= 0x03                                       ; // �˿�ѡ���ⲿ��Ƶ����XT1
    UCSCTL6   &=~XT1OFF                                     ; // ʹ���ⲿ����
    UCSCTL6   |= XCAP_3                                     ; // �����ڲ����ص���
    UCSCTL3   |= SELREF_0                                   ; // DCOref = REFO
    UCSCTL4   |= SELA_0                                     ; // ACLK   = XT1
    __bis_SR_register(SCG0)                                 ; // �ر�FLL���ƻ�·
    UCSCTL0    = 0x0000                                     ; // ����DCOx, MODx
    UCSCTL1    = DCORSEL_5                                  ; // ����DCO�񵴷�Χ
    UCSCTL2    = FLLD__1 + FLL_FACTOR_16M                       ; // Fdco = ( FLL_FACTOR + 1)��FLLRef = (649 + 1) * 32768 = 21.2992MHz
    __bic_SR_register(SCG0)                                 ; // ��FLL���ƻ�·
    __delay_cycles(1024000)                                 ;
    do {
        UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG); // ��� XT2,XT1,DCO �����־
        SFRIFG1 &= ~OFIFG                                     ;
    } while(SFRIFG1&OFIFG)                                   ; // ������������־

    // ACK��MCK��SMCK�����P11.0/1/2
    /*   //������
     P11DS  = TACK + TMCK + TSMCK                            ;
     P11SEL = TACK + TMCK + TSMCK                            ;
     P11DIR = TACK + TMCK + TSMCK                            ;

     while(1);
     */
}

//***************************************************************************//
//                                                                           //
//                       ��ʼ����ʱ��: MCLK = XT2                            //
//                                                                           //
//***************************************************************************//
void Init_EXTERNAL_HF_OSC_CLK(void)
{
    WDTCTL     = WDTPW + WDTHOLD                            ; // �ؿ��Ź�
    P5SEL     |= 0x0C                                       ; // �˿ڹ���ѡ������
    UCSCTL6   &= ~XT2OFF                                    ; // ����ʹ��
    UCSCTL3   |= SELREF_2                                   ; // FLLref = REFO
    UCSCTL4   |= SELA_2                                     ; // ACLK=REFO,SMCLK=DCO,MCLK=DCO
    do {
        UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG); // ��� XT2,XT1,DCO �����־
        SFRIFG1 &= ~OFIFG                                     ;
    } while(SFRIFG1&OFIFG)                                   ; // ������������־
    UCSCTL6   &= ~XT2DRIVE0                                 ; // XT2 ����ģʽ 16~24MHz  Ĭ�� 24~32M
    UCSCTL4   |= SELS_5 + SELM_5                            ; // SMCLK = MCLK = XT2
}
