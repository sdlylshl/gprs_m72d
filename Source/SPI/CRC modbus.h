#ifndef __CRC_modbus_H__
#define __CRC_modbus_H__
#define U16 unsigned int

#include "msp430x54x.h"


//unsigned long CRC(unsigned char* data_value,unsigned int Data_length);

U16 crc_modbus2(unsigned char *puchMsg, unsigned int usDataLen);

#endif