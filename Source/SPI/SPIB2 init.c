#include"SPIB2 init.h"

/***************************************************************************
                    Init_SPI(void): 设置SPIB端口                                             
***************************************************************************/
void Init_UCB2SPI(void)
{
	P9SEL|=0x0E;//P9.1，2，3,功能选择为SPI
	UCB2CTL1|=UCSWRST;//复位SPI状态机
	UCB2CTL0|=UCSYNC+UCMSB;//3-pin, 8-bit SPI slave， MSB                                      
	UCB2CTL1&=~UCSWRST;//**Initialize USCI state machine**
	UCB2IE|=UCRXIE ;//Enable USCI_A0 RX interrupt  
}