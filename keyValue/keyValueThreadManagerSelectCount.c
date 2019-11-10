#define __KEY_VALUE_THREAD_MANAGER_SELECT_COUNT_C

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

void managerSelectCountDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd );
void managerSelectCountDealChildThread( SELECT_COUNT_SHARE *selectCountShare );
int  managerSelectCountDealResult( SELECT_COUNT_SHARE *selectCountShare, int iaStartOffset[], int iaEndOffset[], __int64 iaAddr, __int64 iaTotalLoops, int iaIndepentFileId );
int  managerSelectCountDealResultLeft( SELECT_COUNT_SHARE *selectCountShare, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId );
int  managerSelectCountDealResultOther( SELECT_COUNT_SHARE *selectCountShare,  __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId );
int  managerSelectCountDealResultRight( SELECT_COUNT_SHARE *selectCountShare, __int64 iaAddr, __int64 iaSize, int iaEndOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId );
int  managerSelectCountDealResultMain( SELECT_COUNT_SHARE *selectCountShare, int iaStartOffset[], int iaEndOffset[], __int64 iaAddr, __int64 iaTotalLoops, int iaIndepentId );
int  managerSelectCountDealResultLeftMain( SELECT_COUNT_SHARE *selectCountShare, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId );
int  managerSelectCountDealResultOtherMain( SELECT_COUNT_SHARE *selectCountShare,  __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId );
int  managerSelectCountDealResultRightMain( SELECT_COUNT_SHARE *selectCountShare, __int64 iaAddr, __int64 iaSize, int iaEndOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId );

/**********************************************************************
函数名称: managerSelectCountDeal
函数功能: 管理端处理 select count 操作
参    数：
第    一：SOCKET               I
第    二：数据开始地址         I
第    三：数据结束地址         I
作    者: 李高文
建立时间: 20161022
**********************************************************************/
void managerSelectCountDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd )
{
/*
接收数据结构：长度+操作+值区间（是否有条件、左开右开、左开右闭、左闭右开、左闭右闭）+KEY开始值长度+KEY开始值+KEY结束值长度+KEY结束值
返回数据结构：操作结果+COUNT值
*/
#define MANAGER_SELECT_COUNT_DEAL_SEND_ERROR( value ) \
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
}
	char pcResult[INT_LEN + 1];
	char cResult;
	char pcSendBuf[INT64_LEN + 1 + 1];
	char pcKeyStartLen[KEY_VALUE_LENGTH + 1];
  char pcKeyEndLen[KEY_VALUE_LENGTH + 1];
  char pcKeyStart[MAX_KEY_LEN + 1];
  char pcKeyEnd[MAX_KEY_LEN + 1];
  char pcValueType[KEY_VALUE_LENGTH + 1];
  char pcSendLen[DATA_BUF_LEN + 1];
  char pcRecvLen[DATA_BUF_LEN + 1];
  int  iKeyStartLen;
	int  iKeyEndLen;
	int  iRet;
	char *pcBufStart;
	char *pcBufEnd;
	SELECT_COUNT_SHARE selectStruct;
	char pcOperate[OPERATE_LEN + 1];
	char pcTotalRows[INT64_LEN + 1];
	int iRecvLen;
	int iTotalLen;
	int iDataLen;
	int iSendLen;
	int iTime;
	#ifdef SYSTEM_WINDOWS_64
	  void *threadHandle;                /*windows下的线程句柄*/
	  unsigned long ulWindowsThreadId;
	#else/*#ifdef SYSTEM_UBUNTU*/
	  pthread_t threadId;                /*其他OS下的线程ID*/
	#endif
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
	/*取值区间*/
	memset( pcValueType, 0x00, sizeof(pcValueType) );
	memcpy( pcValueType, pcBufStart, KEY_VALUE_LENGTH );
	pcBufStart += KEY_VALUE_LENGTH;
		
	/*取KEY开始值长度*/
	memset( pcKeyStartLen, 0x00, sizeof(pcKeyStartLen) );
	memcpy( pcKeyStartLen, pcBufStart, KEY_VALUE_LENGTH );
	pcBufStart += KEY_VALUE_LENGTH;
		
	/*取KEY开始值*/
	iKeyStartLen = atoi( pcKeyStartLen );
	memset( pcKeyStart, 0x00, sizeof(pcKeyStart) );
	memcpy( pcKeyStart, pcBufStart, iKeyStartLen );
	pcBufStart += iKeyStartLen;
	
	//logFmtSyn( LVNOR, "pcKeyStart[%s]", pcKeyStart );
	
	/*取KEY结束值长度*/
	memset( pcKeyEndLen, 0x00, sizeof(pcKeyEndLen) );
	memcpy( pcKeyEndLen, pcBufStart, KEY_VALUE_LENGTH );
	pcBufStart += KEY_VALUE_LENGTH;
	
	//logFmtSyn( LVNOR, "pcKeyEndLen[%s]", pcKeyEndLen );
	
	/*取KEY结束值*/
	iKeyEndLen = atoi( pcKeyEndLen );
	memset( pcKeyEnd, 0x00, sizeof(pcKeyEnd) );
	memcpy( pcKeyEnd, pcBufStart, iKeyEndLen );
	pcBufStart += iKeyEndLen;
	
	//logFmtSyn( LVNOR, "pcKeyEnd[%s]", pcKeyEnd );
	
	/*设置与子线程共享数据*/
	memset( &selectStruct, 0x00, sizeof(selectStruct) );
	memcpy( selectStruct.pcValueType, pcValueType, KEY_VALUE_LENGTH ); /*搜索条件的值区间（KEY或VALUE 左开右开、左开右闭、左闭右开、左闭右闭*/
	memcpy( selectStruct.pcKeyStart, pcKeyStart, iKeyStartLen );       /*KEY 的开始值*/
	selectStruct.iKeyStartLen = iKeyStartLen;                          /*KEY 的开始值长度*/
	memcpy( selectStruct.pcKeyEnd, pcKeyEnd, iKeyEndLen );             /*KEY 的结束值*/
	selectStruct.iKeyEndLen = iKeyEndLen;                              /*KEY 的结束值长度*/
	selectStruct.iState = RUN_STATE_UNFINISHED;                        /*子线程运行状态*/
	selectStruct.iCmd = RUN_CMD_RUN;                                   /*子线程运行命令*/
	
	MANAGER_RDLOCK();

	#ifdef SYSTEM_WINDOWS_64
    threadHandle = CreateThread( NULL, 0, (void *)(&managerSelectCountDealChildThread), &selectStruct, 0, &ulWindowsThreadId );
    if (sgManagerThread.threadHandle == NULL)
    {
    	MANAGER_SELECT_COUNT_DEAL_SEND_ERROR( ERROR );
    	MANAGER_RDUNLOCK();
      return;
    }
  #else/*#ifdef SYSTEM_UBUNTU*/
    if (pthread_create(&threadId, NULL, (void *)(&managerSelectCountDealChildThread), &selectStruct) != 0)
    {
  	  MANAGER_SELECT_COUNT_DEAL_SEND_ERROR( ERROR );
  	  MANAGER_RDUNLOCK();
      return;
    }
  
    /*线程分离，结束时自动回收资源*/
    pthread_detach(threadId);
  #endif
  
  /*循环等待输入或者检查线程执行情况*/
  iTime = 0;
	while (iTime < EACH_JOB_TIME * 10) /*EACH_JOB_TIME 的单位是秒，每次睡眠0.1秒*/
	{
		//logFmtSyn( LVNOR, "selectStruct.iState[%d]", selectStruct.iState );
		
		/*如果子线程已完成，表示子线程正常执行完成，并退出*/
		if (selectStruct.iState == RUN_STATE_FINISHED)
		{
			break;
		}
		else
		{
			usleep( 100*1000 );
		}
		iTime++;
	}
	
	/*给子线程发送终止命令*/
	selectStruct.iCmd = RUN_CMD_EXIT;
	
	/*等待子线程执行结束*/
	while (selectStruct.iState != RUN_STATE_FINISHED)
	{
		THREAD_THREAD_SLEEP();
	}
	
	MANAGER_RDUNLOCK();
	
	memset( pcTotalRows, 0x00, sizeof(pcTotalRows) );
  sprintf( pcTotalRows, "%0*lld", INT64_LEN, selectStruct.llTotalRows );
  
  /*组发送数据*/
  memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
  
  sprintf( &cResult, "%d", SUCCESS );
  memcpy( pcSendBuf, &cResult, 1 );
  memcpy( pcSendBuf + 1, pcTotalRows, INT64_LEN );
  
  iSendLen = 1 + INT64_LEN;
  
  memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
  
  send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
  send( iaSocket, pcSendBuf, iSendLen, 0 );
  
  logFmtSyn( LVNOR, "返回数据成功[%s][%s]", pcSendLen, pcSendBuf );
}

/**********************************************************************
函数名称: managerSelectCountDealChildThread
函数功能: 管理端处理 select count 操作
参    数：
第    一：共享结构体               I/O
作    者: 李高文
建立时间: 20161106
**********************************************************************/
void managerSelectCountDealChildThread( SELECT_COUNT_SHARE *selectCountShare )
{
  int  iStartOffsetRecord[20];   /*开始位置记录*/
	int  iEndOffsetRecord[20];     /*结束位置记录*/
	__int64  llDisc;
	__int64  llSize;
	__int64  llLoops;
	int  iRet;
	int  iIndepentId;
	int  iIndepentFileId;
		
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
	    if (!strcmp( selectCountShare->pcValueType, KEY_OPEN_OPEN ) != !strcmp( selectCountShare->pcValueType, KEY_OPEN_CLOSE ))
      {
    	  iRet = managerPositionLeftOpenMain( iStartOffsetRecord, selectCountShare->pcKeyStart, llDisc, llSize, 1, llLoops, iIndepentId );

    	  /*如果没有满足条件*/
    	  if (iRet == NOT_FOUND)
    	  {
    	  	//logFmtSyn( LVNOR, "没有满足左位置的条件" );
          continue ;
    	  }
      }
      else
      {
    	  iRet = managerPositionLeftCloseMain( iStartOffsetRecord, selectCountShare->pcKeyStart, llDisc, llSize, 1, llLoops, iIndepentId );

    	  /*如果没有满足条件*/
    	  if (iRet == NOT_FOUND)
    	  {
    	  	//logFmtSyn( LVNOR, "没有满足左位置的条件" );
          continue ;
    	  }
      }
	  }

	  /*定位结束位置*/
	  {
	  	/*初始化结束位置记录变量*/
	    memset( &iEndOffsetRecord, 0x00, sizeof(iEndOffsetRecord) );
	  
	  	/*如果是右开*/
	    if (!strcmp( selectCountShare->pcValueType, KEY_OPEN_OPEN ) != !strcmp( selectCountShare->pcValueType, KEY_CLOSE_OPEN ))
      {
    	  iRet = managerPositionRightOpenMain( iEndOffsetRecord, selectCountShare->pcKeyEnd, llDisc, llSize, 1, llLoops, iIndepentId );
    	  
    	  /*如果没有满足条件*/
    	  if (iRet == NOT_FOUND)
    	  {
    	  	
    	  	//logFmtSyn( LVNOR, "没有满足右位置的条件" );
          continue ;
    	  }
      }
      else
      {
    	  iRet = managerPositionRightCloseMain( iEndOffsetRecord, selectCountShare->pcKeyEnd, llDisc, llSize, 1, llLoops, iIndepentId );
    	  
    	  /*如果没有满足条件*/
    	  if (iRet == NOT_FOUND)
    	  {
    	  	//logFmtSyn( LVNOR, "没有满足右位置的条件" );
          continue ;
    	  }
      }
	  }

	  /*计算数量*/
	  iRet = managerSelectCountDealResultMain( selectCountShare, iStartOffsetRecord, iEndOffsetRecord, llDisc, llLoops, iIndepentId );
	  
	  if (iRet == INTERRUPT)
	  {
	  	break;
	  }
  }

	for (iIndepentFileId = 0; iIndepentFileId < INDEPENDENT_FILE_SIZE; iIndepentFileId++)
	{
		/*这个结果是上面的*/
		if (iRet == INTERRUPT)
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
	    if (!strcmp( selectCountShare->pcValueType, KEY_OPEN_OPEN ) != !strcmp( selectCountShare->pcValueType, KEY_OPEN_CLOSE ))
      {
    	  iRet = managerPositionLeftOpen( iStartOffsetRecord, selectCountShare->pcKeyStart, llDisc, llSize, 1, llLoops, iIndepentFileId );
    	  
    	  /*如果没有满足条件*/
    	  if (iRet == NOT_FOUND)
    	  {
    	  	//logFmtSyn( LVNOR, "没有满足左位置的条件" );
          continue ;
    	  }
      }
      else
      {
    	  iRet = managerPositionLeftClose( iStartOffsetRecord, selectCountShare->pcKeyStart, llDisc, llSize, 1, llLoops, iIndepentFileId );
    	  
    	  /*如果没有满足条件*/
    	  if (iRet == NOT_FOUND)
    	  {
    	  	//logFmtSyn( LVNOR, "没有满足左位置的条件" );
          continue ;
    	  }
      }
	  }

	  /*定位结束位置*/
	  {
	  	/*初始化结束位置记录变量*/
	    memset( &iEndOffsetRecord, 0x00, sizeof(iEndOffsetRecord) );
	  
	  	/*如果是右开*/
	    if (!strcmp( selectCountShare->pcValueType, KEY_OPEN_OPEN ) != !strcmp( selectCountShare->pcValueType, KEY_CLOSE_OPEN ))
      {
    	  iRet = managerPositionRightOpen( iEndOffsetRecord, selectCountShare->pcKeyEnd, llDisc, llSize, 1, llLoops, iIndepentFileId );
    	  
    	  /*如果没有满足条件*/
    	  if (iRet == NOT_FOUND)
    	  {
    	  	
    	  	//logFmtSyn( LVNOR, "没有满足右位置的条件" );
          continue ;
    	  }
      }
      else
      {
    	  iRet = managerPositionRightClose( iEndOffsetRecord, selectCountShare->pcKeyEnd, llDisc, llSize, 1, llLoops, iIndepentFileId );
    	  
    	  /*如果没有满足条件*/
    	  if (iRet == NOT_FOUND)
    	  {
    	  	//logFmtSyn( LVNOR, "没有满足右位置的条件" );
          continue ;
    	  }
      }
	  }

	  /*计算数量*/
	  iRet = managerSelectCountDealResult( selectCountShare, iStartOffsetRecord, iEndOffsetRecord, llDisc, llLoops, iIndepentFileId );
  }
	selectCountShare->iState = RUN_STATE_FINISHED;
	return;
}

/**********************************************************************
函数名称: managerSelectCountDealResultOther
函数功能: 管理端处理 select count 中的数量计算
参    数：
第    一：线程共享变量     I/O
第    二：开始位置         I
第    三：结束位置         I
第    四：数据地址         I
第    五：总级数           I
返    回：
作    者: 李高文
建立时间: 20161106
**********************************************************************/
int managerSelectCountDealResult( SELECT_COUNT_SHARE *selectCountShare, int iaStartOffset[], int iaEndOffset[], __int64 iaAddr, __int64 iaTotalLoops, int iaIndepentFileId )
{
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	KEY_POINT *key;                /*其他级数据中的数据结构*/
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
  if (iIndex >= iaTotalLoops)
  {
  	selectCountShare->llTotalRows += 1;
  	return SUCCESS;
  }
  
  /*如果只有最后一级数据不同，计算数据量，返回*/
  if (iIndex == iaTotalLoops)
  {
  	selectCountShare->llTotalRows += iaEndOffset[iIndex] - iaStartOffset[iIndex] + 1;
  	return SUCCESS;
  }

  /*取出当前级的数据*/
  iAddr = iaAddr;
  for (iTmp = 1; iTmp < iIndex; iTmp++)
  {
  	memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iAddr, iaIndepentFileId );
    
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iTmp] * sizeof(KEY_POINT));
    
    iAddr = key->llChildAddr;
  }

  memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
  getKeyBlockData( pcDataBuf, iAddr, iaIndepentFileId );

  /*从该级开始，开始位置和结束位置不同*/

  /*处理开始位置的级*/
  key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iIndex] * sizeof(KEY_POINT));
  iRet = managerSelectCountDealResultLeft( selectCountShare, key->llChildAddr, key->llChildSize, iaStartOffset, iIndex + 1, iaTotalLoops, iaIndepentFileId );
  /*如果被中断结束*/
  if (iRet)
  {
  	return iRet;
  }

  /*处理其他位置的级*/
  for (iTmp3 = iaStartOffset[iIndex] + 1; iTmp3 < iaEndOffset[iIndex]; iTmp3++)
  {
    key = (KEY_POINT*)(pcDataBuf + iTmp3 * sizeof(KEY_POINT));
    iRet = managerSelectCountDealResultOther( selectCountShare, key->llChildAddr, key->llChildSize, iIndex + 1, iaTotalLoops, iaIndepentFileId );
    if (iRet)
    {
  	  return iRet;
    }
  }

  /*处理结束位置的级*/
  key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iIndex] * sizeof(KEY_POINT));
  iRet = managerSelectCountDealResultRight( selectCountShare, key->llChildAddr, key->llChildSize, iaEndOffset, iIndex + 1, iaTotalLoops, iaIndepentFileId );

  return SUCCESS;
}

/**********************************************************************
函数名称: managerSelectCountDealResultLeft
函数功能: 管理端处理 select count 中的数量计算
参    数：
第    一：线程共享变量     I
第    二：数据地址         I
第    三：下一级数量       I
第    四：开始位置         I
第    五：级数             I
第    六：总级数           I
返    回：
作    者: 李高文
建立时间: 20161106
**********************************************************************/
int managerSelectCountDealResultLeft( SELECT_COUNT_SHARE *selectCountShare, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId )
{
	char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
	KEY_POINT *key;
	int  iTmp;
	int  iRet;

	if (selectCountShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}

	/*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
		selectCountShare->llTotalRows += iaSize - (__int64)iaStartOffset[iaLoop];
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iaLoop] * sizeof(KEY_POINT));
    iRet = managerSelectCountDealResultLeft( selectCountShare, key->llChildAddr, key->llChildSize, iaStartOffset, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
    if (iRet)
    {
    	return INTERRUPT;
    }

    for (iTmp = iaStartOffset[iaLoop] + 1; iTmp < iaSize; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerSelectCountDealResultOther( selectCountShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
    }
	}

	return SUCCESS;
}

/**********************************************************************
函数名称: managerSelectCountDealResultOther
函数功能: 管理端处理 select count 中的数量计算
参    数：
第    一：线程共享变量     I
第    二：数据地址         I
第    三：下一级数量       I
第    四：级数             I
第    五：总级数           I
返    回：
作    者: 李高文
建立时间: 20161106
**********************************************************************/
int managerSelectCountDealResultOther( SELECT_COUNT_SHARE *selectCountShare,  __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId )
{
	char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
	KEY_POINT *key;
	int  iTmp;
	int  iRet;
	
	if (selectCountShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}

	/*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
		selectCountShare->llTotalRows += iaSize;
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
    for (iTmp = 0; iTmp < iaSize; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerSelectCountDealResultOther( selectCountShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
    }
	}
	return SUCCESS;
}

/**********************************************************************
函数名称: managerSelectCountDealResultRight
函数功能: 管理端处理 select count 中的数量计算
参    数：
第    一：线程共享变量     I
第    二：数据地址         I
第    三：下一级数量       I
第    四：开始位置         I
第    五：级数             I
第    六：总级数           I
返    回：
作    者: 李高文
建立时间: 20161106
**********************************************************************/
int managerSelectCountDealResultRight( SELECT_COUNT_SHARE *selectCountShare, __int64 iaAddr, __int64 iaSize, int iaEndOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId )
{
	char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
	KEY_POINT *key;
	int  iTmp;
	int  iRet;
	
	//logFmtSyn( LVNOR, "开始执行函数[%s]", __FUNCTION__ );
	
	if (selectCountShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}
	
	/*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
		selectCountShare->llTotalRows += iaEndOffset[iaLoop] + 1;
	  return SUCCESS;
	}
	
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iaLoop] * sizeof(KEY_POINT));
    
    iRet = managerSelectCountDealResultRight( selectCountShare, key->llChildAddr, key->llChildSize, iaEndOffset, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
    if (iRet)
    {
    	return INTERRUPT;
    }
    
    for (iTmp = 0; iTmp < iaEndOffset[iaLoop]; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerSelectCountDealResultOther( selectCountShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
    }
	}
	
	return SUCCESS;
}

/**********************************************************************
函数名称: managerSelectCountDealResultMain
函数功能: 管理端处理 select count 中的数量计算
参    数：
第    一：线程共享变量     I/O
第    二：开始位置         I
第    三：结束位置         I
第    四：数据地址         I
第    五：总级数           I
返    回：
作    者: 李高文
建立时间: 20161106
**********************************************************************/
int managerSelectCountDealResultMain( SELECT_COUNT_SHARE *selectCountShare, int iaStartOffset[], int iaEndOffset[], __int64 iaAddr, __int64 iaTotalLoops, int iaIndepentId )
{
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	KEY_POINT *key;                /*其他级数据中的数据结构*/
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
  	selectCountShare->llTotalRows += 1;
  	return SUCCESS;
  }
  
  /*如果只有最后一级数据不同，计算数据量，返回*/
  if (iIndex == iaTotalLoops)
  {
  	selectCountShare->llTotalRows += iaEndOffset[iIndex] - iaStartOffset[iIndex] + 1;
  	return SUCCESS;
  }
  
  /*取出当前级的数据*/
  iAddr = iaAddr;
  for (iTmp = 1; iTmp < iIndex; iTmp++)
  {
  	memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iAddr, iaIndepentId );
    
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iTmp] * sizeof(KEY_POINT));
    
    iAddr = key->llChildAddr;
  }

  memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
  getKeyBlockDataMain( pcDataBuf, iAddr, iaIndepentId );
  
  /*从该级开始，开始位置和结束位置不同*/

  /*处理开始位置的级*/
  key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iIndex] * sizeof(KEY_POINT));
  iRet = managerSelectCountDealResultLeftMain( selectCountShare, key->llChildAddr, key->llChildSize, iaStartOffset, iIndex + 1, iaTotalLoops, iaIndepentId );
  /*如果被中断结束*/
  if (iRet)
  {
  	return iRet;
  }
  
  /*处理其他位置的级*/
  for (iTmp3 = iaStartOffset[iIndex] + 1; iTmp3 < iaEndOffset[iIndex]; iTmp3++)
  {
    key = (KEY_POINT*)(pcDataBuf + iTmp3 * sizeof(KEY_POINT));
    iRet = managerSelectCountDealResultOtherMain( selectCountShare, key->llChildAddr, key->llChildSize, iIndex + 1, iaTotalLoops, iaIndepentId );
    if (iRet)
    {
  	  return iRet;
    }
  }
  
  /*处理结束位置的级*/
  key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iIndex] * sizeof(KEY_POINT));
  iRet = managerSelectCountDealResultRightMain( selectCountShare, key->llChildAddr, key->llChildSize, iaEndOffset, iIndex + 1, iaTotalLoops, iaIndepentId );
  
  return SUCCESS;
}

/**********************************************************************
函数名称: managerSelectCountDealResultLeftMain
函数功能: 管理端处理 select count 中的数量计算
参    数：
第    一：线程共享变量     I
第    二：数据地址         I
第    三：下一级数量       I
第    四：开始位置         I
第    五：级数             I
第    六：总级数           I
返    回：
作    者: 李高文
建立时间: 20161106
**********************************************************************/
int managerSelectCountDealResultLeftMain( SELECT_COUNT_SHARE *selectCountShare, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId )
{
	char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
	KEY_POINT *key;
	int  iTmp;
	int  iRet;
	
	if (selectCountShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}
	
	/*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
		selectCountShare->llTotalRows += iaSize - (__int64)iaStartOffset[iaLoop];
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iaLoop] * sizeof(KEY_POINT));
    
    iRet = managerSelectCountDealResultLeftMain( selectCountShare, key->llChildAddr, key->llChildSize, iaStartOffset, iaLoop + 1, iaTotalLoops, iaIndepentId );
    if (iRet)
    {
    	return INTERRUPT;
    }
    
    for (iTmp = iaStartOffset[iaLoop] + 1; iTmp < iaSize; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerSelectCountDealResultOtherMain( selectCountShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
    }
	}
	
	return SUCCESS;
}

/**********************************************************************
函数名称: managerSelectCountDealResultOtherMain
函数功能: 管理端处理 select count 中的数量计算
参    数：
第    一：线程共享变量     I
第    二：数据地址         I
第    三：下一级数量       I
第    四：级数             I
第    五：总级数           I
返    回：
作    者: 李高文
建立时间: 20161106
**********************************************************************/
int managerSelectCountDealResultOtherMain( SELECT_COUNT_SHARE *selectCountShare,  __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId )
{
	char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
	KEY_POINT *key;
	int  iTmp;
	int  iRet;
	
	if (selectCountShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}
	
	/*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
		selectCountShare->llTotalRows += iaSize;
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    for (iTmp = 0; iTmp < iaSize; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerSelectCountDealResultOtherMain( selectCountShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
    }
	}
	
	return SUCCESS;
}

/**********************************************************************
函数名称: managerSelectCountDealResultRightMain
函数功能: 管理端处理 select count 中的数量计算
参    数：
第    一：线程共享变量     I
第    二：数据地址         I
第    三：下一级数量       I
第    四：开始位置         I
第    五：级数             I
第    六：总级数           I
返    回：
作    者: 李高文
建立时间: 20161106
**********************************************************************/
int managerSelectCountDealResultRightMain( SELECT_COUNT_SHARE *selectCountShare, __int64 iaAddr, __int64 iaSize, int iaEndOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId )
{
	char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
	KEY_POINT *key;
	int  iTmp;
	int  iRet;
	
	//logFmtSyn( LVNOR, "开始执行函数[%s]", __FUNCTION__ );
	
	if (selectCountShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}
	
	/*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
		selectCountShare->llTotalRows += iaEndOffset[iaLoop] + 1;
	  return SUCCESS;
	}
	
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iaLoop] * sizeof(KEY_POINT));
    
    iRet = managerSelectCountDealResultRightMain( selectCountShare, key->llChildAddr, key->llChildSize, iaEndOffset, iaLoop + 1, iaTotalLoops, iaIndepentId );
    if (iRet)
    {
    	return INTERRUPT;
    }
    
    for (iTmp = 0; iTmp < iaEndOffset[iaLoop]; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerSelectCountDealResultOtherMain( selectCountShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
    }
	}
	
	return SUCCESS;
}
