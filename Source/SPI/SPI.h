
/******************************************************************************
*��    �ƣ�SPI()

*��    �ܣ�1.��������ARM����������
		   2.׼����Ӧ��ARM������

*��ڲ�������

*���ڲ�������

*˵    ������

*�޸���ʷ:��
******************************************************************************/

void SPI(void)
{
	if(flag_do)//����������������һ������    
	{ 
		flag_do=0;//��ձ�־��Ϊ��һ����׼��
		lenth_crc=(SPIB2_DATA[4]<<8)+SPIB2_DATA[5]+8;//�������ݵĳ��ȣ���ͷ+��������+���ݳ���+��������+CRC��
		contents=(SPIB2_DATA[2]<<8)+SPIB2_DATA[3];//���յ������ݽ��������жϣ�ȷ������������
		if((SPIB2_DATA[lenth_crc]==0x0d)&&(SPIB2_DATA[lenth_crc+1]==0x0a))//�����λ�����ֹͣ��
		{    
			if(!crc_modbus2(SPIB2_DATA,lenth_crc)) //���CRCУ����ȷ
			{
				switch(contents)  
				{
					case SPI_WOSHOU://���������ź���ȷ
					{
						TX_Data_crc_Length=(SPI_T_WOSHOU_OK[4]<<8)+SPI_T_WOSHOU_OK[5]+6;//430����������ҪУ��ĳ���
						TX_Data_CRC=crc_modbus2(SPI_T_WOSHOU_OK,TX_Data_crc_Length);//CRCУ������
						TX_Data_CRCH=(char)(TX_Data_CRC>>8);
						TX_Data_CRCL=(char)TX_Data_CRC;
						
						SPI_T_WOSHOU_OK[TX_Data_crc_Length]=TX_Data_CRCH;	
						SPI_T_WOSHOU_OK[TX_Data_crc_Length+1]=TX_Data_CRCL;//���CRCУ��
						
						SPI_T_WOSHOU_OK[TX_Data_crc_Length+2]=0x0d;
						SPI_T_WOSHOU_OK[TX_Data_crc_Length+3]=0x0a;//���ֹͣ��
						
						WOSHOU_CRC_OK=1;//CRCУ��ɹ����������������� ��д����
						
						TX_Data_num=0;//�������ݼ�������
						UCB2IE|=UCRXIE;//�������ж�
						break;
					}
					
					
					case SPI_SHEBEI_ID://�����豸ID��ȷ
					{						
						//ֻ�е���ʾ���ϵ�ID�Ÿ��ĺ�ARM�ŷ���ID�ţ�ƽʱ������
						
						for(ID_cnt=0;ID_cnt<4;ID_cnt++)
						{
							SHEBEI_ID_memory[ID_cnt]=SPIB2_DATA[6+ID_cnt];
						}
						
						ID_Flag=1;
						ID_Write_Flag=1;
                      
						
						TX_Data_crc_Length=(SPI_T_SHEBEI_ID_OK[4]<<8)+SPI_T_SHEBEI_ID_OK[5]+6;//430����������ҪУ��ĳ���
						TX_Data_CRC=crc_modbus2(SPI_T_SHEBEI_ID_OK,TX_Data_crc_Length);//CRCУ������
						TX_Data_CRCH=(char)(TX_Data_CRC>>8);
						TX_Data_CRCL=(char)TX_Data_CRC;
						
						SPI_T_SHEBEI_ID_OK[TX_Data_crc_Length]=TX_Data_CRCH;	
						SPI_T_SHEBEI_ID_OK[TX_Data_crc_Length+1]=TX_Data_CRCL;//���CRCУ��
						
						SPI_T_SHEBEI_ID_OK[TX_Data_crc_Length+2]=0x0d;
						SPI_T_SHEBEI_ID_OK[TX_Data_crc_Length+3]=0x0a;//���ֹͣ��
						
						SHEBEI_ID_CRC_OK=1;
						
						TX_Data_num=0;//�������ݼ�������
						
						UCB2IE|=UCRXIE;//�������ж�

						break;
					}
					
					case SPI_CONTENT_DATA://���ճ�̬������ȷ
					{
						//SPIB2_DATA[6]~SPIB2_DATA[67]��62���ֽڶ�Ӧ�ϱ�MCU���������46�ֽڵ�107�ֽ�
						//����GPS��λ��Ϣ
						//����ϵͳ�ɼ���Ϣ �������ݱ�־ MCU���ݵ�
						//���ϳ�̬���� ���һ������������
						//��̬���ݶ��ߺ� Ҫ�򿪽����ж� 
						for(CONTENT_DATA_cnt=0;CONTENT_DATA_cnt<62;CONTENT_DATA_cnt++)
						{
							GSM_MCU[CONTENT_DATA_cnt]=SPIB2_DATA[6+CONTENT_DATA_cnt];//����̬����ת��
						}
						
						ARM_DATA_STU|=0x01;//�ñ�־λ
						
						UCB2IE|=UCRXIE;//�������ж�
						
						//
						//�����������󣬽����콫�����ϴ���ƽ̨
						//
						
						break;
					}
					
					case SHEBEI_Soft_VER://�����豸����汾����ȷ(��ʾ���������������ŵ������汾��)
					{
						for(Soft_VER_cnt=0;Soft_VER_cnt<24;Soft_VER_cnt++)
						{
							Soft_VER_memory[Soft_VER_cnt]=SPIB2_DATA[6+Soft_VER_cnt];//�洢���յ�����汾��
						}
						
						for(Soft_VER_cnt=0;Soft_VER_cnt<8;Soft_VER_cnt++)
						{
							Soft_VER_memory[Soft_VER_cnt+24]=GPS_Soft_VER[Soft_VER_cnt];//GPS����汾��
						}
                        
                        Soft_VER_OK=0X11;
						
						TX_Data_crc_Length=(SPI_T_SHEBEI_Soft_VER_OK[4]<<8)+SPI_T_SHEBEI_Soft_VER_OK[5]+6;//430����������ҪУ��ĳ���
						TX_Data_CRC=crc_modbus2(SPI_T_SHEBEI_Soft_VER_OK,TX_Data_crc_Length);//CRCУ������
						TX_Data_CRCH=(char)(TX_Data_CRC>>8);
						TX_Data_CRCL=(char)TX_Data_CRC;
						
						SPI_T_SHEBEI_Soft_VER_OK[TX_Data_crc_Length]=TX_Data_CRCH;	
						SPI_T_SHEBEI_Soft_VER_OK[TX_Data_crc_Length+1]=TX_Data_CRCL;//���CRCУ��
						
						SPI_T_SHEBEI_Soft_VER_OK[TX_Data_crc_Length+2]=0x0d;
						SPI_T_SHEBEI_Soft_VER_OK[TX_Data_crc_Length+3]=0x0a;//���ֹͣ��
						
						SHEBEI_Soft_VER_CRC_OK=1;
						
						TX_Data_num=0;//�������ݼ�������
						
						UCB2IE|=UCRXIE;//�������ж�
						
						//�����콫����汾���ϴ���ƽ̨
						//
						//
						break;
					}
					
					case ASK_GPS://��������GPS��Ϣ
					{                         
                        if(GPS_Data_OK_Flag==0x11)
						{
							GPS_Data_OK_Flag=0x00;//��־λ����
							GPS_temp[0]=0x3c;
							GPS_temp[1]=0x3a;
						
							GPS_temp[2]=0xa0;
							GPS_temp[3]=0x01;
						
							GPS_temp[4]=0x00;
							GPS_temp[5]=28;
						
							for(GPS_temp_cnt=0;GPS_temp_cnt<28;GPS_temp_cnt++)
							{
								GPS_temp[6+GPS_temp_cnt]=GPS_Inf[GPS_temp_cnt];//��ӳɹ���λ���GPS����
							}
							
							TX_Data_crc_Length=34;//430����������ҪУ��ĳ���
							TX_Data_CRC=crc_modbus2(GPS_temp,TX_Data_crc_Length);//CRCУ������
							TX_Data_CRCH=(char)(TX_Data_CRC>>8);
							TX_Data_CRCL=(char)TX_Data_CRC;
							
							GPS_temp[TX_Data_crc_Length]=TX_Data_CRCH;	
							GPS_temp[TX_Data_crc_Length+1]=TX_Data_CRCL;//���CRCУ��
						
							GPS_temp[TX_Data_crc_Length+2]=0x0d;
							GPS_temp[TX_Data_crc_Length+3]=0x0a;//���ֹͣ��
						
							GPS_temp_OK=1;
						
							GPS_Data_num=0;//�������ݼ�������
						}
						
						
						UCB2IE|=UCRXIE;//�������ж�
				
						break;
					}
					
					case ASK_GPS_VER://��������GPS�汾��
					{
						GPS_mold_VER[0]=0x3c;
						GPS_mold_VER[1]=0x3a;
						
						GPS_mold_VER[2]=0x30;
						GPS_mold_VER[3]=0x03;
						
						GPS_mold_VER[4]=0x00;
						GPS_mold_VER[5]=8;
						
						for(GPS_mold_VER_cnt=0;GPS_mold_VER_cnt<8;GPS_mold_VER_cnt++)
						{
							GPS_mold_VER[GPS_mold_VER_cnt+6]=GPS_Soft_VER[GPS_mold_VER_cnt];//GPS�汾��
						}
						
						TX_Data_crc_Length=14;//430����������ҪУ��ĳ���
						TX_Data_CRC=crc_modbus2(GPS_mold_VER,TX_Data_crc_Length);//CRCУ������
						TX_Data_CRCH=(char)(TX_Data_CRC>>8);
						TX_Data_CRCL=(char)TX_Data_CRC;
						
						GPS_mold_VER[TX_Data_crc_Length]=TX_Data_CRCH;	
						GPS_mold_VER[TX_Data_crc_Length+1]=TX_Data_CRCL;//���CRCУ��
						
						GPS_mold_VER[TX_Data_crc_Length+2]=0x0d;
						GPS_mold_VER[TX_Data_crc_Length+3]=0x0a;//���ֹͣ��
						
						GPS_VER_OK=1;
						
						GPS_VER_num=0;//�������ݼ�������
						
						UCB2IE|=UCRXIE;//�������ж�

						break;
					}
					
					case  ASK_Mode_Parameter://�����������ģʽ��������������ģ��״̬�ı�־λ�����Ƿ���������ݣ���������˵�������Ѿ����������ݣ�
					{
						Mode_Parameter[0]=0x3c;
						Mode_Parameter[1]=0x3a;
						
						Mode_Parameter[2]=0xb0;
						Mode_Parameter[3]=0x01;
						
						Mode_Parameter[4]=0x00;
						Mode_Parameter[5]=20;
						
						for(Mode_Parameter_cnt=0;Mode_Parameter_cnt<20;Mode_Parameter_cnt++)
						{
							Mode_Parameter[Mode_Parameter_cnt+6]=HML_Mode_BuF[Mode_Parameter_cnt];
						}

						TX_Data_crc_Length=26;//430����������ҪУ��ĳ���
						
						TX_Data_CRC=crc_modbus2(Mode_Parameter,TX_Data_crc_Length);//CRCУ������
						TX_Data_CRCH=(char)(TX_Data_CRC>>8);
						TX_Data_CRCL=(char)TX_Data_CRC;
						
						Mode_Parameter[TX_Data_crc_Length]=TX_Data_CRCH;	
						Mode_Parameter[TX_Data_crc_Length+1]=TX_Data_CRCL;//���CRCУ��
						
						Mode_Parameter[TX_Data_crc_Length+2]=0x0d;
						Mode_Parameter[TX_Data_crc_Length+3]=0x0a;//���ֹͣ��
						
						Mode_Parameter_OK=1;
						
						mode_num=0;//�������ݼ�������
						
						UCB2IE|=UCRXIE;//�������ж�
									
						break;
					}
					
					
					
					case ASK_Module_State://��������ģ��״̬
					{
						Module_State[0]=0x3c;
						Module_State[1]=0x3a;
						
						Module_State[2]=0x60;
						Module_State[3]=0x06;
						
						Module_State[4]=0x00;
						Module_State[5]=6;
						
                        
                        if(One_Mint_Warn_Cnt<MSP_A0_Min_10)//һ���Ӽ���)
                        {
                           Module_Status[0] &= ~0x0F;
                           Module_Status[0] &= ~0x30;
                           
                           Module_Status[1]  =  0x00;
                           
                           Module_Status[2] &= ~0xC0;
                           Module_Status[2] &= ~0x30;
                           Module_Status[2] &= ~0x0C;
                        }
                        
						for(Module_State_cnt=0;Module_State_cnt<6;Module_State_cnt++)
						{
							Module_State[Module_State_cnt+6]=Module_Status[Module_State_cnt];//
						}
						
						TX_Data_crc_Length=12;//430����������ҪУ��ĳ���
						TX_Data_CRC=crc_modbus2(Module_State,TX_Data_crc_Length);//CRCУ������
						TX_Data_CRCH=(char)(TX_Data_CRC>>8);
						TX_Data_CRCL=(char)TX_Data_CRC;
						
						Module_State[TX_Data_crc_Length]=TX_Data_CRCH;	
						Module_State[TX_Data_crc_Length+1]=TX_Data_CRCL;//���CRCУ��
						
						Module_State[TX_Data_crc_Length+2]=0x0d;
						Module_State[TX_Data_crc_Length+3]=0x0a;//���ֹͣ��
						
						Module_State_OK=1;
						
						Module_State_num=0;//�������ݼ�������
						
						UCB2IE|=UCRXIE;//�������ж�

						break;
					}
					
					case  ASK_Monitor_Update://����������ʾ��������
					case  ASK_Controller_Update://����������������� 0xC0 0x02
					case  ASK_Motor_Update://�������ſ������������� 0xC0 0x03
					{
						Update_Command_Data[0]=0x3c;
						Update_Command_Data[1]=0x3a;
						
						Update_Command_Data[2]=0xC0;
						Update_Command_Data[3]=0x01;
						
						Update_Command_Data[4]=0x00;
						Update_Command_Data[5]=18;//18���ֽ�
						
						Module_Status[4] &= ~0x03;//�յ�������־�����������־  

                        
						for(Update_Command_Datar_cnt=0;Update_Command_Datar_cnt<18;Update_Command_Datar_cnt++)
						{
							Update_Command_Data[Update_Command_Datar_cnt+6]=UP_Stat_SPI_BuF[Update_Command_Datar_cnt];
						}
						
						TX_Data_crc_Length=24;//430����������ҪУ��ĳ���
						TX_Data_CRC=crc_modbus2(Update_Command_Data,TX_Data_crc_Length);//CRCУ������
						TX_Data_CRCH=(char)(TX_Data_CRC>>8);
						TX_Data_CRCL=(char)TX_Data_CRC;
						
						Update_Command_Data[TX_Data_crc_Length]=TX_Data_CRCH;	
						Update_Command_Data[TX_Data_crc_Length+1]=TX_Data_CRCL;//���CRCУ��
						
						Update_Command_Data[TX_Data_crc_Length+2]=0x0d;
						Update_Command_Data[TX_Data_crc_Length+3]=0x0a;//���ֹͣ��
						
						Update_Command_Data_OK=1;
						
						TX_Update_Command_Data_num=0;//�������ݼ�������
						
						UCB2IE|=UCRXIE;//�������ж�
	
						break;
					}
					
					case  ASK_Monitor_Update_Index://����������ʾ������������
					case  ASK_Controller_Update_Index://�����������������������
					case  ASK_Motor_Update_Index://�����������ſ���������������
					{
						Ask_index=1;
						Index=(SPIB2_DATA[6]<<8)+SPIB2_DATA[7];//������
						UCB2IE|=UCRXIE;//�������ж�
						break;
					}
					
					case  ASK_Monitor_Update_Index_content://��������������ʾ����ÿ����������
					case  ASK_Controller_Update_Index_content://�������������������ÿ����������
					case  ASK_Motor_Update_Index_content://���������������ſ�������ÿ����������
					{

						Module_Status[5]&=~0x0c;//��ÿ��������ɵı�־  
						
						Update_Data[0]=0x3c;
						Update_Data[1]=0x3a;
						
						Update_Data[2]=0xD0;
						Update_Data[3]=0x01;
						
						Update_Data[4]=0x04;
						Update_Data[5]=0x02;//1026���ֽ�
						
						//ȥ��������ȡ��ʾ��������
						//��ȷ��
						Update_Data[6]=(SPI_UP_BIT_SUM >>8);
						Update_Data[7]=SPI_UP_BIT_SUM ;//������
						
						for(Update_Datar_cnt=0;Update_Datar_cnt<1024;Update_Datar_cnt++)
						{
							Update_Data[Update_Datar_cnt+8]=UP_SYSTEM_C_LANG[Update_Datar_cnt];//X3�ĺ����������춨
						}
						
						TX_Data_crc_Length=1032;//430����������ҪУ��ĳ���
						TX_Data_CRC=crc_modbus2(Update_Data,TX_Data_crc_Length);//CRCУ������
						TX_Data_CRCH=(char)(TX_Data_CRC>>8);
						TX_Data_CRCL=(char)TX_Data_CRC;
						
						Update_Data[TX_Data_crc_Length]=TX_Data_CRCH;	
						Update_Data[TX_Data_crc_Length+1]=TX_Data_CRCL;//���CRCУ��
						
						Update_Data[TX_Data_crc_Length+2]=0x0d;
						Update_Data[TX_Data_crc_Length+3]=0x0a;//���ֹͣ��
						
						Update_Data_OK=1;
						
						TX_Update_Data_num=0;//�������ݼ�������
						
						UCB2IE|=UCRXIE;//�������ж�
	
						break;	
					}
					
					
					
					default:
						break;
				}                                     
			}
			
			else //crcУ�����
			{           
				switch(contents)
				{
					case SPI_WOSHOU://���������źŴ���
					{
						TX_Data_crc_Length=(SPI_T_WOSHOU_ERR[4]<<8)+SPI_T_WOSHOU_ERR[5]+6;//430����������ҪУ��ĳ���
						TX_Data_CRC=crc_modbus2(SPI_T_WOSHOU_ERR,TX_Data_crc_Length);//CRCУ������
						TX_Data_CRCH=(char)(TX_Data_CRC>>8);
						TX_Data_CRCL=(char)TX_Data_CRC;
						
						SPI_T_WOSHOU_ERR[TX_Data_crc_Length]=TX_Data_CRCH;	
						SPI_T_WOSHOU_ERR[TX_Data_crc_Length+1]=TX_Data_CRCL;//���CRCУ��
						
						SPI_T_WOSHOU_ERR[TX_Data_crc_Length+2]=0x0d;
						SPI_T_WOSHOU_ERR[TX_Data_crc_Length+3]=0x0a;//���ֹͣ��
						
						WOSHOU_CRC_ERR=1;
						
						TX_Data_num=0;//�������ݼ�������
						
						UCB2IE|=UCRXIE;//�������ж�
						break;
					}
					
					case SPI_SHEBEI_ID://�����豸ID����
					{
						TX_Data_crc_Length=(SPI_T_SHEBEI_ID_ERR[4]<<8)+SPI_T_SHEBEI_ID_ERR[5]+6;//430����������ҪУ��ĳ���
						TX_Data_CRC=crc_modbus2(SPI_T_SHEBEI_ID_ERR,TX_Data_crc_Length);//CRCУ������
						TX_Data_CRCH=(char)(TX_Data_CRC>>8);
						TX_Data_CRCL=(char)TX_Data_CRC;
						
						SPI_T_SHEBEI_ID_ERR[TX_Data_crc_Length]=TX_Data_CRCH;	
						SPI_T_SHEBEI_ID_ERR[TX_Data_crc_Length+1]=TX_Data_CRCL;//���CRCУ��
						
						SPI_T_SHEBEI_ID_ERR[TX_Data_crc_Length+2]=0x0d;
						SPI_T_SHEBEI_ID_ERR[TX_Data_crc_Length+3]=0x0a;//���ֹͣ��
						
						SHEBEI_ID_CRC_ERR=1;
						
						TX_Data_num=0;//�������ݼ�������
						
						UCB2IE|=UCRXIE;//�������ж�
						break;
					}
					
					case SPI_CONTENT_DATA://���ճ�̬���ݴ���
					{
						//�������ϱ�ƽ̨
						//��������Ӵ���
						//
						ARM_DATA_STU&=0xfe;
						UCB2IE|=UCRXIE;//�������ж�,������һ������
						break;
					}
					
					case SHEBEI_Soft_VER://�����豸����汾�Ŵ���
					{
						TX_Data_crc_Length=(SPI_T_SHEBEI_Soft_VER_ERR[4]<<8)+SPI_T_SHEBEI_Soft_VER_ERR[5]+6;//430����������ҪУ��ĳ���
						TX_Data_CRC=crc_modbus2(SPI_T_SHEBEI_Soft_VER_ERR,TX_Data_crc_Length);//CRCУ������
						TX_Data_CRCH=(char)(TX_Data_CRC>>8);
						TX_Data_CRCL=(char)TX_Data_CRC;
						
						SPI_T_SHEBEI_Soft_VER_ERR[TX_Data_crc_Length]=TX_Data_CRCH;	
						SPI_T_SHEBEI_Soft_VER_ERR[TX_Data_crc_Length+1]=TX_Data_CRCL;//���CRCУ��
						
						SPI_T_SHEBEI_Soft_VER_ERR[TX_Data_crc_Length+2]=0x0d;
						SPI_T_SHEBEI_Soft_VER_ERR[TX_Data_crc_Length+3]=0x0a;//���ֹͣ��
						
						SHEBEI_Soft_VER_CRC_ERR=1;
						
						TX_Data_num=0;//�������ݼ�������
						
						UCB2IE|=UCRXIE;//�������ж�
						break;
					}
					
					case ASK_GPS://��������GPS��Ϣ
					{
						UCB2IE|=UCRXIE;//�������ж�,������һ������
						break;
					}
					
					case ASK_GPS_VER://��������GPS�汾��
					{
						UCB2IE|=UCRXIE;//�������ж�,������һ������
						break;
					}
					
					case  ASK_Mode_Parameter://�����������ģʽ����
					{             
						
						UCB2IE|=UCRXIE;//�������ж�
									
						break;
					}
					
					case  ASK_Monitor_Update://����������ʾ��������
					{
						UCB2IE|=UCRXIE;//�������ж�,������һ������
						break;
					}
					
					case  ASK_Monitor_Update_Index://����������ʾ������������
					{
						UCB2IE|=UCRXIE;//�������ж�,������һ������
						break;
					}
					
					case  ASK_Controller_Update://�������������������
					{
						UCB2IE|=UCRXIE;//�������ж�,������һ������
						break;
					}
					
					case  ASK_Controller_Update_Index://�����������������������
					{
						UCB2IE|=UCRXIE;//�������ж�,������һ������
						break;
					}
					
					case  ASK_Motor_Update://�����������ſ�����������
					{
						UCB2IE|=UCRXIE;//�������ж�,������һ������
						break;
					}
					case  ASK_Motor_Update_Index://�����������ſ���������������
					{
						UCB2IE|=UCRXIE;//�������ж�,������һ������
						break;
					}
					case  ASK_Module_State://��������ģ��״̬
					{
						UCB2IE|=UCRXIE;//�������ж�,������һ������
						break;
					}
					default:
						break;	
			}
	  } 
			
	}	
	
	else//�����λ����ֹͣ��
	{
		UCB2IE|=UCRXIE;//�������ж�,������һ������
	}
								
	}//��Ӧ"if(flag_do)"  ��������������������
}