#include "proto.h"
#include "channels.h"
 extern struct Channel  channels[];//РѕР±РѕР±С‰РµРЅРЅР°СЏ СЃС‚СЂСѓРєС‚СѓСЂР° РєР°РЅР°Р»РѕРІ

//typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
//-----------------------------------------------------------------------------------
unsigned char   DEV_NAME[DEVICE_NAME_LENGTH_SYM] ="<<uUSO_2>>"; //РёРјСЏ СѓСЃС‚СЂРѕР№СЃС‚РІР°
unsigned char   NOTICE[DEVICE_DESC_MAX_LENGTH_SYM]="<-- GEOSPHERA_2011 -->";//РїСЂРёРјРµС‡Р°РЅРёРµ
unsigned char   VERSION[DEVICE_VER_LENGTH_SYM] ="\x30\x30\x30\x30\x31";	// РІРµСЂСЃРёСЏ РїСЂРѕРіСЂР°РјРјС‹ РџР—РЈ	РЅРµ Р±РѕР»СЊС€Рµ 5 Р±Р°Р№С‚

volatile  unsigned char   ADRESS_DEV=0x1;

unsigned char   dev_desc_len=20;//РґР»РёРЅР° РѕРїРёСЃР°РЅРёСЏ СѓСЃС‚СЂРѕР№СЃС‚РІР°
//--------------------------------global variable------------------------------------
//volatile unsigned char  	PROTO_STATE;//СЃС‡РµС‚С‡РёРє СЃРѕСЃС‚РѕСЏРЅРёР№
volatile unsigned char   	recieve_count;//СЃС‡РµС‚С‡РёРє РїСЂРёРµРјРЅРѕРіРѕ Р±СѓС„РµСЂР°
volatile unsigned char  	transf_count;//СЃС‡РµС‚С‡РёРє РїРµСЂРµРґР°РІР°РµРјС‹С… Р±Р°Р№С‚РѕРІ
volatile unsigned char  	buf_len;//РґР»РёРЅР° РїРµСЂРµРґР°РІР°РµРјРѕРіРѕ Р±СѓС„РµСЂР°

//------------------------С„Р»Р°РіРё РѕС€РёР±РѕРє--------------------------------
volatile  unsigned char   crc_n_ERR;	//РѕС€РёР±РєР° СЃrc
volatile  unsigned char   COMMAND_ERR;//РЅРµРїРѕРґРґРµСЂР¶РёРІР°РµРјР°СЏ РєРѕРјР°РЅРґР°

//volatile unsigned char   TIMEOUT;//С‚Р°Р№РјР°СѓС‚

volatile  unsigned char   CUT_OUT_NULL;//С„Р»Р°Рі-РІС‹СЂРµР·Р°РµРј 0 РїРѕСЃР»Рµ 0xD7
volatile  unsigned char   frame_len=0;//РґР»РёРЅР° РєР°РґСЂР°, РєРѕС‚РѕСЂСѓСЋ РІС‹С‚Р°СЃРєРёРІР°РµРј РёР· С€РµСЃС‚РѕРіРѕ Р±Р°Р№С‚Р° РєР°РґСЂР°
//--------------------------------------------------------------------
volatile  unsigned char    RecieveBuf[MAX_LENGTH_REC_BUF]={0} ; //Р±СѓС„РµСЂ РїСЂРёРЅРёРјР°РµРјС‹С… РґР°РЅРЅС‹С…
volatile  unsigned char    *TransferBuf;
//static unsigned char /*data*/ volatile  TransferBuf[MAX_LENGTH_TR_BUF] ; //Р±СѓС„РµСЂ РїРµСЂРµРґР°РІР°РµРјС‹С… РґР°РЅРЅС‹С…
//--------------------------------------------------------------------
volatile  unsigned char    STATE_BYTE=0xC0;//Р±Р°Р№С‚ СЃРѕСЃС‚РѕСЏРЅРёСЏ СѓСЃС‚СЂРѕР№СЃС‚РІР°


//uint8_t led=0;
volatile unsigned int fr_err=0;

volatile unsigned char  symbol=0xFF;//РїСЂРёРЅСЏС‚С‹Р№ СЃРёРјРІРѕР»
//-----------------------------------------------------------------------------------
union //РѕР±СЉРµРґРёРЅРµРЅРёРµ РґР»СЏ РєРѕРЅРІРµСЂС‚РёСЂРѕРІР°РЅРёСЏ char->long
{
	float result_float;
	unsigned char result_char[4];
}
sym_8_to_float;

xSemaphoreHandle xProtoSemaphore;


void USART1_IRQHandler (void)
{
 	static portBASE_TYPE xHigherPriorityTaskWoken;
 	  xHigherPriorityTaskWoken = pdFALSE;

//

 	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
   	{

 		USART_ClearITPendingBit(USART1, USART_IT_RXNE);


   		symbol=USART_ReceiveData (USART1);
   //----------------------РѕР±СЂР°Р±Р°С‚С‹РІР°РµРј РІРѕР·РјРѕР¶РЅС‹Рµ РѕС€РёР±РєРё РґР»РёРЅС‹ РєР°РґСЂР°-------------
   		if(recieve_count>MAX_LENGTH_REC_BUF)	//РµСЃР»Рё РїРѕСЃС‹Р»РєР° СЃР»РёС€РєРѕРј РґР»РёРЅРЅР°СЏ
   		{
   			recieve_count=0;

   			return;
   		}


   //--------------------------РЅР°С‡Р°Р»Рѕ РєР°РґСЂР°...РїСЂРѕРІРµСЂРєР° РґРѕ РґР»РёРЅС‹ РєР°РґСЂР°--------
   	    if(recieve_count<6)
   		{
   				switch(recieve_count)
   				{
   					case  0:   //РїРµСЂРІС‹Р№ СЃРёРјРІРѕР» 0
   					{
   	 				 	 if(symbol!=0x00)
   						 {
   	 				 		recieve_count=0;

   	 				 	fr_err++;
   							//return;
   						 }
   					}
   					break;

   					case 1:	 //РІС‚РѕСЂРѕР№ СЃРёРјРІРѕР» 0xD7
   					{
   						 if(symbol!=0xD7)
   						 {
   							recieve_count=0;

   							//return;
   						 }
   					}
   					break;

   					case 2:	 //	С‚СЂРµС‚РёР№ СЃРёРјРІРѕР» 0x29
   					{
   					 	 if(symbol!=0x29)
   						 {
   					 		recieve_count=0;

   							//return;
   						 }
   					}
   					break;

   					case 3:	//РµСЃР»Рё Р°РґСЂРµСЃ РЅРµ СЃРѕРІРїР°Р», С‚Рѕ СЃР±СЂРѕСЃРёРј//NEW
   					{
   						if(symbol!=ADRESS_DEV)//РµСЃР»Рё Р°РґСЂРµСЃ СЃРѕРІРїР°Р»
   						{
   							recieve_count=0;

   							//return;
   						}
   					}
   					break;

   					default:  //
   					{
   					}
   					break;
   				}

   			RecieveBuf[recieve_count]=symbol;//СЃРѕС…СЂР°РЅСЏРµРј РїСЂРёРЅСЏС‚С‹Р№ СЃРёРјРІРѕР» РІ Р±СѓС„РµСЂ
   			recieve_count++;//СЃС‡РµС‚С‡РёРє Р±СѓС„РµСЂР°

   			if(recieve_count==6)
   			{
   				frame_len=RecieveBuf[recieve_count-1]; //РїРѕР»СѓС‡РёРј РѕСЃС‚Р°РІС€СѓСЋСЃСЏ РґР»РёРЅСѓ
   			}
   		}
   //---------------------------------------------------------
   		else  //РѕС‚СЃСЋРґР° Р·РЅР°РµРј РґР»РёРЅСѓ РєР°РґСЂР° Рё СѓРґР°Р»СЏРµРј РЅСѓР»Рё РїРѕСЃР»Рµ 0xD7
   		{
   			switch(symbol)//РїСЂРѕРІРµСЂРёРј, СЌС‚Рѕ 0x0 ,0xD7 РёР»Рё РґСЂСѓРіРѕРµ
   			{
   				case 0xD7:
   				{
   					CUT_OUT_NULL=1;	//
   					RecieveBuf[recieve_count]=symbol;
   					recieve_count++;

   				}
   				break;

   				case 0x0:
   				{
   					if(!CUT_OUT_NULL)  //РµСЃР»Рё РІС‹СЂРµР·Р°С‚СЊ 0x0 РЅРµ РЅР°РґРѕ, С‚Рѕ РЅРµ РІС‹СЂРµР·Р°РµРј
   					{
   						RecieveBuf[recieve_count]=symbol;
   						recieve_count++;
   					}
   					else //РёРЅР°С‡Рµ РІ Р±СѓС„РµСЂ РЅРµ РєР»Р°РґРµРј, СЃР±СЂРѕСЃРёРј С„Р»Р°Рі
   					{
   						CUT_OUT_NULL=0;
   					}
   				}
   				break;

   				default:  //РґСЂСѓРіРёРµ РєР»Р°РґРµРј РІ Р±СѓС„РµСЂ
   				{
   					CUT_OUT_NULL=0;
   					RecieveBuf[recieve_count]=symbol;
   					recieve_count++;
   				}
   				break;
   			}

   			if(recieve_count>=frame_len+6)//РµСЃР»Рё РїСЂРёРЅСЏС‚С‹  РІСЃРµ Р±Р°Р№С‚С‹
   			{
   				 USART_ITConfig(USART1, USART_IT_RXNE , DISABLE);
   				  /* 'Р”Р°С‚СЊ' СЃРµРјР°С„РѕСЂ РґР»СЏ СЂР°Р·Р±Р»РѕРєРёСЂРѕРІРєРё Р·Р°РґР°С‡Рё. */

   				 xSemaphoreGiveFromISR( xProtoSemaphore, &xHigherPriorityTaskWoken );

   				 if( xHigherPriorityTaskWoken != pdFALSE )
   				 {
   					portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
   				 }
   			}
   		}

   	}
   //----------------------------РїРµСЂРµРґР°С‡Р°----------------------------------------------------------------
   	if(USART_GetITStatus(USART1, USART_IT_TC) != RESET)
   	{

   		USART_ClearITPendingBit(USART1, USART_IT_TC);//РѕС‡РёС‰Р°РµРј РїСЂРёР·РЅР°Рє РїСЂРµСЂС‹РІР°РЅРёСЏ

   		if(transf_count<buf_len)
   		{
   			if(transf_count<3)//РїРµСЂРµРґР°РµРј Р·Р°РіРѕР»РѕРІРѕРє
   			{
   				USART_SendData(USART1,TransferBuf[transf_count]);
   				transf_count++;
   			}
   			else   //С‚РµР»Рѕ...   РїРѕРґСЃС‚Р°РІР»СЏРµРј 0 РїРѕСЃР»Рµ 0xD7
   			{
   					if(CUT_OUT_NULL==0)
   					{
   						if(TransferBuf[transf_count]==(unsigned char)0xD7)//РїСЂРѕРІРµСЂРёРј, СЌС‚Рѕ  ,0xD7 РёР»Рё РґСЂСѓРіРѕРµ
   						{
   							CUT_OUT_NULL=0x1;
   						}
   						USART_SendData(USART1,TransferBuf[transf_count]);
   						transf_count++;
   					}
   					else
   					{
   						USART_SendData(USART1,(unsigned char)0x0);
   						CUT_OUT_NULL=0;
   					}
   			}
   		}
   		else
   		{
   			transf_count=0;		//РѕР±РЅСѓР»СЏРµРј СЃС‡РµС‚С‡РёРє
   			recieve_count=0;
   			//PROTO_STATE=PROTO_RESTART;
   			CUT_OUT_NULL=0;
   			 USART_ITConfig(USART1, USART_IT_RXNE , ENABLE);
   		}

   	}
   	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void Proto_Init(void) //using 0
{
	//---------РёРЅРёС†РёР°Р»РёР·Р°С†РёСЏ РѕР±РѕСЂСѓРґРѕРІРЅРёСЏ----------------------------------
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA , ENABLE);//С‚Р°РєС‚РёСЂСѓРµРј СѓР°СЂС‚

	//РЅР°СЃС‚СЂР°РёРІР°РµРј РєРѕРЅС‚СЂРѕР»Р»РµСЂ РїСЂРµСЂС‹РІР°РЅРёР№
	 NVIC_InitTypeDef NVIC_InitStructure;
	 NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 14;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 15;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStructure);


	  //РЅР°СЃС‚СЂР°РёРІР°Рµ РїРёРЅС‹
	  GPIO_InitTypeDef GPIO_InitStructure;

	  // Tx on PD5 as alternate function push-pull
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);

	  /* Rx on PD6 as input floating */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);


	USART_InitTypeDef USART_InitStructure;

		USART_DeInit(USART1);
		//РЅР°СЃС‚СЂР°РёРІР°РµРј СѓСЂР°С‚
	USART_InitStructure.USART_BaudRate = 57600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_ClearFlag(USART1, USART_FLAG_CTS | USART_FLAG_LBD  | USART_FLAG_TC  | USART_FLAG_RXNE );

	USART_ITConfig(USART1, USART_IT_TC, ENABLE);
	USART_ITConfig(USART1, USART_IT_RXNE , ENABLE);

	USART_Cmd(USART1, ENABLE);





	//------------------------С„Р»Р°РіРё РѕС€РёР±РѕРє--------------------------------

	crc_n_ERR=0x0;	//РѕС€РёР±РєР° crc_n
	COMMAND_ERR=0x0;//РЅРµРїРѕРґРґРµСЂР¶РёРІР°РµРјР°СЏ РєРѕРјР°РЅРґР°

	TransferBuf=&RecieveBuf[0];	 //Р±СѓС„РµСЂ РѕС‚РІРµС‚Р° =Р±СѓС„РµСЂ Р·Р°РїСЂРѕСЃР°
	ChannelsInit();


	//Restore_Dev_Address_Desc();

//	PROTO_STATE=PROTO_RESTART;//СЃС‡РµС‚С‡РёРє СЃРѕСЃС‚РѕСЏРЅРёР№
	recieve_count=0x0;//СЃС‡РµС‚С‡РёРє Р±СѓС„РµСЂР° РїСЂРёРµРјР°
	transf_count=0x0;//СЃС‡РµС‚С‡РёРє РїРµСЂРµРґР°РІР°РµРјС‹С… Р±Р°Р№С‚РѕРІ
	buf_len=0x0;//РґР»РёРЅР° РїРµСЂРµРґР°РІР°РµРјРѕРіРѕ Р±СѓС„РµСЂР°
	//DE_RE=0;//Р»РёРЅРёСЏ РЅР° РїСЂРёРµРј
	CUT_OUT_NULL=0;

	 xTaskCreate(ProtoProcess,(signed char*)"PROTO",64,
	            NULL, tskIDLE_PRIORITY + 1, NULL);

	vSemaphoreCreateBinary( xProtoSemaphore );
	return;
}
//-----------------------------------------------------------------------------
unsigned char Send_Info(void) //using 0    //РїРѕСЃС‹Р»РєР° РёРЅС„РѕСЂРјР°С†РёРё РѕР± СѓСЃС‚СЂРѕР№СЃС‚РІРµ
{
	    unsigned char    i=0;

	   //Р·Р°РіРѕР»РѕРІРѕРє РєР°РґСЂР°---
	   TransferBuf[0]=0x00;
	   TransferBuf[1]=0xD7;
	   TransferBuf[2]=0x29;
	   //------------------
	   TransferBuf[3]=ADRESS_DEV;  // Р°РґСЂРµСЃ СѓР·Р»Р°
	   TransferBuf[4]=GET_DEV_INFO_RESP;  // РєРѕРґ РѕРїРµСЂР°С†РёРё
	   TransferBuf[6]=STATE_BYTE;

	   for(i=0;i<20;i++)
	   {				  // Р·Р°РїРёСЃС‹РІР°РµРј РЅР°РёРјРµРЅРѕРІР°РЅРёРµ РёР·РґРµР»РёСЏ
			   if(i<DEVICE_NAME_LENGTH_SYM)
			   {
			     	TransferBuf[i+7]=DEV_NAME[i];
			   }
			   else
			   {
			   		TransferBuf[i+7]=0x00;
			   }
		}

	   for(i=0;i<5;i++)                   // Р·Р°РїРёСЃС‹РІР°РµРј РІРµСЂСЃРёСЋ РџР—РЈ
	   {
	       if(i<DEVICE_VER_LENGTH_SYM)
		   {
		    	 TransferBuf[i+27]=VERSION[i];
		   }
	   }

	   TransferBuf[32]=CHANNEL_NUMBER;		   // РєРѕР»РёС‡РµСЃС‚РІРѕ РєР°РЅР°Р»РѕРІ

	   for(i=0;i<CHANNEL_NUMBER;i++)				   // РґР°РЅРЅС‹Рµ РїРѕ РєР°РЅР°Р»Сѓ
       {
		  	TransferBuf[i*2+33]=((channels[i].settings.set.type)<<4)|channels[i].settings.set.modific; // Р±Р°Р№С‚ РґР°РЅРЅС‹С…
		  	TransferBuf[i*2+33+1]=0x00;							// СЂРµР·РµСЂРІ Р±Р°Р№С‚
	   }
	   for(i=0;i<dev_desc_len;i++)					// Р·Р°РїРёСЃС‹РІР°РµРј РїСЂРёРјРµС‡Р°РЅРёРµ
	   {
			 TransferBuf[i+33+CHANNEL_NUMBER*2]=NOTICE[i];
	   }

	   TransferBuf[5]=28+CHANNEL_NUMBER*2+dev_desc_len;			// РїРѕРґСЃС‡РµС‚ РґР»РёРЅС‹ РґР°РЅРЅС‹С…
	   TransferBuf[33+CHANNEL_NUMBER*2+dev_desc_len]=CRC_Check(&TransferBuf[1],32+CHANNEL_NUMBER*2+dev_desc_len); // РїРѕРґСЃС‡РµС‚ РєРѕРЅС‚СЂРѕР»СЊРЅРѕР№ СЃСѓРјРјС‹

	return (34+CHANNEL_NUMBER*2+dev_desc_len);
}
//-----------------------------------------------------------------------------
unsigned char Node_Full_Init(void) //using 0 //РїРѕР»РЅР°СЏ РёРЅРёС†РёР°Р»РёР·Р°С†РёСЏ СѓР·Р»Р°
{
	return 0;
}
//-----------------------------------------------------------------------------
unsigned char Channel_List_Init(void) //using 0 //Р�РЅРёС†РёР°Р»РёР·Р°С†РёСЏ СЃРїРёСЃРєР° РєР°РЅР°Р»РѕРІ СѓР·Р»Р° (Р±РµР· РїРѕС‚РµСЂРё РґР°РЅРЅС‹С…);
{
	return 0;
}
//-----------------------------------------------------------------------------
unsigned char Channel_Get_Data(void) //using 0 //Р’С‹РґР°С‚СЊ РґР°РЅРЅС‹Рµ РїРѕ РєР°РЅР°Р»Р°Рј, СЃРѕРіР»Р°СЃРЅРѕ Р°Р±СЃРѕР»СЋС‚РЅРѕР№ РЅСѓРјРµСЂР°С†РёРё;
{
	return 0;
}
//-----------------------------------------------------------------------------
unsigned char  Channel_Set_Parameters(void) //using 0 //РЈСЃС‚Р°РЅРѕРІРёС‚СЊ РїР°СЂР°РјРµС‚СЂС‹ РїРѕ РєР°РЅР°Р»Р°Рј, СЃРѕРіР»Р°СЃРЅРѕ Р°Р±СЃРѕР»СЋС‚РЅРѕР№ РЅСѓРјРµСЂР°С†РёРё;
{
         unsigned char   index=0, store_data=0;//i=0;
//	LED=1;
	   while(index<RecieveBuf[5]-1)				   // РґР°РЅРЅС‹Рµ РїРѕ РєР°РЅР°Р»Р°Рј
	      {
			  	if(RecieveBuf[6+index]<CHANNEL_NUMBER)
			    {
					switch((RecieveBuf[6+index+1]>>4)&0xF)
					{
					 		case 0x0://РђР¦Рџ
							{
								if((channels[RecieveBuf[6+index]].settings.set.modific!=RecieveBuf[6+index+1])||(channels[RecieveBuf[6+index]].settings.set.state_byte_1!=RecieveBuf[6+index+2]) || (channels[RecieveBuf[6+index]].settings.set.state_byte_2!=RecieveBuf[6+index+3]))
								{
									channels[RecieveBuf[6+index]].settings.set.state_byte_1=RecieveBuf[6+index+2];
									channels[RecieveBuf[6+index]].settings.set.state_byte_2=RecieveBuf[6+index+3];
									channels[RecieveBuf[6+index]].settings.set.modific	   =RecieveBuf[6+index+1]&0xF;
									//debug----------------
									//if(channels[RecieveBuf[6+index]].settings.set.modific==3)
									{
										GPIOC->ODR |= GPIO_Pin_8;
									}
									//-----------------
									store_data=1;

								}
								index=index+1;
							}
							break;

							case 0x2://С‡Р°СЃС‚РѕС‚РѕРјРµСЂ
							{
							/*   if(channels[RecieveBuf[6+index]].settings.set.state_byte_1!=RecieveBuf[6+index+2])
							   {
							   		channels[RecieveBuf[6+index]].settings.set.state_byte_1=RecieveBuf[6+index+2];
									store_data=1;
							   }*/
							}
							break;
					}
					index=index+3;
				}
				else
				{
					return Request_Error(FR_UNATTENDED_CHANNEL);
				}
		  }
	   if(store_data)
	   {
	   		//Store_Channels_Data();	//СЃРѕС…СЂР°РЅРёРј РЅР°СЃС‚СЂРѕР№РєРё РєР°РЅР°Р»РѕРІ РІ РџРџР—РЈ
		}
//	   LED=0;
	   return Request_Error(FR_SUCCESFUL);
}
//-----------------------------------------------------------------------------
unsigned char Channel_Set_Order_Query(void) //using 0 //Р—Р°РґР°С‚СЊ РїРѕСЃР»РµРґРѕРІР°С‚РµР»СЊРЅРѕСЃС‚СЊ РѕРїСЂРѕСЃР°;
{
	return 0;
}
//-----------------------------------------------------------------------------
unsigned char Channel_Get_Data_Order(void) //using 0 //Р’С‹РґР°С‚СЊ РґР°РЅРЅС‹Рµ РїРѕ РєР°РЅР°Р»Р°Рј, СЃРѕРіР»Р°СЃРЅРѕ РїРѕСЃР»РµРґРѕРІР°С‚РµР»СЊРЅРѕСЃС‚Рё РѕРїСЂРѕСЃР°;
{
	return 0;
}
//-----------------------------------------------------------------------------
unsigned char Channel_Set_State(void) //using 0 //РЈСЃС‚Р°РЅРѕРІРёС‚СЊ СЃРѕСЃС‚РѕСЏРЅРёСЏ РїРѕ РєР°РЅР°Р»Р°Рј, СЃРѕРіР»Р°СЃРЅРѕ Р°Р±СЃРѕР»СЋС‚РЅРѕР№ РЅСѓРјРµСЂР°С†РёРё;
{
	return 0;
}
//-----------------------------------------------------------------------------
unsigned char  Channel_Get_Data_Order_M2(void) //using 0 //Р’С‹РґР°С‚СЊ РґР°РЅРЅС‹Рµ РїРѕ РєР°РЅР°Р»Р°Рј, СЃРѕРіР»Р°СЃРЅРѕ РїРѕСЃР»РµРґРѕРІР°С‚РµР»СЊРЅРѕСЃС‚Рё РѕРїСЂРѕСЃР°;
{
	return 0;
}
//-----------------------------------------------------------------------------
unsigned char Channel_Set_Reset_State_Flags(void) //using 0 //	РЈСЃС‚Р°РЅРѕРІРєР°/РЎР±СЂРѕСЃ С„Р»Р°РіРѕРІ СЃРѕСЃС‚РѕСЏРЅРёСЏ
{
	STATE_BYTE=0x40;
	return	Request_Error(FR_SUCCESFUL);//РѕС€РёР±РєРё РЅРµС‚, РїРѕРґС‚РІРµСЂР¶РґРµРЅРёРµ
}
//-----------------------------------------------------------------------------
unsigned char Channel_All_Get_Data(void) //using 0 //Р’С‹РґР°С‚СЊ РёРЅС„РѕСЂРјР°С†РёСЋ РїРѕ РІСЃРµРј РєР°РЅР°Р»Р°Рј СѓР·Р»Р° (СЂР°СЃС€РёСЂРµРЅРЅС‹Р№ СЂРµР¶РёРј);
{
	   unsigned char  index=0,i=0;


	   TransferBuf[0]=0x00;TransferBuf[1]=0xD7;TransferBuf[2]=0x29;
	   TransferBuf[3]=ADRESS_DEV;  //
	   TransferBuf[4]=CHANNEL_ALL_GET_DATA_RESP;  //
	   TransferBuf[6]=STATE_BYTE;
	    for(i=0;i<CHANNEL_NUMBER;i++)				   //
	    {
			  TransferBuf[index+7]=i;
			  index++;
			  TransferBuf[index+7]=((channels[i].settings.set.type)<<4)|channels[i].settings.set.modific; //
			  index++;
			  switch(channels[i].settings.set.type)
			    {
					 case 0:  //
					 {
						 switch(channels[i].settings.set.modific)
		                 {
							  case 0:
							  {
							  		if(channels[i].calibrate.cal.calibrate)//
									{
									//	 if(channels[i].settings.set.modific==0x00 || channels[i].settings.set.modific==0x01)
									//	 {
										 	TransferBuf[index+7]=((channels[i].channel_data_calibrate)&0x0000FF00)>>8;
										  	index++;
				    					  	TransferBuf[index+7]=((channels[i].channel_data_calibrate)&0x00FF0000)>>16;
										  	index++;
									//	 }
									}
									else
									{
										// if(channels[i].settings.set.modific==0x00 || channels[i].settings.set.modific==0x01)		   //
									//	 {
											TransferBuf[index+7]=((channels[i].channel_data)&0x0000FF00)>>8;
										  	index++;
				    					  	TransferBuf[index+7]=((channels[i].channel_data)&0x00FF0000)>>16;
										  	index++;
										// }
									}


									  TransferBuf[index+7]=channels[i].settings.set.state_byte_1;	 //
			                          index++;
			                          TransferBuf[index+7]=channels[i].settings.set.state_byte_2;	 //
				                      index++;
							  }
							  break;

							  case 1:
							  {
							  }
							  break;

				        	  case 2:
							  {
							  }
							  break;

							  case 3:
							  {
							        if(channels[i].calibrate.cal.calibrate)//
									{
							 			// if(channels[i].settings.set.modific==0x02 || channels[i].settings.set.modific==0x03)		   //
									//	 {
										  	TransferBuf[index+7]=((channels[i].channel_data_calibrate)&0x000000FF); //
								          	index++;
										  	TransferBuf[index+7]=((channels[i].channel_data_calibrate)&0x0000FF00)>>8;
										  	index++;
				    					  	TransferBuf[index+7]=((channels[i].channel_data_calibrate)&0x00FF0000)>>16;
										  	index++;
			  						  	// }
									}
									else
									{
									//	 if(channels[i].settings.set.modific==0x02 || channels[i].settings.set.modific==0x03)		   //
										// {
										  	TransferBuf[index+7]=((channels[i].channel_data)&0x000000FF); //
								          	index++;
										  	TransferBuf[index+7]=((channels[i].channel_data)&0x0000FF00)>>8;
										  	index++;
				    					  	TransferBuf[index+7]=((channels[i].channel_data)&0x00FF0000)>>16;
										  	index++;
									//	 }
									}


									  TransferBuf[index+7]=channels[i].settings.set.state_byte_1;	 //
			                          index++;
			                          TransferBuf[index+7]=channels[i].settings.set.state_byte_2;	 //
				                      index++;
							  }
							  break;
						  }
					  }
					  break;

				 	case 1:	 //Г„ГЋГ‹
					{
						  switch(channels[i].settings.set.modific)
					      {
								  case 0:
								  {
								          TransferBuf[index+7]=((channels[i].channel_data)&0x000000FF); //
								          index++;

										  TransferBuf[index+7]=((channels[i].channel_data)&0x0000FF00)>>8;
										  index++;

								          TransferBuf[index+7]=((channels[i].channel_data)&0x00FF0000)>>16; //
								          index++;

										  TransferBuf[index+7]=((channels[i].channel_data)&0xFF000000)>>24;
										  index++;


										  TransferBuf[index+7]=channels[i].settings.set.state_byte_1;	 //
				                          index++;
								  }
								  break;
						   }
					}
					break;

					 case 2: //
					 {
						  switch(channels[i].settings.set.modific)
					      {
								  case 1:
								  {
								          TransferBuf[index+7]=((channels[i].channel_data)&0x000000FF); //
								          index++;
										  TransferBuf[index+7]=((channels[i].channel_data)&0x0000FF00)>>8;
										  index++;
										  TransferBuf[index+7]=channels[i].settings.set.state_byte_1;	 //
				                          index++;
								  }
								  break;
						   }
					  }
					  break;
			  }
		   }

		  TransferBuf[5]=index+2; 						 //
		  TransferBuf[index+7]=CRC_Check(&TransferBuf[1],(unsigned int)(index+7)-1); //
		  return (unsigned char)(7+index+1);
}

unsigned char Request_Error(unsigned char error_code) //using 0 //	РћС€РёР±РѕС‡РЅС‹Р№ Р·Р°РїСЂРѕСЃ/РѕС‚РІРµС‚;
{
	TransferBuf[0]=0x00;TransferBuf[1]=0xD7;TransferBuf[2]=0x29;
    TransferBuf[3]=ADRESS_DEV;  // Р°РґСЂРµСЃ СѓР·Р»Р°
    TransferBuf[7]=RecieveBuf[4]; // РєРѕРґ СЃР±РѕР№РЅРѕР№ РєРѕРјР°РЅРґС‹
    TransferBuf[4]=0xFF;  // РєРѕРґ РѕРїРµСЂР°С†РёРё

	TransferBuf[6]=STATE_BYTE; // Р±Р°Р№С‚ СЃС‚Р°С‚СѓСЃР° СѓР·Р»Р°
    TransferBuf[8]=error_code;
    TransferBuf[5]=0x04;	  // РґР»РёРЅР° РґР°РЅРЅС‹С…
    TransferBuf[9]=CRC_Check(TransferBuf,9);
	return 10;
}
//-----------------------------------------------------------------------------
void ProtoBufHandling(void) //using 0 //РїСЂРѕС†РµСЃСЃ РѕР±СЂР°Р±РѕС‚РєРё РїСЂРёРЅСЏС‚РѕРіРѕ Р·Р°РїСЂРѕСЃР°
{
  switch(RecieveBuf[4])
  {
//---------------------------------------
  	case GET_DEV_INFO_REQ:
	{
		buf_len=Send_Info();
	}
	break;
//---------------------------------------
  	case NODE_FULL_INIT_REQ:
	{
		buf_len=Node_Full_Init();
	}
	break;
//---------------------------------------
  	case CHANNEL_LIST_INIT_REQ:
	{
		buf_len=Channel_List_Init();
	}
	break;
//---------------------------------------
	case CHANNEL_GET_DATA_REQ:
	{
		buf_len=Channel_Get_Data();
	}
	break;
	//-----------------------------------
	case CHANNEL_SET_PARAMETERS_REQ:
	{
		buf_len=Channel_Set_Parameters();
	}
	break;
	//-----------------------------------
	case CHANNEL_SET_ORDER_QUERY_REQ:
	{
		buf_len=Channel_Set_Order_Query();
	}
	break;
//----------------------------------------
	case CHANNEL_GET_DATA_ORDER_REQ:
	{
		 buf_len=Channel_Get_Data_Order();
	}
	break;
//----------------------------------------
	case CHANNEL_SET_STATE_REQ:
	{
		 buf_len=Channel_Set_State();
	}
	break;
//----------------------------------------
	case CHANNEL_GET_DATA_ORDER_M2_REQ:
	{
		 buf_len=Channel_Get_Data_Order_M2();
	}
	break;
//------------------------------------------
	case CHANNEL_SET_RESET_STATE_FLAGS_REQ:
	{
		buf_len=Channel_Set_Reset_State_Flags();
	}
	break;
//------------------------------------------
	case CHANNEL_ALL_GET_DATA_REQ:
	{
		 buf_len=Channel_All_Get_Data();
	}
	break;
//------------------------------------------
/*	case CHANNEL_SET_ADDRESS_DESC:
	{
		 buf_len=Channel_Set_Address_Desc();
	}
	break;
//------------------------------------------
	case CHANNEL_SET_CALIBRATE:
	{
		 buf_len=Channel_Set_Calibrate();
	}
	break;*/
//------------------------------------------
    default:
	{
       COMMAND_ERR=0x1;//РЅРµСЃСѓС‰РµСЃС‚РІСѓСЋС‰Р°СЏ РєРѕРјР°РЅРґР°
	   buf_len=Request_Error(FR_COMMAND_NOT_EXIST);
	  // PROTO_STATE=PROTO_ERR_HANDLING;//РЅР° РѕР±СЂР°Р±РѕС‚С‡РёРє РѕС€РёР±РєРё
    }
  }

  return;
}
//-----------------------------------------------------------------------------------
void ProtoProcess( void *pvParameters ) //РіР»Р°РІРЅС‹Р№ РїСЂРѕС†РµСЃСЃ
{
			unsigned char   crc_n;
	while(1)
	{
		if( xProtoSemaphore != NULL )
		{

			if( xSemaphoreTake( xProtoSemaphore, ( portTickType ) 10 ) == pdTRUE )
			{

				crc_n=RecieveBuf[recieve_count-1];
				transf_count=0;
				if(CRC_Check(&RecieveBuf,(recieve_count-CRC_LEN))==crc_n)
				{

					ProtoBufHandling();//РїСЂРѕС†РµРґСѓСЂР° РѕР±СЂР°Р±РѕС‚РєРё СЃРѕРѕР±С‰РµРЅРёСЏ

					transf_count=0;
					recieve_count=0;
					CUT_OUT_NULL=0;

					USART_SendData(USART1,TransferBuf[transf_count]);

					transf_count++;//РёРЅРєСЂРµРјРµРЅС‚РёСЂСѓРµРј СЃС‡РµС‚С‡РёРє РїРµСЂРµРґР°РЅРЅС‹С…
				}
				else
				{
					crc_n_ERR=0x1;//РЅРµСЃРѕРІРїР°РґРµРЅРёРµ crc_n

				}

			}
		}
		//--------------------------------------------------
	}
}
//-----------------------crc_n------------------------------------------------------------
 unsigned char  CRC_Check( unsigned char  *Spool_pr,unsigned char Count_pr )  //РїСЂРѕРІРµСЂРёС‚СЊ
{
	unsigned char crc_n = 0;
	unsigned char  *Spool;
	unsigned char  Count ;

	Spool=Spool_pr;
	Count=Count_pr;

  		while(Count!=0x0)
        {
	        crc_n = crc_n ^ (*Spool++);//
	        // С†РёРєР»РёС‡РµСЃРєРёР№ СЃРґРІРёРі РІРїСЂР°РІРѕ
	        crc_n = ((crc_n & 0x01) ? (unsigned char)0x80: (unsigned char)0x00) | (unsigned char)(crc_n >> 1);
	        // РёРЅРІРµСЂСЃРёСЏ Р±РёС‚РѕРІ СЃ 2 РїРѕ 5, РµСЃР»Рё Р±РёС‚ 7 СЂР°РІРµРЅ 1
	        if (crc_n & (unsigned char)0x80) crc_n = crc_n ^ (unsigned char)0x3C;
			Count--;
        }
    return crc_n;
}
 //-----------------------------------------------------------------------------------------------
/*
 void Store_Dev_Address_Desc(void)
 {

	 uint16_t i=0;
	 unsigned int blocks=0;
	 uint32_t EraseCounter = 0x00, Address = 0x00;
	 uint32_t NbrOfPage = 0x00;
	 volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
	 volatile TestStatus MemoryProgramStatus = PASSED;

	 FLASH_UnlockBank1();

	 Define the number of page to be erased
	NbrOfPage = (DESC_BANK1_WRITE_END_ADDR - DESC_BANK1_WRITE_START_ADDR) / FLASH_PAGE_SIZE;

	 Clear All pending flags
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

	 Erase the FLASH pages
	for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
	{
		FLASHStatus = FLASH_ErasePage(DESC_BANK1_WRITE_START_ADDR + (FLASH_PAGE_SIZE * EraseCounter));
	}

	 Program Flash Bank1
	Address = DESC_BANK1_WRITE_START_ADDR;

	FLASH_ProgramWord(Address, (uint32_t)ADRESS_DEV);//Р·Р°РїРёС€РµРј Р°РґСЂРµСЃ
	Address = Address + 4;

	 for(i=0;i<(DEVICE_NAME_LENGTH>>2);i+=4)
	 {
	        FLASHStatus = FLASH_ProgramWord(Address,((uint32_t*)(&DEV_NAME))[i]);
	        Address = Address + 4;
	 }

	 for(i=0;i<(DEVICE_VER_LENGTH>>2);i+=4)
	 {
	        FLASHStatus = FLASH_ProgramWord(Address,((uint32_t*)(&VERSION))[i]);
	        Address = Address + 4;
	 }

	FLASH_ProgramWord(Address, (uint32_t)dev_desc_len);//Р·Р°РїРёС€РµРј РґР»РёРЅСѓ РѕРїРёСЃР°РЅРёСЏ
	Address = Address + 4;

 	for(i=0;i<(dev_desc_len>>2+1);i++)
 	{
 		 FLASHStatus = FLASH_ProgramWord(Address,((uint32_t*)(&NOTICE))[i]);
 		Address+=4;
 	}
 	FLASH_LockBank1();
 //LED=0;
 	return;
 }
 //-----------------------------------------------------------------------------------------------
 void Restore_Dev_Address_Desc(void)//???
 {
 	unsigned int blocks=0,i=0;
 	uint32_t Address;
 	Address = DESC_BANK1_WRITE_START_ADDR;

 	ADRESS_DEV=(*(__IO uint32_t*) Address);
 	Address++;//РїСЂРѕС‡РёС‚Р°Р»Рё Р°РґСЂРµСЃ СѓСЃС‚СЂРѕР№СЃС‚РІР°, РїРµСЂРµС…РѕРґРёРј РЅР° СЃР»РµРґСѓСЋС‰РёР№

 	if(ADRESS_DEV<1 || ADRESS_DEV>15)
 		ADRESS_DEV=1;


 	for(i=0;i<(DEVICE_NAME_LENGTH>>2);i++)
 	{
 		((uint32_t*)(&DEV_NAME))[i]=(*(__IO uint32_t*) Address);
 		Address+=4;
 	}



 	for(i=0;i<(DEVICE_VER_LENGTH>>4);i++)
 	{
 		((uint32_t*)(&VERSION))[i]=(*(__IO uint32_t*) Address);
 		Address+=4;
 	}


 	dev_desc_len==(*(__IO uint32_t*) Address);
 	Address++;

 	if(dev_desc_len>DEVICE_DESC_MAX_LENGTH_SYM)
 		dev_desc_len=DEVICE_DESC_MAX_LENGTH_SYM;



 	for(i=0;i<(dev_desc_len>>2+1);i++)
 	{
 		((uint32_t*)(&NOTICE))[i]=(*(__IO uint32_t*) Address);
 		Address+=4;
 	}
 	return;
 }
*/
