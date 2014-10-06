/*******************************************************************
*文件名：shuzu.h
*创建时间：2013年05月13日
*创建人：  
*版本号：1.0
*功能：定义一些msp430与监控器SPI通信的数组和初始化的定义              
*件属性：特殊文件（只与SPI有关）
*修改历史：（每条详述）
******************************************************************/
unsigned char SPIB2_DATA[255];//接收数据的数组

unsigned char lenth_crc,lenth_data;//接收数据CRC校验使用的变量

unsigned char TX_Data_CRCL,TX_Data_CRCH;//发送数据CRC校验使用的变量
unsigned int TX_Data_CRC,TX_Data_crc_Length;//发送数据CRC校验使用的变量

unsigned char flg=0,flag_do=0,cnt;//识别报头、数据类型的变量


unsigned int contents,ID_Flag;//主函数中使用的变量


unsigned char WOSHOU_CRC_OK,WOSHOU_CRC_ERR,TX_Data_num,SHEBEI_ID_CRC_OK,SHEBEI_ID_CRC_ERR,//控制430发送数据的变量
			  SHEBEI_Soft_VER_CRC_OK,SHEBEI_Soft_VER_CRC_ERR;//控制430发送数据的变量

unsigned char ID_cnt;//主函数中使用的变量（根据需要添加）

unsigned char SHEBEI_ID_memory[4],CONTENT_DATA_memory[62],Soft_VER_cnt,Soft_VER_memory[32];//存储接收的数据

unsigned char GPS_mold_VER[18],GPS_VER_OK,GPS_VER_num,mode_num,
			 
			  Mode_Parameter[30],Mode_Parameter_OK,Mode_Parameter_ERR[30],Mode_Parameter_ASK_ERR,Mode_Parameter_cnt;//主机请求后，从机发给主机数据

unsigned char GPS_temp[38],GPS_cnt,GPS_temp_OK,GPS_Data_num;
unsigned char CONTENT_DATA_cnt;//模拟接收常态数据
unsigned char GPS_mold_VER_cnt;//模拟GPS版本号
extern unsigned char ID_Write_Flag; 
extern unsigned long int One_Mint_Warn_Cnt;//一分钟计数


#define Update_SIZE 1036
unsigned char Update_Data[Update_SIZE];//升级包数据


unsigned int Update_Datar_cnt,TX_Update_Data_num,Index;//升级包数据

unsigned char Update_Command_Data[28],Update_Command_Datar_cnt,Update_Command_Data_OK,TX_Update_Command_Data_num,Update_Data_OK;
			  

unsigned char GPS_Soft_VER[8]={0x42,0x47,0x31,0x30,0x30,0x14,0x05,0x19};//GPS软件版本号 BG.1.00-140519

unsigned char Module_State[16],Module_State_cnt,Module_State_OK,Module_State_num;//6.25添加

char GSM_MCU[62],Do_SPI,ARM_DATA_STU;
extern char Module_Status[6],GPS_Inf[28];
extern unsigned char GPS_Data_OK_Flag;
unsigned char GPS_temp_cnt;
unsigned char Soft_VER_OK ; 
extern char HML_Mode_BuF[21];
extern char Work_Mode_SPI_OK_Flag ;
extern unsigned char UP_Stat_SPI_BuF[18];//存放网关发送升级启动指令
unsigned char Ask_index;
unsigned char Data_Bag_TX_Finish=1;
extern unsigned int      SPI_UP_BIT_SUM                                    ;//累计已接收文件字节数
unsigned char WOSHOU_OK;
/**********************************************************************
       模拟性数据
*********************************************************************/



/**********************************************************************
       430发送的回应和数据到监控器(CRC低位在前) 没有CRC校验和停止符
*********************************************************************/
unsigned char SPI_T_WOSHOU_OK[12]={0x3c,0x3a,0x55,0xaa,0x00,0x02,0x4f,0x4b};//回复握手
unsigned char SPI_T_WOSHOU_ERR[12]={0x3c,0x3a,0x55,0xaa,0x00,0x02,0x4e,0x4f};//

unsigned char SPI_T_SHEBEI_ID_OK[12]={0x3c,0x3a,0x10,0x01,0x00,0x02,0x4f,0x4b};//回复设备ID
unsigned char SPI_T_SHEBEI_ID_ERR[12]={0x3c,0x3a,0x10,0x01,0x00,0x02,0x4e,0x4f};//

//unsigned char SPI_T_CONTENT_DATA_OK[12]={0x3c,0x3a,0x20,0x02,0x00,0x02,0x4f,0x4b};//回复常态数据
//unsigned char SPI_T_CONTENT_DATA_ERR[12]={0x3c,0x3a,0x20,0x02,0x00,0x02,0x4e,0x4f};//

unsigned char SPI_T_SHEBEI_Soft_VER_OK[12]={0x3c,0x3a,0x20,0x02,0x00,0x02,0x4f,0x4b};//回复设备软件版本号
unsigned char SPI_T_SHEBEI_Soft_VER_ERR[12]={0x3c,0x3a,0x20,0x02,0x00,0x02,0x4e,0x4f};//
#define UP_SYSTEM_C_SIZE 1200
extern unsigned char UP_SYSTEM_C_LANG[UP_SYSTEM_C_SIZE]                         ;//升级包数据

/**********************************************************************
		功能		
*********************************************************************/
#define SPI_WOSHOU 21930 //握手 0x55 0xaa
#define SPI_SHEBEI_ID 4097 //设备ID 0x10 0x01
#define SPI_CONTENT_DATA 20485 //常态数据 0x50 0x05
#define SHEBEI_Soft_VER 8194//设备软件版本号 0x20 0x02
#define ASK_GPS 40961//请求GPS数据 0xa0 0x01
#define ASK_GPS_VER 12291//请求GPS版本号 0x30 0x03
#define ASK_Mode_Parameter 45057//请求各档模式参数 0xB0 0x01
#define ASK_Module_State 24582 //请求模块状态 0x60 0x06

#define ASK_Monitor_Update 49153//请求显示器的升级包 0xC0 0x01
#define ASK_Monitor_Update_Index 53249 //请求显示器的升级索引 0xD0 0x01
#define ASK_Monitor_Update_Index_content 57345 //请求显示器的每包数据内容 0xE0 0x01

#define ASK_Controller_Update 49154 //请求控制器的升级包 0xC0 0x02
#define ASK_Controller_Update_Index 53250 //请求控制器的升级索引 0xD0 0x02
#define ASK_Controller_Update_Index_content 57346 //请求控制器的每包数据内容 0xE0 0x02

#define ASK_Motor_Update 49155 //请求油门控制器的升级包 0xC0 0x03
#define ASK_Motor_Update_Index 53251 //请求油门控制器的升级索引 0xD0 0x03
#define ASK_Motor_Update_Index_content 57347 //请求油门电机的每包数据内容 0xE0 0x03


