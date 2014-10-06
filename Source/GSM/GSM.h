#ifndef __GSM_H__
#define __GSM_H__


#define LPM3_Open_Send_Time    5
#define  PWRON BIT0


#define SECD_1    1024
#define SECD_2    2048
#define SECD_3    3072
#define SECD_5    5120
#define SECD_10   10240
#define SECD_20   20480
#define SECD_30   30720
#define SECD_50   51200

#define MSP_A0_Min_1    61440
#define MSP_A0_Min_2    122880
#define MSP_A0_Min_3    184320
#define MSP_A0_Min_5    307200
#define MSP_A0_Min_10   614400
#define MSP_A0_Min_20   1228800
#define MSP_A0_Min_30   1843200

#define MSP_Hour_1   3686400
#define MSP_Hour_2   7372800
#define MSP_Hour_3   11059200


#define MSP_Hour_10  36864000
#define MSP_Hour_20  73728000
#define MSP_Hour_30  110592000  

#define GPS_Model_Bug  0x03   //11 GPS��ģ����ϡ���ʾ��
#define GPS_No_Signal  0x0C   //11 GPS���źţ�
#define GPS_Ant_Open   0x30   //11 GPS�����߼���/δ�ӡ���
#define GPS_Position   0xC0   //11 GPS��λ��־��
/*******************************************************************\
*	      �ļ�����GSM_Brust.c      //ͻ�����ݴ���
\*******************************************************************/
#define Receive_Gprs_Data    0x01
#define GSM_Get_GPS    0x03

#define  NET_Up_Speed_BUF_SIZE 10
extern char REC_Up_Speed_BUF[NET_Up_Speed_BUF_SIZE];//���������� �޿�����
extern char LPM3_Open_Box_Gsm_Flag;

extern char* Low_Pow_Time_Addr   ;//�͹���ʱ�������ñ�־��ַ



extern unsigned char GPS_Vmain_Bat;//���ص�ѹ ���
extern unsigned char LPM                                       ;//�͹���ģʽ
extern unsigned char GSM_STATUS                                         ;
extern unsigned int M72D_RX_Buf_Cnt                                     ;                             
extern unsigned char packet_type                                        ;
#define PACKET_BRUST 0x00
#define M72D_Brust_RX_BUF_SIZE 80                         
extern unsigned char M72D_Brust_RX_Buf[M72D_Brust_RX_BUF_SIZE]          ;
extern unsigned char M72D_Brust_STATUS                                  ; 
#define PACKET_Query 0x01
#define M72D_Query_RX_BUF_SIZE 50                                       //���ݴ�������ֽ���
extern unsigned char M72D_Query_RX_Buf[M72D_Query_RX_BUF_SIZE]          ;
extern unsigned char M72D_Query_STATUS                                  ;//���ð�������� 0 ����Ӧ 0x01 OK 0x02 ERRO 0x04 �������
#define PACKET_CONFIG 0x02
#define M72D_SetConfig_RX_BUF_SIZE 50                         
extern unsigned char M72D_SetConfig_RX_Buf[M72D_SetConfig_RX_BUF_SIZE]  ;
extern unsigned char M72D_SetConfig_STATUS                              ; 
#define PAKET_DATA 0x04
#define M72D_ServerData_RX_BUF_SIZE 1200                         
extern unsigned char M72D_ServerData_RX_Buf[M72D_ServerData_RX_BUF_SIZE];
extern unsigned char M72D_ServerData_STATUS                             ; 
#define PACKET_NETCONFIG 0x08
#define M72D_NetConfig_RX_BUF_SIZE 100                        
extern unsigned char M72D_NetConfig_RX_Buf[M72D_NetConfig_RX_BUF_SIZE]  ;
extern unsigned char M72D_NetConfig_STATUS                              ;//������������ 0 ����Ӧ 0x01 OK 0x02 ERRO 0x04 �������
#define PCKET_OTHER 0x10
#define M72D_Other_RX_BUF_SIZE 100                         
extern unsigned char M72D_Other_RX_Buf[M72D_Other_RX_BUF_SIZE]          ;
extern unsigned char M72D_Other_STATUS                                  ; 


#define OUT_EDGE_BUF_SZ 25                                               //Խ�籨���������û����С
extern char NET_OUT_EDGE_BUF[OUT_EDGE_BUF_SZ]                           ;//ƽ̨�·�Խ�籨������
#define  MCU_TIME_BUF_SZ 12
extern char REC_MCU_TIME_BUF[MCU_TIME_BUF_SZ]                           ;//����MCU�����ϴ�ʱ��������
#define  WORK_MODE_BUF_SZ 29
extern char REC_WORK_MODE_BUF[WORK_MODE_BUF_SZ]                         ;//����MCU�����ϴ�ʱ��������
#define  NET_IP_PORT_BUF_SIZE 14
extern char REC_IP_PORT_BUF[NET_IP_PORT_BUF_SIZE]                       ;//����IP�˿ں�
#define  NET_LOOK_M_BUF_SIZE 10
extern char REC_LOOK_M_BUF[NET_LOOK_M_BUF_SIZE]                         ;//���� GPS�ն�����/��������
#define  CALL_MCU_BUF_SIZE 12
extern char  REC_CALL_MCU_BUF[CALL_MCU_BUF_SIZE]                        ;//����MCU�����ϴ�ʱ��������

extern char UDP_Built_STR[41]                                           ;//ɽ�ؽ���GPS����ƽ̨�˿�
extern char GSM_MCU[62]                                                 ;//��ʾ��MCU���ݻ���
extern char GPS_Inf[28]                                                 ;//��ʾ��ƽ̨GPS���ݻ���
extern char GSM_Send_MCU_Buf[106]                                        ;//�ϱ�MCU��������(�ظ�)

extern char UP_Root_Stat_BuF[29]                                        ;//������ط�����������ָ��
extern char Module_Status[6]                                            ;//ģ��״̬��ϢGPS��Ϣ+GSM��Ϣ+ϵͳ״̬1��2+����״̬1��2
extern char GPS_GSM_System_Stu[4]                                       ;//һ���״̬�洢
extern char GPS_GSM_ID_Memory[4]                                        ;//һ����豸ID�ն˺�
#define UP_SYSTEM_C_SIZE 1200
extern unsigned char UP_SYSTEM_C_LANG[UP_SYSTEM_C_SIZE]                         ;//����������
#define  GSM_GSP_BUF_SIZE 40
extern char  GSM_GSP_BUF[GSM_GSP_BUF_SIZE];//GSM���վ�γ��ʱ����Ϣ����



//****************************************************************************//
extern int GSM_strstr (const char * Main_str,const char * search_str)   ;//GSM_String//�ַ�������
extern int GSM_strlen (const char * str)                                ;//�ַ�������
extern int GSM_strcmp (const char * src,const char * dst)               ;//�ַ����Ƚ�
extern char * GSM_strsep (char **stringp, const char *delim)            ;//�ַ����ָ�
extern void GSM_ltoa(char offset, char*ps,unsigned long src)            ;//����ת�ַ���
extern long GSM_atol( const char * str )                                ;//�ַ���ת����
extern unsigned char GSM_SendCMD(char paktype, const char * s,
                                 unsigned int Len,int ms)               ;//GSM_CMD
extern char GSM_SendData(const char * Data,int num)                     ;
extern unsigned char ASCIITOHEX( unsigned char Input_H,
                                unsigned char Input_L );//��ASCII�ַ�ת��Ϊ16���Ƶ���
extern unsigned int crc_modbus2(unsigned char *puchMsg, 
                                unsigned int usDataLen)                 ;//CRC
extern char Compare_String(char *Source,char *Target,unsigned int Count);//�Ƚ������ַ����Ƿ���ͬ
extern void Flash_WriteChar(unsigned int Addr,unsigned char Data)       ;//��Flash��д��һ���ֽ�
extern void Tran_ID_CmdNum(char* ID_Addr,char CmdNum)                   ;//ת��ID�ź��������                              
extern void Delayms(unsigned long int ms)                               ;//XX*1Ms��ʱ
extern unsigned char HEX_TO_ASCII(char One_Char)                        ;//16�����ַ�ת��ASCII
extern void Tran_ID_CmdNum(char* ID_Addr,char CmdNum)                   ;//ת��ID�ź��������                              
extern char SAVE_SIM_NUM_FH(unsigned char CMD_LEN)                      ;//�洢�ֻ����� 
extern void SIM_CHG_WARN(void)                        ;//�����ֻ������� 
extern void Do_GSM_GPS_Data(void);//����GSM����GPS����

extern char GSM_Lock_Monitor()                                          ;//0x03==������0x00���� 
extern char IP_Port_Reset()                                             ;//����IP��ַ\Port��  
extern char Work_Mode_Set()                                             ;//ϵͳ���й���ģʽ����
extern char Root_UP_Net_Begin(void)                                     ;//���ط�����������ָ��
extern char TCPIP_BASIC_SET(void)                                       ;
extern char SIM_NUM_CHECK(void)                                         ;
extern char GSM_CREG_Init()                                             ;
extern char GSM_GPRS_Init()                                             ;
extern char GSM_TCPIP_Init()                                            ;
extern void GSM_NOM_POW_ON(void)                                            ;//GSM_State
extern void GSM_Sleep()                                                 ;
extern void GSM_Stop()                                                  ;
extern void GSM_State()                                                 ;
extern void DO_ROOT_UP(void)                                            ;//����Զ����������
extern char UP_A2_NUM_CRC(void)                                         ;//��������CRCУ��
extern void GSM()                                                       ;
extern char GSM_Heart_Beat()                                            ;//GSM_HEART_BEAT.C
extern char GSM_Send_MCU(void)                                              ;//GSM_Send_MCU_Data.c
extern void RecData()                                                   ;
extern char MCU_Time_Set()                                              ;//MCU�����ϴ�ʱ�������� 
extern char Call_MCU_Data(void)                                         ;//����GPS�ն��ϱ���λ��������
extern void SHUT_GPS_UCA1(void)                                         ;//�ر�GPS�ж�
extern void Init_UCA0UART(void)                                         ;//GPS�жϳ�ʼ��
extern void RE_START_GSM(void)                                          ;//����ϵͳ
extern char CONNECT_FAIL_RESET(void)                                    ;//����Conect_Fail����
extern char OUT_EDGE_SET(void)                                          ;//Խ�籨����������
extern char ANSW_CALL_MCU(void)                                         ;//�ظ��ϱ���λ��������
extern void RD_MCU_TIME_FLASH(void)                                     ;//��ȡʱ������FLASH
extern void GSM_NOM_POW_OFF(void)                                       ;
extern void SHUT_GSM_UCA0(void)                                         ;
extern void GPS_Stop()                                                  ;
extern char GSM_Brust()                                                     ;//ͻ�����ݴ���
extern void Open_Box_Check(void);
extern void Write_Open_Box_Flash(void);//���м�ⱨ��Flash
extern void Send_SIM_Card_NUM(void);//���������ֻ�����
extern void Change_SIM_Card_NUM(void);//��������ֻ�����
extern void Send_GSM_GPS_AT_Cmd(void);
extern void VmainMon(void);//�������Դ��ѹ��﮵�ص�ѹ   
extern unsigned char GSM_SIM_Signal();//�ź�����ǿ��
extern void ExitLMP_SendMCU(void);//�˳��͹��ķ���MCU���� 
extern char CHECK_MCU_WARN(void);//���MCU���� 
extern char Allow_Up_Speed(void);//���������� �޿�����
extern void READ_UP_SPI_FLASH(void); //ת������汾�ż���������


extern unsigned int RD_UCA0_Time                                             ;
//extern void GSM_Value_Init(void);


extern unsigned long int Soft_Vers_Cnt                                  ;//��������汾��ʱ���ʱ
extern unsigned long int RD_Data_1_Min_Cnt                              ;//���� MUC Ӧ��ʱ�����
extern unsigned long int HeartBeat_TimeOut                              ;//��������������ʱ��  61425
extern unsigned long int MCU_Data_TimeOut                               ;//�������ݰ�����ʱ��   
extern unsigned long int MCU_ACK_TimeOut                                ;//1min ��ѯӦ��� ��Ӧ�����ط�
extern unsigned long int MCU_TimeOut                                    ;//���ݰ�����ʱ�� 
extern unsigned long int Heart_Beat_Send_Cnt                          ;//�������ʹ�������
extern unsigned long int THR_Mint_Time_Cnt                              ;//3.5����ʱ����������ڲ��ϴ����ݼ��
extern unsigned int  GSM_TimeOut_1ms                                    ;//Timer
extern unsigned long int Heart_Beat_Count                               ;//��������1msʱ�����
extern unsigned long int MCU_Data_Count                                 ;//1ms ���ݰ�����
extern unsigned long int CALL_MCU_TIME_CNT                              ;//ʱ�����
extern unsigned long int GSM_INIT_TIME_CNT                              ;//GSM��ʼ����ʱ��
extern unsigned int SIM_CARD_TIME_OUT                              ;//SIM����ʼ����ʱ��
extern unsigned long int RD_Data_Time                                   ;//���� MUC Ӧ��ʱ�����
extern unsigned long int RD_Data_1_Min_Cnt                                        ;//���� MUC Ӧ��ʱ�����
extern unsigned long int SIM_Card_Work_Cnt;//�������ֻ�����ʱ���ʱ
extern unsigned long int No_SIM_CARD_Cnt;//��ⲻ��SIM����ʼ������
extern unsigned long GSM_GPS_AT_Cmd_Cnt;//����GSM��ȡ������Ϣ����
extern unsigned long int ExitLMP_Time_Cnt;//�˳��͹���ʱ����
extern unsigned long int One_JI_Warn_Cnt;//һ����������
extern unsigned long int Two_JI_Warn_Cnt;//������������
extern unsigned long int One_Degree_Alert_Time;//1����������


extern char MSP430_POWER_ON_FLAG;//11=�ϵ磻55=�ϵ�
extern unsigned char GPS_Data_OK_Flag                                     ;//��ʹ��GPS���ݴ�����ɱ�־ 00==δ��ɣ�11==���
extern unsigned char Soft_VER_OK                                        ;//����汾��(��ʾ���������������ŵ������汾��)
extern unsigned char UP_SPI_Num_Flag                                           ;//11==ARM��Ҫ����(����������,ÿ���ֽ���)
extern unsigned char CONNECT_FAIL_Flag                                  ;//CONNECT FAIL����ʹ��
extern unsigned char POWER_ON_FLAG                                      ;//11=�ػ���55=����
extern unsigned char UDP_Built_flag                                     ;//11==���罨����00==����Ͽ�����û�н���
extern unsigned char UDP_Built_flag                                     ;//11==���罨����00==����Ͽ�����û�н���
extern unsigned char GSM_ECHO                                           ;//���ô򿪻��� 0x80
extern char GSM_SIM_Iint_Sig_Num                                    ;



extern unsigned int  BAT_V_main                               ;//���ص�ѹ ���
extern unsigned int  LI_BAT_VOL                               ;//﮵�ص�ѹ ���


extern char SIM_Card_ER_Flag;//�����ֻ�����ʧ�ܱ�־ �ط�
extern char SD_SOFT_V_ER_Flag                                           ;//��������汾��ʧ�ܱ�־ �ط�
extern char ARM_DATA_STU                                                ;//������Ч��־
extern char MCU_Data_ACK                                                ;//���������ݱ�־λ  
extern char LOW_POW_GSM_Flag                                            ;//��ʹ��GPS���ݴ�����ɱ�־ 00==δ��ɣ�11==���
extern char JG_OUT_EDGE_Flag                                            ;//Խ�籨���жϱ�־λ 11=�����ж�
extern char Sys_NOW_UP_Flag;//ϵͳ����������־ 11==������ 55 δ����
extern char Open_Box_Flag;
extern char SIM_Card_Work_Flag;//�ϵ��ֻ����ű�־
extern char Change_SIM_Card_Flag;//�ϵ�����ֻ����ű�־
extern void Read_No_Chg_Card_Flash(void);//�������޿�Flash 
extern void Write_No_Chg_Card_Flash(void);//д�����޿�Flash 
extern void RD_Up_Speed_FLASH(void);//���������� �޿�����
extern char Send_Soft_Version(void)                                 ;//��������汾��

extern unsigned int CREG_Zhu_Ce_Cnt ;//ע�������ѯע�����


/**********************************************************************\
    extern void Test_IP_Port(void);����IP��ַ\Port��  
\**********************************************************************/
extern void Ceshi_Allow_Up_Speed(void);//���Դ��������� �޿�����

#endif