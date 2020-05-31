/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "ESP8266.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include "main.h"
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
//UART_HandleTypeDef husartx_esp8266;
extern UART_HandleTypeDef huart2;
#define husartx_esp8266 huart2

STRUCT_USARTx_Fram strEsp8266_Fram_Record = { 0 };
uint8_t esp8266_rxdata;

char Esp8266IPAddress[30] = "192.168.4.1";  // ģ��IP��ַ
char Esp8266SSID[30] = "ESP8266_yyc";      // ģ��SSID
char Esp8266PWD[30] = "12345678";           //ģ���ȵ����Կ

char User_ESP8266_ApSsid[30] = "111";      //Ҫ���ӵ��ȵ������
char User_ESP8266_ApPwd[30] = "dalao666";   //Ҫ���ӵ��ȵ����Կ
extern char User_ESP8266_TcpServer_IP[4] ; //Ҫ���ӵķ������� IP
extern char User_ESP8266_TcpServer_Port[10];      //Ҫ���ӵķ������Ķ˿�


/**
  * ��������: ����ESP8266ģ��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������ESP8266_AT_Test����
  */
void ESP8266_Rst ( void )
{
#if 1
	 ESP8266_Cmd ( "AT+RST", "OK", "ready", 2500 );   	
#else
	 ESP8266_RST_LOW();
	 HAL_Delay( 500 ); 
	 ESP8266_RST_HIGH(); 
#endif
}

/**
  * ��������: ��ESP8266ģ�鷢��ATָ��
  * �������: cmd�������͵�ָ��
  *           reply1��reply2���ڴ�����Ӧ��ΪNULL������Ӧ������Ϊ���߼���ϵ
  *           waittime���ȴ���Ӧ��ʱ��
  * �� �� ֵ: 1��ָ��ͳɹ�
  *           0��ָ���ʧ��
  * ˵    ������
  */
bool ESP8266_Cmd ( char * cmd, char * reply1, char * reply2, uint32_t waittime )
{
	strEsp8266_Fram_Record .FramLength = 0;               //���¿�ʼ�����µ����ݰ�
	char str[80];
	sprintf ( str,"%s\r\n", cmd );
	HAL_UART_Transmit_DMA(&huart2,(uint8_t *)str,strlen(str));
	if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //����Ҫ��������
		return true;
	
	HAL_Delay( waittime );                 //��ʱ
	
	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .FramLength ]  = '\0';

//	PC_Usart ( "%s", strEsp8266_Fram_Record .Data_RX_BUF );
//  printf("%s->%s\n",cmd,strEsp8266_Fram_Record .Data_RX_BUF);
	if ( ( reply1 != 0 ) && ( reply2 != 0 ) )
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) || 
						 ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) ); 
 	
	else if ( reply1 != 0 )
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) );
	
	else
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) );
	
}

/**
  * ��������: ��ESP8266ģ�����AT��������
  * �������: ��
  * �� �� ֵ: 1��ѡ��ɹ�
  *           0��ѡ��ʧ��
  * ˵    ������
  */
bool ESP8266_AT_Test ( void )
{
	char count=0;
	
	
//	HAL_Delay(1000 );
	while(count<2)
	{
		if(ESP8266_Cmd("AT","OK",NULL,500)) return 1;
		ESP8266_Rst();
    HAL_Delay(2000);
		++count;
	}
	return 0;
}

/**
  * ��������: ѡ��ESP8266ģ��Ĺ���ģʽ
  * �������: enumMode������ģʽ
  * �� �� ֵ: 1��ѡ��ɹ�
  *           0��ѡ��ʧ��
  * ˵    ������
  */
bool ESP8266_Net_Mode_Choose ( ENUM_Net_ModeTypeDef enumMode )
{
	bool result=0;
	char count=0;
	while(count<3)
	{
		switch ( enumMode )
		{
			case STA:
				result=ESP8266_Cmd ( "AT+CWMODE=1", "OK", "no change", 500 ); 
			break;
			case AP:
				result=ESP8266_Cmd ( "AT+CWMODE=2", "OK", "no change", 500 ); 
			break;
			case STA_AP:
				result=ESP8266_Cmd ( "AT+CWMODE=3", "OK", "no change", 500 ); 
			break;
			default:
				result=false;
			break;
		}
		if(result) return result;
		++count;
	}
	return 0;
}

/**
  * ��������: ESP8266ģ�������ⲿWiFi
  * �������: pSSID��WiFi�����ַ���
  *           pPassWord��WiFi�����ַ���
  * �� �� ֵ: 1�����ӳɹ�
  *           0������ʧ��
  * ˵    ������
  */
bool ESP8266_JoinAP ( char * pSSID, char * pPassWord )
{
	char cCmd [120];
	char count=0;
	sprintf ( cCmd, "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassWord );
//	while(count<10)
	{
		if(ESP8266_Cmd(cCmd,"OK",NULL,6000))return 1;
		++count;
	}
	return 0;	
}

/**
  * ��������: ESP8266ģ�鴴��WiFi�ȵ�
  * �������: pSSID��WiFi�����ַ���
  *           pPassWord��WiFi�����ַ���
  *           enunPsdMode��WiFi���ܷ�ʽ�����ַ���
  * �� �� ֵ: 1�������ɹ�
  *           0������ʧ��
  * ˵    ������
  */
bool ESP8266_BuildAP ( char * pSSID, char * pPassWord, ENUM_AP_PsdMode_TypeDef enunPsdMode )
{
	char cCmd [120];
	char count=0;
	sprintf ( cCmd, "AT+CWSAP=\"%s\",\"%s\",1,%d", pSSID, pPassWord, enunPsdMode );
	while(count<3)
	{
		if(ESP8266_Cmd(cCmd,"OK",0,500))return 1;
		++count;
	}
	return 0;	
}

/**
  * ��������: ESP8266ģ������������
  * �������: enumEnUnvarnishTx�������Ƿ������
  * �� �� ֵ: 1�����óɹ�
  *           0������ʧ��
  * ˵    ������
  */
bool ESP8266_Enable_MultipleId ( FunctionalState enumEnUnvarnishTx )
{
	char cStr [20];
	char count=0;
	sprintf ( cStr, "AT+CIPMUX=%d", ( enumEnUnvarnishTx ? 1 : 0 ) );
	while(count<3)
	{
		if(ESP8266_Cmd(cStr,"OK",0,500))return 1;
		++count;
	}
	return 0;		
}

/**
  * ��������: ESP8266ģ�������ⲿ������
  * �������: enumE������Э��
  *           ip��������IP�ַ���
  *           ComNum���������˿��ַ���
  *           id��ģ�����ӷ�������ID
  * �� �� ֵ: 1�����ӳɹ�
  *           0������ʧ��
  * ˵    ������
  */
bool ESP8266_Link_Server ( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id)
{
	char cStr [100] = { 0 }, cCmd [120];

  switch (  enumE )
  {
		case enumTCP:
		  sprintf ( cStr, "\"%s\",\"%d.%d.%d.%d\",%s", "TCP", ip[0], ip[1], ip[2], ip[3], ComNum );
		  break;
		
		case enumUDP:
		  sprintf ( cStr, "\"%s\",\"%d.%d.%d.%d\",%s", "UDP", ip[0], ip[1], ip[2], ip[3], ComNum );
		  break;
		
		default:
			break;
  }

  if ( id < 5 )
    sprintf ( cCmd, "AT+CIPSTART=%d,%s", id, cStr);

  else
	  sprintf ( cCmd, "AT+CIPSTART=%s", cStr );

	return ESP8266_Cmd ( cCmd, "OK", "ALREADY CONNECTED", 2000 );
	
}

/**
  * ��������: ESP8266ģ�鿪����رշ�����ģʽ
  * �������: enumMode������/�ر�
  *           pPortNum���������˿ں��ַ���
  *           pTimeOver����������ʱʱ���ַ�������λ����
  * �� �� ֵ: 1�������ɹ�
  *           0������ʧ��
  * ˵    ������
  */
bool ESP8266_StartOrShutServer ( FunctionalState enumMode, char * pPortNum, char * pTimeOver )
{
	char cCmd1 [120], cCmd2 [120];

	if ( enumMode )
	{
		sprintf ( cCmd1, "AT+CIPSERVER=%d,%s", 1, pPortNum );
		
		sprintf ( cCmd2, "AT+CIPSTO=%s", pTimeOver );

		return ( ESP8266_Cmd ( cCmd1, "OK", 0, 500 ) &&
						 ESP8266_Cmd ( cCmd2, "OK", 0, 500 ) );
	}
	
	else
	{
		sprintf ( cCmd1, "AT+CIPSERVER=%d,%s", 0, pPortNum );

		return ESP8266_Cmd ( cCmd1, "OK", 0, 500 );
	}
}

/**
  * ��������: ��ȡESP8266 ������״̬�����ʺϵ��˿�ʱʹ��
  * �������: ��
  * �� �� ֵ: 2�����ip
  *           3����������
  *           4��ʧȥ����
  *           0����ȡ״̬ʧ��
  * ˵    ������
  */
uint8_t ESP8266_Get_LinkStatus ( void )
{
	if ( ESP8266_Cmd ( "AT+CIPSTATUS", "OK", 0, 500 ) )
	{
		if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "STATUS:2\r\n" ) )
			return 2;
		
		else if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "STATUS:3\r\n" ) )
			return 3;
		
		else if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "STATUS:4\r\n" ) )
			return 4;		
	}
	return 0;
}

/**
  * ��������: ��ȡESP8266 �Ķ˿ڣ�Id������״̬�����ʺ϶�˿�ʱʹ��
  * �������: ��
  * �� �� ֵ: �˿ڣ�Id��������״̬����5λΪ��Чλ���ֱ��ӦId5~0��ĳλ����1���Id���������ӣ�������0���Idδ��������
  * ˵    ������
  */
uint8_t ESP8266_Get_IdLinkStatus ( void )
{
	uint8_t ucIdLinkStatus = 0x00;
	
	
	if ( ESP8266_Cmd ( "AT+CIPSTATUS", "OK", 0, 500 ) )
	{
		if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "+CIPSTATUS:0," ) )
			ucIdLinkStatus |= 0x01;
		else 
			ucIdLinkStatus &= ~ 0x01;
		
		if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "+CIPSTATUS:1," ) )
			ucIdLinkStatus |= 0x02;
		else 
			ucIdLinkStatus &= ~ 0x02;
		
		if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "+CIPSTATUS:2," ) )
			ucIdLinkStatus |= 0x04;
		else 
			ucIdLinkStatus &= ~ 0x04;
		
		if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "+CIPSTATUS:3," ) )
			ucIdLinkStatus |= 0x08;
		else 
			ucIdLinkStatus &= ~ 0x08;
		
		if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "+CIPSTATUS:4," ) )
			ucIdLinkStatus |= 0x10;
		else 
			ucIdLinkStatus &= ~ 0x10;	
	}
	return ucIdLinkStatus;
}

/**
  * ��������: ��ȡESP8266 �� AP IP
  * �������: pApIp����� AP IP ��������׵�ַ
  *           ucArrayLength����� AP IP ������ĳ���
  * �� �� ֵ: 1����ȡ�ɹ�
  *           0����ȡʧ��
  * ˵    ������
  */
uint8_t ESP8266_Inquire_ApIp ( char * pApIp, uint8_t ucArrayLength )
{
	char uc;
	
	char * pCh;
	
	
  ESP8266_Cmd ( "AT+CIFSR", "OK", 0, 500 );
	
	pCh = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "APIP,\"" );
	
	if ( pCh )
		pCh += 6;
	
	else
		return 0;
	
	for ( uc = 0; uc < ucArrayLength; uc ++ )
	{
		pApIp [ uc ] = * ( pCh + uc);
		
		if ( pApIp [ uc ] == '\"' )
		{
			pApIp [ uc ] = '\0';
			break;
		}
	}
	return 1;
}

/**
  * ��������: ��ȡESP8266 �� STA IP
  * �������: pStaIp����� STA IP ��������׵�ַ
  *           ucArrayLength����� STA IP ������ĳ���
  * �� �� ֵ: 1����ȡ�ɹ�
  *           0����ȡʧ��
  * ˵    ������
  */
uint8_t ESP8266_Inquire_StaIp ( char * pStaIp, uint8_t ucArrayLength )
{
	char uc;
	char * pCh;
  ESP8266_Cmd ( "AT+CIFSR", "OK", 0, 500 );
	
	pCh = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "STAIP,\"" );
	
	if ( pCh )
		pCh += 7;
	
	else
		return 0;
	
	for ( uc = 0; uc < ucArrayLength; uc ++ )
	{
		pStaIp [ uc ] = * ( pCh + uc);
		
		if ( pStaIp [ uc ] == '\"' )
		{
			pStaIp [ uc ] = '\0';
			break;
		}
	}
	return 1;
}

/**
  * ��������: ��ȡESP8266 �� AP MAC
  * �������: pApMac����� AP MAC ��������׵�ַ
  *           ucArrayLength����� AP MAC ������ĳ���
  * �� �� ֵ: 1����ȡ�ɹ�
  *           0����ȡʧ��
  * ˵    ������
  */
uint8_t ESP8266_Inquire_ApMac( char * pApMac, uint8_t ucArrayLength )
{
	char uc, tc;
	char * pCh;
  ESP8266_Cmd ( "AT+CIFSR", "OK", 0, 1500 );
	
	pCh = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "APMAC,\"" );
	
	if ( pCh )
		pCh += 7;
	else
		return 0;
	
	for ( uc = 0, tc = 0; uc < ucArrayLength; uc ++, tc++ )
	{
    if (*(pCh + uc) == ':')
    {
      --tc;
      continue;
    }
		pApMac [ tc ] = * ( pCh + uc);
		
		if ( pApMac [ tc ] == '\"' )
		{
			pApMac [ tc ] = '\0';
			break;
		}
	}
	return 1;
}

/**
  * ��������: ����ESP8266ģ�����͸������
  * �������: ��
  * �� �� ֵ: 1�����óɹ�
  *           0������ʧ��
  * ˵    ������
  */
bool ESP8266_UnvarnishSend ( void )
{
	
	if ( ! ESP8266_Cmd ( "AT+CIPMODE=1", "OK", 0, 1000 ) )
		return false;
	return  ESP8266_Cmd ( "AT+CIPSEND", "OK", ">", 1000 );
	
}

/**
  * ��������: ����ESP8266ģ���˳�͸��ģʽ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void ESP8266_ExitUnvarnishSend ( void )
{
	HAL_Delay(1000);	
	HAL_UART_Transmit_DMA(&huart2,"+++",3);	
	HAL_Delay(500);
}

/**
  * ��������: ESP8266ģ�鷢���ַ���
  * �������: enumEnUnvarnishTx�������Ƿ���ʹ����͸��ģʽ
  *           pStr��Ҫ���͵��ַ���
  *           ucId���ĸ�ID���͵��ַ���
  *           ulStrLength��Ҫ���͵��ַ������ֽ���
  * �� �� ֵ: 1�����ͳɹ�
  *           0������ʧ��
  * ˵    ������
  */
bool ESP8266_SendString ( FunctionalState enumEnUnvarnishTx, char * pStr, uint32_t ulStrLength, ENUM_ID_NO_TypeDef ucId )
{
	char cStr [20];
	bool bRet = false;
	
		
	if ( enumEnUnvarnishTx )
	{
		HAL_UART_Transmit_DMA ( &huart2, (uint8_t *)pStr ,ulStrLength);
		
		bRet = true;
		
	}

	else
	{
		if ( ucId < 5 )
			sprintf ( cStr, "AT+CIPSEND=%d,%d", ucId, ulStrLength + 2 );

		else
			sprintf ( cStr, "AT+CIPSEND=%d", ulStrLength + 2 );
		
		ESP8266_Cmd ( cStr, "> ", 0, 1000 );

		bRet = ESP8266_Cmd ( pStr, "SEND OK", 0, 1000 );
  }
	
	return bRet;

}

/**
  * ��������: ESP8266ģ������ַ���
  * �������: enumEnUnvarnishTx�������Ƿ���ʹ����͸��ģʽ
  * �� �� ֵ: ���յ����ַ����׵�ַ
  * ˵    ������
  */
char * ESP8266_ReceiveString ( FunctionalState enumEnUnvarnishTx )
{
	char * pRecStr = 0;
	
	
	strEsp8266_Fram_Record .FramLength = 0;

	
	while(strEsp8266_Fram_Record .FramLength==0);
	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .FramLength ] = '\0';
	
	if ( enumEnUnvarnishTx )
		pRecStr = strEsp8266_Fram_Record .Data_RX_BUF;
	else 
	{
		if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "+IPD" ) )
			pRecStr = strEsp8266_Fram_Record .Data_RX_BUF;

	}
	return pRecStr;	
}

uint8_t InitESP8266(void)
{
			uint8_t cnt = 0;
  static int ret = 1;
	HAL_UART_Transmit_DMA(&huart2,"+++",3)	;
	HAL_Delay(500);
  if(ESP8266_AT_Test())
  {
    printf("AT test OK\n");
  }
  else
	{
		ret=1;
    return ret;
	}
  if(ret<2)
	{
					 ret = 2;
				printf("<1> Set 8266 net mode...\n");
				if(ESP8266_Net_Mode_Choose(STA_AP))
				{
					printf("ESP8266_Net_Mode_Choose OK\n");
				}
				
				if (ESP8266_Inquire_ApMac(Esp8266IPAddress, 30))
				{
					sprintf(Esp8266SSID, "ESP8266_%s", Esp8266IPAddress + 8);
					ESP8266_BuildAP(Esp8266SSID, Esp8266PWD, WPA2_PSK);
				}
				else
					{
						ret=1;
						return ret;
					}
					
	}
	if(ret<3)
	{
			 ret = 3;
		printf("<2> Jion Wifi AP:%s, use password:%s\n", User_ESP8266_ApSsid, User_ESP8266_ApPwd);

		while(!ESP8266_JoinAP(User_ESP8266_ApSsid, User_ESP8266_ApPwd))
			if (++cnt > 1)
			{
				ret=2;
				return ret;
			}
	}
	
 if(ret<4)
 {
  ret = 4;
	printf("<3> Set single link\n");
//	 ESP8266_Cmd ( "AT+CIPMODE=0", "OK", 0, 1000 );
  ESP8266_Enable_MultipleId(DISABLE);
//	 ESP8266_StartOrShutServer(ENABLE,"333","50");
  
  if (ESP8266_Inquire_StaIp(Esp8266IPAddress, 30))
    printf("ESP8266 IP Address:%s\n", Esp8266IPAddress);
	else
	{
		ret=3;
		return ret;
	}
 }
 
  
 
    if(ret<5)
		{
				ret = 5;
			printf("<4> Link to tcp server %d.%d.%d.%d.:%s\n", 
						User_ESP8266_TcpServer_IP[0], User_ESP8266_TcpServer_IP[1], 
						User_ESP8266_TcpServer_IP[2], User_ESP8266_TcpServer_IP[3], User_ESP8266_TcpServer_Port);
			cnt = 0;
			while(!ESP8266_Link_Server(enumTCP,User_ESP8266_TcpServer_IP,User_ESP8266_TcpServer_Port,Single_ID_0))
				if (++cnt > 1)
				{
					ret=4;
					return ret;
				}
		}
  
  
  ret = 6;
	printf("<5> Begin unvarnishSend...\n");
  cnt = 0;
  while(!ESP8266_UnvarnishSend())
    if (++cnt > 10)
		{
			ret=5;
      return ret;
		}
	printf("���� ESP8266 ���\n");
  return 0;
}

int tcp_go(void)
{
	static int ret=0;
	int cnt;
	
if(ret<5)
		{
				ret = 5;
			ESP8266_Cmd ( "AT+CIPSERVER=0", "OK", 0, 500 );
			ESP8266_Cmd ( "AT+CIPMUX=0", "OK", 0, 500 );
			printf("<4> Link to tcp server %d.%d.%d.%d.:%s\n", 
						User_ESP8266_TcpServer_IP[0], User_ESP8266_TcpServer_IP[1], 
						User_ESP8266_TcpServer_IP[2], User_ESP8266_TcpServer_IP[3], User_ESP8266_TcpServer_Port);
			cnt = 0;
			while(!ESP8266_Link_Server(enumTCP,User_ESP8266_TcpServer_IP,User_ESP8266_TcpServer_Port,Single_ID_0))
				if (++cnt > 1)
				{
					ret=4;
					return ret;
				}
		}
  
  
  ret = 6;
	printf("<5> Begin unvarnishSend...\n");
  cnt = 0;
  while(!ESP8266_UnvarnishSend())
    if (++cnt > 10)
		{
			ret=5;
      return ret;
		}
			printf("���� ESP8266 tcp���\n");
  return 0;

}
