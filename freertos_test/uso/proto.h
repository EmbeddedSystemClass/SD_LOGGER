#ifndef PROTO_H
#define PROTO_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_flash.h"
#include "misc.h"

//#include "preferences.h"
//������� �� FreeRTOS:

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

void Proto_Init(void);//������������� ���������, ����������
void ProtoProcess( void *pvParameters );//������� ��������� ���������

#define DEVICE_NAME_LENGTH_SYM	20//����� ����� � ������
#define DEVICE_VER_LENGTH_SYM 0x8
#define DEVICE_DESC_MAX_LENGTH_SYM 40

//--------------------------------------------------------------------------------
#define MAX_LENGTH_REC_BUF 	256 //������������ ����� ������������ �����
#define MIN_LENGTH_REC_BUF	5 //����������� ����� ������������ �����

#define MAX_LENGTH_TR_BUF  	256 //������������ ����� ������������� �����
#define CRC_LEN				1 //����� ���� CRC
//-------------------------��� ��������-------------------------------------------
#define  GET_DEV_INFO_REQ 				0x1 //�������� ���������� �� ����������	(��� �������)
#define  GET_DEV_INFO_RESP				0x2	//�������� ���������� �� ����������	(��� ������)

#define  NODE_FULL_INIT_REQ				0x3	//������ ������������� ����
#define  NODE_FULL_INIT_RESP			//������ ������������� ����-��� ������

#define  CHANNEL_LIST_INIT_REQ			0x5 //������������� ������ ������� ���� (��� ������ ������);
#define  CHANNEL_LIST_INIT_RESP			//������������� ������ ������� ���� (��� ������ ������); ��� ������

#define  CHANNEL_GET_DATA_REQ			0x7// ������ ������ �� �������, �������� ���������� ���������;
#define  CHANNEL_GET_DATA_RESP			0x8// ������ ������ �� �������, �������� ���������� ���������;

#define  CHANNEL_SET_PARAMETERS_REQ  	0x9	//���������� ��������� �� �������, �������� ���������� ���������;
#define  CHANNEL_SET_PARAMETERS_RESP  	//���������� ��������� �� �������, �������� ���������� ���������; ��� ������

#define  CHANNEL_SET_ORDER_QUERY_REQ   	0xA//������ ������������������ ������;
#define  CHANNEL_SET_ORDER_QUERY_RESP 	//������ ������������������ ������; ��� ������

#define  CHANNEL_GET_DATA_ORDER_REQ		0xC	//������ ������ �� �������, �������� ������������������ ������;
#define  CHANNEL_GET_DATA_ORDER_RESP	0xD	//������ ������ �� �������, �������� ������������������ ������;

#define  CHANNEL_SET_STATE_REQ		   	0xE//���������� ��������� �� �������, �������� ���������� ���������;
#define  CHANNEL_SET_STATE_RESP			   //���������� ��������� �� �������, �������� ���������� ���������; ��� ������

#define CHANNEL_GET_DATA_ORDER_M2_REQ	0x10 //������ ������ �� �������, �������� ������������������ ������;
#define CHANNEL_GET_DATA_ORDER_M2_RESP	0x11//������ ������ �� �������, �������� ������������������ ������;

#define CHANNEL_SET_RESET_STATE_FLAGS_REQ  0x12//���������/����� ������ ���������
#define CHANNEL_SET_RESET_STATE_FLAGS_RESP //���������/����� ������ ��������� -��� ������

#define  CHANNEL_ALL_GET_DATA_REQ		0x14 //������ ���������� �� ���� ������� ���� (����������� �����);
#define  CHANNEL_ALL_GET_DATA_RESP		0x15 //������ ���������� �� ���� ������� ���� (����������� �����);

#define  CHANNEL_SET_ADDRESS_DESC		0xCD //���������� ����� ����� ����������, ���, ��������, ������ �������� � �����������

#define  CHANNEL_SET_CALIBRATE			0xCA//���������� ������� ��� ������ ����� ������������ ����������

#define  REQUEST_ERROR					0xFF//��������� ������/�����;

//-------------------------���� ������� ��������-------------------------------------------
#define	FR_SUCCESFUL 								0x0//��� ������ (������������ ��� �������������)
#define	FR_UNATTENDED_CHANNEL 						0x1//� ������� ����� �����, �� ������������� ������������� �������;
#define	FR_SET_UNATTENDED_CHANNEL					0x2//� �������� ������������������ ������ ����� �����, �� ������������� ��-����������� ������� (�������� � ����� �� �������� ������� 0Ah);
#define	FR_ORDER_NOT_SET							0x3//������������������ ������ �� ������ (�������� � ����� �� �������� ������� 0Ch);
#define	FR_CHNL_NOT_IN_ORDER_REQ					0x4//� ������� ������������ �����, �� �������� � ������������������ ������ (�������� � ����� �� �������� ������� 0Ch);
#define	FR_FALSE_TYPE_CHNL_DATA						0x5//���������� ���������� �� ������ � ������� ���� ������ ���������� �� ����� (�������� � ����� �� �������� ������� 09h ��� 0Eh);
#define	FR_COMMAND_NOT_EXIST						0x6//�������������� �������;
#define	FR_CHNL_TYPE_NOT_EXIST						0x7//�������������� ���/����������� ������;
#define	FR_COMMAND_NOT_SUPPORT						0x8//������� �� ��������������;
#define	FR_COMMAND_STRUCT_ERROR						0x9//������ � ��������� �������;
#define	FR_FRAME_LENGTH_ERROR						0xA//�������������� ����� ����� � ���������� �������;
#define	FR_CHANNEL_NUM_TOO_MUCH						0xC//������� ����� ������� � �����.
#define	FR_CHNL_TYPE_ERROR							0xD//�������������� ���� ������
#define	FR_CHNL_NOT_EXIST							0xF//������������� �����
//--------------------------��������� ��������� ��������--------------
#define PROTO_RESTART		0	//������� ����� � ���������
#define PROTO_WAIT 			1	//�������� ������
#define	PROTO_PLACE_BUF		2	//����� ������ � �����
#define	PROTO_ADDR_CHECK	3	//�������� ������
#define	PROTO_CRC_CHECK		4	//�������� ����������� �����
#define	PROTO_BUF_HANDLING	5	//��������� �������
#define PROTO_BUF_TRANSFER	6	//�������� ������ ��������
#define	PROTO_ERR_HANDLING	7	//��������� ������
//--------------------------------------------------------------------


unsigned char Send_Info(void);     //������� ���������� �� ����������
unsigned char Node_Full_Init(void);//������ ������������� ����
unsigned char Channel_List_Init(void);//������������� ������ ������� ���� (��� ������ ������);
unsigned char Channel_Get_Data(void);//������ ������ �� �������, �������� ���������� ���������;
unsigned char Channel_Set_Parameters(void);//���������� ��������� �� �������, �������� ���������� ���������;
unsigned char Channel_Set_Order_Query(void);//������ ������������������ ������;
unsigned char Channel_Get_Data_Order(void);//������ ������ �� �������, �������� ������������������ ������;
unsigned char Channel_Set_State(void);//���������� ��������� �� �������, �������� ���������� ���������;
unsigned char Channel_Get_Data_Order_M2(void);//������ ������ �� �������, �������� ������������������ ������;
unsigned char Channel_Set_Reset_State_Flags(void);//	���������/����� ������ ���������
unsigned char Channel_All_Get_Data(void);//������ ���������� �� ���� ������� ���� (����������� �����);
unsigned char Channel_Set_Address_Desc(void);//���������� ����� ����� ����������, ���, ��������, ������ �������� � �����������
unsigned char Channel_Set_Calibrate(void);//���������� ������� ��� ������ ����� ����������
unsigned char Request_Error(unsigned char error_code);//	��������� ������/�����;


void ProtoBufHandling(void); //������� ��������� ��������� �������
void Store_Dev_Address_Desc(void);
void Restore_Dev_Address_Desc(void);

unsigned char  CRC_Check( unsigned char  *Spool,unsigned char Count);//������ CRC

//void Store_Dev_Address_Desc(unsigned char addr,void* name,void* ver,void* desc,unsigned char desc_len);//��������� � ���� ����� ����� ����������, ���, ������, ��������
//void Restore_Dev_Address_Desc(void);//������������ �� ���� ����� � ���������� �� ����������
 //------------------------------------------------------------------------------


#endif
