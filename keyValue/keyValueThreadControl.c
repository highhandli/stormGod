#define __KEY_VALUE_THREAD_CONTROL_C

/*
负责处理 MANAGER 语句
*/

#ifndef __COMMON_HEAD_H
  #include "commonHead.h"
#endif

void threadControl( );

/**********************************************************************
函数名称: threadControl
函数功能: 根据正在使用的线程数量来增加线程或者减少线程 
参    数：
第    一：SOCKET         I
作    者: 李高文
建立时间: 20180428
**********************************************************************/
void threadControl( )
{
	#ifdef SYSTEM_WINDOWS_64
	  unsigned long ulWindowsThreadId;
	#else/*#ifdef SYSTEM_UBUNTU*/
	  pthread_t iThreadId;
	#endif
	int  iIndex;
	int  iShortThread;
	int  iLongThread;
	int  iRet;

	while (sgThreadControl.iRunState == THREAD_RUN_STATE_RUN)
  {	
  	/*计算当前使用的短连接的线程数量*/
  	iShortThread = 0;
  	
  	for (iIndex = 0; iIndex < MAX_THREADS; iIndex++)
  	{
	  	if (sgShortThreadArray[iIndex].iRunState == THREAD_RUN_STATE_RUN
		  && sgShortThreadArray[iIndex].iAcceptState == ACCEPT_STATE_YES)
	  	{
	  		iShortThread++;
	  	}
	  }
	  
	  /*计算当前使用的长连接的线程数量*/
  	iLongThread = 0;
  	
  	for (iIndex = 0; iIndex < MAX_THREADS; iIndex++)
  	{
	  	if (sgLongThreadArray[iIndex].iRunState == THREAD_RUN_STATE_RUN
			&& sgLongThreadArray[iIndex].iAcceptState == ACCEPT_STATE_YES)
	  	{
	  		iLongThread++;
	  	}
	  }
	  
fprintf( stdout, "[%s][%d]shortRun[%d]longRun[%d]shortTot[%d]longTot[%d]\n", __FILE__, __LINE__, iShortThread, iLongThread, igShortThreads, igLongThreads );fflush(stdout);	
logFmtSyn( LVDET, "shortRun[%d]longRun[%d]shortTot[%d]longTot[%d", iShortThread, iLongThread, igShortThreads, igLongThreads );
    /*如果满足减少线程条件，减少线程*/
    if (igShortThreads > MIN_THREADS && (int)(iShortThread * 100 / igShortThreads) < FREE_THREAD_PERCENT)
    {
    	iShortThread =  (int)(iShortThread*100/NORMAL_THREAD_PERCENT);
fprintf( stdout, "[%s][%d]shortRun[%d]longRun[%d]shortTot[%d]longTot[%d]\n", __FILE__, __LINE__, iShortThread, iLongThread, igShortThreads, igLongThreads );fflush(stdout);	
logFmtSyn( LVDET, "shortRun[%d]longRun[%d]shortTot[%d]longTot[%d", iShortThread, iLongThread, igShortThreads, igLongThreads );
  		if (iShortThread < MIN_THREADS)
	    {
  	  	iShortThread = MIN_THREADS;
  	  }
  	  
  	  while (igShortThreads > iShortThread)
		  {
		  	for (iIndex = 0; iIndex < MAX_THREADS; iIndex++)
		  	{
		  		if (sgShortThreadArray[iIndex].iRunState == THREAD_RUN_STATE_RUN
					  && sgShortThreadArray[iIndex].iAcceptState == ACCEPT_STATE_NO)
          {
          	sgShortThreadArray[iIndex].iRunState = THREAD_RUN_STATE_STOP;
		  	    #ifdef SYSTEM_WINDOWS_64
				      iRet = TerminateThread( sgShortThreadArray[iIndex].threadHandle, 0 );
				      if (iRet == 0)
			        {
		            return;
			        }
				    #else/*#ifdef SYSTEM_UBUNTU*/
				      /*kill线程*//*如果kill失败，直接返回失败*/
			        iRet = pthread_cancel( sgShortThreadArray[iIndex].threadId );
			        if (iRet)
			        {
		            return;
			        }
				    #endif
		  	    igShortThreads--;
		  	    if (igShortThreads == iShortThread)
		      	{
	      			break;
	      		}
		  	  }
		  	}
		  }
    }
		else
	  /*如果满足增加线程条件，增加线程*/
	  if (igShortThreads < MAX_THREADS && (int)(iShortThread*100/igShortThreads) > BUSY_THREAD_PERCENT)
	  {
  		iShortThread =  (int)(iShortThread*100/NORMAL_THREAD_PERCENT);
  		if (iShortThread > MAX_THREADS)
	    {
  	  	iShortThread = MAX_THREADS;
  	  }
  	  
  	  while (igShortThreads < iShortThread)
		  {
  		  for (iIndex = 0; iIndex < MAX_THREADS; iIndex++)
		  	{
		  		if (sgShortThreadArray[iIndex].iRunState == THREAD_RUN_STATE_STOP)
          {
		  	    sgShortThreadArray[iIndex].iSocket = igShortSocket;
		        sgShortThreadArray[iIndex].iRunState = THREAD_RUN_STATE_RUN;
		        sgShortThreadArray[iIndex].iAcceptState = ACCEPT_STATE_NO;
		        
		  	    /*启动短连接线程函数处理*/
            #ifdef SYSTEM_WINDOWS_64
              sgShortThreadArray[iIndex].threadHandle = CreateThread( NULL, 0, (void *)(&shortSocket), &iIndex, 0, &ulWindowsThreadId );
              if (sgShortThreadArray[iIndex].threadHandle == NULL)
              {
		            return;
              }
              /*如果没有这步，参数的值传入不了*/
              THREAD_THREAD_SLEEP();
            #else/*#ifdef SYSTEM_UBUNTU*/
              if (pthread_create(&(sgShortThreadArray[iIndex].threadId), NULL, (void *)(&shortSocket), &iIndex) != 0)
              {
		            return;
              }
            
              /*线程分离，结束时自动回收资源*/
              pthread_detach(sgShortThreadArray[iIndex].threadId);
            #endif
            
            igShortThreads++;
		  	    if (igShortThreads == iShortThread)
		      	{
	      			break;
	      		}
		      }
		    }
		  }
  	}

    #if 1
    /*如果满足减少线程条件，减少线程*/
    if (igLongThreads > MIN_THREADS && (int)(iLongThread * 100 / igLongThreads) < FREE_THREAD_PERCENT)
    {
    	iLongThread =  (int)(iLongThread*100/NORMAL_THREAD_PERCENT);
fprintf( stdout, "[%s][%d]shortRun[%d]longRun[%d]shortTot[%d]longTot[%d]\n", __FILE__, __LINE__, iShortThread, iLongThread, igShortThreads, igLongThreads );fflush(stdout);	
logFmtSyn( LVDET, "shortRun[%d]longRun[%d]shortTot[%d]longTot[%d]", iShortThread, iLongThread, igShortThreads, igLongThreads );
  		if (iLongThread < MIN_THREADS)
	    {
  	  	iLongThread = MIN_THREADS;
  	  }
  	  
  	  while (igLongThreads > iLongThread)
		  {
		  	for (iIndex = 0; iIndex < MAX_THREADS; iIndex++)
		  	{
		  		if (sgLongThreadArray[iIndex].iRunState == THREAD_RUN_STATE_RUN
					  && sgLongThreadArray[iIndex].iAcceptState == ACCEPT_STATE_NO)
          { 
            sgLongThreadArray[iIndex].iRunState = THREAD_RUN_STATE_STOP;
		  	    #ifdef SYSTEM_WINDOWS_64
				      iRet = TerminateThread( sgLongThreadArray[iIndex].threadHandle, 0 );
				      if (iRet == 0)
			        {
		            return;
			        }

				    #else/*#ifdef SYSTEM_UBUNTU*/
				      /*kill线程*//*如果kill失败，直接返回失败*/
			        iRet = pthread_cancel( sgLongThreadArray[iIndex].threadId );
			        if (iRet)
			        {
		            return;
			        }
				    #endif
		  	    igLongThreads--;
		  	    if (igLongThreads == iLongThread)
		      	{
	      			break;
	      		}
fprintf( stdout, "[%s][%d]destLong[%d]longTot[%d]\n", __FILE__, __LINE__, iLongThread, igLongThreads );fflush(stdout);	
logFmtSyn( LVDET, "destLong[%d]longTot[%d]", iLongThread, igLongThreads );
		  	  }
		  	}
		  }
fprintf( stdout, "[%s][%d]destLong[%d]longTot[%d]\n", __FILE__, __LINE__, iLongThread, igLongThreads );fflush(stdout);	
logFmtSyn( LVDET, "destLong[%d]longTot[%d]", iLongThread, igLongThreads );
    }
    else
    #endif
	  /*如果满足增加线程条件，增加线程*/
	  if (igLongThreads < MAX_THREADS && (int)(iLongThread*100/igLongThreads) > BUSY_THREAD_PERCENT)
	  {
  		iLongThread =  (int)(iLongThread*100/NORMAL_THREAD_PERCENT);
fprintf( stdout, "[%s][%d]shortRun[%d]longRun[%d]shortTot[%d]longTot[%d]\n", __FILE__, __LINE__, iShortThread, iLongThread, igShortThreads, igLongThreads );fflush(stdout);	
logFmtSyn( LVDET, "shortRun[%d]longRun[%d]shortTot[%d]longTot[%d", iShortThread, iLongThread, igShortThreads, igLongThreads );
  		if (iLongThread > MAX_THREADS)
	    {
  	  	iLongThread = MAX_THREADS;
  	  }
  	  
  	  while (igLongThreads < iLongThread)
		  {
		  	for (iIndex = 0; iIndex < MAX_THREADS; iIndex++)
		  	{
		  		if (sgLongThreadArray[iIndex].iRunState == THREAD_RUN_STATE_STOP)
          {
		  	    sgLongThreadArray[iIndex].iSocket = igLongSocket;
		        sgLongThreadArray[iIndex].iRunState = THREAD_RUN_STATE_RUN;
		        sgLongThreadArray[iIndex].iAcceptState = ACCEPT_STATE_NO;
		        
		  	    /*启动短连接线程函数处理*/
            #ifdef SYSTEM_WINDOWS_64
              sgLongThreadArray[iIndex].threadHandle = CreateThread( NULL, 0, (void *)(&longSocket), &iIndex, 0, &ulWindowsThreadId );
              if (sgLongThreadArray[iIndex].threadHandle == NULL)
              {
		            return;
              }
              /*如果没有这步，参数的值传入不了*/
              THREAD_THREAD_SLEEP();
            #else/*#ifdef SYSTEM_UBUNTU*/
              if (pthread_create(&(sgLongThreadArray[iIndex].threadId), NULL, (void *)(&longSocket), &iIndex) != 0)
              {
		            return;
              }
            
              /*线程分离，结束时自动回收资源*/
              pthread_detach(sgLongThreadArray[iIndex].threadId);
            #endif
            
            igLongThreads++;
            
		  	    if (igLongThreads == iLongThread)
		      	{
	      			break;
	      		}
		  	  }
		  	}
		  }
  	}
//fprintf( stdout, "[%s][%d][%d][%d]\n", __FILE__, __LINE__, iShortThread, iLongThread );fflush(stdout); 	  
	  THREAD_THREAD_SLEEP();
  }
}
