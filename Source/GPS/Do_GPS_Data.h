#define GPS_RX_Buffer_Size 100

void Do_GPS_Data(void)                                        ;//����GPS��������
void Do_GPS_RMC_Data(void)                                    ;//����GPRMC���ݰ�UTCʱ�����ݴ���
void Do_GPS_GGA_Data(void)                                    ;//����GPGGA���ݰ����ݴ���
void Open_UCA0_UART_Init(void)                                ;//��GPS�ж�
void GPS_Post_GGA_Mesg(void)                                  ;//����GPGGA���ݰ���λ��Ϣ���ݴ���
void GPS_Post_RMC_Mesg(void)                                  ;//����GPRMC���ݰ���λ��Ϣ���ݴ���
void Save_GPS_Inf_Buffer(void)                                ;//��GPS���ݴ��뻺��
unsigned char ASCIITOHEX( unsigned char Input_H,
                         unsigned char Input_L )              ;//��ASCII�ַ�ת��Ϊ16���Ƶ���
extern void SHUT_GPS_UCA1(void)                              ;//�ر�GPS�ж�



extern unsigned char GSM_SIM_Signal_Num                       ;//�ź�����ǿ��
extern unsigned int  BAT_V_main                               ;//���ص�ѹ ���
extern unsigned int  LI_BAT_VOL                               ;//﮵�ص�ѹ ���


extern void VmainMon(void)                                    ;//�������Դ��ѹ��﮵�ص�ѹ
extern char GPS_GSM_System_Stu[4]                             ;//һ���״̬�洢
extern char Module_Status[6]                                  ;//ģ��״̬��ϢGPS��Ϣ+GSM��Ϣ+ϵͳ״̬1��2+����״̬1��2
extern char * GSM_strsep (char **stringp, const char *delim)  ;//�ַ����ָ�
extern char  GPS_RX_Buffer[GPS_RX_Buffer_Size]                ;//GPS���ݻ���100
extern unsigned int GPS_RX_Buffer_Cnt                         ;//GPS���ݻ������
extern void GPS_UCA1_Init(void)                               ;//GPS�жϳ�ʼ��
extern unsigned char Want_GPS_Data_Type                       ;//��ҪGPS��������00==����Ӧ��0x11==GPRMC��0x22==GPGGA�� 
extern unsigned char GPS_R_OK_End_Flag                        ;//GPS�жϽ��ճɹ���־ 00==δ���գ�0x11==GPRMC��0x22==GPGGA��
extern char GSM_SIM_Signal()                                  ;//�ź�����ǿ��
extern void JG_OUT_EDGE(void)                                 ;//�ж�Խ�籨�� 
extern void VmainMon(void)                                    ;//�������Դ��ѹ��﮵�ص�ѹ   




#define GPS_RX_Buffer_Size 100

void Do_GPS_Data(void)                                        ;//����GPS��������
void Do_GPS_RMC_Data(void)                                    ;//����GPRMC���ݰ�UTCʱ�����ݴ���
void Do_GPS_GGA_Data(void)                                    ;//����GPGGA���ݰ����ݴ���
void Open_UCA0_UART_Init(void)                                ;//��GPS�ж�
void GPS_Post_GGA_Mesg(void)                                  ;//����GPGGA���ݰ���λ��Ϣ���ݴ���
void GPS_Post_RMC_Mesg(void)                                  ;//����GPRMC���ݰ���λ��Ϣ���ݴ���
void Save_GPS_Inf_Buffer(void)                                ;//��GPS���ݴ��뻺��
unsigned char ASCIITOHEX( unsigned char Input_H,
                         unsigned char Input_L )              ;//��ASCII�ַ�ת��Ϊ16���Ƶ���
extern void SHUT_GPS_UCA1(void)                              ;//�ر�GPS�ж�



extern unsigned char GSM_SIM_Signal_Num                       ;//�ź�����ǿ��
extern unsigned int  BAT_V_main                               ;//���ص�ѹ ���
extern unsigned int  LI_BAT_VOL                               ;//﮵�ص�ѹ ���


extern void VmainMon(void)                                    ;//�������Դ��ѹ��﮵�ص�ѹ
extern char GPS_GSM_System_Stu[4]                             ;//һ���״̬�洢
extern char Module_Status[6]                                  ;//ģ��״̬��ϢGPS��Ϣ+GSM��Ϣ+ϵͳ״̬1��2+����״̬1��2
extern char * GSM_strsep (char **stringp, const char *delim)  ;//�ַ����ָ�
extern char  GPS_RX_Buffer[GPS_RX_Buffer_Size]                ;//GPS���ݻ���100
extern unsigned int GPS_RX_Buffer_Cnt                         ;//GPS���ݻ������
extern void GPS_UCA1_Init(void)                               ;//GPS�жϳ�ʼ��
extern unsigned char Want_GPS_Data_Type                       ;//��ҪGPS��������00==����Ӧ��0x11==GPRMC��0x22==GPGGA�� 
extern unsigned char GPS_R_OK_End_Flag                        ;//GPS�жϽ��ճɹ���־ 00==δ���գ�0x11==GPRMC��0x22==GPGGA��
extern char GSM_SIM_Signal()                                  ;//�ź�����ǿ��
extern void JG_OUT_EDGE(void)                                 ;//�ж�Խ�籨�� 
extern void VmainMon(void)                                    ;//�������Դ��ѹ��﮵�ص�ѹ   














