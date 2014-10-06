#include"Common.H"
#include"CHeck.H"

// --------------------------------------------------------------------------

UINT8  code  AT[]           = "AT\r";               
UINT8  code  ATE0[]         = "ATE0\r";             
UINT8  code  ATV1[]         = "ATV1\r";            
UINT8  code  AT_SLEEP_OFF[] = "AT%SLEEP=0\r";       
UINT8  code  TSIM[]         = "AT%TSIM\r";         
UINT8  code  cAT_CREG[]     = "AT+CREG=1\r";
UINT8  code  rAT_CREG[]     = "AT+CREG?\r";
UINT8  code  cAT_CGREG[]    = "AT+CGREG=1\r";
UINT8  code  rAT_CGREG[]    = "AT+CGREG?\r";         
UINT8  code  AT_COPS[]      = "AT+COPS?\r";          
UINT8  code  AT_CSQ[]       = "AT+CSQ\r";           
UINT8  code  AT_W[]         = "AT&W1\r";           
UINT8  code  AT_CGDCONT[]   = "AT+CGDCONT=1,\"IP\",\"CMNET\"\r";  
//UINT8  code  ETCPIP[]       = "AT%ETCPIP=,,1\r";
UINT8  code  ETCPIP[]       = "AT%ETCPIP\r";
UINT8  code  IOMODE[]       = "AT%IOMODE=1,1,1\r";
UINT8  code  IPSEND[]       = "%IPSEND:1,";

//UINT8  code  IPOPEN[41]     = "AT%IPOPEN=\"TCP\",\"060.213.047.105\",08080\r"; 错误的IP地址，用于实验
//UINT8  code  IPOPEN[41]     = "AT%IPOPEN=\"TCP\",\"221.215.099.081\",08102\r"; //40字符，末尾'\0' 1个，共41个,IP从第17数组地址开始
//UINT8  code  IPOPEN[41]       = "AT%IPOPEN=\"TCP\",\"060.213.047.151\",09123\r"; //40字符，末尾'\0' 1个，共41个,IP从第17数组地址开始
UINT8  code  IPOPEN[41]     = "AT%IPOPEN=\"TCP\",\"219.143.116.144\",02012\r";
UINT8        IPOPENRAM[40];   // 联网用
UINT8        IPOPENRAM1[40];  // 设置用
UINT8        IPOPENRAM2[40];  // 备份用

UINT8  code  cIPCLOSE1[]    = "AT%IPCLOSE=1\r";
UINT8  code  cIPCLOSE5[]    = "AT%IPCLOSE=5\r";

UINT8        ATRAM[50];

UINT8  code  ATIPSEND[]     = "AT%IPSEND=\"";

UINT8  code  IPCLOSE1[]     = "%IPCLOSE: 1";  
UINT8  code  IPCLOSE5[]     = "%IPCLOSE: 5"; 
UINT8        IPCLOSE1RAM[11];
UINT8        IPCLOSE5RAM[11];

UINT8  code  DataRcvHead[]  = "%IPDATA:";  
UINT8        DataRcvHeadRAM[8];

UINT8  code  INFODATAD[]    = "INFODATA";
UINT8  code  infodatax[]    = "infodata";
UINT8        INFODATADRAM[8], infodataxram[8];

UINT8  code  ascTable[]     = "0123456789ABCDEF";

// --------------------------------------------------------------------------

          UINT8   linkStep; 
volatile  UINT8   GPRSOnLineFlag;       // GPRS在线标记
volatile  UINT8   GPRSTimCntSec;        // GPRS超时重启
          UINT8   NetLinkOneTimeFlag;   // GPRS 上电联网标记
 
 // --------------------------------------------------------------------------
           
volatile  UINT8   GPRSRcvStep;
volatile  UINT8   GPRSDataStartFlag;    // 告诉数据接收是否开始受到%ipdata 
         
volatile  UINT8   GPRSRcvTimStart;             
volatile  UINT16  GPRSRcvTimCnt; 

volatile  UINT8   GPRSDecodeStartCmd;
          UINT8   GPRS_Rcv_Cmd; 

// --------------------------------------------------------------------------

volatile  UINT16  GPRSRcvNum;           // 接收到HEX字符的数量,ASCII数量 == 2 * GPRSRcvNum;
volatile  UINT8   GPRSRcvDataPos;          

// --------------------------------------------------------------------------

          UINT8   GPRS_Snd_Cmd; 
volatile  UINT8   GPRSSendLength;
volatile  UINT8   GPRSSndCountIsr;      // GPRS 发送计数器

volatile  UINT8   GPRS_Rcv_Count_Cmd;
volatile  UINT8   GPRS_Rcv_Count;               
          UINT8   GPRS_Rcv_Shadow_Buf[GPRS_Buf_Length];     
          UINT8   GPRS_Snd_Shadow_Buf[GPRS_Buf_Length];  
          UINT8   GPRS_Snd_Buf       [GPRS_Buf_Length];         
          UINT8   GPRS_Rcv_Buf       [GPRS_Buf_Length]; 
          UINT8   GPRS_Rcv_Buf_Cmd[GPRS_Buf_Length]; 

// --------------------------------------------------------------------------

volatile  UINT8   GPRSCmdType;
          UINT8   GPRSCmdStep;
          
// --------------------------------------------------------------------------    
          
          UINT8   ATStep;               // AT命令执行的步骤
volatile  UINT16  ATTimCnt;             // 10mS为单位，AT命令执行的计时
          UINT8   ATExpectStr[8];       // 期望得到的字符串
          UINT8   ATExpectStrLength;    // 期望得到的字符串长度

          UINT8   TxWindows;                     // 这个变量还没利用，但这个值已赋，在其他模块可以直接使用。
volatile  UINT8   TxWindowsTimCnt;
          UINT8   IPSENDRAM[10];

// --------------------------------------------------------------------------
 
extern              UINT8   MutexFlag; 
extern              UINT8   LEDCntReLoadValue;
extern    volatile  UINT8   SndSvrDataDelayTimCnt;   // 数据延时发送SendDataBlockToServer()
extern    volatile  UINT8   GPRSPacketForbidFlag;
extern    volatile  UINT8   VMCSndToGPRS_RcvTimeOverCnt_Sec;
extern    volatile  UINT8   RequestSndTimCnt;

extern              UINT8   VMC_79_Buf[15];  
extern              UINT8   VMC_7A_Buf[15];
extern              UINT8   VMC_7B_Buf[15];
extern              UINT8   VMC_7D_Buf[35];

extern    void    Delay10Ms(UINT8  cnt);
UINT8  StrSearch(UINT8  *a, UINT8  *b, UINT8  aLength, UINT8  bLength);

void  Clear_GPRS_Rcv_Buf(void)
{
	UINT8  i;
	
	for (i=0; i < GPRS_Buf_Length; i++)
	{
		GPRS_Rcv_Buf[i] = 0;    
	}                                      
}

void  Clear_GPRS_Rcv_Buf_Cmd(void)
{
    UINT8  i;
    
    for(i=0; i<GPRS_Buf_Length; i++) // 200
    {
        GPRS_Rcv_Buf_Cmd[i] = 0;                                
    }
}

void  EM310_Turn(void)
{
    TERM_ON; TERM_ON;
    Delay10Ms(7);       // 高脉冲70mS来开机
    TERM_OFF;TERM_OFF;  // 一直开机防止干扰脉冲关机
}

void   GPRSInit(void)
{ 
    GPRSCmdType        =  _GPRS_CMD_NO; 
    GPRSOnLineFlag     =  _NO;
    GPRSTimCntSec      =  0;                
    NetLinkOneTimeFlag =  _FIRSTLINK;    
    
    linkStep  = 0;                
    GPRSDecodeStartCmd  = 0;
}

void   GPRSRcvInit(void)
{   
    DisableInterrupt;
    
    GPRSRcvTimStart    = 0;  
    
    GPRSDataStartFlag  = 0;
    GPRSRcvStep        = 0;
    
    GPRS_Rcv_Cmd       = 0;  
    GPRSDecodeStartCmd = 0;  
    
    Clear_GPRS_Rcv_Buf();
    GPRS_Rcv_Count  = 0; 
    
    EnableInterrupt;
}

void   SendDataBlockToGPRS(UINT8  *sStr, UINT8  length)
{
    UINT8  i;
    
    for(i=1; i<length; i++)
    {
        GPRS_Snd_Buf[i] = sStr[i];
    }
    
    GPRSSendLength  = length - 1;
    GPRSSndCountIsr = 1;
    
    SBUF1 = sStr[0];
}

void   SendDataBlockToServer(UINT32 VMCAddress, UINT16 packetNo, UINT8 dataType, UINT8 *dataBlock, UINT8 dataLength)
{
    UINT8  i;
    
    for(i=0; i<8; i++)
    {
        GPRS_Snd_Shadow_Buf[i] = INFODATAD[i];
    }
    
	GPRS_Snd_Shadow_Buf[8]  = dataLength;
	
	GPRS_Snd_Shadow_Buf[9]  = (UINT8)(VMCAddress>>24);
	GPRS_Snd_Shadow_Buf[10] = (UINT8)(VMCAddress>>16);
	GPRS_Snd_Shadow_Buf[11] = (UINT8)(VMCAddress>>8);
	GPRS_Snd_Shadow_Buf[12] = (UINT8)(VMCAddress);
	
	GPRS_Snd_Shadow_Buf[13] = packetNo;
	GPRS_Snd_Shadow_Buf[14] = dataType;
	Mymemcpy(&GPRS_Snd_Shadow_Buf[15], dataBlock, dataLength);
	GPRS_Snd_Shadow_Buf[15 + dataLength] = XORCheckGenerate(GPRS_Snd_Shadow_Buf, 15+dataLength);
	
	GPRS_Snd_Cmd   = 1;
    GPRSSendLength = 16 + dataLength;
}

void   Hex2Ascii(UINT8  * source, UINT8  * target, UINT8  length)
{
    UINT8    *pData = target;  
    UINT16    pos;
    UINT8     i;
    
    pos = 0;
    for(i=0; i<length; i++)
    {
        pData[pos++] = ascTable[source[i]>>4];
        pData[pos++] = ascTable[source[i]&0x0F];
    }
    
    pData[pos] = '\0';
}

void   Ascii2Hex(UINT8  * source, UINT8  * target, UINT8   length)
{
    UINT8   i, j, k;
    UINT8   tmpData;
    
    UINT8   * pSource = source;
    UINT8   * pTarget = target;
    
    for(i=0; i<length; i++)
    {
        if( (pSource[i]>='0') && (pSource[i]<='9') )
        {
            tmpData= pSource[i] - '0';
        }
        else if( (pSource[i]>='A') && (pSource[i]<='F') )
        {
            tmpData= pSource[i] - 0x37;  // 'A' = 65
        }
        else if( (pSource[i]>='a') && (pSource[i]<='f') )
        {
            tmpData= pSource[i] - 0x57;  // 'a' = 97
        }
        
        pSource[i] = tmpData;
    }
        
    for(j=0, k=0; j<i; j+=2)
    {
        pTarget[k++] = ((pSource[j]<<4) | (pSource[j+1]));
    }
}

UINT8  SendATThenCheck(UINT8 code * at, UINT8  atLength, UINT16  timCnt)
{
    UINT8  i;
    
    switch(ATStep)
    {
        case 0:
	        Clear_GPRS_Rcv_Buf_Cmd();  //发送命令之前，先清除接收缓冲区
	        GPRS_Rcv_Count_Cmd = 0;
	        
		    for(i=0; i<atLength; i++)
		    {
		        ATRAM[i] = at[i];
		    }
		    
	        SendDataBlockToGPRS(ATRAM, atLength);
	        
	        ATTimCnt = 0;
	        ATStep++;     
	        break;
	    case 1:
	        DisableInterrupt;
	        
	        if(ATTimCnt < 35)
	        {
	            EnableInterrupt;
	        }
	        else
	        {
	        	EnableInterrupt;
	        	
	            ATTimCnt = 0;
		        ATStep++;
		    } 
		    break;
		case 2:
		    
		    DisableInterrupt;
		    
		    if(ATTimCnt < timCnt)
        	{
        	    EnableInterrupt;
        	    
        	    //if(GPRS_Rcv_Count_Cmd > ATExpectStrLength)  已在StrSearch()函数中界定
	        	//{
	        		if(StrSearch(GPRS_Rcv_Buf_Cmd, ATExpectStr, GPRS_Rcv_Count_Cmd, ATExpectStrLength) != 0)
	        	    {
	        	        ATStep = 0;
	        	        return _OK;
	        	    }
	        	    else
	        	    {
	        	        return  _UNDERWAY;
	        	    }
	        	//}
	        	//else
	        	//{
	        	//	return  _UNDERWAY;
	        	//}
            }
            else
            {
                EnableInterrupt;
                
                ATStep = 0;
                //GPRS_Error_AT;
                return  _ERROR;
            }
    		break;
        default:
           ATStep = 0;
           break;
    }
    
    return  _UNDERWAY;
}

UINT8  SendATThenCheckRAM(UINT8  * at, UINT8  atLength, UINT16  timCnt)
{
    switch(ATStep)
    {
        case 0:
	        Clear_GPRS_Rcv_Buf_Cmd();  //发送命令之前，先清除接收缓冲区
	        GPRS_Rcv_Count_Cmd = 0;
	        
	        SendDataBlockToGPRS(at, atLength);
	        
	        ATTimCnt = 0;
	        ATStep++;     
	        break;
	    case 1:
	        DisableInterrupt;
	        
	        if(ATTimCnt < 35)
	        {
	            EnableInterrupt;
	        }
	        else
	        {
	        	EnableInterrupt;
	        	
	            ATTimCnt = 0;
		        ATStep++;
		    }
		    break;
		case 2:		    
		    DisableInterrupt;
		    
		    if(ATTimCnt < timCnt)
        	{
        	    EnableInterrupt;
        	    
        	    //if(GPRS_Rcv_Count_Cmd > ATExpectStrLength)  已在StrSearch()函数中界定
	        	//{
	        		if(StrSearch(GPRS_Rcv_Buf_Cmd, ATExpectStr, GPRS_Rcv_Count_Cmd, ATExpectStrLength) != 0)
	        	    {
	        	        ATStep = 0;
	        	        return _OK;
	        	    }
	        	    else
	        	    {
	        	        return  _UNDERWAY;
	        	    }
	        	//}
	        	//else
	        	//{
	        	//	return  _UNDERWAY;
	        	//}
            }
            else
            {
                EnableInterrupt;
                
                ATStep = 0;
                //GPRS_Error_AT;
                return  _ERROR;
            }
    		break;
        default:
           ATStep = 0;
		   break;
    }
    
    return  _UNDERWAY;
}

void   ATCmd(void)
{
    UINT8  ret;
    
    if(GPRSCmdType == _GPRS_CMD_InitAndConnect) 
    {   
        switch(GPRSCmdStep)
        {
            case 0:
                ATExpectStr[0] = 'O';
                ATExpectStr[1] = 'K';
                ATExpectStrLength = 2;
                ATStep = 0;
                
                GPRSCmdStep++;
                break;
            case 1:
                ret = SendATThenCheck(ATV1, strlen(ATV1), 300);  
                if(ret == _OK)
                {
                    ATStep = 0;
                    //ret = _ERROR; 局部变量更改无效
                    
                    GPRSCmdStep++;
                }
                else if(ret == _ERROR)
                {
                    GPRSCmdStep = 0;
                }
                break;
            case 2:
                ret = SendATThenCheck(ATE0, strlen(ATE0), 300);
                if(ret == _OK)
                {
                    ATStep = 0;
                    
                    GPRSCmdStep++;
                }
                else if(ret == _ERROR)
                {
                    GPRSCmdStep = 0;
                }
                break;
            case 3:
                ret = SendATThenCheck(AT_SLEEP_OFF,strlen(AT_SLEEP_OFF), 300);
                if(ret == _OK)
                {
                    ATStep = 0;
                    
                    GPRSCmdStep++;
                }
                else if(ret == _ERROR)
                {
                    GPRSCmdStep = 0;
                }
                break;
            case 4:
                ret = SendATThenCheck(IOMODE, strlen(IOMODE), 300);
                if(ret == _OK)
                {
                    ATStep = 0;
                    
                    GPRSCmdStep++;     
                }
                else if(ret == _ERROR)
                {
                    GPRSCmdStep = 0;
                }
                break;
            case 5:
                ret = SendATThenCheck(AT_CGDCONT, strlen(AT_CGDCONT), 3000);
                if(ret == _OK)
                {
                    ATStep = 0;
                    
                    GPRSCmdStep++;
                }
                else if(ret == _ERROR)
                {
                    //GPRSCmdStep = 0;
                    ATStep = 0;
                }
                break;
            case 6:
                ret = SendATThenCheck(ETCPIP, strlen(ETCPIP), 6000);//6000
                if(ret == _OK)
                {
                    ATExpectStr[0] = 'C';
                    ATExpectStr[1] = 'O';
                    ATExpectStr[2] = 'N';
                    ATExpectStr[3] = 'N';
                    ATExpectStr[4] = 'E';
                    ATExpectStr[5] = 'C';
                    ATExpectStr[6] = 'T';
                    ATExpectStrLength = 7;
                    
                    ATStep = 0;
                                        
                    GPRSCmdStep++;
                }
                else if(ret == _ERROR)
                {
                    //GPRSCmdStep = 0;
                    ATStep = 0;
                }
                break;
            case 7:
                ret = SendATThenCheckRAM(IPOPENRAM, 40, 7500);  //7500
                
                if(ret == _OK)
                {
                    GPRSOnLineFlag       = _NOWYES;
                    GPRSCmdType          = _GPRS_CMD_NO; 
                    GPRSCmdStep          = 0;  
                }
                else if(ret == _ERROR)
                {
                    GPRS_IP_Fail;
                    
                    GPRSOnLineFlag       = _NOWBAD;
                    GPRSCmdType          = _GPRS_CMD_NO; // 打开链接的75s内禁止后续操作。
                    GPRSCmdStep          = 0;
                }
                break;
        }
    }
    else if(GPRSCmdType == _GPRS_CMD_UnConnect)
    {
        switch(GPRSCmdStep)
        {
            case 0:
                ATExpectStr[0] = 'O';
                ATExpectStr[1] = 'K';
                ATExpectStrLength = 2; 
                ATStep = 0;
                
                GPRSCmdStep++;
                break;
            case 1:
                ret = SendATThenCheck(cIPCLOSE1, strlen(cIPCLOSE1), 3000);  
                
                if(ret == _OK)
                {
                    ATStep = 0;
                    
                    GPRSCmdStep++;
                }
                else if(ret == _ERROR)
                {
                    GPRSCmdStep = 0;
                }
                break;
            case 2:
                ret = SendATThenCheck(cIPCLOSE5, strlen(cIPCLOSE5), 3000);
                
                if(ret == _OK)
                {
                    GPRSOnLineFlag   = _NO;
                    GPRSCmdType      = _GPRS_CMD_NO; 
                    GPRSCmdStep      = 0;
                }
                else if(ret == _ERROR)
                {
                    GPRSCmdStep = 0;   // 等待其他函数的超时判断
                }
                break;
        }
    }
    else
    {
        GPRSCmdStep = 0;
    }
}

void   GPRSSendData(void)
{
    UINT8  i;
    
    if(GPRS_Snd_Cmd != 0)
    {
    	if(SndSvrDataDelayTimCnt == 0)  // 取得资格之后延时发
    	{
    		;
    	}
    	else
    	{
    		return;
    	}
    	
        for(i=0; i<11; i++)
        {
            GPRS_Snd_Buf[i] = ATIPSEND[i];
        }
    
        Hex2Ascii(GPRS_Snd_Shadow_Buf, &GPRS_Snd_Buf[11], GPRSSendLength);
        GPRS_Snd_Shadow_Buf[14] = 0;
        
        GPRS_Snd_Buf[11+2*GPRSSendLength] = '\"';
        GPRS_Snd_Buf[12+2*GPRSSendLength] = '\r';
        
        GPRSSendLength  = 13 + 2*GPRSSendLength - 1;
        GPRSSndCountIsr = 1;
        
    
        GPRSRcvInit(); 
        GPRS_Snd_Cmd = 0;
        GPRS_Snd_Cmd = 0; 
        
        SBUF1 = GPRS_Snd_Buf[0];   
    }
}

/*-------------------------------------------------
字符串寻找
    字符串包含某字符串时，返回_OK, 只要有 1 个不同，返回_ERROR
    UINT8  ptr: 存储区指针

     返回数据相等的末尾位置
     0 1 2 3 4 5 6 7 8    ===> i = 3
           3 4 5 6 |      ===> j = 4
                   |
                   |
                   |
                   |
          i + j == 7
          
author：wangtai

-------------------------------------------------*/

UINT8  StrSearch(UINT8  *a, UINT8  *b, UINT8  aLength, UINT8  bLength)
{
    UINT8  i, j, n;
    
    if(aLength < bLength)   //  重要条件
    {
    	return  0;
    }
    
    for(i=0; i<(aLength-bLength+1); i++)
    {
        n = 0;
        
        for(j=0; j<bLength; j++)
        {
            if(a[i+j] == b[j])
            {
                n++;
            }
            else
            {
                n = 0;
                break;     
            }
        }
        
        if(n == bLength)
        {
            return  (i+j); // 返回比较字符串的后一个字节的位置
        }    
    }
    
    return  0;            
}

void   CalDataNum(UINT8  colonPos, UINT8   commaPos)
{
    UINT8  pos;
    
    pos = commaPos - colonPos;
    
    switch(pos)
    {
        case 2:
            if( (GPRS_Rcv_Buf[colonPos+1]>='0') && (GPRS_Rcv_Buf[colonPos+1]<='9') )
            {
                GPRSRcvDataPos = commaPos + 2;
                GPRSRcvNum  = GPRS_Rcv_Buf[colonPos+1] - '0';
            }
            else
            {
                GPRSRcvNum  = 0;
            }
            break;
        case 3:
            if( (GPRS_Rcv_Buf[colonPos+1]>='0') && (GPRS_Rcv_Buf[colonPos+1]<='9') && (GPRS_Rcv_Buf[colonPos+2]>='0') && (GPRS_Rcv_Buf[colonPos+2]<='9') )
            {
                GPRSRcvDataPos = commaPos + 2;
                GPRSRcvNum = (GPRS_Rcv_Buf[colonPos+1]-'0')*10 + (GPRS_Rcv_Buf[colonPos+2]-'0');
            }
            else
            {
                GPRSRcvNum  = 0;
            }
            break;
        case 4:
            if( (GPRS_Rcv_Buf[colonPos+1]>='0') && (GPRS_Rcv_Buf[colonPos+1]<='9') && (GPRS_Rcv_Buf[colonPos+2]>='0') && (GPRS_Rcv_Buf[colonPos+2]<='9') && (GPRS_Rcv_Buf[colonPos+3]>='0') && (GPRS_Rcv_Buf[colonPos+3]<='9'))
            {
                GPRSRcvDataPos = commaPos + 2;
                GPRSRcvNum = ((UINT16)GPRS_Rcv_Buf[colonPos+1]-'0')*100 + (GPRS_Rcv_Buf[colonPos+2]-'0')*10 + (GPRS_Rcv_Buf[colonPos+3]-'0');
            }
            else
            {
                GPRSRcvNum  = 0;
            }
            break;
        default:   
            GPRSRcvNum  = 0;
           break;
    }
}

void   GPRSDataExtraction(void)
{
    static UINT8  colonNextPos;   // 冒号   
               
    UINT8  i;
    
    if(GPRS_Rcv_Count < 23)  // %IPDATA:X,"infodata......." // 真正的数据一定大于23
    {
        GPRSDataStartFlag  = 0;   
        GPRSRcvStep        = 0;   
        
        if((GPRS_Rcv_Count > 16)) // %IPSEND:1,XX  经验证冒号后无空格，
        {
        	for(i=0; i<10; i++)
	        {
	            IPSENDRAM[i] = IPSEND[i];
	        }
	        
	        i = StrSearch(GPRS_Rcv_Buf, IPSENDRAM, GPRS_Rcv_Count, 10);
	        
	        if(i != 0)   
	        {  
	            if((GPRS_Rcv_Buf[i+1]>='0')&&(GPRS_Rcv_Buf[i+1]<'6'))
	            {
	            	TxWindows = ( (GPRS_Rcv_Buf[i]-'0')*10 + (GPRS_Rcv_Buf[i+1]-'0') );
	            	GPRSPacketForbidFlag = 0;
	            }
	            else
	            {
	            	TxWindows = GPRS_Rcv_Buf[i]-'0';
	            	
	            	if(TxWindows < 8)
	            	{
	            		TxWindowsTimCnt      = 200;  // 这两个变量的顺序不能弄反
	            		GPRSPacketForbidFlag = 1;    // 后半缓冲区采用慢速发送方式
	            	}
	            	else
	            	{
	            		GPRSPacketForbidFlag = 0;
	            	}
	            }
	            
	            GPRSRcvInit();
	        }
	        else
	        {
	        	;// 可能是数据
	        } 
    	}
    	
    	
        return; 
    }
    else 
    {        
        if(GPRSDataStartFlag == 0)
        {
            for(i=0; i<8; i++)
            {
                DataRcvHeadRAM[i] = DataRcvHead[i];
            }
            
            colonNextPos = StrSearch(GPRS_Rcv_Buf, DataRcvHeadRAM, GPRS_Rcv_Count, 8);
            if(colonNextPos != 0)
            {  
                GPRSDataStartFlag   = 1;
            }   
            else
            {
                GPRSRcvInit();     
                
                return;           
            } 
        }
        else
        {
            switch(GPRSRcvStep)
            {
                case 0: 
                    if((GPRS_Rcv_Count-1) < (colonNextPos+3))
                    {
                        ;
                    }
                    else
                    {
                        GPRSRcvStep++;
                    }
                    break;
                case 1:  
                    if(GPRS_Rcv_Buf[colonNextPos+1] == ',')
                    {
                        CalDataNum(colonNextPos-1, colonNextPos+1);
                    }
                    else if(GPRS_Rcv_Buf[colonNextPos+2] == ',')
                    {
                        CalDataNum(colonNextPos-1, colonNextPos+2);
                    }
                    else if(GPRS_Rcv_Buf[colonNextPos+3] == ',')
                    {
                        CalDataNum(colonNextPos-1, colonNextPos+3);
                    }
                    else
                    {
                        GPRSRcvInit();   
                        return;
                    }
                    
                    if(GPRSRcvNum == 0)
                    {
                        GPRSRcvInit();
                        return;
                    }
                    else
                    {
                        GPRSRcvTimCnt   = (GPRSRcvNum*2) * 5;      
                        GPRSRcvTimStart = 1;                 
                        GPRSRcvStep++;
                    }
                    break;
                case 2: 
                    if((GPRS_Rcv_Count-1) > GPRSRcvDataPos)
                    {
                        for(i=GPRSRcvDataPos; i<GPRS_Rcv_Count; i++)
                        {
                            if(GPRS_Rcv_Buf[i] == '\"')
                            {
                                if(GPRSRcvNum == (i-GPRSRcvDataPos)/2)
                                {
                                    GPRSTimCntSec       = 0;    
                                    GPRSRcvTimStart     = 0;  
                                    GPRSDecodeStartCmd  = 1;  
                                }
                                else
                                {
                                    GPRSRcvInit();
                                    return;
                                }
                            }
                            else
                            {
                                ; 
                            }
                        }
                    }
                    break;
            }

            //---------------------------------------------------------- 超时判断
            
            if(GPRSRcvTimStart != 0)
            {
                if(GPRSRcvTimCnt < 10)                          //本计数器中断中自减,10ms递减，不是1ms递减
                {
                    GPRSRcvInit();
                    return;
                }
                else
                {
                    ;
                }
            }           
        }
    }
}

void   GPRSDeCodeData(void)
{
    if(GPRSDecodeStartCmd != 0)  
    {
        Ascii2Hex(&GPRS_Rcv_Buf[GPRSRcvDataPos], GPRS_Rcv_Shadow_Buf, GPRSRcvNum*2); // 接收到的ascii是数据量的2倍
        
        GPRSRcvInit();           // 发送接收后都复位

        GPRS_Rcv_Cmd       = 1;
        GPRSDecodeStartCmd = 0;        
        
        if( (GPRS_Rcv_Shadow_Buf[14]!=0x75) && (GPRS_Rcv_Shadow_Buf[14]!=0x76) && (GPRS_Rcv_Shadow_Buf[14]!=0x7C) && (GPRS_Rcv_Shadow_Buf[14]!=0x7E) )
        {
            GPRS_Rcv_Cmd  = 0;  
        }
        
        if(MutexHave79)
        {
            if( (GPRS_Rcv_Shadow_Buf[0]==105) && (GPRS_Rcv_Shadow_Buf[1]==110) && (GPRS_Rcv_Shadow_Buf[14]==0x79) )
            {
                MutexRel;
                VMC_79_Buf[0] = 0xF9;
                RequestSndTimCnt = 20;
            }
        }
        else if(MutexHave7A)
        {
            if( (GPRS_Rcv_Shadow_Buf[0]==105) && (GPRS_Rcv_Shadow_Buf[1]==110) && (GPRS_Rcv_Shadow_Buf[14]==0x7A) )
            {
                MutexRel;
                VMC_7A_Buf[0] = 0xFA;
                RequestSndTimCnt = 20;
            } 
        }
        else if(MutexHave7B)
        {
            if( (GPRS_Rcv_Shadow_Buf[0]==105) && (GPRS_Rcv_Shadow_Buf[1]==110) && (GPRS_Rcv_Shadow_Buf[14]==0x7B) )
            {
                MutexRel;
                VMC_7B_Buf[0] = 0xFB;
                RequestSndTimCnt = 20;
            }   
        } 
        else if(MutexHave7D)
        {
            if( (GPRS_Rcv_Shadow_Buf[0]==105) && (GPRS_Rcv_Shadow_Buf[1]==110) && (GPRS_Rcv_Shadow_Buf[14]==0x7D) )
            {
                MutexRel;
                VMC_7D_Buf[0] = 0xFD;
                RequestSndTimCnt = 20;
            }   
        } 
    }
    else
    {
        //-------------------------------------------------------------
        
        if(MutexHave79)
        {
            if(VMCSndToGPRS_RcvTimeOverCnt_Sec > 200)
            {
                MutexRel;
                VMC_79_Buf[0] = 0xF9;
            }
        }
        else if(MutexHave7A)
        {
            if(VMCSndToGPRS_RcvTimeOverCnt_Sec > 200)
            {
                MutexRel;
                VMC_7A_Buf[0] = 0xFA;
            }
        }
        else if(MutexHave7B)
        {
            if(VMCSndToGPRS_RcvTimeOverCnt_Sec > 200)
            {
                MutexRel;
                VMC_7B_Buf[0] = 0xFB;
            }
        }
        else if(MutexHave7D)
        {
            if(VMCSndToGPRS_RcvTimeOverCnt_Sec > 200)
            {
                MutexRel;
                VMC_7D_Buf[0] = 0xFD;
            }
        } 
    }
}

void   GPRSLink(void)
{
	if( NetLinkOneTimeFlag == _FIRSTLINK )
    {
	    NetLinkOneTimeFlag = _NODETERMINE; 
	    GPRSTimCntSec  = 61;               // 转换条件
	    
	    linkStep       = 2;
	    GPRS_Connect_ING;                 
		GPRSCmdType    = _GPRS_CMD_NO;     // 防止EM310断电时还执行指令
		GPRSOnLineFlag = _NODETERMINE;     // 防止掉线后直接到这里，清除上线标记
    }
    
    if( GPRSTimCntSec > 60 )     
    {
        switch(linkStep)
        {
            case 0: 
                GPRSTimCntSec  = 61;  // 箝制定时器，防止意外，虽然其基本是41
                GPRSOnLineFlag = _NODETERMINE;
                GPRSCmdType    = _GPRS_CMD_UnConnect;
                
                GPRS_Connect_ING;    
                linkStep++;
                break;
            case 1: 
                if(GPRSOnLineFlag == _NO)
                {
                    GPRSCmdType   = _GPRS_CMD_NO;   //必须要
                    linkStep++;    
                }
                else
                {
                    if(GPRSTimCntSec > 67)
                    {
                        GPRSCmdType   = _GPRS_CMD_NO;//必须要，防止CASE2,3开机等待时，还在执行。
                        linkStep++;  
                    }
                    else
                    {
                        ;
                    }
                }
                break;
            case 2: 
                LDO_POWER_OFF;
                GPRSTimCntSec = 61;
                
                linkStep++;
                break;
            case 3:
                if(GPRSTimCntSec > 65)
                {
                    LDO_POWER_ON;
                }
                
                if(GPRSTimCntSec > 67)
                {
                    EM310_Turn();
                    GPRSTimCntSec = 61;
                    
                    linkStep++;
                }
                break;
            case 4: 
                if(GPRSTimCntSec > 81)
                {
                    GPRSCmdType = _GPRS_CMD_InitAndConnect;
                    GPRSTimCntSec = 61;
                    
                    linkStep++;
                }
                else
                {
                    ;
                }
                break;
            case 5:
                if(GPRSOnLineFlag == _NOWYES)
                {
                    GPRSOnLineFlag = _YES;
                    
                    //linkStep       = 0;
                    GPRSTimCntSec  = 0;
                      
                    //MutexFlag = 74;        // MutexFlag被占用的时候突然掉线，这里重赋值   
                    //MutexRel;    
                    MutexFlag = 85;          // 上线之后强制发送心跳命令，以进行时间同步
                    GPRS_Connect_OK; 
                    
                    GPRS_Rcv_Count  = 0;   // 避免断网命令导致网络监控起作用
                }
                else
                {
                    if( (GPRSTimCntSec>91) || (GPRSOnLineFlag==_NOWBAD))
                    {
                        GPRSTimCntSec      = 61;    
                        NetLinkOneTimeFlag = _FIRSTLINK;
                        
                        GPRSCmdType        = _GPRS_CMD_NO; // >140的情况
                    }
                    else
                    {
                        ;
                    }
                }
                break;
        }
    }
    else
    {
        linkStep  =  0; 
    }
    
    ATCmd();
}

void   NetMonitor(void)
{   
	UINT8  i;
	
	if( GPRSTimCntSec < 60 )  // 没有超时才监控，否则影响GPRSLink()的断网命令
	{
		for(i=0; i<11; i++)
	    {
	        IPCLOSE1RAM[i] = IPCLOSE1[i];
	        IPCLOSE5RAM[i] = IPCLOSE5[i];
	    }
	    
	    if(GPRS_Rcv_Count > 10)
	    {
	        if((StrSearch(GPRS_Rcv_Buf, IPCLOSE1RAM, GPRS_Rcv_Count, 11) != 0) || (StrSearch(GPRS_Rcv_Buf, IPCLOSE5RAM, GPRS_Rcv_Count, 11) != 0))
	        {
	        	GPRSOnLineFlag = _NO;
	        	GPRSCmdType    = _GPRS_CMD_NO;
	            GPRSTimCntSec  = 31;  // 后台断开连接，等30s再连接
	            GPRSRcvInit();        // 防止残渣内容一致钳制时钟在 GPRSTimCntSec == 41, 使重联无法启动
	        }
	    }
	}
}

/* ------------------------------------- The End -------------------------------------------------*/