//#include <intrins.h>
//#include "reg52.h"

#include "mfrc522.h"
#include <string.h> 
#include "pin_config.h"
#include "stdio.h"
#include "motor.h"
#include "stm32f10x.h"
#include "24l01.h"

#define MAXRLEN 18  
unsigned char rc522;

u16 tim2=0,tim3=0,tim4=0;

/////////////////////////////////////////////////////////////////////
//功    能：寻卡
//参数说明: req_code[IN]:寻卡方式
//                0x52 = 寻感应区内所有符合14443A标准的卡
//                0x26 = 寻未进入休眠状态的卡
//          	  pTagType[OUT]：卡片类型代码
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRequest(u8   req_code,u8 *pTagType)
{
   char status;  
   unsigned int  unLen;
   unsigned char ucComMF522Buf[MAXRLEN];  
	ClearBitMask(Status2Reg,0x08);
	WriteRawRC(BitFramingReg,0x07);
	SetBitMask(TxControlReg,0x03);
 
	ucComMF522Buf[0] = req_code;

	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);
	
	if ((status == MI_OK) && (unLen == 0x10))
	{    
		*pTagType     = ucComMF522Buf[0];
		*(pTagType+1) = ucComMF522Buf[1];
	}
	else
	{   status = MI_ERR;   }
   
	return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：防冲撞
//参数说明: pSnr[OUT]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////  
char PcdAnticoll(unsigned char *pSnr)
{
    char status;
    unsigned char i,snr_check=0;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
    

    ClearBitMask(Status2Reg,0x08);
    WriteRawRC(BitFramingReg,0x00);
    ClearBitMask(CollReg,0x80);
 
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if (status == MI_OK)
    {
    	 for (i=0; i<4; i++)
         {   
             *(pSnr+i)  = ucComMF522Buf[i];
             snr_check ^= ucComMF522Buf[i];
         }
         if (snr_check != ucComMF522Buf[i])
         {   status = MI_ERR;    }
    }
    
    SetBitMask(CollReg,0x80);
    return status;
}
/////////////////////////////////////////////////////////////////////
//功    能：选定卡片
//参数说明: pSnr[IN]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdSelect(unsigned char *pSnr)
{
    char status;
    unsigned char i;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++)
    {
    	ucComMF522Buf[i+2] = *(pSnr+i);
    	ucComMF522Buf[6]  ^= *(pSnr+i);
    }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);
  
    ClearBitMask(Status2Reg,0x08);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);
    
    if ((status == MI_OK) && (unLen == 0x18))
    {   status = MI_OK;  }
    else
    {   status = MI_ERR;    }

    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：验证卡片密码
//参数说明: auth_mode[IN]: 密码验证模式
//                 0x60 = 验证A密钥
//                 0x61 = 验证B密钥 
//          addr[IN]：块地址
//          pKey[IN]：密码
//          pSnr[IN]：卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////               
char PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    for (i=0; i<6; i++)
    {    ucComMF522Buf[i+2] = *(pKey+i);   }
    for (i=0; i<4; i++)
    {    ucComMF522Buf[i+8] = *(pSnr+i);   }
 //   memcpy(&ucComMF522Buf[2], pKey, 6); 
 //   memcpy(&ucComMF522Buf[8], pSnr, 4); 
    
    status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
    {   status = MI_ERR;   }
    
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：读取M1卡一块数据
//参数说明: addr[IN]：块地址
//          pData[OUT]：读出的数据，16字节
//返    回: 成功返回MI_OK
///////////////////////////////////////////////////////////////////// 
char PcdRead(unsigned char addr,unsigned char *pData)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
   
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if ((status == MI_OK) && (unLen == 0x90))
 //   {   memcpy(pData, ucComMF522Buf, 16);   }
    {
        for (i=0; i<16; i++)
        {    *(pData+i) = ucComMF522Buf[i];   }
    }
    else
    {   status = MI_ERR;   }
    
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：写数据到M1卡一块
//参数说明: addr[IN]：块地址
//          pData[IN]：写入的数据，16字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////                  
char PcdWrite(unsigned char addr,unsigned char *pData)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
        
    if (status == MI_OK)
    {
        //memcpy(ucComMF522Buf, pData, 16);

        for (i=0; i<16; i++)
        {    ucComMF522Buf[i] = *(pData+i);   }
        CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);

        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    
    return status;
}



/////////////////////////////////////////////////////////////////////
//功    能：命令卡片进入休眠状态
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdHalt(void)
{
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//用MF522计算CRC16函数
/////////////////////////////////////////////////////////////////////
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData)
{
    unsigned char i,n;
    ClearBitMask(DivIrqReg,0x04);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    for (i=0; i<len; i++)
    {   WriteRawRC(FIFODataReg, *(pIndata+i));   }
    WriteRawRC(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do 
    {
        n = ReadRawRC(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));
    pOutData[0] = ReadRawRC(CRCResultRegL);
    pOutData[1] = ReadRawRC(CRCResultRegM);
}

/////////////////////////////////////////////////////////////////////
//功    能：复位RC522
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdReset(void)
{
    if(rc522==0)
	{
		RC522_RST_0_H ;
		delay_ms(1);
		RC522_RST_0_L ;
		delay_ms(1);
		RC522_RST_0_H ;
	}
	else
	{
		RC522_RST_1_H ;
		delay_ms(1);
		RC522_RST_1_L ;
		delay_ms(1);
		RC522_RST_1_H ;
	}
    delay_ms(10);

	if(ReadRawRC(0x02) == 0x80)
	{
		delay_ms(10);	
		delay_ms(10);	
		delay_ms(10);	
		delay_ms(10);	
	}

    WriteRawRC(CommandReg,PCD_RESETPHASE);
    
    WriteRawRC(ModeReg,0x3D);            //和Mifare卡通讯，CRC初始值0x6363
    WriteRawRC(TReloadRegL,30);           
    WriteRawRC(TReloadRegH,0);
    WriteRawRC(TModeReg,0x8D);
    WriteRawRC(TPrescalerReg,0x3E);
    WriteRawRC(TxAutoReg,0x40);     
    return MI_OK;
}
//////////////////////////////////////////////////////////////////////
//设置RC632的工作方式 
//////////////////////////////////////////////////////////////////////
char M500PcdConfigISOType(u8   type)
{
   if (type == 'A')                     //ISO14443_A
   { 
       ClearBitMask(Status2Reg,0x08);
       WriteRawRC(ModeReg,0x3D);//3F
       WriteRawRC(RxSelReg,0x86);//84
       WriteRawRC(RFCfgReg,0x7F);   //4F
   	   WriteRawRC(TReloadRegL,30);//tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
	   WriteRawRC(TReloadRegH,0);
       WriteRawRC(TModeReg,0x8D);
	   WriteRawRC(TPrescalerReg,0x3E);
	   delay_ms(1);
       PcdAntennaOn();
   }
   else{ return 1; }
   
   return MI_OK;
}
/////////////////////////////////////////////////////////////////////
//功    能：读RC632寄存器
//参数说明：Address[IN]:寄存器地址
//返    回：读出的值
/////////////////////////////////////////////////////////////////////
u8 ReadRawRC(u8 Address)
{
    u8   ucAddr;
    u8   ucResult=0;
		if(rc522==0)
	{	
		RC522_NSS_0_L	;
		SCK_L;
		ucAddr = ((Address<<1)&0x7E)|0x80;
		
		SPIWriteByte(ucAddr);
		ucResult=SPIWriteByte(0);
		
		RC522_NSS_0_H	;
		SCK_H;                                                                                                                                                                               
	}
	else
	{
		RC522_NSS_1_L	;
		SCK_L;
		ucAddr = ((Address<<1)&0x7E)|0x80;
		
		SPIWriteByte(ucAddr);
		ucResult=SPIWriteByte(0);
		
		RC522_NSS_1_H	;
		SCK_H;
	}
	
   return ucResult;
}

/////////////////////////////////////////////////////////////////////
//功    能：写RC632寄存器
//参数说明：Address[IN]:寄存器地址
//          value[IN]:写入的值
/////////////////////////////////////////////////////////////////////
void WriteRawRC(u8 Address, u8 value)
{  
    u8 ucAddr;
	if(rc522==0)
	{	
		RC522_NSS_0_L	;
		SCK_L;
		ucAddr = ((Address<<1)&0x7E);

		SPIWriteByte(ucAddr);
		SPIWriteByte(value);
//		printf("%d write",rc522);
		RC522_NSS_0_H	;
		SCK_H;                                                                                                                                                                                  
	}
	else
	{
		RC522_NSS_1_L	;
		SCK_L;
		ucAddr = ((Address<<1)&0x7E);

		SPIWriteByte(ucAddr);  
		SPIWriteByte(value);
//		printf("%dwrite",rc522);
		RC522_NSS_1_H	;
		SCK_H;  
	}
}

/////////////////////////////////////////////////////////////////////
//功    能：置RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:置位值
/////////////////////////////////////////////////////////////////////
void SetBitMask(unsigned char reg,unsigned char mask)  
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg,tmp | mask);  // set bit mask
}

/////////////////////////////////////////////////////////////////////
//功    能：清RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:清位值
/////////////////////////////////////////////////////////////////////
void ClearBitMask(unsigned char reg,unsigned char mask)  
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask);  // clear bit mask
} 

/////////////////////////////////////////////////////////////////////
//功    能：通过RC522和ISO14443卡通讯
//参数说明：Command[IN]:RC522命令字
//          pInData[IN]:通过RC522发送到卡片的数据
//          InLenByte[IN]:发送数据的字节长度
//          pOutData[OUT]:接收到的卡片返回数据
//          *pOutLenBit[OUT]:返回数据的位长度
/////////////////////////////////////////////////////////////////////
char PcdComMF522(unsigned char Command, 
                 unsigned char *pInData, 
                 unsigned char InLenByte,
                 unsigned char *pOutData, 
                 unsigned int  *pOutLenBit)
{
    char status = MI_ERR;
    unsigned char irqEn   = 0x00;
    unsigned char waitFor = 0x00;
    unsigned char lastBits;
    unsigned char n;
    unsigned int i;
    switch (Command)
    {
       case PCD_AUTHENT:
          irqEn   = 0x12;
          waitFor = 0x10;
          break;
       case PCD_TRANSCEIVE:
          irqEn   = 0x77;
          waitFor = 0x30;
          break;
       default:
         break;
    }
   
    WriteRawRC(ComIEnReg,irqEn|0x80);
    ClearBitMask(ComIrqReg,0x80);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    
    for (i=0; i<InLenByte; i++)
    {   WriteRawRC(FIFODataReg, pInData[i]);    }
    WriteRawRC(CommandReg, Command);
   
    
    if (Command == PCD_TRANSCEIVE)
    {    SetBitMask(BitFramingReg,0x80);  }
    
//    i = 600;//根据时钟频率调整，操作M1卡最大等待时间25ms
    i = 2500;
    do 
    {
         n = ReadRawRC(ComIrqReg);
         i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitFor));
    ClearBitMask(BitFramingReg,0x80);
	      
    if (i!=0)
    {    
         if(!(ReadRawRC(ErrorReg)&0x1B))
         {
             status = MI_OK;
             if (n & irqEn & 0x01)
             {   status = MI_NOTAGERR;   }
             if (Command == PCD_TRANSCEIVE)
             {
               	n = ReadRawRC(FIFOLevelReg);
              	lastBits = ReadRawRC(ControlReg) & 0x07;
                if (lastBits)
                {   *pOutLenBit = (n-1)*8 + lastBits;   }
                else
                {   *pOutLenBit = n*8;   }
                if (n == 0)
                {   n = 1;    }
                if (n > MAXRLEN)
                {   n = MAXRLEN;   }
                for (i=0; i<n; i++)
                {   pOutData[i] = ReadRawRC(FIFODataReg);    }
            }
         }
         else
         {   status = MI_ERR;   }
        
   }
   

   SetBitMask(ControlReg,0x80);           // stop timer now
   WriteRawRC(CommandReg,PCD_IDLE); 
   return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：扣款和充值
//参数说明: dd_mode[IN]：命令字
//               0xC0 = 扣款
//               0xC1 = 充值
//          addr[IN]：钱包地址
//          pValue[IN]：4字节增(减)值，低位在前
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////                 
char PcdValue(unsigned char dd_mode,unsigned char addr,unsigned char *pValue)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = dd_mode;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
        
    if (status == MI_OK)
    {
       // memcpy(ucComMF522Buf, pValue, 4);
        for (i=0; i<16; i++)
        {    ucComMF522Buf[i] = *(pValue+i);   }
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
        unLen = 0;
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
        if (status != MI_ERR)
        {    status = MI_OK;    }
    }
    
    if (status == MI_OK)
    {
        ucComMF522Buf[0] = PICC_TRANSFER;
        ucComMF522Buf[1] = addr;
        CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]); 
   
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：备份钱包
//参数说明: sourceaddr[IN]：源地址
//          goaladdr[IN]：目标地址
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdBakValue(unsigned char sourceaddr, unsigned char goaladdr)
{
    char status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_RESTORE;
    ucComMF522Buf[1] = sourceaddr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
    
    if (status == MI_OK)
    {
        ucComMF522Buf[0] = 0;
        ucComMF522Buf[1] = 0;
        ucComMF522Buf[2] = 0;
        ucComMF522Buf[3] = 0;
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
 
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
        if (status != MI_ERR)
        {    status = MI_OK;    }
    }
    
    if (status != MI_OK)
    {    return MI_ERR;   }
    
    ucComMF522Buf[0] = PICC_TRANSFER;
    ucComMF522Buf[1] = goaladdr;

    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }

    return status;
}



/////////////////////////////////////////////////////////////////////
//开启天线  
//每次启动或关闭天险发射之间应至少有1ms的间隔
/////////////////////////////////////////////////////////////////////
void PcdAntennaOn()
{
    unsigned char i;
    i = ReadRawRC(TxControlReg);
    if (!(i & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
}

/////////////////////////////////////////////////////////////////////
//关闭天线
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff()
{
    ClearBitMask(TxControlReg, 0x03);
}

//等待卡离开
void WaitCardOff(void)
{
	char          status;
  unsigned char	TagType[2];

	while(1)
	{
		status = PcdRequest(REQ_ALL, TagType);
		if(status)
		{
			status = PcdRequest(REQ_ALL, TagType);
			if(status)
			{
				status = PcdRequest(REQ_ALL, TagType);
				if(status)
				{
					return;
				}
			}
		}
		delay_ms(100);
	}
}
void USART_SEND(u8 *ptr)
{
	u8 i;  
	for(i=0;i<11;i++)
   {	  
		USART_SendData(USART1,*(ptr+i)); // 发送数据
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
   }                        
}
void RC522_Init(void)              //读卡器初始化
{	
	//读卡器0 初始化
	rc522=0;
	RC522_NSS_1_H;
	NRF24L01_CSN_H;
	PcdReset();
	PcdAntennaOff();
	PcdAntennaOn();  
	M500PcdConfigISOType( 'A' );
	//读卡器1 初始化       
	rc522=1;
	RC522_NSS_0_H;
	NRF24L01_CSN_H;
	PcdReset();
	PcdAntennaOff(); 
	PcdAntennaOn();  
	M500PcdConfigISOType( 'A' );
}
void RC522_0_Handler(void)		//读卡器0处理程序
{
	u8 temp,g_ucTempbuf[2];
	u8 card[4],send[11]="0#1#0000#1@";
	u16 i;
	char status;
	rc522=0;
	RC522_NSS_1_H;
	NRF24L01_CSN_H;
	status = PcdRequest(REQ_ALL,g_ucTempbuf);
//	printf("测试！！！");
	if(status==MI_OK)
	{
		status = PcdAnticoll(card);	 //防冲撞
	
		if (status== MI_OK)
		{ 		   
			printf("卡序列：=%02x %02x %02x %02x\r\n",card[0],card[1],card[2],card[3]);
			send[5]=card[0]/100+'0';  //仅输入卡号第一字节
			temp=card[0]%100;
			send[6]=temp/10+'0';
			send[7]=temp%10+'0';
			status = PcdSelect(card);
    //	operation(data);//操作门和道路灯函数 
//			printf("open door");
			if(status==MI_OK)
			{
				WaitCardOff();            
				delay_ms(10);
//			NRF24L01_TxPacket(send);
			  SEND_BUF(send);//发送数组到上位机，等待回传数据
				
			}
		}
	}			
//	NRF24L01_RX_Mode(); 				//切换成接收模式					
	while(NRF24L01_IRQ==0)		//这个能接收到上位机的预约号和道路卡的固定卡号，预约号格式：0#1#03 卡号格式：1#02
	{	
		u8 data[4]={0};
		NRF24L01_RxPacket(data);//获取数据
		printf("接收=%s",data);//测试数据是否正确
		//NRF24L01_CE_L;             //暂时关闭无线不然会影响电机正常工作                               
		//NRF24L01_CSN_H;	
		//	delay_ms(500);
		operation(data);//操作门和道路灯函数 			
	}		
	if(tim2==10)
	{
		for(i=0;i<130;i++)
		{
			MOTOR0_Reverse(2);//MOTOR0_Foreward(2);
		}	
		MOTOR0_Stop();
		//NRF24L01_CE_H;     //若上位机一直发送预约号，会造成一直进入接收函数，影响门闸的正常工作，                           
		//NRF24L01_CSN_L;	//所以，这是一开始调试的时候的解决方法，让其接收一次就关闭使能，等操作完才打开使能，等待下一次的接收
		//NRF24L01_1_RX_Mode();//修改频段，改成与小车通信的频段80，等待接收小车的卡号消息
	}	
	if(tim3==20)
	{
		for(i=0;i<130;i++)
		{
			MOTOR1_Reverse(2);
		}	
		MOTOR1_Stop();
	}
}	
void RC522_1_Handler(void)		//读卡器1处理程序
{
	u8 g_ucTempbuf[2];
	u8 card[4],send[11]="0#1#0138#0@";
	char status;
	rc522=1;
	RC522_NSS_0_H;
	NRF24L01_CSN_H;
	status = PcdRequest(REQ_ALL,g_ucTempbuf);
	if (status==MI_OK)
	{	
		delay_ms(10);
		status = PcdAnticoll(card);	 //防冲撞
		if (status== MI_OK)
		{  		 
			printf("卡序列：=%02x %02x %02x %02x\r\n",card[0],card[1],card[2],card[3]);
//			send[5]=card[0]/100+'0';  //仅输入卡号第一字节
//			temp=card[0]%100;
//			send[6]=temp/10+'0';
//			send[7]=temp%10+'0';
			status = PcdSelect(card);
			if(status==MI_OK)
			{
				WaitCardOff();
				delay_ms(10);
				SEND_BUF(send);
			}
		} 
			
	}		   

}
void led_handler(void)
{
	u16 a;
	u8 i;
	a=0x0001;	
	for(i=0;i<16;i++)
	{
		a=a<<1;
		a=a&0xffff;
		GPIO_Write(GPIOD,a);
		delay_ms(500);		
	}	
}
void open_door(void)
{
	u16 i;
	for(i=0;i<130;i++)
	{
		MOTOR0_Foreward(2);//MOTOR0_Reverse(2); 
	}	
	TIM2_ON;	
}
void out_open(void)
{
	u8 i;
	for(i=0;i<150;i++)
	{
		MOTOR1_Foreward(2);
	}
		TIM3_ON;
}
	
void elevator_up(void)
{
	u16 i;
	for(i=0;i<1550;i++)
	{
		 elevator_Foreward(2);
	}
	elevator_Stop();
}
void elevator_down(void)
{
	u16 i;
	for(i=0;i<1550;i++)
	{
		elevator_Reverse(2);
	}
	elevator_Stop();
}	

void operation(u8 *data)  //一共会接收到4种数据，预约号 2层定位号 停车号 出库号
{
	if(data[2]=='#')  //预约号 01#3
	{	
		if(data[3]>'0'&& data[3]<='7')//'1'代表是第几层，返回的正确格式为 01#3 ，代表已预约1层，预约了3号车位
		{
			GPIO_Write(GPIOD,0x0000);
			open_door();
		}
//		if (data[3]>'4')//''代表是第2层，返回的正确格式为 01#7 ，代表已预约2层，预约了7号车位
//		{
			elevator_up();
			switch(data[3])// 注意：上位机返回的格式只有4个数据长度，我利用第4个字节来
			{
				 case  '5':{	GPIO_Write(GPIOD,0x0012);	
							}break;
				 case  '6':{	GPIO_Write(GPIOD,0x0038);			
							}break;
				 case  '7':{	GPIO_Write(GPIOD,0x005C);			
							}break;		 	
			}
//		}
	}
	if(data[0]=='#')  //定位号 #7 #8 #9   
	{	
		switch(data[1])// 注意：上位机返回的格式只有4个数据长度，我利用第4个字节来
			{
				 case  '7':{	GPIO_Write(GPIOD,0x0028);		
							}break;
				 case  '8':{	
//								while(data[1]!='7')
//								{
//									printf("eeee");
//									GPIO_Write(GPIOD,0x0028);
//									break;
//								}
								GPIO_Write(GPIOD,0x0020);	
							}break;
				 case  '9':{	GPIO_Write(GPIOD,0xFFF2);//0xFFC0			
							}break;		 	
			}	
	}
	if(data[1]=='#')  //停车号 7#1#0000#1@ 8#1#0000#1@  出车号 6#0#0000#1@
	{
		if(data[9]=='1')
		{	
			SEND_BUF(data);
			if(data[2]=='1')  //停车号 
			{
				delay_ms(1000);
				delay_ms(1000);
					switch(data[0])// 注意：上位机返回的格式只有4个数据长度，我利用第4个字节来
				{
					 case  '5':{	GPIO_Write(GPIOD,0x0000);	 
								}break;
					 case  '6':{	GPIO_Write(GPIOD,0x0000);			
								}break;
					 case  '7':{	GPIO_Write(GPIOD,0x0000);			
								}break;		 	
				}
			}		
			if(data[2]=='0')  //出车号 
			{
				if(data[0]>'0'&& data[0]<='4')
				{
					out_open();	
				}	
				else
				{	
					GPIO_Write(GPIOD,0x0300);  //亮绿灯 亮出库道路灯
				}	
			}
		}	
		else //出库号 0#1#0196#0@
		{	
			SEND_BUF(data);
			elevator_down();
		}	
	} 
	if(data[1]=='@')  //调试电机 @
	{
		GPIO_Write(GPIOD,0xFFFF);
		elevator_down();
	}	
}
void location(u8 *data)
{		
//	NRF24L01_CE_L;	  
//	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);       //设置RF通信频率	频率40	  
//  	NRF24L01_CE_H; 								
	SEND_BUF(data);									//发送到上位机让其同步定位信息
}
///////////////////////////////////////////////////////////////////////
// Delay 10ms
///////////////////////////////////////////////////////////////////////
void delay_ms(int nms)
{
	 u32 temp;
	 SysTick->LOAD = 9000*nms;
	 SysTick->VAL=0X00;//清空计数器
	 SysTick->CTRL=0X01;//使能，减到零是无动作，采用外部时钟源
	 do
	 {
	  temp=SysTick->CTRL;//读取当前倒计数值
	 }while((temp&0x01)&&(!(temp&(1<<16))));//等待时间到达
	 SysTick->CTRL=0x00; //关闭计数器
	 SysTick->VAL =0X00; //清空计数器
}
void delay_us(u32 nus)
{
	 u32 temp;
	 SysTick->LOAD = 9*nus;
	 SysTick->VAL=0X00;//清空计数器
	 SysTick->CTRL=0X01;//使能，减到零是无动作，采用外部时钟源
	 do
	 {
	  temp=SysTick->CTRL;//读取当前倒计数值
	 }while((temp&0x01)&&(!(temp&(1<<16))));//等待时间到达
	 
	 SysTick->CTRL=0x00; //关闭计数器
	 SysTick->VAL =0X00; //清空计数器
}


