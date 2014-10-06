#include "msp430x54x.h"


//***************************************************************************//
//                                                                           //
//������void ERASE(unsigned int)                                             //
//˵��������Ƭ��FLASH���ݶ�                                                  //
//������address �����������ݶε�ַ                                           //
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
//������void ERASE(unsigned int)                                             //
//˵��������Ƭ��FLASH����                                                    //
//������address ����������ʼ��ַ                                             //
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

