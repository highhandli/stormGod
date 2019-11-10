#define __KEY_VALUE_INIT_C

#ifndef __COMMON_HEAD_H
  #include "commonHead.h"
#endif

char *AllTrim ( char *pcaStr );
int isBlank( char c );
int getDateTime( char *pcaDate, char *pcaTime );
int strcmpIgnore( char *p, char *p1 );
int checkPort( char *pcaPort );
int isDigitals( char *pcaBufStart, char *pcaBufEnd );
int getDateTime( char *pcaDate, char *pcaTime );

int keyValueInit( char *pcaInfoPath );
void keyValueRelease( );
int createServerSocket( );
int createOverallBufAndStartThreads( );
int checkFileLimits( );
int checkAndLoadKeyValueFile( );
int keyValueGetInfo( char *pcaInfoPath );

void shortSocket( int *iaIndex );
void longSocket( int *iaIndex );

void managerSocket( );

void threadControl( );

//void releaseBlockMainAll();

/**********************************************************************
函数名称: keyValueInti 
函数功能: KEY VALUE 初始化 
参    数：
第    一：配置信息文件 
作    者: 李高文
建立时间: 20160818
**********************************************************************/
int keyValueInit( char *pcaInfoPath )
{
  int iRet;
  #ifdef SYSTEM_WINDOWS_64
	  WSADATA wsaData;
	#endif
	
	#ifdef SYSTEM_WINDOWS_64
	  /*调用Windows Sockets DLL*/
    if (WSAStartup(MAKEWORD(2,1),&wsaData)) 
    { 
      fprintf( stderr, "Winsock无法初始化!\n" );
      WSACleanup();
      return 0;
    }
  #endif
  
  /*初始化查询ID，不能为0，因为查询ID为0表示空的ID，客户端进行查询时送0过来*/
  llgSelectId = 1;
  
  /*初始化全局变量*/
  memset( pcgUser, 0x00, sizeof(pcgUser) );
  memset( pcgPass, 0x00, sizeof(pcgPass) );
  memset( pcgManagerListen, 0x00, sizeof(pcgManagerListen) );
  memset( pcgShortListen, 0x00, sizeof(pcgShortListen) );
  memset( pcgLongListen, 0x00, sizeof(pcgLongListen) );
  memset( pcgLogPath, 0x00, sizeof(pcgLogPath) );
  memset( pcgLogName, 0x00, sizeof(pcgLogName) );
  memset( pcgKeyRamSize, 0x00, sizeof(pcgKeyRamSize) );
  memset( pcgValueRamSize, 0x00, sizeof(pcgValueRamSize) );
  memset( pcgKeyFilePath, 0x00, sizeof(pcgKeyFilePath) );
  memset( pcgKeyFileName, 0x00, sizeof(pcgKeyFileName) );
  memset( pcgValueFilePath, 0x00, sizeof(pcgValueFilePath) );
  memset( pcgValueFileName, 0x00, sizeof(pcgValueFileName) );
  memset( pcgKeyMappingPath, 0x00, sizeof(pcgKeyMappingPath) );
  memset( pcgKeyMappingName, 0x00, sizeof(pcgKeyMappingName) );
  memset( pcgValueMappingPath, 0x00, sizeof(pcgValueMappingPath) );
  memset( pcgValueMappingName, 0x00, sizeof(pcgValueMappingName) );
  memset( pcgFileUpperSize, 0x00, sizeof(pcgFileUpperSize) );
  memset( pcgFileMaxSize, 0x00, sizeof(pcgFileMaxSize) );
  memset( pcgSelectTempPath, 0x00, sizeof(pcgSelectTempPath) );
  
  memset( &sgBase, 0x00, sizeof(sgBase) );
  
  memset( pcgKeyFree, 0x00, sizeof(pcgKeyFree) );
  memset( pcgValueFree, 0x00, sizeof(pcgValueFree) );
  
  /*初始化主数据的所有变量*/
  memset( &sgGlobalVariableMain, 0x00, sizeof(sgGlobalVariableMain) );
  
  /*加载配置文件信息*/
  iRet = keyValueGetInfo( pcaInfoPath );
  if (iRet)
  {
    fprintf( stderr, "加载配置信息文件[%s]失败\n", pcaInfoPath );
    return __LINE__;
  }
  
  fprintf( stdout, "加载配置信息完成！\n" );fflush(stdout);
  
  /*创建全局缓冲区*/
  iRet = createOverallBufAndStartThreads();
  if (iRet)
  {
  	#ifdef SYSTEM_WINDOWS_64
	    WSACleanup();
	  #endif
    return iRet;
  }
  
  fprintf( stdout, "创建全局缓冲区完成\n" );fflush(stdout);
  
  /*检查 KEY 和 VALUE 文件的完整性，并加载相关信息*/
  iRet = checkAndLoadKeyValueFile();
  if (iRet)
  {
  	fprintf( stdout, "检查 KEY 和 VALUE 文件的完整性，并加载相关失败，%d！\n", iRet );fflush(stdout);
    keyValueRelease();
    exit(0);
  }
  
  fprintf( stdout, "载入数据文件和创建线程完成！\n" );fflush(stdout);
  
  #ifdef SYSTEM_WINDOWS_64
    atexit( (void *)&keyValueRelease);
  #else
    signal( SIGABRT, &keyValueRelease );
    signal( SIGFPE, &keyValueRelease );
    signal( SIGILL, &keyValueRelease );
    signal( SIGINT, &keyValueRelease );
    signal( SIGSEGV, &keyValueRelease );
    signal( SIGTERM, &keyValueRelease );
  #endif
  
  return SUCCESS;
}

/**********************************************************************
函数名称: keyValueRelease
函数功能: KEY VALUE 程序退出前释放资源 
参    数：
第    一：配置信息文件 
作    者: 李高文
建立时间: 20160818
**********************************************************************/
void keyValueRelease( )
{
	/*将内存中的所有数据写入到硬盘中*/
	/*正式应用时，这里要加上*/
  //releaseBlockMainAll();
	
	//fprintf( stdout, "llgKeyFileOffset[%lld] llgKeyRamSizeMain[%lld]\n", llgKeyFileOffset<< BLOCK_LEFT_MOVE, llgKeyRamSizeMain );	
  //fprintf( stdout, "llgValueFileOffset[%lld] llgValueRamOffset[%lld]\n", llgValueFileOffset, llgValueRamSizeMain );
  //fprintf( stdout, "llgLoops[%lld]\n", sgGlobalVariableMain.llgLoopsMain );
  //fprintf( stdout, "llgKeyValueSize[%lld]\n", sgBase[iaIndepentFileId].llKeyValueSize + sgGlobalVariableMain.llgKeyValueSizeMain );
  
  int iTmp;
  __int64 llSize;
  __int64 llSize2;
  __int64 llRam;
  __int64 llValue;
  
  llSize = 0;
  llRam = 0;
  llValue = 0;
  for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
  {
  	llSize += sgGlobalVariableMain[iTmp].llgKeyValueSizeMain;
  	llRam += sgGlobalVariableMain[iTmp].llgKeyRamOffsetMain;
  	llValue += sgGlobalVariableMain[iTmp].llgValueRamOffsetMain;
  }
  
  llSize2 = 0;
  for (iTmp = 0; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
  {
  	llSize2 += sgBase[iTmp].llKeyValueSize;
  }
  fprintf( stdout, "total keys[%lld]ramkeys[%lld]disckeys[%lld]llgKeyRamOffsetMain[%lld]llgValueRamOffsetMain[%lld]\n", llSize + llSize2, llSize, llSize2, llRam, llValue );
  
  /*处理 KEY， VALUE 文件*/
  for (iTmp = 0; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
  {
  	rewind( fpgKey[iTmp] );
	  fwrite( &(sgBase[iTmp]), 1, sizeof(KEY_VALUE_BASE_INFO), fpgKey[iTmp] );
	  
	  fflush( fpgKey[iTmp] );
  	fclose( fpgKey[iTmp] );
  	
  	//fprintf( stdout, "llgKeyValueSize[%lld]\n", sgBase[iTmp].llKeyValueSize );
  	
  	fflush( fpgValue[iTmp] );
	  fclose( fpgValue[iTmp] );
  }
  #if 0
  /*将内存中的数据写入到硬盘*/
  rewind( fpgMappingKey );
  rewind( fpgMappingValue );
	fwrite( &sgGlobalVariableMain, 1, sizeof(sgGlobalVariableMain), fpgMappingKey );
    
  /*写入文件数据*/
  for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
	{
		fwrite( sgGlobalVariableMain[iTmp].pcgKeyBufMain, 1, sgGlobalVariableMain[iTmp].llgKeyRamOffsetMain, fpgMappingKey );
    fwrite( sgGlobalVariableMain[iTmp].pcgValueBufMain, 1, sgGlobalVariableMain[iTmp].llgValueRamOffsetMain, fpgMappingValue );
  }
  #endif
  fflush( fpgMappingKey );
  fclose( fpgMappingKey );
  fflush( fpgMappingValue );
  fclose( fpgMappingValue );

	#ifdef SYSTEM_WINDOWS_64
	  WSACleanup();
	#endif
	
	return ;
}

/**********************************************************************
函数名称: createOverallBufAndStartThreads 
函数功能: 创建全局缓冲区，以及启动线程
参    数：
作    者: 李高文
建立时间: 20160818
**********************************************************************/
int createOverallBufAndStartThreads( )
{
	char pcFilePath[PATH_LEN];
	int iTmp;
	int iTmp2;
	int iManagerSocket;
	int iShortSocket;
	int iLongSocket;
	#ifdef SYSTEM_WINDOWS_64
	  unsigned long ulWindowsThreadId;
	#else/*#ifdef SYSTEM_UBUNTU*/
	  pthread_t iThreadId;
	#endif
	__int64 llSize;
	__int64 llKey;
	__int64 llValue;
	int iKeyFileFlag = 0;
	int iValueFileFlag = 0;
	__int64 llFileLen;
	
	memset( pcFilePath, 0x00, sizeof(pcFilePath) );
  #ifdef SYSTEM_WINDOWS_64
    if (strlen(pcgKeyMappingPath) == 0)
    {
      sprintf( pcFilePath, "%s", pcgKeyMappingName );
    }
    else
    {
    	sprintf( pcFilePath, "\"%s\\%s\"", pcgKeyMappingPath, pcgKeyMappingName );
    }
  #else
    if (strlen(pcgKeyMappingPath) == 0)
    {
      sprintf( pcFilePath, "%s", pcgKeyMappingName );
    }
    else
    {
    	sprintf( pcFilePath, "%s/%s", pcgKeyMappingPath, pcgKeyMappingName );
    }
  #endif
   
  fpgMappingKey = fopen64( pcFilePath, "rb+" );
  if (fpgMappingKey == NULL)
  {
  	fpgMappingKey = fopen64( pcFilePath, "wb+" );
    if (fpgMappingKey == NULL)
    {
      fprintf( stderr, "不能创建或打开 KEY 映射文件[%s]\n", pcFilePath );
      return __LINE__;
    }
    iKeyFileFlag = 1;
  }

#if 0  
  fseeko64( fpgMappingKey, 0, SEEK_END );
  llFileLen = ftello64( fpgMappingKey );
  rewind( fpgMappingKey );
  
  if (llFileLen == 0)
  {
  	iKeyFileFlag = 0;
  }
#endif  
  
  memset( pcFilePath, 0x00, sizeof(pcFilePath) );
  #ifdef SYSTEM_WINDOWS_64
    if (strlen(pcgValueMappingPath) == 0)
    {
      sprintf( pcFilePath, "%s", pcgValueMappingName );
    }
    else
    {
    	sprintf( pcFilePath, "\"%s\\%s\"", pcgValueMappingPath, pcgValueMappingName );
    }
  #else
    if (strlen(pcgValueMappingPath) == 0)
    {
      sprintf( pcFilePath, "%s%d", pcgValueMappingName, iTmp );
    }
    else
    {
    	sprintf( pcFilePath, "%s/%s", pcgValueMappingPath, pcgValueMappingName );
    }
  #endif
   
  fpgMappingValue = fopen64( pcFilePath, "rb+" );
  if (fpgMappingValue == NULL)
  {
  	fpgMappingValue = fopen64( pcFilePath, "wb+" );
    if (fpgMappingValue == NULL)
    {
      fprintf( stderr, "不能创建或打开 VALUE 映射文件[%s]\n", pcFilePath );
      return __LINE__;
    }
    iValueFileFlag = 1;
  }

#if 0  
  fseeko64( fpgMappingValue, 0, SEEK_END );
  llFileLen = ftello64( fpgMappingValue );
  rewind( fpgMappingValue );
  
  if (llFileLen == 0)
  {
  	iValueFileFlag = 0;
  }
#endif
    
  if ((iKeyFileFlag == 0 && iValueFileFlag == 1) || (iKeyFileFlag == 1 && iValueFileFlag == 0))
  {
  	fprintf( stderr, " KEY 映射文件 或 VALUE 映射文件已损坏\n" );
    return __LINE__;
  }
  
  /*计算 KEY 缓冲区*/
	llKey = (__int64)(llgKeyRamSizeMain/INDEPENDENT_SIZE);
	llKey = ((llKey%BLOCK_SIZE) == 0 ? (__int64)(llKey/BLOCK_SIZE):(__int64)(llKey/BLOCK_SIZE) + 1) * BLOCK_SIZE;
	
	/*计算 VALUE 缓冲区*/
  llValue = (__int64)(llgValueRamSizeMain/INDEPENDENT_SIZE);
  llValue = ((llValue%BLOCK_SIZE) == 0 ? (__int64)(llValue/BLOCK_SIZE):(__int64)(llValue/BLOCK_SIZE) + 1) * BLOCK_SIZE;
  
  /*如果都是新建文件*/
  if (iKeyFileFlag == 1 && iValueFileFlag == 1)
  {
  	for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
	  {
      sgGlobalVariableMain[iTmp].pcgKeyBufMain = malloc( llKey );
      if (sgGlobalVariableMain[iTmp].pcgKeyBufMain == NULL)
      {
        fprintf( stderr, "申请 KEY 缓冲区大小[%lld]失败\n", llKey );
        return __LINE__;
      }
      memset( sgGlobalVariableMain[iTmp].pcgKeyBufMain, 0x00, llKey );
      sgGlobalVariableMain[iTmp].llgKeyRamSizeMain = llKey;
      *((__int64 *)(sgGlobalVariableMain[iTmp].pcgKeyFreeMain + BLOCK_SIZE - sizeof(__int64))) = NO_BLOCK;
      
      sgGlobalVariableMain[iTmp].pcgValueBufMain = malloc( llValue );
      if (sgGlobalVariableMain[iTmp].pcgValueBufMain == NULL)
      {
        fprintf( stderr, "申请 KEY 缓冲区大小[%lld]失败\n", llValue );
        return __LINE__;
      }
      memset( sgGlobalVariableMain[iTmp].pcgValueBufMain, 0x00, llValue );
      sgGlobalVariableMain[iTmp].llgValueRamSizeMain = llValue;
      *((__int64 *)(sgGlobalVariableMain[iTmp].pcgValueFreeMain + BLOCK_SIZE - sizeof(__int64))) = NO_BLOCK;
    }
  }
  /*检查新的内存缓冲区是否比文件小*/
  else
  {
  	if (fread( &sgGlobalVariableMain, 1, sizeof(sgGlobalVariableMain), fpgMappingKey ) != sizeof(sgGlobalVariableMain))
  	{
  		fprintf( stderr, " KEY 映射文件 或 VALUE 映射文件已损坏\n" );
  		return __LINE__;
  	}
  	
  	/*检查*/
  	for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
	  {
      if (sgGlobalVariableMain[iTmp].llgKeyRamOffsetMain > llKey )
      {
      	fprintf( stderr, "映射文件和设置的内存大小不匹配，请重新设置内存大小\n" );
        return __LINE__;
      }
      if (sgGlobalVariableMain[iTmp].llgValueRamOffsetMain > llValue )
      {
      	fprintf( stderr, "映射文件和设置的内存大小不匹配，请重新设置内存大小\n" );
        return __LINE__;
      }
    }
    
    /*载入文件数据*/
    for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
	  {
	  	sgGlobalVariableMain[iTmp].pcgKeyBufMain = malloc( llKey );
      if (sgGlobalVariableMain[iTmp].pcgKeyBufMain == NULL)
      {
        fprintf( stderr, "申请 KEY 缓冲区大小[%lld]失败\n", llKey );
        return __LINE__;
      }
      memset( sgGlobalVariableMain[iTmp].pcgKeyBufMain, 0x00, llKey );
      sgGlobalVariableMain[iTmp].llgKeyRamSizeMain = llKey;
      
      sgGlobalVariableMain[iTmp].pcgValueBufMain = malloc( llValue );
      if (sgGlobalVariableMain[iTmp].pcgValueBufMain == NULL)
      {
        fprintf( stderr, "申请 KEY 缓冲区大小[%lld]失败\n", llValue );
        return __LINE__;
      }
      memset( sgGlobalVariableMain[iTmp].pcgValueBufMain, 0x00, llValue );
      sgGlobalVariableMain[iTmp].llgValueRamSizeMain = llValue;
      
	  	if (fread( sgGlobalVariableMain[iTmp].pcgKeyBufMain, 1, sgGlobalVariableMain[iTmp].llgKeyRamOffsetMain, fpgMappingKey ) != sgGlobalVariableMain[iTmp].llgKeyRamOffsetMain)
	  	{
	  		fprintf( stderr, " KEY 映射文件 或 VALUE 映射文件已损坏\n" );
  		  return __LINE__;
	  	}
      if (fread( sgGlobalVariableMain[iTmp].pcgValueBufMain, 1, sgGlobalVariableMain[iTmp].llgValueRamOffsetMain, fpgMappingValue ) != sgGlobalVariableMain[iTmp].llgValueRamOffsetMain)
	  	{
	  		fprintf( stderr, " KEY 映射文件 或 VALUE 映射文件已损坏\n" );
  		  return __LINE__;
	  	}
    }
  }

  iManagerSocket = createServerSocket( igManagerListen );
	if (iShortSocket == -1)
	{
		fprintf( stderr, "创建短连接侦听失败， PORT[%d]\n", igManagerListen );
    return __LINE__;
	}
  
  iShortSocket = createServerSocket( igShortListen );
	if (iShortSocket == -1)
	{
		fprintf( stderr, "创建短连接侦听失败， PORT[%d]\n", igShortListen );
    return __LINE__;
	}
	igShortSocket = iShortSocket;
	
	iLongSocket = createServerSocket( igLongListen );
	if (iLongSocket == -1)
	{
		fprintf( stderr, "创建短连接侦听失败， PORT[%d]\n", igLongListen );
    return __LINE__;
	}
	igLongSocket = iLongSocket;
	
	memset( &sgManagerThread, 0x00, sizeof(sgManagerThread) );
	memset( &sgShortThreadArray, 0x00, sizeof(sgShortThreadArray) );
	memset( &sgLongThreadArray, 0x00, sizeof(sgLongThreadArray) );
  
  /*启动管理线程函数处理*/
  sgManagerThread.iSocket = iManagerSocket;
  sgManagerThread.iRunState = THREAD_RUN_STATE_RUN;
	sgManagerThread.iAcceptState = ACCEPT_STATE_NO;
  #ifdef SYSTEM_WINDOWS_64
    sgManagerThread.threadHandle = CreateThread( NULL, 0, (void *)(&managerSocket), NULL, 0, &ulWindowsThreadId );
    if (sgManagerThread.threadHandle == NULL)
    {
      return __LINE__;
    }
  #else/*#ifdef SYSTEM_UBUNTU*/
    if (pthread_create(&(sgManagerThread.threadId), NULL, (void *)(&managerSocket), NULL ) != 0)
    {
      return __LINE__;
    }
  
    /*线程分离，结束时自动回收资源*/
    pthread_detach(sgManagerThread.threadId);
  #endif
  
	for (iTmp = 0; iTmp < MIN_THREADS; iTmp++)
	{
		sgShortThreadArray[iTmp].iSocket = igShortSocket;
		sgShortThreadArray[iTmp].iRunState = THREAD_RUN_STATE_RUN;
		sgShortThreadArray[iTmp].iAcceptState = ACCEPT_STATE_NO;
		
    /*启动短连接线程函数处理*/
    #ifdef SYSTEM_WINDOWS_64
      sgShortThreadArray[iTmp].threadHandle = CreateThread( NULL, 0, (void *)(&shortSocket), &iTmp, 0, &ulWindowsThreadId );
      if (sgShortThreadArray[iTmp].threadHandle == NULL)
      {
        return __LINE__;
      }
      THREAD_THREAD_SLEEP();
    #else/*#ifdef SYSTEM_UBUNTU*/
      if (pthread_create(&(sgShortThreadArray[iTmp].threadId), NULL, (void *)(&shortSocket), &iTmp) != 0)
      {
        return __LINE__;
      }
    
      /*线程分离，结束时自动回收资源*/
      pthread_detach(sgShortThreadArray[iTmp].threadId);
    #endif
	}
	for (iTmp = MIN_THREADS; iTmp < MAX_THREADS; iTmp++)
	{
		sgShortThreadArray[iTmp].iSocket = igShortSocket;
		sgShortThreadArray[iTmp].iRunState = THREAD_RUN_STATE_STOP;
		sgShortThreadArray[iTmp].iAcceptState = ACCEPT_STATE_NO;
	}
	igShortThreads = MIN_THREADS;
	
  for (iTmp = 0; iTmp < MIN_THREADS; iTmp++)
	{
    sgLongThreadArray[iTmp].iSocket = igLongSocket;
		sgLongThreadArray[iTmp].iRunState = THREAD_RUN_STATE_RUN;
		sgLongThreadArray[iTmp].iAcceptState = ACCEPT_STATE_NO;
		
    /*启动短连接线程函数处理*/
    #ifdef SYSTEM_WINDOWS_64
      sgLongThreadArray[iTmp].threadHandle = CreateThread( NULL, 0, (void *)(&longSocket), &iTmp, 0, &ulWindowsThreadId );
      if (sgLongThreadArray[iTmp].threadHandle == NULL)
      {
        return __LINE__;
      }
      THREAD_THREAD_SLEEP();
    #else/*#ifdef SYSTEM_UBUNTU*/
      if (pthread_create(&(sgLongThreadArray[iTmp].threadId), NULL, (void *)(&longSocket), &iTmp) != 0)
      {
        return __LINE__;
      }
    
      /*线程分离，结束时自动回收资源*/
      pthread_detach(sgLongThreadArray[iTmp].threadId);
    #endif
	}
	for (iTmp = MIN_THREADS; iTmp < MAX_THREADS; iTmp++)
	{
		sgLongThreadArray[iTmp].iSocket = igLongSocket;
		sgLongThreadArray[iTmp].iRunState = THREAD_RUN_STATE_STOP;
		sgLongThreadArray[iTmp].iAcceptState = ACCEPT_STATE_NO;
	}
	
	igLongThreads = MIN_THREADS;
	
	/*启动控制程函数处理*/
  sgThreadControl.iRunState = THREAD_RUN_STATE_RUN;
  #ifdef SYSTEM_WINDOWS_64
    sgThreadControl.threadHandle = CreateThread( NULL, 0, (void *)(&threadControl), NULL, 0, &ulWindowsThreadId );
    if (sgThreadControl.threadHandle == NULL)
    {
      return __LINE__;
    }
  #else/*#ifdef SYSTEM_UBUNTU*/
    if (pthread_create(&(sgThreadControl.threadId), NULL, (void *)(&threadControl), NULL ) != 0)
    {
      return __LINE__;
    }
  
    /*线程分离，结束时自动回收资源*/
    pthread_detach(sgThreadControl.threadId);
  #endif
    
  return SUCCESS;
}

/**********************************************************************
函数名称: checkAndLoadKeyValueFile 
函数功能: 检查 KEY 和 VALUE 文件的完整性，并加载相关信息
参    数：
作    者: 李高文
建立时间: 20160818
**********************************************************************/
int checkAndLoadKeyValueFile( )
{
	char pcFilePath[PATH_LEN];
  char pcData[BLOCK_SIZE];
  char pcDate[DATE_LEN];
  char pcTime[TIME_LEN];
  char c;
  int  iTmp;
  
  for (iTmp = 0; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
  {
  	memset( pcFilePath, 0x00, sizeof(pcFilePath) );
    #ifdef SYSTEM_WINDOWS_64
      if (strlen(pcgKeyFilePath) == 0)
      {
        sprintf( pcFilePath, "%s%d", pcgKeyFileName, iTmp );
      }
      else
      {
      	sprintf( pcFilePath, "\"%s\\%s%d\"", pcgKeyFilePath, pcgKeyFileName, iTmp );
      }
    #else
      if (strlen(pcgKeyFilePath) == 0)
      {
        sprintf( pcFilePath, "%s%d", pcgKeyFileName, iTmp );
      }
      else
      {
      	sprintf( pcFilePath, "%s/%s%d", pcgKeyFilePath, pcgKeyFileName, iTmp );
      }
    #endif
    
    fpgKey[iTmp] = fopen64( pcFilePath, "rb+" );
    if (fpgKey[iTmp] == NULL)
    {
      fpgKey[iTmp] = fopen64( pcFilePath, "wb+" );
      if (fpgKey[iTmp] == NULL)
      {
        fprintf( stderr, "不能创建或打开 KEY 文件[%s]\n", pcFilePath );
        return __LINE__;
      }
    }
    /*获取 KEY 文件大小*/
    fseeko64( fpgKey[iTmp], 0, SEEK_END );
    llgKeyFileSize[iTmp] = ftello64( fpgKey[iTmp] );
    llgKeyFileDisc[iTmp] = llgKeyFileSize[iTmp] >> BLOCK_LEFT_MOVE;
    rewind( fpgKey[iTmp] );
    
    /*是否超出文件最大值*/
    if (llgKeyFileSize[iTmp] > llgFileMaxSize)
    {
      fprintf( stderr, " KEY 文件 超出最大值\n" );
      return __LINE__;
    }
    
    memset( pcFilePath, 0x00, sizeof(pcFilePath) );
    #ifdef SYSTEM_WINDOWS_64
      if (strlen(pcgKeyFilePath) == 0)
      {
        sprintf( pcFilePath, "%s%d", pcgValueFileName, iTmp );
      }
      else
      {
      	sprintf( pcFilePath, "\"%s\\%s%d\"", pcgValueFilePath, pcgValueFileName, iTmp );
      }
    #else
      if (strlen(pcgKeyFilePath) == 0)
      {
        sprintf( pcFilePath, "%s%d", pcgValueFileName, iTmp );
      }
      else
      {
      	sprintf( pcFilePath, "%s/%s%d", pcgValueFilePath, pcgValueFileName, iTmp );
      }
    #endif
    
    fpgValue[iTmp] = fopen64( pcFilePath, "rb+" );
    if (fpgValue[iTmp] == NULL)
    {
      fpgValue[iTmp] = fopen64( pcFilePath, "wb+" );
      if (fpgValue[iTmp] == NULL)
      {
        fprintf( stderr, "不能创建或打开 VALUE 文件[%s]\n", pcFilePath );
        return __LINE__;
      }
    }
    /*获取 VALUE 文件大小*/
    fseeko64( fpgValue[iTmp], 0, SEEK_END );
    llgValueFileSize[iTmp] = ftello64( fpgValue[iTmp] );
    rewind( fpgValue[iTmp] );
    
    /*是否超出文件最大值*/
    if (llgValueFileSize[iTmp] > llgFileMaxSize)
    {
      fprintf( stderr, " VALUE 文件 超出最大值\n" );
      return __LINE__;
    }
    
    /* KEY 文件大小是否满足块的倍数，0、3个块、其他，分别对应 第一次创建、只创建但未使用、已用*/
    
    //fprintf( stdout, "%s %lld %lld\n", pcFilePath, llgKeyFileSize[iTmp], llgValueFileSize[iTmp] );
    
    /*如果有一个文件为空，而另一个文件不为空，表示文件损坏*/
    if ((llgKeyFileSize[iTmp] == 0 && llgValueFileSize[iTmp] != 0) || (llgKeyFileSize[iTmp] != 0 && llgValueFileSize[iTmp] == 0))
    {
      fprintf( stderr, " KEY 文件 或 VALUE 文件已损坏\n" );
      return __LINE__;
    }
    
    /*如果都是初次创建*/
    if (llgKeyFileSize[iTmp] == 0 && llgValueFileSize[iTmp] == 0)
    {
      /*设置创建时间*/
      getDateTime( sgBase[iTmp].pcCreateDateTime, (sgBase[iTmp].pcCreateDateTime) + DATE_LEN );
      
      /*设置启动时间*/
      getDateTime( sgBase[iTmp].pcStartDateTime, (sgBase[iTmp].pcStartDateTime) + DATE_LEN );
      
      /*设置当前 KEY 文件使用偏移量，单位：BLOCK_SIZE*/
      sgBase[iTmp].llKeyFileOffset = 3;
      
      /*设置当前 KEY 文件中的 KEY 的一级数据块地址，单位：BLOCK_SIZE*/
      sgBase[iTmp].llFirstDisc = 0;
      
      /*设置当前 KEY 文件中的 KEY 的当前级数*/
      sgBase[iTmp].llLoops = 0;
      
      /*往 KEY 文件写入基础信息*/
      memset( pcData, 0x00, sizeof(pcData) );
      memcpy( pcData, &(sgBase[iTmp]), sizeof(KEY_VALUE_BASE_INFO) );
      fwrite( pcData, 1, sizeof(pcData), fpgKey[iTmp] );
      
      /*写入 KEY 文件空闲记录块*/
      *((__int64 *)(pcgKeyFree[iTmp] + BLOCK_SIZE - sizeof(__int64))) = NO_BLOCK;
      fwrite( pcgKeyFree[iTmp], 1, BLOCK_SIZE, fpgKey[iTmp] );  
      
      /*写入 VALUE 文件空闲记录块*/
      *((__int64 *)(pcgValueFree[iTmp] + BLOCK_SIZE - sizeof(__int64))) = NO_BLOCK;
      fwrite( pcgValueFree[iTmp], 1, BLOCK_SIZE, fpgKey[iTmp] );  
      
      /*设置 KEY 文件大小为步增值*/
      fseeko64( fpgKey[iTmp], llgFileUpperSize - 1, SEEK_SET );
      c = 0x00;
      fwrite( &c, 1, sizeof(c), fpgKey[iTmp] );
      
      llgKeyFileSize[iTmp] = llgFileUpperSize;
      llgKeyFileDisc[iTmp] = llgKeyFileSize[iTmp] >> BLOCK_LEFT_MOVE;
      
      /*设置 VALUE 文件大小为步增值*/
      fseeko64( fpgValue[iTmp], llgFileUpperSize - 1, SEEK_SET );
      c = 0x00;
      fwrite( &c, 1, sizeof(c), fpgValue[iTmp] );
      
      llgValueFileSize[iTmp] = llgFileUpperSize;
      
    }
    /*不是初次创建*/
    else
    {
      /*如果不是满足块的倍数，或者小于2个块，其中，基础信息1个，2个释放记录块*/
      if ((llgKeyFileSize[iTmp]%BLOCK_SIZE) != 0  || llgKeyFileSize[iTmp]>>BLOCK_LEFT_MOVE < 3)
      {
        fprintf( stderr, " KEY 文件 或 VALUE 文件已损坏\n" );
        return __LINE__;
      }
      
      /*从基础信息块读取 KEY 和 VALUE 文件使用偏移量*/
      memset( pcData, 0x00, sizeof(pcData) );
      fread( pcData, 1, sizeof(pcData), fpgKey[iTmp] );
      
      memset( &sgBase[iTmp], 0x00, sizeof(KEY_VALUE_BASE_INFO) );
      memcpy( &sgBase[iTmp], pcData, sizeof(KEY_VALUE_BASE_INFO) );
          
      /*检查偏移量是否大于文件大小*/
      if (((sgBase[iTmp].llKeyFileOffset) << BLOCK_LEFT_MOVE) > llgKeyFileSize[iTmp] || sgBase[iTmp].llValueFileOffset > llgValueFileSize[iTmp])
      {
        fprintf( stderr, " KEY 文件 或 VALUE 文件已损坏\n" );
        return __LINE__;
      }
      
      /*读取 KEY 文件空闲记录块*/
      fread( pcgKeyFree[iTmp], 1, BLOCK_SIZE, fpgKey[iTmp] ); 
      
      /*读取 VALUE 文件空闲记录块*/
      fread( pcgValueFree[iTmp], 1, BLOCK_SIZE, fpgKey[iTmp] ); 
      
      /*设置启动时间*/
      memset( pcDate, 0x00, sizeof(pcDate) );
      memset( pcTime, 0x00, sizeof(pcTime) );
      getDateTime( pcDate, pcTime );
      sprintf( sgBase[iTmp].pcStartDateTime, "%s%s", pcDate, pcTime );
      rewind(fpgKey[iTmp]);
      fwrite( &(sgBase[iTmp]), 1, sizeof(KEY_VALUE_BASE_INFO), fpgKey[iTmp] );
      fflush(fpgKey[iTmp]);
    }
  }
  return SUCCESS;
}

/**********************************************************************
函数名称: keyValueGetInfo
函数功能: 加载配置文件信息
参    数：
第    一：配置信息文件 
作    者: 李高文
建立时间: 20160818
**********************************************************************/
int keyValueGetInfo( char *pcaInfoPath )
{
  char pcLines[MAX_LINE_BUF];
	char pcKey[256];
  char *pcTmp;
  char *pcBlank;
  char *pcStart;
  char *pcEnd;
  FILE *fp;
  int  iLines;
  
  
  fp = fopen( pcaInfoPath, "rb+" );
  if (fp == NULL)
  {
    fprintf( stderr, "打开配置文件[%s]失败\n", pcaInfoPath );
    return __LINE__;
  }
  
  iLines = 0;
  
  /*循环从文件读取行数据，放入变量中*/
  while (1)
  {
    memset( pcLines, 0x00, sizeof(pcLines) );
    if (fgets( pcLines, sizeof(pcLines), fp ) == NULL)
    {
      break;
    }
    iLines ++;
    AllTrim( pcLines );

    /*如果是注释或者空行，continue*/
    if (strlen(pcLines) == 0)
    {
      continue;
    }
    
    if (pcLines[0] == '#')
    {
      continue;
    }
        
    pcStart = pcLines;
    pcEnd = pcLines + strlen(pcLines);
    
    /*搜索空格*/
    pcBlank = strchr( pcLines, ' ' );
    if (pcBlank == NULL)
    {
    	pcBlank = pcEnd;
    }
    
    /*取得关键字*/
    memset( pcKey, 0x00, sizeof(pcKey) );
    memcpy( pcKey, pcLines, pcBlank - pcLines );
    AllTrim( pcKey );
    
    if (!strcmpIgnore( pcKey, "USER" ))
    {
    	if (strlen( pcgUser ) != 0)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]USER已经存在，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]只有USER关键字，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*如果忽略空格后的数据不是等于号*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]配置文件[%s]USER后面缺少等于号，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart < pcEnd)
    	{
    		memcpy( pcgUser, pcStart, pcEnd - pcStart );
    	}
    }
    else
    if (!strcmpIgnore( pcKey, "PASS" ))
    {
    	if (strlen( pcgPass ) != 0)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]PASS已经存在，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]只有PASS关键字，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*如果忽略空格后的数据不是等于号*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]配置文件[%s]PASS后面缺少等于号，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart < pcEnd)
    	{
    		memcpy( pcgPass, pcStart, pcEnd - pcStart );
    	}
    }
    else
    if (!strcmpIgnore( pcKey, "MANAGER_LISTEN" ))
    {
    	if (strlen( pcgManagerListen ) != 0)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]MANAGER_LISTEN已经存在，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]只有MANAGER_LISTEN关键字，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*如果忽略空格后的数据不是等于号*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]配置文件[%s]MANAGER_LISTEN后面缺少等于号，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart < pcEnd)
    	{
    		memcpy( pcgManagerListen, pcStart, pcEnd - pcStart );
    	}
    	
    	if (strlen( pcgManagerListen ) != 0)
    	{
    		if (!checkPort( pcgManagerListen ))
    		{
    			fprintf( stderr, "[%d]配置文件[%s]MANAGER_LISTEN[%s]不符合格式，第[%d]行\n", __LINE__, pcaInfoPath, pcgShortListen, iLines );
          fclose(fp);
          return __LINE__;
    		}
    	}
    }
    else
    if (!strcmpIgnore( pcKey, "SHORT_LISTEN" ))
    {
    	if (strlen( pcgShortListen ) != 0)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]SHORT_LISTEN已经存在，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]只有SHORT_LISTEN关键字，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*如果忽略空格后的数据不是等于号*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]配置文件[%s]SHORT_LISTEN后面缺少等于号，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart < pcEnd)
    	{
    		memcpy( pcgShortListen, pcStart, pcEnd - pcStart );
    	}
    	
    	if (strlen( pcgShortListen ) != 0)
    	{
    		if (!checkPort( pcgShortListen ))
    		{
    			fprintf( stderr, "[%d]配置文件[%s]SHORT_LISTEN[%s]不符合格式，第[%d]行\n", __LINE__, pcaInfoPath, pcgShortListen, iLines );
          fclose(fp);
          return __LINE__;
    		}
    	}
    }
    else
    if (!strcmpIgnore( pcKey, "LONG_LISTEN" ))
    {
    	if (strlen( pcgLongListen ) != 0)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]LONG_LISTEN已经存在，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]只有LONG_LISTEN关键字，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*如果忽略空格后的数据不是等于号*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]配置文件[%s]LONG_LISTEN后面缺少等于号，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart < pcEnd)
    	{
    		memcpy( pcgLongListen, pcStart, pcEnd - pcStart );
    	}
    	
    	if (strlen( pcgLongListen ) != 0)
    	{
    		if (!checkPort( pcgLongListen ))
    		{
    			fprintf( stderr, "[%d]配置文件[%s]LONG_LISTEN不符合格式，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
          fclose(fp);
          return __LINE__;
    		}
    	}
    }
    else
    if (!strcmpIgnore( pcKey, "LOGPATH" ))
    {
    	if (strlen( pcgLogPath ) != 0)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]LOGPATH已经存在，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]只有LOGPATH关键字，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*如果忽略空格后的数据不是等于号*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]配置文件[%s]LOGPATH后面缺少等于号，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart < pcEnd)
    	{
    		memcpy( pcgLogPath, pcStart, pcEnd - pcStart );
    	}
    }
    else
    if (!strcmpIgnore( pcKey, "LOGNAME" ))
    {
    	if (strlen( pcgLogName ) != 0)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]LOGNAME已经存在，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]只有LOGNAME关键字，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*如果忽略空格后的数据不是等于号*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]配置文件[%s]LOGNAME后面缺少等于号，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart < pcEnd)
    	{
    		memcpy( pcgLogName, pcStart, pcEnd - pcStart );
    	}
    }
    else
    if (!strcmpIgnore( pcKey, "KEY_RAM_SIZE" ))
    {
    	if (strlen( pcgKeyRamSize ) != 0)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]KEY_RAM_SIZE已经存在，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]只有KEY_RAM_SIZE关键字，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*如果忽略空格后的数据不是等于号*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]配置文件[%s]KEY_RAM_SIZE后面缺少等于号，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart < pcEnd)
    	{
    		memcpy( pcgKeyRamSize, pcStart, pcEnd - pcStart );
    	}
    }
    else
    if (!strcmpIgnore( pcKey, "VALUE_RAM_SIZE" ))
    {
    	if (strlen( pcgValueRamSize ) != 0)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]VALUE_RAM_SIZE已经存在，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]只有VALUE_RAM_SIZE关键字，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*如果忽略空格后的数据不是等于号*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]配置文件[%s]VALUE_RAM_SIZE后面缺少等于号，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart < pcEnd)
    	{
    		memcpy( pcgValueRamSize, pcStart, pcEnd - pcStart );
    	}
    }
    else
    if (!strcmpIgnore( pcKey, "KEY_FILE_PATH" ))
    {
    	if (strlen( pcgKeyFilePath ) != 0)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]KEY_FILE_PATH已经存在，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]只有KEY_FILE_PATH关键字，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*如果忽略空格后的数据不是等于号*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]配置文件[%s]KEY_FILE_PATH后面缺少等于号，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart < pcEnd)
    	{
    		memcpy( pcgKeyFilePath, pcStart, pcEnd - pcStart );
    	}
    }
    else
    if (!strcmpIgnore( pcKey, "KEY_FILE_NAME" ))
    {
    	if (strlen( pcgKeyFileName ) != 0)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]KEY_FILE_NAME已经存在，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]只有KEY_FILE_NAME关键字，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*如果忽略空格后的数据不是等于号*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]配置文件[%s]KEY_FILE_NAME后面缺少等于号，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart < pcEnd)
    	{
    		memcpy( pcgKeyFileName, pcStart, pcEnd - pcStart );
    	}
    }
    else
    if (!strcmpIgnore( pcKey, "VALUE_FILE_PATH" ))
    {
    	if (strlen( pcgValueFilePath ) != 0)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]VALUE_FILE_PATH已经存在，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]只有VALUE_FILE_PATH关键字，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*如果忽略空格后的数据不是等于号*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]配置文件[%s]VALUE_FILE_PATH后面缺少等于号，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart < pcEnd)
    	{
    		memcpy( pcgValueFilePath, pcStart, pcEnd - pcStart );
    	}
    }
    else
    if (!strcmpIgnore( pcKey, "VALUE_FILE_NAME" ))
    {
    	if (strlen( pcgValueFileName ) != 0)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]VALUE_FILE_NAME已经存在，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]只有VALUE_FILE_NAME关键字，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*如果忽略空格后的数据不是等于号*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]配置文件[%s]VALUE_FILE_NAME后面缺少等于号，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart < pcEnd)
    	{
    		memcpy( pcgValueFileName, pcStart, pcEnd - pcStart );
    	}
    }
    else
    if (!strcmpIgnore( pcKey, "KEY_MAPPING_PATH" ))
    {
    	if (strlen( pcgKeyMappingPath ) != 0)
    	{
    		fprintf( stderr, "[%d]配置文件[%s] KEY_MAPPING_PATH 已经存在，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]只有 KEY_MAPPING_PATH 关键字，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*如果忽略空格后的数据不是等于号*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]配置文件[%s] KEY_MAPPING_PATH 后面缺少等于号，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart < pcEnd)
    	{
    		memcpy( pcgKeyMappingPath, pcStart, pcEnd - pcStart );
    	}
    }
    else
    if (!strcmpIgnore( pcKey, "KEY_MAPPING_NAME" ))
    {
    	if (strlen( pcgKeyMappingName ) != 0)
    	{
    		fprintf( stderr, "[%d]配置文件[%s] KEY_MAPPING_NAME 已经存在，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]只有 KEY_MAPPING_NAME 关键字，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*如果忽略空格后的数据不是等于号*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]配置文件[%s] KEY_MAPPING_NAME 后面缺少等于号，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart < pcEnd)
    	{
    		memcpy( pcgKeyMappingName, pcStart, pcEnd - pcStart );
    	}
    }
    else
    if (!strcmpIgnore( pcKey, "VALUE_MAPPING_PATH" ))
    {
    	if (strlen( pcgValueMappingPath ) != 0)
    	{
    		fprintf( stderr, "[%d]配置文件[%s] VALUE_MAPPING_PATH 已经存在，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]只有 VALUE_MAPPING_PATH 关键字，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*如果忽略空格后的数据不是等于号*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]配置文件[%s] VALUE_MAPPING_PATH 后面缺少等于号，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart < pcEnd)
    	{
    		memcpy( pcgValueMappingPath, pcStart, pcEnd - pcStart );
    	}
    }
    else
    if (!strcmpIgnore( pcKey, "VALUE_MAPPING_NAME" ))
    {
    	if (strlen( pcgValueMappingName ) != 0)
    	{
    		fprintf( stderr, "[%d]配置文件[%s] VALUE_MAPPING_NAME 已经存在，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]只有 VALUE_MAPPING_NAME 关键字，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*如果忽略空格后的数据不是等于号*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]配置文件[%s] VALUE_MAPPING_NAME 后面缺少等于号，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart < pcEnd)
    	{
    		memcpy( pcgValueMappingName, pcStart, pcEnd - pcStart );
    	}
    }
    else
    if (!strcmpIgnore( pcKey, "FILE_UPPER_SIZE" ))
    {
    	if (strlen( pcgFileUpperSize ) != 0)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]FILE_UPPER_SIZE已经存在，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]只有FILE_UPPER_SIZE关键字，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*如果忽略空格后的数据不是等于号*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]配置文件[%s]KEY_FILE_NAME后面缺少等于号，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart < pcEnd)
    	{
    		memcpy( pcgFileUpperSize, pcStart, pcEnd - pcStart );
    	}
    }
    else
    if (!strcmpIgnore( pcKey, "FILE_MAX_SIZE" ))
    {
    	if (strlen( pcgFileMaxSize ) != 0)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]FILE_MAX_SIZE已经存在，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]只有FILE_MAX_SIZE关键字，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*如果忽略空格后的数据不是等于号*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]配置文件[%s]FILE_MAX_SIZE后面缺少等于号，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart < pcEnd)
    	{
    		memcpy( pcgFileMaxSize, pcStart, pcEnd - pcStart );
    	}
    }
    else
    if (!strcmpIgnore( pcKey, "SELECT_TEMP_PATH" ))
    {
    	if (strlen( pcgSelectTempPath ) != 0)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]SELECT_TEMP_PATH已经存在，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]配置文件[%s]只有SELECT_TEMP_PATH关键字，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*如果忽略空格后的数据不是等于号*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]配置文件[%s]SELECT_TEMP_PATH后面缺少等于号，第[%d]行\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*忽略空格*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart < pcEnd)
    	{
    		memcpy( pcgSelectTempPath, pcStart, pcEnd - pcStart );
    	}
    }
    else
    {
    	fprintf( stderr, "[%d]配置文件[%s]的关键字不存在，第[%d]行[%s]\n", __LINE__, pcaInfoPath, iLines, pcLines );
      fclose(fp);
      return __LINE__;
    }
  }
  fclose(fp);
  
  if (strlen( pcgUser ) == 0)
  {
  	strcpy( pcgUser, DEFAULT_USER );
  }
  
  if (strlen( pcgPass ) == 0)
  {
  	strcpy( pcgPass, DEFAULT_PASS );
  }
  
  if (strlen( pcgManagerListen ) == 0)
  {
  	strcpy( pcgManagerListen, DEFAULT_MANAGER_LISTEN );
  }
  if (!checkPort( pcgManagerListen ))
  {
  	fprintf( stderr, "MANAGER_LISTEN[%s]不符合格式\n", pcgManagerListen );
  	return __LINE__;
  }
  igManagerListen = atoi( pcgManagerListen );
  
  if (strlen( pcgShortListen ) == 0)
  {
  	strcpy( pcgShortListen, DEFAULT_SHORT_LISTEN );
  }
  if (!checkPort( pcgShortListen ))
  {
  	fprintf( stderr, "SHORT_LISTEN[%s]不符合格式\n", pcgShortListen );
  	return __LINE__;
  }
  igShortListen = atoi( pcgShortListen );
  
  if (igShortListen == igManagerListen)
  {
  	fprintf( stderr, "SHORT_LISTEN[%s]和MANAGER_LISTEN[%s]不能相同\n", pcgShortListen, pcgManagerListen );
  	return __LINE__;
  }
  
  if (strlen( pcgLongListen ) == 0)
  {
  	strcpy( pcgLongListen, DEFAULT_LONG_LISTEN );
  }
  if (!checkPort( pcgLongListen ))
  {
  	fprintf( stderr, "LONG_LISTEN[%s]不符合格式\n", pcgLongListen );
  	return __LINE__;
  }
  igLongListen = atoi( pcgLongListen );
  
  if (igLongListen == igShortListen || igLongListen == igManagerListen)
  {
  	fprintf( stderr, "SHORT_LISTEN[%s]、LONG_LISTEN[%s]、MANAGER_LISTEN[%s]不能相同\n", pcgShortListen, pcgLongListen, pcgManagerListen );
  	return __LINE__;
  }
  
  if (strlen( pcgLogPath ) == 0)
  {
  	strcpy( pcgLogPath, DEFAULT_LOGPATH );
  }
  
  if (strlen( pcgLogName ) == 0)
  {
  	strcpy( pcgLogName, DEFAULT_LOGNAME );
  }
  
  
  if (strlen( pcgKeyRamSize ) == 0)
  {
  	strcpy( pcgKeyRamSize, DEFAULT_KEY_RAM_SIZE );
  }
  if (!isDigitals(pcgKeyRamSize, pcgKeyRamSize + strlen(pcgKeyRamSize)) )
  {
  	fprintf( stderr, "KEY_RAM_SIZE[%s]不符合格式\n", pcgKeyRamSize );
  	return __LINE__;
  }
  llgKeyRamSizeMain = atoll( pcgKeyRamSize );
  if (llgKeyRamSizeMain < 0)
  {
  	fprintf( stderr, "KEY_RAM_SIZE[%s]不符合格式\n", pcgKeyRamSize );
  	return __LINE__;
  }
  llgKeyRamSizeMain <<= 20;
  
  if (strlen( pcgValueRamSize ) == 0)
  {
  	strcpy( pcgValueRamSize, DEFAULT_VALUE_RAM_SIZE );
  }
  if (!isDigitals(pcgValueRamSize, pcgValueRamSize + strlen(pcgValueRamSize)) )
  {
  	fprintf( stderr, "VALUE_RAM_SIZE[%s]不符合格式\n", pcgValueRamSize );
  	return __LINE__;
  }
  llgValueRamSizeMain = atoll( pcgValueRamSize );
  if (llgValueRamSizeMain < 0)
  {
  	fprintf( stderr, "VALUE_RAM_SIZE[%s]不符合格式\n", pcgValueRamSize );
  	return __LINE__;
  }
  llgValueRamSizeMain <<= 20;
  
  if (strlen( pcgKeyFilePath ) == 0)
  {
  	strcpy( pcgKeyFilePath, DEFAULT_KEY_FILE_PATH );
  }
  
  if (strlen( pcgKeyFileName ) == 0)
  {
  	strcpy( pcgKeyFileName, DEFAULT_KEY_FILE_NAME );
  }
  
  if (strlen( pcgValueFilePath ) == 0)
  {
  	strcpy( pcgValueFilePath, DEFAULT_VALUE_FILE_PATH );
  }
  
  if (strlen( pcgValueFileName ) == 0)
  {
  	strcpy( pcgValueFileName, DEFAULT_VALUE_FILE_NAME );
  }
  
  if (strlen( pcgKeyMappingPath ) == 0)
  {
  	strcpy( pcgKeyMappingPath, DEFAULT_KEY_MAPPING_PATH );
  }
  
  if (strlen( pcgKeyMappingName ) == 0)
  {
  	strcpy( pcgKeyMappingName, DEFAULT_KEY_MAPPING_NAME );
  }
  
  if (strlen( pcgValueMappingPath ) == 0)
  {
  	strcpy( pcgValueMappingPath, DEFAULT_VALUE_MAPPING_PATH );
  }
  
  if (strlen( pcgValueMappingName ) == 0)
  {
  	strcpy( pcgValueMappingName, DEFAULT_VALUE_MAPPING_NAME );
  }
  
  if (strlen( pcgFileUpperSize ) == 0)
  {
  	strcpy( pcgFileUpperSize, DEFAULT_FILE_UPPER_SIZE );
  }
  if (!isDigitals(pcgFileUpperSize, pcgFileUpperSize + strlen(pcgFileUpperSize)) )
  {
  	fprintf( stderr, "FILE_UPPER_SIZE[%s]不符合格式\n", pcgFileUpperSize );
  	return __LINE__;
  }
  llgFileUpperSize = atoll( pcgFileUpperSize );
  if (llgFileUpperSize <= 0)
  {
  	fprintf( stderr, "FILE_UPPER_SIZE[%s]不符合格式\n", pcgFileUpperSize );
  	return __LINE__;
  }
  llgFileUpperSize <<= 20;
  
  if (strlen( pcgFileMaxSize ) == 0)
  {
  	strcpy( pcgFileMaxSize, DEFAULT_FILE_MAX_SIZE );
  }
  if (!isDigitals(pcgFileMaxSize, pcgFileMaxSize + strlen(pcgFileMaxSize)) )
  {
  	fprintf( stderr, "FILE_MAX_SIZE[%s]不符合格式\n", pcgFileMaxSize );
  	return __LINE__;
  }
  llgFileMaxSize = atoll( pcgFileMaxSize );
  if (llgFileMaxSize <= 0)
  {
  	fprintf( stderr, "FILE_MAX_SIZE[%s]不符合格式\n", pcgFileMaxSize );
  	return __LINE__;
  }
  llgFileMaxSize <<= 20;
  
  if (llgFileUpperSize > llgFileMaxSize)
  {
  	fprintf( stderr, "FILE_UPPER_SIZE[%s]应该比FILE_MAX_SIZE[%s]小\n", pcgFileUpperSize, pcgFileMaxSize );
  	return __LINE__;
  }
  
  if (strlen( pcgLogPath ) == 0)
  {
  	strcpy( pcgSelectTempPath, DEFAULT_SELECT_TEMP_PATH );
  }
  
#if 1
  fprintf( stdout, "USER             = %s \n", pcgUser );
  fprintf( stdout, "PASS             = %s \n", pcgPass );
  fprintf( stdout, "MANAGER_LISTEN   = %s \n", pcgManagerListen );
  fprintf( stdout, "SHORT_LISTEN     = %s \n", pcgShortListen );
  fprintf( stdout, "LONG_LISTEN      = %s \n", pcgLongListen );
  fprintf( stdout, "LOGPATH          = %s \n", pcgLogPath );
  fprintf( stdout, "LOGNAME          = %s \n", pcgLogName );
  fprintf( stdout, "KEY_RAM_SIZE     = %lld \n", llgKeyRamSizeMain );
  fprintf( stdout, "VALUE_RAM_SIZE   = %lld \n", llgValueRamSizeMain );
  fprintf( stdout, "KEY_FILE_PATH    = %s \n", pcgKeyFilePath );
  fprintf( stdout, "KEY_FILE_NAME    = %s \n", pcgKeyFileName );
  fprintf( stdout, "VALUE_FILE_PATH  = %s \n", pcgValueFilePath );
  fprintf( stdout, "VALUE_FILE_NAME  = %s \n", pcgValueFileName );
  fprintf( stdout, "KEY_MAPPING_PATH    = %s \n", pcgKeyMappingPath );
  fprintf( stdout, "KEY_MAPPING_NAME    = %s \n", pcgKeyMappingName );
  fprintf( stdout, "VALUE_MAPPING_PATH  = %s \n", pcgValueMappingPath );
  fprintf( stdout, "VALUE_MAPPING_NAME  = %s \n", pcgValueMappingName );
  fprintf( stdout, "FILE_UPPER_SIZE  = %lld \n", llgFileUpperSize );
  fprintf( stdout, "FILE_MAX_SIZE    = %lld \n", llgFileMaxSize );
  fprintf( stdout, "SELECT_TEMP_PATH = %s \n", pcgSelectTempPath );
  fflush( stdout );
#endif
  
  return SUCCESS;
}
