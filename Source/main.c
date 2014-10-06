//===========================================================================//
//                                                                           //
// 文件：  MAIN.C                                                            //
//                                                                           //
//===========================================================================//

//修改时间 蒋本庆 2014.01.06 

#include "msp430x54x.h"
#include "PortIO.h"
#include "main.h"
#include ".\GPS\GPS.h"
#include ".\GSM\GSM.h"
	 
//李凯 2013.7.2 (在 16Mhz下正常运行, 430更改主频后，ARM也要相应的更改SPI的频率和延时，保证同步）
void SPI(void);																				
#include".\SPI\CRC modbus.h"       															
#include".\SPI\shuzu.h"			 														   
#include".\SPI\SPIB2 init.h"																
#include".\SPI\SPI.h"																		
#include".\SPI\flash.h"			
unsigned char ID_Write_Flag;                                                               
unsigned long Write_Flag_time;
unsigned long LOW_POW_TIME_CNT;

unsigned long int MSP_430_Start_Time;//430开机计时
unsigned long int SPI_Iint_Time_Cnt;//430开机计时
extern void Read_Flash_Data(void);
extern void GSM_NOM_POW_ON(void);
unsigned char LPM_GPS_LOCATE_OK;//低功耗模式下,GPS定位成功
extern char LOW_POW_GPS_Flag;
unsigned char GPS_Start_FLG;
extern unsigned char LPM_GPS_Stop;
extern unsigned int GPS_WDT_CNT;
extern unsigned char LPM_GPS_CNT;
extern unsigned long int WDTnum;
extern unsigned long LPM_SHAKE_CNT;
unsigned char Shake_Open_FLG=1;
extern unsigned char WOSHOU_OK;
extern unsigned char Power_down2on;
//------------------------------------------------------------------------------
//		主函数
//------------------------------------------------------------------------------
void main(void) 
{

    WDTCTL= WDTPW + WDTHOLD; // 关看门狗
	Init_CLK();
	Init_NC_IO();//未使用的IO初始化
    Read_Flash_Data();//读flash
    Init_TimerWDT();
    _EINT();
    LPM = 2;//默认进入低功耗模式
    
    while(1) 
    {     
        Init_ADC();//中断唤醒打开
        Init_PWRON();
		if(Shake_Open_FLG)
		{
			Shake_Open_FLG=0;//上电后标志置1
			Init_Shake();
			LPM_SHAKE_CNT=0;
		}
		Init_Locate();
        //Init_Open_Up();
        //Init_Ring();
      
        Control_Strategy()                              ;
		Close_ADC();
        if(LPM&&((P2IN&PWRON)==0)) 
        {
            WDTnum=0;
			LPM3                                    ;
            _NOP()                                  ;
        }
 		//if((LPM == 1)&&((P2IN&PWRON)==0)) 
		if((P2IN&PWRON)==0) 
        {
            //定时更新电池电压信息
            //电量检测  & 省电模式切换
            //低功耗模式下的控制策略
            //用来设定LPM唤醒时间

             TIME_A0_Init()                                    ;
             GPS_Start()                                       ;
			 GPS_Start_FLG=1;
			 GPS_WDT_CNT=0;
			 LPM_GPS_CNT=0;
             GPS_LOCATE_Time     =10250                        ;//GPS接收时间10S
			 Close_A0();
			 WDTnum=0;
			 LPM3                                              ;//进入低功耗
            _NOP()                                             ;
        }
		
		if(LPM_GPS_Stop)
		{
			LPM_GPS_Stop=0;
			GPS_Stop()                                         ;
			Close_UCA1();			
		}
        Open_Box_Check();
		
		if(LPM_GPS_LOCATE_OK&&((P2IN&PWRON)==0))//在低功耗模式下，GPS定位成功
		{
            Read_Flash_Data();//读flash
            Want_GPS_Data_Type=0x01;
            GPS_UCA1_Init();//打开GPS接收初始化
			GPS_Start_FLG=0;
			LPM_GPS_LOCATE_OK=0;
            
			TIME_A0_Init();
			Init_ADC();
            GPS_Start();
            
			LOW_POW_TIME_CNT=0;
            ARM_DATA_STU &= ~0x04;
			LOW_POW_GPS_Flag=0x55;
            while((!(LOW_POW_GPS_Flag==0x11))&&(LOW_POW_TIME_CNT<MSP_A0_Min_1))                   
            {
                Do_GPS_Data()                               ;
            }
            LOW_POW_GPS_Flag=0x00                           ;
            GPS_Stop()                                      ;
			Close_UCA1();
            
			Read_Flash_Data();//读flash
            GSM_UCA0_Init();
            LPM_GSM_Globl_IN();
            GSM_NOM_POW_ON();
            LOW_POW_TIME_CNT=0;
			LOW_POW_GSM_Flag=0x00;
            ARM_DATA_STU &= ~0x01;

            while((!(LOW_POW_GSM_Flag==0x11))&&(LOW_POW_TIME_CNT<MSP_A0_Min_3))                            
            {
                GSM();
            }
            LOW_POW_GSM_Flag=0x00;
            LPM_GSM_Globl_OUT();
            GSM_NOM_POW_OFF();
			Close_UCA0();
		}
        

        //上电唤醒
        if((!LPM)||((P2IN&PWRON)==1)) 
        {
            //退出低功耗配置
            //关闭 上电、震动、开盒 中断
            //Delayms(1000);
            TIME_A0_Init();
            Close_Open();
			GPS_Data_OK_Flag=0x00;
            Change_SIM_Card_Flag=0xAA;
            MSP430_POWER_ON_FLAG=0x11;//11=上电；55=断电
            SIM_Card_Work_Flag=0xAA;
			Module_Status[0] &=~0xC0;//模块状态信息GPS信息+GSM信息+系统状态1、2+升级状态1、2
            GPS_GSM_System_Stu[0]&=~0xC0;//GPS状态存储
            GPS_Start();//GPS电源开启
            GSM_NOM_POW_ON();
            Want_GPS_Data_Type=0x01;
			Init_ADC();
			GSM_UCA0_Init();
            //GSM_Value_Init();
            GPS_UCA1_Init();
            
            WOSHOU_OK=0;
            Init_UCB2SPI(); 
            //Close_PWRON();
            Close_Shake();
            //Close_Open();  李凯8.22注释掉的
            Close_Ring();
            //策略控制关闭
            VmainMon();
            
           
            //GPS 打开
            //Init_UCA1UART();
            //GPS_Start();

            //GSM如果关闭 打开
            //GSM如果休眠 退出休眠
			Read_Flash_Data()                               ;//读flash
			
            /*
                    if(GSM_STATUS == 0){
                      GSM_Start();
                    }else if(GSM_STATUS == 2){
                        GSM_Sleep_Exit();
                    }
             */
            //电钥匙关断检测
            //上电检测 p1.0  P2.0   IO中断
            //挖机启动后是高电平，掉电是低电平
             //MSP_430_Start_Time=61441;
             Time_Count_Value_Init();                          //时间计数清零
			 Init_PWRON();
             SPI_Iint_Time_Cnt=0;
             ExitLMP_Time_Cnt=0;
             One_Degree_Alert_Time=0;//1级报警计数
			 LPM=0;
             
             while((P2IN&PWRON))
             { 
               //if(!WOSHOU_OK||((P2IN&PWRON)==0))
               if(((!WOSHOU_OK)||Power_down2on)&&(SPI_Iint_Time_Cnt>SECD_10))
               {
                 SPI_Iint_Time_Cnt=0;
                 Power_down2on=0;
                 Init_UCB2SPI();               
               }
               RD_Up_Speed_FLASH();//处理允许换卡 无卡升速
               Open_Box_Check();
               if(ID_Write_Flag)//7.8添加 
				 {
                    unsigned char i,num;
                    ID_Write_Flag=0;
                    Flash_WriteChar(0,1);//ID标志位
                    for(i=0,num=3;i<4;i++,num--)
                    {
                        Flash_WriteChar(1+i,SHEBEI_ID_memory[num]);//将ID写入InfoFlash
                    }
				 } 
               Change_SIM_Card_NUM();//允许更换手机卡号
			   

                if(RD_FLASH_OK_FLAG  == 0x11)
                {
                    GSM();//0 正常 0x01关机 0x02休眠 0x04 SIM卡 0x08 GSM网络正常 0x10GPRS网络正常 0x20 TCPIP正常
                }
                else
                {
                    Read_Flash_Data()                                   ;
                }
                Do_GPS_Data();//接收、处理GPS数据
                GSM_Brust();
            }
            LPM = 2;
            MSP430_POWER_ON_FLAG=0x55;
            ExitLMP_SendMCU();
            
			Init_PWRON();
			Init_Open_Up();
			
            //进入低功耗配置
			LPM_SHAKE_CNT=0;
			Shake_Open_FLG=1;
            //配置为低功耗模式
            //GPS 关闭
            //Close_UCA1UART();
            GPS_Stop();
			Close_UCA1();
            //SHUT_GPS_UCA1()                                 ;//关闭GPS中断
            //GSM 休眠
            //GSM_Sleep();
            LPM_GSM_Globl_OUT();
            GSM_NOM_POW_OFF();//GSM_NOM_POW_ON
			Close_UCA0();
			MSP_430_Start_Time=0;
            SIM_Card_Work_Flag=0x55;
            Change_SIM_Card_Flag=0x55;
        }//end if  上电唤醒结束
		Close_A0();
		Close_ADC();

    }//end while
}//end Main



/******************************************************************************
*名    称：USCI_B2_ISR()

*功    能：1.接收ARM传来的数据并进行解析处理
		   2.在中断中给ARM回应数据

*入口参数：无

*出口参数：无

*说    明：无

*修改历史:无
******************************************************************************/
#pragma vector=USCI_B2_VECTOR
__interrupt void USCI_B2_ISR(void)
{
	switch(UCB2IV)
  	{
    	case 0: break;// Vector 0 - no interrupt
    	case 2:      // Vector 2 - RXIFG
			SPIB2_DATA[cnt++]=UCB2RXBUF;
			
			if(SPIB2_DATA[cnt-1]==0x5a)//接收到0x5a说明ARM要读回数据
			{
				if(WOSHOU_CRC_OK) UCB2TXBUF=SPI_T_WOSHOU_OK[TX_Data_num++];//握手成功
				if(WOSHOU_CRC_ERR) UCB2TXBUF=SPI_T_WOSHOU_ERR[TX_Data_num++];//握手失败
				
				if(SHEBEI_ID_CRC_OK) UCB2TXBUF=SPI_T_SHEBEI_ID_OK[TX_Data_num++];//设备ID正确
				if(SHEBEI_ID_CRC_ERR) UCB2TXBUF=SPI_T_SHEBEI_ID_ERR[TX_Data_num++];//设备ID错误
				
				if(SHEBEI_Soft_VER_CRC_OK) UCB2TXBUF=SPI_T_SHEBEI_Soft_VER_OK[TX_Data_num++];//设备软件版本号正确
				if(SHEBEI_Soft_VER_CRC_ERR) UCB2TXBUF=SPI_T_SHEBEI_Soft_VER_ERR[TX_Data_num++];//设备软件版本号错误
				
				if(GPS_temp_OK) UCB2TXBUF=GPS_temp[GPS_Data_num++];//发送GPS数据
				
				if(GPS_VER_OK) UCB2TXBUF=GPS_mold_VER[GPS_VER_num++];//发送GPS软件版本号
				
				if(Mode_Parameter_OK) UCB2TXBUF=Mode_Parameter[mode_num++];//发送模式参数
				
				if(Module_State_OK) UCB2TXBUF=Module_State[Module_State_num++];//发送模块状态
				
				if(Update_Command_Data_OK) UCB2TXBUF=Update_Command_Data[TX_Update_Command_Data_num++];//发送升级命令
				if(Update_Data_OK) 
                {
                  UCB2TXBUF=Update_Data[TX_Update_Data_num++];//发送升级包数据
                  Data_Bag_TX_Finish=0;
                }
			}
			
			switch(cnt)//解析报头和数据类型
			{
				case 1:
					if(SPIB2_DATA[0]==0x3e)
						cnt=1;
					else
					{
						cnt=0;
						SPIB2_DATA[0]=0;
					}
					break;
					
				case 2:
					if(SPIB2_DATA[1]==0x3a)
						cnt=2;
					else
					{
						cnt=0;
						SPIB2_DATA[0]=0;
						SPIB2_DATA[1]=0;
					}
					break;
				case 3:
					if((SPIB2_DATA[2]==0x55)||(SPIB2_DATA[2]==0x10)||(SPIB2_DATA[2]==0x20)||(SPIB2_DATA[2]==0x30)||(SPIB2_DATA[2]==0x50)||(SPIB2_DATA[2]==0x60)
					   ||(SPIB2_DATA[2]==0xa0)||(SPIB2_DATA[2]==0xb0)||(SPIB2_DATA[2]==0xc0)||(SPIB2_DATA[2]==0xd0)||(SPIB2_DATA[2]==0xe0))
					   	
						cnt=3;
					else
					{
						cnt=0;
						SPIB2_DATA[0]=0;
						SPIB2_DATA[1]=0;
						SPIB2_DATA[2]=0;
					}
					break;
				case 4:
					if((SPIB2_DATA[3]==0xaa)||(SPIB2_DATA[3]==0x01)||(SPIB2_DATA[3]==0x02)||(SPIB2_DATA[3]==0x03)||(SPIB2_DATA[3]==0x05)||(SPIB2_DATA[3]==0x06)
					  ||(SPIB2_DATA[3]==0x01)||(SPIB2_DATA[3]==0x02)||(SPIB2_DATA[3]==0x03))
						
					{
						cnt=4;
					}
					else
					{
						cnt=0;
						SPIB2_DATA[0]=0;
						SPIB2_DATA[1]=0;
						SPIB2_DATA[2]=0;
						SPIB2_DATA[3]=0;
					}
                    break;
				default: 
		  			break;
			}
			
		if(cnt==6)
		{               
			lenth_data=(SPIB2_DATA[4]<<8)+SPIB2_DATA[5];//计算一组数据的长度
			flg=1;
		}
		if(flg)
		{
			if((lenth_data!=0)&&((lenth_data+10)==cnt))//如果接收完一组数据
			{
				flg=0;
				cnt=0;//计数清零，为接收下一组数据做准备
				flag_do=1;
				Do_SPI=1;
                UCB2IE&=~UCRXIE;//关接收中断
			}
		}
      		break;
		case 4: 
      		break;// Vector4-TXIFG
    	default: 
		  	break;
	}
	
	if(Do_SPI)
	{
		Do_SPI=0;
		SPI();//接收完一包数据后，在中断中分析收到的数据内容，准备好回应给ARM的数据
	}
	
	if(TX_Data_num==12)//回应给主机12个字节以后
	{
		TX_Data_num=0;
		WOSHOU_OK=1;
        WOSHOU_CRC_OK=0;
		WOSHOU_CRC_ERR=0;
		SHEBEI_ID_CRC_OK=0;
		SHEBEI_ID_CRC_ERR=0;
		SHEBEI_Soft_VER_CRC_OK=0;
          
		SHEBEI_Soft_VER_CRC_ERR=0;
	}
	if(GPS_Data_num==38)//给主机发完GPS数据以后
	{
		GPS_Data_num=0;
		GPS_temp_OK=0;
	}
	if(GPS_VER_num==18)//给主机发完GPS版本以后
	{
		GPS_VER_num=0;
		GPS_VER_OK=0;
        //ID_Write_Flag=1;
	}
	if(mode_num==30)//给主机发完各档模式工作参数
	{
		mode_num=0;
		Mode_Parameter_OK=0;
        Work_Mode_SPI_OK_Flag=0x00;
        Module_Status[5]&=~0x03;  
	}				
	if(TX_Update_Command_Data_num==28)//给主机发完升级命令
	{
		TX_Update_Command_Data_num=0;
		Update_Command_Data_OK=0;
	}		
	if(TX_Update_Data_num==1036)//给主机发完一包升级数据
	{
		TX_Update_Data_num=0;
		Data_Bag_TX_Finish=1;
        Update_Data_OK=0;

	}	
	if(Module_State_num==16)//给主机发完模块状态
	{
		Module_State_num=0;
		Module_State_OK=0;
	}
}



int __low_level_init(void)  
{  
  WDTCTL = WDTPW+WDTHOLD;                   // Stop WDT  
  return 1;  
}