#include "msp430x54x.h"

// 主时钟设置选项，可选择内部DCO或外部高频晶振

#define  FLL_FACTOR         749
// FLL_FACTOR: DCO倍频系数  默认31 最大1023
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
//                 初始化主时钟: MCLK = XT1×(FLL_FACTOR+1)     16Mhz             //
//                                                                           //
//***************************************************************************//
void Init_CLK(void)
{
    WDTCTL     = WDTPW + WDTHOLD                            ; // 关看门狗
    P7SEL     |= 0x03                                       ; // 端口选择外部低频晶振XT1
    UCSCTL6   &=~XT1OFF                                     ; // 使能外部晶振
    UCSCTL6   |= XCAP_3                                     ; // 设置内部负载电容
    UCSCTL3   |= SELREF_0                                   ; // DCOref = REFO
    UCSCTL4   |= SELA_0                                     ; // ACLK   = XT1
    __bis_SR_register(SCG0)                                 ; // 关闭FLL控制回路
    UCSCTL0    = 0x0000                                     ; // 设置DCOx, MODx
    UCSCTL1    = DCORSEL_5                                  ; // 设置DCO振荡范围
    UCSCTL2    = FLLD__1 + FLL_FACTOR_16M                       ; // Fdco = ( FLL_FACTOR + 1)×FLLRef = (649 + 1) * 32768 = 21.2992MHz
    __bic_SR_register(SCG0)                                 ; // 打开FLL控制回路
    __delay_cycles(1024000)                                 ;
    do {
        UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG); // 清除 XT2,XT1,DCO 错误标志
        SFRIFG1 &= ~OFIFG                                     ;
    } while(SFRIFG1&OFIFG)                                   ; // 检测振荡器错误标志

    // ACK、MCK、SMCK输出至P11.0/1/2
    /*   //测试用
     P11DS  = TACK + TMCK + TSMCK                            ;
     P11SEL = TACK + TMCK + TSMCK                            ;
     P11DIR = TACK + TMCK + TSMCK                            ;

     while(1);
     */
}

//***************************************************************************//
//                                                                           //
//                       初始化主时钟: MCLK = XT2                            //
//                                                                           //
//***************************************************************************//
void Init_EXTERNAL_HF_OSC_CLK(void)
{
    WDTCTL     = WDTPW + WDTHOLD                            ; // 关看门狗
    P5SEL     |= 0x0C                                       ; // 端口功能选择振荡器
    UCSCTL6   &= ~XT2OFF                                    ; // 振荡器使能
    UCSCTL3   |= SELREF_2                                   ; // FLLref = REFO
    UCSCTL4   |= SELA_2                                     ; // ACLK=REFO,SMCLK=DCO,MCLK=DCO
    do {
        UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG); // 清除 XT2,XT1,DCO 错误标志
        SFRIFG1 &= ~OFIFG                                     ;
    } while(SFRIFG1&OFIFG)                                   ; // 检测振荡器错误标志
    UCSCTL6   &= ~XT2DRIVE0                                 ; // XT2 驱动模式 16~24MHz  默认 24~32M
    UCSCTL4   |= SELS_5 + SELM_5                            ; // SMCLK = MCLK = XT2
}
