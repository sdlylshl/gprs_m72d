/*******************************************************************
*�ļ�����shuzu.h
*����ʱ�䣺2013��05��13��
*�����ˣ�  
*�汾�ţ�1.0
*���ܣ�����һЩmsp430������SPIͨ�ŵ�����ͳ�ʼ���Ķ���              
*�����ԣ������ļ���ֻ��SPI�йأ�
*�޸���ʷ����ÿ��������
******************************************************************/
unsigned char SPIB2_DATA[255];//�������ݵ�����

unsigned char lenth_crc,lenth_data;//��������CRCУ��ʹ�õı���

unsigned char TX_Data_CRCL,TX_Data_CRCH;//��������CRCУ��ʹ�õı���
unsigned int TX_Data_CRC,TX_Data_crc_Length;//��������CRCУ��ʹ�õı���

unsigned char flg=0,flag_do=0,cnt;//ʶ��ͷ���������͵ı���


unsigned int contents,ID_Flag;//��������ʹ�õı���


unsigned char WOSHOU_CRC_OK,WOSHOU_CRC_ERR,TX_Data_num,SHEBEI_ID_CRC_OK,SHEBEI_ID_CRC_ERR,//����430�������ݵı���
			  SHEBEI_Soft_VER_CRC_OK,SHEBEI_Soft_VER_CRC_ERR;//����430�������ݵı���

unsigned char ID_cnt;//��������ʹ�õı�����������Ҫ��ӣ�

unsigned char SHEBEI_ID_memory[4],CONTENT_DATA_memory[62],Soft_VER_cnt,Soft_VER_memory[32];//�洢���յ�����

unsigned char GPS_mold_VER[18],GPS_VER_OK,GPS_VER_num,mode_num,
			 
			  Mode_Parameter[30],Mode_Parameter_OK,Mode_Parameter_ERR[30],Mode_Parameter_ASK_ERR,Mode_Parameter_cnt;//��������󣬴ӻ�������������

unsigned char GPS_temp[38],GPS_cnt,GPS_temp_OK,GPS_Data_num;
unsigned char CONTENT_DATA_cnt;//ģ����ճ�̬����
unsigned char GPS_mold_VER_cnt;//ģ��GPS�汾��
extern unsigned char ID_Write_Flag; 
extern unsigned long int One_Mint_Warn_Cnt;//һ���Ӽ���


#define Update_SIZE 1036
unsigned char Update_Data[Update_SIZE];//����������


unsigned int Update_Datar_cnt,TX_Update_Data_num,Index;//����������

unsigned char Update_Command_Data[28],Update_Command_Datar_cnt,Update_Command_Data_OK,TX_Update_Command_Data_num,Update_Data_OK;
			  

unsigned char GPS_Soft_VER[8]={0x42,0x47,0x31,0x30,0x30,0x14,0x05,0x19};//GPS����汾�� BG.1.00-140519

unsigned char Module_State[16],Module_State_cnt,Module_State_OK,Module_State_num;//6.25���

char GSM_MCU[62],Do_SPI,ARM_DATA_STU;
extern char Module_Status[6],GPS_Inf[28];
extern unsigned char GPS_Data_OK_Flag;
unsigned char GPS_temp_cnt;
unsigned char Soft_VER_OK ; 
extern char HML_Mode_BuF[21];
extern char Work_Mode_SPI_OK_Flag ;
extern unsigned char UP_Stat_SPI_BuF[18];//������ط�����������ָ��
unsigned char Ask_index;
unsigned char Data_Bag_TX_Finish=1;
extern unsigned int      SPI_UP_BIT_SUM                                    ;//�ۼ��ѽ����ļ��ֽ���
unsigned char WOSHOU_OK;
/**********************************************************************
       ģ��������
*********************************************************************/



/**********************************************************************
       430���͵Ļ�Ӧ�����ݵ������(CRC��λ��ǰ) û��CRCУ���ֹͣ��
*********************************************************************/
unsigned char SPI_T_WOSHOU_OK[12]={0x3c,0x3a,0x55,0xaa,0x00,0x02,0x4f,0x4b};//�ظ�����
unsigned char SPI_T_WOSHOU_ERR[12]={0x3c,0x3a,0x55,0xaa,0x00,0x02,0x4e,0x4f};//

unsigned char SPI_T_SHEBEI_ID_OK[12]={0x3c,0x3a,0x10,0x01,0x00,0x02,0x4f,0x4b};//�ظ��豸ID
unsigned char SPI_T_SHEBEI_ID_ERR[12]={0x3c,0x3a,0x10,0x01,0x00,0x02,0x4e,0x4f};//

//unsigned char SPI_T_CONTENT_DATA_OK[12]={0x3c,0x3a,0x20,0x02,0x00,0x02,0x4f,0x4b};//�ظ���̬����
//unsigned char SPI_T_CONTENT_DATA_ERR[12]={0x3c,0x3a,0x20,0x02,0x00,0x02,0x4e,0x4f};//

unsigned char SPI_T_SHEBEI_Soft_VER_OK[12]={0x3c,0x3a,0x20,0x02,0x00,0x02,0x4f,0x4b};//�ظ��豸����汾��
unsigned char SPI_T_SHEBEI_Soft_VER_ERR[12]={0x3c,0x3a,0x20,0x02,0x00,0x02,0x4e,0x4f};//
#define UP_SYSTEM_C_SIZE 1200
extern unsigned char UP_SYSTEM_C_LANG[UP_SYSTEM_C_SIZE]                         ;//����������

/**********************************************************************
		����		
*********************************************************************/
#define SPI_WOSHOU 21930 //���� 0x55 0xaa
#define SPI_SHEBEI_ID 4097 //�豸ID 0x10 0x01
#define SPI_CONTENT_DATA 20485 //��̬���� 0x50 0x05
#define SHEBEI_Soft_VER 8194//�豸����汾�� 0x20 0x02
#define ASK_GPS 40961//����GPS���� 0xa0 0x01
#define ASK_GPS_VER 12291//����GPS�汾�� 0x30 0x03
#define ASK_Mode_Parameter 45057//�������ģʽ���� 0xB0 0x01
#define ASK_Module_State 24582 //����ģ��״̬ 0x60 0x06

#define ASK_Monitor_Update 49153//������ʾ���������� 0xC0 0x01
#define ASK_Monitor_Update_Index 53249 //������ʾ������������ 0xD0 0x01
#define ASK_Monitor_Update_Index_content 57345 //������ʾ����ÿ���������� 0xE0 0x01

#define ASK_Controller_Update 49154 //����������������� 0xC0 0x02
#define ASK_Controller_Update_Index 53250 //������������������� 0xD0 0x02
#define ASK_Controller_Update_Index_content 57346 //�����������ÿ���������� 0xE0 0x02

#define ASK_Motor_Update 49155 //�������ſ������������� 0xC0 0x03
#define ASK_Motor_Update_Index 53251 //�������ſ��������������� 0xD0 0x03
#define ASK_Motor_Update_Index_content 57347 //�������ŵ����ÿ���������� 0xE0 0x03


