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
��������: keyValueInti 
��������: KEY VALUE ��ʼ�� 
��    ����
��    һ��������Ϣ�ļ� 
��    ��: �����
����ʱ��: 20160818
**********************************************************************/
int keyValueInit( char *pcaInfoPath )
{
  int iRet;
  #ifdef SYSTEM_WINDOWS_64
	  WSADATA wsaData;
	#endif
	
	#ifdef SYSTEM_WINDOWS_64
	  /*����Windows Sockets DLL*/
    if (WSAStartup(MAKEWORD(2,1),&wsaData)) 
    { 
      fprintf( stderr, "Winsock�޷���ʼ��!\n" );
      WSACleanup();
      return 0;
    }
  #endif
  
  /*��ʼ����ѯID������Ϊ0����Ϊ��ѯIDΪ0��ʾ�յ�ID���ͻ��˽��в�ѯʱ��0����*/
  llgSelectId = 1;
  
  /*��ʼ��ȫ�ֱ���*/
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
  
  /*��ʼ�������ݵ����б���*/
  memset( &sgGlobalVariableMain, 0x00, sizeof(sgGlobalVariableMain) );
  
  /*���������ļ���Ϣ*/
  iRet = keyValueGetInfo( pcaInfoPath );
  if (iRet)
  {
    fprintf( stderr, "����������Ϣ�ļ�[%s]ʧ��\n", pcaInfoPath );
    return __LINE__;
  }
  
  fprintf( stdout, "����������Ϣ��ɣ�\n" );fflush(stdout);
  
  /*����ȫ�ֻ�����*/
  iRet = createOverallBufAndStartThreads();
  if (iRet)
  {
  	#ifdef SYSTEM_WINDOWS_64
	    WSACleanup();
	  #endif
    return iRet;
  }
  
  fprintf( stdout, "����ȫ�ֻ��������\n" );fflush(stdout);
  
  /*��� KEY �� VALUE �ļ��������ԣ������������Ϣ*/
  iRet = checkAndLoadKeyValueFile();
  if (iRet)
  {
  	fprintf( stdout, "��� KEY �� VALUE �ļ��������ԣ����������ʧ�ܣ�%d��\n", iRet );fflush(stdout);
    keyValueRelease();
    exit(0);
  }
  
  fprintf( stdout, "���������ļ��ʹ����߳���ɣ�\n" );fflush(stdout);
  
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
��������: keyValueRelease
��������: KEY VALUE �����˳�ǰ�ͷ���Դ 
��    ����
��    һ��������Ϣ�ļ� 
��    ��: �����
����ʱ��: 20160818
**********************************************************************/
void keyValueRelease( )
{
	/*���ڴ��е���������д�뵽Ӳ����*/
	/*��ʽӦ��ʱ������Ҫ����*/
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
  
  /*���� KEY�� VALUE �ļ�*/
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
  /*���ڴ��е�����д�뵽Ӳ��*/
  rewind( fpgMappingKey );
  rewind( fpgMappingValue );
	fwrite( &sgGlobalVariableMain, 1, sizeof(sgGlobalVariableMain), fpgMappingKey );
    
  /*д���ļ�����*/
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
��������: createOverallBufAndStartThreads 
��������: ����ȫ�ֻ��������Լ������߳�
��    ����
��    ��: �����
����ʱ��: 20160818
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
      fprintf( stderr, "���ܴ������ KEY ӳ���ļ�[%s]\n", pcFilePath );
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
      fprintf( stderr, "���ܴ������ VALUE ӳ���ļ�[%s]\n", pcFilePath );
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
  	fprintf( stderr, " KEY ӳ���ļ� �� VALUE ӳ���ļ�����\n" );
    return __LINE__;
  }
  
  /*���� KEY ������*/
	llKey = (__int64)(llgKeyRamSizeMain/INDEPENDENT_SIZE);
	llKey = ((llKey%BLOCK_SIZE) == 0 ? (__int64)(llKey/BLOCK_SIZE):(__int64)(llKey/BLOCK_SIZE) + 1) * BLOCK_SIZE;
	
	/*���� VALUE ������*/
  llValue = (__int64)(llgValueRamSizeMain/INDEPENDENT_SIZE);
  llValue = ((llValue%BLOCK_SIZE) == 0 ? (__int64)(llValue/BLOCK_SIZE):(__int64)(llValue/BLOCK_SIZE) + 1) * BLOCK_SIZE;
  
  /*��������½��ļ�*/
  if (iKeyFileFlag == 1 && iValueFileFlag == 1)
  {
  	for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
	  {
      sgGlobalVariableMain[iTmp].pcgKeyBufMain = malloc( llKey );
      if (sgGlobalVariableMain[iTmp].pcgKeyBufMain == NULL)
      {
        fprintf( stderr, "���� KEY ��������С[%lld]ʧ��\n", llKey );
        return __LINE__;
      }
      memset( sgGlobalVariableMain[iTmp].pcgKeyBufMain, 0x00, llKey );
      sgGlobalVariableMain[iTmp].llgKeyRamSizeMain = llKey;
      *((__int64 *)(sgGlobalVariableMain[iTmp].pcgKeyFreeMain + BLOCK_SIZE - sizeof(__int64))) = NO_BLOCK;
      
      sgGlobalVariableMain[iTmp].pcgValueBufMain = malloc( llValue );
      if (sgGlobalVariableMain[iTmp].pcgValueBufMain == NULL)
      {
        fprintf( stderr, "���� KEY ��������С[%lld]ʧ��\n", llValue );
        return __LINE__;
      }
      memset( sgGlobalVariableMain[iTmp].pcgValueBufMain, 0x00, llValue );
      sgGlobalVariableMain[iTmp].llgValueRamSizeMain = llValue;
      *((__int64 *)(sgGlobalVariableMain[iTmp].pcgValueFreeMain + BLOCK_SIZE - sizeof(__int64))) = NO_BLOCK;
    }
  }
  /*����µ��ڴ滺�����Ƿ���ļ�С*/
  else
  {
  	if (fread( &sgGlobalVariableMain, 1, sizeof(sgGlobalVariableMain), fpgMappingKey ) != sizeof(sgGlobalVariableMain))
  	{
  		fprintf( stderr, " KEY ӳ���ļ� �� VALUE ӳ���ļ�����\n" );
  		return __LINE__;
  	}
  	
  	/*���*/
  	for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
	  {
      if (sgGlobalVariableMain[iTmp].llgKeyRamOffsetMain > llKey )
      {
      	fprintf( stderr, "ӳ���ļ������õ��ڴ��С��ƥ�䣬�����������ڴ��С\n" );
        return __LINE__;
      }
      if (sgGlobalVariableMain[iTmp].llgValueRamOffsetMain > llValue )
      {
      	fprintf( stderr, "ӳ���ļ������õ��ڴ��С��ƥ�䣬�����������ڴ��С\n" );
        return __LINE__;
      }
    }
    
    /*�����ļ�����*/
    for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
	  {
	  	sgGlobalVariableMain[iTmp].pcgKeyBufMain = malloc( llKey );
      if (sgGlobalVariableMain[iTmp].pcgKeyBufMain == NULL)
      {
        fprintf( stderr, "���� KEY ��������С[%lld]ʧ��\n", llKey );
        return __LINE__;
      }
      memset( sgGlobalVariableMain[iTmp].pcgKeyBufMain, 0x00, llKey );
      sgGlobalVariableMain[iTmp].llgKeyRamSizeMain = llKey;
      
      sgGlobalVariableMain[iTmp].pcgValueBufMain = malloc( llValue );
      if (sgGlobalVariableMain[iTmp].pcgValueBufMain == NULL)
      {
        fprintf( stderr, "���� KEY ��������С[%lld]ʧ��\n", llValue );
        return __LINE__;
      }
      memset( sgGlobalVariableMain[iTmp].pcgValueBufMain, 0x00, llValue );
      sgGlobalVariableMain[iTmp].llgValueRamSizeMain = llValue;
      
	  	if (fread( sgGlobalVariableMain[iTmp].pcgKeyBufMain, 1, sgGlobalVariableMain[iTmp].llgKeyRamOffsetMain, fpgMappingKey ) != sgGlobalVariableMain[iTmp].llgKeyRamOffsetMain)
	  	{
	  		fprintf( stderr, " KEY ӳ���ļ� �� VALUE ӳ���ļ�����\n" );
  		  return __LINE__;
	  	}
      if (fread( sgGlobalVariableMain[iTmp].pcgValueBufMain, 1, sgGlobalVariableMain[iTmp].llgValueRamOffsetMain, fpgMappingValue ) != sgGlobalVariableMain[iTmp].llgValueRamOffsetMain)
	  	{
	  		fprintf( stderr, " KEY ӳ���ļ� �� VALUE ӳ���ļ�����\n" );
  		  return __LINE__;
	  	}
    }
  }

  iManagerSocket = createServerSocket( igManagerListen );
	if (iShortSocket == -1)
	{
		fprintf( stderr, "��������������ʧ�ܣ� PORT[%d]\n", igManagerListen );
    return __LINE__;
	}
  
  iShortSocket = createServerSocket( igShortListen );
	if (iShortSocket == -1)
	{
		fprintf( stderr, "��������������ʧ�ܣ� PORT[%d]\n", igShortListen );
    return __LINE__;
	}
	igShortSocket = iShortSocket;
	
	iLongSocket = createServerSocket( igLongListen );
	if (iLongSocket == -1)
	{
		fprintf( stderr, "��������������ʧ�ܣ� PORT[%d]\n", igLongListen );
    return __LINE__;
	}
	igLongSocket = iLongSocket;
	
	memset( &sgManagerThread, 0x00, sizeof(sgManagerThread) );
	memset( &sgShortThreadArray, 0x00, sizeof(sgShortThreadArray) );
	memset( &sgLongThreadArray, 0x00, sizeof(sgLongThreadArray) );
  
  /*���������̺߳�������*/
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
  
    /*�̷߳��룬����ʱ�Զ�������Դ*/
    pthread_detach(sgManagerThread.threadId);
  #endif
  
	for (iTmp = 0; iTmp < MIN_THREADS; iTmp++)
	{
		sgShortThreadArray[iTmp].iSocket = igShortSocket;
		sgShortThreadArray[iTmp].iRunState = THREAD_RUN_STATE_RUN;
		sgShortThreadArray[iTmp].iAcceptState = ACCEPT_STATE_NO;
		
    /*�����������̺߳�������*/
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
    
      /*�̷߳��룬����ʱ�Զ�������Դ*/
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
		
    /*�����������̺߳�������*/
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
    
      /*�̷߳��룬����ʱ�Զ�������Դ*/
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
	
	/*�������Ƴ̺�������*/
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
  
    /*�̷߳��룬����ʱ�Զ�������Դ*/
    pthread_detach(sgThreadControl.threadId);
  #endif
    
  return SUCCESS;
}

/**********************************************************************
��������: checkAndLoadKeyValueFile 
��������: ��� KEY �� VALUE �ļ��������ԣ������������Ϣ
��    ����
��    ��: �����
����ʱ��: 20160818
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
        fprintf( stderr, "���ܴ������ KEY �ļ�[%s]\n", pcFilePath );
        return __LINE__;
      }
    }
    /*��ȡ KEY �ļ���С*/
    fseeko64( fpgKey[iTmp], 0, SEEK_END );
    llgKeyFileSize[iTmp] = ftello64( fpgKey[iTmp] );
    llgKeyFileDisc[iTmp] = llgKeyFileSize[iTmp] >> BLOCK_LEFT_MOVE;
    rewind( fpgKey[iTmp] );
    
    /*�Ƿ񳬳��ļ����ֵ*/
    if (llgKeyFileSize[iTmp] > llgFileMaxSize)
    {
      fprintf( stderr, " KEY �ļ� �������ֵ\n" );
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
        fprintf( stderr, "���ܴ������ VALUE �ļ�[%s]\n", pcFilePath );
        return __LINE__;
      }
    }
    /*��ȡ VALUE �ļ���С*/
    fseeko64( fpgValue[iTmp], 0, SEEK_END );
    llgValueFileSize[iTmp] = ftello64( fpgValue[iTmp] );
    rewind( fpgValue[iTmp] );
    
    /*�Ƿ񳬳��ļ����ֵ*/
    if (llgValueFileSize[iTmp] > llgFileMaxSize)
    {
      fprintf( stderr, " VALUE �ļ� �������ֵ\n" );
      return __LINE__;
    }
    
    /* KEY �ļ���С�Ƿ������ı�����0��3���顢�������ֱ��Ӧ ��һ�δ�����ֻ������δʹ�á�����*/
    
    //fprintf( stdout, "%s %lld %lld\n", pcFilePath, llgKeyFileSize[iTmp], llgValueFileSize[iTmp] );
    
    /*�����һ���ļ�Ϊ�գ�����һ���ļ���Ϊ�գ���ʾ�ļ���*/
    if ((llgKeyFileSize[iTmp] == 0 && llgValueFileSize[iTmp] != 0) || (llgKeyFileSize[iTmp] != 0 && llgValueFileSize[iTmp] == 0))
    {
      fprintf( stderr, " KEY �ļ� �� VALUE �ļ�����\n" );
      return __LINE__;
    }
    
    /*������ǳ��δ���*/
    if (llgKeyFileSize[iTmp] == 0 && llgValueFileSize[iTmp] == 0)
    {
      /*���ô���ʱ��*/
      getDateTime( sgBase[iTmp].pcCreateDateTime, (sgBase[iTmp].pcCreateDateTime) + DATE_LEN );
      
      /*��������ʱ��*/
      getDateTime( sgBase[iTmp].pcStartDateTime, (sgBase[iTmp].pcStartDateTime) + DATE_LEN );
      
      /*���õ�ǰ KEY �ļ�ʹ��ƫ��������λ��BLOCK_SIZE*/
      sgBase[iTmp].llKeyFileOffset = 3;
      
      /*���õ�ǰ KEY �ļ��е� KEY ��һ�����ݿ��ַ����λ��BLOCK_SIZE*/
      sgBase[iTmp].llFirstDisc = 0;
      
      /*���õ�ǰ KEY �ļ��е� KEY �ĵ�ǰ����*/
      sgBase[iTmp].llLoops = 0;
      
      /*�� KEY �ļ�д�������Ϣ*/
      memset( pcData, 0x00, sizeof(pcData) );
      memcpy( pcData, &(sgBase[iTmp]), sizeof(KEY_VALUE_BASE_INFO) );
      fwrite( pcData, 1, sizeof(pcData), fpgKey[iTmp] );
      
      /*д�� KEY �ļ����м�¼��*/
      *((__int64 *)(pcgKeyFree[iTmp] + BLOCK_SIZE - sizeof(__int64))) = NO_BLOCK;
      fwrite( pcgKeyFree[iTmp], 1, BLOCK_SIZE, fpgKey[iTmp] );  
      
      /*д�� VALUE �ļ����м�¼��*/
      *((__int64 *)(pcgValueFree[iTmp] + BLOCK_SIZE - sizeof(__int64))) = NO_BLOCK;
      fwrite( pcgValueFree[iTmp], 1, BLOCK_SIZE, fpgKey[iTmp] );  
      
      /*���� KEY �ļ���СΪ����ֵ*/
      fseeko64( fpgKey[iTmp], llgFileUpperSize - 1, SEEK_SET );
      c = 0x00;
      fwrite( &c, 1, sizeof(c), fpgKey[iTmp] );
      
      llgKeyFileSize[iTmp] = llgFileUpperSize;
      llgKeyFileDisc[iTmp] = llgKeyFileSize[iTmp] >> BLOCK_LEFT_MOVE;
      
      /*���� VALUE �ļ���СΪ����ֵ*/
      fseeko64( fpgValue[iTmp], llgFileUpperSize - 1, SEEK_SET );
      c = 0x00;
      fwrite( &c, 1, sizeof(c), fpgValue[iTmp] );
      
      llgValueFileSize[iTmp] = llgFileUpperSize;
      
    }
    /*���ǳ��δ���*/
    else
    {
      /*������������ı���������С��2���飬���У�������Ϣ1����2���ͷż�¼��*/
      if ((llgKeyFileSize[iTmp]%BLOCK_SIZE) != 0  || llgKeyFileSize[iTmp]>>BLOCK_LEFT_MOVE < 3)
      {
        fprintf( stderr, " KEY �ļ� �� VALUE �ļ�����\n" );
        return __LINE__;
      }
      
      /*�ӻ�����Ϣ���ȡ KEY �� VALUE �ļ�ʹ��ƫ����*/
      memset( pcData, 0x00, sizeof(pcData) );
      fread( pcData, 1, sizeof(pcData), fpgKey[iTmp] );
      
      memset( &sgBase[iTmp], 0x00, sizeof(KEY_VALUE_BASE_INFO) );
      memcpy( &sgBase[iTmp], pcData, sizeof(KEY_VALUE_BASE_INFO) );
          
      /*���ƫ�����Ƿ�����ļ���С*/
      if (((sgBase[iTmp].llKeyFileOffset) << BLOCK_LEFT_MOVE) > llgKeyFileSize[iTmp] || sgBase[iTmp].llValueFileOffset > llgValueFileSize[iTmp])
      {
        fprintf( stderr, " KEY �ļ� �� VALUE �ļ�����\n" );
        return __LINE__;
      }
      
      /*��ȡ KEY �ļ����м�¼��*/
      fread( pcgKeyFree[iTmp], 1, BLOCK_SIZE, fpgKey[iTmp] ); 
      
      /*��ȡ VALUE �ļ����м�¼��*/
      fread( pcgValueFree[iTmp], 1, BLOCK_SIZE, fpgKey[iTmp] ); 
      
      /*��������ʱ��*/
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
��������: keyValueGetInfo
��������: ���������ļ���Ϣ
��    ����
��    һ��������Ϣ�ļ� 
��    ��: �����
����ʱ��: 20160818
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
    fprintf( stderr, "�������ļ�[%s]ʧ��\n", pcaInfoPath );
    return __LINE__;
  }
  
  iLines = 0;
  
  /*ѭ�����ļ���ȡ�����ݣ����������*/
  while (1)
  {
    memset( pcLines, 0x00, sizeof(pcLines) );
    if (fgets( pcLines, sizeof(pcLines), fp ) == NULL)
    {
      break;
    }
    iLines ++;
    AllTrim( pcLines );

    /*�����ע�ͻ��߿��У�continue*/
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
    
    /*�����ո�*/
    pcBlank = strchr( pcLines, ' ' );
    if (pcBlank == NULL)
    {
    	pcBlank = pcEnd;
    }
    
    /*ȡ�ùؼ���*/
    memset( pcKey, 0x00, sizeof(pcKey) );
    memcpy( pcKey, pcLines, pcBlank - pcLines );
    AllTrim( pcKey );
    
    if (!strcmpIgnore( pcKey, "USER" ))
    {
    	if (strlen( pcgUser ) != 0)
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]USER�Ѿ����ڣ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*���Կո�*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]ֻ��USER�ؼ��֣���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*������Կո������ݲ��ǵ��ں�*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]USER����ȱ�ٵ��ںţ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*���Կո�*/
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
    		fprintf( stderr, "[%d]�����ļ�[%s]PASS�Ѿ����ڣ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*���Կո�*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]ֻ��PASS�ؼ��֣���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*������Կո������ݲ��ǵ��ں�*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]PASS����ȱ�ٵ��ںţ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*���Կո�*/
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
    		fprintf( stderr, "[%d]�����ļ�[%s]MANAGER_LISTEN�Ѿ����ڣ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*���Կո�*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]ֻ��MANAGER_LISTEN�ؼ��֣���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*������Կո������ݲ��ǵ��ں�*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]MANAGER_LISTEN����ȱ�ٵ��ںţ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*���Կո�*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart < pcEnd)
    	{
    		memcpy( pcgManagerListen, pcStart, pcEnd - pcStart );
    	}
    	
    	if (strlen( pcgManagerListen ) != 0)
    	{
    		if (!checkPort( pcgManagerListen ))
    		{
    			fprintf( stderr, "[%d]�����ļ�[%s]MANAGER_LISTEN[%s]�����ϸ�ʽ����[%d]��\n", __LINE__, pcaInfoPath, pcgShortListen, iLines );
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
    		fprintf( stderr, "[%d]�����ļ�[%s]SHORT_LISTEN�Ѿ����ڣ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*���Կո�*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]ֻ��SHORT_LISTEN�ؼ��֣���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*������Կո������ݲ��ǵ��ں�*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]SHORT_LISTEN����ȱ�ٵ��ںţ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*���Կո�*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart < pcEnd)
    	{
    		memcpy( pcgShortListen, pcStart, pcEnd - pcStart );
    	}
    	
    	if (strlen( pcgShortListen ) != 0)
    	{
    		if (!checkPort( pcgShortListen ))
    		{
    			fprintf( stderr, "[%d]�����ļ�[%s]SHORT_LISTEN[%s]�����ϸ�ʽ����[%d]��\n", __LINE__, pcaInfoPath, pcgShortListen, iLines );
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
    		fprintf( stderr, "[%d]�����ļ�[%s]LONG_LISTEN�Ѿ����ڣ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*���Կո�*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]ֻ��LONG_LISTEN�ؼ��֣���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*������Կո������ݲ��ǵ��ں�*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]LONG_LISTEN����ȱ�ٵ��ںţ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*���Կո�*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart < pcEnd)
    	{
    		memcpy( pcgLongListen, pcStart, pcEnd - pcStart );
    	}
    	
    	if (strlen( pcgLongListen ) != 0)
    	{
    		if (!checkPort( pcgLongListen ))
    		{
    			fprintf( stderr, "[%d]�����ļ�[%s]LONG_LISTEN�����ϸ�ʽ����[%d]��\n", __LINE__, pcaInfoPath, iLines );
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
    		fprintf( stderr, "[%d]�����ļ�[%s]LOGPATH�Ѿ����ڣ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*���Կո�*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]ֻ��LOGPATH�ؼ��֣���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*������Կո������ݲ��ǵ��ں�*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]LOGPATH����ȱ�ٵ��ںţ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*���Կո�*/
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
    		fprintf( stderr, "[%d]�����ļ�[%s]LOGNAME�Ѿ����ڣ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*���Կո�*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]ֻ��LOGNAME�ؼ��֣���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*������Կո������ݲ��ǵ��ں�*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]LOGNAME����ȱ�ٵ��ںţ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*���Կո�*/
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
    		fprintf( stderr, "[%d]�����ļ�[%s]KEY_RAM_SIZE�Ѿ����ڣ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*���Կո�*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]ֻ��KEY_RAM_SIZE�ؼ��֣���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*������Կո������ݲ��ǵ��ں�*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]KEY_RAM_SIZE����ȱ�ٵ��ںţ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*���Կո�*/
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
    		fprintf( stderr, "[%d]�����ļ�[%s]VALUE_RAM_SIZE�Ѿ����ڣ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*���Կո�*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]ֻ��VALUE_RAM_SIZE�ؼ��֣���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*������Կո������ݲ��ǵ��ں�*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]VALUE_RAM_SIZE����ȱ�ٵ��ںţ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*���Կո�*/
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
    		fprintf( stderr, "[%d]�����ļ�[%s]KEY_FILE_PATH�Ѿ����ڣ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*���Կո�*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]ֻ��KEY_FILE_PATH�ؼ��֣���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*������Կո������ݲ��ǵ��ں�*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]KEY_FILE_PATH����ȱ�ٵ��ںţ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*���Կո�*/
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
    		fprintf( stderr, "[%d]�����ļ�[%s]KEY_FILE_NAME�Ѿ����ڣ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*���Կո�*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]ֻ��KEY_FILE_NAME�ؼ��֣���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*������Կո������ݲ��ǵ��ں�*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]KEY_FILE_NAME����ȱ�ٵ��ںţ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*���Կո�*/
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
    		fprintf( stderr, "[%d]�����ļ�[%s]VALUE_FILE_PATH�Ѿ����ڣ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*���Կո�*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]ֻ��VALUE_FILE_PATH�ؼ��֣���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*������Կո������ݲ��ǵ��ں�*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]VALUE_FILE_PATH����ȱ�ٵ��ںţ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*���Կո�*/
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
    		fprintf( stderr, "[%d]�����ļ�[%s]VALUE_FILE_NAME�Ѿ����ڣ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*���Կո�*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]ֻ��VALUE_FILE_NAME�ؼ��֣���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*������Կո������ݲ��ǵ��ں�*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]VALUE_FILE_NAME����ȱ�ٵ��ںţ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*���Կո�*/
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
    		fprintf( stderr, "[%d]�����ļ�[%s] KEY_MAPPING_PATH �Ѿ����ڣ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*���Կո�*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]ֻ�� KEY_MAPPING_PATH �ؼ��֣���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*������Կո������ݲ��ǵ��ں�*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s] KEY_MAPPING_PATH ����ȱ�ٵ��ںţ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*���Կո�*/
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
    		fprintf( stderr, "[%d]�����ļ�[%s] KEY_MAPPING_NAME �Ѿ����ڣ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*���Կո�*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]ֻ�� KEY_MAPPING_NAME �ؼ��֣���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*������Կո������ݲ��ǵ��ں�*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s] KEY_MAPPING_NAME ����ȱ�ٵ��ںţ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*���Կո�*/
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
    		fprintf( stderr, "[%d]�����ļ�[%s] VALUE_MAPPING_PATH �Ѿ����ڣ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*���Կո�*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]ֻ�� VALUE_MAPPING_PATH �ؼ��֣���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*������Կո������ݲ��ǵ��ں�*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s] VALUE_MAPPING_PATH ����ȱ�ٵ��ںţ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*���Կո�*/
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
    		fprintf( stderr, "[%d]�����ļ�[%s] VALUE_MAPPING_NAME �Ѿ����ڣ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*���Կո�*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]ֻ�� VALUE_MAPPING_NAME �ؼ��֣���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*������Կո������ݲ��ǵ��ں�*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s] VALUE_MAPPING_NAME ����ȱ�ٵ��ںţ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*���Կո�*/
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
    		fprintf( stderr, "[%d]�����ļ�[%s]FILE_UPPER_SIZE�Ѿ����ڣ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*���Կո�*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]ֻ��FILE_UPPER_SIZE�ؼ��֣���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*������Կո������ݲ��ǵ��ں�*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]KEY_FILE_NAME����ȱ�ٵ��ںţ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*���Կո�*/
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
    		fprintf( stderr, "[%d]�����ļ�[%s]FILE_MAX_SIZE�Ѿ����ڣ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*���Կո�*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]ֻ��FILE_MAX_SIZE�ؼ��֣���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*������Կո������ݲ��ǵ��ں�*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]FILE_MAX_SIZE����ȱ�ٵ��ںţ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*���Կո�*/
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
    		fprintf( stderr, "[%d]�����ļ�[%s]SELECT_TEMP_PATH�Ѿ����ڣ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart = pcBlank;
    	
    	/*���Կո�*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart == pcEnd)
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]ֻ��SELECT_TEMP_PATH�ؼ��֣���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	/*������Կո������ݲ��ǵ��ں�*/
    	if (*pcStart != '=')
    	{
    		fprintf( stderr, "[%d]�����ļ�[%s]SELECT_TEMP_PATH����ȱ�ٵ��ںţ���[%d]��\n", __LINE__, pcaInfoPath, iLines );
        fclose(fp);
        return __LINE__;
    	}
    	
    	pcStart++;
    	
    	/*���Կո�*/
    	IGNORE_BLANK_START( pcStart, pcEnd )
    	
    	if (pcStart < pcEnd)
    	{
    		memcpy( pcgSelectTempPath, pcStart, pcEnd - pcStart );
    	}
    }
    else
    {
    	fprintf( stderr, "[%d]�����ļ�[%s]�Ĺؼ��ֲ����ڣ���[%d]��[%s]\n", __LINE__, pcaInfoPath, iLines, pcLines );
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
  	fprintf( stderr, "MANAGER_LISTEN[%s]�����ϸ�ʽ\n", pcgManagerListen );
  	return __LINE__;
  }
  igManagerListen = atoi( pcgManagerListen );
  
  if (strlen( pcgShortListen ) == 0)
  {
  	strcpy( pcgShortListen, DEFAULT_SHORT_LISTEN );
  }
  if (!checkPort( pcgShortListen ))
  {
  	fprintf( stderr, "SHORT_LISTEN[%s]�����ϸ�ʽ\n", pcgShortListen );
  	return __LINE__;
  }
  igShortListen = atoi( pcgShortListen );
  
  if (igShortListen == igManagerListen)
  {
  	fprintf( stderr, "SHORT_LISTEN[%s]��MANAGER_LISTEN[%s]������ͬ\n", pcgShortListen, pcgManagerListen );
  	return __LINE__;
  }
  
  if (strlen( pcgLongListen ) == 0)
  {
  	strcpy( pcgLongListen, DEFAULT_LONG_LISTEN );
  }
  if (!checkPort( pcgLongListen ))
  {
  	fprintf( stderr, "LONG_LISTEN[%s]�����ϸ�ʽ\n", pcgLongListen );
  	return __LINE__;
  }
  igLongListen = atoi( pcgLongListen );
  
  if (igLongListen == igShortListen || igLongListen == igManagerListen)
  {
  	fprintf( stderr, "SHORT_LISTEN[%s]��LONG_LISTEN[%s]��MANAGER_LISTEN[%s]������ͬ\n", pcgShortListen, pcgLongListen, pcgManagerListen );
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
  	fprintf( stderr, "KEY_RAM_SIZE[%s]�����ϸ�ʽ\n", pcgKeyRamSize );
  	return __LINE__;
  }
  llgKeyRamSizeMain = atoll( pcgKeyRamSize );
  if (llgKeyRamSizeMain < 0)
  {
  	fprintf( stderr, "KEY_RAM_SIZE[%s]�����ϸ�ʽ\n", pcgKeyRamSize );
  	return __LINE__;
  }
  llgKeyRamSizeMain <<= 20;
  
  if (strlen( pcgValueRamSize ) == 0)
  {
  	strcpy( pcgValueRamSize, DEFAULT_VALUE_RAM_SIZE );
  }
  if (!isDigitals(pcgValueRamSize, pcgValueRamSize + strlen(pcgValueRamSize)) )
  {
  	fprintf( stderr, "VALUE_RAM_SIZE[%s]�����ϸ�ʽ\n", pcgValueRamSize );
  	return __LINE__;
  }
  llgValueRamSizeMain = atoll( pcgValueRamSize );
  if (llgValueRamSizeMain < 0)
  {
  	fprintf( stderr, "VALUE_RAM_SIZE[%s]�����ϸ�ʽ\n", pcgValueRamSize );
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
  	fprintf( stderr, "FILE_UPPER_SIZE[%s]�����ϸ�ʽ\n", pcgFileUpperSize );
  	return __LINE__;
  }
  llgFileUpperSize = atoll( pcgFileUpperSize );
  if (llgFileUpperSize <= 0)
  {
  	fprintf( stderr, "FILE_UPPER_SIZE[%s]�����ϸ�ʽ\n", pcgFileUpperSize );
  	return __LINE__;
  }
  llgFileUpperSize <<= 20;
  
  if (strlen( pcgFileMaxSize ) == 0)
  {
  	strcpy( pcgFileMaxSize, DEFAULT_FILE_MAX_SIZE );
  }
  if (!isDigitals(pcgFileMaxSize, pcgFileMaxSize + strlen(pcgFileMaxSize)) )
  {
  	fprintf( stderr, "FILE_MAX_SIZE[%s]�����ϸ�ʽ\n", pcgFileMaxSize );
  	return __LINE__;
  }
  llgFileMaxSize = atoll( pcgFileMaxSize );
  if (llgFileMaxSize <= 0)
  {
  	fprintf( stderr, "FILE_MAX_SIZE[%s]�����ϸ�ʽ\n", pcgFileMaxSize );
  	return __LINE__;
  }
  llgFileMaxSize <<= 20;
  
  if (llgFileUpperSize > llgFileMaxSize)
  {
  	fprintf( stderr, "FILE_UPPER_SIZE[%s]Ӧ�ñ�FILE_MAX_SIZE[%s]С\n", pcgFileUpperSize, pcgFileMaxSize );
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
