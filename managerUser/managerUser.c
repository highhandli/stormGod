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

/*从开始忽略空格*/
#define IGNORE_BLANK_START( start, end ) \
  {\
  	while ((IS_BLANK( *(start) )) && start < end)\
  	{\
	  	start++;\
	  }\
  }

/*从结束忽略空格*/
#define IGNORE_BLANK_END( start, end ) \
  {\
  	while ((IS_BLANK( *(end) )) && end > start)\
  	{\
	  	end--;\
	  }\
  }

/*寻找第一个不是字母的字符*/
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
  
/*寻找第一个不是名字里面的字符，也不是空格，也不是'.'或者'->'的字符*/
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
#define MAX_SYN_LOG_SIZE  8192        /*同步日志的最大值*/

#ifdef FILE_PATH_LEN
  #undef FILE_PATH_LEN
#endif
#define FILE_PATH_LEN     256         /*文件路径的最大值*/

#ifdef NAME_LEN
  #undef NAME_LEN
#endif
#define NAME_LEN          64          /*文件名称的最大值*/

#ifdef SUCCESS
  #undef SUCCESS
#endif
#define SUCCESS           0           /*成功标识*/

#ifdef ERROR
  #undef ERROR
#endif
#define ERROR             1           /*失败标识*/

#define LVSYS  9,__FILE__,__LINE__,__FUNCTION__,"managerUser9.debug"    /* 系统信息日志级别   */
#define LVERR  8,__FILE__,__LINE__,__FUNCTION__,"managerUser8.debug"    /* 错误信息日志级别   */
#define LVWAR  7,__FILE__,__LINE__,__FUNCTION__,"managerUser7.debug"    /* 警告信息日志级别   */
#define LVNOR  6,__FILE__,__LINE__,__FUNCTION__,"managerUser6.debug"    /* 正常信息日志级别   */
#define LVBUG  5,__FILE__,__LINE__,__FUNCTION__,"managerUser5.debug"    /* 除错信息日志级别   */
#define LVDET  4,__FILE__,__LINE__,__FUNCTION__,"managerUser4.debug"    /* 详细信息日志级别   */

int igSynDebugLevel;            /*同步日志级别*/

#define FMT "%012ld %04d-%02d-%02d %02d:%02d:%02d:%03d [%s:%s:%d]\n%s"

/****************************client.h***************************/
#define  MAX_KEY_LEN          512            /* KEY 的最大长度*/
#define  MAX_VALUE_LEN        4096           /* VALUE 的最大长度*/
#define  SELECT_EACH_SIZE     50             /*查询操作时，每次向服务端获取的数量*/
                              
#define  DATA_BUF_LEN         10             /*客户端过来的长度域*/
#define  BAT_RETURN           524288         /*响应批量查询请求时每次返回的数量*/
#define  OPERATE_LEN          4              /*客户端过来的操作长度*/
#define  KEY_VALUE_LENGTH     4              /*客户端过来的数据长度*/
#define  MANAGER_SELECT_KEY_CNT       "2101"         /*管理客户端过来的查询KEY操作*/
#define  MANAGER_SELECT_VALUE_CNT     "2102"         /*管理客户端过来的查询VALUE操作*/
#define  MANAGER_SELECT_KEY_VALUE_CNT "2103"         /*管理客户端过来的查询KEY VALUE操作*/
#define  MANAGER_SELECT_MIN_CNT       "2104"         /*管理客户端过来的查询最小KEY操作*/
#define  MANAGER_SELECT_MAX_CNT       "2105"         /*管理客户端过来的查询最大KEY操作*/
#define  MANAGER_SELECT_COUNT_CNT     "2106"         /*管理客户端过来的查询KEY数量操作*/
#define  MANAGER_DELETE_CNT           "2201"         /*管理客户端过来的删除操作*/
#define  MANAGER_UNLOAD_CNT           "2301"         /*管理客户端过来的导出数据到文件操作*/
#define  MANAGER_LOAD_CNT             "2401"         /*管理客户端过来的从文件导入数据操作*/
#define  MANAGER_EXIT_CNT             "2501"         /*管理客户端过来的正常结束进程操作*/
#define  MANAGER_FLUSH_CNT            "2601"         /*管理客户端过来的将内存数据写入到硬盘操作*/
#define  MANAGER_RAM_TO_DISC_CNT      "2602"         /*管理客户端过来的将内存数据写入到硬盘操作*/
#define  MANAGER_ALTER_THREADS_CNT    "2701"         /*管理客户端过来的增加线程数量操作*/
#define  MANAGER_ALTER_DISCS_CNT      "2702"         /*管理客户端过来的增加文件最大值操作*/
#define  MANAGER_ALTER_UPPERS_CNT     "2703"         /*管理客户端过来的修改文件递增值操作*/
#define  MANAGER_SHOW_RAMS_CNT        "2801"         /*管理客户端过来的显示内存使用情况操作*/
#define  MANAGER_SHOW_DISCS_CNT       "2802"         /*管理客户端过来的显示硬盘使用情况操作*/
#define  MANAGER_SHOW_THREADS_CNT     "2803"         /*管理客户端过来的显示长连接和短连接线程数量操作*/
#define  MANAGER_SHOW_KEYS_CNT        "2804"         /*管理客户端过来的显示key数量操作*/

#define  MANAGER_INTERRUPT_CNT        "2900"         /*管理客户端过来的终止操作*/

#define  NO_WHERE_KEY_VALUE           "0000"         /*没有WHERE条件*/
#define  KEY_OPEN_OPEN                "0001"         /*KEY值区间操作，左开右开()*/
#define  KEY_OPEN_CLOSE               "0002"         /*KEY值区间操作，左开右闭(]*/
#define  KEY_CLOSE_OPEN               "0003"         /*KEY值区间操作，左闭右开[)*/
#define  KEY_CLOSE_CLOSE              "0004"         /*KEY值区间操作，左闭右闭[]*/
#define  VALUE_OPEN_OPEN              "0005"         /*VALUE值区间操作，左开右开()*/
#define  VALUE_OPEN_CLOSE             "0006"         /*VALUE值区间操作，左开右闭(]*/
#define  VALUE_CLOSE_OPEN             "0007"         /*VALUE值区间操作，左闭右开[)*/
#define  VALUE_CLOSE_CLOSE            "0008"         /*VALUE值区间操作，左闭右闭[]*/

#define  INT_LEN           10                 /*整型数据的字符串长度*/
#define  INT64_LEN         20                 /*64位整型数据的字符串长度*/
#define  KEY_LEN 12
#define  VALUE_LEN 12 

#define  WAIT_OUT 0
#define  WAIT_IN  1
#define  TIME_OUT 60
#define  NO_TIME_OUT 0     /*不超时等待*/
#define  TIME_STEP   1000     /*轮流检查时间*/
#define  NULL_SELECT_ID    "00000000000000000000" /*空的SELECT ID（数量为INT64_LEN）*/

#define  WAIT_SERVER_TIME 10*60 /*等待服务端返回的最长时间*/

#define SOCKET_SLEEP_TIME      100000   /*遇阻塞时的睡眠时间，UNIX单位：微秒*/
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
		SHOW_ERRMSG("接收服务端数据失败");\
		fprintf( stdout, "recvTmp[%d]\n", recvTmp );\
		closesocket( socket );\
		return;\
	}\
}

int  igGetFromKeyBoard;        /*从键盘获取输入*/

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
    	SHOW_ERRMSG("Winsock无法初始化!");
      WSACleanup();
      return 0;
    }
	#endif
	
	if (argc < 3)
	{
		fprintf( stdout, "请输入目标 IP ：\n" );
		memset( pcgIp, 0x00, sizeof(pcgIp) );
		gets( pcgIp );
		logFmtSyn( LVNOR, "获取输入数据：%s", pcgIp );
		AllTrim( pcgIp );
		
		fprintf( stdout, "请输入目标端口：\n" );
		memset( pcgPort, 0x00, sizeof(pcgPort) );
		gets( pcgPort );
		logFmtSyn( LVNOR, "获取输入数据：%s", pcgPort );
		AllTrim( pcgPort );
		
		
		if (!checkIP( pcgIp ))
		{
			SHOW_ERRMSG("IP地址不符合格式");
			return 0;
		}
		
		if (!checkPort( pcgPort ))
		{
			SHOW_ERRMSG("端口不符合格式");
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
			SHOW_ERRMSG("IP地址不符合格式");
			return 0;
		}
		
		if (!checkPort( pcgPort ))
		{
			SHOW_ERRMSG("端口不符合格式");
			return 0;
		}
	}
	
	logFmtSyn( LVNOR, "初始化成功，请输入命令" );
	fprintf( stdout, "初始化成功，请输入命令\n" );
	
	logInitSyn( 2 );
	
	while(1)
	{
		//SHOW_ERRMSG("请输入数据");
		memset( pcLine, 0x00, sizeof(pcLine) );
		gets( pcLine );
		logFmtSyn( LVNOR, "获取输入数据：%s", pcLine );
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
函数名称: managerDeal
函数功能: 处理管理端接收到的数据
参    数：
第    一：数据开始地址         I
第    二：数据结束地址         I
作    者: 李高文
建立时间: 20161018
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
	
	logFmtSyn( LVSYS, "开始执行函数：%s", __FUNCTION__ );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
	/*取第一个关键字*/
	{
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	//SHOW_ERRMSG("只有操作符，缺少操作数据");
	    //return;
	    pcBlank = pcBufEnd + 1;
	  }
	  
	  if (pcBlank - pcBufStart >= KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("该操作类型不存在");
	    return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
  }
  
  logFmtSyn( LVSYS, "操作：%s", pcKeyWord );
	
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
		fprintf( stdout, "该操作类型[%s]不存在", pcKeyWord );
		SHOW_ERRMSG("该操作类型不存在");
	  return;
	}

	return;
}

/**********************************************************************
函数名称: managerSelectDeal
函数功能: 管理端处理 select 操作的数据
参    数：
第    一：数据开始地址         I
第    二：数据结束地址         I
作    者: 李高文
建立时间: 20161018
**********************************************************************/
void managerSelectDeal( char *pcaBufStart, char *pcaBufEnd )
{
  char pcKeyWord[KEY_WORD_LEN];
	char *pcBlank;
	char *pcBufStart;
	char *pcBufEnd;
	
	logFmtSyn( LVSYS, "开始执行函数：%s", __FUNCTION__ );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if (pcBufStart == pcBufEnd)
	{
		SHOW_ERRMSG("只有操作符，缺少操作数据");
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
		SHOW_ERRMSG("操作类型不存在");
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
函数名称: managerSelectMinDeal
函数功能: 管理端处理 select min 操作的数据
参    数：
第    一：数据开始地址         I
第    二：数据结束地址         I
作    者: 李高文
建立时间: 20161020
**********************************************************************/
void managerSelectMinDeal( char *pcaBufStart, char *pcaBufEnd )
{
/*
发送数据结构：长度+操作
接收数据结构：操作结果+KEY值长度+KEY值+VALUE值长度+VALUE值
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
	
	logFmtSyn( LVSYS, "开始执行函数：%s", __FUNCTION__ );
	
	memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
	strcpy( pcSendBuf, MANAGER_SELECT_MIN_CNT );
	
	iSendLen = strlen(pcSendBuf);
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
	
	iSocket = connectToServer( pcgIp, pcgPort );
	if (iSocket == -1)
	{
		SHOW_ERRMSG("连接到服务端失败");
		return;
	}
	
	/*发送长度域*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	/*发送数据*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	iRet = waitFd( iSocket, WAIT_IN, TIME_OUT );
	if (iRet == -1)
	{
		SHOW_ERRMSG("等待接收服务端数据失败");
		return;
	}
	
	/*接收数据长度*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*接收数据*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("接收服务端数据失败");
		return;
	}
	
	fprintf( stdout, "result:\n" );
	logFmtSyn( LVNOR, "result:" );
	
	if (cResult == '0')
	{
		/*接收KEY值长度*/
		MANAGER_RECV( iSocket, iRet, pcKeyLen, iRecvLen, KEY_VALUE_LENGTH );
	  iKeyLen = atoi( pcKeyLen );
	  
	  /*接收KEY值*/
	  MANAGER_RECV( iSocket, iRet, pcKey, iRecvLen, iKeyLen );
	  
	  /*接收VALUE长度*/
	  MANAGER_RECV( iSocket, iRet, pcValueLen, iRecvLen, KEY_VALUE_LENGTH );
	  iValueLen = atoi( pcValueLen );
	  
	  /*接收VALUE值*/
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
函数名称: managerSelectMaxDeal
函数功能: 管理端处理 select max 操作的数据
参    数：
第    一：数据开始地址         I
第    二：数据结束地址         I
作    者: 李高文
建立时间: 20161020
**********************************************************************/
void managerSelectMaxDeal( char *pcaBufStart, char *pcaBufEnd )
{
/*
发送数据结构：长度+操作
接收数据结构：操作结果+KEY值长度+KEY值+VALUE值长度+VALUE值
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
	
	logFmtSyn( LVSYS, "开始执行函数：%s", __FUNCTION__ );
	
	memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
	strcpy( pcSendBuf, MANAGER_SELECT_MAX_CNT );
	
	iSendLen = strlen(pcSendBuf);
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
	
	iSocket = connectToServer( pcgIp, pcgPort );
	if (iSocket == -1)
	{
		SHOW_ERRMSG("连接到服务端失败");
		return;
	}
	
	/*发送长度域*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	/*发送数据*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	iRet = waitFd( iSocket, WAIT_IN, TIME_OUT );
	if (iRet == -1)
	{
		SHOW_ERRMSG("等待接收服务端数据失败");
		return;
	}
	
	/*接收数据长度*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*接收数据*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("接收服务端数据失败");
		return;
	}
	
	fprintf( stdout, "result:\n" );
	logFmtSyn( LVNOR, "result:" );
	
	if (cResult == '0')
	{
		/*接收KEY值长度*/
		MANAGER_RECV( iSocket, iRet, pcKeyLen, iRecvLen, KEY_VALUE_LENGTH );
	  iKeyLen = atoi( pcKeyLen );
	  
	  /*接收KEY值*/
	  MANAGER_RECV( iSocket, iRet, pcKey, iRecvLen, iKeyLen );
	  
	   /*接收VALUE长度*/
	  MANAGER_RECV( iSocket, iRet, pcValueLen, iRecvLen, KEY_VALUE_LENGTH );
	  iValueLen = atoi( pcValueLen );
	  
	  /*接收VALUE值*/
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
函数名称: managerSelectCountDeal
函数功能: 管理端处理 select count 操作的数据
参    数：
第    一：数据开始地址         I
第    二：数据结束地址         I
作    者: 李高文
建立时间: 20161020
**********************************************************************/
void managerSelectCountDeal( char *pcaBufStart, char *pcaBufEnd )
{
/*
发送数据结构：长度+操作+值区间（是否有条件、左开右开、左开右闭、左闭右开、左闭右闭）+KEY开始值长度+KEY开始值+KEY结束值长度+KEY结束值
接收数据结构：操作结果+COUNT值
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
	
	logFmtSyn( LVSYS, "开始执行函数：%s", __FUNCTION__ );
	
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
	  /*取出 where 关键字*/
	  {
	    pcBlank = strchr( pcBufStart, ' ' );
	    if (pcBlank == NULL)
	    {
	    	SHOW_ERRMSG("语句不正确");
	    	return;
	    }
	    
	    if (pcBlank - pcBufStart > KEY_WORD_LEN)
	    {
	    	SHOW_ERRMSG("语句不正确");
	    	return;
	    }
	    
	    memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	    memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	    
	    if (strcmpIgnore( pcKeyWord, "WHERE" ))
	    {
	    	SHOW_ERRMSG("语句不正确");
	  	  return;
	    }
	    
	    pcBufStart = pcBlank + 1;
    }
    
    logFmtSyn( LVSYS, "处理 WHERE 关键字成功" );
    
    /*取出 key 关键字*/
	  {
	  	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	    if (pcBufStart == pcBufEnd)
	    {
	  	  SHOW_ERRMSG("语句不正确");
	  	  return;
	    }
	    
	    /*寻找第一个不是字母的字符*/
	    pcTmp = pcBufStart;
	    IS_NOT_LETTER( pcTmp, pcBufEnd );
	    if (pcTmp == pcBufEnd)
	    {
	  	  SHOW_ERRMSG("语句不正确");
	  	  return;
	    }
	    
	    /*如果是空格或者开闭字符*/
	    if (isBlank(*pcTmp) || (*pcTmp) == '[' || (*pcTmp) == '(')
	    {
	    	pcBlank = pcTmp;
	    	
	    	if (pcBlank - pcBufStart > KEY_WORD_LEN)
	      {
	      	SHOW_ERRMSG("语句不正确");
	    	  return;
	      }
	      memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	      memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	    
	      if (strcmpIgnore( pcKeyWord, "KEY" ))
	      {
	    	  SHOW_ERRMSG("语句不正确");
	  	    return;
	      }
	    }
	    else
	    {
	    	SHOW_ERRMSG("语句不正确");
	  	  return;
	    }
	    
	    pcBufStart = pcTmp;
    }
    
    logFmtSyn( LVSYS, "处理 KEY 关键字成功" );
    
    /*指向 key 后面的字符*/
	  {
	  	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	    if (pcBufStart == pcBufEnd)
	    {
	  	  SHOW_ERRMSG("语句不正确");
	  	  return;
	    }
	    
	    /*如果不是开闭字符*/
	    if (!((*pcBufStart) == '[' || (*pcBufStart) == '('))
	    {
	    	SHOW_ERRMSG("语句不正确");
	  	  return;
	    }
	  }
	  
	  cValueStart = *pcBufStart;
    pcBufStart++;
    
	  /*取出第一个key值*/
	  {
	    /*搜索第一个单引号*/
	    IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	    if (pcBufStart == pcBufEnd)
	    {
	      SHOW_ERRMSG("语句不正确");
	      return;
	    }
	    
	    /*如果不是单引号*/
	    if (!((*pcBufStart) == '\''))
	    {
	    	SHOW_ERRMSG("语句不正确");
	      return;
	    }
	    pcBufStart++;
	    
	    /*搜索第二个单引号*/
	    pcTmp = strchr( pcBufStart, '\'' );
	    if (pcTmp == NULL || pcTmp >= pcBufEnd)
	    {
	      SHOW_ERRMSG("语句不正确");
	      return;
	    }
	    
	    /*取出第一个key值*/
	    if (pcTmp - pcBufStart > MAX_KEY_LEN)
	    {
	    	SHOW_ERRMSG("KEY值过长");
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
	  
	  /*取出第二个key值*/
	  {
	    /*搜索逗号*/
	    IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	    if (pcBufStart == pcBufEnd)
	    {
	      SHOW_ERRMSG("语句不正确");
	      return;
	    }
	    
	    /*如果不是逗号*/
	    if (!((*pcBufStart) == ','))
	    {
	    	SHOW_ERRMSG("语句不正确");
	      return;
	    }
	    pcBufStart++;
	    
	    /*搜索第三个单引号*/
	    IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	    if (pcBufStart == pcBufEnd)
	    {
	      SHOW_ERRMSG("语句不正确");
	      return;
	    }
	    
	    /*如果不是单引号*/
	    if (!((*pcBufStart) == '\''))
	    {
	    	SHOW_ERRMSG("语句不正确");
	      return;
	    }
	    pcBufStart++;
	    
	    /*搜索第四个单引号*/
	    pcTmp = strchr( pcBufStart, '\'' );
	    if (pcTmp == NULL || pcTmp >= pcBufEnd)
	    {
	      SHOW_ERRMSG("语句不正确");
	      return;
	    }
	    
	    /*取出第二个key值*/
	    if (pcTmp - pcBufStart > MAX_KEY_LEN)
	    {
	    	SHOW_ERRMSG("KEY值过长");
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
	  
	  /*取出最后的开闭符*/
    IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if ((*pcBufStart) == ']' || (*pcBufStart) == ')')
	  {
	  	cValueEnd = (*pcBufStart);
	  }
	  else
	  {
	  	SHOW_ERRMSG("语句不正确");
	    return;
	  }
	  
	  /*确定值区间*/
	  memset( pcValueType, 0x00, sizeof(pcValueType) );
	  if (cValueStart == '(' && cValueEnd == ')')
	  {
	  	if (strcmp( pcKeyStart, pcKeyEnd ) >= 0)
	  	{
		  	SHOW_ERRMSG("第一个值只可以小于第二个值");
	      return;
		  }
	  	memcpy( pcValueType, KEY_OPEN_OPEN, OPERATE_LEN );
	  }
	  else
	  if (cValueStart == '(' && cValueEnd == ']')
	  {
	  	if (strcmp( pcKeyStart, pcKeyEnd ) > 0)
	  	{
			  SHOW_ERRMSG("第一个值只可以小于或等于第二个值");
	      return;
		  }
	  	memcpy( pcValueType, KEY_OPEN_CLOSE, OPERATE_LEN );
	  }
	  else
	  if (cValueStart == '[' && cValueEnd == ')')
	  {
	  	if (strcmp( pcKeyStart, pcKeyEnd ) >= 0)
	  	{
		  	SHOW_ERRMSG("第一个值只可以小于第二个值");
	      return;
		  }
	  	memcpy( pcValueType, KEY_CLOSE_OPEN, OPERATE_LEN );
	  }
	  else
	  {
	  	if (strcmp( pcKeyStart, pcKeyEnd ) > 0)
	  	{
			  SHOW_ERRMSG("第一个值只可以小于或等于第二个值");
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
		SHOW_ERRMSG("连接到服务端失败");
		return;
	}

	/*发送长度域*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	/*发送数据*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	SHOW_ERRMSG("发送数据到服务端成功，请耐心等待服务端处理");
	
	/*等待接收*/
	iRet = waitFd( iSocket, WAIT_IN, WAIT_SERVER_TIME );
	
	/*如果网络错误*/
  if (iRet == -1)
	{
		SHOW_ERRMSG("等待接收数据失败");
	  return;
	}
	
	/*接收长度*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*如果值是0，表示链路保持*/
	if (atoi( pcRecvLen ) == 0)
	{
		SHOW_ERRMSG("等待接收数据失败");
	  return;
	}

	/*接收数据*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("接收服务端数据失败");
		return;
	}
	
	fprintf( stdout, "result:\n" );
	logFmtSyn( LVNOR, "result:" );
	
	if (cResult == '0')
	{
		/*接收COUNT值*/
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
函数名称: managerSelectKeyValueDeal
函数功能: 管理端处理 select key value 操作的数据
参    数：
第    一：数据开始地址         I
第    二：数据结束地址         I
作    者: 李高文
建立时间: 20161020
**********************************************************************/
void managerSelectKeyValueDeal( char *pcaBufStart, char *pcaBufEnd )
{
/*
发送数据结构：长度+操作+查询ID+获取数量+获取开始位置+值区间（左开右开、左开右闭、左闭右开、左闭右闭）+KEY开始值长度+KEY开始值+KEY结束值长度+KEY结束值
接收数据结构：操作结果+查询ID+当前返回数量+(KEY、VALUE的长度及值)
发送过去的数据，如果不补空格，服务端打出的日志可能会少。统一，整数前补0，字符后补空格，服务端校验字符是否一样时，得忽略空格
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
	int  iKeyFlag;    /*0表示查询字段没有key，1表示有*/
	int  iValueFlag;  /*0表示查询字段没有value，1表示有*/
	int  iWhereFlag;  /*0表示 where key。1表示where value*/
	int  iSendLen;
	int  iRecvLen;
	int  iSocket;
	int  iRet;
	int  iRows;
	int  iKeyValueLen;
	int  iTmp;
	__int64 llRowStartIndex;
	
	logFmtSyn( LVSYS, "开始执行函数：%s", __FUNCTION__ );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if (pcBufStart == pcBufEnd)
	{
	  SHOW_ERRMSG("语句不正确");
	  return;
	}
	
	/*寻找第一个不是字母的字符*/
	pcTmp = pcBufStart;
	IS_NOT_LETTER( pcTmp, pcBufEnd );
	if (pcTmp == pcBufEnd)
	{
	  SHOW_ERRMSG("语句不正确");
	  return;
	}
	
	/*确定查询字段*/
	{
		iKeyFlag = 0;
		iValueFlag = 0;
		
	  /*如果是空格*//*只允许是空格或者逗号*/
	  if (isBlank(*pcTmp))
	  {
	  	pcBlank = pcTmp;
	  	
	  	if (pcBlank - pcBufStart > KEY_WORD_LEN)
	    {
	    	SHOW_ERRMSG("语句不正确");
	    	return;
	    }
	    
	    memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	    memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	    
	    if (!strcmpIgnore( pcKeyWord, "KEY" ))
	    {
	    	iKeyFlag = 1;
	    	pcBufStart = pcBlank + 1;
	    	
	    	/*取下一个字符*/
	    	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	      if (pcBufStart == pcBufEnd)
	      {
		      SHOW_ERRMSG("语句不正确");
		      return;
	      }
	      
	      /*KEY后面只能跟VALUE*/
	      if ((*pcBufStart) == ',')
	      {
	      	pcBufStart++;
	      	
	      	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	        if (pcBufStart == pcBufEnd)
	        {
		        SHOW_ERRMSG("语句不正确");
		        return;
	        }
	        
	        pcBlank = strchr( pcBufStart, ' ' );
	        if (pcBlank == NULL || pcBlank > pcBufEnd)
	        {
	        	SHOW_ERRMSG("语句不正确");
	        	return;
	        }
	        
	        if (pcBlank - pcBufStart > KEY_WORD_LEN)
	        {
	        	SHOW_ERRMSG("语句不正确");
	        	return;
	        }
	        
	        memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	        memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	        
	        if (strcmpIgnore( pcKeyWord, "VALUE" ))
	        {
	        	SHOW_ERRMSG("语句不正确");
		        return;
	        }
	        
	        iValueFlag = 1;
	        
	        pcBufStart = pcBlank + 1;
	      }
	      else
	      /*不需要处理*/
	      if (IS_LETTER((*pcBufStart)))
	      {
	      }
	      else
	      {
		      SHOW_ERRMSG("语句不正确");
		      return;
	      }
	    }
	    else
	    if (!strcmpIgnore( pcKeyWord, "VALUE" ))
	    {
	    	iValueFlag = 1;
	    	
	    	pcBufStart = pcBlank + 1;
	    	
	    	/*取下一个字符*/
	    	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	      if (pcBufStart == pcBufEnd)
	      {
		      SHOW_ERRMSG("语句不正确");
		      return;
	      }
	      
	      /*VALUE后面只能跟KEY*/
	      if ((*pcBufStart) == ',')
	      {
	      	pcBufStart++;
	      	
	      	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	        if (pcBufStart == pcBufEnd)
	        {
		        SHOW_ERRMSG("语句不正确");
		        return;
	        }
	        
	        pcBlank = strchr( pcBufStart, ' ' );
	        if (pcBlank == NULL || pcBlank > pcBufEnd)
	        {
	        	SHOW_ERRMSG("语句不正确");
	        	return;
	        }
	        
	        if (pcBlank - pcBufStart > KEY_WORD_LEN)
	        {
	        	SHOW_ERRMSG("语句不正确");
	        	return;
	        }
	        
	        memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	        memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	        
	        if (strcmpIgnore( pcKeyWord, "KEY" ))
	        {
	        	SHOW_ERRMSG("语句不正确");
		        return;
	        }
	        
	        iKeyFlag = 1;
	        
	        pcBufStart = pcBlank + 1;
	      }
	      else
	      /*不需要处理*/
	      if (IS_LETTER((*pcBufStart)))
	      {
	      }
	      else
	      {
		      SHOW_ERRMSG("语句不正确");
		      return;
	      }
	    }
	    else
	    {
	    	SHOW_ERRMSG("语句不正确");
		    return;
	    }
	  }
	  else
	  /*如果是逗号*/
	  if ((*pcTmp) == ',')
	  {
	  	pcBlank = pcTmp;
	  	
	  	if (pcBlank - pcBufStart > KEY_WORD_LEN)
	    {
	    	SHOW_ERRMSG("语句不正确");
	    	return;
	    }
	    
	    memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	    memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	    
	    if (!strcmpIgnore( pcKeyWord, "KEY" ))
	    {
	    	iKeyFlag = 1;
	    	pcBufStart = pcBlank + 1;
	    	
	    	/*取下一个字符*/
	    	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	      if (pcBufStart == pcBufEnd)
	      {
		      SHOW_ERRMSG("语句不正确");
		      return;
	      }
	      
	      /*KEY后面只能跟VALUE*/
	      {
	        pcBlank = strchr( pcBufStart, ' ' );
	        if (pcBlank == NULL || pcBlank > pcBufEnd)
	        {
	        	SHOW_ERRMSG("语句不正确");
	        	return;
	        }
	        
	        if (pcBlank - pcBufStart > KEY_WORD_LEN)
	        {
	        	SHOW_ERRMSG("语句不正确");
	        	return;
	        }
	        
	        memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	        memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	        
	        if (strcmpIgnore( pcKeyWord, "VALUE" ))
	        {
	        	SHOW_ERRMSG("语句不正确");
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
	    	
	    	/*取下一个字符*/
	    	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	      if (pcBufStart == pcBufEnd)
	      {
		      SHOW_ERRMSG("语句不正确");
		      return;
	      }
	      
	      /*VALUE后面只能跟KEY*/
	      {
	        pcBlank = strchr( pcBufStart, ' ' );
	        if (pcBlank == NULL || pcBlank > pcBufEnd)
	        {
	        	SHOW_ERRMSG("语句不正确");
	        	return;
	        }
	        
	        if (pcBlank - pcBufStart > KEY_WORD_LEN)
	        {
	        	SHOW_ERRMSG("语句不正确");
	        	return;
	        }
	        
	        memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	        memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	        
	        if (strcmpIgnore( pcKeyWord, "KEY" ))
	        {
	        	SHOW_ERRMSG("语句不正确");
		        return;
	        }
	        
	        iKeyFlag = 1;
	        
	        pcBufStart = pcBlank + 1;
	      }
	    }
	    else
	    {
	    	SHOW_ERRMSG("语句不正确");
		    return;
	    }
	  }
	  else
	  {
	  	SHOW_ERRMSG("语句不正确");
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
  	SHOW_ERRMSG("语句不正确");
	  return;
  }
  
  /*设置初始值查询ID*/
  memcpy( pcSendBuf + OPERATE_LEN, NULL_SELECT_ID, INT64_LEN );
  
  /*设置获取数量*/
  memset( pcGetRows, 0x00, sizeof(pcGetRows) );
  sprintf( pcGetRows, "%0*d", INT64_LEN, SELECT_EACH_SIZE );
  memcpy( pcSendBuf + INT64_LEN + OPERATE_LEN, pcGetRows, INT64_LEN );
  
  /*设置获取开始位置*/
  memset( pcRowStartIndex, 0x00, sizeof(pcRowStartIndex) );
  sprintf( pcRowStartIndex, "%0*d", INT64_LEN, 0 );
  memcpy( pcSendBuf + OPERATE_LEN + INT64_LEN + INT64_LEN, pcRowStartIndex, INT64_LEN );
  
  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
  
  /*没有条件*/
	if (pcBufStart == pcBufEnd)
	{
	  memcpy( pcSendBuf + OPERATE_LEN + INT64_LEN + INT64_LEN + INT64_LEN, NO_WHERE_KEY_VALUE, OPERATE_LEN );
	}
	else
	{		
		/*取出 where 关键字*/
	  {
	    pcBlank = strchr( pcBufStart, ' ' );
	    if (pcBlank == NULL)
	    {
	    	SHOW_ERRMSG("语句不正确");
	    	return;
	    }
	    
	    if (pcBlank - pcBufStart > KEY_WORD_LEN)
	    {
	    	SHOW_ERRMSG("语句不正确");
	    	return;
	    }
	    
	    memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	    memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	    
	    if (strcmpIgnore( pcKeyWord, "WHERE" ))
	    {
	    	SHOW_ERRMSG("语句不正确");
	  	  return;
	    }
	    
	    pcBufStart = pcBlank + 1;
    }
        
    /*取出 key 或 value关键字*/
	  {
	  	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	    if (pcBufStart == pcBufEnd)
	    {
	  	  SHOW_ERRMSG("语句不正确");
	  	  return;
	    }
	    
	    /*寻找第一个不是字母的字符*/
	    pcTmp = pcBufStart;
	    IS_NOT_LETTER( pcTmp, pcBufEnd );
	    if (pcTmp == pcBufEnd)
	    {
	  	  SHOW_ERRMSG("语句不正确");
	  	  return;
	    }
	    
	    /*如果是空格或者开闭字符*/
	    if (isBlank(*pcTmp) || (*pcTmp) == '[' || (*pcTmp) == '(')
	    {
	    	pcBlank = pcTmp;
	    	
	    	if (pcBlank - pcBufStart > KEY_WORD_LEN)
	      {
	      	SHOW_ERRMSG("语句不正确");
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
	    	  SHOW_ERRMSG("语句不正确");
	  	    return;
	      }
	    }
	    else
	    {
	    	SHOW_ERRMSG("语句不正确");
	  	  return;
	    }
	    
	    pcBufStart = pcTmp;
    }
        
    /*指向 key 或 value 后面的字符*/
	  {
	  	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	    if (pcBufStart == pcBufEnd)
	    {
	  	  SHOW_ERRMSG("语句不正确");
	  	  return;
	    }
	    
	    /*如果不是开闭字符*/
	    if (!((*pcBufStart) == '[' || (*pcBufStart) == '('))
	    {
	    	SHOW_ERRMSG("语句不正确");
	  	  return;
	    }
	  }
	  	  
	  cValueStart = *pcBufStart;
    pcBufStart++;
    
	  /*取出第一个key或value值*/
	  {
	    /*搜索第一个单引号*/
	    IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	    if (pcBufStart == pcBufEnd)
	    {
	      SHOW_ERRMSG("语句不正确");
	      return;
	    }
	    
	    /*如果不是单引号*/
	    if (!((*pcBufStart) == '\''))
	    {
	    	SHOW_ERRMSG("语句不正确");
	      return;
	    }
	    pcBufStart++;
	    
	    /*搜索第二个单引号*/
	    pcTmp = strchr( pcBufStart, '\'' );
	    if (pcTmp == NULL || pcTmp >= pcBufEnd)
	    {
	      SHOW_ERRMSG("语句不正确");
	      return;
	    }
	    
	    if (iWhereFlag == 0)
	    {
	      /*取出第一个key值*/
	      if (pcTmp - pcBufStart > MAX_KEY_LEN)
	      {
	      	SHOW_ERRMSG("KEY值过长");
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
	    	/*取出第一个key值*/
	      if (pcTmp - pcBufStart > MAX_VALUE_LEN)
	      {
	      	SHOW_ERRMSG("VALUE值过长");
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
	  	  
	  /*取出第二个key值*/
	  {
	    /*搜索逗号*/
	    IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	    if (pcBufStart == pcBufEnd)
	    {
	      SHOW_ERRMSG("语句不正确");
	      return;
	    }
	    
	    /*如果不是逗号*/
	    if (!((*pcBufStart) == ','))
	    {
	    	SHOW_ERRMSG("语句不正确");
	      return;
	    }
	    pcBufStart++;
	    
	    /*搜索第三个单引号*/
	    IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	    if (pcBufStart == pcBufEnd)
	    {
	      SHOW_ERRMSG("语句不正确");
	      return;
	    }
	    
	    /*如果不是单引号*/
	    if (!((*pcBufStart) == '\''))
	    {
	    	SHOW_ERRMSG("语句不正确");
	      return;
	    }
	    pcBufStart++;
	    
	    /*搜索第四个单引号*/
	    pcTmp = strchr( pcBufStart, '\'' );
	    if (pcTmp == NULL || pcTmp >= pcBufEnd)
	    {
	      SHOW_ERRMSG("语句不正确");
	      return;
	    }
	    
	    if (iWhereFlag == 0)
	    {
	      /*取出第二个key值*/
	      if (pcTmp - pcBufStart > MAX_KEY_LEN)
	      {
	      	SHOW_ERRMSG("KEY值过长");
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
	    	/*取出第二个value值*/
	      if (pcTmp - pcBufStart > MAX_VALUE_LEN)
	      {
	      	SHOW_ERRMSG("VALUE值过长");
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
	  	  
	  /*取出最后的开闭符*/
    IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if ((*pcBufStart) == ']' || (*pcBufStart) == ')')
	  {
	  	cValueEnd = (*pcBufStart);
	  }
	  else
	  {
	  	SHOW_ERRMSG("语句不正确");
	    return;
	  }
	  
	  /*确定值区间*/
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
	    SHOW_ERRMSG("语句不正确");
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
		SHOW_ERRMSG("连接到服务端失败");
		return;
	}
	
	/*发送长度域*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	/*发送数据*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	SHOW_ERRMSG("发送数据到服务端成功，请耐心等待服务端处理");
	
	/*等待接收*/
	iRet = waitFd( iSocket, WAIT_IN, NO_TIME_OUT );
	
	/*如果网络错误*/
  if (iRet == -1)
	{
		SHOW_ERRMSG("等待接收数据失败");
	  return;
	}
	
	/*接收长度*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*接收数据*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("接收服务端数据失败");
		return;
	}
	
	fprintf( stdout, "result:\n" );
	logFmtSyn( LVNOR, "result:" );
	
	if (cResult == '0')
	{
		/*接收查询ID*/
		MANAGER_RECV( iSocket, iRet, pcSelectId, iRecvLen, INT64_LEN );
		
		/*接收当前返回数量*/
	  MANAGER_RECV( iSocket, iRet, pcRows, iRecvLen, INT64_LEN );
	  
	  /*显示数据*/
	  fprintf( stdout, "rows :%lld  start index :%lld  end index :%lld \n", 
	    atoll( pcRows ), 0, atoll( pcRows ) );
		logFmtSyn( LVNOR, "rows :%lld  start index :%lld  end index :%lld ", 
		  atoll( pcRows ), 0, atoll( pcRows ) );
		
		/*接收并显示数据*/
		for (iTmp = 0; iTmp < atoll( pcRows ); iTmp++)
	  {
	  	if (iKeyFlag == 1)
	  	{
	  	  /*接收 KEY 长度*/
	  	  MANAGER_RECV( iSocket, iRet, pcKeyValueLen, iRecvLen, KEY_VALUE_LENGTH );
	      iKeyValueLen = atoi( pcKeyValueLen );
	      
	      /*接收 KEY */
	      MANAGER_RECV( iSocket, iRet, pcKey, iRecvLen, iKeyValueLen );
	    }
	    
	    if (iValueFlag == 1)
	  	{
	  	  /*接收 VALUE 长度*/
	  	  MANAGER_RECV( iSocket, iRet, pcKeyValueLen, iRecvLen, KEY_VALUE_LENGTH );
	      iKeyValueLen = atoi( pcKeyValueLen );
	      
	      /*接收 VALUE */
	      MANAGER_RECV( iSocket, iRet, pcValue, iRecvLen, iKeyValueLen );
	    }
	    
	    /*显示数据*/
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
		
		/*设置当前开始位置*/
		llRowStartIndex = 0;
		
		/*根据用户输入，确定下一步操作*/
		while (1)
		{
			cGet = getch();
			//fprintf( stdout, "%c\n", cGet );
			/*完成*/
			if (cGet == 'f' )
			{
				/*组发送数据*//*当查询行数为0时，表示清除SELECT ID对应的缓冲区*/
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
	      	SHOW_ERRMSG("连接到服务端失败");
	      	return;
	      }
	      
	      /*发送长度域*/
	      iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	      if (iRet != DATA_BUF_LEN)
	      {
	      	SHOW_ERRMSG("发送数据到服务端失败");
	      	return;
	      }

	      /*发送数据*/
	      iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	      /*因为是让服务端清理数据，这里没必要判断成功或者失败*/
	      /*
	      if (iRet != iSendLen)
	      {
	      	SHOW_ERRMSG("发送数据到服务端失败");
	      	return;
	      }
	      */
				closesocket( iSocket );
				
				SHOW_ERRMSG("查询完成");
				return;
			}
			else
			/*取下一批*/
			if (cGet == 'n')
			{
				/*设置获取数量*/
				memset( pcGetRows, 0x00, sizeof(pcGetRows) );
				sprintf( pcGetRows, "%0*lld", INT64_LEN, SELECT_EACH_SIZE );
				
				/*设置获取开始位置*/
				llRowStartIndex += atoll(pcRows);
				memset( pcRowStartIndex, 0x00, sizeof(pcRowStartIndex) );
				sprintf( pcRowStartIndex, "%0*lld", INT64_LEN, llRowStartIndex );
			}
			else
			/*取上一批*/
			if (cGet == 'l' )
			{
				/*如果没有数据了*/
				if (llRowStartIndex == 0)
				{
					continue;
				}
				else
				{
					/*设置获取数量*/
				  memset( pcGetRows, 0x00, sizeof(pcGetRows) );
				  sprintf( pcGetRows, "%0*lld", INT64_LEN, SELECT_EACH_SIZE );
				
				  /*设置获取开始位置*/
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
			
			/*组发送数据*/
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
	    	SHOW_ERRMSG("连接到服务端失败");
	    	return;
	    }
	    
	    /*发送长度域*/
	    iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	    if (iRet != DATA_BUF_LEN)
	    {
	    	SHOW_ERRMSG("发送数据到服务端失败");
	    	return;
	    }
	    
	    /*发送数据*/
	    iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	    if (iRet != iSendLen)
	    {
	    	SHOW_ERRMSG("发送数据到服务端失败");
	    	return;
	    }
	    
	    SHOW_ERRMSG("发送数据到服务端成功，请耐心等待服务端处理");
	
	    /*等待接收*/
	    iRet = waitFd( iSocket, WAIT_IN, NO_TIME_OUT );
	    
	    /*如果网络错误*/
      if (iRet == -1)
	    {
	    	SHOW_ERRMSG("等待接收数据失败");
	      return;
	    }
	    
	    /*接收长度*/
	    MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	    
	    /*接收数据*/
	    cResult = 0;
      iRet = recv( iSocket, &cResult, 1, 0 );
      if (iRet <= 0)
	    {
	    	SHOW_ERRMSG("接收服务端数据失败");
	    	return;
	    }
	    
	    fprintf( stdout, "result:\n" );
	    logFmtSyn( LVNOR, "result:" );
	
	    if (cResult == '0')
	    {
	    	/*接收查询ID*/
		    MANAGER_RECV( iSocket, iRet, pcSelectId, iRecvLen, INT64_LEN );
	      
	      /*接收当前返回数量*/
	      MANAGER_RECV( iSocket, iRet, pcRows, iRecvLen, INT64_LEN );
	      
	      /*显示数据*/
	      fprintf( stdout, "rows :%lld  start index :%lld  end index :%lld \n", 
	        atoll( pcRows ), llRowStartIndex, llRowStartIndex + atoll( pcRows ) );
	    	logFmtSyn( LVNOR, "rows :%lld  start index :%lld  end index :%lld ", 
	    	  atoll( pcRows ), llRowStartIndex, llRowStartIndex + atoll( pcRows ) );
	    	
	    	/*接收并显示数据*/
	    	for (iTmp = 0; iTmp < atoll( pcRows ); iTmp++)
	      {
	      	if (iKeyFlag == 1)
	      	{
	      	  /*接收 KEY 长度*/
	      	  MANAGER_RECV( iSocket, iRet, pcKeyValueLen, iRecvLen, KEY_VALUE_LENGTH );
	          iKeyValueLen = atoi( pcKeyValueLen );
	          
	          /*接收 KEY */
	          MANAGER_RECV( iSocket, iRet, pcKey, iRecvLen, iKeyValueLen );
	        }
	        
	        if (iValueFlag == 1)
	      	{
	      	  /*接收 VALUE 长度*/
	      	  MANAGER_RECV( iSocket, iRet, pcKeyValueLen, iRecvLen, KEY_VALUE_LENGTH );
	          iKeyValueLen = atoi( pcKeyValueLen );
	          
	          /*接收 VALUE */
	          MANAGER_RECV( iSocket, iRet, pcValue, iRecvLen, iKeyValueLen );
	        }
	        
	        /*显示数据*/
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
函数名称: managerDeleteDeal
函数功能: 管理端处理 delete 操作的数据
参    数：
第    一：数据开始地址         I
第    二：数据结束地址         I
作    者: 李高文
建立时间: 20161018
**********************************************************************/
void managerDeleteDeal( char *pcaBufStart, char *pcaBufEnd )
{
/*
发送数据结构：长度+操作+值区间（左开右开、左开右闭、左闭右开、左闭右闭）+KEY开始值长度+KEY开始值+KEY结束值长度+KEY结束值+用户名+密码
接收数据结构：操作结果+删除数据总数量
发送过去的数据，如果不补空格，服务端打出的日志可能会少。统一，整数前补0，字符后补空格，服务端校验字符是否一样时，得忽略空格
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
	
	logFmtSyn( LVSYS, "开始执行函数：%s", __FUNCTION__ );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
  
  /*取出 where 关键字*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "WHERE" ))
	  {
	  	SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出 key 关键字*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  /*寻找第一个不是字母的字符*/
	  pcTmp = pcBufStart;
	  IS_NOT_LETTER( pcTmp, pcBufEnd );
	  if (pcTmp == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  /*如果是空格或者开闭字符*/
	  if (isBlank(*pcTmp) || (*pcTmp) == '[' || (*pcTmp) == '(')
	  {
	  	pcBlank = pcTmp;
	  	
	  	if (pcBlank - pcBufStart > KEY_WORD_LEN)
	    {
	    	SHOW_ERRMSG("语句不正确");
	  	  return;
	    }
	    memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	    memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	    if (strcmpIgnore( pcKeyWord, "KEY" ))
	    {
	  	  SHOW_ERRMSG("语句不正确");
		    return;
	    }
	  }
	  else
	  {
	  	SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBufStart = pcTmp;
  }
  
  logFmtSyn( LVSYS, "处理 KEY 关键字成功" );
  
  /*指向 key 后面的字符*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  /*如果不是开闭字符*/
	  if (!((*pcBufStart) == '[' || (*pcBufStart) == '('))
	  {
	  	SHOW_ERRMSG("语句不正确");
		  return;
	  }
	}
	
	cValueStart = *pcBufStart;
  pcBufStart++;
  
	/*取出第一个key值*/
	{
	  /*搜索第一个单引号*/
	  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
	    SHOW_ERRMSG("语句不正确");
	    return;
	  }
	  
	  /*如果不是单引号*/
	  if (!((*pcBufStart) == '\''))
	  {
	  	SHOW_ERRMSG("语句不正确");
	    return;
	  }
	  pcBufStart++;
	  
	  /*搜索第二个单引号*/
	  pcTmp = strchr( pcBufStart, '\'' );
	  if (pcTmp == NULL || pcTmp >= pcBufEnd)
	  {
	    SHOW_ERRMSG("语句不正确");
	    return;
	  }
	  
	  /*取出第一个key值*/
	  if (pcTmp - pcBufStart > MAX_KEY_LEN)
	  {
	  	SHOW_ERRMSG("KEY值过长");
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
	
	/*取出第二个key值*/
	{
	  /*搜索逗号*/
	  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
	    SHOW_ERRMSG("语句不正确");
	    return;
	  }
	  
	  /*如果不是逗号*/
	  if (!((*pcBufStart) == ','))
	  {
	  	SHOW_ERRMSG("语句不正确");
	    return;
	  }
	  pcBufStart++;
	  
	  /*搜索第三个单引号*/
	  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
	    SHOW_ERRMSG("语句不正确");
	    return;
	  }
	  
	  /*如果不是单引号*/
	  if (!((*pcBufStart) == '\''))
	  {
	  	SHOW_ERRMSG("语句不正确");
	    return;
	  }
	  pcBufStart++;
	  
	  /*搜索第四个单引号*/
	  pcTmp = strchr( pcBufStart, '\'' );
	  if (pcTmp == NULL || pcTmp >= pcBufEnd)
	  {
	    SHOW_ERRMSG("语句不正确");
	    return;
	  }
	  
	  /*取出第二个key值*/
	  if (pcTmp - pcBufStart > MAX_KEY_LEN)
	  {
	  	SHOW_ERRMSG("KEY值过长");
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
	
	/*取出最后的开闭符*/
  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if ((*pcBufStart) == ']' || (*pcBufStart) == ')')
	{
		cValueEnd = (*pcBufStart);
	}
	else
	{
		SHOW_ERRMSG("语句不正确");
	  return;
	}
	
	pcBufStart++;
	
	/*确定值区间*/
	memset( pcValueType, 0x00, sizeof(pcValueType) );
	if (cValueStart == '(' && cValueEnd == ')')
	{
		if (strcmp( pcKeyStart, pcKeyEnd ) >= 0)
		{
			SHOW_ERRMSG("第一个值只可以小于第二个值");
	    return;
		}
		memcpy( pcValueType, KEY_OPEN_OPEN, OPERATE_LEN );
	}
	else
	if (cValueStart == '(' && cValueEnd == ']')
	{
		if (strcmp( pcKeyStart, pcKeyEnd ) > 0)
		{
			SHOW_ERRMSG("第一个值只可以小于或等于第二个值");
	    return;
		}
		memcpy( pcValueType, KEY_OPEN_CLOSE, OPERATE_LEN );
	}
	else
	if (cValueStart == '[' && cValueEnd == ')')
	{
		if (strcmp( pcKeyStart, pcKeyEnd ) >= 0)
		{
			SHOW_ERRMSG("第一个值只可以小于第二个值");
	    return;
		}
		memcpy( pcValueType, KEY_CLOSE_OPEN, OPERATE_LEN );
	}
	else
	{
		if (strcmp( pcKeyStart, pcKeyEnd ) > 0)
		{
			SHOW_ERRMSG("第一个值只可以小于或等于第二个值");
	    return;
		}
		memcpy( pcValueType, KEY_CLOSE_CLOSE, OPERATE_LEN );
	}
	
	/*取出 using 关键字*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );

	  if (strcmpIgnore( pcKeyWord, "USING" ))
	  {
	  	SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出用户名*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > NAME_LEN)
	  {
	  	SHOW_ERRMSG("用户名不符合规范");
	  	return;
	  }
	  
	  memset( pcUser, 0x00, sizeof(pcUser) );
	  memcpy( pcUser, pcBufStart, pcBlank - pcBufStart );
	  
	  /*对用户名后补空格*/
	  iDataLen = strlen( pcUser );
	  memset( pcUser + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出 by 关键字*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "BY" ))
	  {
	  	SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出密码*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
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
	  	SHOW_ERRMSG("密码不符合规范");
	  	return;
	  }
	  
	  memset( pcPass, 0x00, sizeof(pcPass) );
	  memcpy( pcPass, pcBufStart, pcBlank - pcBufStart + 1 );
	  
	  /*对密码后补空格*/
	  iDataLen = strlen( pcPass );
	  memset( pcPass + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank;
  }
  
  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if (pcBufStart != pcBufEnd)
	{
	  SHOW_ERRMSG("语句不正确");
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
		SHOW_ERRMSG("连接到服务端失败");
		return;
	}

	/*发送长度域*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	/*发送数据*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	iTimeStart = clock();
	fprintf( stderr, "delete start:%d\n", iTimeStart );
	iRet = waitFd( iSocket, WAIT_IN, NO_TIME_OUT );
	if (iRet == -1)
	{
		SHOW_ERRMSG("等待接收服务端数据失败");
		return;
	}
	
	/*接收数据*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("接收服务端数据失败");
		return;
	}
	
	fprintf( stdout, "result:\n" );
	logFmtSyn( LVNOR, "result:" );
	
	iTimeEnd = clock();
	fprintf( stderr, "delete   end:%d\n", iTimeEnd );
	fprintf( stderr, "delete  time:%d\n", iTimeEnd - iTimeStart );
	
	if (cResult == '0')
	{
		/*接收导出数据的总数量*/
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
函数名称: managerUnloadDeal
函数功能: 管理端处理 unload 操作的数据
参    数：
第    一：数据开始地址         I
第    二：数据结束地址         I
作    者: 李高文
建立时间: 20161018
**********************************************************************/
void managerUnloadDeal( char *pcaBufStart, char *pcaBufEnd )
{
/*
发送数据结构：长度+操作+文件名称+值区间（左开右开、左开右闭、左闭右开、左闭右闭）+KEY开始值长度+KEY开始值+KEY结束值长度+KEY结束值
接收数据结构：操作结果+导出数据总数量
发送过去的数据，如果不补空格，服务端打出的日志可能会少。统一，整数前补0，字符后补空格，服务端校验字符是否一样时，得忽略空格
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
	
	logFmtSyn( LVSYS, "开始执行函数：%s", __FUNCTION__ );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
	/*取出 to 关键字*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "TO" ))
	  {
	  	SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
	
	/*取出文件名称*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > FILE_PATH_LEN)
	  {
	  	SHOW_ERRMSG("文件名称不符合规范");
	  	return;
	  }
	  
	  memset( pcFilePath, 0x00, sizeof(pcFilePath) );
	  memcpy( pcFilePath, pcBufStart, pcBlank - pcBufStart );
	  
	  /*对文件名称后补空格*/
	  iDataLen = strlen( pcFilePath );
	  memset( pcFilePath + iDataLen, ' ', FILE_PATH_LEN - iDataLen );
	  	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出 where 关键字*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "WHERE" ))
	  {
	  	SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出 key 关键字*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  /*寻找第一个不是字母的字符*/
	  pcTmp = pcBufStart;
	  IS_NOT_LETTER( pcTmp, pcBufEnd );
	  if (pcTmp == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  /*如果是空格或者开闭字符*/
	  if (isBlank(*pcTmp) || (*pcTmp) == '[' || (*pcTmp) == '(')
	  {
	  	pcBlank = pcTmp;
	  	
	  	if (pcBlank - pcBufStart > KEY_WORD_LEN)
	    {
	    	SHOW_ERRMSG("语句不正确");
	  	  return;
	    }
	    memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	    memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	    if (strcmpIgnore( pcKeyWord, "KEY" ))
	    {
	  	  SHOW_ERRMSG("语句不正确");
		    return;
	    }
	  }
	  else
	  {
	  	SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBufStart = pcTmp;
  }
  
  logFmtSyn( LVSYS, "处理 KEY 关键字成功" );
  
  /*指向 key 后面的字符*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  /*如果不是开闭字符*/
	  if (!((*pcBufStart) == '[' || (*pcBufStart) == '('))
	  {
	  	SHOW_ERRMSG("语句不正确");
		  return;
	  }
	}
	
	cValueStart = *pcBufStart;
  pcBufStart++;
  
	/*取出第一个key值*/
	{
	  /*搜索第一个单引号*/
	  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
	    SHOW_ERRMSG("语句不正确");
	    return;
	  }
	  
	  /*如果不是单引号*/
	  if (!((*pcBufStart) == '\''))
	  {
	  	SHOW_ERRMSG("语句不正确");
	    return;
	  }
	  pcBufStart++;
	  
	  /*搜索第二个单引号*/
	  pcTmp = strchr( pcBufStart, '\'' );
	  if (pcTmp == NULL || pcTmp >= pcBufEnd)
	  {
	    SHOW_ERRMSG("语句不正确");
	    return;
	  }
	  
	  /*取出第一个key值*/
	  if (pcTmp - pcBufStart > MAX_KEY_LEN)
	  {
	  	SHOW_ERRMSG("KEY值过长");
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
	
	/*取出第二个key值*/
	{
	  /*搜索逗号*/
	  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
	    SHOW_ERRMSG("语句不正确");
	    return;
	  }
	  
	  /*如果不是逗号*/
	  if (!((*pcBufStart) == ','))
	  {
	  	SHOW_ERRMSG("语句不正确");
	    return;
	  }
	  pcBufStart++;
	  
	  /*搜索第三个单引号*/
	  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
	    SHOW_ERRMSG("语句不正确");
	    return;
	  }
	  
	  /*如果不是单引号*/
	  if (!((*pcBufStart) == '\''))
	  {
	  	SHOW_ERRMSG("语句不正确");
	    return;
	  }
	  pcBufStart++;
	  
	  /*搜索第四个单引号*/
	  pcTmp = strchr( pcBufStart, '\'' );
	  if (pcTmp == NULL || pcTmp >= pcBufEnd)
	  {
	    SHOW_ERRMSG("语句不正确");
	    return;
	  }
	  
	  /*取出第二个key值*/
	  if (pcTmp - pcBufStart > MAX_KEY_LEN)
	  {
	  	SHOW_ERRMSG("KEY值过长");
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
	
	/*取出最后的开闭符*/
  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if ((*pcBufStart) == ']' || (*pcBufStart) == ')')
	{
		cValueEnd = (*pcBufStart);
	}
	else
	{
		SHOW_ERRMSG("语句不正确");
	  return;
	}
	
	/*确定值区间*/
	memset( pcValueType, 0x00, sizeof(pcValueType) );
	if (cValueStart == '(' && cValueEnd == ')')
	{
		if (strcmp( pcKeyStart, pcKeyEnd ) >= 0)
		{
			SHOW_ERRMSG("第一个值只可以小于第二个值");
	    return;
		}
		memcpy( pcValueType, KEY_OPEN_OPEN, OPERATE_LEN );
	}
	else
	if (cValueStart == '(' && cValueEnd == ']')
	{
		if (strcmp( pcKeyStart, pcKeyEnd ) > 0)
		{
			SHOW_ERRMSG("第一个值只可以小于或等于第二个值");
	    return;
		}
		memcpy( pcValueType, KEY_OPEN_CLOSE, OPERATE_LEN );
	}
	else
	if (cValueStart == '[' && cValueEnd == ')')
	{
		if (strcmp( pcKeyStart, pcKeyEnd ) >= 0)
		{
			SHOW_ERRMSG("第一个值只可以小于第二个值");
	    return;
		}
		memcpy( pcValueType, KEY_CLOSE_OPEN, OPERATE_LEN );
	}
	else
	{
		if (strcmp( pcKeyStart, pcKeyEnd ) > 0)
		{
			SHOW_ERRMSG("第一个值只可以小于或等于第二个值");
	    return;
		}
		memcpy( pcValueType, KEY_CLOSE_CLOSE, OPERATE_LEN );
	}
  
  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if (pcBufStart != pcBufEnd)
	{
	  SHOW_ERRMSG("语句不正确");
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
		SHOW_ERRMSG("连接到服务端失败");
		return;
	}

	/*发送长度域*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	/*发送数据*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	SHOW_ERRMSG("发送数据到服务端成功，请耐心等待服务端处理");

  /*等待接收*/
	iRet = waitFd( iSocket, WAIT_IN, WAIT_SERVER_TIME );
	
	/*如果网络错误*/
  if (iRet == -1)
	{
		SHOW_ERRMSG("等待接收数据失败");
	  return;
	}
	
	/*接收长度*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*如果值是0，表示链路保持*/
	if (atoi( pcRecvLen ) == 0)
	{
		SHOW_ERRMSG("等待接收数据失败");
	  return;
	}
	
	/*接收数据*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("接收服务端数据失败");
		return;
	}
	
	if (cResult == '0')
	{
		/*接收导出数据的总数量*/
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
函数名称: managerLoadDeal
函数功能: 管理端处理 load 操作的数据
参    数：
第    一：数据开始地址         I
第    二：数据结束地址         I
作    者: 李高文
建立时间: 20161018
**********************************************************************/
void managerLoadDeal( char *pcaBufStart, char *pcaBufEnd )
{
/*
发送数据结构：长度+操作+文件路径+用户名+密码
接收数据结构：操作结果+导入数据总数量+导入数量+相同key数量+失败数量
发送过去的数据，如果不补空格，服务端打出的日志可能会少。统一，整数前补0，字符后补空格，服务端校验字符是否一样时，得忽略空格
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
	
	logFmtSyn( LVSYS, "开始执行函数：%s", __FUNCTION__ );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
	/*取出 from 关键字*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "FROM" ))
	  {
	  	SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
	
	/*取出文件名称*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > FILE_PATH_LEN)
	  {
	  	SHOW_ERRMSG("文件名称不符合规范");
	  	return;
	  }
	  
	  memset( pcFilePath, 0x00, sizeof(pcFilePath) );
	  memcpy( pcFilePath, pcBufStart, pcBlank - pcBufStart );
	  
	  /*对文件名称后补空格*/
	  iDataLen = strlen( pcFilePath );
	  memset( pcFilePath + iDataLen, ' ', FILE_PATH_LEN - iDataLen );
	  	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出 using 关键字*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "USING" ))
	  {
	  	SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出用户名*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > NAME_LEN)
	  {
	  	SHOW_ERRMSG("用户名不符合规范");
	  	return;
	  }
	  
	  memset( pcUser, 0x00, sizeof(pcUser) );
	  memcpy( pcUser, pcBufStart, pcBlank - pcBufStart );
	  
	  /*对用户名后补空格*/
	  iDataLen = strlen( pcUser );
	  memset( pcUser + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出 by 关键字*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "BY" ))
	  {
	  	SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出密码*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
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
	  	SHOW_ERRMSG("密码不符合规范");
	  	return;
	  }
	  
	  memset( pcPass, 0x00, sizeof(pcPass) );
	  memcpy( pcPass, pcBufStart, pcBlank - pcBufStart + 1 );
	  
	  /*对密码后补空格*/
	  iDataLen = strlen( pcPass );
	  memset( pcPass + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank;
  }
  
  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if (pcBufStart != pcBufEnd)
	{
	  SHOW_ERRMSG("语句不正确");
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
		SHOW_ERRMSG("连接到服务端失败");
		return;
	}

	/*发送长度域*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	/*发送数据*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	SHOW_ERRMSG("发送数据到服务端成功，请耐心等待服务端处理");

  /*等待接收*/
	iRet = waitFd( iSocket, WAIT_IN, WAIT_SERVER_TIME );
	
	/*如果网络错误*/
  if (iRet == -1)
	{
		SHOW_ERRMSG("等待接收数据失败");
	  return;
	}
	
	/*接收长度*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*如果值是0，表示链路保持*/
	if (atoi( pcRecvLen ) == 0)
	{
		SHOW_ERRMSG("等待接收数据失败");
	  return;
	}
	
	/*接收数据*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("接收服务端数据失败");
		return;
	}
	
	if (cResult == '0')
	{
		/*接收导入数据的总数量*/
		MANAGER_RECV( iSocket, iRet, pcKeysTotal, iRecvLen, INT64_LEN );
		
		/*接收导入数据数量*/
		MANAGER_RECV( iSocket, iRet, pcKeysLoaded, iRecvLen, INT64_LEN );
	  
	  /*接收相同key数量*/
	  MANAGER_RECV( iSocket, iRet, pcKeysUpdate, iRecvLen, INT64_LEN );
	  
	  /*接收失败数量*/
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
函数名称: managerExitDeal
函数功能: 管理端处理 exit 操作的数据
参    数：
第    一：数据开始地址         I
第    二：数据结束地址         I
作    者: 李高文
建立时间: 20161018
**********************************************************************/
void managerExitDeal( char *pcaBufStart, char *pcaBufEnd )
{
/*
发送数据结构：长度+操作+用户名+密码
接收数据结构：操作结果
发送过去的数据，如果不补空格，服务端打出的日志可能会少。统一，整数前补0，字符后补空格，服务端校验字符是否一样时，得忽略空格
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
	
	logFmtSyn( LVSYS, "开始执行函数：%s", __FUNCTION__ );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
  
  /*取出 using 关键字*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "USING" ))
	  {
	  	SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出用户名*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > NAME_LEN)
	  {
	  	SHOW_ERRMSG("用户名不符合规范");
	  	return;
	  }
	  
	  memset( pcUser, 0x00, sizeof(pcUser) );
	  memcpy( pcUser, pcBufStart, pcBlank - pcBufStart );
	  
	  /*对用户名后补空格*/
	  iDataLen = strlen( pcUser );
	  memset( pcUser + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出 by 关键字*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "BY" ))
	  {
	  	SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出密码*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
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
	  
	  /*对密码后补空格*/
	  iDataLen = strlen( pcPass );
	  memset( pcPass + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank;
  }
  
  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if (pcBufStart != pcBufEnd)
	{
	  SHOW_ERRMSG("语句不正确");
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
		SHOW_ERRMSG("连接到服务端失败");
		return;
	}

	/*发送长度域*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	/*发送数据*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	iRet = waitFd( iSocket, WAIT_IN, TIME_OUT );
	if (iRet == -1)
	{
		SHOW_ERRMSG("等待接收服务端数据失败");
		return;
	}
	
	/*接收数据长度*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*接收数据*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("接收服务端数据失败");
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
函数名称: managerFlushDeal
函数功能: 管理端处理 flush 操作的数据
参    数：
第    一：数据开始地址         I
第    二：数据结束地址         I
作    者: 李高文
建立时间: 20161018
**********************************************************************/
void managerFlushDeal( char *pcaBufStart, char *pcaBufEnd )
{
/*
发送数据结构：长度+操作+用户名+密码
接收数据结构：操作结果
发送过去的数据，如果不补空格，服务端打出的日志可能会少。统一，整数前补0，字符后补空格，服务端校验字符是否一样时，得忽略空格
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
	
	logFmtSyn( LVSYS, "开始执行函数：%s", __FUNCTION__ );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
  
  /*取出 using 关键字*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "USING" ))
	  {
	  	SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出用户名*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > NAME_LEN)
	  {
	  	SHOW_ERRMSG("用户名不符合规范");
	  	return;
	  }
	  
	  memset( pcUser, 0x00, sizeof(pcUser) );
	  memcpy( pcUser, pcBufStart, pcBlank - pcBufStart );
	  
	  /*对用户名后补空格*/
	  iDataLen = strlen( pcUser );
	  memset( pcUser + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出 by 关键字*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "BY" ))
	  {
	  	SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出密码*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
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
	  	SHOW_ERRMSG("密码不符合规范");
	  	return;
	  }
	  
	  memset( pcPass, 0x00, sizeof(pcPass) );
	  memcpy( pcPass, pcBufStart, pcBlank - pcBufStart + 1 );
	  
	  /*对密码后补空格*/
	  iDataLen = strlen( pcPass );
	  memset( pcPass + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank;
  }
  
  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if (pcBufStart != pcBufEnd)
	{
	  SHOW_ERRMSG("语句不正确");
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
		SHOW_ERRMSG("连接到服务端失败");
		return;
	}
	
	/*发送长度域*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	/*发送数据*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	iTimeStart = clock();
	fprintf( stderr, "flush start:%d\n", iTimeStart );
	iRet = waitFd( iSocket, WAIT_IN, NO_TIME_OUT );
	if (iRet == -1)
	{
		SHOW_ERRMSG("等待接收服务端数据失败");
		return;
	}
	
	/*接收数据长度*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*接收数据*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("接收服务端数据失败");
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
函数名称: managerRamDeal
函数功能: 管理端处理 ram to disc 操作的数据
参    数：
第    一：数据开始地址         I
第    二：数据结束地址         I
作    者: 李高文
建立时间: 20170217
**********************************************************************/
void managerRamDeal( char *pcaBufStart, char *pcaBufEnd )
{
/*
发送数据结构：长度+操作+用户名+密码
接收数据结构：操作结果
发送过去的数据，如果不补空格，服务端打出的日志可能会少。统一，整数前补0，字符后补空格，服务端校验字符是否一样时，得忽略空格
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
	
	logFmtSyn( LVSYS, "开始执行函数：%s", __FUNCTION__ );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
  /*取出 to 关键字*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "TO" ))
	  {
	  	SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
	
	/*取出 DISC 关键字*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "DISC" ))
	  {
	  	SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出 using 关键字*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "USING" ))
	  {
	  	SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出用户名*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > NAME_LEN)
	  {
	  	SHOW_ERRMSG("用户名不符合规范");
	  	return;
	  }
	  
	  memset( pcUser, 0x00, sizeof(pcUser) );
	  memcpy( pcUser, pcBufStart, pcBlank - pcBufStart );
	  
	  /*对用户名后补空格*/
	  iDataLen = strlen( pcUser );
	  memset( pcUser + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出 by 关键字*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "BY" ))
	  {
	  	SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出密码*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
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
	  	SHOW_ERRMSG("密码不符合规范");
	  	return;
	  }
	  
	  memset( pcPass, 0x00, sizeof(pcPass) );
	  memcpy( pcPass, pcBufStart, pcBlank - pcBufStart + 1 );
	  
	  /*对密码后补空格*/
	  iDataLen = strlen( pcPass );
	  memset( pcPass + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank;
  }
  
  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if (pcBufStart != pcBufEnd)
	{
	  SHOW_ERRMSG("语句不正确");
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
		SHOW_ERRMSG("连接到服务端失败");
		return;
	}
	
	/*发送长度域*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	/*发送数据*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	iTimeStart = clock();
	fprintf( stderr, "ram to disc start:%d\n", iTimeStart );
	iRet = waitFd( iSocket, WAIT_IN, NO_TIME_OUT );
	if (iRet == -1)
	{
		SHOW_ERRMSG("等待接收服务端数据失败");
		return;
	}
	
	/*接收数据长度*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*接收数据*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("接收服务端数据失败");
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
函数名称: managerAlterDeal
函数功能: 管理端处理 alter 操作的数据
参    数：
第    一：数据开始地址         I
第    二：数据结束地址         I
作    者: 李高文
建立时间: 20161018
**********************************************************************/
void managerAlterDeal( char *pcaBufStart, char *pcaBufEnd )
{
  char pcKeyWord[KEY_WORD_LEN];
	char *pcBlank;
	char *pcBufStart;
	char *pcBufEnd;
	
	logFmtSyn( LVSYS, "开始执行函数：%s", __FUNCTION__ );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if (pcBufStart == pcBufEnd)
	{
		SHOW_ERRMSG("只有操作符，缺少操作数据");
		return;
	}
	
	pcBlank = strchr( pcBufStart, ' ' );
	if (pcBlank == NULL)
	{
		SHOW_ERRMSG("语句不正确");
		return;
	}
	
	if (pcBlank - pcBufStart > KEY_WORD_LEN)
	{
		SHOW_ERRMSG("操作类型不存在");
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
		SHOW_ERRMSG("操作类型不存在");
		return;
	}
	
	return;
}

/**********************************************************************
函数名称: managerAlterThreadsDeal
函数功能: 管理端处理 alter threads 操作的数据
参    数：
第    一：数据开始地址      I
第    二：数据结束地址      I
作    者: 李高文
建立时间: 20161019
**********************************************************************/
void managerAlterThreadsDeal( char *pcaBufStart, char *pcaBufEnd )
{
/*
发送数据结构：长度+操作+短连接线程数量+长连接线程数量+用户名+密码
接收数据结构：操作结果
发送过去的数据，如果不补空格，服务端打出的日志可能会少。统一，整数前补0，字符后补空格，服务端校验字符是否一样时，得忽略空格
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
	
	logFmtSyn( LVSYS, "开始执行函数：%s", __FUNCTION__ );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
	/*取出 to 关键字*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "TO" ))
	  {
	  	SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
	
	/*取出短线程数量*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcComma = strchr( pcBufStart, ',' );
	  if (pcComma == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcComma - pcBufStart > INT64_LEN)
	  {
	  	SHOW_ERRMSG("短线程数量不符合规范");
	  	return;
	  }
	  
	  memset( pcShortThreads, 0x00, sizeof(pcShortThreads) );
	  memcpy( pcShortThreads, pcBufStart, pcComma - pcBufStart );
	  
	  if (!isInt( pcShortThreads, pcShortThreads + strlen(pcShortThreads) ) || atoll( pcShortThreads ) <= 0 )
	  {
	  	SHOW_ERRMSG("短线程数量不符合规范");
		  return;
	  }
	  
	  memset( pcIntTmp, 0x00, sizeof(pcIntTmp) );
	  sprintf( pcIntTmp, "%0*lld", INT64_LEN, atoll(pcShortThreads) );
	  memset( pcShortThreads, 0x00, sizeof(pcShortThreads) );
	  strcpy( pcShortThreads, pcIntTmp );
	  
	  pcBufStart = pcComma + 1;
  }
	
	/*取出长线程数量*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > INT64_LEN)
	  {
	  	SHOW_ERRMSG("长线程数量不符合规范");
	  	return;
	  }
	  
	  memset( pcLongThreads, 0x00, sizeof(pcLongThreads) );
	  memcpy( pcLongThreads, pcBufStart, pcBlank - pcBufStart );
	  
	  if (!isInt( pcLongThreads, pcLongThreads + strlen(pcLongThreads) ) || atoll( pcLongThreads ) <= 0)
	  {
	  	SHOW_ERRMSG("长线程数量不符合规范");
		  return;
	  }
	  
	  memset( pcIntTmp, 0x00, sizeof(pcIntTmp) );
	  sprintf( pcIntTmp, "%0*lld", INT64_LEN, atoll(pcLongThreads) );
	  memset( pcLongThreads, 0x00, sizeof(pcLongThreads) );
	  strcpy( pcLongThreads, pcIntTmp );
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出 using 关键字*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "USING" ))
	  {
	  	SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出用户名*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > NAME_LEN)
	  {
	  	SHOW_ERRMSG("用户名不符合规范");
	  	return;
	  }
	  
	  memset( pcUser, 0x00, sizeof(pcUser) );
	  memcpy( pcUser, pcBufStart, pcBlank - pcBufStart );
	  
	  /*对用户名后补空格*/
	  iDataLen = strlen( pcUser );
	  memset( pcUser + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出 by 关键字*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "BY" ))
	  {
	  	SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出密码*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
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
	  	SHOW_ERRMSG("密码不符合规范");
	  	return;
	  }
	  
	  memset( pcPass, 0x00, sizeof(pcPass) );
	  memcpy( pcPass, pcBufStart, pcBlank - pcBufStart + 1 );
	  
	  /*对密码后补空格*/
	  iDataLen = strlen( pcPass );
	  memset( pcPass + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank;
  }
  
  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if (pcBufStart != pcBufEnd)
	{
	  SHOW_ERRMSG("语句不正确");
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
		SHOW_ERRMSG("连接到服务端失败");
		return;
	}

	/*发送长度域*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	/*发送数据*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	iRet = waitFd( iSocket, WAIT_IN, TIME_OUT );
	if (iRet == -1)
	{
		SHOW_ERRMSG("等待接收服务端数据失败");
		return;
	}
	
	/*接收数据长度*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*接收数据*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("接收服务端数据失败");
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
函数名称: managerAlterDiscsDeal
函数功能: 管理端处理 alter discs 操作的数据
参    数：
第    一：数据开始地址      I
第    二：数据结束地址      I
作    者: 李高文
建立时间: 20161019
**********************************************************************/
void managerAlterDiscsDeal( char *pcaBufStart, char *pcaBufEnd )
{
/*
发送数据结构：长度+操作+文件最大值+用户名+密码
接收数据结构：操作结果
发送过去的数据，如果不补空格，服务端打出的日志可能会少。统一，整数前补0，字符后补空格，服务端校验字符是否一样时，得忽略空格
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
	
	logFmtSyn( LVSYS, "开始执行函数：%s", __FUNCTION__ );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
	/*取出 to 关键字*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "TO" ))
	  {
	  	SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
	
	/*取出文件最大值*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > INT64_LEN)
	  {
	  	SHOW_ERRMSG("文件最大值不符合规范");
	  	return;
	  }
	  
	  memset( pcDiscs, 0x00, sizeof(pcDiscs) );
	  memcpy( pcDiscs, pcBufStart, pcBlank - pcBufStart );
	  
	  if (!isInt( pcDiscs, pcDiscs + strlen(pcDiscs) ) || atoll( pcDiscs ) <= 0 )
	  {
	  	SHOW_ERRMSG("文件最大值不符合规范");
		  return;
	  }
	  
	  memset( pcIntTmp, 0x00, sizeof(pcIntTmp) );
	  sprintf( pcIntTmp, "%0*lld", INT64_LEN, atoll(pcDiscs) );
	  memset( pcDiscs, 0x00, sizeof(pcDiscs) );
	  strcpy( pcDiscs, pcIntTmp );
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出 using 关键字*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "USING" ))
	  {
	  	SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出用户名*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > NAME_LEN)
	  {
	  	SHOW_ERRMSG("用户名不符合规范");
	  	return;
	  }
	  
	  memset( pcUser, 0x00, sizeof(pcUser) );
	  memcpy( pcUser, pcBufStart, pcBlank - pcBufStart );
	  
	  /*对用户名后补空格*/
	  iDataLen = strlen( pcUser );
	  memset( pcUser + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出 by 关键字*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "BY" ))
	  {
	  	SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出密码*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
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
	  	SHOW_ERRMSG("密码不符合规范");
	  	return;
	  }
	  
	  memset( pcPass, 0x00, sizeof(pcPass) );
	  memcpy( pcPass, pcBufStart, pcBlank - pcBufStart + 1 );
	  
	  /*对密码后补空格*/
	  iDataLen = strlen( pcPass );
	  memset( pcPass + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank;
  }
  
  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if (pcBufStart != pcBufEnd)
	{
	  SHOW_ERRMSG("语句不正确");
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
		SHOW_ERRMSG("连接到服务端失败");
		return;
	}
	
	/*发送长度域*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	/*发送数据*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	iRet = waitFd( iSocket, WAIT_IN, TIME_OUT );
	if (iRet == -1)
	{
		SHOW_ERRMSG("等待接收服务端数据失败");
		return;
	}
	
	/*接收数据长度*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*接收数据*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("接收服务端数据失败");
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
函数名称: managerAlterUppersDeal
函数功能: 管理端处理 alter uppers 操作的数据
参    数：
第    一：数据开始地址      I
第    二：数据结束地址      I
作    者: 李高文
建立时间: 20161019
**********************************************************************/
void managerAlterUppersDeal( char *pcaBufStart, char *pcaBufEnd )
{
/*
发送数据结构：长度+操作+文件步增值+用户名+密码
接收数据结构：操作结果
发送过去的数据，如果不补空格，服务端打出的日志可能会少。统一，整数前补0，字符后补空格，服务端校验字符是否一样时，得忽略空格
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
	
	logFmtSyn( LVSYS, "开始执行函数：%s", __FUNCTION__ );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
	/*取出 to 关键字*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "TO" ))
	  {
	  	SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
	
	/*取出文件最大值*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > INT64_LEN)
	  {
	  	SHOW_ERRMSG("文件步增值不符合规范");
	  	return;
	  }
	  
	  memset( pcUppers, 0x00, sizeof(pcUppers) );
	  memcpy( pcUppers, pcBufStart, pcBlank - pcBufStart );
	  
	  if (!isInt( pcUppers, pcUppers + strlen(pcUppers) ) || atoll( pcUppers ) <= 0)
	  {
	  	SHOW_ERRMSG("文件步增值不符合规范");
		  return;
	  }
	  
	  memset( pcIntTmp, 0x00, sizeof(pcIntTmp) );
	  sprintf( pcIntTmp, "%0*lld", INT64_LEN, atoll(pcUppers) );
	  memset( pcUppers, 0x00, sizeof(pcUppers) );
	  strcpy( pcUppers, pcIntTmp );
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出 using 关键字*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "USING" ))
	  {
	  	SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出用户名*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > NAME_LEN)
	  {
	  	SHOW_ERRMSG("用户名不符合规范");
	  	return;
	  }
	  
	  memset( pcUser, 0x00, sizeof(pcUser) );
	  memcpy( pcUser, pcBufStart, pcBlank - pcBufStart );
	  
	  /*对用户名后补空格*/
	  iDataLen = strlen( pcUser );
	  memset( pcUser + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出 by 关键字*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
		  return;
	  }
	
	  pcBlank = strchr( pcBufStart, ' ' );
	  if (pcBlank == NULL)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  if (pcBlank - pcBufStart > KEY_WORD_LEN)
	  {
	  	SHOW_ERRMSG("语句不正确");
	  	return;
	  }
	  
	  memset( pcKeyWord, 0x00, sizeof(pcKeyWord) );
	  memcpy( pcKeyWord, pcBufStart, pcBlank - pcBufStart );
	  
	  if (strcmpIgnore( pcKeyWord, "BY" ))
	  {
	  	SHOW_ERRMSG("语句不正确");
		  return;
	  }
	  
	  pcBufStart = pcBlank + 1;
  }
  
  /*取出密码*/
	{
		IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	  if (pcBufStart == pcBufEnd)
	  {
		  SHOW_ERRMSG("语句不正确");
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
	  	SHOW_ERRMSG("密码不符合规范");
	  	return;
	  }
	  
	  memset( pcPass, 0x00, sizeof(pcPass) );
	  memcpy( pcPass, pcBufStart, pcBlank - pcBufStart + 1 );
	  
	  /*对密码后补空格*/
	  iDataLen = strlen( pcPass );
	  memset( pcPass + iDataLen, ' ', NAME_LEN - iDataLen );
	  
	  pcBufStart = pcBlank;
  }
  
  IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if (pcBufStart != pcBufEnd)
	{
	  SHOW_ERRMSG("语句不正确");
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
		SHOW_ERRMSG("连接到服务端失败");
		return;
	}

	/*发送长度域*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	/*发送数据*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	iRet = waitFd( iSocket, WAIT_IN, TIME_OUT );
	if (iRet == -1)
	{
		SHOW_ERRMSG("等待接收服务端数据失败");
		return;
	}
	
	/*接收数据长度*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*接收数据*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("接收服务端数据失败");
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
函数名称: managerShowDeal
函数功能: 管理端处理 show 操作的数据
参    数：
第    一：数据开始地址         I
第    二：数据结束地址         I
作    者: 李高文
建立时间: 20161018
**********************************************************************/
void managerShowDeal( char *pcaBufStart, char *pcaBufEnd )
{
	char pcKeyWord[KEY_WORD_LEN];
	char *pcBlank;
	char *pcBufStart;
	char *pcBufEnd;
	
	logFmtSyn( LVSYS, "开始执行函数：%s", __FUNCTION__ );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
		
	IGNORE_BLANK_START( pcBufStart, pcBufEnd );
	if (pcBufStart == pcBufEnd)
	{
		SHOW_ERRMSG("只有操作符，缺少操作数据");
		return;
	}
	
	if (pcBufEnd - pcBufStart + 1 > KEY_WORD_LEN)
	{
		SHOW_ERRMSG("操作类型不存在");
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
		SHOW_ERRMSG("操作类型不存在");
		return;
	}
	
	return;
}

/**********************************************************************
函数名称: managerShowRamsDeal
函数功能: 管理端处理 show rams 操作的数据
参    数：
作    者: 李高文
建立时间: 20161019
**********************************************************************/
void managerShowRamsDeal( )
{
/*
发送数据结构：长度+操作
接收数据结构：操作结果+KEY文件缓冲区总数量 + KEY文件缓冲区的使用偏移量（单位 B） + VALUE文件缓冲区总数量 + VALUE文件缓冲区的使用偏移量（单位 B）
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
	
	logFmtSyn( LVSYS, "开始执行函数：%s", __FUNCTION__ );
	
	memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
	strcpy( pcSendBuf, MANAGER_SHOW_RAMS_CNT );
	
	iSendLen = strlen(pcSendBuf);
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
	
	iSocket = connectToServer( pcgIp, pcgPort );
	if (iSocket == -1)
	{
		SHOW_ERRMSG("连接到服务端失败");
		return;
	}
	
	/*发送长度域*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	/*发送数据*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	iRet = waitFd( iSocket, WAIT_IN, TIME_OUT );
	if (iRet == -1)
	{
		SHOW_ERRMSG("等待接收服务端数据失败");
		return;
	}
	
	/*接收数据长度*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*接收操作结果*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("接收服务端数据失败");
		return;
	}
	
	fprintf( stdout, "result:\n" );
	logFmtSyn( LVNOR, "result:" );
	
	if (cResult == '0')
	{
		/*接收KEY文件缓冲区总数量*/
	  MANAGER_RECV( iSocket, iRet, pcKeyTotal, iRecvLen, INT64_LEN );
	  
	  /*接收KEY文件缓冲区的使用偏移量*/
	  MANAGER_RECV( iSocket, iRet, pcKeyUsed, iRecvLen, INT64_LEN );
	  
	  /*接收VALUE文件缓冲区总数量*/
	  MANAGER_RECV( iSocket, iRet, pcValueTotal, iRecvLen, INT64_LEN );
	  
	  /*接收VALUE文件缓冲区的使用偏移量*/
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
函数名称: managerShowDiscsDeal
函数功能: 管理端处理 show discs 操作的数据
参    数：
作    者: 李高文
建立时间: 20161019
**********************************************************************/
void managerShowDiscsDeal( )
{
/*
发送数据结构：长度+操作
接收数据结构：操作结果+KEY文件最大值+KEY文件当前值+KEY文件步增值+ KEY文件使用偏移量 + KEY文件待使用大小+VALUE文件最大值+VALUE文件当前值+VALUE文件步增值+ VALUE文件使用偏移量+VALUE文件待使用大小
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
	
	logFmtSyn( LVSYS, "开始执行函数：%s", __FUNCTION__ );
	
	memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
	strcpy( pcSendBuf, MANAGER_SHOW_DISCS_CNT );
	
	iSendLen = strlen(pcSendBuf);
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
	
	iSocket = connectToServer( pcgIp, pcgPort );
	if (iSocket == -1)
	{
		SHOW_ERRMSG("连接到服务端失败");
		return;
	}

	/*发送长度域*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	/*发送数据*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	iRet = waitFd( iSocket, WAIT_IN, TIME_OUT );
	if (iRet == -1)
	{
		SHOW_ERRMSG("等待接收服务端数据失败");
		return;
	}
	
	/*接收数据长度*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*接收操作结果*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("接收服务端数据失败");
		return;
	}
	
	fprintf( stdout, "result:\n" );
	logFmtSyn( LVNOR, "result:" );
	
	if (cResult == '0')
	{
		/*接收KEY文件最大值*/
	  MANAGER_RECV( iSocket, iRet, pcKeyMax, iRecvLen, INT64_LEN );
	  
		/*接收KEY文件当前值*/
	  MANAGER_RECV( iSocket, iRet, pcKeyTotal, iRecvLen, INT64_LEN );
	  
	  /*接收KEY文件步增值*/
	  MANAGER_RECV( iSocket, iRet, pcKeyUpper, iRecvLen, INT64_LEN );
	  
	  /*接收KEY文件使用偏移量*/
	  MANAGER_RECV( iSocket, iRet, pcKeyUsed, iRecvLen, INT64_LEN );
	  
	  /*接收KEY文件待使用大小*/
	  MANAGER_RECV( iSocket, iRet, pcKeyFree, iRecvLen, INT64_LEN );
	  
	  /*接收VALUE文件最大值*/
	  MANAGER_RECV( iSocket, iRet, pcValueMax, iRecvLen, INT64_LEN );
	  
	  /*接收VALUE文件当前值*/
	  MANAGER_RECV( iSocket, iRet, pcValueTotal, iRecvLen, INT64_LEN );
	  
	  /*接收VALUE文件步增值*/
	  MANAGER_RECV( iSocket, iRet, pcValueUpper, iRecvLen, INT64_LEN );
	  
	  /*接收VALUE文件使用偏移量*/
	  MANAGER_RECV( iSocket, iRet, pcValueUsed, iRecvLen, INT64_LEN );
	  
	  /*接收VALUE文件待使用大小*/
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
函数名称: managerShowThreadsDeal
函数功能: 管理端处理 show threads 操作的数据
参    数：
作    者: 李高文
建立时间: 20161019
**********************************************************************/
void managerShowThreadsDeal( )
{
/*
发送数据结构：长度+操作
接收数据结构：操作结果+短连接线程数量+长连接线程数量
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
	
	logFmtSyn( LVSYS, "开始执行函数：%s", __FUNCTION__ );
	
	memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
	strcpy( pcSendBuf, MANAGER_SHOW_THREADS_CNT );
	
	iSendLen = strlen(pcSendBuf);
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
	
	iSocket = connectToServer( pcgIp, pcgPort );
	if (iSocket == -1)
	{
		SHOW_ERRMSG("连接到服务端失败");
		return;
	}

	/*发送长度域*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	/*发送数据*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	iRet = waitFd( iSocket, WAIT_IN, TIME_OUT );
	if (iRet == -1)
	{
		SHOW_ERRMSG("等待接收服务端数据失败");
		return;
	}
	
	/*接收数据长度*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*接收操作结果*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("接收服务端数据失败");
		return;
	}
	
	fprintf( stdout, "result:\n" );
	logFmtSyn( LVNOR, "result:" );
	
	if (cResult == '0')
	{
		/*接收短连接线程数量*/
	  MANAGER_RECV( iSocket, iRet, pcShortThreads, iRecvLen, INT64_LEN );
	  
	  /*接收长连接线程数量*/
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
函数名称: managerShowKeysDeal
函数功能: 管理端处理 show keys 操作的数据
参    数：
作    者: 李高文
建立时间: 20161019
**********************************************************************/
void managerShowKeysDeal( )
{
/*
发送数据结构：长度+操作
接收数据结构：操作结果+KEY总数量
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
	
	logFmtSyn( LVSYS, "开始执行函数：%s", __FUNCTION__ );
	
	memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
	strcpy( pcSendBuf, MANAGER_SHOW_KEYS_CNT );
	
	iSendLen = strlen(pcSendBuf);
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
	
	iSocket = connectToServer( pcgIp, pcgPort );
	if (iSocket == -1)
	{
		SHOW_ERRMSG("连接到服务端失败");
		return;
	}
	
	/*发送长度域*/
	iRet = send( iSocket, pcSendLen, DATA_BUF_LEN, 0 );
	if (iRet != DATA_BUF_LEN)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	/*发送数据*/
	iRet = send( iSocket, pcSendBuf, iSendLen, 0 );
	if (iRet != iSendLen)
	{
		SHOW_ERRMSG("发送数据到服务端失败");
		return;
	}
	
	iRet = waitFd( iSocket, WAIT_IN, TIME_OUT );
	if (iRet == -1)
	{
		SHOW_ERRMSG("等待接收服务端数据失败");
		return;
	}
	
	/*接收数据长度*/
	MANAGER_RECV( iSocket, iRet, pcRecvLen, iRecvLen, DATA_BUF_LEN );
	
	/*接收操作结果*/
	cResult = 0;
  iRet = recv( iSocket, &cResult, 1, 0 );
  if (iRet <= 0)
	{
		SHOW_ERRMSG("接收服务端数据失败");
		return;
	}
	
	fprintf( stdout, "result:\n" );
	logFmtSyn( LVNOR, "result:" );
	
	if (cResult == '0')
	{
		/*接收KEY总数量*/
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
	int iSocket;                 /*socket文件描述符*/
  struct sockaddr_in servAddr; /*服务器地址信息结构体*/
  struct sockaddr_in addr;     /*接收到的客户端信息结构体*/
  
  #ifdef SYSTEM_WINDOWS_64
	  /*创建套接口*/
    iSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if (iSocket == INVALID_SOCKET)
    {
    	fprintf( stderr, "创建套接口失败，error[%d]\n", GetLastError() );
    	logFmtSyn( LVERR, "创建套接口失败，error[%d]", GetLastError() );
      return -1;
    }
  #else
    /*创建套接口*/
    iSocket = socket( AF_INET, SOCK_STREAM, 0 );
    if (iSocket == INVALID_SOCKET)
    {
    	fprintf( stderr, "创建套接口失败，error[%d]\n", errno );
    	logFmtSyn( LVERR, "创建套接口失败，error[%d]", errno );
      return -1;
    }
  #endif
  
  servAddr.sin_family = AF_INET;
  servAddr.sin_port = htons(atoi(pcaPort));
  servAddr.sin_addr.s_addr = inet_addr(pcaIp);
      
  /*连接真正的FTP服务*/
  if (connect(iSocket, (struct sockaddr *)(&servAddr), sizeof(struct sockaddr)) < 0)
  {
    fprintf( stderr, "连接服务端失败，error[%d]\n", GetLastError() );
    logFmtSyn( LVERR, "创建套接口失败，error[%d]", GetLastError() );
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
函数名：AllTrim
函数功能：去掉头尾空格 
入口参数：
第    一：字符串     I
返    回：1表示该字符为空格，0表示不是
创建时间：20130226
*********************************************************/
char *AllTrim ( char *pcaStr )
{
	char *pcStart;
	char *pcEnd;
	char pcBuf[8192];
	
	pcStart = pcaStr;
	pcEnd = pcStart + strlen(pcStart);
	
	/*忽略开头部分的空格*/
	while (1)
	{
		/*如果全都是空格*/
		if (pcStart == pcEnd)
		{
			return pcaStr;
		}
		
		/*如果是空格*/
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

	/*忽略末尾部分的空格*/
	while (1)
	{
		/*如果全都是空格*/
		if (pcEnd == pcStart)
		{
			return pcaStr;
		}
		/*如果是空格*/
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
函数名  ：checkPort
函数功能：检查端口是否正确
入口参数：
第    一：端口                I
返    回：1表示符合，0表示不符合
创建时间：20151102
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
函数名  ：checkIP
函数功能：检查IP是否正确
入口参数：
第    一：IP                I
返    回：1表示符合，0表示不符合
创建时间：20131024
*********************************************************/
int checkIP( char *pcaIP )
{
  char pcValue[20];
  char *pcBuf;
  char *pcTmp;
  
  pcBuf = pcaIP;
  
  /*取前8b*/
  pcTmp = strchr( pcBuf, '.' );
  if (pcTmp == NULL)
  {
    return 0;
  }
  memset( pcValue, 0x00, sizeof(pcValue) );
  memcpy( pcValue, pcBuf, pcTmp - pcBuf );
  
  /*如果有非法字符*/
  if (!isInt(pcValue, pcValue + strlen(pcValue)))
  {
  	return 0;
  }
  
  /*如果值不对*/
  if (atoi(pcValue) < 0 || atoi(pcValue) > 255)
  {
    return 0;
  }
  pcBuf = pcTmp + 1;
  
  /*取8b到15b*/
  pcTmp = strchr( pcBuf, '.' );
  if (pcTmp == NULL)
  {
    return 0;
  }
  memset( pcValue, 0x00, sizeof(pcValue) );
  memcpy( pcValue, pcBuf, pcTmp - pcBuf );
  /*如果有非法字符*/
  if (!isInt(pcValue, pcValue + strlen(pcValue)))
  {
  	return 0;
  }
  
  /*如果值不对*/
  if (atoi(pcValue) < 0 || atoi(pcValue) > 255)
  {
    return 0;
  }
  pcBuf = pcTmp + 1;
  
  /*取16b到23b*/
  pcTmp = strchr( pcBuf, '.' );
  if (pcTmp == NULL)
  {
    return 0;
  }
  memset( pcValue, 0x00, sizeof(pcValue) );
  memcpy( pcValue, pcBuf, pcTmp - pcBuf );
  /*如果有非法字符*/
  if (!isInt(pcValue, pcValue + strlen(pcValue)))
  {
  	return 0;
  }
  
  /*如果值不对*/
  if (atoi(pcValue) < 0 || atoi(pcValue) > 255)
  {
    return 0;
  }
  pcBuf = pcTmp + 1;
  
  /*取前24b到31b*/
  memset( pcValue, 0x00, sizeof(pcValue) );
  strcpy( pcValue, pcBuf );
  /*如果有非法字符*/
  if (!isInt(pcValue, pcValue + strlen(pcValue)))
  {
  	return 0;
  }
  
  /*如果值不对*/
  if (atoi(pcValue) < 0 || atoi(pcValue) > 255)
  {
    return 0;
  }
  return 1;
}

/********************************************************
函数名：isBlank
函数功能：检查字符串是否为空格 
入口参数：
第    一：字符     I
返    回：1表示该字符为空格，0表示不是
创建时间：20130226
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
函数名：isInt
函数功能：检查字符串是为整型 
入口参数：
第    一：缓冲区的开始     I
第    二：缓冲区的结束     I 
返    回：1表示该字符串都为数字，0表示不是
创建时间：20130226
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
函数名：isDigitals
函数功能：检查字符串是否都为数字 
入口参数：
第    一：缓冲区的开始     I
第    二：缓冲区的结束     I 
返    回：1表示该字符串都为数字，0表示不是
创建时间：20130226
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
函数名称: strcmpIgnore
函数功能: 比较，忽略大小写 
作    者: 李高文 
建立时间: 
参    数：
第    一：第一个缓冲区   I 
第    二：第二个缓冲区   I
返    回：0表示相等，其它表示失败 
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
函数名称: strcmpIgnore
函数功能: 比较，忽略大小写 
作    者: 李高文 
建立时间: 
参    数：
第    一：第一个缓冲区   I 
第    二：第二个缓冲区   I
返    回：0表示相等，其它表示失败 
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
函数名称：logInitSyn
函数功能：同步日志初始化
参    数：
第    一：日志级别          I
返    回：0
作    者：李高文
建立时间：20160809
**********************************************************************/
int logInitSyn( int iaLevel )
{
  /*设置日志级别*/
  igSynDebugLevel = iaLevel;
  
  return SUCCESS;
}

/**********************************************************************
函数名称：logFmtSyn
函数功能：带格式化的同步日志
参    数：
第    一：级别（级别小于系统定义的就不打日志）    I
第    二：代码所在的文件   I
第    三：代码所在的行     I
第    四：日志文件名称     I
第    五：格式化           I
第    六：不定参数         I
作    者：李高文
建立时间：20160809
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
函数名称：logSyn
函数功能：不格式化的同步日志
参    数：
第    一：级别（级别小于系统定义的就不打日志）    I
第    二：代码所在的文件   I
第    三：代码所在的行     I
第    四：日志文件名称     I
第    五：日志缓冲区       I
第    六：日志长度         I
作    者：李高文
建立时间：20160812
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
