#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <memory.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <sys/timeb.h>
#include <time.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pthread.h>
#include <signal.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include "keyValueClientLib.h"
#include "keyValueClientLibErrMsg.h"

#define SYSTEM_WINDOWS_64

#ifdef SYSTEM_WINDOWS_64
  #include <winsock2.h>
#endif

#define WAIT_OUT 0
#define WAIT_IN  1

/*�ӿ�ʼ���Կո�*/
#define IGNORE_BLANK_START( var, end ) \
  {\
  	while (keyValueIsBlank( *(var) ) && var < end)\
  	{\
	  	var++;\
	  }\
  }

#define IGNORE_BLANK_END( var, end ) \
  {\
  	while (keyValueIsBlank( *(end) ) && end > var)\
  	{\
	  	end--;\
	  }\
  }

/*******************************�����˽���**************************************/
#define  MAX_SEND_LEN         65536          /*ÿ�η������ĳ���*/
#define  TIME_OUT             60             /*��ʱʱ��*/
#define  LONG_CONNECT_TIME    30             /*��������·���ַ���ʱ����*/
#define  MAX_LONG_CONNECTS    1000           /*�������������*/
                              
#define  MAX_KEY_LEN          512            /* KEY ����󳤶�*/
#define  MAX_VALUE_LEN        4096           /* VALUE ����󳤶�*/
                              
#define  DATA_BUF_LEN         10             /*�ͻ��˹����ĳ�����*/
#define  BAT_RETURN           524288         /*��Ӧ������ѯ����ʱÿ�η��ص�����*/
#define  OPERATE_LEN          4              /*�ͻ��˹����Ĳ�������*/
#define  KEY_VALUE_LENGTH     4              /*�ͻ��˹��������ݳ���*/
#define  UPDATE_SINGLE_CNT    "0001"         /*�ͻ��˹����ĵ�������*/
#define  DELETE_SINGLE_CNT    "0002"         /*�ͻ��˹����ĵ���ɾ��*/
#define  SELECT_SINGLE_CNT    "0003"         /*�ͻ��˹����ĵ�����ѯ*/
#define  UPDATE_BAT_CNT       "0004"         /*�ͻ��˹�������������*/
#define  DELETE_BAT_CNT       "0005"         /*�ͻ��˹���������ɾ��*/
#define  SELECT_BAT_CNT       "0006"         /*�ͻ��˹�����������ѯ*/
#define  UP_DE_SE_BAT_CNT     "0007"         /*�ͻ��˹������������»�ɾ�����ѯ*/
#define  UPDATE_OPERATE_CNT   "1001"         /*�ͻ��˹����ĸ��²���*/
#define  DELETE_OPERATE_CNT   "1002"         /*�ͻ��˹�����ɾ������*/
#define  SELECT_OPERATE_CNT   "1003"         /*�ͻ��˹����Ĳ�ѯ����*/

struct keyValueLongSocketID
{
	char pcIp[20];
	char pcPort[10];
	int  iSocket;
	pthread_t threadID;
};

int igWindowsSocketDll  = 0;

struct keyValueLongSocketID sgKeyValueLongSocketID[MAX_LONG_CONNECTS];
int igKeyValueLongSocketIdSize = 0;

/*******************************�߳����**************************************/
pthread_mutex_t keyValueClientLibMutex;

#define KEY_VALUE_CLIENT_LIB_LOCK()   pthread_mutex_lock(&keyValueClientLibMutex);
#define KEY_VALUE_CLIENT_LIB_UNLOCK() pthread_mutex_unlock(&keyValueClientLibMutex);

/*******************************�ͻ����������**************************************/
#define MAX_REQUIRE 65536   /*���������*/
void *sgKeyValueMalloc[MAX_REQUIRE];
int  igKeyValueMallocSize = 0;

/*******************************�ͻ����������**************************************/
#define MAX_OPERATEION_LEN   20

#define SET_SINGLE            "set "         /*��������*/
#define DEL_SINGLE            "del "         /*����ɾ��*/
#define GET_SINGLE            "get "         /*������ѯ*/
#define SET_LIST              "setlist "     /*��������*/
#define DEL_LIST              "dellist "     /*����ɾ��*/
#define GET_LIST              "getlist "     /*������ѯ*/
#define MIX_LIST              "list "        /*�������*/


struct keyValueSendBuf
{
	char pcSendBuf[MAX_SEND_LEN];
	int  iBufLen;
	struct keyValueSendBuf *next;
};


char keyValueErrMsg[256];         /*����ʱ����Ϣ*/
int  keyValueErrCode;             /*����ʱ�Ĵ���*/
int  igTestLine;

#define SET_ERRMSG( msg, code ) \
{\
  memset( keyValueErrMsg, 0x00, sizeof(keyValueErrMsg) );\
	strcpy( keyValueErrMsg, msg );\
	keyValueErrCode=code;\
	igTestLine=__LINE__;\
}

#define INIT_ERRMSG() memset( keyValueErrMsg, 0x00, sizeof(keyValueErrMsg) );keyValueErrCode=SUCCESS;


/**********************************************************************
��������: keyValueMalloc 
��������: �����ڴ�
��    ����
��    һ�������С        I
��    �أ��ڴ��ַ
��    ��: �����
����ʱ��: 20161010
**********************************************************************/
void *keyValueMalloc( int iaSize )
{
	void *pcBuf;
	
	INIT_ERRMSG();
	
	KEY_VALUE_CLIENT_LIB_LOCK();
	
	if (igKeyValueMallocSize + 1 > MAX_REQUIRE)
	{
		SET_ERRMSG( MALLOC_ERRMSG_1, MALLOC_ERRCODE_1 );
		return NULL;
	}
	
	pcBuf = malloc( iaSize );
	if (pcBuf == NULL)
	{
		SET_ERRMSG( MALLOC_ERRMSG_2, MALLOC_ERRCODE_2 );
		return NULL;
	}
	
	sgKeyValueMalloc[igKeyValueMallocSize] = pcBuf;
	igKeyValueMallocSize++;
	
	KEY_VALUE_CLIENT_LIB_UNLOCK();
	
	return pcBuf;
}

/**********************************************************************
��������: keyValueFree 
��������: �ͷ��ڴ�
��    ����
��    һ��������ĵ�ַ     I
��    �أ�
��    ��: �����
����ʱ��: 20161010
**********************************************************************/
void keyValueFree( void *pcaAddress )
{
	int iTmp;
	
	INIT_ERRMSG();
	
	KEY_VALUE_CLIENT_LIB_LOCK();
	
	for (iTmp = 0; iTmp < igKeyValueMallocSize; iTmp++)
	{
		if (sgKeyValueMalloc[igKeyValueMallocSize] == pcaAddress)
		{
			free(pcaAddress);
			if (iTmp == igKeyValueMallocSize - 1)
			{
				igKeyValueMallocSize--;
			}
			else
			{
				sgKeyValueMalloc[iTmp] = sgKeyValueMalloc[igKeyValueMallocSize];
				igKeyValueMallocSize--;
			}
			break;
		}
	}
	
	KEY_VALUE_CLIENT_LIB_UNLOCK();
	
	return;
}

/**********************************************************************
��������: keyValueClientShort 
��������: �����Ӳ���
��    ����
��    һ�������IP        I
��    ��������˶˿�      I
��    �������ݵ�ַ        I/O
��    �ģ����ݴ�С        I/O
��    �أ�0�ǳɹ�������ʧ��
��    ��: �����
����ʱ��: 20161010
**********************************************************************/
int keyValueClientShort( char *pcaIp, char *pcaPort, void **pcaData, int *iaDataLen )
{
	char *pcBufStart;
	char *pcBufEnd;
	char *pcBlank;
	char pcOperation[MAX_OPERATEION_LEN+1];
	int  iSocket;
	int  iTmp;
	#ifdef SYSTEM_WINDOWS_64
	  WSADATA wsaData;
	#endif
	char *pcRecvBuf;
  int  iRecvLen;
  int  iRet;
	
	INIT_ERRMSG();
	
	#ifdef SYSTEM_WINDOWS_64
	  /*����Windows Sockets DLL*/
	  if (igWindowsSocketDll == 0)
	  {
      if (WSAStartup(MAKEWORD(2,1),&wsaData)) 
      { 
        WSACleanup();
        SET_ERRMSG( CLIENT_SHORT_ERRMSG_1, CLIENT_SHORT_ERRCODE_1 );
        return CLIENT_SHORT_ERRCODE_1;
      }
      igWindowsSocketDll = 1;
    }
  #endif
  	
	if (!keyValueCheckIP( pcaIp ))
	{
		SET_ERRMSG( CLIENT_SHORT_ERRMSG_2, CLIENT_SHORT_ERRCODE_2 );
		return CLIENT_SHORT_ERRCODE_2;
	}
	
	if (!keyValueCheckPort( pcaPort ))
	{
		SET_ERRMSG( CLIENT_SHORT_ERRMSG_3, CLIENT_SHORT_ERRCODE_3 );
		return CLIENT_SHORT_ERRCODE_3;
	}

	KEY_VALUE_CLIENT_LIB_LOCK();
	
	/*�����Ƿ���keyValueMalloc�������*/
	for (iTmp = 0; iTmp < igKeyValueMallocSize; iTmp++)
	{
		if (sgKeyValueMalloc[igKeyValueMallocSize] == *pcaData)
		{
			break;
		}
	}
	KEY_VALUE_CLIENT_LIB_UNLOCK();
	
	/*�������*/
	if (iTmp == igKeyValueMallocSize)
	{
		SET_ERRMSG( CLIENT_SHORT_ERRMSG_4, CLIENT_SHORT_ERRCODE_4 );
		return CLIENT_SHORT_ERRCODE_4;
	}
	
	/*ָ�����ݵĿ�ʼ�ͽ���*/
	pcBufStart = *pcaData;
	pcBufEnd = pcBufStart + *iaDataLen;
	
	/*���Կ�ʼ�ո�*/
	IGNORE_BLANK_START( pcBufStart, pcBufEnd )
	if (pcBufStart == pcBufEnd)
	{
		SET_ERRMSG( CLIENT_SHORT_ERRMSG_5, CLIENT_SHORT_ERRCODE_5 );
		return CLIENT_SHORT_ERRCODE_5;
	}
	
	/*����ĩβ�ո�*/
	IGNORE_BLANK_END( pcBufStart, pcBufEnd )
	
	/*������һ���ؼ���*//*����ո񲻴��ڣ���ʾֻ�йؼ���*/
	pcBlank = strchr( pcBufStart, ' ' );
	if (pcBlank == NULL)
	{
		SET_ERRMSG( CLIENT_SHORT_ERRMSG_6, CLIENT_SHORT_ERRCODE_6 );
		return CLIENT_SHORT_ERRCODE_6;
	}
	
	/*����������ͳ��ȹ�������ʾ������*/
	if (pcBlank - pcBufStart > MAX_OPERATEION_LEN)
	{
		SET_ERRMSG( CLIENT_SHORT_ERRMSG_7, CLIENT_SHORT_ERRCODE_7 );
		return CLIENT_SHORT_ERRCODE_7;
	}
	
	memset( pcOperation, 0x00, sizeof(pcOperation) );
	memcpy( pcOperation, pcBufStart, pcBlank - pcBufStart );
	
	/*ָ�����ݵĿ�ʼ*/
	pcBufStart = pcBlank + 1;
	
	IGNORE_BLANK_START( pcBufStart, pcBufEnd )
	if (pcBufStart == pcBufEnd)
	{
		SET_ERRMSG( CLIENT_SHORT_ERRMSG_8, CLIENT_SHORT_ERRCODE_8 );
		return CLIENT_SHORT_ERRCODE_8;
	}
	
	/*����SOCKET*/
	iSocket = keyValueConnectToServer( pcaIp, pcaPort );
	if (iSocket == -1)
	{
		#ifdef SYSTEM_WINDOWS_64
	    closesocket( iSocket );
	  #else
	    close( iSocket );
    #endif
    
    SET_ERRMSG( CLIENT_SHORT_ERRMSG_9, CLIENT_SHORT_ERRCODE_9 );
    return CLIENT_SHORT_ERRCODE_9;
	}
	
	pcRecvBuf = NULL;
	iRecvLen = 0;
	
	/*���ݲ������ͣ����ö�Ӧ�ĺ���������*/
	if (!keyValueStrcnmpIgnore( pcBufStart, SET_SINGLE, strlen(SET_SINGLE) ))
	{
		iRet = keyValueUpdateSingle( pcBufStart, pcBufEnd, iSocket, &pcRecvBuf, &iRecvLen );
	}
	else
	if (!keyValueStrcnmpIgnore( pcBufStart, DEL_SINGLE, strlen(DEL_SINGLE) ))
	{
		iRet = keyValueDeleteSingle( pcBufStart, pcBufEnd, iSocket, &pcRecvBuf, &iRecvLen );
	}
	else
	if (!keyValueStrcnmpIgnore( pcBufStart, GET_SINGLE, strlen(GET_SINGLE) ))
	{
		iRet = keyValueSelectSingle( pcBufStart, pcBufEnd, iSocket, &pcRecvBuf, &iRecvLen );
	}
	else
	if (!keyValueStrcnmpIgnore( pcBufStart, SET_LIST, strlen(SET_LIST) ))
	{
		iRet = keyValueUpdateBat( pcBufStart, pcBufEnd, iSocket, &pcRecvBuf, &iRecvLen );
	}
	else
	if (!keyValueStrcnmpIgnore( pcBufStart, DEL_LIST, strlen(DEL_LIST) ))
	{
		iRet = keyValueDeleteBat( pcBufStart, pcBufEnd, iSocket, &pcRecvBuf, &iRecvLen );
	}
	else
	if (!keyValueStrcnmpIgnore( pcBufStart, GET_LIST, strlen(GET_LIST) ))
	{
		iRet = keyValueSelectBat( pcBufStart, pcBufEnd, iSocket, &pcRecvBuf, &iRecvLen );
	}
	else
	if (!keyValueStrcnmpIgnore( pcBufStart, MIX_LIST, strlen(MIX_LIST) ))
	{
		iRet = keyValueMixBat( pcBufStart, pcBufEnd, iSocket, &pcRecvBuf, &iRecvLen );
	}
	else
	{
		#ifdef SYSTEM_WINDOWS_64
	    closesocket( iSocket );
	  #else
	    close( iSocket );
    #endif
    
    SET_ERRMSG( CLIENT_SHORT_ERRMSG_10, CLIENT_SHORT_ERRCODE_10 );
    return CLIENT_SHORT_ERRCODE_10;
	}
	
	#ifdef SYSTEM_WINDOWS_64
	  closesocket( iSocket );
	#else
	  close( iSocket );
  #endif
  
  /*����������*/ 
	if (iRet)
	{
    return iRet;
	}
	
	/*�ͷ���������ڴ�*/
	keyValueFree( *pcaData );
	
	/*��������������*/
	*pcaData = pcRecvBuf;
	*iaDataLen = iRecvLen;
  
	return SUCCESS;
}

/**********************************************************************
��������: keyValueClientLongInti 
��������: �����ӳ�ʼ��
��    ����
��    һ�������IP        I
��    ��������˶˿�      I
��    �أ�PTHREAD ID��������ʾ�ɹ���������ʾʧ��
��    ��: �����
����ʱ��: 20161010
**********************************************************************/
pthread_t keyValueClientLongInti( char *pcaIp, char *pcaPort )
{
	#ifdef SYSTEM_WINDOWS_64
	  WSADATA wsaData;
	#endif
	char *pcRecvBuf;
  int  iRecvLen;
  int  iRet;
  int  iSocket;
  int  iTmp;
  pthread_t id;
  pthread_t threadID;
	
	INIT_ERRMSG();
	
	if (!keyValueCheckIP( pcaIp ))
	{
		SET_ERRMSG( CLIENT_LONG_INTI_ERRMSG_1, CLIENT_LONG_INTI_ERRCODE_1 );
		return CLIENT_LONG_INTI_ERRCODE_1;
	}
	
	if (!keyValueCheckPort( pcaPort ))
	{
		SET_ERRMSG( CLIENT_LONG_INTI_ERRMSG_2, CLIENT_LONG_INTI_ERRCODE_2 );
		return CLIENT_LONG_INTI_ERRCODE_2;
	}
	
	KEY_VALUE_CLIENT_LIB_LOCK();
	
	/*����Ѿ������*/
	for (iTmp = 0; iTmp < igKeyValueLongSocketIdSize; iTmp++)
	{
		if (!strcmp( sgKeyValueLongSocketID[iTmp].pcIp, pcaIp ) && !strcmp( sgKeyValueLongSocketID[iTmp].pcPort, pcaPort ))
		{
			SET_ERRMSG( CLIENT_LONG_INTI_ERRMSG_3, CLIENT_LONG_INTI_ERRCODE_3 );
			KEY_VALUE_CLIENT_LIB_UNLOCK();
		  return CLIENT_LONG_INTI_ERRCODE_3;
		}
	}
	
	#ifdef SYSTEM_WINDOWS_64
	  /*����Windows Sockets DLL*/
	  if (igWindowsSocketDll == 0)
	  {
      if (WSAStartup(MAKEWORD(2,1),&wsaData)) 
      { 
        WSACleanup();
        SET_ERRMSG( CLIENT_LONG_INTI_ERRMSG_4, CLIENT_LONG_INTI_ERRCODE_4 );
        KEY_VALUE_CLIENT_LIB_UNLOCK();
        return CLIENT_LONG_INTI_ERRCODE_4;
      }
      igWindowsSocketDll = 1;
    }
  #endif
	
	/*����SOCKET*/
	iSocket = keyValueConnectToServer( pcaIp, pcaPort );
	if (iSocket == -1)
	{
		#ifdef SYSTEM_WINDOWS_64
	    closesocket( iSocket );
	  #else
	    close( iSocket );
    #endif
    
    SET_ERRMSG( CLIENT_LONG_INTI_ERRMSG_5, CLIENT_LONG_INTI_ERRCODE_5 );
    KEY_VALUE_CLIENT_LIB_UNLOCK();
    return CLIENT_LONG_INTI_ERRCODE_5;
	}
	
	/*�����߳�*/
	id = pthread_create(&threadID, NULL, (void *)(&keyValueLongSocket), &iSocket);
	if (id)
	{
		SET_ERRMSG( CLIENT_LONG_INTI_ERRMSG_6, CLIENT_LONG_INTI_ERRCODE_6 );
		KEY_VALUE_CLIENT_LIB_UNLOCK();
		return CLIENT_LONG_INTI_ERRCODE_6;
	}
	/*�̷߳��룬����ʱ�Զ�������Դ*/
  pthread_detach(threadID); 
	
	memset( sgKeyValueLongSocketID[igKeyValueLongSocketIdSize].pcIp, 0x00, sizeof(sgKeyValueLongSocketID[igKeyValueLongSocketIdSize].pcIp) );
	memset( sgKeyValueLongSocketID[igKeyValueLongSocketIdSize].pcPort, 0x00, sizeof(sgKeyValueLongSocketID[igKeyValueLongSocketIdSize].pcPort) );
	strcpy( sgKeyValueLongSocketID[igKeyValueLongSocketIdSize].pcIp, pcaIp );
	strcpy( sgKeyValueLongSocketID[igKeyValueLongSocketIdSize].pcPort, pcaPort );
	sgKeyValueLongSocketID[igKeyValueLongSocketIdSize].iSocket = iSocket;
	sgKeyValueLongSocketID[igKeyValueLongSocketIdSize].threadID = threadID;
	igKeyValueLongSocketIdSize++;
	
	KEY_VALUE_CLIENT_LIB_UNLOCK();
	return threadID;
}

/**********************************************************************
��������: keyValueLongSocket 
��������: ��������·����
��    ����
��    �أ�0�ǳɹ�������ʧ��
��    ��: �����
����ʱ��: 20161010
**********************************************************************/
void keyValueLongSocket( int *iaSocket )
{
	char pcSendLen[DATA_BUF_LEN+1];
	char cResult;
	int iRet;
	
	while (1)
	{
		sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, 0 );
	  iRet = keyValueWaitFd( *iaSocket, WAIT_OUT, TIME_OUT );
	  if (iRet == -1)
	  {
		  return;
	  }
	  
		iRet = send( *iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
		if (iRet != DATA_BUF_LEN)
	  {
		  return;
	  }
	  
	  iRet = keyValueWaitFd( *iaSocket, WAIT_IN, TIME_OUT );
	  if (iRet == -1)
	  {
		  return;
	  }
		
		iRet = recv( *iaSocket, &cResult, 1, 0 );
		if (iRet != 1)
	  {
		  return;
	  }
	  
	  if (cResult != '0')
	  {
	  	return;
	  }
	  
	  usleep( LONG_CONNECT_TIME * 1000 );
	}
	
	return;
}

/**********************************************************************
��������: keyValueClientLongRls 
��������: �ͷų�������Դ
��    ����
��    һ���߳�ID        I
��    �أ�
��    ��: �����
����ʱ��: 20161010
**********************************************************************/
void keyValueClientLongRls(pthread_t threadID)
{
	int iTmp;
	
	KEY_VALUE_CLIENT_LIB_LOCK();
	
	/*�Ƿ����*/
	for (iTmp = 0; iTmp < igKeyValueLongSocketIdSize; iTmp++)
	{
		if (sgKeyValueLongSocketID[iTmp].threadID == threadID )
		{
			break;
		}
	}
	
	if (iTmp == igKeyValueLongSocketIdSize)
	{
		KEY_VALUE_CLIENT_LIB_UNLOCK();
		return;
	}
	
	#ifdef SYSTEM_WINDOWS_64
	  closesocket( sgKeyValueLongSocketID[iTmp].iSocket );
	#else
	  close( sgKeyValueLongSocketID[iTmp].iSocket );
  #endif  
  
  memset( sgKeyValueLongSocketID[iTmp].pcIp, 0x00, sizeof(sgKeyValueLongSocketID[iTmp].pcIp) );
	memset( sgKeyValueLongSocketID[iTmp].pcPort, 0x00, sizeof(sgKeyValueLongSocketID[iTmp].pcPort) );
	strcpy( sgKeyValueLongSocketID[iTmp].pcIp, sgKeyValueLongSocketID[igKeyValueLongSocketIdSize-1].pcIp );
	strcpy( sgKeyValueLongSocketID[iTmp].pcPort, sgKeyValueLongSocketID[igKeyValueLongSocketIdSize-1].pcPort );
	sgKeyValueLongSocketID[iTmp].iSocket = sgKeyValueLongSocketID[igKeyValueLongSocketIdSize-1].iSocket;
	sgKeyValueLongSocketID[iTmp].threadID = sgKeyValueLongSocketID[igKeyValueLongSocketIdSize-1].threadID;
	igKeyValueLongSocketIdSize--;
	
	pthread_cancel( threadID );
  
  KEY_VALUE_CLIENT_LIB_UNLOCK();
	return;
}

/**********************************************************************
��������: keyValueClientLong 
��������: �����Ӳ���
��    ����
��    һ���߳�ID          I
��    �������ݵ�ַ        I/O
��    �������ݴ�С        I/O
��    �أ�0�ǳɹ�������ʧ��
��    ��: �����
����ʱ��: 20161010
**********************************************************************/
int keyValueClientLong( pthread_t threadID, void **pcaData, int *iaDataLen )
{
	char *pcBufStart;
	char *pcBufEnd;
	char *pcBlank;
	char pcOperation[MAX_OPERATEION_LEN+1];
	int  iSocket;
	int  iTmp;
	char *pcRecvBuf;
  int  iRecvLen;
  int  iRet;
	
	INIT_ERRMSG();
	
	KEY_VALUE_CLIENT_LIB_LOCK();
	
	/*�����Ƿ���keyValueMalloc�������*/
	for (iTmp = 0; iTmp < igKeyValueMallocSize; iTmp++)
	{
		if (sgKeyValueMalloc[igKeyValueMallocSize] == *pcaData)
		{
			break;
		}
	}
	/*�������*/
	if (iTmp == igKeyValueMallocSize)
	{
		SET_ERRMSG( CLIENT_LONG_ERRMSG_1, CLIENT_LONG_ERRCODE_1 );
		KEY_VALUE_CLIENT_LIB_UNLOCK();
		return CLIENT_LONG_ERRCODE_1;
	}
	
	/*�����Ƿ��ѳ�ʼ��*/
	for (iTmp = 0; iTmp < igKeyValueLongSocketIdSize; iTmp++)
	{
		if (sgKeyValueLongSocketID[igKeyValueLongSocketIdSize].threadID == threadID)
		{
			break;
		}
	}
	/*�������*/
	if (iTmp == igKeyValueLongSocketIdSize)
	{
		SET_ERRMSG( CLIENT_LONG_ERRMSG_2, CLIENT_LONG_ERRCODE_2 );
		KEY_VALUE_CLIENT_LIB_UNLOCK();
		return CLIENT_LONG_ERRCODE_2;
	}
	
	iSocket = sgKeyValueLongSocketID[igKeyValueLongSocketIdSize].iSocket;
	
	KEY_VALUE_CLIENT_LIB_UNLOCK();
	
	/*ָ�����ݵĿ�ʼ�ͽ���*/
	pcBufStart = *pcaData;
	pcBufEnd = pcBufStart + *iaDataLen;
	
	/*���Կ�ʼ�ո�*/
	IGNORE_BLANK_START( pcBufStart, pcBufEnd )
	if (pcBufStart == pcBufEnd)
	{
		SET_ERRMSG( CLIENT_LONG_ERRMSG_3, CLIENT_LONG_ERRCODE_3 );
		return CLIENT_LONG_ERRCODE_3;
	}
	
	/*����ĩβ�ո�*/
	IGNORE_BLANK_END( pcBufStart, pcBufEnd )
	
	/*������һ���ؼ���*//*����ո񲻴��ڣ���ʾֻ�йؼ���*/
	pcBlank = strchr( pcBufStart, ' ' );
	if (pcBlank == NULL)
	{
		SET_ERRMSG( CLIENT_LONG_ERRMSG_4, CLIENT_LONG_ERRCODE_4 );
		return CLIENT_LONG_ERRCODE_4;
	}
	
	/*����������ͳ��ȹ�������ʾ������*/
	if (pcBlank - pcBufStart > MAX_OPERATEION_LEN)
	{
		SET_ERRMSG( CLIENT_LONG_ERRMSG_5, CLIENT_LONG_ERRCODE_5 );
		return CLIENT_LONG_ERRCODE_5;
	}
	
	memset( pcOperation, 0x00, sizeof(pcOperation) );
	memcpy( pcOperation, pcBufStart, pcBlank - pcBufStart );
	
	/*ָ�����ݵĿ�ʼ*/
	pcBufStart = pcBlank + 1;
	
	IGNORE_BLANK_START( pcBufStart, pcBufEnd )
	if (pcBufStart == pcBufEnd)
	{
		SET_ERRMSG( CLIENT_LONG_ERRMSG_6, CLIENT_LONG_ERRCODE_6 );
		return CLIENT_LONG_ERRCODE_6;
	}
	
	pcRecvBuf = NULL;
	iRecvLen = 0;
	
	iRet = keyValueWaitFd( iSocket, WAIT_OUT, TIME_OUT );
	if (iRet == -1)
	{
		SET_ERRMSG( CLIENT_LONG_ERRMSG_7, CLIENT_LONG_ERRCODE_7 );
		return CLIENT_LONG_ERRCODE_7;
	}
	
	/*���ݲ������ͣ����ö�Ӧ�ĺ���������*/
	if (!keyValueStrcnmpIgnore( pcBufStart, SET_SINGLE, strlen(SET_SINGLE) ))
	{
		iRet = keyValueUpdateSingle( pcBufStart, pcBufEnd, iSocket, &pcRecvBuf, &iRecvLen );
	}
	else
	if (!keyValueStrcnmpIgnore( pcBufStart, DEL_SINGLE, strlen(DEL_SINGLE) ))
	{
		iRet = keyValueDeleteSingle( pcBufStart, pcBufEnd, iSocket, &pcRecvBuf, &iRecvLen );
	}
	else
	if (!keyValueStrcnmpIgnore( pcBufStart, GET_SINGLE, strlen(GET_SINGLE) ))
	{
		iRet = keyValueSelectSingle( pcBufStart, pcBufEnd, iSocket, &pcRecvBuf, &iRecvLen );
	}
	else
	if (!keyValueStrcnmpIgnore( pcBufStart, SET_LIST, strlen(SET_LIST) ))
	{
		iRet = keyValueUpdateBat( pcBufStart, pcBufEnd, iSocket, &pcRecvBuf, &iRecvLen );
	}
	else
	if (!keyValueStrcnmpIgnore( pcBufStart, DEL_LIST, strlen(DEL_LIST) ))
	{
		iRet = keyValueDeleteBat( pcBufStart, pcBufEnd, iSocket, &pcRecvBuf, &iRecvLen );
	}
	else
	if (!keyValueStrcnmpIgnore( pcBufStart, GET_LIST, strlen(GET_LIST) ))
	{
		iRet = keyValueSelectBat( pcBufStart, pcBufEnd, iSocket, &pcRecvBuf, &iRecvLen );
	}
	else
	if (!keyValueStrcnmpIgnore( pcBufStart, MIX_LIST, strlen(MIX_LIST) ))
	{
		iRet = keyValueMixBat( pcBufStart, pcBufEnd, iSocket, &pcRecvBuf, &iRecvLen );
	}
	else
	{ 
    SET_ERRMSG( CLIENT_LONG_ERRMSG_8, CLIENT_LONG_ERRCODE_8 );
    return CLIENT_LONG_ERRCODE_8;
	}
  
  /*����������*/ 
	if (iRet)
	{
    return iRet;
	}
	
	/*�ͷ���������ڴ�*/
	keyValueFree( *pcaData );
	
	/*��������������*/
	*pcaData = pcRecvBuf;
	*iaDataLen = iRecvLen;
  
	return SUCCESS;
}

/**********************************************************************
��������: keyValueUpdateSingle 
��������: �������²���
��    ����
��    һ���������ݿ�ʼ��ַ      I
��    �����������ݽ�����ַ      I
��    ����SOCKET                I
��    �ģ�������ݵ�ַ          O
��    �壺������ݴ�С          O
��    �أ�0�ǳɹ�������ʧ��
��    ��: �����
����ʱ��: 20161010
**********************************************************************/
int keyValueUpdateSingle( char *pcaBufStart, char *pcaBufEnd, int iaSocket, char **pcaOutBuf, int *iaOutLen )
{
	char pcSendLen[DATA_BUF_LEN+1];
	char pcSendBuf[OPERATE_LEN+KEY_VALUE_LENGTH+MAX_KEY_LEN+KEY_VALUE_LENGTH+MAX_VALUE_LEN+1];
	char pcKeyValueLen[KEY_VALUE_LENGTH + 1];
	char pcKey[MAX_KEY_LEN + 1];
	char pcValue[MAX_VALUE_LEN + 1];
	char *pcKeyEnd;
	char *pcValueStart;
	int iTmp;
	int iTmp2;
	int iKeyValueLen;
	int iSendLen;
	int iRet;
	int iSocket;
	char cResult;
	char *pcBufStart;
	char *pcBufEnd;
	
	INIT_ERRMSG();
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	iSocket = iaSocket;
	
	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
  if (pcBufStart == pcBufEnd)
  {
  	SET_ERRMSG( UPDATE_SINGLE_ERRMSG_1, UPDATE_SINGLE_ERRCODE_1 );
  	return UPDATE_SINGLE_ERRCODE_1;
  }
  
  memset( pcKey, 0x00, sizeof(pcKey) );
  pcKeyEnd = strchr( pcBufStart, ' ' );
  if (pcKeyEnd == NULL || pcKeyEnd > pcBufEnd)
  {
  	SET_ERRMSG( UPDATE_SINGLE_ERRMSG_2, UPDATE_SINGLE_ERRCODE_2 );
  	return UPDATE_SINGLE_ERRCODE_2;
  }
  
  if (pcKeyEnd - pcBufStart > MAX_KEY_LEN)
  {
  	SET_ERRMSG( UPDATE_SINGLE_ERRMSG_3, UPDATE_SINGLE_ERRCODE_3 );
  	return UPDATE_SINGLE_ERRCODE_3;
  }
  memcpy( pcKey, pcBufStart, pcKeyEnd - pcBufStart );
  
  pcBufStart = pcKeyEnd;

  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
  if (pcBufStart == pcBufEnd)
  {
  	SET_ERRMSG( UPDATE_SINGLE_ERRMSG_4, UPDATE_SINGLE_ERRCODE_4 );
  	return UPDATE_SINGLE_ERRCODE_4;
  }
  
  if (pcBufEnd - pcBufStart > MAX_VALUE_LEN)
  {
  	SET_ERRMSG( UPDATE_SINGLE_ERRMSG_5, UPDATE_SINGLE_ERRCODE_5 );
  	return UPDATE_SINGLE_ERRCODE_5;
  }
  memset( pcValue, 0x00, sizeof(pcValue) );
  memcpy( pcValue, pcBufStart, pcBufEnd - pcBufStart + 1 );
  
  memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
  
  iKeyValueLen = strlen( pcKey );
	sprintf( pcSendBuf, "%s%0*d%s", UPDATE_SINGLE_CNT, KEY_VALUE_LENGTH, iKeyValueLen, pcKey );
	
	iKeyValueLen = strlen( pcValue );
	sprintf( pcSendBuf + strlen( pcSendBuf ), "%0*d%s", KEY_VALUE_LENGTH, iKeyValueLen, pcValue );
  
  iSendLen = strlen( pcSendBuf );
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
	
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SET_ERRMSG( UPDATE_SINGLE_ERRMSG_6, UPDATE_SINGLE_ERRCODE_6 );
		return UPDATE_SINGLE_ERRCODE_6;
	}
	
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SET_ERRMSG( UPDATE_SINGLE_ERRMSG_7, UPDATE_SINGLE_ERRCODE_7 );
		return UPDATE_SINGLE_ERRCODE_7;
	}
		
	iRet = keyValueWaitFd( iSocket, WAIT_IN, TIME_OUT );
	if (iRet == -1)
	{
		SET_ERRMSG( UPDATE_SINGLE_ERRMSG_8, UPDATE_SINGLE_ERRCODE_8 );
		return UPDATE_SINGLE_ERRCODE_8;
	}
  
  cResult = 0;
  
	iRet = recv( iSocket, &cResult, 1, 0 );
	if (iRet <= 0)
	{
		SET_ERRMSG( UPDATE_SINGLE_ERRMSG_9, UPDATE_SINGLE_ERRCODE_9 );
		return UPDATE_SINGLE_ERRCODE_9;
	}
	
	*pcaOutBuf = keyValueMalloc( 1 + 1 );
	if (*pcaOutBuf == NULL)
	{
		SET_ERRMSG( UPDATE_SINGLE_ERRMSG_10, UPDATE_SINGLE_ERRCODE_10 );
		return UPDATE_SINGLE_ERRCODE_10;
	}
	
	memset( *pcaOutBuf, 0x00, 1 + 1 );
	memcpy( *pcaOutBuf, &cResult, 1 );
	*iaOutLen = 1;
	
	return SUCCESS;
}

/**********************************************************************
��������: keyValueDeleteSingle 
��������: ����ɾ������
��    ����
��    һ���������ݿ�ʼ��ַ      I
��    �����������ݽ�����ַ      I
��    ����SOCKET                I
��    �ģ�������ݵ�ַ          O
��    �壺������ݴ�С          O
��    �أ�0�ǳɹ�������ʧ��
��    ��: �����
����ʱ��: 20161010
**********************************************************************/
int keyValueDeleteSingle( char *pcaBufStart, char *pcaBufEnd, int iaSocket, char **pcaOutBuf, int *iaOutLen )
{
	char pcSendLen[DATA_BUF_LEN+1];
	char pcSendBuf[OPERATE_LEN+KEY_VALUE_LENGTH+MAX_KEY_LEN+1];
	char pcKeyValueLen[KEY_VALUE_LENGTH + 1];
	char pcKey[MAX_KEY_LEN + 1];
	char *pcValueStart;
	int iKeyValueLen;
	int iSendLen;
	int iRet;
	int iSocket;
	char cResult;
	char *pcBufStart;
	char *pcBufEnd;
	
	INIT_ERRMSG();
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	iSocket = iaSocket;

  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
  if (pcBufStart == pcBufEnd)
  {
  	SET_ERRMSG( DELETE_SINGLE_ERRMSG_1, DELETE_SINGLE_ERRCODE_1 );
  	return DELETE_SINGLE_ERRCODE_1;
  }
  
  if (pcBufEnd - pcBufStart > MAX_KEY_LEN)
  {
  	SET_ERRMSG( DELETE_SINGLE_ERRMSG_2, DELETE_SINGLE_ERRCODE_2 );
  	return DELETE_SINGLE_ERRCODE_2;
  }
  memset( pcKey, 0x00, sizeof(pcKey) );
  memcpy( pcKey, pcBufStart, pcBufEnd - pcBufStart + 1 );
  
  memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
  
  iKeyValueLen = strlen( pcKey );
	sprintf( pcSendBuf, "%s%0*d%s", DELETE_SINGLE_CNT, KEY_VALUE_LENGTH, iKeyValueLen, pcKey );
  
  iSendLen = strlen( pcSendBuf );
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
	
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SET_ERRMSG( DELETE_SINGLE_ERRMSG_3, DELETE_SINGLE_ERRCODE_3 );
		return DELETE_SINGLE_ERRCODE_3;
	}
	
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SET_ERRMSG( DELETE_SINGLE_ERRMSG_4, DELETE_SINGLE_ERRCODE_4 );
		return DELETE_SINGLE_ERRCODE_4;
	}
		
	iRet = keyValueWaitFd( iSocket, WAIT_IN, TIME_OUT );
	if (iRet == -1)
	{
		SET_ERRMSG( DELETE_SINGLE_ERRMSG_5, DELETE_SINGLE_ERRCODE_5 );
		return DELETE_SINGLE_ERRCODE_5;
	}
  
  cResult = 0;
  
	iRet = recv( iSocket, &cResult, 1, 0 );
	if (iRet <= 0)
	{
		SET_ERRMSG( DELETE_SINGLE_ERRMSG_6, DELETE_SINGLE_ERRCODE_6 );
		return DELETE_SINGLE_ERRCODE_6;
	}
	
	*pcaOutBuf = keyValueMalloc( 1 + 1 );
	if (*pcaOutBuf == NULL)
	{
		SET_ERRMSG( DELETE_SINGLE_ERRMSG_7, DELETE_SINGLE_ERRCODE_7 );
		return DELETE_SINGLE_ERRCODE_7;
	}
	
	memset( *pcaOutBuf, 0x00, 1 + 1 );
	memcpy( *pcaOutBuf, &cResult, 1 );
	*iaOutLen = 1;
	
	return SUCCESS;
}

/**********************************************************************
��������: keyValueSelectSingle 
��������: ������ѯ����
��    ����
��    һ���������ݿ�ʼ��ַ      I
��    �����������ݽ�����ַ      I
��    ����SOCKET                I
��    �ģ�������ݵ�ַ          O
��    �壺������ݴ�С          O
��    �أ�0�ǳɹ�������ʧ��
��    ��: �����
����ʱ��: 20161010
**********************************************************************/
int keyValueSelectSingle( char *pcaBufStart, char *pcaBufEnd, int iaSocket, char **pcaOutBuf, int *iaOutLen )
{
	char pcSendLen[DATA_BUF_LEN+1];
	char pcSendBuf[OPERATE_LEN+KEY_VALUE_LENGTH+MAX_KEY_LEN+1];
	char pcKeyValueLen[KEY_VALUE_LENGTH + 1];
	char pcKey[MAX_KEY_LEN + 1];
	char pcValue[MAX_VALUE_LEN + 1];
	int iKeyValueLen;
	int iSendLen;
	int iRecvLen;
	int iRet;
	int iSocket;
	char cResult;
	char *pcBufStart;
	char *pcBufEnd;
	
	INIT_ERRMSG();
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	iSocket = iaSocket;

  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
  if (pcBufStart == pcBufEnd)
  {
  	SET_ERRMSG( SELECT_SINGLE_ERRMSG_1, SELECT_SINGLE_ERRCODE_1 );
  	return SELECT_SINGLE_ERRCODE_1;
  }
  
  if (pcBufEnd - pcBufStart > MAX_KEY_LEN)
  {
  	SET_ERRMSG( SELECT_SINGLE_ERRMSG_2, SELECT_SINGLE_ERRCODE_2 );
  	return SELECT_SINGLE_ERRCODE_2;
  }
  memset( pcKey, 0x00, sizeof(pcKey) );
  memcpy( pcKey, pcBufStart, pcBufEnd - pcBufStart + 1 );
  
  memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
  
  iKeyValueLen = strlen( pcKey );
	sprintf( pcSendBuf, "%s%0*d%s", DELETE_SINGLE_CNT, KEY_VALUE_LENGTH, iKeyValueLen, pcKey );
  
  iSendLen = strlen( pcSendBuf );
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
	
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SET_ERRMSG( SELECT_SINGLE_ERRMSG_3, SELECT_SINGLE_ERRCODE_3 );
		return SELECT_SINGLE_ERRCODE_3;
	}
	
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SET_ERRMSG( SELECT_SINGLE_ERRMSG_4, SELECT_SINGLE_ERRCODE_4 );
		return SELECT_SINGLE_ERRCODE_4;
	}
		
	iRet = keyValueWaitFd( iSocket, WAIT_IN, TIME_OUT );
	if (iRet == -1)
	{
		SET_ERRMSG( SELECT_SINGLE_ERRMSG_5, SELECT_SINGLE_ERRCODE_5 );
		return SELECT_SINGLE_ERRCODE_5;
	}
  
  cResult = 0;
  
	iRet = recv( iSocket, &cResult, 1, 0 );
	if (iRet <= 0)
	{
		SET_ERRMSG( SELECT_SINGLE_ERRMSG_6, SELECT_SINGLE_ERRCODE_6 );
		return SELECT_SINGLE_ERRCODE_6;
	}
	
	if (cResult == '0')
  {
  	memset( pcKeyValueLen, 0x00, sizeof(pcKeyValueLen) );
  	iRecvLen = 0;
  	while (1)
  	{
	    iRet = recv( iSocket, pcKeyValueLen + iRecvLen, KEY_VALUE_LENGTH - iRecvLen, 0 );
	    if (iRet <= 0)
	    {
	    	SET_ERRMSG( SELECT_SINGLE_ERRMSG_7, SELECT_SINGLE_ERRCODE_7 );
		    return SELECT_SINGLE_ERRCODE_7;
	    }
	    iRecvLen += iRet;
	    if (iRecvLen == KEY_VALUE_LENGTH)
	    {
	    	break;
	    }
	  }
	  
	  iKeyValueLen = atoi( pcKeyValueLen );
	  memset( pcKeyValueLen, 0x00, sizeof(pcKeyValueLen) );
	  iRecvLen = 0;
  	while (1)
  	{
	    iRet = recv( iSocket, pcValue + iRecvLen, iKeyValueLen - iRecvLen, 0 );
	    if (iRet <= 0)
	    {
	    	SET_ERRMSG( SELECT_SINGLE_ERRMSG_7, SELECT_SINGLE_ERRCODE_7 );
		    return SELECT_SINGLE_ERRCODE_7;
	    }
	    iRecvLen += iRet;
	    if (iRecvLen == iKeyValueLen)
	    {
	    	break;
	    }
	  }
	}
	
	if (cResult == '0')
	{
	  *pcaOutBuf = keyValueMalloc( 1 + 1 );
	  if (*pcaOutBuf == NULL)
	  {
	  	SET_ERRMSG( SELECT_SINGLE_ERRMSG_8, SELECT_SINGLE_ERRCODE_8 );
	  	return SELECT_SINGLE_ERRCODE_8;
	  }
	  
	  memset( *pcaOutBuf, 0x00, 1 + 1 );
	  memcpy( *pcaOutBuf, &cResult, 1 );
	  *iaOutLen = 1;
  }
  else
  {
  	pcaOutBuf = keyValueMalloc( 1 + iKeyValueLen + 1 );
	  if (*pcaOutBuf == NULL)
	  {
	  	SET_ERRMSG( SELECT_SINGLE_ERRMSG_8, SELECT_SINGLE_ERRCODE_8 );
	  	return SELECT_SINGLE_ERRCODE_8;
	  }
	  
	  memset( *pcaOutBuf, 0x00, 1 + 1 );
	  memcpy( *pcaOutBuf, &cResult, 1 );
	  memcpy( (*pcaOutBuf) + 1, pcValue, iKeyValueLen );
	  
	  *iaOutLen = 1 + iKeyValueLen;
  }
	
	return SUCCESS;
}

/**********************************************************************
��������: keyValueUpdateBat 
��������: �������²���
��    ����
��    һ���������ݿ�ʼ��ַ      I
��    �����������ݽ�����ַ      I
��    ����SOCKET                I
��    �ģ�������ݵ�ַ          O
��    �壺������ݴ�С          O
��    �أ�0�ǳɹ�������ʧ��
��    ��: �����
����ʱ��: 20161010
**********************************************************************/
int keyValueUpdateBat( char *pcaBufStart, char *pcaBufEnd, int iaSocket, char **pcaOutBuf, int *iaOutLen )
{
	char pcSendLen[DATA_BUF_LEN+1];
	char pcKey[MAX_KEY_LEN + 1];
	char pcValue[MAX_VALUE_LEN + 1];
	char pcRecvLen[DATA_BUF_LEN + 1];
	struct keyValueSendBuf *kvsb;
	struct keyValueSendBuf *kvsbTmp;
	struct keyValueSendBuf *kvsbNext;
	char *pcSemicolon;
	char *pcBlank;
	char *pcSendBuf;
	char *pcRecvBuf;
	int iTmp;
	int iKeyLen;
	int iValueLen;
	int iSendLen;
	int iRecvLen;
	int iRecvSize;
	int iSocket;
	int iRet;
	int iTotalSend;
	int iOffset;
	char *pcBufStart;
	char *pcBufEnd;
	
	INIT_ERRMSG();
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	iSocket = iaSocket;
	
	/*���Կ�ʼ�ո�*/
	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
  if (pcBufStart == pcBufEnd)
  {
  	SET_ERRMSG( UPDATE_BAT_ERRMSG_1, UPDATE_BAT_ERRCODE_1 );
  	return UPDATE_BAT_ERRCODE_1;
  }
  
  /*����ĩβ�ո�*/
  IGNORE_BLANK_END( pcBufStart, pcBufEnd );
	
	kvsb = malloc( sizeof(struct keyValueSendBuf) );
	if (kvsb == NULL)
	{
		SET_ERRMSG( UPDATE_BAT_ERRMSG_2, UPDATE_BAT_ERRCODE_2 );
		return UPDATE_BAT_ERRCODE_2;
	}
	memset( kvsb, 0x00, sizeof(struct keyValueSendBuf) );
	
	strcpy( kvsb->pcSendBuf, UPDATE_BAT_CNT );
	kvsb->iBufLen = OPERATE_LEN;
	
	/*���ͻ���Ҫ���͵�����ת��*/
	while (1)
  {
  	/*���Կ�ʼ�ո�*/
	  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
    if (pcBufStart == pcBufEnd)
    {
  	  break;
    }
      
  	/*���� KEY VALUE �ָ��*/
  	pcSemicolon = strchr( pcBufStart, ';' );
  	if (pcSemicolon == NULL || pcSemicolon > pcBufEnd )
  	{
  		pcSemicolon = pcBufEnd;
  	}
  	
  	/*����ĩβ�ո�*/
    IGNORE_BLANK_END( pcBufStart, pcSemicolon );
  	
  	/*ȡ KEY */
  	{
  	  pcBlank = strchr( pcBufStart, ' ' );
  	  if (pcBlank == NULL || pcBlank > pcSemicolon)
  	  {
  	  	kvsbNext = kvsb;
  	  	while (kvsbNext != NULL)
  	    {
  	    	kvsbTmp = kvsbNext;
  	    	kvsbNext = kvsbNext->next;
  	    	free(kvsbTmp);
  	    }
  	    SET_ERRMSG( UPDATE_BAT_ERRMSG_3, UPDATE_BAT_ERRCODE_3 );
  	  	return UPDATE_BAT_ERRCODE_3;
  	  }
  	  
  	  iKeyLen = pcBlank - pcBufStart;
  	  if (iKeyLen > MAX_KEY_LEN)
  	  {
  	  	kvsbNext = kvsb;
  	  	while (kvsbNext != NULL)
  	    {
  	    	kvsbTmp = kvsbNext;
  	    	kvsbNext = kvsbNext->next;
  	    	free(kvsbTmp);
  	    }
  	    SET_ERRMSG( UPDATE_BAT_ERRMSG_4, UPDATE_BAT_ERRCODE_4 );
  	  	return UPDATE_BAT_ERRCODE_4;
  	  }
  	  
  	  memset( pcKey, 0x00, sizeof(pcKey) );
  	  memcpy( pcKey, pcBufStart, iKeyLen );
  	  
  	  pcBufStart = pcBlank + 1;
    }
  	
  	/*ȡ VALLUE */
  	{
  		/*���Կ�ʼ�ո�*/
	    IGNORE_BLANK_START( pcBufStart, pcSemicolon );
      if (pcBufStart == pcSemicolon)
      {
      	kvsbNext = kvsb;
  	  	while (kvsbNext != NULL)
  	    {
  	    	kvsbTmp = kvsbNext;
  	    	kvsbNext = kvsbNext->next;
  	    	free(kvsbTmp);
  	    }
  	    SET_ERRMSG( UPDATE_BAT_ERRMSG_5, UPDATE_BAT_ERRCODE_5 );
  	    return UPDATE_BAT_ERRCODE_5;
      }
      
      iValueLen = pcSemicolon - pcBufStart;
      if (iValueLen > MAX_VALUE_LEN)
      {
      	kvsbNext = kvsb;
  	  	while (kvsbNext != NULL)
  	    {
  	    	kvsbTmp = kvsbNext;
  	    	kvsbNext = kvsbNext->next;
  	    	free(kvsbTmp);
  	    }
  	    SET_ERRMSG( UPDATE_BAT_ERRMSG_6, UPDATE_BAT_ERRCODE_6 );
  	  	return UPDATE_BAT_ERRCODE_6;
      }
      memset( pcValue, 0x00, sizeof(pcValue) );
  	  memcpy( pcValue, pcBufStart, iValueLen );
  	  
  	  pcBufStart = pcSemicolon + 1;
  	}
  	
  	/*����KEY VALUE*/
  	{
  	  kvsbNext = kvsb;
  	  while (kvsbNext->next != NULL)
  	  {
  		  kvsbNext = kvsbNext->next;
  	  }
  	  
  	  if (kvsbNext->iBufLen + KEY_VALUE_LENGTH + iKeyLen + KEY_VALUE_LENGTH + iValueLen > MAX_SEND_LEN)
  	  {
  	  	kvsbNext->next = malloc( sizeof(struct keyValueSendBuf) );
  	  	if (kvsbNext->next == NULL)
  	  	{
  	  		kvsbNext = kvsb;
  	  	  while (kvsbNext != NULL)
  	      {
  	      	kvsbTmp = kvsbNext;
  	      	kvsbNext = kvsbNext->next;
  	      	free(kvsbTmp);
  	      }
  	      SET_ERRMSG( UPDATE_BAT_ERRMSG_7, UPDATE_BAT_ERRCODE_7 );
  	  	  return UPDATE_BAT_ERRCODE_7;
  	  	}
  	  	
  	  	memset( kvsbNext->next, 0x00, sizeof(struct keyValueSendBuf) );
  	  	kvsbNext = kvsbNext->next;
  	  }
  	  
  	  sprintf( (kvsbNext->pcSendBuf) + (kvsbNext->iBufLen), "%0*d%s%0*d%s", KEY_VALUE_LENGTH, iKeyLen, pcKey, KEY_VALUE_LENGTH, iValueLen, pcValue );
  	  kvsbNext->iBufLen += KEY_VALUE_LENGTH + iKeyLen + KEY_VALUE_LENGTH + iValueLen;
    }
  	
  	if (pcBufStart >= pcBufEnd)
  	{
  		break;
  	}
  }
  
  /*�����ܷ������ݴ�С*/
  kvsbNext = kvsb;
  iTotalSend = 0;
  while (kvsbNext != NULL)
  {
  	iTotalSend += kvsbNext->iBufLen;
  	kvsbNext = kvsbNext->next;
  }
  
  /*����ռ�*/
  pcSendBuf = malloc( iTotalSend );
  if (pcSendBuf == NULL)
  {
  	kvsbNext = kvsb;
  	while (kvsbNext != NULL)
  	{
  		kvsbTmp = kvsbNext;
  		kvsbNext = kvsbNext->next;
  		free(kvsbTmp);
  	}
  	SET_ERRMSG( UPDATE_BAT_ERRMSG_8, UPDATE_BAT_ERRCODE_8 );
	  return UPDATE_BAT_ERRCODE_8;
  }
  memset( pcSendBuf, 0x00, iTotalSend );
  
  /*ȡ�����е�����*/
  kvsbNext = kvsb;
  iOffset = 0;
  while (kvsbNext != NULL)
  {
  	memcpy( pcSendBuf + iOffset, kvsbNext->pcSendBuf, kvsbNext->iBufLen );
  	iOffset += kvsbNext->iBufLen;
  	kvsbNext = kvsbNext->next;
  }
  
  /*�ͷŷ�����������*/
  kvsbNext = kvsb;
  while (kvsbNext != NULL)
  {
  	kvsbTmp = kvsbNext;
  	kvsbNext = kvsbNext->next;
  	free(kvsbTmp);
  }
  	
  /*�����ݷ��͵������*/
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iTotalSend );
  	
  iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
  if (iRet != DATA_BUF_LEN)
  {
  	SET_ERRMSG( UPDATE_BAT_ERRMSG_9, UPDATE_BAT_ERRCODE_9 );
		return UPDATE_BAT_ERRCODE_9;
  }
  
	iRet = send( iSocket, pcSendBuf, iTotalSend, 0 );
	if (iRet != iTotalSend)
  {
  	SET_ERRMSG( UPDATE_BAT_ERRMSG_10, UPDATE_BAT_ERRCODE_10 );
		return UPDATE_BAT_ERRCODE_10;
  }
	
	free( pcSendBuf );
	
	iRet = keyValueWaitFd( iSocket, WAIT_IN, TIME_OUT );
	if (iRet == -1)
	{
		SET_ERRMSG( UPDATE_BAT_ERRMSG_11, UPDATE_BAT_ERRCODE_11 );
		return UPDATE_BAT_ERRCODE_11;
	}
	  
  /*��������*/
  iRecvLen = 0;
  memset( pcRecvLen, 0x00, sizeof(pcRecvLen) );
  
  while (1)
  {
	  iTmp = recv( iSocket, pcRecvLen + iRecvLen, DATA_BUF_LEN - iRecvLen, 0 );
	  if (iTmp < 0)
	  {
	  	break;
	  }
	  else
	  {
	  	iRecvLen += iTmp;
	  }
	  
	  if (iRecvLen == DATA_BUF_LEN)
	  {
	  	break;
	  }
	}
	if (iRecvLen != DATA_BUF_LEN)
	{
		SET_ERRMSG( UPDATE_BAT_ERRMSG_12, UPDATE_BAT_ERRCODE_12 );
		return UPDATE_BAT_ERRCODE_12;
	}
	
	iRecvSize = atoi( pcRecvLen );
	
	pcRecvBuf = keyValueMalloc( iRecvSize );
	if (pcRecvBuf == NULL)
	{
		SET_ERRMSG( UPDATE_BAT_ERRMSG_13, UPDATE_BAT_ERRCODE_13 );
		return UPDATE_BAT_ERRCODE_13;
	}
	
	iRecvLen = 0;
	while (1)
  {
	  iTmp = recv( iSocket, pcRecvBuf + iRecvLen, iRecvSize - iRecvLen, 0 );
	  
	  if (iTmp < 0)
	  {
	  	break;
	  }
	  else
	  {
	  	iRecvLen += iTmp;
	  }
	  
	  if (iRecvLen == iRecvSize)
	  {
	  	break;
	  }
	}
	
	if (iRecvLen != iRecvSize)
	{
		keyValueFree( pcRecvBuf );
		SET_ERRMSG( UPDATE_BAT_ERRMSG_14, UPDATE_BAT_ERRCODE_14 );
		return UPDATE_BAT_ERRCODE_14;
	}
	
	*pcaOutBuf = pcRecvBuf;
	*iaOutLen = iRecvLen;

  return SUCCESS;
}

/**********************************************************************
��������: keyValueDeleteBat 
��������: ����ɾ������
��    ����
��    һ���������ݿ�ʼ��ַ      I
��    �����������ݽ�����ַ      I
��    ����SOCKET                I
��    �ģ�������ݵ�ַ          O
��    �壺������ݴ�С          O
��    �أ�0�ǳɹ�������ʧ��
��    ��: �����
����ʱ��: 20161010
**********************************************************************/
int keyValueDeleteBat( char *pcaBufStart, char *pcaBufEnd, int iaSocket, char **pcaOutBuf, int *iaOutLen )
{
	char pcSendLen[DATA_BUF_LEN+1];
	char pcKey[MAX_KEY_LEN + 1];
	char pcRecvLen[DATA_BUF_LEN + 1];
	struct keyValueSendBuf *kvsb;
	struct keyValueSendBuf *kvsbTmp;
	struct keyValueSendBuf *kvsbNext;
	char *pcSemicolon;
	char *pcBlank;
	char *pcSendBuf;
	char *pcRecvBuf;
	int iTmp;
	int iKeyLen;
	int iSendLen;
	int iRecvLen;
	int iRecvSize;
	int iSocket;
	int iRet;
	int iTotalSend;
	int iOffset;
	char *pcBufStart;
	char *pcBufEnd;
	
	INIT_ERRMSG();
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	iSocket = iaSocket;
	
	/*���Կ�ʼ�ո�*/
	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
  if (pcBufStart == pcBufEnd)
  {
  	SET_ERRMSG( DELETE_BAT_ERRMSG_1, DELETE_BAT_ERRCODE_1 );
  	return DELETE_BAT_ERRCODE_1;
  }
  
  /*����ĩβ�ո�*/
  IGNORE_BLANK_END( pcBufStart, pcBufEnd );
	
	kvsb = malloc( sizeof(struct keyValueSendBuf) );
	if (kvsb == NULL)
	{
		SET_ERRMSG( DELETE_BAT_ERRMSG_2, DELETE_BAT_ERRCODE_2 );
		return DELETE_BAT_ERRCODE_2;
	}
	memset( kvsb, 0x00, sizeof(struct keyValueSendBuf) );
	
	strcpy( kvsb->pcSendBuf, DELETE_BAT_CNT );
	kvsb->iBufLen = OPERATE_LEN;
	
	/*���ͻ���Ҫ���͵�����ת��*/
	while (1)
  {
  	/*���Կ�ʼ�ո�*/
	  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
    if (pcBufStart == pcBufEnd)
    {
  	  break;
    }
      
  	/*���� KEY VALUE �ָ��*/
  	pcSemicolon = strchr( pcBufStart, ';' );
  	if (pcSemicolon == NULL || pcSemicolon > pcBufEnd )
  	{
  		pcSemicolon = pcBufEnd;
  	}
  	
  	/*����ĩβ�ո�*/
    IGNORE_BLANK_END( pcBufStart, pcSemicolon );
  	
  	/*ȡ KEY */
  	{
  	  iKeyLen = pcSemicolon - pcBufStart;
  	  if (iKeyLen > MAX_KEY_LEN)
  	  {
  	  	kvsbNext = kvsb;
  	  	while (kvsbNext != NULL)
  	    {
  	    	kvsbTmp = kvsbNext;
  	    	kvsbNext = kvsbNext->next;
  	    	free(kvsbTmp);
  	    }
  	    SET_ERRMSG( DELETE_BAT_ERRMSG_3, DELETE_BAT_ERRCODE_3 );
  	  	return DELETE_BAT_ERRCODE_3;
  	  }
  	  
  	  memset( pcKey, 0x00, sizeof(pcKey) );
  	  memcpy( pcKey, pcBufStart, iKeyLen );
  	  
  	  pcBufStart = pcSemicolon + 1;
    }
  	
  	/*����KEY*/
  	{
  	  kvsbNext = kvsb;
  	  while (kvsbNext->next != NULL)
  	  {
  		  kvsbNext = kvsbNext->next;
  	  }
  	  
  	  if (kvsbNext->iBufLen + KEY_VALUE_LENGTH + iKeyLen > MAX_SEND_LEN)
  	  {
  	  	kvsbNext->next = malloc( sizeof(struct keyValueSendBuf) );
  	  	if (kvsbNext->next == NULL)
  	  	{
  	  		kvsbNext = kvsb;
  	  	  while (kvsbNext != NULL)
  	      {
  	      	kvsbTmp = kvsbNext;
  	      	kvsbNext = kvsbNext->next;
  	      	free(kvsbTmp);
  	      }
  	      SET_ERRMSG( DELETE_BAT_ERRMSG_4, DELETE_BAT_ERRCODE_4 );
  	  	  return DELETE_BAT_ERRCODE_4;
  	  	}
  	  	
  	  	memset( kvsbNext->next, 0x00, sizeof(struct keyValueSendBuf) );
  	  	kvsbNext = kvsbNext->next;
  	  }
  	  
  	  sprintf( (kvsbNext->pcSendBuf) + (kvsbNext->iBufLen), "%0*d%s", KEY_VALUE_LENGTH, iKeyLen, pcKey );
  	  kvsbNext->iBufLen += KEY_VALUE_LENGTH + iKeyLen;
    }
  	
  	if (pcBufStart >= pcBufEnd)
  	{
  		break;
  	}
  }
  
  /*�����ܷ������ݴ�С*/
  kvsbNext = kvsb;
  iTotalSend = 0;
  while (kvsbNext != NULL)
  {
  	iTotalSend += kvsbNext->iBufLen;
  	kvsbNext = kvsbNext->next;
  }
  
  /*����ռ�*/
  pcSendBuf = malloc( iTotalSend );
  if (pcSendBuf == NULL)
  {
  	kvsbNext = kvsb;
  	while (kvsbNext != NULL)
  	{
  		kvsbTmp = kvsbNext;
  		kvsbNext = kvsbNext->next;
  		free(kvsbTmp);
  	}
  	SET_ERRMSG( DELETE_BAT_ERRMSG_5, DELETE_BAT_ERRCODE_5 );
	  return DELETE_BAT_ERRCODE_5;
  }
  memset( pcSendBuf, 0x00, iTotalSend );
  
  /*ȡ�����е�����*/
  kvsbNext = kvsb;
  iOffset = 0;
  while (kvsbNext != NULL)
  {
  	memcpy( pcSendBuf + iOffset, kvsbNext->pcSendBuf, kvsbNext->iBufLen );
  	iOffset += kvsbNext->iBufLen;
  	kvsbNext = kvsbNext->next;
  }
  
  /*�ͷŷ�����������*/
  kvsbNext = kvsb;
  while (kvsbNext != NULL)
  {
  	kvsbTmp = kvsbNext;
  	kvsbNext = kvsbNext->next;
  	free(kvsbTmp);
  }
  	
  /*�����ݷ��͵������*/
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iTotalSend );
  	
  iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
  if (iRet != DATA_BUF_LEN)
  {
  	SET_ERRMSG( DELETE_BAT_ERRMSG_6, DELETE_BAT_ERRCODE_6 );
  	free(pcSendBuf);
	  return DELETE_BAT_ERRCODE_6;
  }
  
	iRet = send( iSocket, pcSendBuf, iTotalSend, 0 );
	if (iRet != iTotalSend)
  {
  	SET_ERRMSG( DELETE_BAT_ERRMSG_7, DELETE_BAT_ERRCODE_7 );
  	free(pcSendBuf);
	  return DELETE_BAT_ERRCODE_7;
  }
	
	free( pcSendBuf );
	
	iRet = keyValueWaitFd( iSocket, WAIT_IN, TIME_OUT );
	if (iRet == -1)
	{
		SET_ERRMSG( DELETE_BAT_ERRMSG_8, DELETE_BAT_ERRCODE_8 );
		return DELETE_BAT_ERRCODE_8;
	}
	  
  /*��������*/
  iRecvLen = 0;
  memset( pcRecvLen, 0x00, sizeof(pcRecvLen) );
  
  while (1)
  {
	  iTmp = recv( iSocket, pcRecvLen + iRecvLen, DATA_BUF_LEN - iRecvLen, 0 );
	  if (iTmp < 0)
	  {
	  	break;
	  }
	  else
	  {
	  	iRecvLen += iTmp;
	  }
	  
	  if (iRecvLen == DATA_BUF_LEN)
	  {
	  	break;
	  }
	}
	if (iRecvLen != DATA_BUF_LEN)
	{
		SET_ERRMSG( DELETE_BAT_ERRMSG_9, DELETE_BAT_ERRCODE_9 );
		return DELETE_BAT_ERRCODE_9;
	}
	
	iRecvSize = atoi( pcRecvLen );
	
	pcRecvBuf = keyValueMalloc( iRecvSize );
	if (pcRecvBuf == NULL)
	{
		SET_ERRMSG( DELETE_BAT_ERRMSG_10, DELETE_BAT_ERRCODE_10 );
		return DELETE_BAT_ERRCODE_10;
	}
	
	iRecvLen = 0;
	while (1)
  {
	  iTmp = recv( iSocket, pcRecvBuf + iRecvLen, iRecvSize - iRecvLen, 0 );
	  
	  if (iTmp < 0)
	  {
	  	break;
	  }
	  else
	  {
	  	iRecvLen += iTmp;
	  }
	  
	  if (iRecvLen == iRecvSize)
	  {
	  	break;
	  }
	}
	
	if (iRecvLen != iRecvSize)
	{
		keyValueFree( pcRecvBuf );
		SET_ERRMSG( DELETE_BAT_ERRMSG_11, DELETE_BAT_ERRCODE_11 );
		return DELETE_BAT_ERRCODE_11;
	}
	
	*pcaOutBuf = pcRecvBuf;
	*iaOutLen = iRecvLen;

  return 0;
}

/**********************************************************************
��������: keyValueSelectBat 
��������: ������ѯ����
��    ����
��    һ���������ݿ�ʼ��ַ      I
��    �����������ݽ�����ַ      I
��    ����SOCKET                I
��    �ģ�������ݵ�ַ          O
��    �壺������ݴ�С          O
��    �أ�0�ǳɹ�������ʧ��
��    ��: �����
����ʱ��: 20161010
**********************************************************************/
int keyValueSelectBat( char *pcaBufStart, char *pcaBufEnd, int iaSocket, char **pcaOutBuf, int *iaOutLen )
{
	char pcSendLen[DATA_BUF_LEN+1];
	char pcKey[MAX_KEY_LEN + 1];
	char pcRecvLen[DATA_BUF_LEN + 1];
	struct keyValueSendBuf *kvsb;
	struct keyValueSendBuf *kvsbTmp;
	struct keyValueSendBuf *kvsbNext;
	char *pcSemicolon;
	char *pcBlank;
	char *pcSendBuf;
	char *pcRecvBuf;
	int iTmp;
	int iKeyLen;
	int iSendLen;
	int iRecvLen;
	int iRecvSize;
	int iSocket;
	int iRet;
	int iTotalSend;
	int iOffset;
	char *pcBufStart;
	char *pcBufEnd;
	
	INIT_ERRMSG();
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	iSocket = iaSocket;
	
	/*���Կ�ʼ�ո�*/
	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
  if (pcBufStart == pcBufEnd)
  {
  	SET_ERRMSG( SELECT_BAT_ERRMSG_1, SELECT_BAT_ERRCODE_1 );
  	return SELECT_BAT_ERRCODE_1;
  }
  
  /*����ĩβ�ո�*/
  IGNORE_BLANK_END( pcBufStart, pcBufEnd );
	
	kvsb = malloc( sizeof(struct keyValueSendBuf) );
	if (kvsb == NULL)
	{
		SET_ERRMSG( SELECT_BAT_ERRMSG_2, SELECT_BAT_ERRCODE_2 );
		return SELECT_BAT_ERRCODE_2;
	}
	memset( kvsb, 0x00, sizeof(struct keyValueSendBuf) );
	
	strcpy( kvsb->pcSendBuf, SELECT_BAT_CNT );
	kvsb->iBufLen = OPERATE_LEN;
	
	/*���ͻ���Ҫ���͵�����ת��*/
	while (1)
  {
  	/*���Կ�ʼ�ո�*/
	  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
    if (pcBufStart == pcBufEnd)
    {
  	  break;
    }
      
  	/*���� KEY VALUE �ָ��*/
  	pcSemicolon = strchr( pcBufStart, ';' );
  	if (pcSemicolon == NULL || pcSemicolon > pcBufEnd )
  	{
  		pcSemicolon = pcBufEnd;
  	}
  	
  	/*����ĩβ�ո�*/
    IGNORE_BLANK_END( pcBufStart, pcSemicolon );
  	
  	/*ȡ KEY */
  	{
  	  iKeyLen = pcSemicolon - pcBufStart;
  	  if (iKeyLen > MAX_KEY_LEN)
  	  {
  	  	kvsbNext = kvsb;
  	  	while (kvsbNext != NULL)
  	    {
  	    	kvsbTmp = kvsbNext;
  	    	kvsbNext = kvsbNext->next;
  	    	free(kvsbTmp);
  	    }
  	    SET_ERRMSG( SELECT_BAT_ERRMSG_3, SELECT_BAT_ERRCODE_3 );
  	  	return SELECT_BAT_ERRCODE_3;
  	  }
  	  
  	  memset( pcKey, 0x00, sizeof(pcKey) );
  	  memcpy( pcKey, pcBufStart, iKeyLen );
  	  
  	  pcBufStart = pcSemicolon + 1;
    }
  	
  	/*����KEY*/
  	{
  	  kvsbNext = kvsb;
  	  while (kvsbNext->next != NULL)
  	  {
  		  kvsbNext = kvsbNext->next;
  	  }
  	  
  	  if (kvsbNext->iBufLen + KEY_VALUE_LENGTH + iKeyLen > MAX_SEND_LEN)
  	  {
  	  	kvsbNext->next = malloc( sizeof(struct keyValueSendBuf) );
  	  	if (kvsbNext->next == NULL)
  	  	{
  	  		kvsbNext = kvsb;
  	  	  while (kvsbNext != NULL)
  	      {
  	      	kvsbTmp = kvsbNext;
  	      	kvsbNext = kvsbNext->next;
  	      	free(kvsbTmp);
  	      }
  	      SET_ERRMSG( SELECT_BAT_ERRMSG_4, SELECT_BAT_ERRCODE_4 );
  	  	  return SELECT_BAT_ERRCODE_4;
  	  	}
  	  	
  	  	memset( kvsbNext->next, 0x00, sizeof(struct keyValueSendBuf) );
  	  	kvsbNext = kvsbNext->next;
  	  }
  	  
  	  sprintf( (kvsbNext->pcSendBuf) + (kvsbNext->iBufLen), "%0*d%s", KEY_VALUE_LENGTH, iKeyLen, pcKey );
  	  kvsbNext->iBufLen += KEY_VALUE_LENGTH + iKeyLen;
    }
  	
  	if (pcBufStart >= pcBufEnd)
  	{
  		break;
  	}
  }
  
  /*�����ܷ������ݴ�С*/
  kvsbNext = kvsb;
  iTotalSend = 0;
  while (kvsbNext != NULL)
  {
  	iTotalSend += kvsbNext->iBufLen;
  	kvsbNext = kvsbNext->next;
  }
  
  /*����ռ�*/
  pcSendBuf = malloc( iTotalSend );
  if (pcSendBuf == NULL)
  {
  	kvsbNext = kvsb;
  	while (kvsbNext != NULL)
  	{
  		kvsbTmp = kvsbNext;
  		kvsbNext = kvsbNext->next;
  		free(kvsbTmp);
  	}
  	SET_ERRMSG( SELECT_BAT_ERRMSG_5, SELECT_BAT_ERRCODE_5 );
	  return SELECT_BAT_ERRCODE_5;
  }
  memset( pcSendBuf, 0x00, iTotalSend );
  
  /*ȡ�����е�����*/
  kvsbNext = kvsb;
  iOffset = 0;
  while (kvsbNext != NULL)
  {
  	memcpy( pcSendBuf + iOffset, kvsbNext->pcSendBuf, kvsbNext->iBufLen );
  	iOffset += kvsbNext->iBufLen;
  	kvsbNext = kvsbNext->next;
  }
  
  /*�ͷŷ�����������*/
  kvsbNext = kvsb;
  while (kvsbNext != NULL)
  {
  	kvsbTmp = kvsbNext;
  	kvsbNext = kvsbNext->next;
  	free(kvsbTmp);
  }
  	
  /*�����ݷ��͵������*/
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iTotalSend );
  	
  iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
  if (iRet != DATA_BUF_LEN)
  {
  	SET_ERRMSG( SELECT_BAT_ERRMSG_6, SELECT_BAT_ERRCODE_6 );
  	free( pcSendBuf );
	  return SELECT_BAT_ERRCODE_6;
  }
  
	iRet = send( iSocket, pcSendBuf, iTotalSend, 0 );
	if (iRet != iTotalSend)
  {
  	SET_ERRMSG( SELECT_BAT_ERRMSG_7, SELECT_BAT_ERRCODE_7 );
  	free( pcSendBuf );
	  return SELECT_BAT_ERRCODE_7;
  }
	
	free( pcSendBuf );
	
	iRet = keyValueWaitFd( iSocket, WAIT_IN, TIME_OUT );
	if (iRet == -1)
	{
		SET_ERRMSG( SELECT_BAT_ERRMSG_8, SELECT_BAT_ERRCODE_8 );
		return SELECT_BAT_ERRCODE_8;
	}
	  
  /*��������*/
  iRecvLen = 0;
  memset( pcRecvLen, 0x00, sizeof(pcRecvLen) );
  
  while (1)
  {
	  iTmp = recv( iSocket, pcRecvLen + iRecvLen, DATA_BUF_LEN - iRecvLen, 0 );
	  if (iTmp < 0)
	  {
	  	break;
	  }
	  else
	  {
	  	iRecvLen += iTmp;
	  }
	  
	  if (iRecvLen == DATA_BUF_LEN)
	  {
	  	break;
	  }
	}
	
	if (iRecvLen != DATA_BUF_LEN)
	{
		SET_ERRMSG( SELECT_BAT_ERRMSG_9, SELECT_BAT_ERRCODE_9 );
		return SELECT_BAT_ERRCODE_9;
	}
	
	iRecvSize = atoi( pcRecvLen );
	
	pcRecvBuf = keyValueMalloc( iRecvSize );
	if (pcRecvBuf == NULL)
	{
		SET_ERRMSG( SELECT_BAT_ERRMSG_10, SELECT_BAT_ERRCODE_10 );
		return SELECT_BAT_ERRCODE_10;
	}
	
	iRecvLen = 0;
	while (1)
  {
	  iTmp = recv( iSocket, pcRecvBuf + iRecvLen, iRecvSize - iRecvLen, 0 );
	  
	  if (iTmp < 0)
	  {
	  	break;
	  }
	  else
	  {
	  	iRecvLen += iTmp;
	  }
	  
	  if (iRecvLen == iRecvSize)
	  {
	  	break;
	  }
	}
	
	if (iRecvLen != iRecvSize)
	{
		keyValueFree(pcRecvBuf);
		SET_ERRMSG( SELECT_BAT_ERRMSG_11, SELECT_BAT_ERRCODE_11 );
		return SELECT_BAT_ERRCODE_11;
	}
	
	*pcaOutBuf = pcRecvBuf;
	*iaOutLen = iRecvLen;

  return 0;
}

/**********************************************************************
��������: keyValueMixBat 
��������: ������ϲ���
��    ����
��    һ���������ݿ�ʼ��ַ      I
��    �����������ݽ�����ַ      I
��    ����SOCKET                I
��    �ģ�������ݵ�ַ          O
��    �壺������ݴ�С          O
��    �أ�0�ǳɹ�������ʧ��
��    ��: �����
����ʱ��: 20161010
**********************************************************************/
int keyValueMixBat( char *pcaBufStart, char *pcaBufEnd, int iaSocket, char **pcaOutBuf, int *iaOutLen )
{
	char pcSendLen[DATA_BUF_LEN+1];
	char pcKey[MAX_KEY_LEN + 1];
	char pcValue[MAX_VALUE_LEN + 1];
	char pcOperation[MAX_OPERATEION_LEN+1];
	char pcRecvLen[DATA_BUF_LEN + 1];
	struct keyValueSendBuf *kvsb;
	struct keyValueSendBuf *kvsbTmp;
	struct keyValueSendBuf *kvsbNext;
	char *pcSemicolon;
	char *pcBlank;
	char *pcSendBuf;
	char *pcRecvBuf;
	int iTmp;
	int iKeyLen;
	int iValueLen;
	int iSendLen;
	int iRecvLen;
	int iRecvSize;
	int iSocket;
	int iRet;
	int iTotalSend;
	int iOffset;
	char *pcBufStart;
	char *pcBufEnd;
	
	INIT_ERRMSG();
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	iSocket = iaSocket;
	
	/*���Կ�ʼ�ո�*/
	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
  if (pcBufStart == pcBufEnd)
  {
  	SET_ERRMSG( MIX_BAT_ERRMSG_1, MIX_BAT_ERRCODE_1 );
  	return MIX_BAT_ERRCODE_1;
  }
  
  /*����ĩβ�ո�*/
  IGNORE_BLANK_END( pcBufStart, pcBufEnd );
	
	kvsb = malloc( sizeof(struct keyValueSendBuf) );
	if (kvsb == NULL)
	{
		SET_ERRMSG( MIX_BAT_ERRMSG_2, MIX_BAT_ERRCODE_2 );
		return MIX_BAT_ERRCODE_2;
	}
	memset( kvsb, 0x00, sizeof(struct keyValueSendBuf) );
	
	strcpy( kvsb->pcSendBuf, UP_DE_SE_BAT_CNT );
	kvsb->iBufLen = OPERATE_LEN;
	
	/*���ͻ���Ҫ���͵�����ת��*/
	while (1)
  {
  	/*���Կ�ʼ�ո�*/
	  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
    if (pcBufStart == pcBufEnd)
    {
  	  break;
    }
      
  	/*���� KEY VALUE �ָ��*/
  	pcSemicolon = strchr( pcBufStart, ';' );
  	if (pcSemicolon == NULL || pcSemicolon > pcBufEnd )
  	{
  		pcSemicolon = pcBufEnd;
  	}
  	
  	/*����ĩβ�ո�*/
    IGNORE_BLANK_END( pcBufStart, pcSemicolon );
    
    /*ȡ������*/
    {
    	pcBlank = strchr( pcBufStart, ' ' );
  	  if (pcBlank == NULL || pcBlank > pcSemicolon)
  	  {
  	  	kvsbNext = kvsb;
  	  	while (kvsbNext != NULL)
  	    {
  	    	kvsbTmp = kvsbNext;
  	    	kvsbNext = kvsbNext->next;
  	    	free(kvsbTmp);
  	    }
  	    SET_ERRMSG( MIX_BAT_ERRMSG_3, MIX_BAT_ERRCODE_3 );
  	  	return MIX_BAT_ERRCODE_3;
  	  }
  	  
  	  iKeyLen = pcBlank - pcBufStart;
  	  if (iKeyLen > MAX_OPERATEION_LEN)
  	  {
  	  	kvsbNext = kvsb;
  	  	while (kvsbNext != NULL)
  	    {
  	    	kvsbTmp = kvsbNext;
  	    	kvsbNext = kvsbNext->next;
  	    	free(kvsbTmp);
  	    }
  	    SET_ERRMSG( MIX_BAT_ERRMSG_4, MIX_BAT_ERRCODE_4 );
  	  	return MIX_BAT_ERRCODE_4;
  	  }
  	  
  	  memset( pcOperation, 0x00, sizeof(pcOperation) );
  	  memcpy( pcOperation, pcBufStart, iKeyLen );
  	  
  	  pcBufStart = pcBlank + 1;
    }
    
    if (!keyValueStrcmpIgnore( pcOperation, SET_SINGLE ))
    {
    	/*ȡ KEY */
  	  {
  	    pcBlank = strchr( pcBufStart, ' ' );
  	    if (pcBlank == NULL || pcBlank > pcSemicolon)
  	    {
  	    	kvsbNext = kvsb;
  	    	while (kvsbNext != NULL)
  	      {
  	      	kvsbTmp = kvsbNext;
  	      	kvsbNext = kvsbNext->next;
  	      	free(kvsbTmp);
  	      }
  	      SET_ERRMSG( MIX_BAT_ERRMSG_5, MIX_BAT_ERRCODE_5 );
  	    	return MIX_BAT_ERRCODE_5;
  	    }
  	    
  	    iKeyLen = pcBlank - pcBufStart;
  	    if (iKeyLen > MAX_KEY_LEN)
  	    {
  	    	kvsbNext = kvsb;
  	    	while (kvsbNext != NULL)
  	      {
  	      	kvsbTmp = kvsbNext;
  	      	kvsbNext = kvsbNext->next;
  	      	free(kvsbTmp);
  	      }
  	      SET_ERRMSG( MIX_BAT_ERRMSG_6, MIX_BAT_ERRCODE_6 );
  	    	return MIX_BAT_ERRCODE_6;
  	    }
  	    
  	    memset( pcKey, 0x00, sizeof(pcKey) );
  	    memcpy( pcKey, pcBufStart, iKeyLen );
  	    
  	    pcBufStart = pcBlank + 1;
      }
      
      /*ȡ VALLUE */
  	  {
  	  	/*���Կ�ʼ�ո�*/
	      IGNORE_BLANK_START( pcBufStart, pcSemicolon );
        if (pcBufStart == pcSemicolon)
        {
        	kvsbNext = kvsb;
  	    	while (kvsbNext != NULL)
  	      {
  	      	kvsbTmp = kvsbNext;
  	      	kvsbNext = kvsbNext->next;
  	      	free(kvsbTmp);
  	      }
  	      SET_ERRMSG( MIX_BAT_ERRMSG_7, MIX_BAT_ERRCODE_7 );
  	      return MIX_BAT_ERRCODE_7;
        }
        
        iValueLen = pcSemicolon - pcBufStart;
        if (iValueLen > MAX_VALUE_LEN)
        {
        	kvsbNext = kvsb;
  	    	while (kvsbNext != NULL)
  	      {
  	      	kvsbTmp = kvsbNext;
  	      	kvsbNext = kvsbNext->next;
  	      	free(kvsbTmp);
  	      }
  	      SET_ERRMSG( MIX_BAT_ERRMSG_8, MIX_BAT_ERRCODE_8 );
  	    	return MIX_BAT_ERRCODE_8;
        }
        memset( pcValue, 0x00, sizeof(pcValue) );
  	    memcpy( pcValue, pcBufStart, iValueLen );
  	    
  	    pcBufStart = pcSemicolon + 1;
  	  }
  	  
  	  /*����KEY VALUE*/
  	  {
  	    kvsbNext = kvsb;
  	    while (kvsbNext->next != NULL)
  	    {
  	  	  kvsbNext = kvsbNext->next;
  	    }
  	    
  	    if (kvsbNext->iBufLen + OPERATE_LEN + KEY_VALUE_LENGTH + iKeyLen + KEY_VALUE_LENGTH + iValueLen > MAX_SEND_LEN)
  	    {
  	    	kvsbNext->next = malloc( sizeof(struct keyValueSendBuf) );
  	    	if (kvsbNext->next == NULL)
  	    	{
  	    		kvsbNext = kvsb;
  	    	  while (kvsbNext != NULL)
  	        {
  	        	kvsbTmp = kvsbNext;
  	        	kvsbNext = kvsbNext->next;
  	        	free(kvsbTmp);
  	        }
  	        SET_ERRMSG( MIX_BAT_ERRMSG_9, MIX_BAT_ERRCODE_9 );
  	    	  return MIX_BAT_ERRCODE_9;
  	    	}
  	    	
  	    	memset( kvsbNext->next, 0x00, sizeof(struct keyValueSendBuf) );
  	    	kvsbNext = kvsbNext->next;
  	    }
  	    
  	    sprintf( (kvsbNext->pcSendBuf) + (kvsbNext->iBufLen), "%s%0*d%s%0*d%s", SELECT_SINGLE_CNT, KEY_VALUE_LENGTH, iKeyLen, pcKey, KEY_VALUE_LENGTH, iValueLen, pcValue );
  	    kvsbNext->iBufLen += KEY_VALUE_LENGTH + iKeyLen + KEY_VALUE_LENGTH + iValueLen;
      }
  	  
  	  if (pcBufStart >= pcBufEnd)
  	  {
  	  	break;
  	  }
    }
    else
  	if (!keyValueStrcmpIgnore( pcOperation, DEL_SINGLE ))
    {
    	/*ȡ KEY */
  	  {
  	  	pcBlank = strchr( pcBufStart, ' ' );
  	    if (pcBlank == NULL || pcBlank > pcSemicolon)
  	    {
  	    	kvsbNext = kvsb;
  	    	while (kvsbNext != NULL)
  	      {
  	      	kvsbTmp = kvsbNext;
  	      	kvsbNext = kvsbNext->next;
  	      	free(kvsbTmp);
  	      }
  	      SET_ERRMSG( MIX_BAT_ERRMSG_10, MIX_BAT_ERRCODE_10 );
  	    	return MIX_BAT_ERRCODE_10;
  	    }
  	    
  	    iKeyLen = pcSemicolon - pcBufStart;
  	    if (iKeyLen > MAX_KEY_LEN)
  	    {
  	    	kvsbNext = kvsb;
  	    	while (kvsbNext != NULL)
  	      {
  	      	kvsbTmp = kvsbNext;
  	      	kvsbNext = kvsbNext->next;
  	      	free(kvsbTmp);
  	      }
  	      SET_ERRMSG( MIX_BAT_ERRMSG_11, MIX_BAT_ERRCODE_11 );
  	    	return MIX_BAT_ERRCODE_11;
  	    }
  	    
  	    memset( pcKey, 0x00, sizeof(pcKey) );
  	    memcpy( pcKey, pcBufStart, iKeyLen );
  	    
  	    pcBufStart = pcSemicolon + 1;
      }
  	  
  	  /*����KEY*/
  	  {
  	    kvsbNext = kvsb;
  	    while (kvsbNext->next != NULL)
  	    {
  	  	  kvsbNext = kvsbNext->next;
  	    }
  	    
  	    if (kvsbNext->iBufLen + OPERATE_LEN + KEY_VALUE_LENGTH + iKeyLen > MAX_SEND_LEN)
  	    {
  	    	kvsbNext->next = malloc( sizeof(struct keyValueSendBuf) );
  	    	if (kvsbNext->next == NULL)
  	    	{
  	    		kvsbNext = kvsb;
  	    	  while (kvsbNext != NULL)
  	        {
  	        	kvsbTmp = kvsbNext;
  	        	kvsbNext = kvsbNext->next;
  	        	free(kvsbTmp);
  	        }
  	        SET_ERRMSG( MIX_BAT_ERRMSG_12, MIX_BAT_ERRCODE_12 );
  	    	  return MIX_BAT_ERRCODE_12;
  	    	}
  	    	
  	    	memset( kvsbNext->next, 0x00, sizeof(struct keyValueSendBuf) );
  	    	kvsbNext = kvsbNext->next;
  	    }
  	    
  	    sprintf( (kvsbNext->pcSendBuf) + (kvsbNext->iBufLen), "%s%0*d%s", DELETE_SINGLE_CNT, KEY_VALUE_LENGTH, iKeyLen, pcKey );
  	    kvsbNext->iBufLen += KEY_VALUE_LENGTH + iKeyLen;
      }
  	  
  	  if (pcBufStart >= pcBufEnd)
  	  {
  	  	break;
  	  }
    }
    else
  	if (!keyValueStrcmpIgnore( pcOperation, GET_SINGLE ))
    {
    	/*ȡ KEY */
  	  {
  	  	pcBlank = strchr( pcBufStart, ' ' );
  	    if (pcBlank == NULL || pcBlank > pcSemicolon)
  	    {
  	    	kvsbNext = kvsb;
  	    	while (kvsbNext != NULL)
  	      {
  	      	kvsbTmp = kvsbNext;
  	      	kvsbNext = kvsbNext->next;
  	      	free(kvsbTmp);
  	      }
  	      SET_ERRMSG( MIX_BAT_ERRMSG_13, MIX_BAT_ERRCODE_13 );
  	    	return MIX_BAT_ERRCODE_13;
  	    }
  	    
  	    iKeyLen = pcSemicolon - pcBufStart;
  	    if (iKeyLen > MAX_KEY_LEN)
  	    {
  	    	kvsbNext = kvsb;
  	    	while (kvsbNext != NULL)
  	      {
  	      	kvsbTmp = kvsbNext;
  	      	kvsbNext = kvsbNext->next;
  	      	free(kvsbTmp);
  	      }
  	      SET_ERRMSG( MIX_BAT_ERRMSG_14, MIX_BAT_ERRCODE_14 );
  	    	return MIX_BAT_ERRCODE_14;
  	    }
  	    
  	    memset( pcKey, 0x00, sizeof(pcKey) );
  	    memcpy( pcKey, pcBufStart, iKeyLen );
  	    
  	    pcBufStart = pcSemicolon + 1;
      }
  	  
  	  /*����KEY*/
  	  {
  	    kvsbNext = kvsb;
  	    while (kvsbNext->next != NULL)
  	    {
  	  	  kvsbNext = kvsbNext->next;
  	    }
  	    
  	    if (kvsbNext->iBufLen + KEY_VALUE_LENGTH + iKeyLen > MAX_SEND_LEN)
  	    {
  	    	kvsbNext->next = malloc( sizeof(struct keyValueSendBuf) );
  	    	if (kvsbNext->next == NULL)
  	    	{
  	    		kvsbNext = kvsb;
  	    	  while (kvsbNext != NULL)
  	        {
  	        	kvsbTmp = kvsbNext;
  	        	kvsbNext = kvsbNext->next;
  	        	free(kvsbTmp);
  	        }
  	        SET_ERRMSG( MIX_BAT_ERRMSG_15, MIX_BAT_ERRCODE_15 );
  	    	  return MIX_BAT_ERRCODE_15;
  	    	}
  	    	
  	    	memset( kvsbNext->next, 0x00, sizeof(struct keyValueSendBuf) );
  	    	kvsbNext = kvsbNext->next;
  	    }
  	    
  	    sprintf( (kvsbNext->pcSendBuf) + (kvsbNext->iBufLen), "%s%0*d%s", SELECT_SINGLE_CNT, KEY_VALUE_LENGTH, iKeyLen, pcKey );
  	    kvsbNext->iBufLen += KEY_VALUE_LENGTH + iKeyLen;
      }
  	  
  	  if (pcBufStart >= pcBufEnd)
  	  {
  	  	break;
  	  }
    }
    else
    {
    	kvsbNext = kvsb;
  	  while (kvsbNext != NULL)
  	  {
  	  	kvsbTmp = kvsbNext;
  	  	kvsbNext = kvsbNext->next;
  	  	free(kvsbTmp);
  	  }
  	  SET_ERRMSG( MIX_BAT_ERRMSG_16, MIX_BAT_ERRCODE_16 );
  	  return MIX_BAT_ERRCODE_16;
    }
  }
  
  /*�����ܷ������ݴ�С*/
  kvsbNext = kvsb;
  iTotalSend = 0;
  while (kvsbNext != NULL)
  {
  	iTotalSend += kvsbNext->iBufLen;
  	kvsbNext = kvsbNext->next;
  }
  
  /*����ռ�*/
  pcSendBuf = malloc( iTotalSend );
  if (pcSendBuf == NULL)
  {
  	kvsbNext = kvsb;
  	while (kvsbNext != NULL)
  	{
  		kvsbTmp = kvsbNext;
  		kvsbNext = kvsbNext->next;
  		free(kvsbTmp);
  	}
  	SET_ERRMSG( MIX_BAT_ERRMSG_17, MIX_BAT_ERRCODE_17 );
	  return MIX_BAT_ERRCODE_17;
  }
  memset( pcSendBuf, 0x00, iTotalSend );
  
  /*ȡ�����е�����*/
  kvsbNext = kvsb;
  iOffset = 0;
  while (kvsbNext != NULL)
  {
  	memcpy( pcSendBuf + iOffset, kvsbNext->pcSendBuf, kvsbNext->iBufLen );
  	iOffset += kvsbNext->iBufLen;
  	kvsbNext = kvsbNext->next;
  }
  
  /*�ͷŷ�����������*/
  kvsbNext = kvsb;
  while (kvsbNext != NULL)
  {
  	kvsbTmp = kvsbNext;
  	kvsbNext = kvsbNext->next;
  	free(kvsbTmp);
  }
  	
  /*�����ݷ��͵������*/
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iTotalSend );
  	
  iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
  if (iRet != DATA_BUF_LEN)
  {
  	SET_ERRMSG( MIX_BAT_ERRMSG_18, MIX_BAT_ERRCODE_18 );
  	free( pcSendBuf );
	  return MIX_BAT_ERRCODE_18;
  }
  
	iRet = send( iSocket, pcSendBuf, iTotalSend, 0 );
	if (iRet != iTotalSend)
  {
  	SET_ERRMSG( MIX_BAT_ERRMSG_19, MIX_BAT_ERRCODE_19 );
  	free( pcSendBuf );
	  return MIX_BAT_ERRCODE_19;
  }
	
	free( pcSendBuf );
	
	iRet = keyValueWaitFd( iSocket, WAIT_IN, TIME_OUT );
	if (iRet == -1)
	{
		SET_ERRMSG( MIX_BAT_ERRMSG_20, MIX_BAT_ERRCODE_20 );
		return MIX_BAT_ERRCODE_20;
	}
	  
  /*��������*/
  iRecvLen = 0;
  memset( pcRecvLen, 0x00, sizeof(pcRecvLen) );
  
  while (1)
  {
	  iTmp = recv( iSocket, pcRecvLen + iRecvLen, DATA_BUF_LEN - iRecvLen, 0 );
	  if (iTmp < 0)
	  {
	  	break;
	  }
	  else
	  {
	  	iRecvLen += iTmp;
	  }
	  
	  if (iRecvLen == DATA_BUF_LEN)
	  {
	  	break;
	  }
	}
	
	if (iRecvLen != DATA_BUF_LEN)
	{
		SET_ERRMSG( MIX_BAT_ERRMSG_21, MIX_BAT_ERRCODE_21 );
		return MIX_BAT_ERRCODE_21;
	}
	
	iRecvSize = atoi( pcRecvLen );
	
	pcRecvBuf = keyValueMalloc( iRecvSize );
	if (pcRecvBuf == NULL)
	{
		SET_ERRMSG( MIX_BAT_ERRMSG_22, MIX_BAT_ERRCODE_22 );
		return MIX_BAT_ERRCODE_22;
	}
	
	iRecvLen = 0;
	while (1)
  {
	  iTmp = recv( iSocket, pcRecvBuf + iRecvLen, iRecvSize - iRecvLen, 0 );
	  
	  if (iTmp < 0)
	  {
	  	break;
	  }
	  else
	  {
	  	iRecvLen += iTmp;
	  }
	  
	  if (iRecvLen == iRecvSize)
	  {
	  	break;
	  }
	}
	
	if (iRecvLen != iRecvSize)
  {
  	keyValueFree(pcRecvBuf);
  	SET_ERRMSG( MIX_BAT_ERRMSG_23, MIX_BAT_ERRCODE_23 );
	  return MIX_BAT_ERRCODE_23;
  }
	
	*pcaOutBuf = pcRecvBuf;
	*iaOutLen = iRecvLen;

  return 0;
}

/**********************************************************************
��������: keyValueConnectToServer 
��������: ����SOCKET
��    ����
��    ��: �����
����ʱ��: 20160818
**********************************************************************/
int keyValueConnectToServer( char *pcaIp, char *pcaPort )
{
  struct sockaddr_in servAddr; /*��������ַ��Ϣ�ṹ��*/
  struct sockaddr_in addr;     /*���յ��Ŀͻ�����Ϣ�ṹ��*/
  int    iSocket;
  
  #ifdef SYSTEM_WINDOWS_64
    /*����socket*/
    iSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP); 
    if (iSocket == INVALID_SOCKET)
    {
  	  return -1;
    }
    
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(atoi(pcaPort));
    servAddr.sin_addr.s_addr = inet_addr(pcaIp);
    
    /*����������FTP����*/
    if (connect(iSocket, (struct sockaddr *)(&servAddr), sizeof(struct sockaddr)) < 0)
    {
      return -1;
    }
    
  #else
    /*����socket*/
    iSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (iSocket == -1)
    {
  	  return -1;
    }

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(atoi(pcaPort));
    servAddr.sin_addr.s_addr = inet_addr(pcaIp);
  
    /*����������FTP����*/
    if (connect(iSocket, (struct sockaddr *)(&servAddr), sizeof(struct sockaddr)) < 0)
    {
      return -1;
    }
  
  #endif
  
  return iSocket;
}

/**********************************************************************
��������: keyValueWaitFd 
��������: �ȴ�SOCKET
��    ����
��    һ��SOCKET      I
��    ��������        I
��    ������ʱʱ��    I
��    �أ�-1ʧ�ܣ������ɹ�
��    ��: �����
����ʱ��: 20161010
**********************************************************************/
int keyValueWaitFd( int iFd, int iFlag, int iTimeOut )
{
	int iRc;
  fd_set fds;
  struct timeval sTimeval;
  fd_set *fds_read = NULL;
  fd_set *fds_write = NULL;
  struct timeval *tv = NULL;
  
  memset( &sTimeval, 0x00, sizeof(sTimeval) );
  sTimeval.tv_sec = iTimeOut;
  
  FD_ZERO( &fds );
  FD_SET( iFd, &fds );
  
  if (iFlag == WAIT_OUT)
  {
  	fds_write = &fds;
  }
  else
  {
  	fds_read = &fds;
  }
  
  if (iTimeOut >= 0)
  {
  	tv = &sTimeval;
  }
  else
  {
  	tv = NULL;
  }
  
  return select( iFd + 1, fds_read, fds_write, NULL, tv );
}


/********************************************************
�������ƣ�keyValueAllTrim
�������ܣ�ȥ��ͷβ�ո� 
��ڲ�����
��    һ���ַ���     I
��    �أ�1��ʾ���ַ�Ϊ�ո�0��ʾ����
����ʱ�䣺20130226
*********************************************************/
char *keyValueAllTrim( char *pcaStr )
{
	char *pcStart;
	char *pcEnd;
	char pcBuf[8192];
	
	pcStart = pcaStr;
	pcEnd = pcStart + strlen(pcStart);
	
	/*���Կ�ͷ���ֵĿո�*/
	while (1)
	{
		/*���ȫ���ǿո�*/
		if (pcStart == pcEnd)
		{
			return pcaStr;
		}
		
		/*����ǿո�*/
		if (*pcStart == ' ' || *pcStart == '\t' || *pcStart == '\r' || *pcStart == '\n' /*|| *pcStart == '\0'*/)
		{
			*pcStart = 0x00;
			pcStart++;
		}
		else
		{
			break;
		}
	}

	/*����ĩβ���ֵĿո�*/
	while (1)
	{
		/*���ȫ���ǿո�*/
		if (pcEnd == pcStart)
		{
			return pcaStr;
		}
		/*����ǿո�*/
		if (*pcEnd == ' ' || *pcEnd == '\t' || *pcEnd == '\r' || *pcEnd == '\n' /*|| *pcEnd == '\0'*/)
		{
			*pcEnd = 0x00;
			pcEnd--;
		}
		else
		{
			break;
		}
	}
	memset( pcBuf, 0x00, sizeof(pcBuf) );
	memcpy( pcBuf, pcStart, pcEnd - pcStart + 1 );
	strcpy( pcaStr, pcBuf );
	return pcaStr;
}

/********************************************************
������  ��keyValueCheckPort
�������ܣ����˿��Ƿ���ȷ
��ڲ�����
��    һ���˿�                I
��    �أ�1��ʾ���ϣ�0��ʾ������
����ʱ�䣺20151102
*********************************************************/
int keyValueCheckPort( char *pcaPort )
{
	if (keyValueIsDigitals( pcaPort, pcaPort + strlen(pcaPort)) == 0 || (atoi(pcaPort) < 0 || atoi(pcaPort) > 65535))
  {
  	return 0;
  }
  else
  {
  	return 1;
  }
}

/********************************************************
������  ��keyValueCheckIP
�������ܣ����IP�Ƿ���ȷ
��ڲ�����
��    һ��IP                I
��    �أ�1��ʾ���ϣ�0��ʾ������
����ʱ�䣺20131024
*********************************************************/
int keyValueCheckIP( char *pcaIP )
{
  char pcValue[20];
  char *pcBuf;
  char *pcTmp;
  
  pcBuf = pcaIP;
  
  /*ȡǰ8b*/
  pcTmp = strchr( pcBuf, '.' );
  if (pcTmp == NULL)
  {
    return 0;
  }
  memset( pcValue, 0x00, sizeof(pcValue) );
  memcpy( pcValue, pcBuf, pcTmp - pcBuf );
  
  /*����зǷ��ַ�*/
  if (!keyValueIsInt(pcValue, pcValue + strlen(pcValue)))
  {
  	return 0;
  }
  
  /*���ֵ����*/
  if (atoi(pcValue) < 0 || atoi(pcValue) > 255)
  {
    return 0;
  }
  pcBuf = pcTmp + 1;
  
  /*ȡ8b��15b*/
  pcTmp = strchr( pcBuf, '.' );
  if (pcTmp == NULL)
  {
    return 0;
  }
  memset( pcValue, 0x00, sizeof(pcValue) );
  memcpy( pcValue, pcBuf, pcTmp - pcBuf );
  /*����зǷ��ַ�*/
  if (!keyValueIsInt(pcValue, pcValue + strlen(pcValue)))
  {
  	return 0;
  }
  
  /*���ֵ����*/
  if (atoi(pcValue) < 0 || atoi(pcValue) > 255)
  {
    return 0;
  }
  pcBuf = pcTmp + 1;
  
  /*ȡ16b��23b*/
  pcTmp = strchr( pcBuf, '.' );
  if (pcTmp == NULL)
  {
    return 0;
  }
  memset( pcValue, 0x00, sizeof(pcValue) );
  memcpy( pcValue, pcBuf, pcTmp - pcBuf );
  /*����зǷ��ַ�*/
  if (!keyValueIsInt(pcValue, pcValue + strlen(pcValue)))
  {
  	return 0;
  }
  
  /*���ֵ����*/
  if (atoi(pcValue) < 0 || atoi(pcValue) > 255)
  {
    return 0;
  }
  pcBuf = pcTmp + 1;
  
  /*ȡǰ24b��31b*/
  memset( pcValue, 0x00, sizeof(pcValue) );
  strcpy( pcValue, pcBuf );
  /*����зǷ��ַ�*/
  if (!keyValueIsInt(pcValue, pcValue + strlen(pcValue)))
  {
  	return 0;
  }
  
  /*���ֵ����*/
  if (atoi(pcValue) < 0 || atoi(pcValue) > 255)
  {
    return 0;
  }
  return 1;
}

/********************************************************
��������keyValueIsBlank
�������ܣ�����ַ����Ƿ�Ϊ�ո� 
��ڲ�����
��    һ���ַ�     I
��    �أ�1��ʾ���ַ�Ϊ�ո�0��ʾ����
����ʱ�䣺20130226
*********************************************************/
int keyValueIsBlank( char c )
{
  if (c == '\t' || c == ' ' || c == '\n' || c == '\0' || c == '\r')
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/********************************************************
��������keyValueIsInt
�������ܣ�����ַ�����Ϊ���� 
��ڲ�����
��    һ���������Ŀ�ʼ     I
��    �����������Ľ���     I 
��    �أ�1��ʾ���ַ�����Ϊ���֣�0��ʾ����
����ʱ�䣺20130226
*********************************************************/
int keyValueIsInt( char *pcaBufStart, char *pcaBufEnd )
{
  char *pcBuf;
  
  pcBuf = pcaBufStart;
  while(1)
  {
    if (pcBuf == pcaBufEnd)
    {
      return 1;
    }
    if (*pcBuf < '0' || *pcBuf >'9')
    {
      return 0;
    }
    else
    {
      pcBuf++;
    }
  }
}

/********************************************************
��������keyValueIsDigitals
�������ܣ�����ַ����Ƿ�Ϊ���� 
��ڲ�����
��    һ���������Ŀ�ʼ     I
��    �����������Ľ���     I 
��    �أ�1��ʾ���ַ�����Ϊ���֣�0��ʾ����
����ʱ�䣺20130226
*********************************************************/
int keyValueIsDigitals( char *pcaBufStart, char *pcaBufEnd )
{
  char *pcBuf;
  
  pcBuf = pcaBufStart;
  while(1)
  {
    if (pcBuf == pcaBufEnd)
    {
      return 1;
    }
    if ((*pcBuf < '0' || *pcBuf >'9') && *pcBuf != '.')
    {
      return 0;
    }
    else
    {
      pcBuf++;
    }
  }
}

/**********************************************************************
��������: keyValueStrcmpIgnore
��������: �Ƚϣ����Դ�Сд 
��    ��: ����� 
����ʱ��: 
��    ����
��    һ����һ��������   I 
��    �����ڶ���������   I
��    �أ�0��ʾ��ȣ�������ʾʧ�� 
**********************************************************************/
int keyValueStrcmpIgnore( char *p, char *p1 )
{
  char c;
  char c1;
  int i;
  int size;
  int size1;
  
  size = strlen(p);
  size1 = strlen(p1);
  
  if (size != size1)
  {
    return -1;
  }
  for (i = 0; i < size; i++)
  {
    c = *(p + i);
    c1 = *(p1 + i);
    if (c >= 'a' && c <= 'z')
     {
       c -= 32;
    }
    if (c1 >= 'a' && c1 <= 'z')
     {
       c1 -= 32;
    }
    if (c != c1)
    {
      return -1;
    }
  }
  return 0;
}

/**********************************************************************
��������: keyValueStrcnmpIgnore
��������: �Ƚϣ����Դ�Сд 
��    ��: ����� 
����ʱ��: 
��    ����
��    һ����һ��������   I 
��    �����ڶ���������   I
��    �أ�0��ʾ��ȣ�������ʾʧ�� 
**********************************************************************/
int keyValueStrcnmpIgnore( char *p, char *p1, int iaSize )
{
  char c;
  char c1;
  int i;
  int size;
  
  size = iaSize;
  
  for (i = 0; i < size; i++)
  {
    c = *(p + i);
    c1 = *(p1 + i);
    if (c >= 'a' && c <= 'z')
     {
       c -= 32;
    }
    if (c1 >= 'a' && c1 <= 'z')
     {
       c1 -= 32;
    }
    if (c != c1)
    {
      return -1;
    }
  }
  return 0;
}

