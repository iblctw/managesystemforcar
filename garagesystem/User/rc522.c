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
//��    �ܣ�Ѱ��
//����˵��: req_code[IN]:Ѱ����ʽ
//                0x52 = Ѱ��Ӧ�������з���14443A��׼�Ŀ�
//                0x26 = Ѱδ��������״̬�Ŀ�
//          	  pTagType[OUT]����Ƭ���ʹ���
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ�����ײ
//����˵��: pSnr[OUT]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ�ѡ����Ƭ
//����˵��: pSnr[IN]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ���֤��Ƭ����
//����˵��: auth_mode[IN]: ������֤ģʽ
//                 0x60 = ��֤A��Կ
//                 0x61 = ��֤B��Կ 
//          addr[IN]�����ַ
//          pKey[IN]������
//          pSnr[IN]����Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ���ȡM1��һ������
//����˵��: addr[IN]�����ַ
//          pData[OUT]�����������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ�д���ݵ�M1��һ��
//����˵��: addr[IN]�����ַ
//          pData[IN]��д������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ����Ƭ��������״̬
//��    ��: �ɹ�����MI_OK
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
//��MF522����CRC16����
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
//��    �ܣ���λRC522
//��    ��: �ɹ�����MI_OK
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
    
    WriteRawRC(ModeReg,0x3D);            //��Mifare��ͨѶ��CRC��ʼֵ0x6363
    WriteRawRC(TReloadRegL,30);           
    WriteRawRC(TReloadRegH,0);
    WriteRawRC(TModeReg,0x8D);
    WriteRawRC(TPrescalerReg,0x3E);
    WriteRawRC(TxAutoReg,0x40);     
    return MI_OK;
}
//////////////////////////////////////////////////////////////////////
//����RC632�Ĺ�����ʽ 
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
//��    �ܣ���RC632�Ĵ���
//����˵����Address[IN]:�Ĵ�����ַ
//��    �أ�������ֵ
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
//��    �ܣ�дRC632�Ĵ���
//����˵����Address[IN]:�Ĵ�����ַ
//          value[IN]:д���ֵ
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
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
/////////////////////////////////////////////////////////////////////
void SetBitMask(unsigned char reg,unsigned char mask)  
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg,tmp | mask);  // set bit mask
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
/////////////////////////////////////////////////////////////////////
void ClearBitMask(unsigned char reg,unsigned char mask)  
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask);  // clear bit mask
} 

/////////////////////////////////////////////////////////////////////
//��    �ܣ�ͨ��RC522��ISO14443��ͨѶ
//����˵����Command[IN]:RC522������
//          pInData[IN]:ͨ��RC522���͵���Ƭ������
//          InLenByte[IN]:�������ݵ��ֽڳ���
//          pOutData[OUT]:���յ��Ŀ�Ƭ��������
//          *pOutLenBit[OUT]:�������ݵ�λ����
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
    
//    i = 600;//����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
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
//��    �ܣ��ۿ�ͳ�ֵ
//����˵��: dd_mode[IN]��������
//               0xC0 = �ۿ�
//               0xC1 = ��ֵ
//          addr[IN]��Ǯ����ַ
//          pValue[IN]��4�ֽ���(��)ֵ����λ��ǰ
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ�����Ǯ��
//����˵��: sourceaddr[IN]��Դ��ַ
//          goaladdr[IN]��Ŀ���ַ
//��    ��: �ɹ�����MI_OK
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
//��������  
//ÿ��������ر����շ���֮��Ӧ������1ms�ļ��
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
//�ر�����
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff()
{
    ClearBitMask(TxControlReg, 0x03);
}

//�ȴ����뿪
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
		USART_SendData(USART1,*(ptr+i)); // ��������
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
   }                        
}
void RC522_Init(void)              //��������ʼ��
{	
	//������0 ��ʼ��
	rc522=0;
	RC522_NSS_1_H;
	NRF24L01_CSN_H;
	PcdReset();
	PcdAntennaOff();
	PcdAntennaOn();  
	M500PcdConfigISOType( 'A' );
	//������1 ��ʼ��       
	rc522=1;
	RC522_NSS_0_H;
	NRF24L01_CSN_H;
	PcdReset();
	PcdAntennaOff(); 
	PcdAntennaOn();  
	M500PcdConfigISOType( 'A' );
}
void RC522_0_Handler(void)		//������0�������
{
	u8 temp,g_ucTempbuf[2];
	u8 card[4],send[11]="0#1#0000#1@";
	u16 i;
	char status;
	rc522=0;
	RC522_NSS_1_H;
	NRF24L01_CSN_H;
	status = PcdRequest(REQ_ALL,g_ucTempbuf);
//	printf("���ԣ�����");
	if(status==MI_OK)
	{
		status = PcdAnticoll(card);	 //����ײ
	
		if (status== MI_OK)
		{ 		   
			printf("�����У�=%02x %02x %02x %02x\r\n",card[0],card[1],card[2],card[3]);
			send[5]=card[0]/100+'0';  //�����뿨�ŵ�һ�ֽ�
			temp=card[0]%100;
			send[6]=temp/10+'0';
			send[7]=temp%10+'0';
			status = PcdSelect(card);
    //	operation(data);//�����ź͵�·�ƺ��� 
//			printf("open door");
			if(status==MI_OK)
			{
				WaitCardOff();            
				delay_ms(10);
//			NRF24L01_TxPacket(send);
			  SEND_BUF(send);//�������鵽��λ�����ȴ��ش�����
				
			}
		}
	}			
//	NRF24L01_RX_Mode(); 				//�л��ɽ���ģʽ					
	while(NRF24L01_IRQ==0)		//����ܽ��յ���λ����ԤԼ�ź͵�·���Ĺ̶����ţ�ԤԼ�Ÿ�ʽ��0#1#03 ���Ÿ�ʽ��1#02
	{	
		u8 data[4]={0};
		NRF24L01_RxPacket(data);//��ȡ����
		printf("����=%s",data);//���������Ƿ���ȷ
		//NRF24L01_CE_L;             //��ʱ�ر����߲�Ȼ��Ӱ������������                               
		//NRF24L01_CSN_H;	
		//	delay_ms(500);
		operation(data);//�����ź͵�·�ƺ��� 			
	}		
	if(tim2==10)
	{
		for(i=0;i<130;i++)
		{
			MOTOR0_Reverse(2);//MOTOR0_Foreward(2);
		}	
		MOTOR0_Stop();
		//NRF24L01_CE_H;     //����λ��һֱ����ԤԼ�ţ������һֱ������պ�����Ӱ����բ������������                           
		//NRF24L01_CSN_L;	//���ԣ�����һ��ʼ���Ե�ʱ��Ľ���������������һ�ξ͹ر�ʹ�ܣ��Ȳ�����Ŵ�ʹ�ܣ��ȴ���һ�εĽ���
		//NRF24L01_1_RX_Mode();//�޸�Ƶ�Σ��ĳ���С��ͨ�ŵ�Ƶ��80���ȴ�����С���Ŀ�����Ϣ
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
void RC522_1_Handler(void)		//������1�������
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
		status = PcdAnticoll(card);	 //����ײ
		if (status== MI_OK)
		{  		 
			printf("�����У�=%02x %02x %02x %02x\r\n",card[0],card[1],card[2],card[3]);
//			send[5]=card[0]/100+'0';  //�����뿨�ŵ�һ�ֽ�
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

void operation(u8 *data)  //һ������յ�4�����ݣ�ԤԼ�� 2�㶨λ�� ͣ���� �����
{
	if(data[2]=='#')  //ԤԼ�� 01#3
	{	
		if(data[3]>'0'&& data[3]<='7')//'1'�����ǵڼ��㣬���ص���ȷ��ʽΪ 01#3 ��������ԤԼ1�㣬ԤԼ��3�ų�λ
		{
			GPIO_Write(GPIOD,0x0000);
			open_door();
		}
//		if (data[3]>'4')//''�����ǵ�2�㣬���ص���ȷ��ʽΪ 01#7 ��������ԤԼ2�㣬ԤԼ��7�ų�λ
//		{
			elevator_up();
			switch(data[3])// ע�⣺��λ�����صĸ�ʽֻ��4�����ݳ��ȣ������õ�4���ֽ���
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
	if(data[0]=='#')  //��λ�� #7 #8 #9   
	{	
		switch(data[1])// ע�⣺��λ�����صĸ�ʽֻ��4�����ݳ��ȣ������õ�4���ֽ���
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
	if(data[1]=='#')  //ͣ���� 7#1#0000#1@ 8#1#0000#1@  ������ 6#0#0000#1@
	{
		if(data[9]=='1')
		{	
			SEND_BUF(data);
			if(data[2]=='1')  //ͣ���� 
			{
				delay_ms(1000);
				delay_ms(1000);
					switch(data[0])// ע�⣺��λ�����صĸ�ʽֻ��4�����ݳ��ȣ������õ�4���ֽ���
				{
					 case  '5':{	GPIO_Write(GPIOD,0x0000);	 
								}break;
					 case  '6':{	GPIO_Write(GPIOD,0x0000);			
								}break;
					 case  '7':{	GPIO_Write(GPIOD,0x0000);			
								}break;		 	
				}
			}		
			if(data[2]=='0')  //������ 
			{
				if(data[0]>'0'&& data[0]<='4')
				{
					out_open();	
				}	
				else
				{	
					GPIO_Write(GPIOD,0x0300);  //���̵� �������·��
				}	
			}
		}	
		else //����� 0#1#0196#0@
		{	
			SEND_BUF(data);
			elevator_down();
		}	
	} 
	if(data[1]=='@')  //���Ե�� @
	{
		GPIO_Write(GPIOD,0xFFFF);
		elevator_down();
	}	
}
void location(u8 *data)
{		
//	NRF24L01_CE_L;	  
//	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);       //����RFͨ��Ƶ��	Ƶ��40	  
//  	NRF24L01_CE_H; 								
	SEND_BUF(data);									//���͵���λ������ͬ����λ��Ϣ
}
///////////////////////////////////////////////////////////////////////
// Delay 10ms
///////////////////////////////////////////////////////////////////////
void delay_ms(int nms)
{
	 u32 temp;
	 SysTick->LOAD = 9000*nms;
	 SysTick->VAL=0X00;//��ռ�����
	 SysTick->CTRL=0X01;//ʹ�ܣ����������޶����������ⲿʱ��Դ
	 do
	 {
	  temp=SysTick->CTRL;//��ȡ��ǰ������ֵ
	 }while((temp&0x01)&&(!(temp&(1<<16))));//�ȴ�ʱ�䵽��
	 SysTick->CTRL=0x00; //�رռ�����
	 SysTick->VAL =0X00; //��ռ�����
}
void delay_us(u32 nus)
{
	 u32 temp;
	 SysTick->LOAD = 9*nus;
	 SysTick->VAL=0X00;//��ռ�����
	 SysTick->CTRL=0X01;//ʹ�ܣ����������޶����������ⲿʱ��Դ
	 do
	 {
	  temp=SysTick->CTRL;//��ȡ��ǰ������ֵ
	 }while((temp&0x01)&&(!(temp&(1<<16))));//�ȴ�ʱ�䵽��
	 
	 SysTick->CTRL=0x00; //�رռ�����
	 SysTick->VAL =0X00; //��ռ�����
}


