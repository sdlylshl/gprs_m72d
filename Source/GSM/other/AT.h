#ifndef __M72D_H__
#define __M72D_H__
//ϵͳ�����ļ�
//GPRS ���ջ����С
#define GPRS_BUF_SIZE 560         //���ݴ�������ֽ���
#define GPRS_SVRCMD_SIZE 64       //�������·�ָ�����ֵ

extern unsigned char GPRS_RX_Buffer[GPRS_BUF_SIZE];      //USCI_A3�жϽ�������
extern unsigned int  GPRS_RX_Buffer_Cnt;                 //USCI_A3�жϽ�������ͳ��

extern unsigned char GPRS_RX_BufferA[GPRS_BUF_SIZE];      //USCI_A3���ݽ�������
extern unsigned char GPRS_RX_SVRCMD_BufferA[GPRS_SVRCMD_SIZE];   //�������·�ָ��� ��ȡƽ̨�·���������
extern unsigned char GPRS_R_SVRCMD_Flag;                    //ƽ̨�·������־

extern unsigned char M72_TCPIP_OK;
extern unsigned char M72_UDP_OK;
//���������ͱ�־ 1���Է��� 0 ������
extern unsigned char GPS_HEART_BEAT_OK;
//���ݰ����ͱ�־ 1���Է���
extern unsigned char GPS_SEND_DATA_OK;

extern unsigned long GPRS_Heart_Beat_Index;  //ÿ���ӷ���������ʱ
extern unsigned long Heart_Beat_Cnt;                                     //��������ʱ������
extern unsigned long GPRS_Data_Cnt;                                      //���ݰ����䶨ʱ������
//���ݰ�����ʱ��������
extern unsigned long GPS_DATA_TIME;
// AT+CPIN  ���� PIN 
char * SIM_AT_CPIN = "AT+CPIN";

//AT+QCCID ��ȡSIM��ID CCID
//AT+QCCID
//89860022150959D05635
//OK
//AT+QCCID?
//ERROR
char * SIM_AT_QCCID_Query = "AT+QCCID";

//AT+CIMI  ��������ƶ�̨�豸��ʶ��IMSI�� 
//AT+CIMI?
//+CIMI: "460023209180635"
//OK
//AT+CIMI
//460023209180635
//OK
char * SIM_AT_CIMI_Query = "AT+CIMI";

//AT+CREG ����ע����Ϣ<stat>  0   δע�᣻ME��ǰû��������Ҫע��ҵ�����Ӫ���� 
  // 1   ��ע�ᣬ������ 
  // 2   δע�ᣬ�� ME ��������Ҫע��ҵ�����Ӫ���� 
  // 3   ע�ᱻ�ܾ� 
  // 4   δ֪ 
  // 5   ��ע�ᣬ���� 
//AT+CREG?
//+CREG: 0,1
//OK
char * GSM_AT_CREG_Query = "AT+CREG?";

// AT+CGATT GPRS ���źͷ���
  //AT+CGATT?
  //+CGATT: 1
  //OK
char * GPRS_AT_CGATT_Query = "AT+CGATT?";

  //AT+CGATT=1
  //OK
char * GPRS_AT_CGATT_ATTACH = "AT+CGATT=1"; // 1 ����

char * GPRS_AT_CGATT_DETACH = "AT+CGATT=0"; // 1 ����

// AT+CGACT PDP �����ļ����ȥ����
	// AT+CGDCONT=1,"IP","CMNET"    
	// OK       //  ����<cid>=1��PDP�����ģ�PDP����Ϊ��IP����APNΪ��CMNET�� 
	// AT+CGACT=1,1    // PDP���� 
	// OK 
	// AT+CGACT=0,1   // PDPȥ���� 
	// NO CARRIER 
char * GPRS_AT_CGACT_Query = "AT+CGACT?";
//char * GPRS_AT_CGACT_ACTIVE = "AT+CGACT=1,1";
//char * GPRS_AT_CGACT_DEACTIVE = "AT+CGACT=0,1";
//AT+CGCLASS GPRS �ƶ�̨��� 
char * GPRS_AT_CGCLASS = "AT+CGCLASS=\"B\"";

// AT+QIFGCNT  ����ǰ�ó��� 
char * TCPIP_AT_QIFGCNT ="AT+QIFGCNT=0";

// AT+QISDE  ���� AT+QISEND �Ƿ��������ݻ��� 
char * TCPIP_AT_QISDE ="AT+QISDE=0"; // 0 ������ 1 ����
// AT+QICSGP ���� CSD �� GPRS ����ģʽ 
char * TCPIP_AT_QICSGP ="AT+QICSGP=1,\"IP\",\"CMNET\"";
// AT+QIPROMPT  ���÷�������ʱ�Ƿ���ʾ��>���͡�SEND OK��
	  // 0  ���ͳɹ�ʱ����ʾ��>�������ء�SEND OK�� 
	  // 1   ���ͳɹ�ʱ��ʾ��>�������ء�SEND OK�� 
	  // 2   ���ͳɹ�ʱ����ʾ��>���������ء�SEND OK��
char * TCPIP_AT_QIPROMPT = "AT+QIPROMPT=1";

    // AT+QNTP  ͨ��TCP/IP����ʱ�������ͬ������ʱ�� 
char * TCPIP_AT_QNTP ="AT+QNTP";


  //1. ������� 
    //2. SIM �����
	
    // AT+CSNS0  ��������/����ģʽ 
    // 1  ����/���潻��ģʽ���������� 
    // 2  ���� 
    // 3  ����/���ݽ���ģʽ���������� 
    // 4  ���� 
    // 5  ����/���潻��ģʽ���������� 
    // 6  ����/���ݽ���ģʽ���������� 
    // 7  ���ݸ�������ģʽ 

    // AT+CCLK? ʵʱʱ��
    //+CCLK: "13/06/19,02:34:02+00"
    
    // AT+QNITZ=1  ����/���� GSM����ʱ��ͬ��
    
    //AT+QGSMLOC ��ȡ��γ�Ⱥ�ʱ��
      //+QGSMLOC: 0,118.441844,34.955625,2013/06/19,02:30:47
      //AT+QGSMLOC=1
      //+QGSMLOC: 0,118.441844,34.955625,2013/06/19,02:30:55


#endif





