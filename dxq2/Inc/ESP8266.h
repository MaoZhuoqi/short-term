#ifndef  _ESP8266_H_
#define	 _ESP8266_H_

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <stdbool.h>

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif
/* ���Ͷ��� ------------------------------------------------------------------*/
/******************************* ESP8266 �������Ͷ��� ***************************/
typedef enum{
	STA,
  AP,
  STA_AP  
} ENUM_Net_ModeTypeDef;

typedef enum{
	 enumTCP,
	 enumUDP,
} ENUM_NetPro_TypeDef;

typedef enum{
	Multiple_ID_0 = 0,
	Multiple_ID_1 = 1,
	Multiple_ID_2 = 2,
	Multiple_ID_3 = 3,
	Multiple_ID_4 = 4,
	Single_ID_0 = 5,
} ENUM_ID_NO_TypeDef;
	
typedef enum{
	OPEN = 0,
	WEP = 1,
	WPA_PSK = 2,
	WPA2_PSK = 3,
	WPA_WPA2_PSK = 4,
} ENUM_AP_PsdMode_TypeDef;

#define RX_BUF_MAX_LEN    256                                     //�����ջ����ֽ���

typedef struct                                    //��������֡�Ĵ���ṹ��
{
	char  Data_RX_BUF [ RX_BUF_MAX_LEN ];

    __IO uint16_t InfAll;
		  __IO uint16_t FramLength       :15;                               // 14:0 
     	
}STRUCT_USARTx_Fram;

/* �궨�� --------------------------------------------------------------------*/

/******************************** ESP8266 �������Ŷ��� ***********************************/
             
/*********************************************** ESP8266 �����궨�� *******************************************/
#define ESP8266_Usart( fmt, ... )                      USART_printf (ESP8266_USARTx, fmt, ##__VA_ARGS__ ) 
#define PC_Usart( fmt, ... )                           printf ( fmt, ##__VA_ARGS__ )
//#define PC_Usart( fmt, ... )                



/* ��չ���� ------------------------------------------------------------------*/
extern UART_HandleTypeDef husartx_esp8266;
extern STRUCT_USARTx_Fram strEsp8266_Fram_Record;
extern uint8_t esp8266_rxdata;
extern char Esp8266IPAddress[30];  // ģ��IP��ַ
extern char Esp8266SSID[30];       // ģ��SSID
extern char Esp8266PWD[30];           //ģ���ȵ����Կ

extern char User_ESP8266_ApSsid[30];  //Ҫ���ӵ��ȵ������
extern char User_ESP8266_ApPwd[30];   //Ҫ���ӵ��ȵ����Կ
extern char User_ESP8266_TcpServer_IP[4];    //Ҫ���ӵķ������� IP
extern char User_ESP8266_TcpServer_Port[10];  //Ҫ���ӵķ������Ķ˿�

/* �������� ------------------------------------------------------------------*/
void  ESP8266_Init( void );
void  ESP8266_stop( void );
void  ESP8266_Rst( void );
bool  ESP8266_Cmd( char * cmd, char * reply1, char * reply2, uint32_t waittime );
bool  ESP8266_AT_Test( void );
bool  ESP8266_Net_Mode_Choose( ENUM_Net_ModeTypeDef enumMode );
bool  ESP8266_JoinAP( char * pSSID, char * pPassWord );
bool  ESP8266_BuildAP( char * pSSID, char * pPassWord, ENUM_AP_PsdMode_TypeDef enunPsdMode );
bool  ESP8266_Enable_MultipleId( FunctionalState enumEnUnvarnishTx );
bool  ESP8266_Link_Server( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id);
bool  ESP8266_StartOrShutServer( FunctionalState enumMode, char * pPortNum, char * pTimeOver );
uint8_t ESP8266_Get_LinkStatus( void );
uint8_t ESP8266_Get_IdLinkStatus( void );
uint8_t ESP8266_Inquire_ApIp( char * pApIp, uint8_t ucArrayLength );
uint8_t ESP8266_Inquire_StaIp ( char * pStaIp, uint8_t ucArrayLength );
uint8_t ESP8266_Inquire_ApMac( char * pApMac, uint8_t ucArrayLength );
bool ESP8266_UnvarnishSend( void );
void ESP8266_ExitUnvarnishSend( void );
bool ESP8266_SendString( FunctionalState enumEnUnvarnishTx, char * pStr, uint32_t ulStrLength, ENUM_ID_NO_TypeDef ucId );
char * ESP8266_ReceiveString( FunctionalState enumEnUnvarnishTx );
uint8_t InitESP8266(void);
void  USART_printf(USART_TypeDef * USARTx, char * Data, ... );
int tcp_go(void);

#endif

/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/
