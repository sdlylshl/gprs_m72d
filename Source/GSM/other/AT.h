#ifndef __M72D_H__
#define __M72D_H__
//系统配置文件
//GPRS 接收缓存大小
#define GPRS_BUF_SIZE 560         //数据传输最大字节数
#define GPRS_SVRCMD_SIZE 64       //服务器下发指令最大值

extern unsigned char GPRS_RX_Buffer[GPRS_BUF_SIZE];      //USCI_A3中断接收数组
extern unsigned int  GPRS_RX_Buffer_Cnt;                 //USCI_A3中断接收数组统计

extern unsigned char GPRS_RX_BufferA[GPRS_BUF_SIZE];      //USCI_A3数据解析数组
extern unsigned char GPRS_RX_SVRCMD_BufferA[GPRS_SVRCMD_SIZE];   //服务器下发指令缓存 存取平台下发数据数组
extern unsigned char GPRS_R_SVRCMD_Flag;                    //平台下发命令标志

extern unsigned char M72_TCPIP_OK;
extern unsigned char M72_UDP_OK;
//心跳包发送标志 1可以发送 0 不发送
extern unsigned char GPS_HEART_BEAT_OK;
//数据包发送标志 1可以发送
extern unsigned char GPS_SEND_DATA_OK;

extern unsigned long GPRS_Heart_Beat_Index;  //每分钟发送心跳计时
extern unsigned long Heart_Beat_Cnt;                                     //心跳包定时计数器
extern unsigned long GPRS_Data_Cnt;                                      //数据包传输定时计数器
//数据包发送时间间隔配置
extern unsigned long GPS_DATA_TIME;
// AT+CPIN  输入 PIN 
char * SIM_AT_CPIN = "AT+CPIN";

//AT+QCCID 获取SIM卡ID CCID
//AT+QCCID
//89860022150959D05635
//OK
//AT+QCCID?
//ERROR
char * SIM_AT_QCCID_Query = "AT+QCCID";

//AT+CIMI  请求国际移动台设备标识（IMSI） 
//AT+CIMI?
//+CIMI: "460023209180635"
//OK
//AT+CIMI
//460023209180635
//OK
char * SIM_AT_CIMI_Query = "AT+CIMI";

//AT+CREG 网络注册信息<stat>  0   未注册；ME当前没有搜索到要注册业务的新营运商 
  // 1   已注册，本地网 
  // 2   未注册，但 ME 正在搜索要注册业务的新营运商 
  // 3   注册被拒绝 
  // 4   未知 
  // 5   已注册，漫游 
//AT+CREG?
//+CREG: 0,1
//OK
char * GSM_AT_CREG_Query = "AT+CREG?";

// AT+CGATT GPRS 附着和分离
  //AT+CGATT?
  //+CGATT: 1
  //OK
char * GPRS_AT_CGATT_Query = "AT+CGATT?";

  //AT+CGATT=1
  //OK
char * GPRS_AT_CGATT_ATTACH = "AT+CGATT=1"; // 1 附着

char * GPRS_AT_CGATT_DETACH = "AT+CGATT=0"; // 1 分离

// AT+CGACT PDP 上下文激活和去激活
	// AT+CGDCONT=1,"IP","CMNET"    
	// OK       //  定义<cid>=1的PDP上下文，PDP类型为“IP”，APN为“CMNET” 
	// AT+CGACT=1,1    // PDP激活 
	// OK 
	// AT+CGACT=0,1   // PDP去激活 
	// NO CARRIER 
char * GPRS_AT_CGACT_Query = "AT+CGACT?";
//char * GPRS_AT_CGACT_ACTIVE = "AT+CGACT=1,1";
//char * GPRS_AT_CGACT_DEACTIVE = "AT+CGACT=0,1";
//AT+CGCLASS GPRS 移动台类别 
char * GPRS_AT_CGCLASS = "AT+CGCLASS=\"B\"";

// AT+QIFGCNT  配置前置场景 
char * TCPIP_AT_QIFGCNT ="AT+QIFGCNT=0";

// AT+QISDE  控制 AT+QISEND 是否允许数据回显 
char * TCPIP_AT_QISDE ="AT+QISDE=0"; // 0 不回显 1 回显
// AT+QICSGP 设置 CSD 或 GPRS 连接模式 
char * TCPIP_AT_QICSGP ="AT+QICSGP=1,\"IP\",\"CMNET\"";
// AT+QIPROMPT  设置发送数据时是否显示“>”和“SEND OK”
	  // 0  发送成功时不显示“>”，返回“SEND OK” 
	  // 1   发送成功时显示“>”，返回“SEND OK” 
	  // 2   发送成功时不显示“>”，不返回“SEND OK”
char * TCPIP_AT_QIPROMPT = "AT+QIPROMPT=1";

    // AT+QNTP  通过TCP/IP网络时间服务器同步本地时间 
char * TCPIP_AT_QNTP ="AT+QNTP";


  //1. 开机检测 
    //2. SIM 卡检测
	
    // AT+CSNS0  配置语音/数据模式 
    // 1  语音/传真交替模式，语音优先 
    // 2  传真 
    // 3  语音/数据交替模式，语音优先 
    // 4  数据 
    // 5  语音/传真交替模式，传真优先 
    // 6  语音/数据交替模式，数据优先 
    // 7  数据跟随语音模式 

    // AT+CCLK? 实时时钟
    //+CCLK: "13/06/19,02:34:02+00"
    
    // AT+QNITZ=1  启用/禁用 GSM网络时间同步
    
    //AT+QGSMLOC 获取经纬度和时间
      //+QGSMLOC: 0,118.441844,34.955625,2013/06/19,02:30:47
      //AT+QGSMLOC=1
      //+QGSMLOC: 0,118.441844,34.955625,2013/06/19,02:30:55


#endif





