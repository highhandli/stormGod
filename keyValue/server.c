#ifndef __COMMON_HEAD_H
  #include "commonHead.h"
#endif

void serverExit();
void waitForThreadExit();

int main( int argc, char *argv[] )
{ 
  int iTmp;
  
  int iRet;
  
  //atexit( (void *)&writeRamToBuf2);
  
  if (argc < 2)
	{
		fprintf( stderr, "usage: %s info\n", argv[0] );
		return 0;
	}
	
	for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
	{
		//pthread_rwlock_init( &(rwlockMain[iTmp]), NULL );
		//rwlockMain[iTmp] = PTHREAD_RWLOCK_INITIALIZER;
		
		if(pthread_mutex_init(&(mutexMain[iTmp]),NULL) != 0 )  
    {  
      printf("Init metux error.");  
      exit(1);  
    }
	}
	
	for (iTmp = 0; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
	{
		if(pthread_mutex_init(&(mutexFile[iTmp]),NULL) != 0 )  
    {  
      printf("Init metux error.");  
      exit(1);  
    }
	}
  
  if(pthread_mutex_init(&srMutex,NULL) != 0 )  
  {  
    printf("Init metux error.");  
    exit(1);  
  }
	
	fprintf( stdout, "程序正在初始化，请耐心等待\n" );fflush(stdout);
	iRet = keyValueInit(argv[1]);
	if (iRet)
	{
		//fprintf( stdout, "%d\n", iRet );
		return 0;
	}
	
	#if 1
	/*用异步日志*/
	if (logInitAsy( 5, "server.log", 32 ))
	{
		return 0;
	}
	#endif
	fprintf( stdout, "程序启动完成\n" );fflush(stdout);
	
	igExitFlag = SERVER_RUN;
	
	signal( SIGABRT, &serverExit );
	
	while (igExitFlag == SERVER_RUN){THREAD_THREAD_SLEEP();}
	
	waitForThreadExit( );
	
	keyValueRelease( );
	
	/*释放异步日志*/
	logRlsAsy( );
	
	return 0;   
}

void serverExit()
{
	igExitFlag = SERVER_EXIT;
}

void waitForThreadExit()
{
	int  iIndex;
	int  iShortThread;
	int  iLongThread;
	int  iRet;
	
	while (igShortThreads > 0)
	{
		for (iIndex = 0; iIndex < igShortThreads; iIndex++)
		{
			if (sgShortThreadArray[iIndex].iRunState == THREAD_RUN_STATE_RUN
			&& sgShortThreadArray[iIndex].iAcceptState == ACCEPT_STATE_NO)
		  {
			#ifdef SYSTEM_WINDOWS_64
			  iRet = TerminateThread( sgShortThreadArray[iIndex - 1].threadHandle, 0 );
			  if (iRet == 0)
		    {
		    	logFmtSyn( LVNOR, "kill线程[%d]失败", sgShortThreadArray[iIndex - 1].threadHandle );
	        return;
		    }
			#else/*#ifdef SYSTEM_UBUNTU*/
			  /*kill线程*//*如果kill失败，直接返回失败*/
		    iRet = pthread_cancel( sgShortThreadArray[iIndex - 1].threadId );
		    if (iRet)
		    {
		    	logFmtSyn( LVNOR, "thread errno num: ESRCH[%d]", ESRCH );
		    	logFmtSyn( LVNOR, "kill线程[%d]失败[%d]", sgShortThreadArray[iIndex - 1].threadId, iRet );
	        return;
		    }
			#endif
			  sgShortThreadArray[iIndex - 1].iRunState = THREAD_RUN_STATE_STOP;
	    	igShortThreads--;
	    	
	    	if (igShortThreads == iShortThread)
	    	{
	  			break;
	  		}
		  }
		}
	}
	
	while (igLongThreads > 0)
	{
		for (iIndex = 0; iIndex < igLongThreads; iIndex++)
		{
			if (sgLongThreadArray[iIndex].iRunState == THREAD_RUN_STATE_RUN
			&& sgLongThreadArray[iIndex].iAcceptState == ACCEPT_STATE_NO)
		  {
			#ifdef SYSTEM_WINDOWS_64
			  iRet = TerminateThread( sgLongThreadArray[iIndex - 1].threadHandle, 0 );
			  if (iRet == 0)
		    {
		    	logFmtSyn( LVNOR, "kill线程[%d]失败", sgLongThreadArray[iIndex - 1].threadHandle );
	        return;
		    }
			#else/*#ifdef SYSTEM_UBUNTU*/
			  /*kill线程*//*如果kill失败，直接返回失败*/
		    iRet = pthread_cancel( sgLongThreadArray[iIndex - 1].threadId );
		    if (iRet)
		    {
		    	logFmtSyn( LVNOR, "thread errno num: ESRCH[%d]", ESRCH );
		    	logFmtSyn( LVNOR, "kill线程[%d]失败[%d]", sgLongThreadArray[iIndex - 1].threadId, iRet );
	        return;
		    }
			#endif
			  sgLongThreadArray[iIndex - 1].iRunState = THREAD_RUN_STATE_STOP;
	    	igLongThreads--;
	    	
	    	if (igLongThreads == iLongThread)
	    	{
	  			break;
	  		}
		  }
		}      
	}
}
