
#include "msp430x54x.h"


unsigned char RD_FLASH_OK_FLAG                                      ;//读取flash标志11==OK；00==fail


extern char Module_Status[6]                                        ;//模块状态信息GPS信息+GSM信息+系统状态1、2+升级状态1、2
extern char GPS_GSM_System_Stu[4]                                   ;//GSM\GPS状态
extern char Heart_Beat[10]                                          ;//心跳数据包
extern char GPS_GSM_ID_Memory[4]                                    ;//一体机设备ID终端号
extern char GSM_Send_MCU_Buf[106]                                        ;//上报MCU数据命令(回复)


extern void Delayms(unsigned long int ms)                           ;//XX*1Ms延时
extern void Tran_ID_CmdNum(char* ID_Addr,char CmdNum)                   ;//转存ID号和命令编码                              


extern void Read_No_Chg_Card_Flash(void);//读换卡无卡Flash 
extern void READ_SIM_NUM_FH(void)                                   ;//存储手机卡号 
extern void ReaD_IP_PORT_FLASH(void)                                ;//读取IP地址\Port号 
extern char RD_OUT_EDGE_FH(void)                                    ;//读取越界报警 
extern void READ_UP_SPI_FLASH(void)                                 ;//转移软件版本号及升级类型
extern void RD_LOCK_BUS_FLASH(void)                                 ;//读取锁车状态
extern void Read_Open_Box_Flash(void);
/********************************************************\
*	 函数名：Delayms
     作用域：外部文件调用
*	 功能：  延时1ms
*	 参数：  
*    返回值：无
*	 修改历史：（每条详述）
\********************************************************/
void Delayms(unsigned long int ms)
{
    unsigned int n;                               //10 Cylce
    for(n=0; n<ms; n++)                           //(N+1)Cylce
        __delay_cycles(15996);                        //(n+3)Cylce
}



//*******************************************************************//
//                                                                   //
//      函数：Read_Flash_Data                                        //
//      说明：读取FLASH存取的设置数据数据                            //
//      参数：无                                                     //
//                                                                   //
//*******************************************************************//
void Read_Flash_Data(void)
{     
    unsigned char ID_NUM                                            ;

    char* RD_FH_HD_AD   =  (char *)0x1800                           ;//flash读取首地址
    char* RD_ID_Addr    =  (char *)0x1801                           ;//ID首地址
    RD_FLASH_OK_FLAG     = 0x00                                     ;//置位读取flash标志
    
    if(*(RD_FH_HD_AD)==0x01)
    {
        RD_FLASH_OK_FLAG     = 0x11                                 ;//读取flash OK；
        for(ID_NUM=0;ID_NUM<4;ID_NUM++)                              //ID
        {   
            GPS_GSM_ID_Memory[ID_NUM]  = *(RD_ID_Addr)++            ; 
        }
        
        ReaD_IP_PORT_FLASH()                                        ;//读取IP地址\Port号
        
        Tran_ID_CmdNum(Heart_Beat,0xB2)                             ;//转存ID号和命令编码 
        Tran_ID_CmdNum(GSM_Send_MCU_Buf,0x81)                        ;//转存ID号和命令编码

   }    
        
        RD_LOCK_BUS_FLASH()                                         ;//读取锁车状态
        READ_SIM_NUM_FH()                                           ;//存储手机卡号
        RD_OUT_EDGE_FH()                                            ;//读取越界报警 
        READ_UP_SPI_FLASH()                                         ;//转移软件版本号及升级类型
        Read_Open_Box_Flash();
        Read_No_Chg_Card_Flash();//读换卡无卡Flash 
}


/*******************************************************************\
*	      函数名：Do_GPS_Data             
*	      作用域：外部文件调用
*	      功能：  接收、处理GPS数据  
*	      参数：  
           1、通过控制GPS数据类型（00=无响应；11=GPRMC；22=GPGGA）控制步骤
           2、Do_GPS_Mesg_OK_Flag：GPS数据处理完成标志 00==未完成；11==完成
           3、Open_UCA1_UART_Init(void)==用接收完成数据不可用时打开GPS中断
*	      返回值：无     
*
*	      修改历史：（每条详述）
\*******************************************************************/
void Tran_ID_CmdNum(char* ID_Addr,char CmdNum)                       //转存ID号和命令编码
{
    char ID_Num                                                     ;//
    char* ID_CmdNum_Addr;
    
    ID_CmdNum_Addr=ID_Addr;
    for(ID_Num=0;ID_Num<4;ID_Num++,ID_CmdNum_Addr++)               
    {
        *ID_CmdNum_Addr    =     GPS_GSM_ID_Memory[ID_Num]                 ;
    }
    
    *(ID_CmdNum_Addr++)    =     CmdNum                 ;
    *(ID_CmdNum_Addr++)    =     CmdNum                 ;
}


/********************************************************\
*	函数名：ASCIITOHEX
    作用域：外部文件调用
*	功能：   将ASCII字符转换为16进制的数
             比如0x31,0x32转换完后返回的结果为12
*	参数：  Input_H==ASCII高位
            Input_L==ASCII低位
*	返回值：Input_H*16+Input_L
            
*	修改历史：（每条详述）
\********************************************************/
unsigned char ASCIITOHEX( unsigned char Input_H,unsigned char Input_L )//将ASCII字符转换为16进制的数
{
    if( Input_H>0x39 ) Input_H=Input_H-0x37;
    else Input_H=Input_H-0x30;
    if( Input_L>0x39 ) Input_L=Input_L-0x37;
    else Input_L=Input_L-0x30;
    return( Input_H*16+Input_L );
}


/********************************************************\
*	 函数名：HEX_TO_ASCII
     作用域：外部文件调用
*	 功能：  把一个16进制字符转换为ASCII
*	 参数：  Input_H，Input_L
*        返回值：One_Char 一个ASCII数
*	 修改历史：（每条详述）
\********************************************************/
unsigned char HEX_TO_ASCII(char One_Char)                //16进制字符转换ASCII
{
    if(((One_Char>0)&&(One_Char<=9))||(One_Char==0))
        One_Char += 0x30;
    else One_Char = 0xff;
    return One_Char; 
}



/********************************************************\
*	函数名：Compare_String
   1    作用域：外部文件调用
*	功能：  比较两个字符串是否相同
*	参数：  *Source 需要比较的字符串的的头指针
                *Target 用来做比较标准的字符串头指针
                 Count  需要比较的长

*	返回值：0   表示比较结果不相同
                1   表示比较结果相同
*	修改历史：（每条详述）
\********************************************************/
char Compare_String(char *Source,
                             char *Target,
                             unsigned int Count )         // 1 比较两个字符串是否相同
{
    while(Count>0)
    {
        if((*Source)!=(*Target)) return(0);
        Source++;
        Target++;
        Count--;
    }
    return(1);
}


/********************************************************\
*	函数名：Compare_String
   1    作用域：外部文件调用
*	功能：  比较两个字符串是否相同
*	参数：  *Source 需要比较的字符串的的头指针
                *Target 用来做比较标准的字符串头指针
                 Count  需要比较的长

*	返回值：0   表示比较结果不相同
                1   表示比较结果相同
*	修改历史：（每条详述）
\********************************************************/
char COMP_Peice_STR(char *Source,char *Target,unsigned int T_Cnt,unsigned int S_Cnt,
                             unsigned int Count )         // 1 比较两个字符串是否相同
{
    Source=Source+S_Cnt;
    Target=Target+T_Cnt;
    while(Count>0)
    {
        if((*Source)!=(*Target)) return(0);
        Source++;
        Target++;
        Count--;
    }
    return(1);
}
