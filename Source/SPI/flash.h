
/*************************************************************************
		InfoFlash的地址映射

Info A: 0019FF~001980
Info B: 00197F~001900
Info C: 0018FF~001880
Info D: 00187F~001800
001800~001800 == ID标志位
001801~001804 == ID
001805~001806 == 锁车标志位
001807~001807 ==7== MCU数据上传时间标志位
001808~00180B == MCU数据上传时间间隔设置

00180C~00180C ==12== IP地址及端口号标志位
00180D~001812 ==13+=  IP地址及端口号

001813~001813 ==19== 越界标志位
001814~001823 ==20+= 越界经纬度值

001824~001824 ==36== 存储手机卡号标志位
001825~001838 ==37+= 手机卡号

001839~001839 ==57== 升级程序标志位
00183A~001842 ==58+= 升级程序种类1+升级文件版本号8+
001844~001845 ==68\69= 开盒标志

001846~001847 ==70\71= 无卡==0xC0； 换卡===0x30

001848~001849 ==72\73= 无卡允许==0xC0； 换卡允许===0x30

00184A        74      低功耗时间间隔设置标志

00184B~001856 ==75\86  低功耗时间间隔设置

001857~001856 ==87\86  低功耗时间间隔设置

001858~001859 ==88\89  换卡无卡报警次数

*************************************************************************/

#define Info_D_Addr 0x1800

/**********************************************************************
*名    称：Flash_Back2RAM
*功    能：备份InfoFlash的数据到RAM中的数组
*入口参数：Ptr：段起始地址
		   Array：备份数组名
		   length：备份数据长度
*出口参数：
***********************************************************************/
void Flash_Back2RAM(unsigned char*Ptr,unsigned char*Array,unsigned char length)
{
	unsigned char i;
	
	for(i=0;i<length;i++)
	{
		Array[i]=Ptr[i];//备份一个字节到RAM
	}
	
}
/**********************************************************************
*名    称：Flash_WriteChar
*功    能：向InfoFlash中随机写入一个字节
*入口参数：Addr：存放数据的地址（0~127）
		   Data：待存入的数据
*出口参数：
***********************************************************************/
void Flash_WriteChar(unsigned int Addr,unsigned char Data)
{
	unsigned char *Flash_ptrD;
	unsigned char *Flash_ptrB;
	unsigned char i;
	unsigned char back[100];

	Flash_ptrD=(unsigned char*)Info_D_Addr;
	
	Flash_ptrB=back;
	
	_DINT();//关总中断
	
	Flash_Back2RAM((unsigned char*)Info_D_Addr,back,100);//InfoFlash的数据备份到RAM的back[]数组中
	
	FCTL3=FWKEY;//清除锁定位
	
	FCTL1=FWKEY+ERASE;//段擦除
	*Flash_ptrD=0;//擦除段(需要23ms~32ms)
	while(FCTL3&BUSY);//等待擦除完成
	
	FCTL1=FWKEY+WRT; //字节写入	
	for(i=0;i<100;i++)
	{		
		if(i==Addr)//新写入数据地址
		{
			*Flash_ptrD++=Data;//写入新数据
			while((FCTL3&WAIT)==0);//等待写操作完成
			Flash_ptrB++;//跳过备份区内该单元（*不要漏掉此句）
		}
		
		else
		{
			*Flash_ptrD++=*Flash_ptrB++;//恢复原数据
			while((FCTL3&WAIT)==0);//等待写操作完成
		}
	}
	
	FCTL1=FWKEY;//退出写状态
	while(FCTL3&BUSY);//等待
    FCTL3=FWKEY+LOCK;//锁定
	
	_EINT();//开总中断	
}