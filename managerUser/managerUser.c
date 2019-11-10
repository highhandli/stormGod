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
#include <conio.h>
#include <sys/stat.h>
#include <sys/unistd.h>

#ifndef __OS_H
  #include "os.h"
#endif

#ifdef SYSTEM_WINDOWS_64
  #include <winsock2.h>
#endif

/****************************define.h***************************/
#define IS_BLANK(c) (c == '\t' || c == ' ' || c == '\n' || c == '\0' || c == '\r')
#define IS_LETTER(c) ((c >= 'A' && c <='Z') || (c >= 'a' && c <='z'))
#define IS_DIGITAL(c) ((c >='0' && c <='9') || c == '.')

/*�ӿ�ʼ���Կո�*/
#define IGNORE_BLANK_START( start, end ) \
  {\
  	while ((IS_BLANK( *(start) )) && start < end)\
  	{\
	  	start++;\
	  }\
  }

/*�ӽ������Կո�*/
#define IGNORE_BLANK_END( start, end ) \
  {\
  	while ((IS_BLANK( *(end) )) && end > start)\
  	{\
	  	end--;\
	  }\
  }

/*Ѱ�ҵ�һ��������ĸ���ַ�*/
#define IS_NOT_LETTER( start, end ) \
  {\
  	while (start < end)\
  	{\
  	  if (IS_LETTER(*(start)))\
  	  {\
  	    start++;\
  	  }\
  	  else\
  	  {\
  	    break;\
  	  }\
	  }\
  }
  
/*Ѱ�ҵ�һ����������������ַ���Ҳ���ǿո�Ҳ����'.'����'->'���ַ�*/
#define IS_NOT_LETTER_NOR_DIGITALS_NOR_BLANK( start, end ) \
  {\
  	while (start < end)\
  	{\
  	  if (IS_LETTER(*(start)) || IS_DIGITAL(*(start)) || IS_BLANK(*(start)) || ( *(start) == '-' && *(start) == '>' ))\
  	  {\
  	    start++;\
  	  }\
  	  else\
  	  {\
  	    break;\
  	  }\
	  }\
  }

/****************************log.h***************************/
#define MAX_SYN_LOG_SIZE  8192        /*ͬ����־�����ֵ*/

#ifdef FILE_PATH_LEN
  #undef FILE_PATH_LEN
#endif
#define FILE_PATH_LEN     256         /*�ļ�·�������ֵ*/

#ifdef NAME_LEN
  #undef NAME_LEN
#endif
#define NAME_LEN          64          /*�ļ����Ƶ����ֵ*/

#ifdef SUCCESS
  #undef SUCCESS
#endif
#define SUCCESS           0           /*�ɹ���ʶ*/

#ifdef ERROR
  #undef ERROR
#endif
#define ERROR             1           /*ʧ�ܱ�ʶ*/

#define LVSYS  9,__FILE__,__LINE__,__FUNCTION__,"managerUser9.debug"    /* ϵͳ��Ϣ��־����   */
#define LVERR  8,__FILE__,__LINE__,__FUNCTION__,"managerUser8.debug"    /* ������Ϣ��־����   */
#define LVWAR  7,__FILE__,__LINE__,__FUNCTION__,"managerUser7.debug"    /* ������Ϣ��־����   */
#define LVNOR  6,__FILE__,__LINE__,__FUNCTION__,"managerUser6.debug"    /* ������Ϣ��־����   */
#define LVBUG  5,__FILE__,__LINE__,__FUNCTION__,"managerUser5.debug"    /* ������Ϣ��־����   */
#define LVDET  4,__FILE__,__LINE__,__FUNCTION__,"managerUser4.debug"    /* ��ϸ��Ϣ��־����   */

int igSynDebugLevel;            /*ͬ����־����*/

#define FMT "%012ld %04d-%02d-%02d %02d:%02d:%02d:%03d [%s:%s:%d]\n%s"

/****************************client.h***************************/
#define  MAX_KEY_LEN          512            /* KEY ����󳤶�*/
#define  MAX_VALUE_LEN        4096           /* VALUE ����󳤶�*/
#define  SELECT_EACH_SIZE     50             /*��ѯ����ʱ��ÿ�������˻�ȡ������*/
                              
#define  DATA_BUF_LEN         10             /*�ͻ��˹����ĳ�����*/
#define  BAT_RETURN           524288         /*��Ӧ������ѯ����ʱÿ�η��ص�����*/
#define  OPERATE_LEN          4              /*�ͻ��˹����Ĳ�������*/
#define  KEY_VALUE_LENGTH     4              /*�ͻ��˹��������ݳ���*/
#define  MANAGER_SELECT_KEY_CNT       "2101"         /*����ͻ��˹����Ĳ�ѯKEY����*/
#define  MANAGER_SELECT_VALUE_CNT     "2102"         /*����ͻ��˹����Ĳ�ѯVALUE����*/
#define  MANAGER_SELECT_KEY_VALUE_CNT "2103"         /*����ͻ��˹����Ĳ�ѯKEY VALUE����*/
#define  MANAGER_SELECT_MIN_CNT       "2104"         /*����ͻ��˹����Ĳ�ѯ��СKEY����*/
#define  MANAGER_SELECT_MAX_CNT       "2105"         /*����ͻ��˹����Ĳ�ѯ���KEY����*/
#define  MANAGER_SELECT_COUNT_CNT     "2106"         /*����ͻ��˹����Ĳ�ѯKEY��������*/
#define  MANAGER_DELETE_CNT           "2201"         /*����ͻ��˹�����ɾ������*/
#define  MANAGER_UNLOAD_CNT           "2301"         /*����ͻ��˹����ĵ������ݵ��ļ�����*/
#define  MANAGER_LOAD_CNT             "2401"         /*����ͻ��˹����Ĵ��ļ��������ݲ���*/
#define  MANAGER_EXIT_CNT             "2501"         /*����ͻ��˹����������������̲���*/
#define  MANAGER_FLUSH_CNT            "2601"         /*����ͻ��˹����Ľ��ڴ�����д�뵽Ӳ�̲���*/
#define  MANAGER_RAM_TO_DISC_CNT      "2602"         /*����ͻ��˹����Ľ��ڴ�����д�뵽Ӳ�̲���*/
#define  MANAGER_ALTER_THREADS_CNT    "2701"         /*����ͻ��˹����������߳���������*/
#define  MANAGER_ALTER_DISCS_CNT      "2702"         /*����ͻ��˹����������ļ����ֵ����*/
#define  MANAGER_ALTER_UPPERS_CNT     "2703"         /*����ͻ��˹������޸��ļ�����ֵ����*/
#define  MANAGER_SHOW_RAMS_CNT        "2801"         /*����ͻ��˹�������ʾ�ڴ�ʹ���������*/
#define  MANAGER_SHOW_DISCS_CNT       "2802"         /*����ͻ��˹�������ʾӲ��ʹ���������*/
#define  MANAGER_SHOW_THREADS_CNT     "2803"         /*����ͻ��˹�������ʾ�����ӺͶ������߳���������*/
#define  MANAGER_SHOW_KEYS_CNT        "2804"         /*����ͻ��˹�������ʾkey��������*/

#define  MANAGER_INTERRUPT_CNT        "2900"         /*����ͻ��˹�������ֹ����*/

#define  NO_WHERE_KEY_VALUE           "0000"         /*û��WHERE����*/
#define  KEY_OPEN_OPEN                "0001"         /*KEYֵ������������ҿ�()*/
#define  KEY_OPEN_CLOSE               "0002"         /*KEYֵ������������ұ�(]*/
#define  KEY_CLOSE_OPEN               "0003"         /*KEYֵ�������������ҿ�[)*/
#define  KEY_CLOSE_CLOSE              "0004"         /*KEYֵ�������������ұ�[]*/
#define  VALUE_OPEN_OPEN              "0005"         /*VALUEֵ������������ҿ�()*/
#define  VALUE_OPEN_CLOSE             "0006"         /*VALUEֵ������������ұ�(]*/
#define  VALUE_CLOSE_OPEN             "0007"         /*VALUEֵ�������������ҿ�[)*/
#define  VALUE_CLOSE_CLOSE            "0008"         /*VALUEֵ�������������ұ�[]*/

#define  INT_LEN           10                 /*�������ݵ��ַ�������*/
#define  INT64_LEN         20                 /*64λ�������ݵ��ַ�������*/
#define  KEY_LEN 12
#define  VALUE_LEN 12 

#define  WAIT_OUT 0
#define  WAIT_IN  1
#define  TIME_OUT 60
#define  NO_TIME_OUT 0     /*����ʱ�ȴ�*/
#define  TIME_STEP   1000     /*�������ʱ��*/
#define  NULL_SELECT_ID    "00000000000000000000" /*�յ�SELECT ID������ΪINT64_LEN��*/

#define  WAIT_SERVER_TIME 10*60 /*�ȴ�����˷��ص��ʱ��*/

#define SOCKET_SLEEP_TIME      100000   /*������ʱ��˯��ʱ�䣬UNIX��λ��΢��*/
#define SOCKET_THREAD_SLEEP() usleep( SOCKET_SLEEP_TIME );

/****************************func.h***************************/
void managerDeal( char *pcaBufStart, char *pcaBufEnd );
void managerSelectDeal( char *pcaBufStart, char *pcaBufEnd );
void managerSelectMinDeal( char *pcaBufStart, char *pcaBufEnd );
void managerSelectMaxDeal( char *pcaBufStart, char *pcaBufEnd );
void managerSelectCountDeal( char *pcaBufStart, char *pcaBufEnd );
void managerSelectKeyValueDeal( char *pcaBufStart, char *pcaBufEnd );
void managerDeleteDeal( char *pcaBufStart, char *pcaBufEnd );
void managerUnloadDeal( char *pcaBufStart, char *pcaBufEnd );
void managerLoadDeal( char *pcaBufStart, char *pcaBufEnd );
void managerExitDeal( char *pcaBufStart, char *pcaBufEnd );
void managerFlushDeal( char *pcaBufStart, char *pcaBufEnd );
void managerRamDeal( char *pcaBufStart, char *pcaBufEnd );
void managerAlterDeal( char *pcaBufStart, char *pcaBufEnd );
void managerAlterThreadsDeal( char *pcaBufStart, char *pcaBufEnd );
void managerAlterDiscsDeal( char *pcaBufStart, char *pcaBufEnd );
void managerAlterUppersDeal( char *pcaBufStart, char *pcaBufEnd );
void managerShowDeal( char *pcaBufStart, char *pcaBufEnd );
void managerShowRamsDeal( );
void managerShowDiscsDeal( );
void managerShowThreadsDeal( );
void managerShowKeysDeal( );
void managerShowLoopsDeal( );
void managerCheckDeal( );
int connectToServer( char *pcaIp, char *pcaPort );
int waitFd( int iFd, int iFlag, int iTimeOut );
char *AllTrim ( char *pcaStr );
int checkPort( char *pcaPort );
int checkIP( char *pcaIP );
int isBlank( char c );
int isInt( char *pcaBufStart, char *pcaBufEnd );
int isDigitals( char *pcaBufStart, char *pcaBufEnd );
int strcmpIgnore( char *p, char *p1 );
int strncmpIgnore( char *p, char *p1, int iaSize );
int logInitSyn( int iaLevel );
int logFmtSyn( int iaLevel, char *pcaFile, int iaLine, const char *pcaFunc, char *pcaLogFile, const char *aFmt, ... );
int logSyn( int iaLevel, char *pcaFile, int iaLine, const char *pcaFunc, char *pcaLogFile,  char *pcaBuf, int iaSize );

char pcgIp[20];
char pcgPort[10];

#define KEY_WORD_LEN       256

#define SHOW_ERRMSG( msg ) \
{\
	/*fprintf( stderr, "line:%d %s\n", __LINE__, msg );\*/\
	fprintf( stderr, "%s\n", msg );\
	logFmtSyn( LVERR, "%s", msg );\
}

#ifdef SYSTEM_WINDOWS_64
#else
  #define closesocket( socket ) close( socket )
#endif

#define MANAGER_RECV( socket, tmp, recvBuf, recvTmp, recvSize ) \
{\
  memset( recvBuf, 0x00, sizeof(recvBuf) );\
  recvTmp = 0;\
  while (1)\
  {\
    tmp = recv( socket, recvBuf + recvTmp, recvSize - recvTmp, 0 );\
    \
    if (tmp <= 0)\
    {\
    	break;\
    }\
    else\
    {\
    	recvTmp += tmp;\
    }\
    \
    if (recvTmp == recvSize)\
    {\
    	break;\
    }\
  }\
  if (recvTmp != recvSize)\
	{\
		SHOW_ERRMSG("���շ��������ʧ��");\
		fprintf( stdout, "recvTmp[%d]\n", recvTmp );\
		closesocket( socket );\
		return;\
	}\
}

int  igGetFromKeyBoard;        /*�Ӽ��̻�ȡ����*/

int main( int argc, char *argv[] )
{
	#ifdef SYSTEM_WINDOWS_64
	  WSADATA wsaData;
	#endif
	char pcLine[8192];
	char *pcStart;
	char *pcEnd;
	
	#ifdef SYSTEM_WINDOWS_64
	  if (WSAStartup(MAKEWORD(2,1),&wsaData)) 
    {
    	SHOW_ERRMSG("Winsock�޷���ʼ��!");
      WSACleanup();
      return 0;
    }
	#endif
	
	if (argc < 3)
	{
		fprintf( stdout, "������Ŀ�� IP ��\n" );
		memset( pcgIp, 0x00, sizeof(pcgIp) );
		gets( pcgIp );
		logFmtSyn( LVNOR, "��ȡ�������ݣ�%s", pcgIp );
		AllTrim( pcgIp );
		
		fprintf( stdout, "������Ŀ��˿ڣ�\n" );
		memset( pcgPort, 0x00, sizeof(pcgPort) );
		gets( pcgPort );
		logFmtSyn( LVNOR, "��ȡ�������ݣ�%s", pcgPort );
		AllTrim( pcgPort );
		
		
		if (!checkIP( pcgIp ))
		{
			SHOW_ERRMSG("IP��ַ�����ϸ�ʽ");
			return 0;
		}
		
		if (!checkPort( pcgPort ))
		{
			SHOW_ERRMSG("�˿ڲ����ϸ�ʽ");
			return 0;
		}
	}
	else
	{
		memset( pcgIp, 0x00, sizeof(pcgIp) );
		memset( pcgPort, 0x00, sizeof(pcgPort) );
		
		strcpy( pcgIp, argv[1] );
		strcpy( pcgPort, argv[2] );
		
		if (!checkIP( pcgIp ))
		{
			SHOW_ERRMSG("IP��ַ�����ϸ�ʽ");
			return 0;
		}
		
		if (!checkPort( pcgPort ))
		{
			SHOW_ERRMSG("�˿ڲ����ϸ�ʽ");
			return 0;
		}
	}
	
	logFmtSyn( LVNOR, "��ʼ���ɹ�������������" );
	fprintf( stdout, "��ʼ���ɹ�������������\n" );
	
	logInitSyn( 2 );
	
	while(1)
	{
		//SHOW_ERRMSG("����������");
		memset( pcLine, 0x00, sizeof(pcLine) );
		gets( pcLine );
		logFmtSyn( LVNOR, "��ȡ�������ݣ�%s", pcLine );
		AllTrim( pcLine );
		
		if (strlen( pcLine ) == 0)
		{
			continue;
		}
		
		pcStart = pcLine;
		pcEnd = pcLine + strlen(pcLine);
		
		IGNORE_BLANK_START( pcStart, pcEnd );
		if (pcStart == pcEnd)
		{
			continue;
		}
		
		IGNORE_BLANK_END( pcStart, pcEnd );
		
		if (!strcmpIgnore( pcStart, "exit" ))
    {
    	exit(0);
    }
    else
    {
    	managerDeal( pcStart, pcEnd );
    }
	}
}

/**********************************************************************
��������: managerDeal
��������: �������˽��յ�������
��    ����
��    һ�����ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20161018
**********************************************************************/
void managerDeal( char *pcaBufStart, char *pcaBufEnd )
{
	char pcKeyWord[KEY_WORD_LEN];
	char pcSendLen[DATA_BUF_LEN+1];
	char pcSendErrMsg[256];
	char *pcBlank;
	char *pcBufStart;
	char *pcBufEnd;
	int  iSendLen;
	
	logFmtSyn( LVSYS, "��ʼִ�к�����%s", __FUNCTION__ );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
	/*ȡ��һ���ؼ���*/
	{
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	//SHOW_ERRMSG("ֻ�в�������ȱ�ٲ�������");
	    //return;
	    pcBlank = pcBufEnd + 1;
	  }
	  
	  if (pcBlank - pcBufStart >= KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("�ò������Ͳ�����");
	    return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
  }
  
  logFmtSyn( LVSYS, "������%s", pcKeyWord );
	
	if (!strcmpIgnore( pcKeyWord, "SELECT" ))
	{
		managerSelectDeal( pcBlank + 1, pcBufEnd );
	}
	else
	if (!strcmpIgnore( pcKeyWord, "DELETE" ))
	{
		managerDeleteDeal( pcBlank + 1, pcBufEnd );
	}
	else
	if (!strcmpIgnore( pcKeyWord, "UNLOAD" ))
	{
		managerUnloadDeal( pcBlank + 1, pcBufEnd );
	}
	else
	if (!strcmpIgnore( pcKeyWord, "LOAD" ))
	{
		managerLoadDeal( pcBlank + 1, pcBufEnd );
	}
	else
	if (!strcmpIgnore( pcKeyWord, "EXIT" ))
	{
		managerExitDeal( pcBlank + 1, pcBufEnd );
	}
	else
	if (!strcmpIgnore( pcKeyWord, "FLUSH" ))
	{
		managerFlushDeal( pcBlank + 1, pcBufEnd );
	}
	else
	if (!strcmpIgnore( pcKeyWord, "RAM" ))
	{
		managerRamDeal( pcBlank + 1, pcBufEnd );
	}
	else
	if (!strcmpIgnore( pcKeyWord, "ALTER" ))
	{
		managerAlterDeal( pcBlank + 1, pcBufEnd );
	}
	else
	if (!strcmpIgnore( pcKeyWord, "SHOW" ))
	{
		managerShowDeal( pcBlank + 1, pcBufEnd );
	}
	else
	{
		fprintf( stdout, "�ò�������[%s]������", pcKeyWord );
		SHOW_ERRMSG("�ò������Ͳ�����");
	  return;
	}

	return;
}

/**********************************************************************
��������: managerSelectDeal
��������: ����˴��� select ����������
��    ����
��    һ�����ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20161018
**********************************************************************/
void managerSelectDeal( char *pcaBufStart, char *pcaBufEnd )
{
  char pcKeyWord[KEY_WORD_LEN];
	char *pcBlank;
	char *pcBufStart;
	char *pcBufEnd;
	
	logFmtSyn( LVSYS, "��ʼִ�к�����%s", __FUNCTION__ );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if (pcBufStart == pcBufEnd)
	{
		SHOW_ERRMSG("ֻ�в�������ȱ�ٲ�������");
		return;
	}
	
	pcBlank = strchr( pcBufStart, ' ' );
	if (pcBlank == NULL)
	{
		pcBlank = pcBufEnd;
	}
	else
	{
		pcBlank = pcBlank - 1;
	}
	
	if (pcBlank - pcBufStart > KEY_WORD_LEN)
	{
		SHOW_ERRMSG("�������Ͳ�����");
		return;
	}
	
	memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart + 1);

	if (!strcmpIgnore( pcKeyWord, "MIN" ))
	{
		managerSelectMinDeal( pcBlank + 1, pcBufEnd );
	}
	else
	if (!strcmpIgnore( pcKeyWord, "MAX" ))
	{
		managerSelectMaxDeal( pcBlank + 1, pcBufEnd );
	}
	else
	if (!strcmpIgnore( pcKeyWord, "COUNT" ))
	{
		managerSelectCountDeal( pcBlank + 1, pcBufEnd );
	}
	else
	{
		managerSelectKeyValueDeal( pcBufStart, pcBufEnd );
	}

	return;
}

/**********************************************************************
��������: managerSelectMinDeal
��������: ����˴��� select min ����������
��    ����
��    һ�����ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20161020
**********************************************************************/
void managerSelectMinDeal( char *pcaBufStart, char *pcaBufEnd )
{
/*
�������ݽṹ������+����
�������ݽṹ���������+KEYֵ����+KEYֵ+VALUEֵ����+VALUEֵ
*/
	char pcSendLen[DATA_BUF_LEN + 1];
	char pcSendBuf[OPERATE_LEN + 1];
	char pcKeyLen[KEY_VALUE_LENGTH + 1];
	char pcKey[MAX_KEY_LEN + 1];
	char pcValueLen[KEY_VALUE_LENGTH + 1];
	char pcValue[MAX_VALUE_LEN + 1];
	char pcRecvLen[DATA_BUF_LEN + 1];
	char cResult;
	int  iSendLen;
	int  iRecvLen;
	int  iKeyLen;
	int  iValueLen;
	int  iSocket;
	int  iRet;
	
	logFmtSyn( LVSYS, "��ʼִ�к�����%s", __FUNCTION__ );
	
	memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
	strcpy( pcSendBuf, MANAGER_SELECT_MIN_CNT );
	
	iSendLen = strlen(pcSendBuf);
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
	
	iSocket = connectToServer( pcgIp, pcgPort );
	if (iSocket == -1)
	{
		SHOW_ERRMSG("���ӵ������ʧ��");
		return;
	}
	
	/*���ͳ�����*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	/*��������*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	iRet = waitFd( iSocket, WAIT_IN, TIME_OUT );
	if (iRet == -1)
	{
		SHOW_ERRMSG("�ȴ����շ��������ʧ��");
		return;
	}
	
	/*�������ݳ���*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*��������*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("���շ��������ʧ��");
		return;
	}
	
	fprintf( stdout, "result:\n" );
	logFmtSyn( LVNOR, "result:" );
	
	if (cResult == '0')
	{
		/*����KEYֵ����*/
		MANAGER_RECV( iSocket, iRet, pcKeyLen, iRecvLen, KEY_VALUE_LENGTH );
	  iKeyLen = atoi( pcKeyLen );
	  
	  /*����KEYֵ*/
	  MANAGER_RECV( iSocket, iRet, pcKey, iRecvLen, iKeyLen );
	  
	  /*����VALUE����*/
	  MANAGER_RECV( iSocket, iRet, pcValueLen, iRecvLen, KEY_VALUE_LENGTH );
	  iValueLen = atoi( pcValueLen );
	  
	  /*����VALUEֵ*/
	  MANAGER_RECV( iSocket, iRet, pcValue, iRecvLen, iValueLen );
	  
	  fprintf( stdout, "min key :%s\nvalue   :%s\n", pcKey, pcValue );	  
		logFmtSyn( LVNOR, "min key :%s\nvalue   :%s", pcKey, pcValue );	  
	}
	else
	{
		fprintf( stdout, "fail\n" );
		logFmtSyn( LVNOR, "fail" );
	}
	
	return;
}

/**********************************************************************
��������: managerSelectMaxDeal
��������: ����˴��� select max ����������
��    ����
��    һ�����ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20161020
**********************************************************************/
void managerSelectMaxDeal( char *pcaBufStart, char *pcaBufEnd )
{
/*
�������ݽṹ������+����
�������ݽṹ���������+KEYֵ����+KEYֵ+VALUEֵ����+VALUEֵ
*/
	char pcSendLen[DATA_BUF_LEN + 1];
	char pcSendBuf[OPERATE_LEN + 1];
	char pcKeyLen[KEY_VALUE_LENGTH + 1];
	char pcKey[MAX_KEY_LEN + 1];
	char pcValueLen[KEY_VALUE_LENGTH + 1];
	char pcValue[MAX_VALUE_LEN + 1];
	char pcRecvLen[DATA_BUF_LEN + 1];
	char cResult;
	int  iSendLen;
	int  iRecvLen;
	int  iKeyLen;
	int  iValueLen;
	int  iSocket;
	int  iRet;
	
	logFmtSyn( LVSYS, "��ʼִ�к�����%s", __FUNCTION__ );
	
	memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
	strcpy( pcSendBuf, MANAGER_SELECT_MAX_CNT );
	
	iSendLen = strlen(pcSendBuf);
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
	
	iSocket = connectToServer( pcgIp, pcgPort );
	if (iSocket == -1)
	{
		SHOW_ERRMSG("���ӵ������ʧ��");
		return;
	}
	
	/*���ͳ�����*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	/*��������*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	iRet = waitFd( iSocket, WAIT_IN, TIME_OUT );
	if (iRet == -1)
	{
		SHOW_ERRMSG("�ȴ����շ��������ʧ��");
		return;
	}
	
	/*�������ݳ���*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*��������*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("���շ��������ʧ��");
		return;
	}
	
	fprintf( stdout, "result:\n" );
	logFmtSyn( LVNOR, "result:" );
	
	if (cResult == '0')
	{
		/*����KEYֵ����*/
		MANAGER_RECV( iSocket, iRet, pcKeyLen, iRecvLen, KEY_VALUE_LENGTH );
	  iKeyLen = atoi( pcKeyLen );
	  
	  /*����KEYֵ*/
	  MANAGER_RECV( iSocket, iRet, pcKey, iRecvLen, iKeyLen );
	  
	   /*����VALUE����*/
	  MANAGER_RECV( iSocket, iRet, pcValueLen, iRecvLen, KEY_VALUE_LENGTH );
	  iValueLen = atoi( pcValueLen );
	  
	  /*����VALUEֵ*/
	  MANAGER_RECV( iSocket, iRet, pcValue, iRecvLen, iValueLen );
	  
	  fprintf( stdout, "max key :%s\nvalue   :%s\n", pcKey, pcValue );	  
		logFmtSyn( LVNOR, "max key :%s\nvalue   :%s", pcKey, pcValue );	  
	}
	else
	{
		fprintf( stdout, "fail\n" );
		logFmtSyn( LVNOR, "fail" );
	}
	
	return;
}

/**********************************************************************
��������: managerSelectCountDeal
��������: ����˴��� select count ����������
��    ����
��    һ�����ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20161020
**********************************************************************/
void managerSelectCountDeal( char *pcaBufStart, char *pcaBufEnd )
{
/*
�������ݽṹ������+����+ֵ���䣨�Ƿ������������ҿ������ұա�����ҿ�������ұգ�+KEY��ʼֵ����+KEY��ʼֵ+KEY����ֵ����+KEY����ֵ
�������ݽṹ���������+COUNTֵ
*/
  char pcKeyWord[KEY_WORD_LEN];
  char *pcBufStart;
	char *pcBufEnd;
	char *pcBlank;
	char *pcTmp;
  char pcKeyStartLen[KEY_VALUE_LENGTH + 1];
  char pcKeyEndLen[KEY_VALUE_LENGTH + 1];
  char pcKeyStart[MAX_KEY_LEN + 1];
  char pcKeyEnd[MAX_KEY_LEN + 1];
  char pcValueType[KEY_VALUE_LENGTH + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	char pcSendBuf[OPERATE_LEN + OPERATE_LEN + KEY_VALUE_LENGTH + MAX_KEY_LEN + KEY_VALUE_LENGTH + MAX_KEY_LEN + 1];
	char pcRecvLen[DATA_BUF_LEN + 1];
	char pcCount[INT64_LEN + 1];
	char cResult;
	char cValueStart;
	char cValueEnd;
	int  iKeyStartLen;
	int  iKeyEndLen;
	int  iSendLen;
	int  iRecvLen;
	int  iSocket;
	int  iRet;
	
	logFmtSyn( LVSYS, "��ʼִ�к�����%s", __FUNCTION__ );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if (pcBufStart == pcBufEnd)
	{
	  memset( pcValueType, 0x00, sizeof(pcValueType) );
	  memcpy( pcValueType, NO_WHERE_KEY_VALUE, OPERATE_LEN );
	  
	  memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
	  memcpy( pcSendBuf, MANAGER_SELECT_COUNT_CNT, OPERATE_LEN );
	  memcpy( pcSendBuf + OPERATE_LEN, pcValueType, OPERATE_LEN );
	}
	else
	{
	  /*ȡ�� where �ؼ���*/
	  {
	    pcBlank = strchr( pcBufStart, ' ' );
	    if (pcBlank == NULL)
	    {
	    	SHOW_ERRMSG("��䲻��ȷ");
	    	return;
	    }
	    
	    if (pcBlank - pcBufStart > KEY_WORD_LEN)
	    {
	    	SHOW_ERRMSG("��䲻��ȷ");
	    	return;
	    }
	    
	    memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	    memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	    
	    if (strcmpIgnore( pcKeyWord, "WHERE" ))
	    {
	    	SHOW_ERRMSG("��䲻��ȷ");
	  	  return;
	    }
	    
	    pcBufStart = pcBlank + 1;
    }
    
    logFmtSyn( LVSYS, "���� WHERE �ؼ��ֳɹ�" );
    
    /*ȡ�� key �ؼ���*/
	  {
	  	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	    if (pcBufStart == pcBufEnd)
	    {
	  	  SHOW_ERRMSG("��䲻��ȷ");
	  	  return;
	    }
	    
	    /*Ѱ�ҵ�һ��������ĸ���ַ�*/
	    pcTmp = pcBufStart;
	    IS_NOT_LETTER( pcTmp, pcBufEnd );
	    if (pcTmp == pcBufEnd)
	    {
	  	  SHOW_ERRMSG("��䲻��ȷ");
	  	  return;
	    }
	    
	    /*����ǿո���߿����ַ�*/
	    if (isBlank(*pcTmp) || (*pcTmp) == '[' || (*pcTmp) == '(')
	    {
	    	pcBlank = pcTmp;
	    	
	    	if (pcBlank - pcBufStart > KEY_WORD_LEN)
	      {
	      	SHOW_ERRMSG("��䲻��ȷ");
	    	  return;
	      }
	      memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	      memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	    
	      if (strcmpIgnore( pcKeyWord, "KEY" ))
	      {
	    	  SHOW_ERRMSG("��䲻��ȷ");
	  	    return;
	      }
	    }
	    else
	    {
	    	SHOW_ERRMSG("��䲻��ȷ");
	  	  return;
	    }
	    
	    pcBufStart = pcTmp;
    }
    
    logFmtSyn( LVSYS, "���� KEY �ؼ��ֳɹ�" );
    
    /*ָ�� key ������ַ�*/
	  {
	  	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	    if (pcBufStart == pcBufEnd)
	    {
	  	  SHOW_ERRMSG("��䲻��ȷ");
	  	  return;
	    }
	    
	    /*������ǿ����ַ�*/
	    if (!((*pcBufStart) == '[' || (*pcBufStart) == '('))
	    {
	    	SHOW_ERRMSG("��䲻��ȷ");
	  	  return;
	    }
	  }
	  
	  cValueStart = *pcBufStart;
    pcBufStart++;
    
	  /*ȡ����һ��keyֵ*/
	  {
	    /*������һ��������*/
	    IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	    if (pcBufStart == pcBufEnd)
	    {
	      SHOW_ERRMSG("��䲻��ȷ");
	      return;
	    }
	    
	    /*������ǵ�����*/
	    if (!((*pcBufStart) == '\''))
	    {
	    	SHOW_ERRMSG("��䲻��ȷ");
	      return;
	    }
	    pcBufStart++;
	    
	    /*�����ڶ���������*/
	    pcTmp = strchr( pcBufStart, '\'' );
	    if (pcTmp == NULL || pcTmp >= pcBufEnd)
	    {
	      SHOW_ERRMSG("��䲻��ȷ");
	      return;
	    }
	    
	    /*ȡ����һ��keyֵ*/
	    if (pcTmp - pcBufStart > MAX_KEY_LEN)
	    {
	    	SHOW_ERRMSG("KEYֵ����");
	      return;
	    }
	    memset( pcKeyStart, 0x00, sizeof(pcKeyStart) );
	    memcpy( pcKeyStart, pcBufStart, pcTmp - pcBufStart );
	    
	    iKeyStartLen = strlen( pcKeyStart );
	    memset( pcKeyStartLen, 0x00, sizeof(pcKeyStartLen) );
	    sprintf( pcKeyStartLen, "%0*d", KEY_VALUE_LENGTH, iKeyStartLen );
	    
	    pcBufStart = pcTmp + 1;
	  }
	  
	  logFmtSyn( LVSYS, "pcKeyStart[%s]", pcKeyStart );
	  
	  /*ȡ���ڶ���keyֵ*/
	  {
	    /*��������*/
	    IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	    if (pcBufStart == pcBufEnd)
	    {
	      SHOW_ERRMSG("��䲻��ȷ");
	      return;
	    }
	    
	    /*������Ƕ���*/
	    if (!((*pcBufStart) == ','))
	    {
	    	SHOW_ERRMSG("��䲻��ȷ");
	      return;
	    }
	    pcBufStart++;
	    
	    /*����������������*/
	    IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	    if (pcBufStart == pcBufEnd)
	    {
	      SHOW_ERRMSG("��䲻��ȷ");
	      return;
	    }
	    
	    /*������ǵ�����*/
	    if (!((*pcBufStart) == '\''))
	    {
	    	SHOW_ERRMSG("��䲻��ȷ");
	      return;
	    }
	    pcBufStart++;
	    
	    /*�������ĸ�������*/
	    pcTmp = strchr( pcBufStart, '\'' );
	    if (pcTmp == NULL || pcTmp >= pcBufEnd)
	    {
	      SHOW_ERRMSG("��䲻��ȷ");
	      return;
	    }
	    
	    /*ȡ���ڶ���keyֵ*/
	    if (pcTmp - pcBufStart > MAX_KEY_LEN)
	    {
	    	SHOW_ERRMSG("KEYֵ����");
	      return;
	    }
	    memset( pcKeyEnd, 0x00, sizeof(pcKeyEnd) );
	    memcpy( pcKeyEnd, pcBufStart, pcTmp - pcBufStart );
	    
	    iKeyEndLen = strlen( pcKeyEnd );
	    memset( pcKeyEndLen, 0x00, sizeof(pcKeyEndLen) );
	    sprintf( pcKeyEndLen, "%0*d", KEY_VALUE_LENGTH, iKeyEndLen );
	    
	    pcBufStart = pcTmp + 1;
	  }
	  
	  logFmtSyn( LVSYS, "pcKeyEnd[%s]", pcKeyEnd );
	  
	  /*ȡ�����Ŀ��շ�*/
    IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if ((*pcBufStart) == ']' || (*pcBufStart) == ')')
	  {
	  	cValueEnd = (*pcBufStart);
	  }
	  else
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	    return;
	  }
	  
	  /*ȷ��ֵ����*/
	  memset( pcValueType, 0x00, sizeof(pcValueType) );
	  if (cValueStart == '(' && cValueEnd == ')')
	  {
	  	if (strcmp( pcKeyStart, pcKeyEnd ) >= 0)
	  	{
		  	SHOW_ERRMSG("��һ��ֵֻ����С�ڵڶ���ֵ");
	      return;
		  }
	  	memcpy( pcValueType, KEY_OPEN_OPEN, OPERATE_LEN );
	  }
	  else
	  if (cValueStart == '(' && cValueEnd == ']')
	  {
	  	if (strcmp( pcKeyStart, pcKeyEnd ) > 0)
	  	{
			  SHOW_ERRMSG("��һ��ֵֻ����С�ڻ���ڵڶ���ֵ");
	      return;
		  }
	  	memcpy( pcValueType, KEY_OPEN_CLOSE, OPERATE_LEN );
	  }
	  else
	  if (cValueStart == '[' && cValueEnd == ')')
	  {
	  	if (strcmp( pcKeyStart, pcKeyEnd ) >= 0)
	  	{
		  	SHOW_ERRMSG("��һ��ֵֻ����С�ڵڶ���ֵ");
	      return;
		  }
	  	memcpy( pcValueType, KEY_CLOSE_OPEN, OPERATE_LEN );
	  }
	  else
	  {
	  	if (strcmp( pcKeyStart, pcKeyEnd ) > 0)
	  	{
			  SHOW_ERRMSG("��һ��ֵֻ����С�ڻ���ڵڶ���ֵ");
	      return;
		  }
	  	memcpy( pcValueType, KEY_CLOSE_CLOSE, OPERATE_LEN );
	  }
	  
	  memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
	
	  memcpy( pcSendBuf, MANAGER_SELECT_COUNT_CNT, OPERATE_LEN );
	  memcpy( pcSendBuf + OPERATE_LEN, pcValueType, OPERATE_LEN );
	  memcpy( pcSendBuf + OPERATE_LEN + OPERATE_LEN, pcKeyStartLen, KEY_VALUE_LENGTH );
	  memcpy( pcSendBuf + OPERATE_LEN + OPERATE_LEN + KEY_VALUE_LENGTH, pcKeyStart, iKeyStartLen );
  	memcpy( pcSendBuf + OPERATE_LEN + OPERATE_LEN + KEY_VALUE_LENGTH + iKeyStartLen, pcKeyEndLen, KEY_VALUE_LENGTH );
	  memcpy( pcSendBuf + OPERATE_LEN + OPERATE_LEN + KEY_VALUE_LENGTH + iKeyStartLen + KEY_VALUE_LENGTH, pcKeyEnd, iKeyEndLen );
  }
  
  iSendLen = strlen(pcSendBuf);
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
	
	iSocket = connectToServer( pcgIp, pcgPort );
	if (iSocket == -1)
	{
		SHOW_ERRMSG("���ӵ������ʧ��");
		return;
	}

	/*���ͳ�����*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	/*��������*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	SHOW_ERRMSG("�������ݵ�����˳ɹ��������ĵȴ�����˴���");
	
	/*�ȴ�����*/
	iRet = waitFd( iSocket, WAIT_IN, WAIT_SERVER_TIME );
	
	/*����������*/
  if (iRet == -1)
	{
		SHOW_ERRMSG("�ȴ���������ʧ��");
	  return;
	}
	
	/*���ճ���*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*���ֵ��0����ʾ��·����*/
	if (atoi( pcRecvLen ) == 0)
	{
		SHOW_ERRMSG("�ȴ���������ʧ��");
	  return;
	}

	/*��������*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("���շ��������ʧ��");
		return;
	}
	
	fprintf( stdout, "result:\n" );
	logFmtSyn( LVNOR, "result:" );
	
	if (cResult == '0')
	{
		/*����COUNTֵ*/
		MANAGER_RECV( iSocket, iRet, pcCount, iRecvLen, INT64_LEN );
	  
	  fprintf( stdout, "count :%lld\n", atoll( pcCount ) );	  
		logFmtSyn( LVNOR, "count :%lld", atoll( pcCount ) );
	}
	else
	{
		fprintf( stdout, "fail\n" );
		logFmtSyn( LVNOR, "fail" );
	}
	
	return;
}

/**********************************************************************
��������: managerSelectKeyValueDeal
��������: ����˴��� select key value ����������
��    ����
��    һ�����ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20161020
**********************************************************************/
void managerSelectKeyValueDeal( char *pcaBufStart, char *pcaBufEnd )
{
/*
�������ݽṹ������+����+��ѯID+��ȡ����+��ȡ��ʼλ��+ֵ���䣨���ҿ������ұա�����ҿ�������ұգ�+KEY��ʼֵ����+KEY��ʼֵ+KEY����ֵ����+KEY����ֵ
�������ݽṹ���������+��ѯID+��ǰ��������+(KEY��VALUE�ĳ��ȼ�ֵ)
���͹�ȥ�����ݣ���������ո񣬷���˴������־���ܻ��١�ͳһ������ǰ��0���ַ��󲹿ո񣬷����У���ַ��Ƿ�һ��ʱ���ú��Կո�
*/
  char pcKeyWord[KEY_WORD_LEN];
	char *pcBufStart;
	char *pcBufEnd;
	char *pcBlank;
	char *pcTmp;
	char pcGetRows[INT64_LEN+1];
  char pcKeyStartLen[KEY_VALUE_LENGTH + 1];
  char pcKeyEndLen[KEY_VALUE_LENGTH + 1];
  char pcKeyStart[MAX_KEY_LEN + 1];
  char pcKeyEnd[MAX_KEY_LEN + 1];
  char pcValueStartLen[KEY_VALUE_LENGTH + 1];
  char pcValueEndLen[KEY_VALUE_LENGTH + 1];
  char pcValueStart[MAX_VALUE_LEN + 1];
  char pcValueEnd[MAX_VALUE_LEN + 1];
  char pcValueType[KEY_VALUE_LENGTH + 1];
  char pcTotalRows[INT64_LEN + 1];
  char pcRowStartIndex[INT64_LEN + 1];
  char pcRows[INT64_LEN + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	char pcSendBuf[OPERATE_LEN + INT64_LEN + INT64_LEN + OPERATE_LEN + KEY_VALUE_LENGTH + MAX_VALUE_LEN + KEY_VALUE_LENGTH + MAX_VALUE_LEN + 1];
	char pcKeyValueLen[KEY_VALUE_LENGTH + 1];
	char pcKey[MAX_KEY_LEN + 1];
	char pcValue[MAX_VALUE_LEN + 1];
	char pcRecvLen[DATA_BUF_LEN + 1];
	char pcSelectId[INT64_LEN+1];
	char cValueStart;
	char cValueEnd;
	char cResult;
	char cGet;
	int  iKeyStartLen;
	int  iKeyEndLen;
	int  iValueStartLen;
	int  iValueEndLen;
	int  iKeyFlag;    /*0��ʾ��ѯ�ֶ�û��key��1��ʾ��*/
	int  iValueFlag;  /*0��ʾ��ѯ�ֶ�û��value��1��ʾ��*/
	int  iWhereFlag;  /*0��ʾ where key��1��ʾwhere value*/
	int  iSendLen;
	int  iRecvLen;
	int  iSocket;
	int  iRet;
	int  iRows;
	int  iKeyValueLen;
	int  iTmp;
	__int64 llRowStartIndex;
	
	logFmtSyn( LVSYS, "��ʼִ�к�����%s", __FUNCTION__ );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if (pcBufStart == pcBufEnd)
	{
	  SHOW_ERRMSG("��䲻��ȷ");
	  return;
	}
	
	/*Ѱ�ҵ�һ��������ĸ���ַ�*/
	pcTmp = pcBufStart;
	IS_NOT_LETTER( pcTmp, pcBufEnd );
	if (pcTmp == pcBufEnd)
	{
	  SHOW_ERRMSG("��䲻��ȷ");
	  return;
	}
	
	/*ȷ����ѯ�ֶ�*/
	{
		iKeyFlag = 0;
		iValueFlag = 0;
		
	  /*����ǿո�*//*ֻ�����ǿո���߶���*/
	  if (isBlank(*pcTmp))
	  {
	  	pcBlank = pcTmp;
	  	
	  	if (pcBlank - pcBufStart > KEY_WORD_LEN)
	    {
	    	SHOW_ERRMSG("��䲻��ȷ");
	    	return;
	    }
	    
	    memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	    memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	    
	    if (!strcmpIgnore( pcKeyWord, "KEY" ))
	    {
	    	iKeyFlag = 1;
	    	pcBufStart = pcBlank + 1;
	    	
	    	/*ȡ��һ���ַ�*/
	    	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	      if (pcBufStart == pcBufEnd)
	      {
		      SHOW_ERRMSG("��䲻��ȷ");
		      return;
	      }
	      
	      /*KEY����ֻ�ܸ�VALUE*/
	      if ((*pcBufStart) == ',')
	      {
	      	pcBufStart++;
	      	
	      	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	        if (pcBufStart == pcBufEnd)
	        {
		        SHOW_ERRMSG("��䲻��ȷ");
		        return;
	        }
	        
	        pcBlank = strchr( pcBufStart, ' ' );
	        if (pcBlank == NULL || pcBlank > pcBufEnd)
	        {
	        	SHOW_ERRMSG("��䲻��ȷ");
	        	return;
	        }
	        
	        if (pcBlank - pcBufStart > KEY_WORD_LEN)
	        {
	        	SHOW_ERRMSG("��䲻��ȷ");
	        	return;
	        }
	        
	        memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	        memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	        
	        if (strcmpIgnore( pcKeyWord, "VALUE" ))
	        {
	        	SHOW_ERRMSG("��䲻��ȷ");
		        return;
	        }
	        
	        iValueFlag = 1;
	        
	        pcBufStart = pcBlank + 1;
	      }
	      else
	      /*����Ҫ����*/
	      if (IS_LETTER((*pcBufStart)))
	      {
	      }
	      else
	      {
		      SHOW_ERRMSG("��䲻��ȷ");
		      return;
	      }
	    }
	    else
	    if (!strcmpIgnore( pcKeyWord, "VALUE" ))
	    {
	    	iValueFlag = 1;
	    	
	    	pcBufStart = pcBlank + 1;
	    	
	    	/*ȡ��һ���ַ�*/
	    	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	      if (pcBufStart == pcBufEnd)
	      {
		      SHOW_ERRMSG("��䲻��ȷ");
		      return;
	      }
	      
	      /*VALUE����ֻ�ܸ�KEY*/
	      if ((*pcBufStart) == ',')
	      {
	      	pcBufStart++;
	      	
	      	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	        if (pcBufStart == pcBufEnd)
	        {
		        SHOW_ERRMSG("��䲻��ȷ");
		        return;
	        }
	        
	        pcBlank = strchr( pcBufStart, ' ' );
	        if (pcBlank == NULL || pcBlank > pcBufEnd)
	        {
	        	SHOW_ERRMSG("��䲻��ȷ");
	        	return;
	        }
	        
	        if (pcBlank - pcBufStart > KEY_WORD_LEN)
	        {
	        	SHOW_ERRMSG("��䲻��ȷ");
	        	return;
	        }
	        
	        memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	        memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	        
	        if (strcmpIgnore( pcKeyWord, "KEY" ))
	        {
	        	SHOW_ERRMSG("��䲻��ȷ");
		        return;
	        }
	        
	        iKeyFlag = 1;
	        
	        pcBufStart = pcBlank + 1;
	      }
	      else
	      /*����Ҫ����*/
	      if (IS_LETTER((*pcBufStart)))
	      {
	      }
	      else
	      {
		      SHOW_ERRMSG("��䲻��ȷ");
		      return;
	      }
	    }
	    else
	    {
	    	SHOW_ERRMSG("��䲻��ȷ");
		    return;
	    }
	  }
	  else
	  /*����Ƕ���*/
	  if ((*pcTmp) == ',')
	  {
	  	pcBlank = pcTmp;
	  	
	  	if (pcBlank - pcBufStart > KEY_WORD_LEN)
	    {
	    	SHOW_ERRMSG("��䲻��ȷ");
	    	return;
	    }
	    
	    memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	    memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	    
	    if (!strcmpIgnore( pcKeyWord, "KEY" ))
	    {
	    	iKeyFlag = 1;
	    	pcBufStart = pcBlank + 1;
	    	
	    	/*ȡ��һ���ַ�*/
	    	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	      if (pcBufStart == pcBufEnd)
	      {
		      SHOW_ERRMSG("��䲻��ȷ");
		      return;
	      }
	      
	      /*KEY����ֻ�ܸ�VALUE*/
	      {
	        pcBlank = strchr( pcBufStart, ' ' );
	        if (pcBlank == NULL || pcBlank > pcBufEnd)
	        {
	        	SHOW_ERRMSG("��䲻��ȷ");
	        	return;
	        }
	        
	        if (pcBlank - pcBufStart > KEY_WORD_LEN)
	        {
	        	SHOW_ERRMSG("��䲻��ȷ");
	        	return;
	        }
	        
	        memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	        memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	        
	        if (strcmpIgnore( pcKeyWord, "VALUE" ))
	        {
	        	SHOW_ERRMSG("��䲻��ȷ");
		        return;
	        }
	        
	        iValueFlag = 1;
	        
	        pcBufStart = pcBlank + 1;
	      }
	    }
	    else
	    if (!strcmpIgnore( pcKeyWord, "VALUE" ))
	    {
	    	iValueFlag = 1;
	    	
	    	pcBufStart = pcBlank + 1;
	    	
	    	/*ȡ��һ���ַ�*/
	    	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	      if (pcBufStart == pcBufEnd)
	      {
		      SHOW_ERRMSG("��䲻��ȷ");
		      return;
	      }
	      
	      /*VALUE����ֻ�ܸ�KEY*/
	      {
	        pcBlank = strchr( pcBufStart, ' ' );
	        if (pcBlank == NULL || pcBlank > pcBufEnd)
	        {
	        	SHOW_ERRMSG("��䲻��ȷ");
	        	return;
	        }
	        
	        if (pcBlank - pcBufStart > KEY_WORD_LEN)
	        {
	        	SHOW_ERRMSG("��䲻��ȷ");
	        	return;
	        }
	        
	        memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	        memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	        
	        if (strcmpIgnore( pcKeyWord, "KEY" ))
	        {
	        	SHOW_ERRMSG("��䲻��ȷ");
		        return;
	        }
	        
	        iKeyFlag = 1;
	        
	        pcBufStart = pcBlank + 1;
	      }
	    }
	    else
	    {
	    	SHOW_ERRMSG("��䲻��ȷ");
		    return;
	    }
	  }
	  else
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	    return;
	  }
  }
  
  memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
  if (iKeyFlag == 1 && iValueFlag == 1)
  {
  	memcpy( pcSendBuf, MANAGER_SELECT_KEY_VALUE_CNT, OPERATE_LEN );
  }
  else
  if (iKeyFlag == 1 && iValueFlag == 0)
  {
  	memcpy( pcSendBuf, MANAGER_SELECT_KEY_CNT, OPERATE_LEN );
  }
	else
  if (iKeyFlag == 0 && iValueFlag == 1)
  {
  	memcpy( pcSendBuf, MANAGER_SELECT_VALUE_CNT, OPERATE_LEN );
  }
  else
  {
  	SHOW_ERRMSG("��䲻��ȷ");
	  return;
  }
  
  /*���ó�ʼֵ��ѯID*/
  memcpy( pcSendBuf + OPERATE_LEN, NULL_SELECT_ID, INT64_LEN );
  
  /*���û�ȡ����*/
  memset( pcGetRows, 0x00, sizeof(pcGetRows) );
  sprintf( pcGetRows, "%0*d", INT64_LEN, SELECT_EACH_SIZE );
  memcpy( pcSendBuf + INT64_LEN + OPERATE_LEN, pcGetRows, INT64_LEN );
  
  /*���û�ȡ��ʼλ��*/
  memset( pcRowStartIndex, 0x00, sizeof(pcRowStartIndex) );
  sprintf( pcRowStartIndex, "%0*d", INT64_LEN, 0 );
  memcpy( pcSendBuf + OPERATE_LEN + INT64_LEN + INT64_LEN, pcRowStartIndex, INT64_LEN );
  
  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
  
  /*û������*/
	if (pcBufStart == pcBufEnd)
	{
	  memcpy( pcSendBuf + OPERATE_LEN + INT64_LEN + INT64_LEN + INT64_LEN, NO_WHERE_KEY_VALUE, OPERATE_LEN );
	}
	else
	{		
		/*ȡ�� where �ؼ���*/
	  {
	    pcBlank = strchr( pcBufStart, ' ' );
	    if (pcBlank == NULL)
	    {
	    	SHOW_ERRMSG("��䲻��ȷ");
	    	return;
	    }
	    
	    if (pcBlank - pcBufStart > KEY_WORD_LEN)
	    {
	    	SHOW_ERRMSG("��䲻��ȷ");
	    	return;
	    }
	    
	    memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	    memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	    
	    if (strcmpIgnore( pcKeyWord, "WHERE" ))
	    {
	    	SHOW_ERRMSG("��䲻��ȷ");
	  	  return;
	    }
	    
	    pcBufStart = pcBlank + 1;
    }
        
    /*ȡ�� key �� value�ؼ���*/
	  {
	  	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	    if (pcBufStart == pcBufEnd)
	    {
	  	  SHOW_ERRMSG("��䲻��ȷ");
	  	  return;
	    }
	    
	    /*Ѱ�ҵ�һ��������ĸ���ַ�*/
	    pcTmp = pcBufStart;
	    IS_NOT_LETTER( pcTmp, pcBufEnd );
	    if (pcTmp == pcBufEnd)
	    {
	  	  SHOW_ERRMSG("��䲻��ȷ");
	  	  return;
	    }
	    
	    /*����ǿո���߿����ַ�*/
	    if (isBlank(*pcTmp) || (*pcTmp) == '[' || (*pcTmp) == '(')
	    {
	    	pcBlank = pcTmp;
	    	
	    	if (pcBlank - pcBufStart > KEY_WORD_LEN)
	      {
	      	SHOW_ERRMSG("��䲻��ȷ");
	    	  return;
	      }
	      memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	      memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	    
	      if (!strcmpIgnore( pcKeyWord, "KEY" ))
	      {
	    	  iWhereFlag = 0;
	      }
	      else
	      if (!strcmpIgnore( pcKeyWord, "VALUE" ))
	      {
	    	  iWhereFlag = 1;
	      }
	      else
	      {
	    	  SHOW_ERRMSG("��䲻��ȷ");
	  	    return;
	      }
	    }
	    else
	    {
	    	SHOW_ERRMSG("��䲻��ȷ");
	  	  return;
	    }
	    
	    pcBufStart = pcTmp;
    }
        
    /*ָ�� key �� value ������ַ�*/
	  {
	  	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	    if (pcBufStart == pcBufEnd)
	    {
	  	  SHOW_ERRMSG("��䲻��ȷ");
	  	  return;
	    }
	    
	    /*������ǿ����ַ�*/
	    if (!((*pcBufStart) == '[' || (*pcBufStart) == '('))
	    {
	    	SHOW_ERRMSG("��䲻��ȷ");
	  	  return;
	    }
	  }
	  	  
	  cValueStart = *pcBufStart;
    pcBufStart++;
    
	  /*ȡ����һ��key��valueֵ*/
	  {
	    /*������һ��������*/
	    IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	    if (pcBufStart == pcBufEnd)
	    {
	      SHOW_ERRMSG("��䲻��ȷ");
	      return;
	    }
	    
	    /*������ǵ�����*/
	    if (!((*pcBufStart) == '\''))
	    {
	    	SHOW_ERRMSG("��䲻��ȷ");
	      return;
	    }
	    pcBufStart++;
	    
	    /*�����ڶ���������*/
	    pcTmp = strchr( pcBufStart, '\'' );
	    if (pcTmp == NULL || pcTmp >= pcBufEnd)
	    {
	      SHOW_ERRMSG("��䲻��ȷ");
	      return;
	    }
	    
	    if (iWhereFlag == 0)
	    {
	      /*ȡ����һ��keyֵ*/
	      if (pcTmp - pcBufStart > MAX_KEY_LEN)
	      {
	      	SHOW_ERRMSG("KEYֵ����");
	        return;
	      }
	      
	      memset( pcKeyStart, 0x00, sizeof(pcKeyStart) );
	      memcpy( pcKeyStart, pcBufStart, pcTmp - pcBufStart );
	    
	      iKeyStartLen = strlen( pcKeyStart );
	      memset( pcKeyStartLen, 0x00, sizeof(pcKeyStartLen) );
	      sprintf( pcKeyStartLen, "%0*d", KEY_VALUE_LENGTH, iKeyStartLen );
	    }
	    else
	    {
	    	/*ȡ����һ��keyֵ*/
	      if (pcTmp - pcBufStart > MAX_VALUE_LEN)
	      {
	      	SHOW_ERRMSG("VALUEֵ����");
	        return;
	      }
	      
	      memset( pcValueStart, 0x00, sizeof(pcValueStart) );
	      memcpy( pcValueStart, pcBufStart, pcTmp - pcBufStart );
	    
	      iValueStartLen = strlen( pcValueStart );
	      memset( pcValueStartLen, 0x00, sizeof(pcValueStartLen) );
	      sprintf( pcValueStartLen, "%0*d", KEY_VALUE_LENGTH, iValueStartLen );
	    }
	    pcBufStart = pcTmp + 1;
	  }
	  	  
	  /*ȡ���ڶ���keyֵ*/
	  {
	    /*��������*/
	    IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	    if (pcBufStart == pcBufEnd)
	    {
	      SHOW_ERRMSG("��䲻��ȷ");
	      return;
	    }
	    
	    /*������Ƕ���*/
	    if (!((*pcBufStart) == ','))
	    {
	    	SHOW_ERRMSG("��䲻��ȷ");
	      return;
	    }
	    pcBufStart++;
	    
	    /*����������������*/
	    IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	    if (pcBufStart == pcBufEnd)
	    {
	      SHOW_ERRMSG("��䲻��ȷ");
	      return;
	    }
	    
	    /*������ǵ�����*/
	    if (!((*pcBufStart) == '\''))
	    {
	    	SHOW_ERRMSG("��䲻��ȷ");
	      return;
	    }
	    pcBufStart++;
	    
	    /*�������ĸ�������*/
	    pcTmp = strchr( pcBufStart, '\'' );
	    if (pcTmp == NULL || pcTmp >= pcBufEnd)
	    {
	      SHOW_ERRMSG("��䲻��ȷ");
	      return;
	    }
	    
	    if (iWhereFlag == 0)
	    {
	      /*ȡ���ڶ���keyֵ*/
	      if (pcTmp - pcBufStart > MAX_KEY_LEN)
	      {
	      	SHOW_ERRMSG("KEYֵ����");
	        return;
	      }
	      memset( pcKeyEnd, 0x00, sizeof(pcKeyEnd) );
	      memcpy( pcKeyEnd, pcBufStart, pcTmp - pcBufStart );
	      
	      iKeyEndLen = strlen( pcKeyEnd );
	      memset( pcKeyEndLen, 0x00, sizeof(pcKeyEndLen) );
	      sprintf( pcKeyEndLen, "%0*d", KEY_VALUE_LENGTH, iKeyEndLen );
	    }
	    else
	    {
	    	/*ȡ���ڶ���valueֵ*/
	      if (pcTmp - pcBufStart > MAX_VALUE_LEN)
	      {
	      	SHOW_ERRMSG("VALUEֵ����");
	        return;
	      }
	      memset( pcValueEnd, 0x00, sizeof(pcValueEnd) );
	      memcpy( pcValueEnd, pcBufStart, pcTmp - pcBufStart );
	      
	      iValueEndLen = strlen( pcValueEnd );
	      memset( pcValueEndLen, 0x00, sizeof(pcValueEndLen) );
	      sprintf( pcValueEndLen, "%0*d", KEY_VALUE_LENGTH, iValueEndLen );
	    }
	    pcBufStart = pcTmp + 1;
	  }
	  	  
	  /*ȡ�����Ŀ��շ�*/
    IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if ((*pcBufStart) == ']' || (*pcBufStart) == ')')
	  {
	  	cValueEnd = (*pcBufStart);
	  }
	  else
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	    return;
	  }
	  
	  /*ȷ��ֵ����*/
	  memset( pcValueType, 0x00, sizeof(pcValueType) );
	  if (iWhereFlag == 0)
	  {
	    if (cValueStart == '(' && cValueEnd == ')')
	    {
	    	memcpy( pcValueType, KEY_OPEN_OPEN, OPERATE_LEN );
	    }
	    else
	    if (cValueStart == '(' && cValueEnd == ']')
	    {
	    	memcpy( pcValueType, KEY_OPEN_CLOSE, OPERATE_LEN );
	    }
	    else
	    if (cValueStart == '[' && cValueEnd == ')')
	    {
	    	memcpy( pcValueType, KEY_CLOSE_OPEN, OPERATE_LEN );
	    }
	    else
	    {
	    	memcpy( pcValueType, KEY_CLOSE_CLOSE, OPERATE_LEN );
	    }
	  }
	  else
	  {
	  	if (cValueStart == '(' && cValueEnd == ')')
	    {
	    	memcpy( pcValueType, VALUE_OPEN_OPEN, OPERATE_LEN );
	    }
	    else
	    if (cValueStart == '(' && cValueEnd == ']')
	    {
	    	memcpy( pcValueType, VALUE_OPEN_CLOSE, OPERATE_LEN );
	    }
	    else
	    if (cValueStart == '[' && cValueEnd == ')')
	    {
	    	memcpy( pcValueType, VALUE_CLOSE_OPEN, OPERATE_LEN );
	    }
	    else
	    {
	    	memcpy( pcValueType, VALUE_CLOSE_CLOSE, OPERATE_LEN );
	    }
	  }
	  	  
	  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart != pcBufEnd)
	  {
	    SHOW_ERRMSG("��䲻��ȷ");
	    return;
	  }
	  
	  if (iWhereFlag == 0)
	  {
	    memcpy( pcSendBuf + OPERATE_LEN + INT64_LEN + INT64_LEN + INT64_LEN, pcValueType, OPERATE_LEN );
	    memcpy( pcSendBuf + OPERATE_LEN + INT64_LEN + INT64_LEN + INT64_LEN + OPERATE_LEN, pcKeyStartLen, KEY_VALUE_LENGTH );
	    memcpy( pcSendBuf + OPERATE_LEN + INT64_LEN + INT64_LEN + INT64_LEN + OPERATE_LEN + KEY_VALUE_LENGTH, pcKeyStart, iKeyStartLen );
  	  memcpy( pcSendBuf + OPERATE_LEN + INT64_LEN + INT64_LEN + INT64_LEN + OPERATE_LEN + KEY_VALUE_LENGTH + iKeyStartLen, pcKeyEndLen, KEY_VALUE_LENGTH );
	    memcpy( pcSendBuf + OPERATE_LEN + INT64_LEN + INT64_LEN + INT64_LEN + OPERATE_LEN + KEY_VALUE_LENGTH + iKeyStartLen + KEY_VALUE_LENGTH, pcKeyEnd, iKeyEndLen );
    }
    else
    {
    	memcpy( pcSendBuf + OPERATE_LEN + INT64_LEN + INT64_LEN + INT64_LEN, pcValueType, OPERATE_LEN );
	    memcpy( pcSendBuf + OPERATE_LEN + INT64_LEN + INT64_LEN + INT64_LEN + OPERATE_LEN, pcValueStartLen, KEY_VALUE_LENGTH );
	    memcpy( pcSendBuf + OPERATE_LEN + INT64_LEN + INT64_LEN + INT64_LEN + OPERATE_LEN + KEY_VALUE_LENGTH, pcValueStart, iValueStartLen );
  	  memcpy( pcSendBuf + OPERATE_LEN + INT64_LEN + INT64_LEN + INT64_LEN + OPERATE_LEN + KEY_VALUE_LENGTH + iValueStartLen, pcValueEndLen, KEY_VALUE_LENGTH );
	    memcpy( pcSendBuf + OPERATE_LEN + INT64_LEN + INT64_LEN + INT64_LEN + OPERATE_LEN + KEY_VALUE_LENGTH + iValueStartLen + KEY_VALUE_LENGTH, pcValueEnd, iValueEndLen );
    }
	}
	
	iSendLen = strlen(pcSendBuf);
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );

	iSocket = connectToServer( pcgIp, pcgPort );
	if (iSocket == -1)
	{
		SHOW_ERRMSG("���ӵ������ʧ��");
		return;
	}
	
	/*���ͳ�����*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	/*��������*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	SHOW_ERRMSG("�������ݵ�����˳ɹ��������ĵȴ�����˴���");
	
	/*�ȴ�����*/
	iRet = waitFd( iSocket, WAIT_IN, NO_TIME_OUT );
	
	/*����������*/
  if (iRet == -1)
	{
		SHOW_ERRMSG("�ȴ���������ʧ��");
	  return;
	}
	
	/*���ճ���*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*��������*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("���շ��������ʧ��");
		return;
	}
	
	fprintf( stdout, "result:\n" );
	logFmtSyn( LVNOR, "result:" );
	
	if (cResult == '0')
	{
		/*���ղ�ѯID*/
		MANAGER_RECV( iSocket, iRet, pcSelectId, iRecvLen, INT64_LEN );
		
		/*���յ�ǰ��������*/
	  MANAGER_RECV( iSocket, iRet, pcRows, iRecvLen, INT64_LEN );
	  
	  /*��ʾ����*/
	  fprintf( stdout, "rows :%lld  start index :%lld  end index :%lld \n", 
	    atoll( pcRows ), 0, atoll( pcRows ) );
		logFmtSyn( LVNOR, "rows :%lld  start index :%lld  end index :%lld ", 
		  atoll( pcRows ), 0, atoll( pcRows ) );
		
		/*���ղ���ʾ����*/
		for (iTmp = 0; iTmp < atoll( pcRows ); iTmp++)
	  {
	  	if (iKeyFlag == 1)
	  	{
	  	  /*���� KEY ����*/
	  	  MANAGER_RECV( iSocket, iRet, pcKeyValueLen, iRecvLen, KEY_VALUE_LENGTH );
	      iKeyValueLen = atoi( pcKeyValueLen );
	      
	      /*���� KEY */
	      MANAGER_RECV( iSocket, iRet, pcKey, iRecvLen, iKeyValueLen );
	    }
	    
	    if (iValueFlag == 1)
	  	{
	  	  /*���� VALUE ����*/
	  	  MANAGER_RECV( iSocket, iRet, pcKeyValueLen, iRecvLen, KEY_VALUE_LENGTH );
	      iKeyValueLen = atoi( pcKeyValueLen );
	      
	      /*���� VALUE */
	      MANAGER_RECV( iSocket, iRet, pcValue, iRecvLen, iKeyValueLen );
	    }
	    
	    /*��ʾ����*/
	    if (iKeyFlag == 1)
	    {
	    	fprintf( stdout, "key :%s\t", pcKey );
	    	logFmtSyn( LVNOR, "key :%s", pcKey );
	    }
	    if (iValueFlag == 1)
	    {
	    	fprintf( stdout, "value :%s", pcValue );
	    	logFmtSyn( LVNOR, "value :%s", pcValue );
	    }
	    fprintf( stdout, "\n" );
	  }
		
		/*���õ�ǰ��ʼλ��*/
		llRowStartIndex = 0;
		
		/*�����û����룬ȷ����һ������*/
		while (1)
		{
			cGet = getch();
			//fprintf( stdout, "%c\n", cGet );
			/*���*/
			if (cGet == 'f' )
			{
				/*�鷢������*//*����ѯ����Ϊ0ʱ����ʾ���SELECT ID��Ӧ�Ļ�����*/
			  memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
			  
			  memset( pcGetRows, 0x00, sizeof(pcGetRows) );
			  memset( pcGetRows, '0', INT64_LEN );
			  
			  memcpy( pcSendBuf, pcSelectId, INT64_LEN );
			  memcpy( pcSendBuf + INT64_LEN, pcGetRows, INT64_LEN );
			  
			  iSendLen = strlen(pcSendBuf);
	      memset( pcSendLen, 0x00, sizeof(pcSendLen) );
	      sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
	      
	      iSocket = connectToServer( pcgIp, pcgPort );
	      if (iSocket == -1)
	      {
	      	SHOW_ERRMSG("���ӵ������ʧ��");
	      	return;
	      }
	      
	      /*���ͳ�����*/
	      iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	      if (iRet != DATA_BUF_LEN)
	      {
	      	SHOW_ERRMSG("�������ݵ������ʧ��");
	      	return;
	      }

	      /*��������*/
	      iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	      /*��Ϊ���÷�����������ݣ�����û��Ҫ�жϳɹ�����ʧ��*/
	      /*
	      if (iRet != iSendLen)
	      {
	      	SHOW_ERRMSG("�������ݵ������ʧ��");
	      	return;
	      }
	      */
				closesocket( iSocket );
				
				SHOW_ERRMSG("��ѯ���");
				return;
			}
			else
			/*ȡ��һ��*/
			if (cGet == 'n')
			{
				/*���û�ȡ����*/
				memset( pcGetRows, 0x00, sizeof(pcGetRows) );
				sprintf( pcGetRows, "%0*lld", INT64_LEN, SELECT_EACH_SIZE );
				
				/*���û�ȡ��ʼλ��*/
				llRowStartIndex += atoll(pcRows);
				memset( pcRowStartIndex, 0x00, sizeof(pcRowStartIndex) );
				sprintf( pcRowStartIndex, "%0*lld", INT64_LEN, llRowStartIndex );
			}
			else
			/*ȡ��һ��*/
			if (cGet == 'l' )
			{
				/*���û��������*/
				if (llRowStartIndex == 0)
				{
					continue;
				}
				else
				{
					/*���û�ȡ����*/
				  memset( pcGetRows, 0x00, sizeof(pcGetRows) );
				  sprintf( pcGetRows, "%0*lld", INT64_LEN, SELECT_EACH_SIZE );
				
				  /*���û�ȡ��ʼλ��*/
				  //llRowStartIndex -= atoll(pcRows);
				  if (atoll(pcRows) == 0)
				  {
				    llRowStartIndex -= SELECT_EACH_SIZE;
				  }
				  else
				  {
				  	llRowStartIndex -= atoll(pcRows);
				  }
				  memset( pcRowStartIndex, 0x00, sizeof(pcRowStartIndex) );
				  sprintf( pcRowStartIndex, "%0*lld", INT64_LEN, llRowStartIndex );
				}
			}
			else
			{
				continue;
			}
			
			/*�鷢������*/
			memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
			
			if (iKeyFlag == 1 && iValueFlag == 1)
      {
      	memcpy( pcSendBuf, MANAGER_SELECT_KEY_VALUE_CNT, OPERATE_LEN );
      }
      else
      if (iKeyFlag == 1 && iValueFlag == 0)
      {
      	memcpy( pcSendBuf, MANAGER_SELECT_VALUE_CNT, OPERATE_LEN );
      }
	    else
      /*if (iKeyFlag == 1 && iValueFlag == 0)*/
      {
      	memcpy( pcSendBuf, MANAGER_SELECT_KEY_CNT, OPERATE_LEN );
      }
			
			memcpy( pcSendBuf + OPERATE_LEN, pcSelectId, INT64_LEN );
			memcpy( pcSendBuf + OPERATE_LEN + INT64_LEN, pcGetRows, INT64_LEN );
			memcpy( pcSendBuf + OPERATE_LEN + INT64_LEN + INT64_LEN, pcRowStartIndex, INT64_LEN );
			
			iSendLen = strlen(pcSendBuf);
	    memset( pcSendLen, 0x00, sizeof(pcSendLen) );
	    sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
	    
	    iSocket = connectToServer( pcgIp, pcgPort );
	    if (iSocket == -1)
	    {
	    	SHOW_ERRMSG("���ӵ������ʧ��");
	    	return;
	    }
	    
	    /*���ͳ�����*/
	    iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	    if (iRet != DATA_BUF_LEN)
	    {
	    	SHOW_ERRMSG("�������ݵ������ʧ��");
	    	return;
	    }
	    
	    /*��������*/
	    iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	    if (iRet != iSendLen)
	    {
	    	SHOW_ERRMSG("�������ݵ������ʧ��");
	    	return;
	    }
	    
	    SHOW_ERRMSG("�������ݵ�����˳ɹ��������ĵȴ�����˴���");
	
	    /*�ȴ�����*/
	    iRet = waitFd( iSocket, WAIT_IN, NO_TIME_OUT );
	    
	    /*����������*/
      if (iRet == -1)
	    {
	    	SHOW_ERRMSG("�ȴ���������ʧ��");
	      return;
	    }
	    
	    /*���ճ���*/
	    MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	    
	    /*��������*/
	    cResult = 0;
      iRet = recv( iSocket, &cResult, 1, 0 );
      if (iRet <= 0)
	    {
	    	SHOW_ERRMSG("���շ��������ʧ��");
	    	return;
	    }
	    
	    fprintf( stdout, "result:\n" );
	    logFmtSyn( LVNOR, "result:" );
	
	    if (cResult == '0')
	    {
	    	/*���ղ�ѯID*/
		    MANAGER_RECV( iSocket, iRet, pcSelectId, iRecvLen, INT64_LEN );
	      
	      /*���յ�ǰ��������*/
	      MANAGER_RECV( iSocket, iRet, pcRows, iRecvLen, INT64_LEN );
	      
	      /*��ʾ����*/
	      fprintf( stdout, "rows :%lld  start index :%lld  end index :%lld \n", 
	        atoll( pcRows ), llRowStartIndex, llRowStartIndex + atoll( pcRows ) );
	    	logFmtSyn( LVNOR, "rows :%lld  start index :%lld  end index :%lld ", 
	    	  atoll( pcRows ), llRowStartIndex, llRowStartIndex + atoll( pcRows ) );
	    	
	    	/*���ղ���ʾ����*/
	    	for (iTmp = 0; iTmp < atoll( pcRows ); iTmp++)
	      {
	      	if (iKeyFlag == 1)
	      	{
	      	  /*���� KEY ����*/
	      	  MANAGER_RECV( iSocket, iRet, pcKeyValueLen, iRecvLen, KEY_VALUE_LENGTH );
	          iKeyValueLen = atoi( pcKeyValueLen );
	          
	          /*���� KEY */
	          MANAGER_RECV( iSocket, iRet, pcKey, iRecvLen, iKeyValueLen );
	        }
	        
	        if (iValueFlag == 1)
	      	{
	      	  /*���� VALUE ����*/
	      	  MANAGER_RECV( iSocket, iRet, pcKeyValueLen, iRecvLen, KEY_VALUE_LENGTH );
	          iKeyValueLen = atoi( pcKeyValueLen );
	          
	          /*���� VALUE */
	          MANAGER_RECV( iSocket, iRet, pcValue, iRecvLen, iKeyValueLen );
	        }
	        
	        /*��ʾ����*/
	        if (iKeyFlag == 1)
	        {
	        	fprintf( stdout, "key :%s\t", pcKey );
	        	logFmtSyn( LVNOR, "key :%s", pcKey );
	        }
	        if (iValueFlag == 1)
	        {
	        	fprintf( stdout, "value :%s", pcValue );
	        	logFmtSyn( LVNOR, "value :%s", pcValue );
	        }
	        fprintf( stdout, "\n" );
	      }
	    }
	    else
	    {
	    	fprintf( stdout, "fail\n" );
	    	logFmtSyn( LVNOR, "fail" );
	    }
	    
	    closesocket( iSocket );
		}
	}
	else
	{
		fprintf( stdout, "fail\n" );
		logFmtSyn( LVNOR, "fail" );
	}
}

/**********************************************************************
��������: managerDeleteDeal
��������: ����˴��� delete ����������
��    ����
��    һ�����ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20161018
**********************************************************************/
void managerDeleteDeal( char *pcaBufStart, char *pcaBufEnd )
{
/*
�������ݽṹ������+����+ֵ���䣨���ҿ������ұա�����ҿ�������ұգ�+KEY��ʼֵ����+KEY��ʼֵ+KEY����ֵ����+KEY����ֵ+�û���+����
�������ݽṹ���������+ɾ������������
���͹�ȥ�����ݣ���������ո񣬷���˴������־���ܻ��١�ͳһ������ǰ��0���ַ��󲹿ո񣬷����У���ַ��Ƿ�һ��ʱ���ú��Կո�
*/
  char pcKeyWord[KEY_WORD_LEN];
	char *pcBufStart;
	char *pcBufEnd;
	char *pcBlank;
	char *pcTmp;
  char pcKeyStartLen[KEY_VALUE_LENGTH + 1];
  char pcKeyEndLen[KEY_VALUE_LENGTH + 1];
  char pcKeyStart[MAX_KEY_LEN + 1];
  char pcKeyEnd[MAX_KEY_LEN + 1];
  char pcValueType[KEY_VALUE_LENGTH + 1];
  char pcUser[NAME_LEN + 1];
  char pcPass[NAME_LEN + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	char pcSendBuf[OPERATE_LEN + OPERATE_LEN + KEY_VALUE_LENGTH + MAX_KEY_LEN + KEY_VALUE_LENGTH + MAX_KEY_LEN + NAME_LEN + NAME_LEN + 1];
	char cValueStart;
	char cValueEnd;
	char cResult;
	char pcDeleteTotal[INT64_LEN + 1];
	int  iKeyStartLen;
	int  iKeyEndLen;
	int  iSendLen;
	int  iRecvLen;
	int  iSocket;
	int  iDataLen;
	int  iRet;
	int  iTimeStart;
	int  iTimeEnd;
	pthread_t id;
	
	logFmtSyn( LVSYS, "��ʼִ�к�����%s", __FUNCTION__ );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
  
  /*ȡ�� where �ؼ���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "WHERE" ))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ�� key �ؼ���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  /*Ѱ�ҵ�һ��������ĸ���ַ�*/
	  pcTmp = pcBufStart;
	  IS_NOT_LETTER( pcTmp, pcBufEnd );
	  if (pcTmp == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  /*����ǿո���߿����ַ�*/
	  if (isBlank(*pcTmp) || (*pcTmp) == '[' || (*pcTmp) == '(')
	  {
	  	pcBlank = pcTmp;
	  	
	  	if (pcBlank - pcBufStart > KEY_WORD_LEN)
	    {
	    	SHOW_ERRMSG("��䲻��ȷ");
	  	  return;
	    }
	    memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	    memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	    if (strcmpIgnore( pcKeyWord, "KEY" ))
	    {
	  	  SHOW_ERRMSG("��䲻��ȷ");
		    return;
	    }
	  }
	  else
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBufStart = pcTmp;
  }
  
  logFmtSyn( LVSYS, "���� KEY �ؼ��ֳɹ�" );
  
  /*ָ�� key ������ַ�*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  /*������ǿ����ַ�*/
	  if (!((*pcBufStart) == '[' || (*pcBufStart) == '('))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	}
	
	cValueStart = *pcBufStart;
  pcBufStart++;
  
	/*ȡ����һ��keyֵ*/
	{
	  /*������һ��������*/
	  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
	    SHOW_ERRMSG("��䲻��ȷ");
	    return;
	  }
	  
	  /*������ǵ�����*/
	  if (!((*pcBufStart) == '\''))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	    return;
	  }
	  pcBufStart++;
	  
	  /*�����ڶ���������*/
	  pcTmp = strchr( pcBufStart, '\'' );
	  if (pcTmp == NULL || pcTmp >= pcBufEnd)
	  {
	    SHOW_ERRMSG("��䲻��ȷ");
	    return;
	  }
	  
	  /*ȡ����һ��keyֵ*/
	  if (pcTmp - pcBufStart > MAX_KEY_LEN)
	  {
	  	SHOW_ERRMSG("KEYֵ����");
	    return;
	  }
	  memset( pcKeyStart, 0x00, sizeof(pcKeyStart) );
	  memcpy( pcKeyStart, pcBufStart, pcTmp - pcBufStart );
	  
	  iKeyStartLen = strlen( pcKeyStart );
	  memset( pcKeyStartLen, 0x00, sizeof(pcKeyStartLen) );
	  sprintf( pcKeyStartLen, "%0*d", KEY_VALUE_LENGTH, iKeyStartLen );
	  
	  pcBufStart = pcTmp + 1;
	}
	
	logFmtSyn( LVSYS, "pcKeyStart[%s]", pcKeyStart );
	
	/*ȡ���ڶ���keyֵ*/
	{
	  /*��������*/
	  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
	    SHOW_ERRMSG("��䲻��ȷ");
	    return;
	  }
	  
	  /*������Ƕ���*/
	  if (!((*pcBufStart) == ','))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	    return;
	  }
	  pcBufStart++;
	  
	  /*����������������*/
	  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
	    SHOW_ERRMSG("��䲻��ȷ");
	    return;
	  }
	  
	  /*������ǵ�����*/
	  if (!((*pcBufStart) == '\''))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	    return;
	  }
	  pcBufStart++;
	  
	  /*�������ĸ�������*/
	  pcTmp = strchr( pcBufStart, '\'' );
	  if (pcTmp == NULL || pcTmp >= pcBufEnd)
	  {
	    SHOW_ERRMSG("��䲻��ȷ");
	    return;
	  }
	  
	  /*ȡ���ڶ���keyֵ*/
	  if (pcTmp - pcBufStart > MAX_KEY_LEN)
	  {
	  	SHOW_ERRMSG("KEYֵ����");
	    return;
	  }
	  memset( pcKeyEnd, 0x00, sizeof(pcKeyEnd) );
	  memcpy( pcKeyEnd, pcBufStart, pcTmp - pcBufStart );
	  
	  iKeyEndLen = strlen( pcKeyEnd );
	  memset( pcKeyEndLen, 0x00, sizeof(pcKeyEndLen) );
	  sprintf( pcKeyEndLen, "%0*d", KEY_VALUE_LENGTH, iKeyEndLen );
	  
	  pcBufStart = pcTmp + 1;
	}
	
	logFmtSyn( LVSYS, "pcKeyEnd[%s]", pcKeyEnd );
	
	/*ȡ�����Ŀ��շ�*/
  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if ((*pcBufStart) == ']' || (*pcBufStart) == ')')
	{
		cValueEnd = (*pcBufStart);
	}
	else
	{
		SHOW_ERRMSG("��䲻��ȷ");
	  return;
	}
	
	pcBufStart++;
	
	/*ȷ��ֵ����*/
	memset( pcValueType, 0x00, sizeof(pcValueType) );
	if (cValueStart == '(' && cValueEnd == ')')
	{
		if (strcmp( pcKeyStart, pcKeyEnd ) >= 0)
		{
			SHOW_ERRMSG("��һ��ֵֻ����С�ڵڶ���ֵ");
	    return;
		}
		memcpy( pcValueType, KEY_OPEN_OPEN, OPERATE_LEN );
	}
	else
	if (cValueStart == '(' && cValueEnd == ']')
	{
		if (strcmp( pcKeyStart, pcKeyEnd ) > 0)
		{
			SHOW_ERRMSG("��һ��ֵֻ����С�ڻ���ڵڶ���ֵ");
	    return;
		}
		memcpy( pcValueType, KEY_OPEN_CLOSE, OPERATE_LEN );
	}
	else
	if (cValueStart == '[' && cValueEnd == ')')
	{
		if (strcmp( pcKeyStart, pcKeyEnd ) >= 0)
		{
			SHOW_ERRMSG("��һ��ֵֻ����С�ڵڶ���ֵ");
	    return;
		}
		memcpy( pcValueType, KEY_CLOSE_OPEN, OPERATE_LEN );
	}
	else
	{
		if (strcmp( pcKeyStart, pcKeyEnd ) > 0)
		{
			SHOW_ERRMSG("��һ��ֵֻ����С�ڻ���ڵڶ���ֵ");
	    return;
		}
		memcpy( pcValueType, KEY_CLOSE_CLOSE, OPERATE_LEN );
	}
	
	/*ȡ�� using �ؼ���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );

	  if (strcmpIgnore( pcKeyWord, "USING" ))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ���û���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > NAME_LEN)
	  {
	  	SHOW_ERRMSG("�û��������Ϲ淶");
	  	return;
	  }
	  
	  memset( pcUser, 0x00, sizeof(pcUser) );
	  memcpy( pcUser, pcBufStart, pcBlank - pcBufStart );
	  
	  /*���û����󲹿ո�*/
	  iDataLen = strlen( pcUser );
	  memset( pcUser + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ�� by �ؼ���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "BY" ))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ������*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	pcBlank = pcBufEnd;
	  }
	  else
	  {
	  	pcBlank = pcBlank - 1;
	  }
	  
	  if (pcBlank - pcBufStart + 1 > NAME_LEN)
	  {
	  	SHOW_ERRMSG("���벻���Ϲ淶");
	  	return;
	  }
	  
	  memset( pcPass, 0x00, sizeof(pcPass) );
	  memcpy( pcPass, pcBufStart, pcBlank - pcBufStart + 1 );
	  
	  /*������󲹿ո�*/
	  iDataLen = strlen( pcPass );
	  memset( pcPass + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank;
  }
  
  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if (pcBufStart != pcBufEnd)
	{
	  SHOW_ERRMSG("��䲻��ȷ");
	  return;
	}
	
	memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
	
	memcpy( pcSendBuf, MANAGER_DELETE_CNT, OPERATE_LEN );
	memcpy( pcSendBuf + OPERATE_LEN, pcValueType, OPERATE_LEN );
	memcpy( pcSendBuf + OPERATE_LEN + OPERATE_LEN, pcKeyStartLen, KEY_VALUE_LENGTH );
	memcpy( pcSendBuf + OPERATE_LEN + OPERATE_LEN + KEY_VALUE_LENGTH, pcKeyStart, iKeyStartLen );
	memcpy( pcSendBuf + OPERATE_LEN + OPERATE_LEN + KEY_VALUE_LENGTH + iKeyStartLen, pcKeyEndLen, KEY_VALUE_LENGTH );
	memcpy( pcSendBuf + OPERATE_LEN + OPERATE_LEN + KEY_VALUE_LENGTH + iKeyStartLen + KEY_VALUE_LENGTH, pcKeyEnd, iKeyEndLen );
	memcpy( pcSendBuf + OPERATE_LEN + OPERATE_LEN + KEY_VALUE_LENGTH + iKeyStartLen + KEY_VALUE_LENGTH + iKeyEndLen, pcUser, NAME_LEN );
	memcpy( pcSendBuf + OPERATE_LEN + OPERATE_LEN + KEY_VALUE_LENGTH + iKeyStartLen + KEY_VALUE_LENGTH + iKeyEndLen + NAME_LEN, pcPass, NAME_LEN );
	
	iSendLen = OPERATE_LEN + OPERATE_LEN + KEY_VALUE_LENGTH + iKeyStartLen + KEY_VALUE_LENGTH + iKeyEndLen + NAME_LEN + NAME_LEN;
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
	
	iSocket = connectToServer( pcgIp, pcgPort );
	if (iSocket == -1)
	{
		SHOW_ERRMSG("���ӵ������ʧ��");
		return;
	}

	/*���ͳ�����*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	/*��������*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	iTimeStart = clock();
	fprintf( stderr, "delete start:%d\n", iTimeStart );
	iRet = waitFd( iSocket, WAIT_IN, NO_TIME_OUT );
	if (iRet == -1)
	{
		SHOW_ERRMSG("�ȴ����շ��������ʧ��");
		return;
	}
	
	/*��������*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("���շ��������ʧ��");
		return;
	}
	
	fprintf( stdout, "result:\n" );
	logFmtSyn( LVNOR, "result:" );
	
	iTimeEnd = clock();
	fprintf( stderr, "delete   end:%d\n", iTimeEnd );
	fprintf( stderr, "delete  time:%d\n", iTimeEnd - iTimeStart );
	
	if (cResult == '0')
	{
		/*���յ������ݵ�������*/
		MANAGER_RECV( iSocket, iRet, pcDeleteTotal, iRecvLen, INT64_LEN );
	  
	  fprintf( stdout, "delete size :%lld\n", atoll( pcDeleteTotal ) );
	  
		logFmtSyn( LVNOR, "delete size :%lld\n", atoll( pcDeleteTotal ) );
	}
	else
	{
		fprintf( stdout, "fail\n" );
		logFmtSyn( LVNOR, "fail" );
	}
	
	return;
}

/**********************************************************************
��������: managerUnloadDeal
��������: ����˴��� unload ����������
��    ����
��    һ�����ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20161018
**********************************************************************/
void managerUnloadDeal( char *pcaBufStart, char *pcaBufEnd )
{
/*
�������ݽṹ������+����+�ļ�����+ֵ���䣨���ҿ������ұա�����ҿ�������ұգ�+KEY��ʼֵ����+KEY��ʼֵ+KEY����ֵ����+KEY����ֵ
�������ݽṹ���������+��������������
���͹�ȥ�����ݣ���������ո񣬷���˴������־���ܻ��١�ͳһ������ǰ��0���ַ��󲹿ո񣬷����У���ַ��Ƿ�һ��ʱ���ú��Կո�
*/
  char pcKeyWord[KEY_WORD_LEN];
	char *pcBufStart;
	char *pcBufEnd;
	char *pcBlank;
	char *pcTmp;
  char pcFilePath[FILE_PATH_LEN + 1];
  char pcKeyStartLen[KEY_VALUE_LENGTH + 1];
  char pcKeyEndLen[KEY_VALUE_LENGTH + 1];
  char pcKeyStart[MAX_KEY_LEN + 1];
  char pcKeyEnd[MAX_KEY_LEN + 1];
  char pcValueType[KEY_VALUE_LENGTH + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	char pcSendBuf[OPERATE_LEN + FILE_PATH_LEN + OPERATE_LEN + KEY_VALUE_LENGTH + MAX_KEY_LEN + KEY_VALUE_LENGTH + MAX_KEY_LEN + 1];
	char pcRecvLen[DATA_BUF_LEN + 1];
	char cValueStart;
	char cValueEnd;
	char cResult;
	char pcUnloadTotal[INT64_LEN + 1];
	int  iKeyStartLen;
	int  iKeyEndLen;
	int  iSendLen;
	int  iRecvLen;
	int  iSocket;
	int  iDataLen;
	int  iRet;
	
	logFmtSyn( LVSYS, "��ʼִ�к�����%s", __FUNCTION__ );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
	/*ȡ�� to �ؼ���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "TO" ))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
	
	/*ȡ���ļ�����*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > FILE_PATH_LEN)
	  {
	  	SHOW_ERRMSG("�ļ����Ʋ����Ϲ淶");
	  	return;
	  }
	  
	  memset( pcFilePath, 0x00, sizeof(pcFilePath) );
	  memcpy( pcFilePath, pcBufStart, pcBlank - pcBufStart );
	  
	  /*���ļ����ƺ󲹿ո�*/
	  iDataLen = strlen( pcFilePath );
	  memset( pcFilePath + iDataLen, ' ', FILE_PATH_LEN - iDataLen );
	  	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ�� where �ؼ���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "WHERE" ))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ�� key �ؼ���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  /*Ѱ�ҵ�һ��������ĸ���ַ�*/
	  pcTmp = pcBufStart;
	  IS_NOT_LETTER( pcTmp, pcBufEnd );
	  if (pcTmp == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  /*����ǿո���߿����ַ�*/
	  if (isBlank(*pcTmp) || (*pcTmp) == '[' || (*pcTmp) == '(')
	  {
	  	pcBlank = pcTmp;
	  	
	  	if (pcBlank - pcBufStart > KEY_WORD_LEN)
	    {
	    	SHOW_ERRMSG("��䲻��ȷ");
	  	  return;
	    }
	    memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	    memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	    if (strcmpIgnore( pcKeyWord, "KEY" ))
	    {
	  	  SHOW_ERRMSG("��䲻��ȷ");
		    return;
	    }
	  }
	  else
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBufStart = pcTmp;
  }
  
  logFmtSyn( LVSYS, "���� KEY �ؼ��ֳɹ�" );
  
  /*ָ�� key ������ַ�*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  /*������ǿ����ַ�*/
	  if (!((*pcBufStart) == '[' || (*pcBufStart) == '('))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	}
	
	cValueStart = *pcBufStart;
  pcBufStart++;
  
	/*ȡ����һ��keyֵ*/
	{
	  /*������һ��������*/
	  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
	    SHOW_ERRMSG("��䲻��ȷ");
	    return;
	  }
	  
	  /*������ǵ�����*/
	  if (!((*pcBufStart) == '\''))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	    return;
	  }
	  pcBufStart++;
	  
	  /*�����ڶ���������*/
	  pcTmp = strchr( pcBufStart, '\'' );
	  if (pcTmp == NULL || pcTmp >= pcBufEnd)
	  {
	    SHOW_ERRMSG("��䲻��ȷ");
	    return;
	  }
	  
	  /*ȡ����һ��keyֵ*/
	  if (pcTmp - pcBufStart > MAX_KEY_LEN)
	  {
	  	SHOW_ERRMSG("KEYֵ����");
	    return;
	  }
	  memset( pcKeyStart, 0x00, sizeof(pcKeyStart) );
	  memcpy( pcKeyStart, pcBufStart, pcTmp - pcBufStart );
	  
	  iKeyStartLen = strlen( pcKeyStart );
	  memset( pcKeyStartLen, 0x00, sizeof(pcKeyStartLen) );
	  sprintf( pcKeyStartLen, "%0*d", KEY_VALUE_LENGTH, iKeyStartLen );
	  
	  pcBufStart = pcTmp + 1;
	}
	
	logFmtSyn( LVSYS, "pcKeyStart[%s]", pcKeyStart );
	
	/*ȡ���ڶ���keyֵ*/
	{
	  /*��������*/
	  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
	    SHOW_ERRMSG("��䲻��ȷ");
	    return;
	  }
	  
	  /*������Ƕ���*/
	  if (!((*pcBufStart) == ','))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	    return;
	  }
	  pcBufStart++;
	  
	  /*����������������*/
	  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
	    SHOW_ERRMSG("��䲻��ȷ");
	    return;
	  }
	  
	  /*������ǵ�����*/
	  if (!((*pcBufStart) == '\''))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	    return;
	  }
	  pcBufStart++;
	  
	  /*�������ĸ�������*/
	  pcTmp = strchr( pcBufStart, '\'' );
	  if (pcTmp == NULL || pcTmp >= pcBufEnd)
	  {
	    SHOW_ERRMSG("��䲻��ȷ");
	    return;
	  }
	  
	  /*ȡ���ڶ���keyֵ*/
	  if (pcTmp - pcBufStart > MAX_KEY_LEN)
	  {
	  	SHOW_ERRMSG("KEYֵ����");
	    return;
	  }
	  memset( pcKeyEnd, 0x00, sizeof(pcKeyEnd) );
	  memcpy( pcKeyEnd, pcBufStart, pcTmp - pcBufStart );
	  
	  iKeyEndLen = strlen( pcKeyEnd );
	  memset( pcKeyEndLen, 0x00, sizeof(pcKeyEndLen) );
	  sprintf( pcKeyEndLen, "%0*d", KEY_VALUE_LENGTH, iKeyEndLen );
	  
	  pcBufStart = pcTmp + 1;
	}
	
	logFmtSyn( LVSYS, "pcKeyEnd[%s]", pcKeyEnd );
	
	/*ȡ�����Ŀ��շ�*/
  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if ((*pcBufStart) == ']' || (*pcBufStart) == ')')
	{
		cValueEnd = (*pcBufStart);
	}
	else
	{
		SHOW_ERRMSG("��䲻��ȷ");
	  return;
	}
	
	/*ȷ��ֵ����*/
	memset( pcValueType, 0x00, sizeof(pcValueType) );
	if (cValueStart == '(' && cValueEnd == ')')
	{
		if (strcmp( pcKeyStart, pcKeyEnd ) >= 0)
		{
			SHOW_ERRMSG("��һ��ֵֻ����С�ڵڶ���ֵ");
	    return;
		}
		memcpy( pcValueType, KEY_OPEN_OPEN, OPERATE_LEN );
	}
	else
	if (cValueStart == '(' && cValueEnd == ']')
	{
		if (strcmp( pcKeyStart, pcKeyEnd ) > 0)
		{
			SHOW_ERRMSG("��һ��ֵֻ����С�ڻ���ڵڶ���ֵ");
	    return;
		}
		memcpy( pcValueType, KEY_OPEN_CLOSE, OPERATE_LEN );
	}
	else
	if (cValueStart == '[' && cValueEnd == ')')
	{
		if (strcmp( pcKeyStart, pcKeyEnd ) >= 0)
		{
			SHOW_ERRMSG("��һ��ֵֻ����С�ڵڶ���ֵ");
	    return;
		}
		memcpy( pcValueType, KEY_CLOSE_OPEN, OPERATE_LEN );
	}
	else
	{
		if (strcmp( pcKeyStart, pcKeyEnd ) > 0)
		{
			SHOW_ERRMSG("��һ��ֵֻ����С�ڻ���ڵڶ���ֵ");
	    return;
		}
		memcpy( pcValueType, KEY_CLOSE_CLOSE, OPERATE_LEN );
	}
  
  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if (pcBufStart != pcBufEnd)
	{
	  SHOW_ERRMSG("��䲻��ȷ");
	  return;
	}
	
	memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
	
	memcpy( pcSendBuf, MANAGER_UNLOAD_CNT, OPERATE_LEN );
	memcpy( pcSendBuf + OPERATE_LEN, pcFilePath, FILE_PATH_LEN );
	memcpy( pcSendBuf + OPERATE_LEN + FILE_PATH_LEN, pcValueType, OPERATE_LEN );
	memcpy( pcSendBuf + OPERATE_LEN + FILE_PATH_LEN + OPERATE_LEN, pcKeyStartLen, KEY_VALUE_LENGTH );
	memcpy( pcSendBuf + OPERATE_LEN + FILE_PATH_LEN + OPERATE_LEN + KEY_VALUE_LENGTH, pcKeyStart, iKeyStartLen );
	memcpy( pcSendBuf + OPERATE_LEN + FILE_PATH_LEN + OPERATE_LEN + KEY_VALUE_LENGTH + iKeyStartLen, pcKeyEndLen, KEY_VALUE_LENGTH );
	memcpy( pcSendBuf + OPERATE_LEN + FILE_PATH_LEN + OPERATE_LEN + KEY_VALUE_LENGTH + iKeyStartLen + KEY_VALUE_LENGTH, pcKeyEnd, iKeyEndLen );
	
	iSendLen = OPERATE_LEN + FILE_PATH_LEN + OPERATE_LEN + KEY_VALUE_LENGTH + iKeyStartLen + KEY_VALUE_LENGTH + iKeyEndLen;
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
	
	iSocket = connectToServer( pcgIp, pcgPort );
	if (iSocket == -1)
	{
		SHOW_ERRMSG("���ӵ������ʧ��");
		return;
	}

	/*���ͳ�����*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	/*��������*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	SHOW_ERRMSG("�������ݵ�����˳ɹ��������ĵȴ�����˴���");

  /*�ȴ�����*/
	iRet = waitFd( iSocket, WAIT_IN, WAIT_SERVER_TIME );
	
	/*����������*/
  if (iRet == -1)
	{
		SHOW_ERRMSG("�ȴ���������ʧ��");
	  return;
	}
	
	/*���ճ���*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*���ֵ��0����ʾ��·����*/
	if (atoi( pcRecvLen ) == 0)
	{
		SHOW_ERRMSG("�ȴ���������ʧ��");
	  return;
	}
	
	/*��������*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("���շ��������ʧ��");
		return;
	}
	
	if (cResult == '0')
	{
		/*���յ������ݵ�������*/
		MANAGER_RECV( iSocket, iRet, pcUnloadTotal, iRecvLen, INT64_LEN );
	  
	  fprintf( stdout, "unload size :%lld\n", atoll( pcUnloadTotal ) );
	  
		logFmtSyn( LVNOR, "unload size :%lld", atoll( pcUnloadTotal ) );
	}
	else
	{
		fprintf( stdout, "fail\n" );
		logFmtSyn( LVNOR, "fail" );
	}
	
	return;
}

/**********************************************************************
��������: managerLoadDeal
��������: ����˴��� load ����������
��    ����
��    һ�����ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20161018
**********************************************************************/
void managerLoadDeal( char *pcaBufStart, char *pcaBufEnd )
{
/*
�������ݽṹ������+����+�ļ�·��+�û���+����
�������ݽṹ���������+��������������+��������+��ͬkey����+ʧ������
���͹�ȥ�����ݣ���������ո񣬷���˴������־���ܻ��١�ͳһ������ǰ��0���ַ��󲹿ո񣬷����У���ַ��Ƿ�һ��ʱ���ú��Կո�
*/
  char pcKeyWord[KEY_WORD_LEN];
	char *pcBufStart;
	char *pcBufEnd;
	char *pcBlank;
  char pcFilePath[FILE_PATH_LEN + 1];
  char pcUser[NAME_LEN + 1];
  char pcPass[NAME_LEN + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	char pcSendBuf[OPERATE_LEN + FILE_PATH_LEN + NAME_LEN + NAME_LEN + 1];
	char pcRecvLen[DATA_BUF_LEN + 1];
	char cResult;
	char pcKeysTotal[INT64_LEN + 1];
	char pcKeysLoaded[INT64_LEN + 1];
	char pcKeysUpdate[INT64_LEN + 1];
	char pcKeysFailed[INT64_LEN + 1];
	int  iSendLen;
	int  iRecvLen;
	int  iSocket;
	int  iDataLen;
	int  iRet;
	
	logFmtSyn( LVSYS, "��ʼִ�к�����%s", __FUNCTION__ );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
	/*ȡ�� from �ؼ���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "FROM" ))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
	
	/*ȡ���ļ�����*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > FILE_PATH_LEN)
	  {
	  	SHOW_ERRMSG("�ļ����Ʋ����Ϲ淶");
	  	return;
	  }
	  
	  memset( pcFilePath, 0x00, sizeof(pcFilePath) );
	  memcpy( pcFilePath, pcBufStart, pcBlank - pcBufStart );
	  
	  /*���ļ����ƺ󲹿ո�*/
	  iDataLen = strlen( pcFilePath );
	  memset( pcFilePath + iDataLen, ' ', FILE_PATH_LEN - iDataLen );
	  	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ�� using �ؼ���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "USING" ))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ���û���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > NAME_LEN)
	  {
	  	SHOW_ERRMSG("�û��������Ϲ淶");
	  	return;
	  }
	  
	  memset( pcUser, 0x00, sizeof(pcUser) );
	  memcpy( pcUser, pcBufStart, pcBlank - pcBufStart );
	  
	  /*���û����󲹿ո�*/
	  iDataLen = strlen( pcUser );
	  memset( pcUser + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ�� by �ؼ���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "BY" ))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ������*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	pcBlank = pcBufEnd;
	  }
	  else
	  {
	  	pcBlank = pcBlank - 1;
	  }
	  
	  if (pcBlank - pcBufStart + 1 > NAME_LEN)
	  {
	  	SHOW_ERRMSG("���벻���Ϲ淶");
	  	return;
	  }
	  
	  memset( pcPass, 0x00, sizeof(pcPass) );
	  memcpy( pcPass, pcBufStart, pcBlank - pcBufStart + 1 );
	  
	  /*������󲹿ո�*/
	  iDataLen = strlen( pcPass );
	  memset( pcPass + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank;
  }
  
  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if (pcBufStart != pcBufEnd)
	{
	  SHOW_ERRMSG("��䲻��ȷ");
	  return;
	}
	
	memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
	
	memcpy( pcSendBuf, MANAGER_LOAD_CNT, OPERATE_LEN );
	memcpy( pcSendBuf + OPERATE_LEN, pcFilePath, FILE_PATH_LEN );
	memcpy( pcSendBuf + OPERATE_LEN + FILE_PATH_LEN, pcUser, NAME_LEN );
	memcpy( pcSendBuf + OPERATE_LEN + FILE_PATH_LEN + NAME_LEN, pcPass, NAME_LEN );
	
	iSendLen = OPERATE_LEN + FILE_PATH_LEN + NAME_LEN + NAME_LEN;
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
	
	iSocket = connectToServer( pcgIp, pcgPort );
	if (iSocket == -1)
	{
		SHOW_ERRMSG("���ӵ������ʧ��");
		return;
	}

	/*���ͳ�����*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	/*��������*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	SHOW_ERRMSG("�������ݵ�����˳ɹ��������ĵȴ�����˴���");

  /*�ȴ�����*/
	iRet = waitFd( iSocket, WAIT_IN, WAIT_SERVER_TIME );
	
	/*����������*/
  if (iRet == -1)
	{
		SHOW_ERRMSG("�ȴ���������ʧ��");
	  return;
	}
	
	/*���ճ���*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*���ֵ��0����ʾ��·����*/
	if (atoi( pcRecvLen ) == 0)
	{
		SHOW_ERRMSG("�ȴ���������ʧ��");
	  return;
	}
	
	/*��������*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("���շ��������ʧ��");
		return;
	}
	
	if (cResult == '0')
	{
		/*���յ������ݵ�������*/
		MANAGER_RECV( iSocket, iRet, pcKeysTotal, iRecvLen, INT64_LEN );
		
		/*���յ�����������*/
		MANAGER_RECV( iSocket, iRet, pcKeysLoaded, iRecvLen, INT64_LEN );
	  
	  /*������ͬkey����*/
	  MANAGER_RECV( iSocket, iRet, pcKeysUpdate, iRecvLen, INT64_LEN );
	  
	  /*����ʧ������*/
		MANAGER_RECV( iSocket, iRet, pcKeysFailed, iRecvLen, INT64_LEN );
	  
	  fprintf( stdout, "all    keys :%lld\n", atoll( pcKeysTotal ) );
	  fprintf( stdout, "loaded keys :%lld\n", atoll( pcKeysLoaded ) );
	  fprintf( stdout, "update keys :%lld\n", atoll( pcKeysUpdate ) );
	  fprintf( stdout, "failed keys :%lld\n", atoll( pcKeysFailed ) );
	  
		logFmtSyn( LVNOR, "all    keys :%lld", atoll( pcKeysTotal ) );
		logFmtSyn( LVNOR, "loaded keys :%lld", atoll( pcKeysLoaded ) );
		logFmtSyn( LVNOR, "update keys :%lld", atoll( pcKeysUpdate ) );
		logFmtSyn( LVNOR, "failed keys :%lld", atoll( pcKeysFailed ) );
	}
	else
	{
		fprintf( stdout, "fail\n" );
		logFmtSyn( LVNOR, "fail" );
	}

	return;
}

/**********************************************************************
��������: managerExitDeal
��������: ����˴��� exit ����������
��    ����
��    һ�����ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20161018
**********************************************************************/
void managerExitDeal( char *pcaBufStart, char *pcaBufEnd )
{
/*
�������ݽṹ������+����+�û���+����
�������ݽṹ���������
���͹�ȥ�����ݣ���������ո񣬷���˴������־���ܻ��١�ͳһ������ǰ��0���ַ��󲹿ո񣬷����У���ַ��Ƿ�һ��ʱ���ú��Կո�
*/
  char pcKeyWord[KEY_WORD_LEN];
	char *pcBufStart;
	char *pcBufEnd;
	char *pcBlank;
  char pcUser[NAME_LEN + 1];
  char pcPass[NAME_LEN + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	char pcSendBuf[OPERATE_LEN + NAME_LEN + NAME_LEN + 1];
	char pcRecvLen[DATA_BUF_LEN + 1];
	char cResult;
	int  iSendLen;
	int  iSocket;
	int  iDataLen;
	int  iRecvLen;
	int  iRet;
	
	logFmtSyn( LVSYS, "��ʼִ�к�����%s", __FUNCTION__ );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
  
  /*ȡ�� using �ؼ���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "USING" ))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ���û���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > NAME_LEN)
	  {
	  	SHOW_ERRMSG("�û��������Ϲ淶");
	  	return;
	  }
	  
	  memset( pcUser, 0x00, sizeof(pcUser) );
	  memcpy( pcUser, pcBufStart, pcBlank - pcBufStart );
	  
	  /*���û����󲹿ո�*/
	  iDataLen = strlen( pcUser );
	  memset( pcUser + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ�� by �ؼ���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "BY" ))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ������*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	pcBlank = pcBufEnd;
	  }
	  else
	  {
	  	pcBlank = pcBlank - 1;
	  }
	  
	  if (pcBlank - pcBufStart + 1 > NAME_LEN)
	  {
	  	pcBlank = pcBufEnd;
	  }
	  
	  memset( pcPass, 0x00, sizeof(pcPass) );
	  memcpy( pcPass, pcBufStart, pcBlank - pcBufStart + 1 );
	  
	  /*������󲹿ո�*/
	  iDataLen = strlen( pcPass );
	  memset( pcPass + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank;
  }
  
  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if (pcBufStart != pcBufEnd)
	{
	  SHOW_ERRMSG("��䲻��ȷ");
	  return;
	}
	
	memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
	
	memcpy( pcSendBuf, MANAGER_EXIT_CNT, OPERATE_LEN );
	memcpy( pcSendBuf + OPERATE_LEN, pcUser, NAME_LEN );
	memcpy( pcSendBuf + OPERATE_LEN + NAME_LEN, pcPass, NAME_LEN );
	
	iSendLen = OPERATE_LEN + NAME_LEN + NAME_LEN;
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
	
	iSocket = connectToServer( pcgIp, pcgPort );
	if (iSocket == -1)
	{
		SHOW_ERRMSG("���ӵ������ʧ��");
		return;
	}

	/*���ͳ�����*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	/*��������*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	iRet = waitFd( iSocket, WAIT_IN, TIME_OUT );
	if (iRet == -1)
	{
		SHOW_ERRMSG("�ȴ����շ��������ʧ��");
		return;
	}
	
	/*�������ݳ���*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*��������*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("���շ��������ʧ��");
		return;
	}
	
	fprintf( stdout, "result:\n" );
	logFmtSyn( LVNOR, "result:" );
	
	if (cResult == '0')
	{
		fprintf( stdout, "success\n" );
		logFmtSyn( LVNOR, "success" );
	}
	else
	{
		fprintf( stdout, "fail\n" );
		logFmtSyn( LVNOR, "fail" );
	}
	
	return;
}

/**********************************************************************
��������: managerFlushDeal
��������: ����˴��� flush ����������
��    ����
��    һ�����ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20161018
**********************************************************************/
void managerFlushDeal( char *pcaBufStart, char *pcaBufEnd )
{
/*
�������ݽṹ������+����+�û���+����
�������ݽṹ���������
���͹�ȥ�����ݣ���������ո񣬷���˴������־���ܻ��١�ͳһ������ǰ��0���ַ��󲹿ո񣬷����У���ַ��Ƿ�һ��ʱ���ú��Կո�
*/
  char pcKeyWord[KEY_WORD_LEN];
	char *pcBufStart;
	char *pcBufEnd;
	char *pcBlank;
  char pcUser[NAME_LEN + 1];
  char pcPass[NAME_LEN + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	char pcSendBuf[OPERATE_LEN + NAME_LEN + NAME_LEN + 1];
	char pcRecvLen[DATA_BUF_LEN + 1];
	char cResult;
	int  iSendLen;
	int  iSocket;
	int  iDataLen;
	int  iRecvLen;
	int  iRet;
	int  iTimeStart;
	int  iTimeEnd;
	
	logFmtSyn( LVSYS, "��ʼִ�к�����%s", __FUNCTION__ );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
  
  /*ȡ�� using �ؼ���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "USING" ))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ���û���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > NAME_LEN)
	  {
	  	SHOW_ERRMSG("�û��������Ϲ淶");
	  	return;
	  }
	  
	  memset( pcUser, 0x00, sizeof(pcUser) );
	  memcpy( pcUser, pcBufStart, pcBlank - pcBufStart );
	  
	  /*���û����󲹿ո�*/
	  iDataLen = strlen( pcUser );
	  memset( pcUser + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ�� by �ؼ���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "BY" ))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ������*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	pcBlank = pcBufEnd;
	  }
	  else
	  {
	  	pcBlank = pcBlank - 1;
	  }
	  
	  if (pcBlank - pcBufStart + 1 > NAME_LEN)
	  {
	  	SHOW_ERRMSG("���벻���Ϲ淶");
	  	return;
	  }
	  
	  memset( pcPass, 0x00, sizeof(pcPass) );
	  memcpy( pcPass, pcBufStart, pcBlank - pcBufStart + 1 );
	  
	  /*������󲹿ո�*/
	  iDataLen = strlen( pcPass );
	  memset( pcPass + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank;
  }
  
  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if (pcBufStart != pcBufEnd)
	{
	  SHOW_ERRMSG("��䲻��ȷ");
	  return;
	}
	
	memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
	
	memcpy( pcSendBuf, MANAGER_FLUSH_CNT, OPERATE_LEN );
	memcpy( pcSendBuf + OPERATE_LEN, pcUser, NAME_LEN );
	memcpy( pcSendBuf + OPERATE_LEN + NAME_LEN, pcPass, NAME_LEN );
	
	iSendLen = OPERATE_LEN + NAME_LEN + NAME_LEN;
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
	
	iSocket = connectToServer( pcgIp, pcgPort );
	if (iSocket == -1)
	{
		SHOW_ERRMSG("���ӵ������ʧ��");
		return;
	}
	
	/*���ͳ�����*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	/*��������*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	iTimeStart = clock();
	fprintf( stderr, "flush start:%d\n", iTimeStart );
	iRet = waitFd( iSocket, WAIT_IN, NO_TIME_OUT );
	if (iRet == -1)
	{
		SHOW_ERRMSG("�ȴ����շ��������ʧ��");
		return;
	}
	
	/*�������ݳ���*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*��������*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("���շ��������ʧ��");
		return;
	}
	
	iTimeEnd = clock();
	fprintf( stderr, "flush   end:%d\n", iTimeEnd );
	fprintf( stderr, "flush  time:%d\n", iTimeEnd - iTimeStart );
	
	fprintf( stdout, "result:\n" );
	logFmtSyn( LVNOR, "result:" );
	
	if (cResult == '0')
	{
		fprintf( stdout, "success\n" );
		logFmtSyn( LVNOR, "success" );
	}
	else
	{
		fprintf( stdout, "fail\n" );
		logFmtSyn( LVNOR, "fail" );
	}
	
	return;
}

/**********************************************************************
��������: managerRamDeal
��������: ����˴��� ram to disc ����������
��    ����
��    һ�����ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20170217
**********************************************************************/
void managerRamDeal( char *pcaBufStart, char *pcaBufEnd )
{
/*
�������ݽṹ������+����+�û���+����
�������ݽṹ���������
���͹�ȥ�����ݣ���������ո񣬷���˴������־���ܻ��١�ͳһ������ǰ��0���ַ��󲹿ո񣬷����У���ַ��Ƿ�һ��ʱ���ú��Կո�
*/
  char pcKeyWord[KEY_WORD_LEN];
	char *pcBufStart;
	char *pcBufEnd;
	char *pcBlank;
  char pcUser[NAME_LEN + 1];
  char pcPass[NAME_LEN + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	char pcSendBuf[OPERATE_LEN + NAME_LEN + NAME_LEN + 1];
	char pcRecvLen[DATA_BUF_LEN + 1];
	char cResult;
	int  iSendLen;
	int  iSocket;
	int  iDataLen;
	int  iRecvLen;
	int  iRet;
	int  iTimeStart;
	int  iTimeEnd;
	
	logFmtSyn( LVSYS, "��ʼִ�к�����%s", __FUNCTION__ );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
  /*ȡ�� to �ؼ���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "TO" ))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
	
	/*ȡ�� DISC �ؼ���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "DISC" ))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ�� using �ؼ���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "USING" ))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ���û���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > NAME_LEN)
	  {
	  	SHOW_ERRMSG("�û��������Ϲ淶");
	  	return;
	  }
	  
	  memset( pcUser, 0x00, sizeof(pcUser) );
	  memcpy( pcUser, pcBufStart, pcBlank - pcBufStart );
	  
	  /*���û����󲹿ո�*/
	  iDataLen = strlen( pcUser );
	  memset( pcUser + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ�� by �ؼ���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "BY" ))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ������*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	pcBlank = pcBufEnd;
	  }
	  else
	  {
	  	pcBlank = pcBlank - 1;
	  }
	  
	  if (pcBlank - pcBufStart + 1 > NAME_LEN)
	  {
	  	SHOW_ERRMSG("���벻���Ϲ淶");
	  	return;
	  }
	  
	  memset( pcPass, 0x00, sizeof(pcPass) );
	  memcpy( pcPass, pcBufStart, pcBlank - pcBufStart + 1 );
	  
	  /*������󲹿ո�*/
	  iDataLen = strlen( pcPass );
	  memset( pcPass + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank;
  }
  
  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if (pcBufStart != pcBufEnd)
	{
	  SHOW_ERRMSG("��䲻��ȷ");
	  return;
	}
	
	memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
	
	memcpy( pcSendBuf, MANAGER_RAM_TO_DISC_CNT, OPERATE_LEN );
	memcpy( pcSendBuf + OPERATE_LEN, pcUser, NAME_LEN );
	memcpy( pcSendBuf + OPERATE_LEN + NAME_LEN, pcPass, NAME_LEN );
	
	iSendLen = OPERATE_LEN + NAME_LEN + NAME_LEN;
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
	
	iSocket = connectToServer( pcgIp, pcgPort );
	if (iSocket == -1)
	{
		SHOW_ERRMSG("���ӵ������ʧ��");
		return;
	}
	
	/*���ͳ�����*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	/*��������*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	iTimeStart = clock();
	fprintf( stderr, "ram to disc start:%d\n", iTimeStart );
	iRet = waitFd( iSocket, WAIT_IN, NO_TIME_OUT );
	if (iRet == -1)
	{
		SHOW_ERRMSG("�ȴ����շ��������ʧ��");
		return;
	}
	
	/*�������ݳ���*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*��������*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("���շ��������ʧ��");
		return;
	}
	
	iTimeEnd = clock();
	fprintf( stderr, "ram to disc   end:%d\n", iTimeEnd );
	fprintf( stderr, "ram to disc  time:%d\n", iTimeEnd - iTimeStart );
	
	fprintf( stdout, "result:\n" );
	logFmtSyn( LVNOR, "result:" );
	
	if (cResult == '0')
	{
		fprintf( stdout, "success\n" );
		logFmtSyn( LVNOR, "success" );
	}
	else
	{
		fprintf( stdout, "fail\n" );
		logFmtSyn( LVNOR, "fail" );
	}
	
	return;
}

/**********************************************************************
��������: managerAlterDeal
��������: ����˴��� alter ����������
��    ����
��    һ�����ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20161018
**********************************************************************/
void managerAlterDeal( char *pcaBufStart, char *pcaBufEnd )
{
  char pcKeyWord[KEY_WORD_LEN];
	char *pcBlank;
	char *pcBufStart;
	char *pcBufEnd;
	
	logFmtSyn( LVSYS, "��ʼִ�к�����%s", __FUNCTION__ );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if (pcBufStart == pcBufEnd)
	{
		SHOW_ERRMSG("ֻ�в�������ȱ�ٲ�������");
		return;
	}
	
	pcBlank = strchr( pcBufStart, ' ' );
	if (pcBlank == NULL)
	{
		SHOW_ERRMSG("��䲻��ȷ");
		return;
	}
	
	if (pcBlank - pcBufStart > KEY_WORD_LEN)
	{
		SHOW_ERRMSG("�������Ͳ�����");
		return;
	}
	
	memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );

	if (!strcmpIgnore( pcKeyWord, "THREADS" ))
	{
		managerAlterThreadsDeal( pcBlank + 1, pcBufEnd );
	}
	else
	if (!strcmpIgnore( pcKeyWord, "DISCS" ))
	{
		managerAlterDiscsDeal( pcBlank + 1, pcBufEnd );
	}
	else
	if (!strcmpIgnore( pcKeyWord, "UPPERS" ))
	{
		managerAlterUppersDeal( pcBlank + 1, pcBufEnd );
	}
	else
	{
		SHOW_ERRMSG("�������Ͳ�����");
		return;
	}
	
	return;
}

/**********************************************************************
��������: managerAlterThreadsDeal
��������: ����˴��� alter threads ����������
��    ����
��    һ�����ݿ�ʼ��ַ      I
��    �������ݽ�����ַ      I
��    ��: �����
����ʱ��: 20161019
**********************************************************************/
void managerAlterThreadsDeal( char *pcaBufStart, char *pcaBufEnd )
{
/*
�������ݽṹ������+����+�������߳�����+�������߳�����+�û���+����
�������ݽṹ���������
���͹�ȥ�����ݣ���������ո񣬷���˴������־���ܻ��١�ͳһ������ǰ��0���ַ��󲹿ո񣬷����У���ַ��Ƿ�һ��ʱ���ú��Կո�
*/
  char pcKeyWord[KEY_WORD_LEN];
	char *pcBufStart;
	char *pcBufEnd;
	char *pcBlank;
	char *pcComma;
	char pcIntTmp[INT64_LEN + 1];
  char pcShortThreads[INT64_LEN + 1];
  char pcLongThreads[INT64_LEN + 1];
  char pcUser[NAME_LEN + 1];
  char pcPass[NAME_LEN + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	char pcSendBuf[OPERATE_LEN + INT64_LEN + INT64_LEN + NAME_LEN + NAME_LEN + 1];
	char pcRecvLen[DATA_BUF_LEN + 1];
	char cResult;
	int  iSendLen;
	int  iSocket;
	int  iDataLen;
	int  iRecvLen;
	int  iRet;
	
	logFmtSyn( LVSYS, "��ʼִ�к�����%s", __FUNCTION__ );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
	/*ȡ�� to �ؼ���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "TO" ))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
	
	/*ȡ�����߳�����*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcComma = strchr( pcBufStart, ',' );
	  if (pcComma == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcComma - pcBufStart > INT64_LEN)
	  {
	  	SHOW_ERRMSG("���߳����������Ϲ淶");
	  	return;
	  }
	  
	  memset( pcShortThreads, 0x00, sizeof(pcShortThreads) );
	  memcpy( pcShortThreads, pcBufStart, pcComma - pcBufStart );
	  
	  if (!isInt( pcShortThreads, pcShortThreads + strlen(pcShortThreads) ) || atoll( pcShortThreads ) <= 0 )
	  {
	  	SHOW_ERRMSG("���߳����������Ϲ淶");
		  return;
	  }
	  
	  memset( pcIntTmp, 0x00, sizeof(pcIntTmp) );
	  sprintf( pcIntTmp, "%0*lld", INT64_LEN, atoll(pcShortThreads) );
	  memset( pcShortThreads, 0x00, sizeof(pcShortThreads) );
	  strcpy( pcShortThreads, pcIntTmp );
	  
	  pcBufStart = pcComma + 1;
  }
	
	/*ȡ�����߳�����*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > INT64_LEN)
	  {
	  	SHOW_ERRMSG("���߳����������Ϲ淶");
	  	return;
	  }
	  
	  memset( pcLongThreads, 0x00, sizeof(pcLongThreads) );
	  memcpy( pcLongThreads, pcBufStart, pcBlank - pcBufStart );
	  
	  if (!isInt( pcLongThreads, pcLongThreads + strlen(pcLongThreads) ) || atoll( pcLongThreads ) <= 0)
	  {
	  	SHOW_ERRMSG("���߳����������Ϲ淶");
		  return;
	  }
	  
	  memset( pcIntTmp, 0x00, sizeof(pcIntTmp) );
	  sprintf( pcIntTmp, "%0*lld", INT64_LEN, atoll(pcLongThreads) );
	  memset( pcLongThreads, 0x00, sizeof(pcLongThreads) );
	  strcpy( pcLongThreads, pcIntTmp );
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ�� using �ؼ���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "USING" ))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ���û���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > NAME_LEN)
	  {
	  	SHOW_ERRMSG("�û��������Ϲ淶");
	  	return;
	  }
	  
	  memset( pcUser, 0x00, sizeof(pcUser) );
	  memcpy( pcUser, pcBufStart, pcBlank - pcBufStart );
	  
	  /*���û����󲹿ո�*/
	  iDataLen = strlen( pcUser );
	  memset( pcUser + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ�� by �ؼ���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "BY" ))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ������*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	pcBlank = pcBufEnd;
	  }
	  else
	  {
	  	pcBlank = pcBlank - 1;
	  }
	  
	  if (pcBlank - pcBufStart + 1 > NAME_LEN)
	  {
	  	SHOW_ERRMSG("���벻���Ϲ淶");
	  	return;
	  }
	  
	  memset( pcPass, 0x00, sizeof(pcPass) );
	  memcpy( pcPass, pcBufStart, pcBlank - pcBufStart + 1 );
	  
	  /*������󲹿ո�*/
	  iDataLen = strlen( pcPass );
	  memset( pcPass + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank;
  }
  
  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if (pcBufStart != pcBufEnd)
	{
	  SHOW_ERRMSG("��䲻��ȷ");
	  return;
	}
	
	memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
	
	memcpy( pcSendBuf, MANAGER_ALTER_THREADS_CNT, OPERATE_LEN );
	memcpy( pcSendBuf + OPERATE_LEN, pcShortThreads, INT64_LEN );
	memcpy( pcSendBuf + OPERATE_LEN + INT64_LEN, pcLongThreads, INT64_LEN );
	memcpy( pcSendBuf + OPERATE_LEN + INT64_LEN + INT64_LEN, pcUser, NAME_LEN );
	memcpy( pcSendBuf + OPERATE_LEN + INT64_LEN + INT64_LEN + NAME_LEN, pcPass, NAME_LEN );
	
	iSendLen = OPERATE_LEN + INT64_LEN + INT64_LEN + NAME_LEN + NAME_LEN;
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
	
	iSocket = connectToServer( pcgIp, pcgPort );
	if (iSocket == -1)
	{
		SHOW_ERRMSG("���ӵ������ʧ��");
		return;
	}

	/*���ͳ�����*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	/*��������*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	iRet = waitFd( iSocket, WAIT_IN, TIME_OUT );
	if (iRet == -1)
	{
		SHOW_ERRMSG("�ȴ����շ��������ʧ��");
		return;
	}
	
	/*�������ݳ���*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*��������*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("���շ��������ʧ��");
		return;
	}
	
	fprintf( stdout, "result:\n" );
	logFmtSyn( LVNOR, "result:" );
	
	if (cResult == '0')
	{
		fprintf( stdout, "success\n" );
		logFmtSyn( LVNOR, "success" );
	}
	else
	{
		fprintf( stdout, "fail\n" );
		logFmtSyn( LVNOR, "fail" );
	}
	
	return;
}

/**********************************************************************
��������: managerAlterDiscsDeal
��������: ����˴��� alter discs ����������
��    ����
��    һ�����ݿ�ʼ��ַ      I
��    �������ݽ�����ַ      I
��    ��: �����
����ʱ��: 20161019
**********************************************************************/
void managerAlterDiscsDeal( char *pcaBufStart, char *pcaBufEnd )
{
/*
�������ݽṹ������+����+�ļ����ֵ+�û���+����
�������ݽṹ���������
���͹�ȥ�����ݣ���������ո񣬷���˴������־���ܻ��١�ͳһ������ǰ��0���ַ��󲹿ո񣬷����У���ַ��Ƿ�һ��ʱ���ú��Կո�
*/
  char pcKeyWord[KEY_WORD_LEN];
	char *pcBufStart;
	char *pcBufEnd;
	char *pcBlank;
	char pcIntTmp[INT64_LEN + 1];
  char pcDiscs[INT64_LEN + 1];
  char pcUser[NAME_LEN + 1];
  char pcPass[NAME_LEN + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	char pcSendBuf[OPERATE_LEN + INT64_LEN + NAME_LEN + NAME_LEN + 1];
	char pcRecvLen[DATA_BUF_LEN + 1];
	char cResult;
	int  iSendLen;
	int  iSocket;
	int  iDataLen;
	int  iRecvLen;
	int  iRet;
	
	logFmtSyn( LVSYS, "��ʼִ�к�����%s", __FUNCTION__ );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
	/*ȡ�� to �ؼ���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "TO" ))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
	
	/*ȡ���ļ����ֵ*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > INT64_LEN)
	  {
	  	SHOW_ERRMSG("�ļ����ֵ�����Ϲ淶");
	  	return;
	  }
	  
	  memset( pcDiscs, 0x00, sizeof(pcDiscs) );
	  memcpy( pcDiscs, pcBufStart, pcBlank - pcBufStart );
	  
	  if (!isInt( pcDiscs, pcDiscs + strlen(pcDiscs) ) || atoll( pcDiscs ) <= 0 )
	  {
	  	SHOW_ERRMSG("�ļ����ֵ�����Ϲ淶");
		  return;
	  }
	  
	  memset( pcIntTmp, 0x00, sizeof(pcIntTmp) );
	  sprintf( pcIntTmp, "%0*lld", INT64_LEN, atoll(pcDiscs) );
	  memset( pcDiscs, 0x00, sizeof(pcDiscs) );
	  strcpy( pcDiscs, pcIntTmp );
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ�� using �ؼ���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "USING" ))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ���û���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > NAME_LEN)
	  {
	  	SHOW_ERRMSG("�û��������Ϲ淶");
	  	return;
	  }
	  
	  memset( pcUser, 0x00, sizeof(pcUser) );
	  memcpy( pcUser, pcBufStart, pcBlank - pcBufStart );
	  
	  /*���û����󲹿ո�*/
	  iDataLen = strlen( pcUser );
	  memset( pcUser + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ�� by �ؼ���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "BY" ))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ������*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	pcBlank = pcBufEnd;
	  }
	  else
	  {
	  	pcBlank = pcBlank - 1;
	  }
	  
	  if (pcBlank - pcBufStart + 1 > NAME_LEN)
	  {
	  	SHOW_ERRMSG("���벻���Ϲ淶");
	  	return;
	  }
	  
	  memset( pcPass, 0x00, sizeof(pcPass) );
	  memcpy( pcPass, pcBufStart, pcBlank - pcBufStart + 1 );
	  
	  /*������󲹿ո�*/
	  iDataLen = strlen( pcPass );
	  memset( pcPass + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank;
  }
  
  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if (pcBufStart != pcBufEnd)
	{
	  SHOW_ERRMSG("��䲻��ȷ");
	  return;
	}
	
	memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
	
	memcpy( pcSendBuf, MANAGER_ALTER_DISCS_CNT, OPERATE_LEN );
	memcpy( pcSendBuf + OPERATE_LEN, pcDiscs, INT64_LEN );
	memcpy( pcSendBuf + OPERATE_LEN + INT64_LEN, pcUser, NAME_LEN );
	memcpy( pcSendBuf + OPERATE_LEN + INT64_LEN + NAME_LEN, pcPass, NAME_LEN );
	
	iSendLen = OPERATE_LEN + INT64_LEN + NAME_LEN + NAME_LEN;
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
	
	iSocket = connectToServer( pcgIp, pcgPort );
	if (iSocket == -1)
	{
		SHOW_ERRMSG("���ӵ������ʧ��");
		return;
	}
	
	/*���ͳ�����*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	/*��������*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	iRet = waitFd( iSocket, WAIT_IN, TIME_OUT );
	if (iRet == -1)
	{
		SHOW_ERRMSG("�ȴ����շ��������ʧ��");
		return;
	}
	
	/*�������ݳ���*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*��������*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("���շ��������ʧ��");
		return;
	}
	
	fprintf( stdout, "result:\n" );
	logFmtSyn( LVNOR, "result:" );
	
	if (cResult == '0')
	{
		fprintf( stdout, "success\n" );
		logFmtSyn( LVNOR, "success" );
	}
	else
	{
		fprintf( stdout, "fail\n" );
		logFmtSyn( LVNOR, "fail" );
	}
	
	return;
}

/**********************************************************************
��������: managerAlterUppersDeal
��������: ����˴��� alter uppers ����������
��    ����
��    һ�����ݿ�ʼ��ַ      I
��    �������ݽ�����ַ      I
��    ��: �����
����ʱ��: 20161019
**********************************************************************/
void managerAlterUppersDeal( char *pcaBufStart, char *pcaBufEnd )
{
/*
�������ݽṹ������+����+�ļ�����ֵ+�û���+����
�������ݽṹ���������
���͹�ȥ�����ݣ���������ո񣬷���˴������־���ܻ��١�ͳһ������ǰ��0���ַ��󲹿ո񣬷����У���ַ��Ƿ�һ��ʱ���ú��Կո�
*/
  char pcKeyWord[KEY_WORD_LEN];
	char *pcBufStart;
	char *pcBufEnd;
	char *pcBlank;
	char pcIntTmp[INT64_LEN + 1];
  char pcUppers[INT64_LEN + 1];
  char pcUser[NAME_LEN + 1];
  char pcPass[NAME_LEN + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	char pcSendBuf[OPERATE_LEN + INT64_LEN + NAME_LEN + NAME_LEN + 1];
	char pcRecvLen[DATA_BUF_LEN + 1];
	char cResult;
	int  iSendLen;
	int  iSocket;
	int  iDataLen;
	int  iRecvLen;
	int  iRet;
	
	logFmtSyn( LVSYS, "��ʼִ�к�����%s", __FUNCTION__ );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
	/*ȡ�� to �ؼ���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "TO" ))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
	
	/*ȡ���ļ����ֵ*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > INT64_LEN)
	  {
	  	SHOW_ERRMSG("�ļ�����ֵ�����Ϲ淶");
	  	return;
	  }
	  
	  memset( pcUppers, 0x00, sizeof(pcUppers) );
	  memcpy( pcUppers, pcBufStart, pcBlank - pcBufStart );
	  
	  if (!isInt( pcUppers, pcUppers + strlen(pcUppers) ) || atoll( pcUppers ) <= 0)
	  {
	  	SHOW_ERRMSG("�ļ�����ֵ�����Ϲ淶");
		  return;
	  }
	  
	  memset( pcIntTmp, 0x00, sizeof(pcIntTmp) );
	  sprintf( pcIntTmp, "%0*lld", INT64_LEN, atoll(pcUppers) );
	  memset( pcUppers, 0x00, sizeof(pcUppers) );
	  strcpy( pcUppers, pcIntTmp );
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ�� using �ؼ���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "USING" ))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ���û���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > NAME_LEN)
	  {
	  	SHOW_ERRMSG("�û��������Ϲ淶");
	  	return;
	  }
	  
	  memset( pcUser, 0x00, sizeof(pcUser) );
	  memcpy( pcUser, pcBufStart, pcBlank - pcBufStart );
	  
	  /*���û����󲹿ո�*/
	  iDataLen = strlen( pcUser );
	  memset( pcUser + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ�� by �ؼ���*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "BY" ))
	  {
	  	SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*ȡ������*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("��䲻��ȷ");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	pcBlank = pcBufEnd;
	  }
	  else
	  {
	  	pcBlank = pcBlank - 1;
	  }
	  
	  if (pcBlank - pcBufStart + 1 > NAME_LEN)
	  {
	  	SHOW_ERRMSG("���벻���Ϲ淶");
	  	return;
	  }
	  
	  memset( pcPass, 0x00, sizeof(pcPass) );
	  memcpy( pcPass, pcBufStart, pcBlank - pcBufStart + 1 );
	  
	  /*������󲹿ո�*/
	  iDataLen = strlen( pcPass );
	  memset( pcPass + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank;
  }
  
  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if (pcBufStart != pcBufEnd)
	{
	  SHOW_ERRMSG("��䲻��ȷ");
	  return;
	}
	
	memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
	
	memcpy( pcSendBuf, MANAGER_ALTER_UPPERS_CNT, OPERATE_LEN );
	memcpy( pcSendBuf + OPERATE_LEN, pcUppers, INT64_LEN );
	memcpy( pcSendBuf + OPERATE_LEN + INT64_LEN, pcUser, NAME_LEN );
	memcpy( pcSendBuf + OPERATE_LEN + INT64_LEN + NAME_LEN, pcPass, NAME_LEN );
	
	iSendLen = OPERATE_LEN + INT64_LEN + NAME_LEN + NAME_LEN;
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
	
	iSocket = connectToServer( pcgIp, pcgPort );
	if (iSocket == -1)
	{
		SHOW_ERRMSG("���ӵ������ʧ��");
		return;
	}

	/*���ͳ�����*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	/*��������*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	iRet = waitFd( iSocket, WAIT_IN, TIME_OUT );
	if (iRet == -1)
	{
		SHOW_ERRMSG("�ȴ����շ��������ʧ��");
		return;
	}
	
	/*�������ݳ���*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*��������*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("���շ��������ʧ��");
		return;
	}
	
	fprintf( stdout, "result:\n" );
	logFmtSyn( LVNOR, "result:" );
	  
	if (cResult == '0')
	{
		fprintf( stdout, "success\n" );
		logFmtSyn( LVNOR, "success" );
	}
	else
	{
		fprintf( stdout, "fail\n" );
		logFmtSyn( LVNOR, "fail" );
	}
	
	return;
}

/**********************************************************************
��������: managerShowDeal
��������: ����˴��� show ����������
��    ����
��    һ�����ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20161018
**********************************************************************/
void managerShowDeal( char *pcaBufStart, char *pcaBufEnd )
{
	char pcKeyWord[KEY_WORD_LEN];
	char *pcBlank;
	char *pcBufStart;
	char *pcBufEnd;
	
	logFmtSyn( LVSYS, "��ʼִ�к�����%s", __FUNCTION__ );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
		
	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if (pcBufStart == pcBufEnd)
	{
		SHOW_ERRMSG("ֻ�в�������ȱ�ٲ�������");
		return;
	}
	
	if (pcBufEnd - pcBufStart + 1 > KEY_WORD_LEN)
	{
		SHOW_ERRMSG("�������Ͳ�����");
		return;
	}
	
	memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	memcpy( pcKeyWord, pcBufStart, pcBufEnd - pcBufStart + 1 );

	if (!strcmpIgnore( pcKeyWord, "RAMS" ))
	{
		managerShowRamsDeal();
	}
	else
	if (!strcmpIgnore( pcKeyWord, "DISCS" ))
	{
		managerShowDiscsDeal();
	}
	else
	if (!strcmpIgnore( pcKeyWord, "THREADS" ))
	{
		managerShowThreadsDeal();
	}
	else
	if (!strcmpIgnore( pcKeyWord, "KEYS" ))
	{
		managerShowKeysDeal();
	}
	else
	{
		SHOW_ERRMSG("�������Ͳ�����");
		return;
	}
	
	return;
}

/**********************************************************************
��������: managerShowRamsDeal
��������: ����˴��� show rams ����������
��    ����
��    ��: �����
����ʱ��: 20161019
**********************************************************************/
void managerShowRamsDeal( )
{
/*
�������ݽṹ������+����
�������ݽṹ���������+KEY�ļ������������� + KEY�ļ���������ʹ��ƫ��������λ B�� + VALUE�ļ������������� + VALUE�ļ���������ʹ��ƫ��������λ B��
*/
	char pcSendLen[DATA_BUF_LEN + 1];
	char pcSendBuf[OPERATE_LEN + 1];
	char cResult;
	char pcKeyTotal[INT64_LEN + 1];
	char pcKeyUsed[INT64_LEN + 1];
	char pcValueTotal[INT64_LEN + 1];
	char pcValueUsed[INT64_LEN + 1];
	char pcRecvLen[DATA_BUF_LEN + 1];
	int  iSendLen;
	int  iRecvLen;
	int  iRecvSize;
	int  iSocket;
	int  iRet;
	
	logFmtSyn( LVSYS, "��ʼִ�к�����%s", __FUNCTION__ );
	
	memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
	strcpy( pcSendBuf, MANAGER_SHOW_RAMS_CNT );
	
	iSendLen = strlen(pcSendBuf);
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
	
	iSocket = connectToServer( pcgIp, pcgPort );
	if (iSocket == -1)
	{
		SHOW_ERRMSG("���ӵ������ʧ��");
		return;
	}
	
	/*���ͳ�����*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	/*��������*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	iRet = waitFd( iSocket, WAIT_IN, TIME_OUT );
	if (iRet == -1)
	{
		SHOW_ERRMSG("�ȴ����շ��������ʧ��");
		return;
	}
	
	/*�������ݳ���*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*���ղ������*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("���շ��������ʧ��");
		return;
	}
	
	fprintf( stdout, "result:\n" );
	logFmtSyn( LVNOR, "result:" );
	
	if (cResult == '0')
	{
		/*����KEY�ļ�������������*/
	  MANAGER_RECV( iSocket, iRet, pcKeyTotal, iRecvLen, INT64_LEN );
	  
	  /*����KEY�ļ���������ʹ��ƫ����*/
	  MANAGER_RECV( iSocket, iRet, pcKeyUsed, iRecvLen, INT64_LEN );
	  
	  /*����VALUE�ļ�������������*/
	  MANAGER_RECV( iSocket, iRet, pcValueTotal, iRecvLen, INT64_LEN );
	  
	  /*����VALUE�ļ���������ʹ��ƫ����*/
	  MANAGER_RECV( iSocket, iRet, pcValueUsed, iRecvLen, INT64_LEN );
	  
	  fprintf( stdout, "key   rams total   :%lld\n", atoll(pcKeyTotal) );
	  fprintf( stdout, "key   rams used    :%lld\n", atoll(pcKeyUsed) );
	  fprintf( stdout, "key   rams used per:%.2lf%%\n", (atof(pcKeyUsed)/atof(pcKeyTotal))*100 );
	  fprintf( stdout, "value rams total   :%lld\n", atoll(pcValueTotal) );
	  fprintf( stdout, "value rams used    :%lld\n", atoll(pcValueUsed) );
	  fprintf( stdout, "value rams used per:%.2lf%%\n", (atof(pcValueUsed)/atof(pcValueTotal))*100 );
	  
	  logFmtSyn( LVNOR, "key   rams total   :%lld", atoll(pcKeyTotal) );
	  logFmtSyn( LVNOR, "key   rams used    :%lld", atoll(pcKeyUsed) );
	  logFmtSyn( LVNOR, "key   rams used per:%.2lf%%", (atof(pcKeyUsed)/atof(pcKeyTotal))*100 );
	  logFmtSyn( LVNOR, "value rams total   :%lld", atoll(pcValueTotal) );
	  logFmtSyn( LVNOR, "value rams used    :%lld", atoll(pcValueUsed) );
	  logFmtSyn( LVNOR, "value rams used per:%.2lf%%", (atof(pcValueUsed)/atof(pcValueTotal))*100 );
	}
	else
	{
		fprintf( stdout, "fail\n" );
		logFmtSyn( LVNOR, "fail" );
	}

	return;
}

/**********************************************************************
��������: managerShowDiscsDeal
��������: ����˴��� show discs ����������
��    ����
��    ��: �����
����ʱ��: 20161019
**********************************************************************/
void managerShowDiscsDeal( )
{
/*
�������ݽṹ������+����
�������ݽṹ���������+KEY�ļ����ֵ+KEY�ļ���ǰֵ+KEY�ļ�����ֵ+ KEY�ļ�ʹ��ƫ���� + KEY�ļ���ʹ�ô�С+VALUE�ļ����ֵ+VALUE�ļ���ǰֵ+VALUE�ļ�����ֵ+ VALUE�ļ�ʹ��ƫ����+VALUE�ļ���ʹ�ô�С
*/
	char pcSendLen[DATA_BUF_LEN + 1];
	char pcSendBuf[OPERATE_LEN + 1];
	char cResult;
	char pcKeyMax[INT64_LEN + 1];
	char pcKeyTotal[INT64_LEN + 1];
	char pcKeyUpper[INT64_LEN + 1];
	char pcKeyUsed[INT64_LEN + 1];
	char pcKeyFree[INT64_LEN + 1];
	char pcValueMax[INT64_LEN + 1];
	char pcValueTotal[INT64_LEN + 1];
	char pcValueUpper[INT64_LEN + 1];
	char pcValueUsed[INT64_LEN + 1];
	char pcValueFree[INT64_LEN + 1];
	char pcRecvLen[DATA_BUF_LEN + 1];
	int  iSendLen;
	int  iRecvLen;
	int  iRecvSize;
	int  iSocket;
	int  iRet;
	
	logFmtSyn( LVSYS, "��ʼִ�к�����%s", __FUNCTION__ );
	
	memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
	strcpy( pcSendBuf, MANAGER_SHOW_DISCS_CNT );
	
	iSendLen = strlen(pcSendBuf);
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
	
	iSocket = connectToServer( pcgIp, pcgPort );
	if (iSocket == -1)
	{
		SHOW_ERRMSG("���ӵ������ʧ��");
		return;
	}

	/*���ͳ�����*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	/*��������*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	iRet = waitFd( iSocket, WAIT_IN, TIME_OUT );
	if (iRet == -1)
	{
		SHOW_ERRMSG("�ȴ����շ��������ʧ��");
		return;
	}
	
	/*�������ݳ���*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*���ղ������*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("���շ��������ʧ��");
		return;
	}
	
	fprintf( stdout, "result:\n" );
	logFmtSyn( LVNOR, "result:" );
	
	if (cResult == '0')
	{
		/*����KEY�ļ����ֵ*/
	  MANAGER_RECV( iSocket, iRet, pcKeyMax, iRecvLen, INT64_LEN );
	  
		/*����KEY�ļ���ǰֵ*/
	  MANAGER_RECV( iSocket, iRet, pcKeyTotal, iRecvLen, INT64_LEN );
	  
	  /*����KEY�ļ�����ֵ*/
	  MANAGER_RECV( iSocket, iRet, pcKeyUpper, iRecvLen, INT64_LEN );
	  
	  /*����KEY�ļ�ʹ��ƫ����*/
	  MANAGER_RECV( iSocket, iRet, pcKeyUsed, iRecvLen, INT64_LEN );
	  
	  /*����KEY�ļ���ʹ�ô�С*/
	  MANAGER_RECV( iSocket, iRet, pcKeyFree, iRecvLen, INT64_LEN );
	  
	  /*����VALUE�ļ����ֵ*/
	  MANAGER_RECV( iSocket, iRet, pcValueMax, iRecvLen, INT64_LEN );
	  
	  /*����VALUE�ļ���ǰֵ*/
	  MANAGER_RECV( iSocket, iRet, pcValueTotal, iRecvLen, INT64_LEN );
	  
	  /*����VALUE�ļ�����ֵ*/
	  MANAGER_RECV( iSocket, iRet, pcValueUpper, iRecvLen, INT64_LEN );
	  
	  /*����VALUE�ļ�ʹ��ƫ����*/
	  MANAGER_RECV( iSocket, iRet, pcValueUsed, iRecvLen, INT64_LEN );
	  
	  /*����VALUE�ļ���ʹ�ô�С*/
	  MANAGER_RECV( iSocket, iRet, pcValueFree, iRecvLen, INT64_LEN );
	  
	  fprintf( stdout, "key   discs max     :%lld\n", atoll(pcKeyMax) );
	  fprintf( stdout, "key   discs total   :%lld\n", atoll(pcKeyTotal) );
	  fprintf( stdout, "key   discs upper   :%lld\n", atoll(pcKeyUpper) );
	  fprintf( stdout, "key   discs used    :%lld\n", atoll(pcKeyUsed) );
	  fprintf( stdout, "key   discs free    :%lld\n", atoll(pcKeyFree) );
	  fprintf( stdout, "key   discs used per:%.2lf%%\n", ((atof(pcKeyUsed) + atof(pcKeyFree))/atof(pcKeyTotal))*100 );
	  fprintf( stdout, "value discs max     :%lld\n", atoll(pcValueMax) );
	  fprintf( stdout, "value discs total   :%lld\n", atoll(pcValueTotal) );
	  fprintf( stdout, "value discs upper   :%lld\n", atoll(pcValueUpper) );
	  fprintf( stdout, "value discs used    :%lld\n", atoll(pcValueUsed) );
	  fprintf( stdout, "value discs free    :%lld\n", atoll(pcValueFree) );
	  fprintf( stdout, "value discs used per:%.2lf%%\n", ((atof(pcValueUsed) + atof(pcValueFree))/atof(pcValueTotal))*100 );
	  
	  logFmtSyn( LVNOR, "key   discs max     :%lld", atoll(pcKeyMax) );
	  logFmtSyn( LVNOR, "key   discs total   :%lld", atoll(pcKeyTotal) );
	  logFmtSyn( LVNOR, "key   discs upper   :%lld", atoll(pcKeyUpper) );
	  logFmtSyn( LVNOR, "key   discs used    :%lld", atoll(pcKeyUsed) );
	  logFmtSyn( LVNOR, "key   discs free    :%lld", atoll(pcKeyFree) );
	  logFmtSyn( LVNOR, "key   discs used per:%.2lf%%", ((atof(pcKeyUsed) + atof(pcKeyFree))/atof(pcKeyTotal))*100 );
	  logFmtSyn( LVNOR, "value discs total   :%lld", atoll(pcValueTotal) );
	  logFmtSyn( LVNOR, "value discs total   :%lld", atoll(pcValueTotal) );
	  logFmtSyn( LVNOR, "value discs upper   :%lld", atoll(pcValueUpper) );
	  logFmtSyn( LVNOR, "value discs used    :%lld", atoll(pcValueUsed) );
	  logFmtSyn( LVNOR, "value discs free    :%lld", atoll(pcValueFree) );
	  logFmtSyn( LVNOR, "value discs used per:%.2lf%%", ((atof(pcKeyUsed) + atof(pcKeyFree))/atof(pcKeyTotal))*100 );
	}
	else
	{
		fprintf( stdout, "fail\n" );
		logFmtSyn( LVNOR, "fail" );
	}
	
	return;
}

/**********************************************************************
��������: managerShowThreadsDeal
��������: ����˴��� show threads ����������
��    ����
��    ��: �����
����ʱ��: 20161019
**********************************************************************/
void managerShowThreadsDeal( )
{
/*
�������ݽṹ������+����
�������ݽṹ���������+�������߳�����+�������߳�����
*/
	char pcSendLen[DATA_BUF_LEN + 1];
	char pcSendBuf[OPERATE_LEN + 1];
	char cResult;
	char pcShortThreads[INT64_LEN + 1];
	char pcLongThreads[INT64_LEN + 1];
	char pcRecvLen[DATA_BUF_LEN + 1];
	int  iSendLen;
	int  iRecvLen;
	int  iRecvSize;
	int  iSocket;
	int  iRet;
	
	logFmtSyn( LVSYS, "��ʼִ�к�����%s", __FUNCTION__ );
	
	memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
	strcpy( pcSendBuf, MANAGER_SHOW_THREADS_CNT );
	
	iSendLen = strlen(pcSendBuf);
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
	
	iSocket = connectToServer( pcgIp, pcgPort );
	if (iSocket == -1)
	{
		SHOW_ERRMSG("���ӵ������ʧ��");
		return;
	}

	/*���ͳ�����*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	/*��������*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	iRet = waitFd( iSocket, WAIT_IN, TIME_OUT );
	if (iRet == -1)
	{
		SHOW_ERRMSG("�ȴ����շ��������ʧ��");
		return;
	}
	
	/*�������ݳ���*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*���ղ������*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("���շ��������ʧ��");
		return;
	}
	
	fprintf( stdout, "result:\n" );
	logFmtSyn( LVNOR, "result:" );
	
	if (cResult == '0')
	{
		/*���ն������߳�����*/
	  MANAGER_RECV( iSocket, iRet, pcShortThreads, iRecvLen, INT64_LEN );
	  
	  /*���ճ������߳�����*/
	  MANAGER_RECV( iSocket, iRet, pcLongThreads, iRecvLen, INT64_LEN );

	  fprintf( stdout, "short threads :%lld\n", atoll(pcShortThreads) );
  	fprintf( stdout, "long  threads :%lld\n", atoll(pcLongThreads) );
	
	  logFmtSyn( LVNOR, "short threads :%lld", atoll(pcShortThreads) );
	  logFmtSyn( LVNOR, "long  threads :%lld", atoll(pcLongThreads) );
	}
	else
	{
		fprintf( stdout, "fail\n" );
		logFmtSyn( LVNOR, "fail" );
	}
	
	return;
}

/**********************************************************************
��������: managerShowKeysDeal
��������: ����˴��� show keys ����������
��    ����
��    ��: �����
����ʱ��: 20161019
**********************************************************************/
void managerShowKeysDeal( )
{
/*
�������ݽṹ������+����
�������ݽṹ���������+KEY������
*/
	char pcSendLen[DATA_BUF_LEN + 1];
	char pcSendBuf[OPERATE_LEN + 1];
	char cResult;
	char pcTotalKeys[INT64_LEN + 1];
	char pcRecvLen[DATA_BUF_LEN + 1];
	int  iSendLen;
	int  iRecvLen;
	int  iRecvSize;
	int  iSocket;
	int  iRet;
	
	logFmtSyn( LVSYS, "��ʼִ�к�����%s", __FUNCTION__ );
	
	memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
	strcpy( pcSendBuf, MANAGER_SHOW_KEYS_CNT );
	
	iSendLen = strlen(pcSendBuf);
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
	
	iSocket = connectToServer( pcgIp, pcgPort );
	if (iSocket == -1)
	{
		SHOW_ERRMSG("���ӵ������ʧ��");
		return;
	}
	
	/*���ͳ�����*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	/*��������*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("�������ݵ������ʧ��");
		return;
	}
	
	iRet = waitFd( iSocket, WAIT_IN, TIME_OUT );
	if (iRet == -1)
	{
		SHOW_ERRMSG("�ȴ����շ��������ʧ��");
		return;
	}
	
	/*�������ݳ���*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*���ղ������*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("���շ��������ʧ��");
		return;
	}
	
	fprintf( stdout, "result:\n" );
	logFmtSyn( LVNOR, "result:" );
	
	if (cResult == '0')
	{
		/*����KEY������*/
	  MANAGER_RECV( iSocket, iRet, pcTotalKeys, iRecvLen, INT64_LEN );
	  
	  fprintf( stdout, "keys  total   :%lld\n", atoll(pcTotalKeys) );
  	logFmtSyn( LVNOR, "keys  total   :%lld", atoll(pcTotalKeys) );
	}
	else
	{
		fprintf( stdout, "fail\n" );
		logFmtSyn( LVNOR, "fail" );
	}
	
	return;
}

int connectToServer( char *pcaIp, char *pcaPort )
{
	int iSocket;                 /*socket�ļ�������*/
  struct sockaddr_in servAddr; /*��������ַ��Ϣ�ṹ��*/
  struct sockaddr_in addr;     /*���յ��Ŀͻ�����Ϣ�ṹ��*/
  
  #ifdef SYSTEM_WINDOWS_64
	  /*�����׽ӿ�*/
    iSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if (iSocket == INVALID_SOCKET)
    {
    	fprintf( stderr, "�����׽ӿ�ʧ�ܣ�error[%d]\n", GetLastError() );
    	logFmtSyn( LVERR, "�����׽ӿ�ʧ�ܣ�error[%d]", GetLastError() );
      return -1;
    }
  #else
    /*�����׽ӿ�*/
    iSocket = socket( AF_INET, SOCK_STREAM, 0 );
    if (iSocket == INVALID_SOCKET)
    {
    	fprintf( stderr, "�����׽ӿ�ʧ�ܣ�error[%d]\n", errno );
    	logFmtSyn( LVERR, "�����׽ӿ�ʧ�ܣ�error[%d]", errno );
      return -1;
    }
  #endif
  
  servAddr.sin_family = AF_INET;
  servAddr.sin_port = htons(atoi(pcaPort));
  servAddr.sin_addr.s_addr = inet_addr(pcaIp);
      
  /*����������FTP����*/
  if (connect(iSocket, (struct sockaddr *)(&servAddr), sizeof(struct sockaddr)) < 0)
  {
    fprintf( stderr, "���ӷ����ʧ�ܣ�error[%d]\n", GetLastError() );
    logFmtSyn( LVERR, "�����׽ӿ�ʧ�ܣ�error[%d]", GetLastError() );
    return -1;
  }
    
  return iSocket;
}

int waitFd( int iFd, int iFlag, int iTimeOut )
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
��������AllTrim
�������ܣ�ȥ��ͷβ�ո� 
��ڲ�����
��    һ���ַ���     I
��    �أ�1��ʾ���ַ�Ϊ�ո�0��ʾ����
����ʱ�䣺20130226
*********************************************************/
char *AllTrim ( char *pcaStr )
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
������  ��checkPort
�������ܣ����˿��Ƿ���ȷ
��ڲ�����
��    һ���˿�                I
��    �أ�1��ʾ���ϣ�0��ʾ������
����ʱ�䣺20151102
*********************************************************/
int checkPort( char *pcaPort )
{
	if (isDigitals( pcaPort, pcaPort + strlen(pcaPort)) == 0 || (atoi(pcaPort) < 0 || atoi(pcaPort) > 65535))
  {
  	return 0;
  }
  else
  {
  	return 1;
  }
}

/********************************************************
������  ��checkIP
�������ܣ����IP�Ƿ���ȷ
��ڲ�����
��    һ��IP                I
��    �أ�1��ʾ���ϣ�0��ʾ������
����ʱ�䣺20131024
*********************************************************/
int checkIP( char *pcaIP )
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
  if (!isInt(pcValue, pcValue + strlen(pcValue)))
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
  if (!isInt(pcValue, pcValue + strlen(pcValue)))
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
  if (!isInt(pcValue, pcValue + strlen(pcValue)))
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
  if (!isInt(pcValue, pcValue + strlen(pcValue)))
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
��������isBlank
�������ܣ�����ַ����Ƿ�Ϊ�ո� 
��ڲ�����
��    һ���ַ�     I
��    �أ�1��ʾ���ַ�Ϊ�ո�0��ʾ����
����ʱ�䣺20130226
*********************************************************/
int isBlank( char c )
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
��������isInt
�������ܣ�����ַ�����Ϊ���� 
��ڲ�����
��    һ���������Ŀ�ʼ     I
��    �����������Ľ���     I 
��    �أ�1��ʾ���ַ�����Ϊ���֣�0��ʾ����
����ʱ�䣺20130226
*********************************************************/
int isInt( char *pcaBufStart, char *pcaBufEnd )
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
��������isDigitals
�������ܣ�����ַ����Ƿ�Ϊ���� 
��ڲ�����
��    һ���������Ŀ�ʼ     I
��    �����������Ľ���     I 
��    �أ�1��ʾ���ַ�����Ϊ���֣�0��ʾ����
����ʱ�䣺20130226
*********************************************************/
int isDigitals( char *pcaBufStart, char *pcaBufEnd )
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
��������: strcmpIgnore
��������: �Ƚϣ����Դ�Сд 
��    ��: ����� 
����ʱ��: 
��    ����
��    һ����һ��������   I 
��    �����ڶ���������   I
��    �أ�0��ʾ��ȣ�������ʾʧ�� 
**********************************************************************/
int strcmpIgnore( char *p, char *p1 )
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
��������: strcmpIgnore
��������: �Ƚϣ����Դ�Сд 
��    ��: ����� 
����ʱ��: 
��    ����
��    һ����һ��������   I 
��    �����ڶ���������   I
��    �أ�0��ʾ��ȣ�������ʾʧ�� 
**********************************************************************/
int strncmpIgnore( char *p, char *p1, int iaSize )
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

/**********************************************************************
�������ƣ�logInitSyn
�������ܣ�ͬ����־��ʼ��
��    ����
��    һ����־����          I
��    �أ�0
��    �ߣ������
����ʱ�䣺20160809
**********************************************************************/
int logInitSyn( int iaLevel )
{
  /*������־����*/
  igSynDebugLevel = iaLevel;
  
  return SUCCESS;
}

/**********************************************************************
�������ƣ�logFmtSyn
�������ܣ�����ʽ����ͬ����־
��    ����
��    һ�����𣨼���С��ϵͳ����ľͲ�����־��    I
��    �����������ڵ��ļ�   I
��    �����������ڵ���     I
��    �ģ���־�ļ�����     I
��    �壺��ʽ��           I
��    ������������         I
��    �ߣ������
����ʱ�䣺20160809
**********************************************************************/
int  logFmtSyn( int iaLevel, char *pcaFile, int iaLine, const char *pcaFunc, char *pcaLogFile, const char *aFmt, ... )
{
  va_list ap;
  FILE    *fp;
  char    pcBuf[MAX_SYN_LOG_SIZE+1];
  char    pcLogPath[FILE_PATH_LEN];
  struct timeb  tp;
  struct tm *timeNow;
  
  if (iaLevel < igSynDebugLevel)
  {
    return SUCCESS;
  }
  
  va_start( ap, aFmt );  
  vsprintf( pcBuf, aFmt, ap );
  va_end( ap );
  
  ftime(&tp);
  timeNow = ( localtime(&(tp.time)) );
  sprintf( pcLogPath, "%s.%02d%02d", pcaLogFile, timeNow->tm_mon+1, timeNow->tm_mday );
  fp = fopen( pcLogPath, "ab+" );
  if (fp == NULL)
  {
    fclose(fp);
    if (fp == NULL)
    {
      return ERROR;
    }
    fp = fopen( pcLogPath, "wb+" );
  }
  
  fprintf( fp, FMT, clock(), timeNow->tm_year+1900, timeNow->tm_mon+1, timeNow->tm_mday,
    timeNow->tm_hour, timeNow->tm_min, timeNow->tm_sec, tp.millitm, pcaFile, pcaFunc, iaLine, pcBuf );
  fprintf( fp, "\n\n" );
  fflush(fp);
  fclose(fp);
  return SUCCESS;
}

/**********************************************************************
�������ƣ�logSyn
�������ܣ�����ʽ����ͬ����־
��    ����
��    һ�����𣨼���С��ϵͳ����ľͲ�����־��    I
��    �����������ڵ��ļ�   I
��    �����������ڵ���     I
��    �ģ���־�ļ�����     I
��    �壺��־������       I
��    ������־����         I
��    �ߣ������
����ʱ�䣺20160812
**********************************************************************/
int logSyn( int iaLevel, char *pcaFile, int iaLine, const char *pcaFunc, char *pcaLogFile,  char *pcaBuf, int iaSize )
{
  FILE    *fp;
  char    pcLogPath[FILE_PATH_LEN];
  struct timeb  tp;
  struct tm *timeNow;
  
  if (iaLevel < igSynDebugLevel)
  {
    return SUCCESS;
  }
  
  ftime(&tp);
  timeNow = ( localtime(&(tp.time)) );
  sprintf( pcLogPath, "%s.%02d%02d", pcaLogFile, timeNow->tm_mon+1, timeNow->tm_mday );
  fp = fopen( pcLogPath, "ab+" );
  if (fp == NULL)
  {
    fclose(fp);
    if (fp == NULL)
    {
      return ERROR;
    }
    fp = fopen( pcLogPath, "wb+" );
  }
  
  fprintf( fp, FMT, clock(), timeNow->tm_year+1900, timeNow->tm_mon+1, timeNow->tm_mday,
    timeNow->tm_hour, timeNow->tm_min, timeNow->tm_sec, tp.millitm, pcaFile, pcaFunc, iaLine, "" );
  fwrite( pcaBuf, 1, iaSize, fp );
  fprintf( fp, "\n" );
  fflush(fp);
  fclose(fp);
  return SUCCESS;
}
