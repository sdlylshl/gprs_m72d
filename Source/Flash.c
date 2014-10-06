#include "msp430x54x.h"


//***************************************************************************//
//                                                                           //
//函数：void ERASE(unsigned int)                                             //
//说明：擦除片内FLASH数据段                                                  //
//参数：address ——擦除数据段地址                                           //
//                                                                           //
//***************************************************************************//
void FERASE(unsigned int address)
{
    unsigned int *Erase_address                             ;
    Erase_address  = (unsigned int *)address                ;
    FCTL1          = FWKEY + ERASE                          ;
    FCTL3          = FWKEY                                  ;
    *Erase_address = 0                                      ;
    FCTL1          = FWKEY                                  ;
    FCTL3          = FWKEY + LOCK                           ;
}

//***************************************************************************//
//                                                                           //
//函数：void ERASE(unsigned int)                                             //
//说明：擦除片内FLASH数据                                                    //
//参数：address ——擦除起始地址                                             //
//                                                                           //
//***************************************************************************//
void FWRITE(unsigned int address, unsigned char* data,unsigned int length)
{
    unsigned int   i                                        ;
    unsigned char* Wr_Addr                                  ;
    _DINT();
    Wr_Addr =(unsigned char*)address                        ;
    FCTL1   = FWKEY + WRT                                   ; // Set WRT bit for write operation
    FCTL3   = FWKEY                                         ;
    for(i=0; i<length; i++) {
        *Wr_Addr= data[i]                                     ;
        Wr_Addr++                                             ;
    }
    FCTL1   = FWKEY                                         ;
    FCTL3   = FWKEY + LOCK                                  ;
    _EINT();
}

