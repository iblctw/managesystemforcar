#include "stm32f10x.h"
#include "misc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_spi.h"
#include "stdio.h"
#include "24l01.h"
#include "pbdata.h"
   
const u8 TX_ADDRESS[TX_ADR_WIDTH]={0x01,0xFF,0xFF,0xFF,0xFF}; //���͵�ַ
const u8 RX_ADDRESS[RX_ADR_WIDTH]={0x01,0xFF,0xFF,0xFF,0xFF}; //���͵�ַ ��λ����ַ
const u8 RX_ADDRESS1[RX_ADR_WIDTH]={0x02,0xFF,0xFF,0xFF,0xFF}; //���յ�ַ

void SPI_Configuration(void)
{													 
	SPI_InitTypeDef 	SPI_InitStructure;
	GPIO_InitTypeDef   	GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5|GPIO_Pin_7;//SCK   MOSI
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;   // MISO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode=SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL=SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA=SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI1,&SPI_InitStructure);

	SPI_Cmd(SPI1,ENABLE); //ʹ��SPI
//	SPI1_ReadWriteByte(0xff);//��������
                           
}
void SPI1_SetSpeed(u8 SpeedSet)
{
	//SPI_InitStructure.SPI_BaudRatePrescaler = SpeedSet ;
  	//SPI_Init(SPI1, &SPI_InitStructure);
//	SPI_Cmd(SPI1,ENABLE);
	SPI1->CR1&=0XFFC7; 
	SPI1->CR1|=SpeedSet;	//����SPI1�ٶ�  
	SPI1->CR1|=1<<6; 		//SPI�豸ʹ�� 
} 
//SPIx ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPIWriteByte(u8 TxData)
{                
        u8 retry=0;                                         
        while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
        {
                retry++;
                if(retry>200)
                        return 0;
        }                          
        SPI_I2S_SendData(SPI1, TxData); //ͨ������SPIx����һ������
        retry=0;

        while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
        {
                retry++;
                if(retry>200)
                        return 0;
        }                                                              
        return SPI_I2S_ReceiveData(SPI1); //����ͨ��SPIx������յ�����                                            
}

void NRF24L01_Init(void)
{  
	NRF24L01_CE_L;
	
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,(u8*)TX_ADDRESS,TX_ADR_WIDTH);//дTX�ڵ��ַ 
	NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH);//дRX�ڵ��ַ
	NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P1,(u8*)RX_ADDRESS1,RX_ADR_WIDTH);//дRX�ڵ��ַ
	//  	NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK��RX_ADDR_P0Ϊ0ͨ������ַ��ͬ����ʹ��		  
   	
  	
	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x3F);    	//ʹ��ͨ��0���Զ�Ӧ��������0x01������յ���ͨ��1ô        
  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x3F);	//ʹ��ͨ��0�Ľ��յ�ַ  	 
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);	    	//����RFͨ��Ƶ��40		  
  	NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x1a);//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
	NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ�� 
	NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P1,RX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ�� 
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);	//����TX�������,0db����,2Mbps,���������濪�� 	
	NRF24L01_CE_H;
	//ʹ��24L01
	//SPIƬѡȡ��
}

//���24L01�Ƿ����
//����ֵ:0���ɹ�;1��ʧ��	
u8 NRF24L01_Check(void)
{
	u8 buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	u8 i;
	SPI1_SetSpeed(SPI_BaudRatePrescaler_8); //spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   	 
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//д��5���ֽڵĵ�ַ.	
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //����д��ĵ�ַ  
	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
	if(i!=5)return 1;//���24L01����	
	return 0;		 //��⵽24L01
}	 	 
//SPIд�Ĵ���
//reg:ָ���Ĵ�����ַ
//value:д���ֵ
u8 NRF24L01_Write_Reg(u8 reg,u8 value)
{
	u8 status;	
   	NRF24L01_CSN_L;                 //ʹ��SPI����
  	status =SPIWriteByte(reg);//���ͼĴ����� 
  	SPIWriteByte(value);      //д��Ĵ�����ֵ
  	NRF24L01_CSN_H;                 //��ֹSPI����	   
  	return(status);       			//����״ֵ̬
}
//��ȡSPI�Ĵ���ֵ
//reg:Ҫ���ļĴ���
u8 NRF24L01_Read_Reg(u8 reg)
{
	u8 reg_val;	    
 	NRF24L01_CSN_L;          //ʹ��SPI����		
  	SPIWriteByte(reg);   //���ͼĴ�����
  	reg_val=SPIWriteByte(0XFF);//��ȡ�Ĵ�������
  	NRF24L01_CSN_H;          //��ֹSPI����		    
  	return(reg_val);           //����״ֵ̬
}	
//��ָ��λ�ö���ָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ 
u8 NRF24L01_Read_Buf(u8 reg,u8 *pBuf,u8 len)
{
	u8 status,u8_ctr;	       
  	NRF24L01_CSN_L;           //ʹ��SPI����
  	status=SPIWriteByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬   	   
 	for(u8_ctr=0;u8_ctr<len;u8_ctr++)pBuf[u8_ctr]=SPIWriteByte(0XFF);//��������
  	NRF24L01_CSN_H;       //�ر�SPI����
  	return status;        //���ض�����״ֵ̬
}
//��ָ��λ��дָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ
u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 len)
{
	u8 status,u8_ctr;	    
 	NRF24L01_CSN_L;          //ʹ��SPI����
  	status = SPIWriteByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
  	for(u8_ctr=0; u8_ctr<len; u8_ctr++)SPIWriteByte(*pBuf++); //д������	 
  	NRF24L01_CSN_H;       //�ر�SPI����
  	return status;          //���ض�����״ֵ̬
}				   
//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:�������״��
u8 NRF24L01_TxPacket(u8 *txbuf)
{
	u8 sta;
 	SPI1_SetSpeed(SPI_BaudRatePrescaler_8);//spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   
	NRF24L01_CE_L;
  	NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//д���ݵ�TX BUF  32���ֽ�
 	NRF24L01_CE_H;//��������	   
//	delay_ms(100);//�ȴ�������ɣ�������10US!!!
	while(NRF24L01_IRQ!=0);//�ȴ��������
	sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ	   
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
	
	if(sta&MAX_TX)//�ﵽ����ط�����
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);//���TX FIFO�Ĵ��� 
		return MAX_TX;	
	}
	if(sta&TX_OK)//�������
	{
		return TX_OK;
//		printf("״̬=%x\r\n",sta);	
	}
	return 0xff;//����ԭ����ʧ��

}
//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:0��������ɣ��������������
u8 NRF24L01_RxPacket(u8 *rxbuf)
{
	u8 sta;		    							   
	SPI1_SetSpeed(SPI_BaudRatePrescaler_8); //spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   
	sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ    	 
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
	if(sta&RX_OK)//���յ�����
	{
		NRF24L01_CE_L;
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//��ȡ����
		NRF24L01_Write_Reg(FLUSH_RX,0xff);//���RX FIFO�Ĵ��� 
		NRF24L01_CE_H;
		delay_us(150);
		return 0; 
	}	   
	return 1;//û�յ��κ�����
}					    
//�ú�����ʼ��NRF24L01��RXģʽ
//����RX��ַ,дRX���ݿ��,ѡ��RFƵ��,�����ʺ�LNA HCURR
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
void NRF24L01_RX_Mode(void)
{
	NRF24L01_CE_L;
  	NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0f);		//���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ 
  	NRF24L01_CE_H; //CEΪ��,�������ģʽ 
}

//�ú�����ʼ��NRF24L01��TXģʽ
//����TX��ַ,дTX���ݿ��,����RX�Զ�Ӧ��ĵ�ַ,���TX��������,ѡ��RFƵ��,�����ʺ�LNA HCURR
//PWR_UP,CRCʹ��
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
//CEΪ�ߴ���10us,����������.	 
void NRF24L01_TX_Mode(void)
{														 
	NRF24L01_CE_L;	
	//NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0c);	    
  	NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,0x0e);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
	NRF24L01_CE_H;//CEΪ��,10us����������
	delay_us(15);

}
void SEND_BUF(u8 *buf)
{
	NRF24L01_CE_L;
	NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,0x0e);
	NRF24L01_CE_H;
	delay_us(15);
	NRF24L01_TxPacket(buf);
	NRF24L01_CE_L;
	NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0f);
	NRF24L01_CE_H;	
}
//���ڷ���  ���յ��ַ���
void USART_send(u8 *ptr)
{
	u8 i;  
	for(i=0;i<11;i++)
   {	  
		USART_SendData(USART1,*(ptr+i)); // ��������
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
		//delay_ms(200);
   }                    
      
}
void USART_receive(u8 *ptr)
{
	u8 i;  
	for(i=0;i<6;i++)
   {	  
		*(ptr+i)=USART_ReceiveData(USART1); // ��������
		while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==RESET);
		//delay_ms(200);
   }                    
//   return  *ptr;  
}









