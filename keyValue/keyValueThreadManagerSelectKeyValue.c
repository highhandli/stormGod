#define __KEY_VALUE_THREAD_MANAGER_SELECT_KEY_VALUE_C

/*
负责处理 MANAGER 中的 SELECT 语句
*/

#ifndef __COMMON_HEAD_H
  #include "commonHead.h"
#endif

int  managerPositionLeftOpen( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId );
int  managerPositionLeftClose( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId );
int  managerPositionRightOpen( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId );
int  managerPositionRightClose( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId );
int  managerPositionLeftOpenMain( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId );
int  managerPositionLeftCloseMain( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId );
int  managerPositionRightOpenMain( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId );
int  managerPositionRightCloseMain( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId );

void managerSelectKeyValueDeal( int iaSocket, char *pcaOperate, char *pcaBufStart, char *pcaBufEnd );
void managerSelectKeyValueDealFreeSelectId( __int64 llaSelectId );
void managerSelectKeyValueDealFreeSelectIdData( __int64 llaSelectId );
void managerSelectKeyValueDealFreeResultBuf( __int64 llaSelectId, int iaSize );
void managerSelectKeyValueDealChildThread( SELECT_SHARE *selectShare );
int  managerSelectKeyValueDealScanfValue( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId );
int  managerSelectKeyValueDealResult( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, int iaStartOffset[], int iaEndOffset[], __int64 iaAddr, __int64 iaTotalLoops, int iaIndepentFileId );
int  managerSelectKeyValueDealGetDataLeft( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId );
int  managerSelectKeyValueDealGetDataOther( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId );
int  managerSelectKeyValueDealGetDataRight( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex,  __int64 iaAddr, __int64 iaSize, int iaEndOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId );
int  managerSelectKeyValueInsertKeyValuePoint( __int64 llaSelectId, int iaKeyLen, char *pcaKey, int iaValueLen, char *pcaValue );

int  managerSelectKeyValueDealScanfValueMain( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId );
int  managerSelectKeyValueDealResultMain( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, int iaStartOffset[], int iaEndOffset[], __int64 iaAddr, __int64 iaTotalLoops, int iaIndepentId );
int  managerSelectKeyValueDealGetDataLeftMain( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId );
int  managerSelectKeyValueDealGetDataOtherMain( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId );
int  managerSelectKeyValueDealGetDataRightMain( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex,  __int64 iaAddr, __int64 iaSize, int iaEndOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId );

#define SELECT_MALLOC( buf, selectid, size ) \
{\
  /*检查是否有足够的查询缓冲空间，如果不足，释放一部分*//*在查询分配内存前*/\
  if (llgResultBufOffset + size > MAX_SELECT_BUF)\
  {\
    managerSelectKeyValueDealFreeResultBuf( selectid, size );\
  }\
  buf = malloc(size);\
  if (buf != NULL)\
  {\
    memset( buf, 0x00, size );\
    llgResultBufOffset += size;\
  }\
}

#define SELECT_FREE( buf, size ) \
{\
  free( buf );\
  buf = NULL;\
  llgResultBufOffset -= size;\
}


/**********************************************************************
函数名称: managerSelectKeyValueDeal
函数功能: 管理端处理 select key, value 操作
参    数：
第    一：SOCKET               I
第    二：数据开始地址         I
第    三：数据结束地址         I
作    者: 李高文
建立时间: 20161022
**********************************************************************/
void managerSelectKeyValueDeal( int iaSocket, char *pcaOperate, char *pcaBufStart, char *pcaBufEnd )
{
/*
接收数据结构：长度+操作+获取数量+获取开始位置+值区间（左开右开、左开右闭、左闭右开、左闭右闭）+KEY开始值长度+KEY开始值+KEY结束值长度+KEY结束值
返回数据结构：操作结果+总数量+当前返回数量+(KEY、VALUE的长度及值)
接收数据结构：长度+获取数量+获取开始位置
返回数据结构：操作结果+总数量+当前返回数量+(KEY、VALUE的长度及值)
*/
#define MANAGER_SELECT_KEY_VALUE_DEAL_SEND_ERROR( value ) \
{\
	memset( pcResult, 0x00, sizeof(pcResult) );\
	sprintf( pcResult, "%d", value ); \
	cResult = pcResult[0];\
	\
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );\
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, 1 );\
  \
  send( iaSocket, pcSendLen, DATA_BUF_LEN,0 );\
	send( iaSocket, &cResult, 1, 0 );\
	return;\
}
  char pcSysDate[DATE_LEN + 1];
	char pcSysTime[TIME_LEN + 1];
	char pcSysDateTime[DATE_TIME + 1];
	char pcResult[INT_LEN + 1];
	char cResult;
	char *pcSendBuf;
	char pcRows[INT64_LEN + 1];
	char pcTotalRows[INT64_LEN + 1];
	char pcReturnRows[INT64_LEN + 1];
  char pcRowStartIndex[INT64_LEN + 1];
	char pcKeyValueStartLen[KEY_VALUE_LENGTH + 1];
  char pcKeyValueEndLen[KEY_VALUE_LENGTH + 1];
  char pcKeyValueStart[MAX_VALUE_LEN + 1];
  char pcKeyValueEnd[MAX_VALUE_LEN + 1];
  char pcValueType[KEY_VALUE_LENGTH + 1];
  char pcKeyValueLen[KEY_VALUE_LENGTH + 1];
  char pcSendLen[DATA_BUF_LEN + 1];
  char pcRecvLen[DATA_BUF_LEN + 1];
  char pcSelectResultPath[PATH_LEN];
  char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iKeyValueLen;
  int  iKeyValueStartLen;
	int  iKeyValueEndLen;
	int  iKeyValueFlag;                     /*获取KEY或者VALUE或者KEY VALUE*/
	__int64 llStartIndex;
	__int64 llStartIndexTmp;
	__int64 llSelectRows;
	__int64 llReturnRows;
	__int64 llTmp;
	__int64 llTmp2;
	int  iRet;
	char *pcBufStart;
	char *pcBufEnd;
	SELECT_SHARE selectStruct;
	char pcOperate[OPERATE_LEN + 1];
	char pcSelectId[INT64_LEN+1];
	int iRecvLen;
	int iTotalLen;
	int iDataLen;
	int iSendLen;
	int iTmp;
	int iIndex;
	int iTime;
	int iKeyLen;
	int iValueLen;
	__int64 llSelectId;
	SELECT_RESULT_BUF *sResultBufTmp;
	SELECT_KEY_VALUE_RESULT *skvr;
	SELECT_KEY_VALUE_RESULT *skvrTmp;
	FILE *fp;
	
	
	#ifdef SYSTEM_WINDOWS_64
	  void *threadHandle;                /*windows下的线程句柄*/
	  unsigned long ulWindowsThreadId;
	#else/*#ifdef SYSTEM_UBUNTU*/
	  pthread_t threadId;                /*其他OS下的线程ID*/
	#endif
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
	/*获取系统的时间*/
	getDateTime( pcSysDate, pcSysTime );
	
	memset( pcSysDateTime, 0x00, sizeof(pcSysDateTime) );
	sprintf( pcSysDateTime, "%s%s", pcSysDate, pcSysTime );
	
	/*获取查询ID*/
	memset( pcSelectId, 0x00, sizeof(pcSelectId) );
	memcpy( pcSelectId, pcBufStart, INT64_LEN );
	pcBufStart += INT64_LEN;
	
	/*获取请求数量*//*值为0时，表示清除查询ID对应的缓冲区*/
	memset( pcRows, 0x00, sizeof(pcRows) );
	memcpy( pcRows, pcBufStart, INT64_LEN );
	pcBufStart += INT64_LEN;
	llSelectRows = atoll( pcRows );
	
	/*如果值为0*//*清理缓冲区即可*/
	if (llSelectRows == 0)
	{
		llSelectId = atoll( pcSelectId );
		
		MANAGER_SELECT_LOCK();
		{
		  managerSelectKeyValueDealFreeSelectId( llSelectId );
		}
		MANAGER_SELECT_UNLOCK();
		
		/*返回结果*/
		MANAGER_SELECT_KEY_VALUE_DEAL_SEND_ERROR( SUCCESS );
		return;
	}
	
	/*如果是初始值的查询ID*/
	if (!strcmp( pcSelectId, NULL_SELECT_ID ))
  {
  	/*获取开始位置*/
	  memset( pcRowStartIndex, 0x00, sizeof(pcRowStartIndex) );
	  memcpy( pcRowStartIndex, pcBufStart, INT64_LEN );
	  pcBufStart += INT64_LEN;
	  llStartIndex = atoll( pcRowStartIndex );
	  llStartIndexTmp = llStartIndex;
	  
	  /*取值区间*/
	  memset( pcValueType, 0x00, sizeof(pcValueType) );
	  memcpy( pcValueType, pcBufStart, KEY_VALUE_LENGTH );
	  pcBufStart += KEY_VALUE_LENGTH;
	  
	  /*取KEY或VALUE开始值长度*/
	  memset( pcKeyValueStartLen, 0x00, sizeof(pcKeyValueStartLen) );
	  memcpy( pcKeyValueStartLen, pcBufStart, KEY_VALUE_LENGTH );
	  pcBufStart += KEY_VALUE_LENGTH;
	  
	  /*取KEY或VALUE开始值*/
	  iKeyValueStartLen = atoi( pcKeyValueStartLen );
	  memset( pcKeyValueStart, 0x00, sizeof(pcKeyValueStart) );
	  memcpy( pcKeyValueStart, pcBufStart, iKeyValueStartLen );
	  pcBufStart += iKeyValueStartLen;
	  
	  /*取KEY或VALUE结束值长度*/
	  memset( pcKeyValueEndLen, 0x00, sizeof(pcKeyValueEndLen) );
	  memcpy( pcKeyValueEndLen, pcBufStart, KEY_VALUE_LENGTH );
	  pcBufStart += KEY_VALUE_LENGTH;
	  
	  /*取KEY或VALUE结束值*/
	  iKeyValueEndLen = atoi( pcKeyValueEndLen );
	  memset( pcKeyValueEnd, 0x00, sizeof(pcKeyValueEnd) );
	  memcpy( pcKeyValueEnd, pcBufStart, iKeyValueEndLen );
	  pcBufStart += iKeyValueEndLen;
	  
	  if (!strcmp( pcaOperate, MANAGER_SELECT_KEY_CNT ))
    {
      iKeyValueFlag = SELECT_KEY;
    }
    else
    if (!strcmp( pcaOperate, MANAGER_SELECT_VALUE_CNT ))
    {
      iKeyValueFlag = SELECT_VALUE;
    }
    else
    /*if (!strcmp( pcaOperate, MANAGER_SELECT_KEY_VALUE_CNT ))*/
    {
      iKeyValueFlag = SELECT_KEY_VALUE;
    }
    
    /*取得当前查询ID*/
    MANAGER_SELECT_LOCK();
      llSelectId = llgSelectId;
    	llgSelectId++;
  	MANAGER_SELECT_UNLOCK();
  	
  	/*将查询条件放入到缓冲区*/
  	MANAGER_SELECT_LOCK();
  	{
  	  if (sgResultBuf == NULL)
  	  {
  	    SELECT_MALLOC( sgResultBuf, llSelectId, sizeof(SELECT_RESULT_BUF) );
  	  	if (sgResultBuf == NULL)
  	  	{
  	  		MANAGER_SELECT_KEY_VALUE_DEAL_SEND_ERROR( ERROR );
  	  	}
  	  	
  	  	memcpy( sgResultBuf->pcDateTime, pcSysDateTime, DATE_TIME );                /*上一次使用查询ID的时间*/
  	  	sgResultBuf->iKeyValueFlag = iKeyValueFlag;                                 /*获取KEY或者VALUE或者KEY VALUE*/
	      memcpy( sgResultBuf->pcValueType, pcValueType, KEY_VALUE_LENGTH );          /*搜索条件的值区间（KEY或VALUE 左开右开、左开右闭、左闭右开、左闭右闭*/
	      memcpy( sgResultBuf->pcKeyValueStart, pcKeyValueStart, iKeyValueStartLen ); /*KEY VALUE的开始值*/
	      sgResultBuf->iKeyValueStartLen = iKeyValueStartLen;                         /*KEY VALUE 的开始值长度*/
	      memcpy( sgResultBuf->pcKeyValueEnd, pcKeyValueEnd, iKeyValueEndLen );       /*KEY VALUE 的结束值*/
	      sgResultBuf->iKeyValueEndLen = iKeyValueEndLen;                             /*KEY VALUE 的结束值长度*/
	      
	      sgResultBuf->llSelectId = llSelectId;                                       /*查询ID*/
  	  }
  	  else
  	  {
  	  	sResultBufTmp = sgResultBuf;
  	  	
  	  	while (sResultBufTmp ->next != NULL)
  	  	{
  	  		sResultBufTmp = sResultBufTmp->next;
  	  	}
  	  	
  	  	SELECT_MALLOC( sResultBufTmp->next, llSelectId, sizeof(SELECT_RESULT_BUF) );
  	  	if (sResultBufTmp->next == NULL)
  	  	{
  	  		MANAGER_SELECT_KEY_VALUE_DEAL_SEND_ERROR( ERROR );
  	  	}
  	  	
  	  	memcpy( sResultBufTmp->next->pcDateTime, pcSysDateTime, DATE_TIME );                /*上一次使用查询ID的时间*/
  	  	sResultBufTmp->next->iKeyValueFlag = iKeyValueFlag;                                 /*获取KEY或者VALUE或者KEY VALUE*/
	      memcpy( sResultBufTmp->next->pcValueType, pcValueType, KEY_VALUE_LENGTH );          /*搜索条件的值区间（KEY或VALUE 左开右开、左开右闭、左闭右开、左闭右闭*/
	      memcpy( sResultBufTmp->next->pcKeyValueStart, pcKeyValueStart, iKeyValueStartLen ); /*KEY VALUE的开始值*/
	      sResultBufTmp->next->iKeyValueStartLen = iKeyValueStartLen;                         /*KEY VALUE 的开始值长度*/
	      memcpy( sResultBufTmp->next->pcKeyValueEnd, pcKeyValueEnd, iKeyValueEndLen );       /*KEY VALUE 的结束值*/
	      sResultBufTmp->next->iKeyValueEndLen = iKeyValueEndLen;                             /*KEY VALUE 的结束值长度*/
	      
	      sResultBufTmp->next->llSelectId = llSelectId;                                       /*查询ID*/
	      sResultBufTmp->next->llStartIndex = llStartIndex;                                   /*当前结点中的查询结果的位置偏移量*/
  	  }  	  
    }
    MANAGER_SELECT_UNLOCK();
  	
  	/*设置与子线程共享数据*/
	  memset( &selectStruct, 0x00, sizeof(selectStruct) );
	  selectStruct.llSelectId = llSelectId;                                       /*查询ID*/
	  selectStruct.iState = RUN_STATE_UNFINISHED;                                 /*子线程运行状态*/
	  selectStruct.iCmd = RUN_CMD_RUN;                                            /*子线程运行命令*/
  	
  	/*执行搜索*/
  	MANAGER_RDLOCK();
  	{
	    #ifdef SYSTEM_WINDOWS_64
        threadHandle = CreateThread( NULL, 0, (void *)(&managerSelectKeyValueDealChildThread), &selectStruct, 0, &ulWindowsThreadId );
        if (sgManagerThread.threadHandle == NULL)
        {
        	MANAGER_SELECT_KEY_VALUE_DEAL_SEND_ERROR( ERROR );
        	MANAGER_WRUNLOCK();
          return;
        }
      #else/*#ifdef SYSTEM_UBUNTU*/
        if (pthread_create(&threadId, NULL, (void *)(&managerSelectKeyValueDealChildThread), &selectStruct) != 0)
        {
      	  MANAGER_SELECT_KEY_VALUE_DEAL_SEND_ERROR( ERROR );
      	  MANAGER_WRUNLOCK();
          return;
        }
      
        /*线程分离，结束时自动回收资源*/
        pthread_detach(threadId);
      #endif
      
      /*循环等待输入或者检查线程执行情况*/
      iTime = 0;
	    while (iTime < EACH_JOB_TIME * 10) /*EACH_JOB_TIME 的单位是秒，每次睡眠0.1秒*/
	    {		
	    	/*如果子线程已完成，表示子线程正常执行完成，并退出*/
	    	if (selectStruct.iState == RUN_STATE_FINISHED)
	    	{
	    		logFmtSyn( LVNOR, "selectStruct.iState[%d]", selectStruct.iState );
	    		break;
	    	}
	    	else
	    	{
	    		usleep( 100*1000 );
	    	}
	    	iTime++;
	    }
	    
	    if (selectStruct.iState != RUN_STATE_FINISHED)
      {
      	logFmtSyn( LVNOR, "给子线程发送终止命令" );
	      /*给子线程发送终止命令*/
	      selectStruct.iCmd = RUN_CMD_EXIT;
	      
	      /*等待子线程执行结束*/
	      while (selectStruct.iState != RUN_STATE_FINISHED)
	      {
	      	THREAD_THREAD_SLEEP();
	      }
      }
  	}
  	MANAGER_RDUNLOCK();
  	  	
  	/*设置查询ID*/
  	memset( pcSelectId, 0x00, sizeof(pcSelectId) );
  	sprintf( pcSelectId, "%0*lld", INT64_LEN, llSelectId );
  }
  /*查询ID的继续执行*/
  else
  {
  	/*获取开始位置*/
	  memset( pcRowStartIndex, 0x00, sizeof(pcRowStartIndex) );
	  memcpy( pcRowStartIndex, pcBufStart, INT64_LEN );
	  pcBufStart += INT64_LEN;
	  llStartIndex = atoll( pcRowStartIndex );
	  llStartIndexTmp = llStartIndex;
	  
	  /*更新查询ID使用时间、当前结点中的查询结果的位置偏移量*/
  	llSelectId = atoll( pcSelectId );
  	sResultBufTmp = sgResultBuf;
  	
  	MANAGER_SELECT_LOCK();
  	logFmtSyn( LVNOR, "sResultBufTmp[%lld]", sResultBufTmp );

  	  while (sResultBufTmp != NULL)
  	  {
  	  	if (sResultBufTmp->llSelectId == llSelectId)
  	  	{
  	  		memcpy( sResultBufTmp->pcDateTime, pcSysDateTime, DATE_TIME );                /*上一次使用查询ID的时间*/
  	  		break;
  	  	}
  	  	else
  	  	{
  	  		sResultBufTmp = sResultBufTmp->next;
  	  	}
  	  }
  	MANAGER_SELECT_UNLOCK();
  	
  	/*如果结点被清理*/
  	if (sResultBufTmp == NULL)
  	{
  	  MANAGER_SELECT_KEY_VALUE_DEAL_SEND_ERROR( ERROR );
  	}
  	
  	/*是否有足够的数据*//*缓冲区中的数据位置小于待取数据的位置，并且有足够的数据*/
  	if ((sResultBufTmp->llStartIndex <= llStartIndex) && (sResultBufTmp->llStartIndex + sResultBufTmp->llRows >= llStartIndex + llSelectRows))
  	{
  	  /*有足够的数据，不需要处理，直接跳到下面从缓冲区执行取数据*/
  	}
  	/*如果不满足条件，执行查询*/
  	else
  	{
  		logFmtSyn( LVNOR, "sResultBufTmp->llStartIndex[%lld]sResultBufTmp->llRows[%lld]llStartIndex[%lld]llSelectRows[%lld]", sResultBufTmp->llStartIndex, sResultBufTmp->llRows, llStartIndex, llSelectRows );
  		
  	  /*释放缓冲区的数据*/
  	  managerSelectKeyValueDealFreeSelectIdData( llSelectId );
  	  
  	  /*设置当前结点中的查询结果的位置偏移量和当前数量*/
  	  sResultBufTmp->llStartIndex = llStartIndex;
  	  sResultBufTmp->llRows = 0;
  	  
  	  /*设置与子线程共享数据*/
	    memset( &selectStruct, 0x00, sizeof(selectStruct) );
	    selectStruct.llSelectId = llSelectId;                                       /*查询ID*/
	    selectStruct.iState = RUN_STATE_UNFINISHED;                                 /*子线程运行状态*/
	    selectStruct.iCmd = RUN_CMD_RUN;                                            /*子线程运行命令*/
	    
	    /*执行搜索*/
	    MANAGER_RDLOCK();
  	  {
	      #ifdef SYSTEM_WINDOWS_64
          threadHandle = CreateThread( NULL, 0, (void *)(&managerSelectKeyValueDealChildThread), &selectStruct, 0, &ulWindowsThreadId );
          if (sgManagerThread.threadHandle == NULL)
          {
          	MANAGER_SELECT_KEY_VALUE_DEAL_SEND_ERROR( ERROR );
          	MANAGER_WRUNLOCK();
            return;
          }
        #else/*#ifdef SYSTEM_UBUNTU*/
          if (pthread_create(&threadId, NULL, (void *)(&managerSelectKeyValueDealChildThread), &selectStruct) != 0)
          {
        	  MANAGER_SELECT_KEY_VALUE_DEAL_SEND_ERROR( ERROR );
        	  MANAGER_WRUNLOCK();
            return;
          }
        
          /*线程分离，结束时自动回收资源*/
          pthread_detach(threadId);
        #endif
        
        /*循环等待输入或者检查线程执行情况*/
        iTime = 0;
	      while (iTime < EACH_JOB_TIME * 10) /*EACH_JOB_TIME 的单位是秒，每次睡眠0.1秒*/
	      {		
	      	/*如果子线程已完成，表示子线程正常执行完成，并退出*/
	      	if (selectStruct.iState == RUN_STATE_FINISHED)
	      	{
	      		logFmtSyn( LVNOR, "selectStruct.iState[%d]", selectStruct.iState );
	      		break;
	      	}
	      	else
	      	{
	      		usleep( 100*1000 );
	      	}
	      	iTime++;
	      }
	      
	      if (selectStruct.iState != RUN_STATE_FINISHED)
        {
        	logFmtSyn( LVNOR, "给子线程发送终止命令" );
	        /*给子线程发送终止命令*/
	        selectStruct.iCmd = RUN_CMD_EXIT;
	        
	        /*等待子线程执行结束*/
	        while (selectStruct.iState != RUN_STATE_FINISHED)
	        {
	        	THREAD_THREAD_SLEEP();
	        }
        }
  	  }
  	  MANAGER_RDUNLOCK();
  	}
  }
  
  logFmtSyn( LVNOR, "开始从缓冲区获取数据" );
  
  MANAGER_SELECT_LOCK();
  {
    /*从缓冲区搜索数据*/
    sResultBufTmp = sgResultBuf;
    	
    while (sResultBufTmp != NULL)
    {
    	if (sResultBufTmp->llSelectId == llSelectId)
    	{
    		memcpy( sResultBufTmp->pcDateTime, pcSysDateTime, DATE_TIME );                /*上一次使用查询ID的时间*/
    		break;
    	}
    	else
    	{
    		sResultBufTmp = sResultBufTmp->next;
    	}
    }
    
    /*如果结点被清理*/
  	if (sResultBufTmp == NULL)
  	{
  	  MANAGER_SELECT_KEY_VALUE_DEAL_SEND_ERROR( ERROR );
  	  MANAGER_SELECT_UNLOCK();
  	  return;
  	}
  	logFmtSyn( LVNOR, "sResultBufTmp->llStartIndex[%lld]sResultBufTmp->llRows[%lld]llStartIndex[%lld]", sResultBufTmp->llStartIndex, sResultBufTmp->llRows, llStartIndex );
    /*如果满足条件，返回*/
    if ((sResultBufTmp->llStartIndex <= llStartIndex) && (sResultBufTmp->llStartIndex + sResultBufTmp->llRows > llStartIndex))
  	{
  		/*如果数据被释放*//*从文件读取数据*/
    	if (sResultBufTmp->skvr == NULL)
    	{
    		/*组查询结果缓冲文件路径*/
	      memset( pcSelectResultPath, 0x00, sizeof(pcSelectResultPath) );
	      
	      #ifdef SYSTEM_WINDOWS_64
	        if (strlen(pcgSelectTempPath) == 0)
          {
            sprintf( pcSelectResultPath, "\"%lld\"", llSelectId );
          }
          else
          {
          	sprintf( pcSelectResultPath, "\"%s\\%lld\"", pcgSelectTempPath, llSelectId );
          }
	      #else
	        if (strlen(pcgSelectTempPath) == 0)
          {
            sprintf( pcSelectResultPath, "\"%lld\"", llSelectId );
          }
          else
          {
          	sprintf( pcSelectResultPath, "%s/%lld", pcgSelectTempPath, llSelectId );
          }
	      #endif
	      
	      /*打开文件*/
	      fp = fopen( pcSelectResultPath, "rb" );
	      if (fp == NULL)
	      {
	      	MANAGER_SELECT_KEY_VALUE_DEAL_SEND_ERROR( ERROR );
  	      MANAGER_SELECT_UNLOCK();
  	      return;
	      }
	      
	      while (1)
	      {
	      	memset( pcKeyValueLen, 0x00, sizeof(pcKeyValueLen) );
          fread( pcKeyValueLen, 1, KEY_VALUE_LENGTH, fp );
          
          iKeyValueLen = atoi(pcKeyValueLen);
          
          if (iKeyValueLen == 0)
          {
          	break;
          }
            
	      	if (sResultBufTmp->iKeyValueFlag == SELECT_KEY)
          {
            fread( pcKey, 1, iKeyValueLen, fp );
            pcKey[iKeyValueLen] = 0x00;
            
            managerSelectKeyValueInsertKeyValuePoint( llSelectId, iKeyValueLen, pcKey, 0, NULL );
          }
          else
          if (sResultBufTmp->iKeyValueFlag == SELECT_VALUE)
          {
            fread( pcValue, 1, iKeyValueLen, fp );
            pcValue[iKeyValueLen] = 0x00;
            
            managerSelectKeyValueInsertKeyValuePoint( llSelectId, 0, NULL, iKeyValueLen, pcValue );
          }
          else
          /*if (sResultBufTmp->iKeyValueFlag = SELECT_KEY_VALUE)*/
          {
            fread( pcKey, 1, iKeyValueLen, fp );
            pcKey[iKeyValueLen] = 0x00;
            
            memset( pcKeyValueLen, 0x00, sizeof(pcKeyValueLen) );
            fread( pcKeyValueLen, 1, KEY_VALUE_LENGTH, fp );
            iValueLen = atoi(pcKeyValueLen);
            
            fread( pcValue, 1, iValueLen, fp );
            pcValue[iValueLen] = 0x00;
            
            managerSelectKeyValueInsertKeyValuePoint( llSelectId, iKeyValueLen, pcKey, iValueLen, pcValue );
          }
	      }
	      fclose(fp);
  	  }
  	
  	  llTmp = sResultBufTmp->llStartIndex;
  	  skvr = sResultBufTmp->skvr;
  	  
  	  /*切换到数据的位置*/
	    while (1)
	    {
	      if (llTmp == llStartIndex)
	      {
	        break;
	      }
	      else
	      {
	        skvr = skvr->next;
	        llTmp++;
	      }
	    }

	    /*计算要发送的数据总量*/
	    iSendLen = 1 + INT64_LEN + INT64_LEN;
	    skvrTmp = skvr;
	    
	    llReturnRows = 0;
	    for (llTmp2 = llTmp; llTmp2 <= (sResultBufTmp->llStartIndex) + (sResultBufTmp->llRows), llTmp2 < llStartIndex + llSelectRows; llTmp2++)
	    {
	      if (sResultBufTmp->iKeyValueFlag == SELECT_KEY)
        {
          iSendLen += skvrTmp->iKeyLen;
      	  iSendLen += KEY_VALUE_LENGTH;
        }
        else
        if (sResultBufTmp->iKeyValueFlag == SELECT_VALUE)
        {
          iSendLen += skvrTmp->iValueLen;
	    	  iSendLen += KEY_VALUE_LENGTH;
        }
        else
        /*if (sResultBufTmp->iKeyValueFlag = SELECT_KEY_VALUE)*/
        {
          iSendLen += skvrTmp->iKeyLen;
      	  iSendLen += KEY_VALUE_LENGTH;
      	
	    	  iSendLen += skvrTmp->iValueLen;
	    	  iSendLen += KEY_VALUE_LENGTH;
        }
        llReturnRows++;
        skvrTmp = skvrTmp->next;
	    }
	    
	    skvrTmp = skvr;
	    
	    /*组发送数据*/
	    pcSendBuf = malloc( iSendLen + 1 );
	    if (pcSendBuf == NULL)
	    {
	    	MANAGER_SELECT_KEY_VALUE_DEAL_SEND_ERROR( ERROR );
	    	MANAGER_SELECT_UNLOCK();
	      return;
	    }
	    memset( pcSendBuf, 0x00, iSendLen + 1 );
      
      memset( pcReturnRows, 0x00, sizeof(pcReturnRows) );
      sprintf( pcReturnRows, "%0*lld", INT64_LEN, llReturnRows );
	    
	    sprintf( &cResult, "%d", SUCCESS );
      memcpy( pcSendBuf, &cResult, 1 );
      memcpy( pcSendBuf + 1, pcSelectId, INT64_LEN );
      memcpy( pcSendBuf + 1 + INT64_LEN, pcReturnRows, INT64_LEN );
      
      iIndex = 1 + INT64_LEN + INT64_LEN;
      
      for (llTmp2 = 0; llTmp2 < llReturnRows; llTmp2++)
	    {
	      if (sResultBufTmp->iKeyValueFlag == SELECT_KEY)
	      {
	        memset( pcKeyValueLen, 0x00, sizeof(pcKeyValueLen) );
	    	  sprintf( pcKeyValueLen, "%0*d", KEY_VALUE_LENGTH, skvrTmp->iKeyLen );
	      	memcpy( pcSendBuf + iIndex, pcKeyValueLen, KEY_VALUE_LENGTH );
	    	  iIndex += KEY_VALUE_LENGTH;
	    	
	    	  memcpy( pcSendBuf + iIndex, skvrTmp->pcKey, skvrTmp->iKeyLen );
	        iIndex += skvrTmp->iKeyLen;
	      }
	      else
        if (sResultBufTmp->iKeyValueFlag == SELECT_VALUE)
        {
          memset( pcKeyValueLen, 0x00, sizeof(pcKeyValueLen) );
          sprintf( pcKeyValueLen, "%0*d", KEY_VALUE_LENGTH, skvrTmp->iValueLen );
	    	  memcpy( pcSendBuf + iIndex, pcKeyValueLen, KEY_VALUE_LENGTH );
	    	  iIndex += KEY_VALUE_LENGTH;
	    	
	        memcpy( pcSendBuf + iIndex, skvrTmp->pcValue, skvrTmp->iValueLen );
	        iIndex += skvrTmp->iValueLen;
        }
        else
        /*if (sResultBufTmp->iKeyValueFlag = SELECT_KEY_VALUE)*/
        {
          memset( pcKeyValueLen, 0x00, sizeof(pcKeyValueLen) );
	    	  sprintf( pcKeyValueLen, "%0*d", KEY_VALUE_LENGTH, skvrTmp->iKeyLen );
	    	  memcpy( pcSendBuf + iIndex, pcKeyValueLen, KEY_VALUE_LENGTH );
	    	  iIndex += KEY_VALUE_LENGTH;
	    	  
	    	  memcpy( pcSendBuf + iIndex, skvrTmp->pcKey, skvrTmp->iKeyLen );
	        iIndex += skvrTmp->iKeyLen;
	        
	    	  
	    	  memset( pcKeyValueLen, 0x00, sizeof(pcKeyValueLen) );
          sprintf( pcKeyValueLen, "%0*d", KEY_VALUE_LENGTH, skvrTmp->iValueLen );
	    	  memcpy( pcSendBuf + iIndex, pcKeyValueLen, KEY_VALUE_LENGTH );
	    	  iIndex += KEY_VALUE_LENGTH;
	    	  
	        memcpy( pcSendBuf + iIndex, skvrTmp->pcValue, skvrTmp->iValueLen );
	        iIndex += skvrTmp->iValueLen;
        }
        
        skvrTmp = skvrTmp->next;
	    }
	    
	    memset( pcSendLen, 0x00, sizeof(pcSendLen) );
      sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
      
      send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
      send( iaSocket, pcSendBuf, iSendLen, 0 );
      free( pcSendBuf );
  	}
  	/*返回数据0*/
  	else
  	{
  	  /*计算要发送的数据总量*/
	    iSendLen = 1 + INT64_LEN + INT64_LEN;
	    
	    pcSendBuf = malloc( iSendLen + 1 );
	    if (pcSendBuf == NULL)
	    {
	    	MANAGER_SELECT_KEY_VALUE_DEAL_SEND_ERROR( ERROR );logFmtSyn( LVNOR, "[aaa]" );
	      MANAGER_SELECT_UNLOCK();
  	    return;
	    }
	    memset( pcSendBuf, 0x00, iSendLen + 1 );
	    
	    sprintf( &cResult, "%d", SUCCESS );
	    
	    memset( pcReturnRows, 0x00, sizeof(pcReturnRows) );
	    sprintf( pcReturnRows, "%0*d", INT64_LEN, 0 );
	    
      memcpy( pcSendBuf, &cResult, 1 );
      memcpy( pcSendBuf + 1, pcSelectId, INT64_LEN );
      memcpy( pcSendBuf + 1 + INT64_LEN, pcReturnRows, INT64_LEN );
	    
	    memset( pcSendLen, 0x00, sizeof(pcSendLen) );
      sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
      
      send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
      send( iaSocket, pcSendBuf, iSendLen, 0 );
      
      free( pcSendBuf );
  	}
  }
  MANAGER_SELECT_UNLOCK();
  return;
}

/**********************************************************************
函数名称: managerSelectKeyValueDealFreeSelectId
函数功能: 释放查询ID
参    数：
第    一：查询ID               I
作    者: 李高文
建立时间: 20170108
**********************************************************************/
void managerSelectKeyValueDealFreeSelectId( __int64 llaSelectId )
{
	char pcSelectResultPath[PATH_LEN];
	__int64 llSelectId;
	SELECT_RESULT_BUF *sResultBufTmp;
	SELECT_RESULT_BUF *sResultBufTmp2;
	SELECT_KEY_VALUE_RESULT *skvr;
	SELECT_KEY_VALUE_RESULT *skvrTmp;
	
	llSelectId = llaSelectId;
	
	/*释放结点*//*sgResultBuf的值为NULL，表示未创建过结点*/
	if (sgResultBuf != NULL)
	{
		/*如果第一个结点就是*/
		if (sgResultBuf->llSelectId == llSelectId)
		{
			/*记录下一个结点的值*/
			sResultBufTmp = sgResultBuf->next;
			
			/*指向该结点的键值缓冲区*/
			skvr = sgResultBuf->skvr;
			
			/*释放结点*/
			while (skvr != NULL)
			{
				skvrTmp = skvr;
				skvr = skvr->next;
				
				/*如果分配过空间，释放结点，并记录当前申请的数量*/
				if (skvrTmp->pcKey != NULL)
				{
					SELECT_FREE( skvrTmp->pcKey, skvrTmp->iKeyLen + 1 );
				}
				
				if (skvrTmp->pcValue != NULL)
				{
					SELECT_FREE( skvrTmp->pcValue, skvrTmp->iValueLen + 1 );
				}
				
				SELECT_FREE( skvrTmp, sizeof(SELECT_KEY_VALUE_RESULT) );
			}
			
			SELECT_FREE( sgResultBuf, sizeof(SELECT_RESULT_BUF) );
			sgResultBuf = sResultBufTmp;
		}
		else
		{
			/*搜索结点*/
			sResultBufTmp = sgResultBuf;
			
			while (sResultBufTmp != NULL)
			{
				/*如果找到结点*/
				if (sResultBufTmp->llSelectId == llSelectId)
				{
					/*指向该结点的键值缓冲区*/
			    skvr = sResultBufTmp->skvr;
			    
			    /*释放结点*/
			    while (skvr != NULL)
			    {
			    	skvrTmp = skvr;
			    	skvr = skvr->next;
			    	
			    	/*如果分配过空间，释放结点，并记录当前申请的数量*/
			    	if (skvrTmp->pcKey != NULL)
			    	{
			    		SELECT_FREE( skvrTmp->pcKey, skvrTmp->iKeyLen + 1 );
			    	}
			    	
			    	if (skvrTmp->pcValue != NULL)
			    	{
			    		SELECT_FREE( skvrTmp->pcValue, skvrTmp->iValueLen + 1 );
			    	}
			    	
			    	SELECT_FREE( skvrTmp, sizeof(SELECT_KEY_VALUE_RESULT) );
			    }
			    
			    sResultBufTmp2 = sResultBufTmp;
			    while (sResultBufTmp2 ->next != sResultBufTmp)
			    {
			    	sResultBufTmp2 = sResultBufTmp2->next;
			    }
			    sResultBufTmp2->next = sResultBufTmp->next;
			    
			    SELECT_FREE( sResultBufTmp, sizeof(SELECT_RESULT_BUF) );
			    
			    break;
				}
				else
				{
					sResultBufTmp = sResultBufTmp->next;
				}
			}
		}
	}
	
	/*组查询结果缓冲文件路径*/
	memset( pcSelectResultPath, 0x00, sizeof(pcSelectResultPath) );
	
	#ifdef SYSTEM_WINDOWS_64
	  if (strlen(pcgSelectTempPath) == 0)
    {
      sprintf( pcSelectResultPath, "\"%lld\"", llSelectId );
    }
    else
    {
    	sprintf( pcSelectResultPath, "\"%s\\%lld\"", pcgSelectTempPath, llSelectId );
    }
	#else
	  if (strlen(pcgSelectTempPath) == 0)
    {
      sprintf( pcSelectResultPath, "\"%lld\"", llSelectId );
    }
    else
    {
    	sprintf( pcSelectResultPath, "%s/%lld", pcgSelectTempPath, llSelectId );
    }
	#endif
	
	/*如果缓冲区文件存在，删除*/
	if (!access( pcSelectResultPath, F_OK ))
	{
		unlink( pcSelectResultPath );
	}
	
	return;
}

/**********************************************************************
函数名称: managerSelectKeyValueDealFreeSelectIdData
函数功能: 仅仅释放查询ID中的数据
参    数：
第    一：查询ID               I
作    者: 李高文
建立时间: 20170110
**********************************************************************/
void managerSelectKeyValueDealFreeSelectIdData( __int64 llaSelectId )
{
	char pcSelectResultPath[PATH_LEN];
	__int64 llSelectId;
	SELECT_RESULT_BUF *sResultBufTmp;
	SELECT_KEY_VALUE_RESULT *skvr;
	SELECT_KEY_VALUE_RESULT *skvrTmp;
	
	llSelectId = llaSelectId;
	
	/*释放结点*//*sgResultBuf的值为NULL，表示未创建过结点*/
	if (sgResultBuf != NULL)
	{
		/*如果第一个结点就是*/
		if (sgResultBuf->llSelectId == llSelectId)
		{
			/*指向该结点的键值缓冲区*/
			skvr = sgResultBuf->skvr;
			
			/*释放结点*/
			while (skvr != NULL)
			{
				skvrTmp = skvr;
				skvr = skvr->next;
				
				/*如果分配过空间，释放结点，并记录当前申请的数量*/
				if (skvrTmp->pcKey != NULL)
				{
					SELECT_FREE( skvrTmp->pcKey, skvrTmp->iKeyLen + 1 );
				}
				
				if (skvrTmp->pcValue != NULL)
				{
					SELECT_FREE( skvrTmp->pcValue, skvrTmp->iValueLen + 1 );
				}
				
				SELECT_FREE( skvrTmp, sizeof(SELECT_KEY_VALUE_RESULT) );
			}
			
			sgResultBuf->skvr = NULL;
			sgResultBuf->skvrLast = NULL;
		}
		else
		{
			/*搜索结点*/
			sResultBufTmp = sgResultBuf;
			
			while (sResultBufTmp != NULL)
			{
				/*如果找到结点*/
				if (sResultBufTmp->llSelectId == llSelectId)
				{
					/*指向该结点的键值缓冲区*/
			    skvr = sResultBufTmp->skvr;
			    
			    /*释放结点*/
			    while (skvr != NULL)
			    {
			    	skvrTmp = skvr;
			    	skvr = skvr->next;
			    	
			    	/*如果分配过空间，释放结点，并记录当前申请的数量*/
			    	if (skvrTmp->pcKey != NULL)
			    	{
			    		SELECT_FREE( skvrTmp->pcKey, skvrTmp->iKeyLen + 1 );
			    	}
			    	
			    	if (skvrTmp->pcValue != NULL)
			    	{
			    		SELECT_FREE( skvrTmp->pcValue, skvrTmp->iValueLen + 1 );
			    	}
			    	
			    	SELECT_FREE( skvrTmp, sizeof(SELECT_KEY_VALUE_RESULT) );
			    }
			    
			    sResultBufTmp->skvr = NULL;
			    sResultBufTmp->skvrLast = NULL;
			    break;
				}
				else
				{
					sResultBufTmp = sResultBufTmp->next;
				}
			}
		}
	}
	
	return;
}

/**********************************************************************
函数名称: managerSelectKeyValueDealFreeResultBuf
函数功能: 释放查询结果中的部分数据，将数据写入到硬盘中
参    数：
第    一：自身的查询ID
第    二：要释放的空间
作    者: 李高文
建立时间: 20170108
**********************************************************************/
void managerSelectKeyValueDealFreeResultBuf( __int64 llaSelectId, int iaSize )
{
	char pcSysDate[DATE_LEN + 1];
	char pcSysTime[TIME_LEN + 1];
	char pcSysDateTime[DATE_TIME + 1];
	char pcSelectResultPath[PATH_LEN];
	FILE *fp;
	__int64 llSelectId;
	int iSize;
	SELECT_RESULT_BUF *sResultBufTmp;
	SELECT_RESULT_BUF *sResultBufTmp2;
	SELECT_KEY_VALUE_RESULT *skvr;
	SELECT_KEY_VALUE_RESULT *skvrTmp;
	
	memset( pcSysDate, 0x00, sizeof(pcSysDate) );
	memset( pcSysTime, 0x00, sizeof(pcSysTime) );
	
	/*获取系统的时间*/
	getDateTime( pcSysDate, pcSysTime );
	
	memset( pcSysDateTime, 0x00, sizeof(pcSysDateTime) );
	sprintf( pcSysDateTime, "%s%s", pcSysDate, pcSysTime );
	
	llSelectId = llaSelectId;
	
	iSize = 0;
	sResultBufTmp = sgResultBuf;
	while (sResultBufTmp != NULL)
	{
		/*如果是本结点，执行下一个*/
		if (sResultBufTmp->llSelectId == llSelectId)
		{
			sResultBufTmp = sResultBufTmp->next;
			continue;
		}
		else
		{
			/*组查询结果缓冲文件路径*/
	    memset( pcSelectResultPath, 0x00, sizeof(pcSelectResultPath) );
	    
	    #ifdef SYSTEM_WINDOWS_64
	      if (strlen(pcgSelectTempPath) == 0)
        {
          sprintf( pcSelectResultPath, "\"%lld\"", llSelectId );
        }
        else
        {
        	sprintf( pcSelectResultPath, "\"%s\\%lld\"", pcgSelectTempPath, llSelectId );
        }
	    #else
	      if (strlen(pcgSelectTempPath) == 0)
        {
          sprintf( pcSelectResultPath, "\"%lld\"", llSelectId );
        }
        else
        {
        	sprintf( pcSelectResultPath, "%s/%lld", pcgSelectTempPath, llSelectId );
        }
	    #endif
	    
	    /*释放结点数据，并将释放的数据写入到缓冲文件中*/
	    fp = fopen( pcSelectResultPath, "wb+" );
	    if (fp == NULL)
	    {
	    	return;
	    }
	    
	    /*指向该结点的键值缓冲区*/
			skvr = sResultBufTmp->skvr;
			
			/*释放结点*/
			while (skvr != NULL)
			{
				skvrTmp = skvr;
				skvr = skvr->next;
				
				/*如果分配过空间，释放结点，并记录当前申请的数量*/
				if (skvrTmp->pcKey != NULL)
				{
					fprintf( fp, "%0*d%s", KEY_VALUE_LENGTH, skvrTmp->iKeyLen, skvrTmp->pcKey );
					SELECT_FREE( skvrTmp->pcKey, skvrTmp->iKeyLen + 1);
					iSize += skvrTmp->iKeyLen + 1;
				}
				
				if (skvrTmp->pcValue != NULL)
				{
					fprintf( fp, "%0*d%s", KEY_VALUE_LENGTH, skvrTmp->iValueLen, skvrTmp->pcValue );
					SELECT_FREE( skvrTmp->pcValue, skvrTmp->iValueLen + 1 );
					iSize += skvrTmp->iValueLen + 1;
				}
				
				SELECT_FREE( skvrTmp, sizeof(SELECT_KEY_VALUE_RESULT) );
				iSize += sizeof(SELECT_KEY_VALUE_RESULT);
			}
			
			/*写入长度0表示结束*/
			fprintf( fp, "%0*d", KEY_VALUE_LENGTH, 0 );
			fclose(fp);
			
			sResultBufTmp->skvr = NULL;
			sResultBufTmp->skvrLast = NULL;
	    
	    /*如果当前查询结点超时，把结点也顺带释放*/
	    if (atoi(pcSysDateTime) - atoi(sResultBufTmp->pcDateTime) > SELECT_ID_OVER_TIME)
	    {
		    sResultBufTmp2 = sResultBufTmp;
			  while (sResultBufTmp2 ->next != sResultBufTmp)
			  {
			  	sResultBufTmp2 = sResultBufTmp2->next;
			  }
			  sResultBufTmp2->next = sResultBufTmp->next;
			  
			  SELECT_FREE( sResultBufTmp, sizeof(SELECT_RESULT_BUF) );
			  iSize += sizeof(SELECT_RESULT_BUF);
			  
			  /*将结点指向当前结点*/
			  sResultBufTmp = sResultBufTmp2;
	    }
	    
	    /*如果释放的空间满足条件*/
	    if (iSize >= iaSize)
	    {
	    	break;
	    }
			sResultBufTmp = sResultBufTmp->next;
		}
	}
	
	return;
}

/**********************************************************************
函数名称: managerSelectKeyValueDealChildThread
函数功能: 管理端处理 select key 操作
参    数：
第    一：共享结构体               I/O
作    者: 李高文
建立时间: 20161108
**********************************************************************/
void managerSelectKeyValueDealChildThread( SELECT_SHARE *selectShare )
{
  int  iStartOffsetRecord[20];   /*开始位置记录*/
	int  iEndOffsetRecord[20];     /*结束位置记录*/
	__int64  llDisc;
	__int64  llSize;
	__int64  llLoops;
	__int64  llSelectIndex;
	int  iLoop;
	int  iKeyValueFlag;             /*0--WHERE条件是对KEY，1--对VALUE*/
	int  iRet;
	SELECT_RESULT_BUF *srb;
	int  iIndepentId;
	int  iIndepentFileId;
		
	llSelectIndex = 0;
	
	/*搜索结点*/
	srb = sgResultBuf;
	logFmtSyn( LVNOR, "%lld", srb );
	while (srb != NULL)
	{
		if (srb->llSelectId == selectShare->llSelectId)
		{
			break;
		}
		else
		{
			srb = srb->next;
		}
	}
	logFmtSyn( LVNOR, "%lld", srb );
	/*如果数据出现异常，可能是因为客户端长时间没和服务端通讯，又或者是客户端没正常结束与服务端的通讯，被清理掉，次数客户端应当重新发起查询，获取查询ID*/
	if (srb == NULL)
	{
    selectShare->iState = RUN_STATE_FINISHED;
    return;
	}
	
	/*确定是KEY或者VALUE作为搜索条件，并且定位*/
	iKeyValueFlag = 0;
	
	/*如果是对VALUE搜索*//*总行数返回0*/
	if (!strcmp( srb->pcValueType, VALUE_OPEN_OPEN ) || 
		  !strcmp( srb->pcValueType, VALUE_OPEN_CLOSE ) || 
		  !strcmp( srb->pcValueType, VALUE_CLOSE_OPEN ) ||
		  !strcmp( srb->pcValueType, VALUE_CLOSE_CLOSE )
		 )
	{
		/*根据 VALUE 搜索，总行数为0 */
		for (iIndepentId = 0; iIndepentId < INDEPENDENT_SIZE; iIndepentId++)
	  {
	    llDisc = sgGlobalVariableMain[iIndepentId].llgFirstDiscMain;
	    llSize = sgGlobalVariableMain[iIndepentId].llgFirstSizeMain;
	    llLoops = sgGlobalVariableMain[iIndepentId].llgLoopsMain;
	  
		  iRet = managerSelectKeyValueDealScanfValueMain( selectShare, srb, &llSelectIndex, llDisc, llSize, 1, llLoops, iIndepentId );
		  if (iRet == INTERRUPT)
		  {
		  	break;
		  }
		  if (srb->llRows >= EACH_SELECT_SIZE)
      {
        break;
      }
	  }
		for (iIndepentFileId = 0; iIndepentFileId < INDEPENDENT_FILE_SIZE; iIndepentFileId++)
	  {
	  	if (iRet == INTERRUPT)
		  {
		  	break;
		  }
		  if (srb->llRows >= EACH_SELECT_SIZE)
      {
        break;
      }
	    llDisc = sgBase[iIndepentFileId].llFirstDisc;
	    llSize = sgBase[iIndepentFileId].llFirstSize;
	    llLoops = sgBase[iIndepentFileId].llLoops;
		  iRet = managerSelectKeyValueDealScanfValue( selectShare, srb, &llSelectIndex, llDisc, llSize, 1, llLoops, iIndepentFileId );
	  }
		selectShare->iState = RUN_STATE_FINISHED;
	  return;
	}
	else
	{
		for (iIndepentId = 0; iIndepentId < INDEPENDENT_SIZE; iIndepentId++)
	  {
	    llDisc = sgGlobalVariableMain[iIndepentId].llgFirstDiscMain;
	    llSize = sgGlobalVariableMain[iIndepentId].llgFirstSizeMain;
	    llLoops = sgGlobalVariableMain[iIndepentId].llgLoopsMain;

		  /*定位开始位置*/
	    {
	    	/*初始化开始位置记录变量*/
	      memset( &iStartOffsetRecord, 0x00, sizeof(iStartOffsetRecord) );
	    
	    	/*如果是左开*/
	      if (!strcmp( srb->pcValueType, KEY_OPEN_OPEN ) != !strcmp( srb->pcValueType, KEY_OPEN_CLOSE ))
        {
      	  iRet = managerPositionLeftOpenMain( iStartOffsetRecord, srb->pcKeyValueStart, llDisc, llSize, 1, llLoops, iIndepentId );
      	  
      	  /*如果没有满足条件*/
      	  if (iRet == NOT_FOUND)
      	  {
            selectShare->iState = RUN_STATE_FINISHED;
            continue;
      	  }
        }
        else
        {
      	  iRet = managerPositionLeftCloseMain( iStartOffsetRecord, srb->pcKeyValueStart, llDisc, llSize, 1, llLoops, iIndepentId );
      	  
      	  /*如果没有满足条件*/
      	  if (iRet == NOT_FOUND)
      	  {
            selectShare->iState = RUN_STATE_FINISHED;
            continue;
      	  }
        }
	    }

	    //logFmtSyn( LVNOR, "定位开始位置结束" );
	    
	    /*定位结束位置*/
	    {
	    	/*初始化结束位置记录变量*/
	      memset( &iEndOffsetRecord, 0x00, sizeof(iEndOffsetRecord) );
	    
	    	/*如果是右开*/
	      if (!strcmp( srb->pcValueType, KEY_OPEN_OPEN ) != !strcmp( srb->pcValueType, KEY_CLOSE_OPEN ))
        {
      	  iRet = managerPositionRightOpenMain( iEndOffsetRecord, srb->pcKeyValueEnd, llDisc, llSize, 1, llLoops, iIndepentId );
      	  
      	  /*如果没有满足条件*/
      	  if (iRet == NOT_FOUND)
      	  {
            selectShare->iState = RUN_STATE_FINISHED;
            //logFmtSyn( LVNOR, "查无条件" );
            continue;
      	  }
        }
        else
        {
      	  iRet = managerPositionRightCloseMain( iEndOffsetRecord, srb->pcKeyValueEnd, llDisc, llSize, 1, llLoops, iIndepentId );
      	  
      	  /*如果没有满足条件*/
      	  if (iRet == NOT_FOUND)
      	  {
            selectShare->iState = RUN_STATE_FINISHED;
            //logFmtSyn( LVNOR, "查无条件" );
            continue;
      	  }
        }
	    }

	    //logFmtSyn( LVNOR, "定位结束位置结束" );
	    
	    /*获取执行结果*/
	    iRet = managerSelectKeyValueDealResultMain( selectShare, srb, &llSelectIndex, iStartOffsetRecord, iEndOffsetRecord, llDisc, llLoops, iIndepentId );
	    
	    if (iRet == INTERRUPT)
		  {
		  	break;
		  }
	    if (srb->llRows >= EACH_SELECT_SIZE)
      {
      	break;
      }
    }

		for (iIndepentFileId = 0; iIndepentFileId < INDEPENDENT_FILE_SIZE; iIndepentFileId++)
	  {
	  	if (iRet == INTERRUPT)
		  {
		  	break;
		  }
	    if (srb->llRows >= EACH_SELECT_SIZE)
      {
      	break;
      }
      
	    llDisc = sgBase[iIndepentFileId].llFirstDisc;
	    llSize = sgBase[iIndepentFileId].llFirstSize;
	    llLoops = sgBase[iIndepentFileId].llLoops;
	    
		  /*定位开始位置*/
	    {
	    	/*初始化开始位置记录变量*/
	      memset( &iStartOffsetRecord, 0x00, sizeof(iStartOffsetRecord) );
	    
	    	/*如果是左开*/
	      if (!strcmp( srb->pcValueType, KEY_OPEN_OPEN ) != !strcmp( srb->pcValueType, KEY_OPEN_CLOSE ))
        {
      	  iRet = managerPositionLeftOpen( iStartOffsetRecord, srb->pcKeyValueStart, llDisc, llSize, 1, llLoops, iIndepentFileId );
      	  
      	  /*如果没有满足条件*/
      	  if (iRet == NOT_FOUND)
      	  {
            selectShare->iState = RUN_STATE_FINISHED;
            continue;
      	  }
        }
        else
        {
      	  iRet = managerPositionLeftClose( iStartOffsetRecord, srb->pcKeyValueStart, llDisc, llSize, 1, llLoops, iIndepentFileId );
      	  
      	  /*如果没有满足条件*/
      	  if (iRet == NOT_FOUND)
      	  {
            selectShare->iState = RUN_STATE_FINISHED;
            continue;
      	  }
        }
	    }
	    
	    //logFmtSyn( LVNOR, "定位开始位置结束" );
	    
	    /*定位结束位置*/
	    {
	    	/*初始化结束位置记录变量*/
	      memset( &iEndOffsetRecord, 0x00, sizeof(iEndOffsetRecord) );
	    
	    	/*如果是右开*/
	      if (!strcmp( srb->pcValueType, KEY_OPEN_OPEN ) != !strcmp( srb->pcValueType, KEY_CLOSE_OPEN ))
        {
      	  iRet = managerPositionRightOpen( iEndOffsetRecord, srb->pcKeyValueEnd, llDisc, llSize, 1, llLoops, iIndepentFileId );
      	  
      	  /*如果没有满足条件*/
      	  if (iRet == NOT_FOUND)
      	  {
            selectShare->iState = RUN_STATE_FINISHED;
            //logFmtSyn( LVNOR, "查无条件" );
            continue;
      	  }
        }
        else
        {
      	  iRet = managerPositionRightClose( iEndOffsetRecord, srb->pcKeyValueEnd, llDisc, llSize, 1, llLoops, iIndepentFileId );
      	  
      	  /*如果没有满足条件*/
      	  if (iRet == NOT_FOUND)
      	  {
            selectShare->iState = RUN_STATE_FINISHED;
            //logFmtSyn( LVNOR, "查无条件" );
            continue;
      	  }
        }
	    }
	    
	    //logFmtSyn( LVNOR, "定位结束位置结束" );
	    
	    /*获取执行结果*/
	    iRet = managerSelectKeyValueDealResult( selectShare, srb, &llSelectIndex, iStartOffsetRecord, iEndOffsetRecord, llDisc, llLoops, iIndepentFileId );
    }
	}
	selectShare->iState = RUN_STATE_FINISHED;
	return;
}

/**********************************************************************
函数名称: managerSelectKeyValueDealScanfValue
函数功能: 管理端处理 select key where value[] 中的结果
参    数：
第    一：线程共享变量     I/O
第    二：块地址           I
第    三：数据量           I
第    四：级数             I
第    五：总级数           I
返    回：
作    者: 李高文
建立时间: 20161109
**********************************************************************/
int managerSelectKeyValueDealScanfValue( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId )
{
	char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
	KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
	KEY_POINT *key;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iTmp;
	int  iRet;
	
	if (selectShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}
	
	/*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
		for (iTmp = 0; iTmp < iaSize; iTmp++ )
	  {
	  	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iTmp * sizeof(KEY_VALUE_POINT));
	      	
	    /*从 VALUE 文件中获取 VALUE 的值*/
      valueToBuf( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentFileId );
      pcValue[keyData->llValueLen] = 0x00;
	    
	    if (!strcmp( selectResult->pcValueType, VALUE_OPEN_OPEN ))
	    {
	    	/*如果不满足条件*/
	    	if (!(strcmp( pcValue, selectResult->pcKeyValueStart ) > 0 && strcmp( pcValue, selectResult->pcKeyValueEnd ) < 0))
	    	{
	    		continue;
	    	}
	    }
	    else
	    if (!strcmp( selectResult->pcValueType, VALUE_OPEN_CLOSE ))
	    {
	    	/*如果不满足条件*/
	    	if (!(strcmp( pcValue, selectResult->pcKeyValueStart ) > 0 && strcmp( pcValue, selectResult->pcKeyValueEnd ) <= 0))
	    	{
	    		continue;
	    	}
	    }
	    else
	    if (!strcmp( selectResult->pcValueType, VALUE_CLOSE_OPEN ))
	    {
	    	/*如果不满足条件*/
	    	if (!(strcmp( pcValue, selectResult->pcKeyValueStart ) >= 0 && strcmp( pcValue, selectResult->pcKeyValueEnd ) < 0))
	    	{
	    		continue;
	    	}
	    }
	    else
	    /*if (!strcmp( selectResult->pcValueType, VALUE_CLOSE_CLOSE ))*/
	    {
	    	/*如果不满足条件*/
	    	if (strcmp( pcValue, selectResult->pcKeyValueStart ) >= 0 && strcmp( pcValue, selectResult->pcKeyValueEnd ) <= 0)
	    	{
	    		continue;
	    	}
	    }
	      	
	  	/*如果到了指定的位置*/
	  	if (*llaSelectIndex >= selectResult->llStartIndex )
	  	{
		    /*如果取到足够的查询数据*/
        if (selectResult->llRows >= EACH_SELECT_SIZE)
        {
        	return SUCCESS;
        }
	      else
	      {
	      	if (selectResult->iKeyValueFlag == SELECT_KEY)
	      	{
	      	  /*从 VALUE 文件中获取 KEY 的值*/
            valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
            pcKey[keyData->llKeyLen] = 0x00;
            
            MANAGER_SELECT_LOCK();
              iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, 0, NULL );
            MANAGER_SELECT_UNLOCK();
            /*如果出现异常*/
            if (iRet)
            {
            	return INTERRUPT;
            }
          }
          else
          if (selectResult->iKeyValueFlag == SELECT_VALUE)
	      	{
	      	  /*设置 VALUE 的值*/
	      	  MANAGER_SELECT_LOCK();
	      	    iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, 0, NULL, keyData->llValueLen, pcValue );
	      	  MANAGER_SELECT_UNLOCK();
	      	  /*如果出现异常*/
            if (iRet)
            {
            	return INTERRUPT;
            }
          }
          else
          /*if (selectResult->iKeyValueFlag == SELECT_KEY_VALUE)*/
	      	{
	      		/*从 VALUE 文件中获取 KEY 的值*/
            valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
            pcKey[keyData->llKeyLen] = 0x00;
            
	      	  /*设置 KEY VALUE 的值*/
	      	  MANAGER_SELECT_LOCK();
              iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, keyData->llValueLen, pcValue );
            MANAGER_SELECT_UNLOCK();
            /*如果出现异常*/
            if (iRet)
            {
            	return INTERRUPT;
            }
          }
          selectResult->llRows++;
	      }
	    }
	    
	    (*llaSelectIndex)++;
	  }
	  
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
    for (iTmp = 0; iTmp < iaSize; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerSelectKeyValueDealScanfValue( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
    }
	}
	
	return SUCCESS;
}

/**********************************************************************
函数名称: managerSelectKeyValueDealResult
函数功能: 管理端处理 select key 中的结果
参    数：
第    一：线程共享变量     I/O
第    二：开始位置         I
第    三：结束位置         I
第    四：数据地址         I
第    五：总级数           I
返    回：
作    者: 李高文
建立时间: 20161108
**********************************************************************/
int managerSelectKeyValueDealResult( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, int iaStartOffset[], int iaEndOffset[], __int64 iaAddr, __int64 iaTotalLoops, int iaIndepentFileId )
{
	SELECT_COUNT_SHARE *selectCountShare;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	KEY_POINT *key;                /*其他级数据中的数据结构*/
	KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
  char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
  __int64 iAddr;
  int  iSize;
  int  iIndex;
  int  iTmp;
  int  iTmp2;
  int  iTmp3;
  int  iRet;
  
  /*搜索开始位置和结束位置不同的级*/
  for (iIndex = 1; iIndex <= iaTotalLoops; iIndex++)
  {
  	if (iaStartOffset[iIndex] < iaEndOffset[iIndex] )
  	{
  		break;
  	}
  }

  /*如果最后的位置都相同，表示只有一个数据满足条件*/
  if (iIndex > iaTotalLoops)
  {
  	/*如果查询开始位置小于或等于查询到的行数，将数据放入到缓冲区*/
  	if (selectResult->llStartIndex <= 1)/*这里的 1 是指当前的查询结果只有1行*/
  	{
  	  /*取出该数据*/
  	  {
  	    /*取出最后级的数据*/
        iAddr = iaAddr;
        for (iTmp2 = 1; iTmp2 < iaTotalLoops; iTmp2++)
        {
        	memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
          getKeyBlockData( pcDataBuf, iAddr, iaIndepentFileId );
        
          key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iTmp2] * sizeof(KEY_POINT));
        
          iAddr = key->llChildAddr;
        }
        
        memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
        getKeyBlockData( pcDataBuf, iAddr, iaIndepentFileId );
        keyData = (KEY_VALUE_POINT*)(pcDataBuf + iaStartOffset[iaTotalLoops] * sizeof(KEY_VALUE_POINT));
  	  }
  	  
  	  /*插入数据*/
      if (selectResult->iKeyValueFlag == SELECT_KEY)
	    {
	      /*从 VALUE 文件中获取 KEY 的值*/
        valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
        pcKey[keyData->llKeyLen] = 0x00;
        
        MANAGER_SELECT_LOCK();
          iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, 0, NULL );
        MANAGER_SELECT_UNLOCK();
        /*如果出现异常*/
        if (iRet)
        {
        	return iRet;
        }
      }
      else
      if (selectResult->iKeyValueFlag == SELECT_VALUE)
	    {
	    	/*从 VALUE 文件中获取 KEY 的值*/
        valueToBuf( pcKey, keyData->llValueAddr, keyData->llValueLen, iaIndepentFileId );
        pcValue[keyData->llValueLen] = 0x00;
        
	      /*设置 VALUE 的值*/
	      MANAGER_SELECT_LOCK();
	        iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, 0, NULL, keyData->llValueLen, pcValue );
	      MANAGER_SELECT_UNLOCK();
	      /*如果出现异常*/
        if (iRet)
        {
        	return iRet;
        }
      }
      else
      /*if (selectResult->iKeyValueFlag == SELECT_KEY_VALUE)*/
	    {
	    	/*从 VALUE 文件中获取 KEY 的值*/
        valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
        pcKey[keyData->llKeyLen] = 0x00;
        
        /*从 VALUE 文件中获取 KEY 的值*/
        valueToBuf( pcKey, keyData->llValueAddr, keyData->llValueLen, iaIndepentFileId );
        pcValue[keyData->llValueLen] = 0x00;
        
	      /*设置 KEY VALUE 的值*/
	      MANAGER_SELECT_LOCK();
          iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, keyData->llValueLen, pcValue );
        MANAGER_SELECT_UNLOCK();
        /*如果出现异常*/
        if (iRet)
        {
        	return iRet;
        }
      }
      
      selectResult->llRows += 1;
  	}
  	return SUCCESS;
  }
  
  //logFmtSyn( LVNOR, "开始获取数据" );
  
  /*取出当前级的数据*/
  iAddr = iaAddr;
  memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
  getKeyBlockData( pcDataBuf, iAddr, iaIndepentFileId );

  for (iTmp = 2; iTmp < iIndex; iTmp++)
  {
  	memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iAddr, iaIndepentFileId );
    
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iTmp] * sizeof(KEY_POINT));
    
    iAddr = key->llChildAddr;
  }

  /*从该级开始，开始位置和结束位置不同*/
    
  /*获取数据*/
  {
    /*处理开始位置的级*/
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iIndex] * sizeof(KEY_POINT));
    iRet = managerSelectKeyValueDealGetDataLeft( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaStartOffset, iIndex + 1, iaTotalLoops, iaIndepentFileId );
    /*如果被中断结束*/
    if (iRet)
    {
    	return iRet;
    }

    /*如果把所有数据都取出来*/
    if (selectResult->llRows >= EACH_SELECT_SIZE)
    {
    	return SUCCESS;
    }
    
    /*处理其他位置的级*/
    for (iTmp3 = iaStartOffset[iIndex] + 1; iTmp3 < iaEndOffset[iIndex]; iTmp3++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp3 * sizeof(KEY_POINT));
      iRet = managerSelectKeyValueDealGetDataOther( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iIndex + 1, iaTotalLoops, iaIndepentFileId );
      if (iRet)
      {
    	  return iRet;
      }
      logFmtSyn( LVNOR, "selectResult->llRows[%lld]llSelectIndex[%lld]", selectResult->llRows, llaSelectIndex );
      /*如果把所有数据都取出来*/
      if (selectResult->llRows == EACH_SELECT_SIZE)
      {
    	  return SUCCESS;
      }
    }
    
    /*处理结束位置的级*/
    key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iIndex] * sizeof(KEY_POINT));
    iRet = managerSelectKeyValueDealGetDataRight( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaEndOffset, iIndex + 1, iaTotalLoops, iaIndepentFileId );
    if (iRet)
    {
    	return iRet;
    }
    //logFmtSyn( LVNOR, "selectResult->llRows[%lld]llSelectIndex[%lld]", selectResult->llRows, llSelectIndex );
  }
  
  return SUCCESS;
}

/**********************************************************************
函数名称: managerSelectKeyValueDealGetDataLeft
函数功能: 管理端处理 select key 中的获取数据
参    数：
第    一：线程共享变量     I
第    二：数据地址         I
第    三：下一级数量       I
第    四：开始位置         I
第    五：级数             I
第    六：总级数           I
返    回：
作    者: 李高文
建立时间: 20161109
**********************************************************************/
int managerSelectKeyValueDealGetDataLeft( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId )
{
	char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
	KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
	KEY_POINT *key;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iTmp;
	int  iRet;
	
	if (selectShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}
		
	/*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
	  /*如果到了指定的位置*/
	  if((*llaSelectIndex) + iaSize - iaStartOffset[iaLoop] >= selectResult->llStartIndex)
	  {
	    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
      getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
      
		  for (iTmp = iaStartOffset[iaLoop]; iTmp < iaSize; iTmp++ )
	    {
	    	/*如果到了指定的位置*/
	    	if (*llaSelectIndex >= selectResult->llStartIndex )
	    	{
		      /*如果取到足够的查询数据*/
          if (selectResult->llRows >= EACH_SELECT_SIZE)
          {
          	return SUCCESS;
          }
	        else
	        {
	        	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iTmp * sizeof(KEY_VALUE_POINT));
	        	if (selectResult->iKeyValueFlag == SELECT_KEY)
	          {
	        	  /*从 VALUE 文件中获取 KEY 的值*/
              valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
              pcKey[keyData->llKeyLen] = 0x00;
              MANAGER_SELECT_LOCK();
                iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, 0, NULL );
              MANAGER_SELECT_UNLOCK();
              /*如果出现异常*/
              if (iRet)
              {
              	return INTERRUPT;
              }
            }
            else
            if (selectResult->iKeyValueFlag == SELECT_VALUE)
	          {
	          	/*从 VALUE 文件中获取 VALUE 的值*/
              valueToBuf( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentFileId );
              pcValue[keyData->llValueLen] = 0x00;          
              
              /*设置 VALUE 的值*/
	      	    MANAGER_SELECT_LOCK();
	      	      iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, 0, NULL, keyData->llValueLen, pcValue );
	      	    MANAGER_SELECT_UNLOCK();
	      	    /*如果出现异常*/
              if (iRet)
              {
              	return INTERRUPT;
              }
	          }
	          else
	          /*if (selectResult->iKeyValueFlag == SELECT_KEY_VALUE)*/
	          {
	          	/*从 VALUE 文件中获取 KEY 的值*/
              valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
              pcKey[keyData->llKeyLen] = 0x00;
              
              /*从 VALUE 文件中获取 VALUE 的值*/
              valueToBuf( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentFileId );
              pcValue[keyData->llValueLen] = 0x00;
              /*设置 KEY VALUE 的值*/
	      	    MANAGER_SELECT_LOCK();
                iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, keyData->llValueLen, pcValue );logFmtSyn( LVNOR, "%d", __LINE__ );
              MANAGER_SELECT_UNLOCK();
              /*如果出现异常*/
              if (iRet)
              {
              	return INTERRUPT;
              }
	          }
            selectResult->llRows++;
	        }
	      }
	      else
	      {
	        (*llaSelectIndex)++;
	      }
	    }
	  }
	  else
	  {
	    *llaSelectIndex += iaSize - iaStartOffset[iaLoop];
	  }
	  
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iaLoop] * sizeof(KEY_POINT));
    
    iRet = managerSelectKeyValueDealGetDataLeft( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaStartOffset, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
    if (iRet)
    {
    	return INTERRUPT;
    }
    
    if (selectResult->llRows >= EACH_SELECT_SIZE)
    {
    	return SUCCESS;
    }
    for (iTmp = iaStartOffset[iaLoop] + 1; iTmp < iaSize; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerSelectKeyValueDealGetDataOther( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
      
      if (selectResult->llRows >= EACH_SELECT_SIZE)
      {
      	return SUCCESS;
      }
    }
	}
	
	return SUCCESS;
}

/**********************************************************************
函数名称: managerSelectKeyValueDealGetDataOther
函数功能: 管理端处理 select key 中的获取数据
参    数：
第    一：线程共享变量     I
第    二：数据地址         I
第    三：下一级数量       I
第    四：级数             I
第    五：总级数           I
返    回：
作    者: 李高文
建立时间: 20161109
**********************************************************************/
int managerSelectKeyValueDealGetDataOther( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId )
{
	char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
	KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
	KEY_POINT *key;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iTmp;
	int  iRet;
	
	if (selectShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}
	
	/*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
	  /*如果到了指定的位置*/
	  if((*llaSelectIndex) + iaSize >= selectResult->llStartIndex)
	  {
	    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
      getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
      
		  for (iTmp = 0; iTmp < iaSize; iTmp++ )
	    {
	    	/*如果到了指定的位置*/
	    	if (*llaSelectIndex >= selectResult->llStartIndex )
	    	{
		      /*如果取到足够的查询数据*/
          if (selectResult->llRows >= EACH_SELECT_SIZE)
          {
          	return SUCCESS;
          }
	        else
	        {
	        	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iTmp * sizeof(KEY_VALUE_POINT));
	        	
	        	if (selectResult->iKeyValueFlag == SELECT_KEY)
	          {
	        	  /*从 VALUE 文件中获取 KEY 的值*/
              valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
              pcKey[keyData->llKeyLen] = 0x00;
              
              MANAGER_SELECT_LOCK();
                iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, 0, NULL );
              MANAGER_SELECT_UNLOCK();
              /*如果出现异常*/
              if (iRet)
              {
              	return INTERRUPT;
              }
            }
            else
            if (selectResult->iKeyValueFlag == SELECT_VALUE)
	          {
	          	/*从 VALUE 文件中获取 VALUE 的值*/
              valueToBuf( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentFileId );
              pcValue[keyData->llValueLen] = 0x00;          
              
              /*设置 VALUE 的值*/
	      	    MANAGER_SELECT_LOCK();
	      	      iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, 0, NULL, keyData->llValueLen, pcValue );
	      	    MANAGER_SELECT_UNLOCK();
	      	    /*如果出现异常*/
              if (iRet)
              {
              	return INTERRUPT;
              }
	          }
	          else
	          /*if (selectResult->iKeyValueFlag == SELECT_KEY_VALUE)*/
	          {
	          	/*从 VALUE 文件中获取 KEY 的值*/
              valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
              pcKey[keyData->llKeyLen] = 0x00;
              
              /*从 VALUE 文件中获取 VALUE 的值*/
              valueToBuf( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentFileId );
              pcValue[keyData->llValueLen] = 0x00;
              
              /*设置 KEY VALUE 的值*/
	      	    MANAGER_SELECT_LOCK();
                iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, keyData->llValueLen, pcValue );
              MANAGER_SELECT_UNLOCK();
              /*如果出现异常*/
              if (iRet)
              {
              	return INTERRUPT;
              }
	          }
            selectResult->llRows++;
	        }
	      }
	      else
	      {
	        (*llaSelectIndex)++;
	      }
	    }
	  }
	  else
	  {
	    *llaSelectIndex += iaSize;
	  }
	  
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
    for (iTmp = 0; iTmp < iaSize; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerSelectKeyValueDealGetDataOther( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
      
      if (selectResult->llRows >= EACH_SELECT_SIZE)
      {
      	return SUCCESS;
      }
    }
	}
	
	return SUCCESS;
}

/**********************************************************************
函数名称: managerSelectKeyValueDealGetDataRight
函数功能: 管理端处理 select key 中的获取数据
参    数：
第    一：线程共享变量     I
第    二：数据地址         I
第    三：下一级数量       I
第    四：开始位置         I
第    五：级数             I
第    六：总级数           I
返    回：
作    者: 李高文
建立时间: 20161109
**********************************************************************/
int managerSelectKeyValueDealGetDataRight( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex,  __int64 iaAddr, __int64 iaSize, int iaEndOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId )
{
	char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
	KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
	KEY_POINT *key;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iTmp;
	int  iRet;
	
	if (selectShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}

	/*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
	  /*如果到了指定的位置*/
	  if( (*llaSelectIndex) + iaEndOffset[iaLoop] + 1 >= selectResult->llStartIndex)
	  {
	    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
      getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
      
		  for (iTmp = 0; iTmp <= iaEndOffset[iaLoop]; iTmp++ )
	    {
	    	/*如果到了指定的位置*/
	    	if (*llaSelectIndex >= selectResult->llStartIndex )
	    	{
		      /*如果取到足够的查询数据*/
          if (selectResult->llRows >= EACH_SELECT_SIZE)
          {
          	return SUCCESS;
          }
	        else
	        {
	        	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iTmp * sizeof(KEY_VALUE_POINT));
	        	
	        	if (selectResult->iKeyValueFlag == SELECT_KEY)
	          {
	        	  /*从 VALUE 文件中获取 KEY 的值*/
              valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
              pcKey[keyData->llKeyLen] = 0x00;
              
              MANAGER_SELECT_LOCK();
                iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, 0, NULL );
              MANAGER_SELECT_UNLOCK();
              /*如果出现异常*/
              if (iRet)
              {
              	return INTERRUPT;
              }
            }
            else
            if (selectResult->iKeyValueFlag == SELECT_VALUE)
	          {
	          	/*从 VALUE 文件中获取 VALUE 的值*/
              valueToBuf( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentFileId );
              pcValue[keyData->llValueLen] = 0x00;          
              
              /*设置 VALUE 的值*/
	      	    MANAGER_SELECT_LOCK();
	      	      iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, 0, NULL, keyData->llValueLen, pcValue );
	      	    MANAGER_SELECT_UNLOCK();
	      	    /*如果出现异常*/
              if (iRet)
              {
              	return INTERRUPT;
              }
	          }
	          else
	          /*if (selectResult->iKeyValueFlag == SELECT_KEY_VALUE)*/
	          {
	          	/*从 VALUE 文件中获取 KEY 的值*/
              valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
              pcKey[keyData->llKeyLen] = 0x00;
              
              /*从 VALUE 文件中获取 VALUE 的值*/
              valueToBuf( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentFileId );
              pcValue[keyData->llValueLen] = 0x00;
              
              /*设置 KEY VALUE 的值*/
	      	    MANAGER_SELECT_LOCK();
                iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, keyData->llValueLen, pcValue );
              MANAGER_SELECT_UNLOCK();
              /*如果出现异常*/
              if (iRet)
              {
              	return INTERRUPT;
              }
	          }
            selectResult->llRows++;
	        }
	      }
	      else
	      {
	        (*llaSelectIndex)++;
	      }
	    }
	  }
	  else
	  {
	    *llaSelectIndex += iaEndOffset[iaLoop] + 1;
	  }
	  
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
    for (iTmp = 0; iTmp < iaEndOffset[iaLoop]; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerSelectKeyValueDealGetDataOther( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
      
      if (selectResult->llRows >= EACH_SELECT_SIZE)
      {
      	return SUCCESS;
      }
    }
    
    key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iaLoop] * sizeof(KEY_POINT));
    
    iRet = managerSelectKeyValueDealGetDataRight( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaEndOffset, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
    if (iRet)
    {
    	return INTERRUPT;
    }
    if (selectResult->llRows >= EACH_SELECT_SIZE)
    {
    	return SUCCESS;
    }
	}
	
	return SUCCESS;
}

/**********************************************************************
函数名称: managerSelectKeyValueInsertKeyValuePoint
函数功能: 将结果数据插入到结果缓冲区
参    数：
第    一：查询ID           I
第    二：KEY 长度         I
第    三：KEY 缓冲区       I
第    四：VALUE 长度       I
第    五：VALUE 缓冲区     I
返    回：
作    者: 李高文
建立时间: 20170108
**********************************************************************/
int managerSelectKeyValueInsertKeyValuePoint( __int64 llaSelectId, int iaKeyLen, char *pcaKey, int iaValueLen, char *pcaValue )
{
  SELECT_KEY_VALUE_RESULT *skvr;             /*查询结果*/
  SELECT_RESULT_BUF *srb;
  //logFmtSyn( LVNOR, "pcaKey[%s]pcaValue[%s]", pcaKey, pcaValue );
	/*搜索结点*/
	srb = sgResultBuf;
	while (srb != NULL)
	{
		if (srb->llSelectId == llaSelectId)
		{
			break;
		}
		else
		{
			srb = srb->next;
		}
	}
	/*如果数据出现异常，可能是因为客户端长时间没和服务端通讯，又或者是客户端没正常结束与服务端的通讯，被清理掉，次数客户端应当重新发起查询，获取查询ID*/
	if (srb == NULL)
	{
    return ERROR;
	}
	
	if (srb->skvr == NULL)
	{
	  /*如果没有足够的空间，释放结点中的结果数据*/
  	SELECT_MALLOC( srb->skvr, llaSelectId, sizeof(SELECT_KEY_VALUE_RESULT) );
	  if (srb->skvr == NULL)
	  {
	    return ERROR;
	  }
	  /*如果有 KEY 要插入*/
	  if (iaKeyLen != 0 && pcaKey != NULL)
	  {
	    srb->skvr->iKeyLen = iaKeyLen;
	    
	    SELECT_MALLOC( srb->skvr->pcKey, llaSelectId, iaKeyLen + 1 );
	    if (srb->skvr->pcKey == NULL)
	    {
	      SELECT_FREE( srb->skvr, sizeof(SELECT_KEY_VALUE_RESULT) );
	      if (srb->skvr->pcValue != NULL)
	      {
	        SELECT_FREE( srb->skvr->pcValue, srb->skvr->iValueLen + 1 );
	      }
	      
	      return ERROR;
	    }
	    memcpy( srb->skvr->pcKey, pcaKey, iaKeyLen );
	  }
	  
	  /*如果有 VALUE 要插入*/
	  if (iaValueLen != 0 && pcaValue != NULL)
	  {
	    srb->skvr->iValueLen = iaValueLen;
	    
	    SELECT_MALLOC( srb->skvr->pcValue, llaSelectId, iaValueLen + 1 );
	    if (srb->skvr->pcValue == NULL)
	    {
	      SELECT_FREE( srb->skvr, sizeof(SELECT_KEY_VALUE_RESULT) );
	      if (srb->skvr->pcKey != NULL)
	      {
	        SELECT_FREE( srb->skvr->pcKey, srb->skvr->iKeyLen + 1 );
	      }
	      
	      return ERROR;
	    }
	    memcpy( srb->skvr->pcValue, pcaValue, iaValueLen );
	  }
	  srb->skvrLast = srb->skvr;
	}
	else
	{
		if (srb->skvrLast == srb->skvr)
		{
			SELECT_MALLOC( srb->skvrLast, llaSelectId, sizeof(SELECT_KEY_VALUE_RESULT) );
	    if (srb->skvrLast == NULL)
	    {
	      return ERROR;
	    }
		  
		  /*如果有 KEY 要插入*/
	    if (iaKeyLen != 0 && pcaKey != NULL)
	    {
	      srb->skvrLast->iKeyLen = iaKeyLen;
	      
	      SELECT_MALLOC( srb->skvrLast->pcKey, llaSelectId, iaKeyLen + 1 );
	      if (srb->skvrLast->pcKey == NULL)
	      {
	        SELECT_FREE( srb->skvrLast, sizeof(SELECT_KEY_VALUE_RESULT) );
	        if (srb->skvrLast->pcValue != NULL)
	        {
	          SELECT_FREE( srb->skvrLast->pcValue, srb->skvrLast->iValueLen + 1 );
	        }
	        
	        return ERROR;
	      }
	      memcpy( srb->skvrLast->pcKey, pcaKey, iaKeyLen );
	    }
	    
	    /*如果有 VALUE 要插入*/
	    if (iaValueLen != 0 && pcaValue != NULL)
	    {
	      srb->skvrLast->iValueLen = iaValueLen;
	      
	      SELECT_MALLOC( srb->skvrLast->pcValue, llaSelectId, iaValueLen + 1 );
	      if (srb->skvrLast->pcValue == NULL)
	      {
	        SELECT_FREE( srb->skvrLast, sizeof(SELECT_KEY_VALUE_RESULT) );
	        if (srb->skvrLast->pcKey != NULL)
	        {
	          SELECT_FREE( srb->skvrLast->pcKey, srb->skvrLast->iKeyLen + 1 );
	        }
	        
	        return ERROR;
	      }
	      memcpy( srb->skvrLast->pcValue, pcaValue, iaValueLen );
	    }
	    
	    srb->skvr->next = srb->skvrLast;
		}
		else
		{
			SELECT_MALLOC( srb->skvrLast->next, llaSelectId, sizeof(SELECT_KEY_VALUE_RESULT) );
	    if (srb->skvrLast->next == NULL)
	    {
	      return ERROR;
	    }
		  
		  /*如果有 KEY 要插入*/
	    if (iaKeyLen != 0 && pcaKey != NULL)
	    {
	      srb->skvrLast->next->iKeyLen = iaKeyLen;
	      
	      SELECT_MALLOC( srb->skvrLast->next->pcKey, llaSelectId, iaKeyLen + 1 );
	      if (srb->skvrLast->next->pcKey == NULL)
	      {
	        SELECT_FREE( srb->skvrLast->next, sizeof(SELECT_KEY_VALUE_RESULT) );
	        if (srb->skvrLast->next->pcValue != NULL)
	        {
	          SELECT_FREE( srb->skvrLast->next->pcValue, srb->skvrLast->next->iValueLen + 1 );
	        }
	        
	        return ERROR;
	      }
	      memcpy( srb->skvrLast->next->pcKey, pcaKey, iaKeyLen );
	    }
	    
	    /*如果有 VALUE 要插入*/
	    if (iaValueLen != 0 && pcaValue != NULL)
	    {
	      srb->skvrLast->next->iValueLen = iaValueLen;
	      
	      SELECT_MALLOC( srb->skvrLast->next->pcValue, llaSelectId, iaValueLen + 1 );
	      if (srb->skvrLast->next->pcValue == NULL)
	      {
	        SELECT_FREE( srb->skvrLast->next, sizeof(SELECT_KEY_VALUE_RESULT) );
	        if (srb->skvrLast->next->pcKey != NULL)
	        {
	          SELECT_FREE( srb->skvrLast->next->pcKey, srb->skvrLast->next->iKeyLen + 1 );
	        }
	        
	        return ERROR;
	      }
	      memcpy( srb->skvrLast->next->pcValue, pcaValue, iaValueLen );
	    }
	    
	    srb->skvrLast = srb->skvrLast->next;
		}
	}
	
	return SUCCESS;
}

/**********************************************************************
函数名称: managerSelectKeyValueDealScanfValueMain
函数功能: 管理端处理 select key where value[] 中的结果
参    数：
第    一：线程共享变量     I/O
第    二：块地址           I
第    三：数据量           I
第    四：级数             I
第    五：总级数           I
返    回：
作    者: 李高文
建立时间: 20161109
**********************************************************************/
int managerSelectKeyValueDealScanfValueMain( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId )
{
	char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
	KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
	KEY_POINT *key;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iTmp;
	int  iRet;
	
	if (selectShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}
	
	
	
	/*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
		for (iTmp = 0; iTmp < iaSize; iTmp++ )
	  {
	  	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iTmp * sizeof(KEY_VALUE_POINT));
	      	
	    /*从 VALUE 文件中获取 VALUE 的值*/
      valueToBufMain( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentId );
      pcValue[keyData->llValueLen] = 0x00;
	    
	    if (!strcmp( selectResult->pcValueType, VALUE_OPEN_OPEN ))
	    {
	    	/*如果不满足条件*/
	    	if (!(strcmp( pcValue, selectResult->pcKeyValueStart ) > 0 && strcmp( pcValue, selectResult->pcKeyValueEnd ) < 0))
	    	{
	    		continue;
	    	}
	    }
	    else
	    if (!strcmp( selectResult->pcValueType, VALUE_OPEN_CLOSE ))
	    {
	    	/*如果不满足条件*/
	    	if (!(strcmp( pcValue, selectResult->pcKeyValueStart ) > 0 && strcmp( pcValue, selectResult->pcKeyValueEnd ) <= 0))
	    	{
	    		continue;
	    	}
	    }
	    else
	    if (!strcmp( selectResult->pcValueType, VALUE_CLOSE_OPEN ))
	    {
	    	/*如果不满足条件*/
	    	if (!(strcmp( pcValue, selectResult->pcKeyValueStart ) >= 0 && strcmp( pcValue, selectResult->pcKeyValueEnd ) < 0))
	    	{
	    		continue;
	    	}
	    }
	    else
	    /*if (!strcmp( selectResult->pcValueType, VALUE_CLOSE_CLOSE ))*/
	    {
	    	/*如果不满足条件*/
	    	if (strcmp( pcValue, selectResult->pcKeyValueStart ) >= 0 && strcmp( pcValue, selectResult->pcKeyValueEnd ) <= 0)
	    	{
	    		continue;
	    	}
	    }
	      	
	  	/*如果到了指定的位置*/
	  	if (*llaSelectIndex >= selectResult->llStartIndex )
	  	{
		    /*如果取到足够的查询数据*/
        if (selectResult->llRows >= EACH_SELECT_SIZE)
        {
        	return SUCCESS;
        }
	      else
	      {
	      	if (selectResult->iKeyValueFlag == SELECT_KEY)
	      	{
	      	  /*从 VALUE 文件中获取 KEY 的值*/
            valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
            pcKey[keyData->llKeyLen] = 0x00;
            
            MANAGER_SELECT_LOCK();
              iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, 0, NULL );logFmtSyn(LVNOR,"a");
            MANAGER_SELECT_UNLOCK();
            /*如果出现异常*/
            if (iRet)
            {
            	return INTERRUPT;
            }
          }
          else
          if (selectResult->iKeyValueFlag == SELECT_VALUE)
	      	{
	      	  /*设置 VALUE 的值*/
	      	  MANAGER_SELECT_LOCK();
	      	    iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, 0, NULL, keyData->llValueLen, pcValue );logFmtSyn(LVNOR,"a");
	      	  MANAGER_SELECT_UNLOCK();
	      	  /*如果出现异常*/
            if (iRet)
            {
            	return INTERRUPT;
            }
          }
          else
          /*if (selectResult->iKeyValueFlag == SELECT_KEY_VALUE)*/
	      	{
	      		/*从 VALUE 文件中获取 KEY 的值*/
            valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
            pcKey[keyData->llKeyLen] = 0x00;
            
	      	  /*设置 KEY VALUE 的值*/
	      	  MANAGER_SELECT_LOCK();
              iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, keyData->llValueLen, pcValue );logFmtSyn(LVNOR,"a");
            MANAGER_SELECT_UNLOCK();
            /*如果出现异常*/
            if (iRet)
            {
            	return INTERRUPT;
            }
          }
          selectResult->llRows++;
	      }
	    }
	    
	    (*llaSelectIndex)++;
	  }
	  
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    for (iTmp = 0; iTmp < iaSize; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerSelectKeyValueDealScanfValueMain( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
    }
	}
	
	return SUCCESS;
}

/**********************************************************************
函数名称: managerSelectKeyValueDealResultMain
函数功能: 管理端处理 select key 中的结果
参    数：
第    一：线程共享变量     I/O
第    二：开始位置         I
第    三：结束位置         I
第    四：数据地址         I
第    五：总级数           I
返    回：
作    者: 李高文
建立时间: 20161108
**********************************************************************/
int managerSelectKeyValueDealResultMain( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, int iaStartOffset[], int iaEndOffset[], __int64 iaAddr, __int64 iaTotalLoops, int iaIndepentId )
{
	SELECT_COUNT_SHARE *selectCountShare;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	KEY_POINT *key;                /*其他级数据中的数据结构*/
	KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
  char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
  __int64 iAddr;
  int  iSize;
  int  iIndex;
  int  iTmp;
  int  iTmp2;
  int  iTmp3;
  int  iRet;

  /*搜索开始位置和结束位置不同的级*/
  for (iIndex = 1; iIndex <= iaTotalLoops; iIndex++)
  {
  	if (iaStartOffset[iIndex] < iaEndOffset[iIndex] )
  	{
  		break;
  	}
  }

  /*如果最后的位置都相同，表示只有一个数据满足条件*/
  if (iIndex > iaTotalLoops)
  {
  	/*如果查询开始位置小于或等于查询到的行数，将数据放入到缓冲区*/
  	if (selectResult->llStartIndex <= 1)/*这里的 1 是指当前的查询结果只有1行*/
  	{
  	  /*取出该数据*/
  	  {
  	    /*取出最后级的数据*/
        iAddr = iaAddr;
        for (iTmp2 = 1; iTmp2 < iaTotalLoops; iTmp2++)
        {
        	memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
          getKeyBlockDataMain( pcDataBuf, iAddr, iaIndepentId );
        
          key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iTmp2] * sizeof(KEY_POINT));
        
          iAddr = key->llChildAddr;
        }
        
        memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
        getKeyBlockDataMain( pcDataBuf, iAddr, iaIndepentId );
        keyData = (KEY_VALUE_POINT*)(pcDataBuf + iaStartOffset[iaTotalLoops] * sizeof(KEY_VALUE_POINT));
  	  }
  	  
  	  /*插入数据*/
      if (selectResult->iKeyValueFlag == SELECT_KEY)
	    {
	      /*从 VALUE 文件中获取 KEY 的值*/
        valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
        pcKey[keyData->llKeyLen] = 0x00;
        
        MANAGER_SELECT_LOCK();
          iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, 0, NULL );
        MANAGER_SELECT_UNLOCK();
        /*如果出现异常*/
        if (iRet)
        {
        	return iRet;
        }
      }
      else
      if (selectResult->iKeyValueFlag == SELECT_VALUE)
	    {
	    	/*从 VALUE 文件中获取 KEY 的值*/
        valueToBufMain( pcKey, keyData->llValueAddr, keyData->llValueLen, iaIndepentId );
        pcValue[keyData->llValueLen] = 0x00;
        
	      /*设置 VALUE 的值*/
	      MANAGER_SELECT_LOCK();
	        iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, 0, NULL, keyData->llValueLen, pcValue );
	      MANAGER_SELECT_UNLOCK();
	      /*如果出现异常*/
        if (iRet)
        {
        	return iRet;
        }
      }
      else
      /*if (selectResult->iKeyValueFlag == SELECT_KEY_VALUE)*/
	    {
	    	/*从 VALUE 文件中获取 KEY 的值*/
        valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
        pcKey[keyData->llKeyLen] = 0x00;
        
        /*从 VALUE 文件中获取 KEY 的值*/
        valueToBufMain( pcKey, keyData->llValueAddr, keyData->llValueLen, iaIndepentId );
        pcValue[keyData->llValueLen] = 0x00;
        
	      /*设置 KEY VALUE 的值*/
	      MANAGER_SELECT_LOCK();
          iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, keyData->llValueLen, pcValue );
        MANAGER_SELECT_UNLOCK();
        /*如果出现异常*/
        if (iRet)
        {
        	return iRet;
        }
      }
      
      selectResult->llRows += 1;
  	}
  	return SUCCESS;
  }

  //logFmtSyn( LVNOR, "开始获取数据" );

  /*取出当前级的数据*/
  iAddr = iaAddr;
  memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
  getKeyBlockDataMain( pcDataBuf, iAddr, iaIndepentId );

  for (iTmp = 2; iTmp < iIndex; iTmp++)
  {
  	memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iAddr, iaIndepentId );
    
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iTmp] * sizeof(KEY_POINT));
    
    iAddr = key->llChildAddr;
  }
 
  /*从该级开始，开始位置和结束位置不同*/
  
  /*获取数据*/
  {
    /*处理开始位置的级*/
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iIndex] * sizeof(KEY_POINT));
    iRet = managerSelectKeyValueDealGetDataLeftMain( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaStartOffset, iIndex + 1, iaTotalLoops, iaIndepentId );
    /*如果被中断结束*/
    if (iRet)
    {
    	return iRet;
    }
//logFmtSyn( LVNOR, "selectResult->llRows[%lld]llSelectIndex[%lld]", selectResult->llRows, *llaSelectIndex );
    /*如果把所有数据都取出来*/
    if (selectResult->llRows >= EACH_SELECT_SIZE)
    {
    	return SUCCESS;
    }

    /*处理其他位置的级*/
    for (iTmp3 = iaStartOffset[iIndex] + 1; iTmp3 < iaEndOffset[iIndex]; iTmp3++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp3 * sizeof(KEY_POINT));
      iRet = managerSelectKeyValueDealGetDataOtherMain( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iIndex + 1, iaTotalLoops, iaIndepentId );
      if (iRet)
      {
    	  return iRet;
      }
      /*如果把所有数据都取出来*/
      if (selectResult->llRows == EACH_SELECT_SIZE)
      {
    	  return SUCCESS;
      }
    }
//logFmtSyn( LVNOR, "selectResult->llRows[%lld]llSelectIndex[%lld]", selectResult->llRows, *llaSelectIndex );
    /*处理结束位置的级*/
    key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iIndex] * sizeof(KEY_POINT));
    iRet = managerSelectKeyValueDealGetDataRightMain( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaEndOffset, iIndex + 1, iaTotalLoops, iaIndepentId );
    if (iRet)
    {
    	return iRet;
    }
  }
//logFmtSyn( LVNOR, "selectResult->llRows[%lld]llSelectIndex[%lld]", selectResult->llRows, *llaSelectIndex );  
  return SUCCESS;
}

/**********************************************************************
函数名称: managerSelectKeyValueDealGetDataLeftMain
函数功能: 管理端处理 select key 中的获取数据
参    数：
第    一：线程共享变量     I
第    二：数据地址         I
第    三：下一级数量       I
第    四：开始位置         I
第    五：级数             I
第    六：总级数           I
返    回：
作    者: 李高文
建立时间: 20161109
**********************************************************************/
int managerSelectKeyValueDealGetDataLeftMain( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId )
{
	char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
	KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
	KEY_POINT *key;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iTmp;
	int  iRet;

	if (selectShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}
//logFmtSyn( LVNOR, "%lld:%lld", iaLoop, iaTotalLoops );
	/*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
		//logFmtSyn( LVNOR, "%lld:%lld", *llaSelectIndex, selectResult->llStartIndex );
	  /*如果到了指定的位置*/
	  if((*llaSelectIndex) + iaSize - iaStartOffset[iaLoop] >= selectResult->llStartIndex)
	  {
	    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
      getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );

		  for (iTmp = iaStartOffset[iaLoop]; iTmp < iaSize; iTmp++ )
	    {
	    	/*如果到了指定的位置*/
	    	if (*llaSelectIndex >= selectResult->llStartIndex )
	    	{
		      /*如果取到足够的查询数据*/
          if (selectResult->llRows >= EACH_SELECT_SIZE)
          {
          	return SUCCESS;
          }
	        else
	        {
	        	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iTmp * sizeof(KEY_VALUE_POINT));
	        	
	        	if (selectResult->iKeyValueFlag == SELECT_KEY)
	          {
	        	  /*从 VALUE 文件中获取 KEY 的值*/
              valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
              pcKey[keyData->llKeyLen] = 0x00;
              MANAGER_SELECT_LOCK();
                iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, 0, NULL );
              MANAGER_SELECT_UNLOCK();
              /*如果出现异常*/
              if (iRet)
              {
              	return INTERRUPT;
              }
            }
            else
            if (selectResult->iKeyValueFlag == SELECT_VALUE)
	          {
	          	/*从 VALUE 文件中获取 VALUE 的值*/
              valueToBufMain( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentId );
              pcValue[keyData->llValueLen] = 0x00;     
              
              //logFmtSyn( LVNOR, "pcValue[%s][%d]", pcValue, keyData->llValueLen );     
              
              /*设置 VALUE 的值*/
	      	    MANAGER_SELECT_LOCK();
	      	      iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, 0, NULL, keyData->llValueLen, pcValue );
	      	    MANAGER_SELECT_UNLOCK();
	      	    /*如果出现异常*/
              if (iRet)
              {
              	return INTERRUPT;
              }
	          }
	          else
	          /*if (selectResult->iKeyValueFlag == SELECT_KEY_VALUE)*/
	          {
	          	/*从 VALUE 文件中获取 KEY 的值*/
              valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
              pcKey[keyData->llKeyLen] = 0x00;
              
              /*从 VALUE 文件中获取 VALUE 的值*/
              valueToBufMain( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentId );
              pcValue[keyData->llValueLen] = 0x00;
              /*设置 KEY VALUE 的值*/
	      	    MANAGER_SELECT_LOCK();
                iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, keyData->llValueLen, pcValue );
              MANAGER_SELECT_UNLOCK();
              /*如果出现异常*/
              if (iRet)
              {
              	return INTERRUPT;
              }
	          }
            selectResult->llRows++;
	        }
	      }
	      else
	      {
	        (*llaSelectIndex)++;
	      }
	    }
	  }
	  else
	  {
	    *llaSelectIndex += iaSize - iaStartOffset[iaLoop];
	  }
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iaLoop] * sizeof(KEY_POINT));
    
    iRet = managerSelectKeyValueDealGetDataLeftMain( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaStartOffset, iaLoop + 1, iaTotalLoops, iaIndepentId );
    if (iRet)
    {
    	return INTERRUPT;
    }
    if (selectResult->llRows >= EACH_SELECT_SIZE)
    {
    	return SUCCESS;
    }
    for (iTmp = iaStartOffset[iaLoop] + 1; iTmp < iaSize; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerSelectKeyValueDealGetDataOtherMain( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
      
      if (selectResult->llRows >= EACH_SELECT_SIZE)
      {
      	return SUCCESS;
      }
    }
	}
	
	return SUCCESS;
}

/**********************************************************************
函数名称: managerSelectKeyValueDealGetDataOtherMain
函数功能: 管理端处理 select key 中的获取数据
参    数：
第    一：线程共享变量     I
第    二：数据地址         I
第    三：下一级数量       I
第    四：级数             I
第    五：总级数           I
返    回：
作    者: 李高文
建立时间: 20161109
**********************************************************************/
int managerSelectKeyValueDealGetDataOtherMain( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId )
{
	char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
	KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
	KEY_POINT *key;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iTmp;
	int  iRet;
	
	//logFmtSyn( LVNOR, "selectResult->llRows[%lld]llSelectIndex[%lld]", selectResult->llRows, *llaSelectIndex );
	if (selectShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}
	
	/*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
		//logFmtSyn( LVNOR, "selectResult->llRows[%lld]llSelectIndex[%lld]", selectResult->llRows, *llaSelectIndex );
	  /*如果到了指定的位置*/
	  if((*llaSelectIndex) + iaSize >= selectResult->llStartIndex)
	  {
	    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
      getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
      
		  for (iTmp = 0; iTmp < iaSize; iTmp++ )
	    {
	    	/*如果到了指定的位置*/
	    	if (*llaSelectIndex >= selectResult->llStartIndex )
	    	{
		      /*如果取到足够的查询数据*/
          if (selectResult->llRows >= EACH_SELECT_SIZE)
          {
          	return SUCCESS;
          }
	        else
	        {
	        	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iTmp * sizeof(KEY_VALUE_POINT));
	        	
	        	if (selectResult->iKeyValueFlag == SELECT_KEY)
	          {
	        	  /*从 VALUE 文件中获取 KEY 的值*/
              valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
              pcKey[keyData->llKeyLen] = 0x00;
              
              MANAGER_SELECT_LOCK();
                iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, 0, NULL );
              MANAGER_SELECT_UNLOCK();
              /*如果出现异常*/
              if (iRet)
              {
              	return INTERRUPT;
              }
            }
            else
            if (selectResult->iKeyValueFlag == SELECT_VALUE)
	          {
	          	/*从 VALUE 文件中获取 VALUE 的值*/
              valueToBufMain( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentId );
              pcValue[keyData->llValueLen] = 0x00;          
              
              /*设置 VALUE 的值*/
	      	    MANAGER_SELECT_LOCK();
	      	      iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, 0, NULL, keyData->llValueLen, pcValue );
	      	    MANAGER_SELECT_UNLOCK();
	      	    /*如果出现异常*/
              if (iRet)
              {
              	return INTERRUPT;
              }
	          }
	          else
	          /*if (selectResult->iKeyValueFlag == SELECT_KEY_VALUE)*/
	          {
	          	/*从 VALUE 文件中获取 KEY 的值*/
              valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
              pcKey[keyData->llKeyLen] = 0x00;
              
              /*从 VALUE 文件中获取 VALUE 的值*/
              valueToBufMain( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentId );
              pcValue[keyData->llValueLen] = 0x00;
              
              /*设置 KEY VALUE 的值*/
	      	    MANAGER_SELECT_LOCK();
                iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, keyData->llValueLen, pcValue );
              MANAGER_SELECT_UNLOCK();
              /*如果出现异常*/
              if (iRet)
              {
              	return INTERRUPT;
              }
	          }
            selectResult->llRows++;
	        }
	      }
	      else
	      {
	        (*llaSelectIndex)++;
	      }
	    }
	  }
	  else
	  {
	    *llaSelectIndex += iaSize;
	  }
	  
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    for (iTmp = 0; iTmp < iaSize; iTmp++)
    {
    	//logFmtSyn( LVNOR, "selectResult->llRows[%lld]llSelectIndex[%lld]", selectResult->llRows, *llaSelectIndex );
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerSelectKeyValueDealGetDataOtherMain( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
      
      if (selectResult->llRows >= EACH_SELECT_SIZE)
      {
      	return SUCCESS;
      }
      //logFmtSyn( LVNOR, "selectResult->llRows[%lld]llSelectIndex[%lld]", selectResult->llRows, *llaSelectIndex );
    }
	}
	
	return SUCCESS;
}

/**********************************************************************
函数名称: managerSelectKeyValueDealGetDataRightMain
函数功能: 管理端处理 select key 中的获取数据
参    数：
第    一：线程共享变量     I
第    二：数据地址         I
第    三：下一级数量       I
第    四：开始位置         I
第    五：级数             I
第    六：总级数           I
返    回：
作    者: 李高文
建立时间: 20161109
**********************************************************************/
int managerSelectKeyValueDealGetDataRightMain( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex,  __int64 iaAddr, __int64 iaSize, int iaEndOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId )
{
	char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
	KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
	KEY_POINT *key;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iTmp;
	int  iRet;
	
	if (selectShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}

	/*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
	  /*如果到了指定的位置*/
	  if( (*llaSelectIndex) + iaEndOffset[iaLoop] + 1 >= selectResult->llStartIndex)
	  {
	    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
      getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
      
		  for (iTmp = 0; iTmp <= iaEndOffset[iaLoop]; iTmp++ )
	    {
	    	/*如果到了指定的位置*/
	    	if (*llaSelectIndex >= selectResult->llStartIndex )
	    	{
		      /*如果取到足够的查询数据*/
          if (selectResult->llRows >= EACH_SELECT_SIZE)
          {
          	return SUCCESS;
          }
	        else
	        {
	        	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iTmp * sizeof(KEY_VALUE_POINT));
	        	
	        	if (selectResult->iKeyValueFlag == SELECT_KEY)
	          {
	        	  /*从 VALUE 文件中获取 KEY 的值*/
              valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
              pcKey[keyData->llKeyLen] = 0x00;
              
              MANAGER_SELECT_LOCK();
                iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, 0, NULL );
              MANAGER_SELECT_UNLOCK();
              /*如果出现异常*/
              if (iRet)
              {
              	return INTERRUPT;
              }
            }
            else
            if (selectResult->iKeyValueFlag == SELECT_VALUE)
	          {
	          	/*从 VALUE 文件中获取 VALUE 的值*/
              valueToBufMain( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentId );
              pcValue[keyData->llValueLen] = 0x00;          
              
              /*设置 VALUE 的值*/
	      	    MANAGER_SELECT_LOCK();
	      	      iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, 0, NULL, keyData->llValueLen, pcValue );
	      	    MANAGER_SELECT_UNLOCK();
	      	    /*如果出现异常*/
              if (iRet)
              {
              	return INTERRUPT;
              }
	          }
	          else
	          /*if (selectResult->iKeyValueFlag == SELECT_KEY_VALUE)*/
	          {
	          	/*从 VALUE 文件中获取 KEY 的值*/
              valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
              pcKey[keyData->llKeyLen] = 0x00;
              
              /*从 VALUE 文件中获取 VALUE 的值*/
              valueToBufMain( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentId );
              pcValue[keyData->llValueLen] = 0x00;
              
              /*设置 KEY VALUE 的值*/
	      	    MANAGER_SELECT_LOCK();
                iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, keyData->llValueLen, pcValue );
              MANAGER_SELECT_UNLOCK();
              /*如果出现异常*/
              if (iRet)
              {
              	return INTERRUPT;
              }
	          }
            selectResult->llRows++;
	        }
	      }
	      else
	      {
	        (*llaSelectIndex)++;
	      }
	    }
	  }
	  else
	  {
	    *llaSelectIndex += iaEndOffset[iaLoop] + 1;
	  }
	  
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    for (iTmp = 0; iTmp < iaEndOffset[iaLoop]; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerSelectKeyValueDealGetDataOtherMain( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
      
      if (selectResult->llRows >= EACH_SELECT_SIZE)
      {
      	return SUCCESS;
      }
    }
    
    key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iaLoop] * sizeof(KEY_POINT));
    
    iRet = managerSelectKeyValueDealGetDataRightMain( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaEndOffset, iaLoop + 1, iaTotalLoops, iaIndepentId );
    if (iRet)
    {
    	return INTERRUPT;
    }
    if (selectResult->llRows >= EACH_SELECT_SIZE)
    {
    	return SUCCESS;
    }
	}
	
	return SUCCESS;
}

