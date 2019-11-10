#define __LOG_C

#ifndef __LOG_H
  #include "log.h"
#endif

#define MAX_SYN_LOG_SIZE  8192        /*同步日志的最大值*/

#ifdef FILE_PATH_LEN
  #undef FILE_PATH_LEN
#endif
#define FILE_PATH_LEN     256         /*文件路径的最大值*/

#ifdef NAME_LEN
  #undef NAME_LEN
#endif
#define NAME_LEN          64          /*文件名称的最大值*/

#ifdef LOG_DATE_LEN
  #undef LOG_DATE_LEN
#endif
#define LOG_DATE_LEN          10          /*日期长度*/

#ifdef LOG_TIME_LEN
  #undef LOG_TIME_LEN
#endif
#define LOG_TIME_LEN          10          /*时间长度*/

#ifdef SUCCESS
  #undef SUCCESS
#endif
#define SUCCESS           0           /*成功标识*/

#ifdef ERROR
  #undef ERROR
#endif
#define ERROR             2           /*失败标识*/

//#define SYSTEM_WINDOWS64

/*
日志结构：系统时钟 日期 时间 毫秒 [文件名:所在行]
*/
#define FMT_DATE "%04d-%02d-%02d"
#define FMT_TIME "%02d:%02d:%02d:%03d"
#define FMT "%012ld %04d-%02d-%02d %02d:%02d:%02d:%03d [%s:%s:%d]\n%s"

int igSynDebugLevel;            /*同步日志级别*/
int igAsyDebugLevel;            /*异步日志级别*/

char pcgAsyLogFile[NAME_LEN];   /*异步日志文件名称*/
char *pcgLogBuf;                /*异步日志缓冲区*/
char *pcgLogBufEnd;             /*异步日志缓冲区结束位置*/
int  igLogBufSize;              /*异步日志缓冲区大小*/
char *pcgLogBufWrite;           /*异步日志，当前内存写位置*/
int  igLogUsedFlag;             /*异步日志，当前使用标识*/
int  igLogErrFlag;              /*异步日志错误标识*/
int  igLogRunState;             /*日志线程运行状态*/
pthread_mutex_t mutex;          /*互斥锁*/
pthread_t  igThreadId;
void *threadHandle;

#define LOG_NOT_USED      0     /*异步日志，未有使用*/
#define LOG_USED          1     /*异步日志，正在使用*/
#define LOG_SUCCESS       0     /*异步日志，正常标识*/
#define LOG_ERROR         1     /*异步日志，出错标识*/
#define THREAD_SUCCESS    0     /*异步日志线程状态，运行*/
#define THREAD_ERROR      1     /*异步日志线程状态，结束*/

#define MAX_LOOPS         30    /*写日志阻塞时最大的轮询次数*/

#define LOG_SLEEP_TIME    1     /*异步日志，遇阻塞时的睡眠时间，UNIX单位：微秒*/

/*获取占用权限*/
#define ASY_LOG_LOCK()   pthread_mutex_lock(&mutex); 

#define ASY_LOG_UNLOCK() pthread_mutex_unlock(&mutex);

#define ASY_LOG_THREAD_SLEEP() usleep(LOG_SLEEP_TIME);

#define ASY_SLEEP_TIME      10000   /*遇阻塞时的睡眠时间，UNIX单位：微秒*/
#define ASY_THREAD_SLEEP() usleep( ASY_SLEEP_TIME );

int  logFmtAsy( int iaLevel, char *pcaFile, int iaLine, const char *pcaFunc, char *pcaLogFile, const char *aFmt, ... );
int  logAsyWrite( );
void logAsyThread( );

typedef struct asyLogStruct
{
  int iLogLen;               /*日志内容的长度*/
  int iLevel;                /*日志级别*/
  int iLine;                 /*所在的代码行数*/
  char pcFileName[NAME_LEN]; /*代码文件名称*/
  char pcFuncName[NAME_LEN]; /*代码所在函数名称*/
  char pcDate[LOG_DATE_LEN];     /*日期*/
  char pcTime[LOG_TIME_LEN];     /*时间*/
}ASY_LOG_STRUCT;


void logAsyThread( );
void logRlsAsy( );

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
函数名称：logInitAsy
函数功能：异步日志初始化
参    数：
第    一：日志级别                I
第    二：异步日志文件名称        I
第    三：日志缓存大小，单位：MB  I
返    回：0
作    者：李高文
建立时间：20160809
**********************************************************************/
int logInitAsy( int iaLevel, char *pcaAsyLogFile, int iaLogBufSize )
{
	#ifdef SYSTEM_WINDOWS_64
	  unsigned long ulWindowsThreadId;
	#else/*#ifdef SYSTEM_UBUNTU*/
	  pthread_t iThreadId;
	#endif
	
  if (iaLogBufSize <= 0 || iaLogBufSize >= 2048)
  {
    return ERROR;
  }
  
  /*设置日志级别*/
  igAsyDebugLevel = iaLevel;
  
  /*设置日志名称*/
  memset( pcgAsyLogFile, 0x00, sizeof(pcgAsyLogFile) );
  strcpy( pcgAsyLogFile, pcaAsyLogFile );
  
  /*计算缓冲区大小*/
  igLogBufSize = iaLogBufSize<<20;
  
  /*分配内存并初始化*/
  pcgLogBuf = malloc( igLogBufSize );
  if (pcgLogBuf == NULL)
  {
    return ERROR;
  }
  memset( pcgLogBuf, 0x00, igLogBufSize );
  
  pcgLogBufEnd = pcgLogBuf + igLogBufSize;
  
  pcgLogBufWrite = pcgLogBuf;
   
  /*初始化标识*/
  igLogErrFlag = LOG_SUCCESS;
  
  #ifdef SYSTEM_WINDOWS_64
    threadHandle = CreateThread( NULL, 0, (void *)(&logAsyThread), NULL, 0, &ulWindowsThreadId );
    if (threadHandle == NULL)
    {
      return __LINE__;
    }
  #else/*#ifdef SYSTEM_UBUNTU*/
    if (pthread_create(&iThreadId, NULL, (void *) logAsyThread, NULL) != 0)
    {
      return __LINE__;
    }
  
    /*线程分离，结束时自动回收资源*/
    pthread_detach(iThreadId);
  #endif
  //pthread_create(&igThreadId, NULL, (void *) logAsyThread, NULL);
  
  /*设置日志线程状态*/
  igLogRunState = THREAD_SUCCESS;
  
  return SUCCESS;
}

/**********************************************************************
函数名称：logRlsAsy
函数功能：异步日志释放资源
参    数：
返    回：0
作    者：李高文
建立时间：20160810
**********************************************************************/
void logRlsAsy( )
{
  /*等待日志写完*/
  while (pcgLogBufWrite != pcgLogBuf)
  {
    ASY_LOG_THREAD_SLEEP();
  }
  
  /*结束日志线程*/
  igLogErrFlag = LOG_ERROR;
  
  while (igLogRunState == THREAD_SUCCESS)
  {
    ASY_LOG_THREAD_SLEEP();
  }
  
  if (pcgLogBuf != NULL)
  {
    free( pcgLogBuf );
    pcgLogBuf == NULL;
  }
  return;
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


/**********************************************************************
函数名称：logFmtAsy
函数功能：带格式化的异步日志
参    数：
第    一：级别（不做级别判断）           I
第    二：代码所在的文件   I
第    三：代码所在的行     I
第    四：日志文件名称（通常是NULL）     I
第    五：格式化           I
第    六：不定参数         I
作    者：李高文
建立时间：20160809
**********************************************************************/
int  logFmtAsy( int iaLevel, char *pcaFile, int iaLine, const char *pcaFunc, char *pcaLogFile, const char *aFmt, ... )
{
  va_list ap;
  FILE    *fp;
  char    pcBuf[MAX_SYN_LOG_SIZE+1];
  struct timeb  tp;
  struct tm *timeNow;
  int     iLogLen;
  int     iTotalLen;
  int     iLoop;
  int     iTmp;
  ASY_LOG_STRUCT asyLog;
  
  va_start( ap, aFmt );  
  vsprintf( pcBuf, aFmt, ap );
  va_end( ap );

  /*计算日志的大小*/
  iLogLen = strlen( pcBuf );
  if (iLogLen > MAX_SYN_LOG_SIZE)
  {
    return ERROR;
  }
  
  ftime(&tp);
  timeNow = ( localtime(&(tp.time)) );
  
  /*初始化日志头结构*/
  memset( &asyLog, 0x00, sizeof(asyLog) );
  
  /*设置日志头的值*/
  asyLog.iLogLen = iLogLen;
  asyLog.iLevel = iaLevel;
  strcpy( asyLog.pcFileName, pcaFile );
  strcpy( asyLog.pcFuncName, pcaFunc );
  asyLog.iLine = iaLine;
  sprintf( asyLog.pcDate, "%04d%02d%02d", timeNow->tm_year+1900, timeNow->tm_mon+1, timeNow->tm_mday );
  sprintf( asyLog.pcTime, "%02d%02d%02d%03d", timeNow->tm_hour, timeNow->tm_min, timeNow->tm_sec, tp.millitm );
  
  /*日志总长度*/
  iTotalLen = iLogLen + sizeof(ASY_LOG_STRUCT);

  
  /*占用*/
  ASY_LOG_LOCK();

  
  /*计算是否有足够的空间容纳当前日志，如果没有足够空间，调用函数，将日志从内存写入到硬盘*/
  iLoop = 0;
  while (1)
  {
    /*如果剩下的空间不够写日志*/
    if (pcgLogBufWrite + iTotalLen > pcgLogBufEnd)
    {
    	/*释放占用*/
      ASY_LOG_UNLOCK();
      
      /*调用函数，将日志从内存写入到硬盘*/
      logAsyWrite();
      
      /*占用*/
      ASY_LOG_LOCK();
      continue;
    }
    else
    {
    	/*写入日志头*/
      memcpy( pcgLogBufWrite, &asyLog, sizeof(asyLog) );
      
      /*写入日志内容*/
      memcpy( pcgLogBufWrite + sizeof(asyLog), pcBuf, iLogLen );
      
      /*设置新的日志的写位置*/
      pcgLogBufWrite += iTotalLen;
      
      /*释放占用*/
      ASY_LOG_UNLOCK();
  
      return SUCCESS;
    }
    iLoop++;
    if (iLoop >= MAX_LOOPS)
    {
    	/*占用*/
      ASY_LOG_LOCK();
      return ERROR;
    }
  }
}

/**********************************************************************
函数名称：logAsy
函数功能：带格式化的异步日志
参    数：
第    一：级别（不做级别判断）           I
第    二：代码所在的文件   I
第    三：代码所在的行     I
第    四：日志文件名称（通常是NULL）     I
第    五：日志缓冲区           I
第    六：日志长度         I
作    者：李高文
建立时间：20160812
**********************************************************************/
int  logAsy( int iaLevel, char *pcaFile, int iaLine, const char *pcaFunc, char *pcaLogFile, char *pcaBuf, int iaSize )
{
  FILE    *fp;
  struct timeb  tp;
  struct tm *timeNow;
  int     iLogLen;
  int     iTotalLen;
  int     iLoop;
  int     iTmp;
  ASY_LOG_STRUCT asyLog;
  
  ftime(&tp);
  timeNow = ( localtime(&(tp.time)) );
  
  /*初始化日志头结构*/
  memset( &asyLog, 0x00, sizeof(asyLog) );
  
  iLogLen = iaSize;
  
  /*设置日志头的值*/
  asyLog.iLogLen = iLogLen;
  asyLog.iLevel = iaLevel;
  strcpy( asyLog.pcFileName, pcaFile );
  strcpy( asyLog.pcFuncName, pcaFunc );
  asyLog.iLine = iaLine;
  sprintf( asyLog.pcDate, "%04d%02d%02d", timeNow->tm_year+1900, timeNow->tm_mon+1, timeNow->tm_mday );
  sprintf( asyLog.pcTime, "%02d%02d%02d%03d", timeNow->tm_hour, timeNow->tm_min, timeNow->tm_sec, tp.millitm );
  
  /*日志总长度*/
  iTotalLen = iLogLen + sizeof(ASY_LOG_STRUCT);

  
  /*占用*/
  ASY_LOG_LOCK();
    
  /*计算是否有足够的空间容纳当前日志，如果没有足够空间，调用函数，将日志从内存写入到硬盘*/
  iLoop = 0;
  while (1)
  {
    /*如果剩下的空间不够写日志*/
    if (pcgLogBufWrite + iTotalLen > pcgLogBufEnd)
    {
    	/*释放占用*/
      ASY_LOG_UNLOCK();
      
      /*调用函数，将日志从内存写入到硬盘*/
      logAsyWrite();
      
      /*占用*/
      ASY_LOG_LOCK();
      continue;
    }
    else
    {
    	/*写入日志头*/
      memcpy( pcgLogBufWrite, &asyLog, sizeof(asyLog) );
      
      /*写入日志内容*/
      memcpy( pcgLogBufWrite + sizeof(asyLog), pcaBuf, iLogLen );
      
      /*设置新的日志的写位置*/
      pcgLogBufWrite += iTotalLen;
      
      /*释放占用*/
      ASY_LOG_UNLOCK();
  
      return SUCCESS;
    }
    iLoop++;
    if (iLoop >= MAX_LOOPS)
    {
    	/*占用*/
      ASY_LOG_LOCK();
      return ERROR;
    }
  }
}

/**********************************************************************
函数名称：logAsyWrite
函数功能：将日志从内存写入到硬盘
参    数：
返    回：
作    者：李高文
建立时间：20160809
**********************************************************************/
int logAsyWrite( )
{
  char pcLogPath[FILE_PATH_LEN];
  struct timeb  tp;
  struct tm *timeNow;
  FILE *fp;
  
  if (pcgLogBuf == NULL)
  {
  	return SUCCESS;
  }
  /*如果没有数据处理*/
  if (pcgLogBufWrite == pcgLogBuf)
  {
    return SUCCESS;
  }
  
  ftime(&tp);
  timeNow = ( localtime(&(tp.time)) );
  
  sprintf( pcLogPath, "%s.%02d%02d", pcgAsyLogFile, timeNow->tm_mon+1, timeNow->tm_mday );
  
  /*占用*/
  ASY_LOG_LOCK();
  
  fp = fopen( pcLogPath, "ab+" );
  if (fp == NULL)
  {
    fclose(fp);
    fp = fopen( pcLogPath, "wb+" );
    if (fp == NULL)
    {
      return ERROR;
    }
  }
  
  /*该函数执行时，将内存中的所有日志写入到文件*/  
  fwrite( pcgLogBuf, 1, pcgLogBufWrite - pcgLogBuf, fp );
  pcgLogBufWrite = pcgLogBuf;
  fflush(fp);
  fclose(fp);
  
  /*释放占用*/
  ASY_LOG_UNLOCK();
  
  return SUCCESS;
}

/**********************************************************************
函数名称：logAsyThread
函数功能：异步日志执行线程
参    数：
返    回：
作    者：李高文
建立时间：20160810
**********************************************************************/
void logAsyThread( )
{
  while (igLogErrFlag == LOG_SUCCESS)
  {
    logAsyWrite();
    ASY_THREAD_SLEEP();
  }
  
  /*线程结束状态*/
  igLogRunState = THREAD_ERROR;
  
  return;
}


