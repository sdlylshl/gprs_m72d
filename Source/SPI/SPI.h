
/******************************************************************************
*名    称：SPI()

*功    能：1.解析处理ARM传来的数据
		   2.准备回应给ARM的数据

*入口参数：无

*出口参数：无

*说    明：无

*修改历史:无
******************************************************************************/

void SPI(void)
{
	if(flag_do)//接收完主机发来的一组数据    
	{ 
		flag_do=0;//清空标志，为下一次做准备
		lenth_crc=(SPIB2_DATA[4]<<8)+SPIB2_DATA[5]+8;//整个数据的长度（报头+数据类型+数据长度+数据内容+CRC）
		contents=(SPIB2_DATA[2]<<8)+SPIB2_DATA[3];//对收到的数据进行内容判断，确定是哪组数据
		if((SPIB2_DATA[lenth_crc]==0x0d)&&(SPIB2_DATA[lenth_crc+1]==0x0a))//最后两位如果是停止符
		{    
			if(!crc_modbus2(SPIB2_DATA,lenth_crc)) //如果CRC校验正确
			{
				switch(contents)  
				{
					case SPI_WOSHOU://接收握手信号正确
					{
						TX_Data_crc_Length=(SPI_T_WOSHOU_OK[4]<<8)+SPI_T_WOSHOU_OK[5]+6;//430发送数据需要校验的长度
						TX_Data_CRC=crc_modbus2(SPI_T_WOSHOU_OK,TX_Data_crc_Length);//CRC校验数据
						TX_Data_CRCH=(char)(TX_Data_CRC>>8);
						TX_Data_CRCL=(char)TX_Data_CRC;
						
						SPI_T_WOSHOU_OK[TX_Data_crc_Length]=TX_Data_CRCH;	
						SPI_T_WOSHOU_OK[TX_Data_crc_Length+1]=TX_Data_CRCL;//添加CRC校验
						
						SPI_T_WOSHOU_OK[TX_Data_crc_Length+2]=0x0d;
						SPI_T_WOSHOU_OK[TX_Data_crc_Length+3]=0x0a;//添加停止符
						
						WOSHOU_CRC_OK=1;//CRC校验成功，发给主机的数据 填写完整
						
						TX_Data_num=0;//发送数据计数清零
						UCB2IE|=UCRXIE;//开接收中断
						break;
					}
					
					
					case SPI_SHEBEI_ID://接收设备ID正确
					{						
						//只有当显示器上的ID号更改后，ARM才发送ID号，平时不发送
						
						for(ID_cnt=0;ID_cnt<4;ID_cnt++)
						{
							SHEBEI_ID_memory[ID_cnt]=SPIB2_DATA[6+ID_cnt];
						}
						
						ID_Flag=1;
						ID_Write_Flag=1;
                      
						
						TX_Data_crc_Length=(SPI_T_SHEBEI_ID_OK[4]<<8)+SPI_T_SHEBEI_ID_OK[5]+6;//430发送数据需要校验的长度
						TX_Data_CRC=crc_modbus2(SPI_T_SHEBEI_ID_OK,TX_Data_crc_Length);//CRC校验数据
						TX_Data_CRCH=(char)(TX_Data_CRC>>8);
						TX_Data_CRCL=(char)TX_Data_CRC;
						
						SPI_T_SHEBEI_ID_OK[TX_Data_crc_Length]=TX_Data_CRCH;	
						SPI_T_SHEBEI_ID_OK[TX_Data_crc_Length+1]=TX_Data_CRCL;//添加CRC校验
						
						SPI_T_SHEBEI_ID_OK[TX_Data_crc_Length+2]=0x0d;
						SPI_T_SHEBEI_ID_OK[TX_Data_crc_Length+3]=0x0a;//添加停止符
						
						SHEBEI_ID_CRC_OK=1;
						
						TX_Data_num=0;//发送数据计数清零
						
						UCB2IE|=UCRXIE;//开接收中断

						break;
					}
					
					case SPI_CONTENT_DATA://接收常态数据正确
					{
						//SPIB2_DATA[6]~SPIB2_DATA[67]这62个字节对应上报MCU数据命令的46字节到107字节
						//补充GPS定位信息
						//补充系统采集信息 附加数据标志 MCU数据等
						//加上常态数据 组成一包完整的数据
						//常态数据读走后 要打开接收中断 
						for(CONTENT_DATA_cnt=0;CONTENT_DATA_cnt<62;CONTENT_DATA_cnt++)
						{
							GSM_MCU[CONTENT_DATA_cnt]=SPIB2_DATA[6+CONTENT_DATA_cnt];//将常态数据转存
						}
						
						ARM_DATA_STU|=0x01;//置标志位
						
						UCB2IE|=UCRXIE;//开接收中断
						
						//
						//数据填完整后，蒋本庆将数据上传至平台
						//
						
						break;
					}
					
					case SHEBEI_Soft_VER://接收设备软件版本号正确(显示器，控制器，油门电机软件版本号)
					{
						for(Soft_VER_cnt=0;Soft_VER_cnt<24;Soft_VER_cnt++)
						{
							Soft_VER_memory[Soft_VER_cnt]=SPIB2_DATA[6+Soft_VER_cnt];//存储接收的软件版本号
						}
						
						for(Soft_VER_cnt=0;Soft_VER_cnt<8;Soft_VER_cnt++)
						{
							Soft_VER_memory[Soft_VER_cnt+24]=GPS_Soft_VER[Soft_VER_cnt];//GPS软件版本号
						}
                        
                        Soft_VER_OK=0X11;
						
						TX_Data_crc_Length=(SPI_T_SHEBEI_Soft_VER_OK[4]<<8)+SPI_T_SHEBEI_Soft_VER_OK[5]+6;//430发送数据需要校验的长度
						TX_Data_CRC=crc_modbus2(SPI_T_SHEBEI_Soft_VER_OK,TX_Data_crc_Length);//CRC校验数据
						TX_Data_CRCH=(char)(TX_Data_CRC>>8);
						TX_Data_CRCL=(char)TX_Data_CRC;
						
						SPI_T_SHEBEI_Soft_VER_OK[TX_Data_crc_Length]=TX_Data_CRCH;	
						SPI_T_SHEBEI_Soft_VER_OK[TX_Data_crc_Length+1]=TX_Data_CRCL;//添加CRC校验
						
						SPI_T_SHEBEI_Soft_VER_OK[TX_Data_crc_Length+2]=0x0d;
						SPI_T_SHEBEI_Soft_VER_OK[TX_Data_crc_Length+3]=0x0a;//添加停止符
						
						SHEBEI_Soft_VER_CRC_OK=1;
						
						TX_Data_num=0;//发送数据计数清零
						
						UCB2IE|=UCRXIE;//开接收中断
						
						//蒋本庆将软件版本号上传至平台
						//
						//
						break;
					}
					
					case ASK_GPS://主机请求GPS信息
					{                         
                        if(GPS_Data_OK_Flag==0x11)
						{
							GPS_Data_OK_Flag=0x00;//标志位清零
							GPS_temp[0]=0x3c;
							GPS_temp[1]=0x3a;
						
							GPS_temp[2]=0xa0;
							GPS_temp[3]=0x01;
						
							GPS_temp[4]=0x00;
							GPS_temp[5]=28;
						
							for(GPS_temp_cnt=0;GPS_temp_cnt<28;GPS_temp_cnt++)
							{
								GPS_temp[6+GPS_temp_cnt]=GPS_Inf[GPS_temp_cnt];//添加成功定位后的GPS数据
							}
							
							TX_Data_crc_Length=34;//430发送数据需要校验的长度
							TX_Data_CRC=crc_modbus2(GPS_temp,TX_Data_crc_Length);//CRC校验数据
							TX_Data_CRCH=(char)(TX_Data_CRC>>8);
							TX_Data_CRCL=(char)TX_Data_CRC;
							
							GPS_temp[TX_Data_crc_Length]=TX_Data_CRCH;	
							GPS_temp[TX_Data_crc_Length+1]=TX_Data_CRCL;//添加CRC校验
						
							GPS_temp[TX_Data_crc_Length+2]=0x0d;
							GPS_temp[TX_Data_crc_Length+3]=0x0a;//添加停止符
						
							GPS_temp_OK=1;
						
							GPS_Data_num=0;//发送数据计数清零
						}
						
						
						UCB2IE|=UCRXIE;//开接收中断
				
						break;
					}
					
					case ASK_GPS_VER://主机请求GPS版本号
					{
						GPS_mold_VER[0]=0x3c;
						GPS_mold_VER[1]=0x3a;
						
						GPS_mold_VER[2]=0x30;
						GPS_mold_VER[3]=0x03;
						
						GPS_mold_VER[4]=0x00;
						GPS_mold_VER[5]=8;
						
						for(GPS_mold_VER_cnt=0;GPS_mold_VER_cnt<8;GPS_mold_VER_cnt++)
						{
							GPS_mold_VER[GPS_mold_VER_cnt+6]=GPS_Soft_VER[GPS_mold_VER_cnt];//GPS版本号
						}
						
						TX_Data_crc_Length=14;//430发送数据需要校验的长度
						TX_Data_CRC=crc_modbus2(GPS_mold_VER,TX_Data_crc_Length);//CRC校验数据
						TX_Data_CRCH=(char)(TX_Data_CRC>>8);
						TX_Data_CRCL=(char)TX_Data_CRC;
						
						GPS_mold_VER[TX_Data_crc_Length]=TX_Data_CRCH;	
						GPS_mold_VER[TX_Data_crc_Length+1]=TX_Data_CRCL;//添加CRC校验
						
						GPS_mold_VER[TX_Data_crc_Length+2]=0x0d;
						GPS_mold_VER[TX_Data_crc_Length+3]=0x0a;//添加停止符
						
						GPS_VER_OK=1;
						
						GPS_VER_num=0;//发送数据计数清零
						
						UCB2IE|=UCRXIE;//开接收中断

						break;
					}
					
					case  ASK_Mode_Parameter://主机请求各档模式参数（主机根据模块状态的标志位决定是否申请该数据，申请了则说明主机已经发来了数据）
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

						TX_Data_crc_Length=26;//430发送数据需要校验的长度
						
						TX_Data_CRC=crc_modbus2(Mode_Parameter,TX_Data_crc_Length);//CRC校验数据
						TX_Data_CRCH=(char)(TX_Data_CRC>>8);
						TX_Data_CRCL=(char)TX_Data_CRC;
						
						Mode_Parameter[TX_Data_crc_Length]=TX_Data_CRCH;	
						Mode_Parameter[TX_Data_crc_Length+1]=TX_Data_CRCL;//添加CRC校验
						
						Mode_Parameter[TX_Data_crc_Length+2]=0x0d;
						Mode_Parameter[TX_Data_crc_Length+3]=0x0a;//添加停止符
						
						Mode_Parameter_OK=1;
						
						mode_num=0;//发送数据计数清零
						
						UCB2IE|=UCRXIE;//开接收中断
									
						break;
					}
					
					
					
					case ASK_Module_State://主机请求模块状态
					{
						Module_State[0]=0x3c;
						Module_State[1]=0x3a;
						
						Module_State[2]=0x60;
						Module_State[3]=0x06;
						
						Module_State[4]=0x00;
						Module_State[5]=6;
						
                        
                        if(One_Mint_Warn_Cnt<MSP_A0_Min_10)//一分钟计数)
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
						
						TX_Data_crc_Length=12;//430发送数据需要校验的长度
						TX_Data_CRC=crc_modbus2(Module_State,TX_Data_crc_Length);//CRC校验数据
						TX_Data_CRCH=(char)(TX_Data_CRC>>8);
						TX_Data_CRCL=(char)TX_Data_CRC;
						
						Module_State[TX_Data_crc_Length]=TX_Data_CRCH;	
						Module_State[TX_Data_crc_Length+1]=TX_Data_CRCL;//添加CRC校验
						
						Module_State[TX_Data_crc_Length+2]=0x0d;
						Module_State[TX_Data_crc_Length+3]=0x0a;//添加停止符
						
						Module_State_OK=1;
						
						Module_State_num=0;//发送数据计数清零
						
						UCB2IE|=UCRXIE;//开接收中断

						break;
					}
					
					case  ASK_Monitor_Update://主机请求显示器升级包
					case  ASK_Controller_Update://请求控制器的升级包 0xC0 0x02
					case  ASK_Motor_Update://请求油门控制器的升级包 0xC0 0x03
					{
						Update_Command_Data[0]=0x3c;
						Update_Command_Data[1]=0x3a;
						
						Update_Command_Data[2]=0xC0;
						Update_Command_Data[3]=0x01;
						
						Update_Command_Data[4]=0x00;
						Update_Command_Data[5]=18;//18个字节
						
						Module_Status[4] &= ~0x03;//收到升级标志，清除升级标志  

                        
						for(Update_Command_Datar_cnt=0;Update_Command_Datar_cnt<18;Update_Command_Datar_cnt++)
						{
							Update_Command_Data[Update_Command_Datar_cnt+6]=UP_Stat_SPI_BuF[Update_Command_Datar_cnt];
						}
						
						TX_Data_crc_Length=24;//430发送数据需要校验的长度
						TX_Data_CRC=crc_modbus2(Update_Command_Data,TX_Data_crc_Length);//CRC校验数据
						TX_Data_CRCH=(char)(TX_Data_CRC>>8);
						TX_Data_CRCL=(char)TX_Data_CRC;
						
						Update_Command_Data[TX_Data_crc_Length]=TX_Data_CRCH;	
						Update_Command_Data[TX_Data_crc_Length+1]=TX_Data_CRCL;//添加CRC校验
						
						Update_Command_Data[TX_Data_crc_Length+2]=0x0d;
						Update_Command_Data[TX_Data_crc_Length+3]=0x0a;//添加停止符
						
						Update_Command_Data_OK=1;
						
						TX_Update_Command_Data_num=0;//发送数据计数清零
						
						UCB2IE|=UCRXIE;//开接收中断
	
						break;
					}
					
					case  ASK_Monitor_Update_Index://主机请求显示器升级索引号
					case  ASK_Controller_Update_Index://主机请求控制器升级索引号
					case  ASK_Motor_Update_Index://主机请求油门控制器升级索引号
					{
						Ask_index=1;
						Index=(SPIB2_DATA[6]<<8)+SPIB2_DATA[7];//索引号
						UCB2IE|=UCRXIE;//开接收中断
						break;
					}
					
					case  ASK_Monitor_Update_Index_content://主机请求请求显示器的每包数据内容
					case  ASK_Controller_Update_Index_content://主机请求请求控制器的每包数据内容
					case  ASK_Motor_Update_Index_content://主机请求请求油门控制器的每包数据内容
					{

						Module_Status[5]&=~0x0c;//清每包数据完成的标志  
						
						Update_Data[0]=0x3c;
						Update_Data[1]=0x3a;
						
						Update_Data[2]=0xD0;
						Update_Data[3]=0x01;
						
						Update_Data[4]=0x04;
						Update_Data[5]=0x02;//1026个字节
						
						//去蒋本庆那取显示器升级包
						//待确定
						Update_Data[6]=(SPI_UP_BIT_SUM >>8);
						Update_Data[7]=SPI_UP_BIT_SUM ;//索引号
						
						for(Update_Datar_cnt=0;Update_Datar_cnt<1024;Update_Datar_cnt++)
						{
							Update_Data[Update_Datar_cnt+8]=UP_SYSTEM_C_LANG[Update_Datar_cnt];//X3的函数名蒋本庆定
						}
						
						TX_Data_crc_Length=1032;//430发送数据需要校验的长度
						TX_Data_CRC=crc_modbus2(Update_Data,TX_Data_crc_Length);//CRC校验数据
						TX_Data_CRCH=(char)(TX_Data_CRC>>8);
						TX_Data_CRCL=(char)TX_Data_CRC;
						
						Update_Data[TX_Data_crc_Length]=TX_Data_CRCH;	
						Update_Data[TX_Data_crc_Length+1]=TX_Data_CRCL;//添加CRC校验
						
						Update_Data[TX_Data_crc_Length+2]=0x0d;
						Update_Data[TX_Data_crc_Length+3]=0x0a;//添加停止符
						
						Update_Data_OK=1;
						
						TX_Update_Data_num=0;//发送数据计数清零
						
						UCB2IE|=UCRXIE;//开接收中断
	
						break;	
					}
					
					
					
					default:
						break;
				}                                     
			}
			
			else //crc校验错误
			{           
				switch(contents)
				{
					case SPI_WOSHOU://接收握手信号错误
					{
						TX_Data_crc_Length=(SPI_T_WOSHOU_ERR[4]<<8)+SPI_T_WOSHOU_ERR[5]+6;//430发送数据需要校验的长度
						TX_Data_CRC=crc_modbus2(SPI_T_WOSHOU_ERR,TX_Data_crc_Length);//CRC校验数据
						TX_Data_CRCH=(char)(TX_Data_CRC>>8);
						TX_Data_CRCL=(char)TX_Data_CRC;
						
						SPI_T_WOSHOU_ERR[TX_Data_crc_Length]=TX_Data_CRCH;	
						SPI_T_WOSHOU_ERR[TX_Data_crc_Length+1]=TX_Data_CRCL;//添加CRC校验
						
						SPI_T_WOSHOU_ERR[TX_Data_crc_Length+2]=0x0d;
						SPI_T_WOSHOU_ERR[TX_Data_crc_Length+3]=0x0a;//添加停止符
						
						WOSHOU_CRC_ERR=1;
						
						TX_Data_num=0;//发送数据计数清零
						
						UCB2IE|=UCRXIE;//开接收中断
						break;
					}
					
					case SPI_SHEBEI_ID://接收设备ID错误
					{
						TX_Data_crc_Length=(SPI_T_SHEBEI_ID_ERR[4]<<8)+SPI_T_SHEBEI_ID_ERR[5]+6;//430发送数据需要校验的长度
						TX_Data_CRC=crc_modbus2(SPI_T_SHEBEI_ID_ERR,TX_Data_crc_Length);//CRC校验数据
						TX_Data_CRCH=(char)(TX_Data_CRC>>8);
						TX_Data_CRCL=(char)TX_Data_CRC;
						
						SPI_T_SHEBEI_ID_ERR[TX_Data_crc_Length]=TX_Data_CRCH;	
						SPI_T_SHEBEI_ID_ERR[TX_Data_crc_Length+1]=TX_Data_CRCL;//添加CRC校验
						
						SPI_T_SHEBEI_ID_ERR[TX_Data_crc_Length+2]=0x0d;
						SPI_T_SHEBEI_ID_ERR[TX_Data_crc_Length+3]=0x0a;//添加停止符
						
						SHEBEI_ID_CRC_ERR=1;
						
						TX_Data_num=0;//发送数据计数清零
						
						UCB2IE|=UCRXIE;//开接收中断
						break;
					}
					
					case SPI_CONTENT_DATA://接收常态数据错误
					{
						//无数据上报平台
						//蒋本庆添加处理
						//
						ARM_DATA_STU&=0xfe;
						UCB2IE|=UCRXIE;//开接收中断,接收下一组数据
						break;
					}
					
					case SHEBEI_Soft_VER://接收设备软件版本号错误
					{
						TX_Data_crc_Length=(SPI_T_SHEBEI_Soft_VER_ERR[4]<<8)+SPI_T_SHEBEI_Soft_VER_ERR[5]+6;//430发送数据需要校验的长度
						TX_Data_CRC=crc_modbus2(SPI_T_SHEBEI_Soft_VER_ERR,TX_Data_crc_Length);//CRC校验数据
						TX_Data_CRCH=(char)(TX_Data_CRC>>8);
						TX_Data_CRCL=(char)TX_Data_CRC;
						
						SPI_T_SHEBEI_Soft_VER_ERR[TX_Data_crc_Length]=TX_Data_CRCH;	
						SPI_T_SHEBEI_Soft_VER_ERR[TX_Data_crc_Length+1]=TX_Data_CRCL;//添加CRC校验
						
						SPI_T_SHEBEI_Soft_VER_ERR[TX_Data_crc_Length+2]=0x0d;
						SPI_T_SHEBEI_Soft_VER_ERR[TX_Data_crc_Length+3]=0x0a;//添加停止符
						
						SHEBEI_Soft_VER_CRC_ERR=1;
						
						TX_Data_num=0;//发送数据计数清零
						
						UCB2IE|=UCRXIE;//开接收中断
						break;
					}
					
					case ASK_GPS://主机请求GPS信息
					{
						UCB2IE|=UCRXIE;//开接收中断,接收下一组数据
						break;
					}
					
					case ASK_GPS_VER://主机请求GPS版本号
					{
						UCB2IE|=UCRXIE;//开接收中断,接收下一组数据
						break;
					}
					
					case  ASK_Mode_Parameter://主机请求各档模式参数
					{             
						
						UCB2IE|=UCRXIE;//开接收中断
									
						break;
					}
					
					case  ASK_Monitor_Update://主机请求显示器升级包
					{
						UCB2IE|=UCRXIE;//开接收中断,接收下一组数据
						break;
					}
					
					case  ASK_Monitor_Update_Index://主机请求显示器升级索引号
					{
						UCB2IE|=UCRXIE;//开接收中断,接收下一组数据
						break;
					}
					
					case  ASK_Controller_Update://主机请求控制器升级包
					{
						UCB2IE|=UCRXIE;//开接收中断,接收下一组数据
						break;
					}
					
					case  ASK_Controller_Update_Index://主机请求控制器升级索引号
					{
						UCB2IE|=UCRXIE;//开接收中断,接收下一组数据
						break;
					}
					
					case  ASK_Motor_Update://主机请求油门控制器升级包
					{
						UCB2IE|=UCRXIE;//开接收中断,接收下一组数据
						break;
					}
					case  ASK_Motor_Update_Index://主机请求油门控制器升级索引号
					{
						UCB2IE|=UCRXIE;//开接收中断,接收下一组数据
						break;
					}
					case  ASK_Module_State://主机请求模块状态
					{
						UCB2IE|=UCRXIE;//开接收中断,接收下一组数据
						break;
					}
					default:
						break;	
			}
	  } 
			
	}	
	
	else//最后两位不是停止符
	{
		UCB2IE|=UCRXIE;//开接收中断,接收下一组数据
	}
								
	}//对应"if(flag_do)"  处理完主机发来的数据
}