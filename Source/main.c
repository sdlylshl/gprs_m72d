//===========================================================================//
//                                                                           //
// �ļ���  MAIN.C                                                            //
//                                                                           //
//===========================================================================//

//�޸�ʱ�� ������ 2014.01.06 

#include "msp430x54x.h"
#include "PortIO.h"
#include "main.h"
#include ".\GPS\GPS.h"
#include ".\GSM\GSM.h"
	 
//� 2013.7.2 (�� 16Mhz����������, 430������Ƶ��ARMҲҪ��Ӧ�ĸ���SPI��Ƶ�ʺ���ʱ����֤ͬ����
void SPI(void);																				
#include".\SPI\CRC modbus.h"       															
#include".\SPI\shuzu.h"			 														   
#include".\SPI\SPIB2 init.h"																
#include".\SPI\SPI.h"																		
#include".\SPI\flash.h"			
unsigned char ID_Write_Flag;                                                               
unsigned long Write_Flag_time;
unsigned long LOW_POW_TIME_CNT;

unsigned long int MSP_430_Start_Time;//430������ʱ
unsigned long int SPI_Iint_Time_Cnt;//430������ʱ
extern void Read_Flash_Data(void);
extern void GSM_NOM_POW_ON(void);
unsigned char LPM_GPS_LOCATE_OK;//�͹���ģʽ��,GPS��λ�ɹ�
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
//		������
//------------------------------------------------------------------------------
void main(void) 
{

    WDTCTL= WDTPW + WDTHOLD; // �ؿ��Ź�
	Init_CLK();
	Init_NC_IO();//δʹ�õ�IO��ʼ��
    Read_Flash_Data();//��flash
    Init_TimerWDT();
    _EINT();
    LPM = 2;//Ĭ�Ͻ���͹���ģʽ
    
    while(1) 
    {     
        Init_ADC();//�жϻ��Ѵ�
        Init_PWRON();
		if(Shake_Open_FLG)
		{
			Shake_Open_FLG=0;//�ϵ���־��1
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
            //��ʱ���µ�ص�ѹ��Ϣ
            //�������  & ʡ��ģʽ�л�
            //�͹���ģʽ�µĿ��Ʋ���
            //�����趨LPM����ʱ��

             TIME_A0_Init()                                    ;
             GPS_Start()                                       ;
			 GPS_Start_FLG=1;
			 GPS_WDT_CNT=0;
			 LPM_GPS_CNT=0;
             GPS_LOCATE_Time     =10250                        ;//GPS����ʱ��10S
			 Close_A0();
			 WDTnum=0;
			 LPM3                                              ;//����͹���
            _NOP()                                             ;
        }
		
		if(LPM_GPS_Stop)
		{
			LPM_GPS_Stop=0;
			GPS_Stop()                                         ;
			Close_UCA1();			
		}
        Open_Box_Check();
		
		if(LPM_GPS_LOCATE_OK&&((P2IN&PWRON)==0))//�ڵ͹���ģʽ�£�GPS��λ�ɹ�
		{
            Read_Flash_Data();//��flash
            Want_GPS_Data_Type=0x01;
            GPS_UCA1_Init();//��GPS���ճ�ʼ��
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
            
			Read_Flash_Data();//��flash
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
        

        //�ϵ绽��
        if((!LPM)||((P2IN&PWRON)==1)) 
        {
            //�˳��͹�������
            //�ر� �ϵ硢�𶯡����� �ж�
            //Delayms(1000);
            TIME_A0_Init();
            Close_Open();
			GPS_Data_OK_Flag=0x00;
            Change_SIM_Card_Flag=0xAA;
            MSP430_POWER_ON_FLAG=0x11;//11=�ϵ磻55=�ϵ�
            SIM_Card_Work_Flag=0xAA;
			Module_Status[0] &=~0xC0;//ģ��״̬��ϢGPS��Ϣ+GSM��Ϣ+ϵͳ״̬1��2+����״̬1��2
            GPS_GSM_System_Stu[0]&=~0xC0;//GPS״̬�洢
            GPS_Start();//GPS��Դ����
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
            //Close_Open();  �8.22ע�͵���
            Close_Ring();
            //���Կ��ƹر�
            VmainMon();
            
           
            //GPS ��
            //Init_UCA1UART();
            //GPS_Start();

            //GSM����ر� ��
            //GSM������� �˳�����
			Read_Flash_Data()                               ;//��flash
			
            /*
                    if(GSM_STATUS == 0){
                      GSM_Start();
                    }else if(GSM_STATUS == 2){
                        GSM_Sleep_Exit();
                    }
             */
            //��Կ�׹ضϼ��
            //�ϵ��� p1.0  P2.0   IO�ж�
            //�ڻ��������Ǹߵ�ƽ�������ǵ͵�ƽ
             //MSP_430_Start_Time=61441;
             Time_Count_Value_Init();                          //ʱ���������
			 Init_PWRON();
             SPI_Iint_Time_Cnt=0;
             ExitLMP_Time_Cnt=0;
             One_Degree_Alert_Time=0;//1����������
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
               RD_Up_Speed_FLASH();//���������� �޿�����
               Open_Box_Check();
               if(ID_Write_Flag)//7.8��� 
				 {
                    unsigned char i,num;
                    ID_Write_Flag=0;
                    Flash_WriteChar(0,1);//ID��־λ
                    for(i=0,num=3;i<4;i++,num--)
                    {
                        Flash_WriteChar(1+i,SHEBEI_ID_memory[num]);//��IDд��InfoFlash
                    }
				 } 
               Change_SIM_Card_NUM();//��������ֻ�����
			   

                if(RD_FLASH_OK_FLAG  == 0x11)
                {
                    GSM();//0 ���� 0x01�ػ� 0x02���� 0x04 SIM�� 0x08 GSM�������� 0x10GPRS�������� 0x20 TCPIP����
                }
                else
                {
                    Read_Flash_Data()                                   ;
                }
                Do_GPS_Data();//���ա�����GPS����
                GSM_Brust();
            }
            LPM = 2;
            MSP430_POWER_ON_FLAG=0x55;
            ExitLMP_SendMCU();
            
			Init_PWRON();
			Init_Open_Up();
			
            //����͹�������
			LPM_SHAKE_CNT=0;
			Shake_Open_FLG=1;
            //����Ϊ�͹���ģʽ
            //GPS �ر�
            //Close_UCA1UART();
            GPS_Stop();
			Close_UCA1();
            //SHUT_GPS_UCA1()                                 ;//�ر�GPS�ж�
            //GSM ����
            //GSM_Sleep();
            LPM_GSM_Globl_OUT();
            GSM_NOM_POW_OFF();//GSM_NOM_POW_ON
			Close_UCA0();
			MSP_430_Start_Time=0;
            SIM_Card_Work_Flag=0x55;
            Change_SIM_Card_Flag=0x55;
        }//end if  �ϵ绽�ѽ���
		Close_A0();
		Close_ADC();

    }//end while
}//end Main



/******************************************************************************
*��    �ƣ�USCI_B2_ISR()

*��    �ܣ�1.����ARM���������ݲ����н�������
		   2.���ж��и�ARM��Ӧ����

*��ڲ�������

*���ڲ�������

*˵    ������

*�޸���ʷ:��
******************************************************************************/
#pragma vector=USCI_B2_VECTOR
__interrupt void USCI_B2_ISR(void)
{
	switch(UCB2IV)
  	{
    	case 0: break;// Vector 0 - no interrupt
    	case 2:      // Vector 2 - RXIFG
			SPIB2_DATA[cnt++]=UCB2RXBUF;
			
			if(SPIB2_DATA[cnt-1]==0x5a)//���յ�0x5a˵��ARMҪ��������
			{
				if(WOSHOU_CRC_OK) UCB2TXBUF=SPI_T_WOSHOU_OK[TX_Data_num++];//���ֳɹ�
				if(WOSHOU_CRC_ERR) UCB2TXBUF=SPI_T_WOSHOU_ERR[TX_Data_num++];//����ʧ��
				
				if(SHEBEI_ID_CRC_OK) UCB2TXBUF=SPI_T_SHEBEI_ID_OK[TX_Data_num++];//�豸ID��ȷ
				if(SHEBEI_ID_CRC_ERR) UCB2TXBUF=SPI_T_SHEBEI_ID_ERR[TX_Data_num++];//�豸ID����
				
				if(SHEBEI_Soft_VER_CRC_OK) UCB2TXBUF=SPI_T_SHEBEI_Soft_VER_OK[TX_Data_num++];//�豸����汾����ȷ
				if(SHEBEI_Soft_VER_CRC_ERR) UCB2TXBUF=SPI_T_SHEBEI_Soft_VER_ERR[TX_Data_num++];//�豸����汾�Ŵ���
				
				if(GPS_temp_OK) UCB2TXBUF=GPS_temp[GPS_Data_num++];//����GPS����
				
				if(GPS_VER_OK) UCB2TXBUF=GPS_mold_VER[GPS_VER_num++];//����GPS����汾��
				
				if(Mode_Parameter_OK) UCB2TXBUF=Mode_Parameter[mode_num++];//����ģʽ����
				
				if(Module_State_OK) UCB2TXBUF=Module_State[Module_State_num++];//����ģ��״̬
				
				if(Update_Command_Data_OK) UCB2TXBUF=Update_Command_Data[TX_Update_Command_Data_num++];//������������
				if(Update_Data_OK) 
                {
                  UCB2TXBUF=Update_Data[TX_Update_Data_num++];//��������������
                  Data_Bag_TX_Finish=0;
                }
			}
			
			switch(cnt)//������ͷ����������
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
			lenth_data=(SPIB2_DATA[4]<<8)+SPIB2_DATA[5];//����һ�����ݵĳ���
			flg=1;
		}
		if(flg)
		{
			if((lenth_data!=0)&&((lenth_data+10)==cnt))//���������һ������
			{
				flg=0;
				cnt=0;//�������㣬Ϊ������һ��������׼��
				flag_do=1;
				Do_SPI=1;
                UCB2IE&=~UCRXIE;//�ؽ����ж�
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
		SPI();//������һ�����ݺ����ж��з����յ����������ݣ�׼���û�Ӧ��ARM������
	}
	
	if(TX_Data_num==12)//��Ӧ������12���ֽ��Ժ�
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
	if(GPS_Data_num==38)//����������GPS�����Ժ�
	{
		GPS_Data_num=0;
		GPS_temp_OK=0;
	}
	if(GPS_VER_num==18)//����������GPS�汾�Ժ�
	{
		GPS_VER_num=0;
		GPS_VER_OK=0;
        //ID_Write_Flag=1;
	}
	if(mode_num==30)//�������������ģʽ��������
	{
		mode_num=0;
		Mode_Parameter_OK=0;
        Work_Mode_SPI_OK_Flag=0x00;
        Module_Status[5]&=~0x03;  
	}				
	if(TX_Update_Command_Data_num==28)//������������������
	{
		TX_Update_Command_Data_num=0;
		Update_Command_Data_OK=0;
	}		
	if(TX_Update_Data_num==1036)//����������һ����������
	{
		TX_Update_Data_num=0;
		Data_Bag_TX_Finish=1;
        Update_Data_OK=0;

	}	
	if(Module_State_num==16)//����������ģ��״̬
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