#define __KEY_VALUE_THREAD_MANAGER_UNLOAD_LOAD_C

/*
负责处理 MANAGER 中的 UNLOAD LOAD 语句
*/

#ifndef __COMMON_HEAD_H
  #include "commonHead.h"
#endif

void managerUnloadDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd );
void managerUnloadDealChildThread( UNLOAD_SHARE *unloadShare );
int  managerUnloadDealUnloadData( UNLOAD_SHARE *unloadShare, int iaStartOffset[], int iaEndOffset[], __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId );
int  managerUnloadDealGetDataLeft( UNLOAD_SHARE *unloadShare, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], int iaLoop, int iaTotalLoops, int iaIndepentFileId );
int  managerUnloadDealGetDataOther( UNLOAD_SHARE *unloadShare,  __int64 iaAddr, __int64 iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId );
int  managerUnloadDealGetDataRight( UNLOAD_SHARE *unloadShare, __int64 iaAddr, __int64 iaSize, int iaEndOffset[], int iaLoop, int iaTotalLoops, int iaIndepentFileId );
int  managerUnloadDealUnloadDataMain( UNLOAD_SHARE *unloadShare, int iaStartOffset[], int iaEndOffset[], __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId );
int  managerUnloadDealGetDataLeftMain( UNLOAD_SHARE *unloadShare, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], int iaLoop, int iaTotalLoops, int iaIndepentId );
int  managerUnloadDealGetDataOtherMain( UNLOAD_SHARE *unloadShare,  __int64 iaAddr, __int64 iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId );
int  managerUnloadDealGetDataRightMain( UNLOAD_SHARE *unloadShare, __int64 iaAddr, __int64 iaSize, int iaEndOffset[], int iaLoop, int iaTotalLoops, int iaIndepentId );
void managerLoadDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd );
void managerLoadDealChildThread( LOAD_SHARE *loadShare );

/**********************************************************************
函数名称: managerUnloadDeal
函数功能: 管理端处理 unload 操作
参    数：
第    一：SOCKET               I
第    二：数据开始地址         I
第    三：数据结束地址         I
作    者: 李高文
建立时间: 20161022
**********************************************************************/
void managerUnloadDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd )
{
/*
接收数据结构：长度+操作+文件名称+值区间（左开右开、左开右闭、左闭右开、左闭右闭）+KEY开始值长度+KEY开始值+KEY结束值长度+KEY结束值
返回数据结构：操作结果+导出数据总数量
*/
/*
如果导出10亿条数据怎么办
*/
#define MANAGER_UNLOAD_DEAL_SEND_ERROR( value ) \
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
	char pcFilePath[FILE_PATH_LEN + 1];
	char pcKeyStartLen[KEY_VALUE_LENGTH + 1];
  char pcKeyEndLen[KEY_VALUE_LENGTH + 1];
  char pcKeyStart[MAX_KEY_LEN + 1];
  char pcKeyEnd[MAX_KEY_LEN + 1];
  char pcValueType[KEY_VALUE_LENGTH + 1];
  char pcSendLen[DATA_BUF_LEN + 1];
  char pcRecvLen[DATA_BUF_LEN + 1];
  int  iKeyStartLen;
	int  iKeyEndLen;
	FILE *fp;
	int  iRet;
	char *pcBufStart;
	char *pcBufEnd;
	UNLOAD_SHARE unloadStruct;
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
	pcBufEnd = pcBufEnd;
	
	/*取文件路径*/
	memset( pcFilePath, 0x00, sizeof(pcFilePath) );
	memcpy( pcFilePath, pcBufStart, NAME_LEN );
	AllTrim( pcFilePath );
	pcBufStart += FILE_PATH_LEN;
	
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
	
	/*取KEY结束值长度*/
	memset( pcKeyEndLen, 0x00, sizeof(pcKeyEndLen) );
	memcpy( pcKeyEndLen, pcBufStart, KEY_VALUE_LENGTH );
	pcBufStart += KEY_VALUE_LENGTH;
	
	/*取KEY结束值*/
	iKeyEndLen = atoi( pcKeyEndLen );
	memset( pcKeyEnd, 0x00, sizeof(pcKeyEnd) );
	memcpy( pcKeyEnd, pcBufStart, iKeyEndLen );
	pcBufStart += iKeyEndLen;
	
	logFmtSyn( LVNOR, "pcFilePath[%s]pcValueType[%s]pcKeyStart[%s]pcKeyEnd[%s]", pcFilePath, pcValueType, pcKeyStart, pcKeyEnd );
	
	/*新建文件*/
	fp = fopen64( pcFilePath, "wb+" );
	if (fp == NULL)
	{
		MANAGER_UNLOAD_DEAL_SEND_ERROR( ERROR );
		return;
	}
	
	/*设置与子线程共享数据*/
	memset( &unloadStruct, 0x00, sizeof(unloadStruct) );
	unloadStruct.fp = fp;                                              /*文件指针*/
	memcpy( unloadStruct.pcValueType, pcValueType, KEY_VALUE_LENGTH ); /*搜索条件的值区间（KEY或VALUE 左开右开、左开右闭、左闭右开、左闭右闭*/
	memcpy( unloadStruct.pcKeyStart, pcKeyStart, iKeyStartLen );       /*KEY 的开始值*/
	unloadStruct.iKeyStartLen = iKeyStartLen;                          /*KEY 的开始值长度*/
	memcpy( unloadStruct.pcKeyEnd, pcKeyEnd, iKeyEndLen );             /*KEY 的结束值*/
	unloadStruct.iKeyEndLen = iKeyEndLen;                              /*KEY 的结束值长度*/
	unloadStruct.iState = RUN_STATE_UNFINISHED;                        /*子线程运行状态*/
	unloadStruct.iCmd = RUN_CMD_RUN;                                   /*子线程运行命令*/
	
	MANAGER_RDLOCK();
	
	#ifdef SYSTEM_WINDOWS_64
    threadHandle = CreateThread( NULL, 0, (void *)(&managerUnloadDealChildThread), &unloadStruct, 0, &ulWindowsThreadId );
    if (sgManagerThread.threadHandle == NULL)
    {
    	MANAGER_UNLOAD_DEAL_SEND_ERROR( ERROR );
    	MANAGER_RDUNLOCK();
      return;
    }
  #else/*#ifdef SYSTEM_UBUNTU*/
    if (pthread_create(&threadId, NULL, (void *)(&managerUnloadDealChildThread), &unloadStruct) != 0)
    {
  	  MANAGER_UNLOAD_DEAL_SEND_ERROR( ERROR );
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
		/*如果子线程已完成，表示子线程正常执行完成，并退出*/
		if (unloadStruct.iState == RUN_STATE_FINISHED)
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
	unloadStruct.iCmd = RUN_CMD_EXIT;
	
	/*等待子线程执行结束*/
	while (unloadStruct.iState != RUN_STATE_FINISHED)
	{
		THREAD_THREAD_SLEEP();
	}
	
	fclose(fp);
		
	MANAGER_RDUNLOCK();
	
	memset( pcTotalRows, 0x00, sizeof(pcTotalRows) );
  sprintf( pcTotalRows, "%0*lld", INT64_LEN, unloadStruct.llTotalRows );
  
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
	
	return;
}

/**********************************************************************
函数名称: managerUnloadDealChildThread
函数功能: 管理端处理 unload 操作
参    数：
第    一：共享结构体               I/O
作    者: 李高文
建立时间: 20161031
**********************************************************************/
void managerUnloadDealChildThread( UNLOAD_SHARE *unloadShare )
{
	FILE *fp;
  int  iStartOffsetRecord[20];   /*开始位置记录*/
	int  iEndOffsetRecord[20];     /*结束位置记录*/
	__int64  llDisc;
	__int64  llSize;
	__int64  llLoops;
	int  iRet;
	int  iIndepentId;
	int  iIndepentFileId;
	
	/*指向输出文件指针*/
	fp = unloadShare->fp;
	
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
	    if (!strcmp( unloadShare->pcValueType, KEY_OPEN_OPEN ) != !strcmp( unloadShare->pcValueType, KEY_OPEN_CLOSE ))
      {
    	  iRet = managerPositionLeftOpenMain( iStartOffsetRecord, unloadShare->pcKeyStart, llDisc, llSize, 1, llLoops, iIndepentId );
    	  
    	  /*如果没有满足条件*/
    	  if (iRet == NOT_FOUND)
    	  {
          continue ;
    	  }
      }
      else
      {
    	  iRet = managerPositionLeftCloseMain( iStartOffsetRecord, unloadShare->pcKeyStart, llDisc, llSize, 1, llLoops, iIndepentId );
    	  
    	  /*如果没有满足条件*/
    	  if (iRet == NOT_FOUND)
    	  {
          continue ;
    	  }
      }
	  }
	  
	  /*定位结束位置*/
	  {
	  	/*初始化结束位置记录变量*/
	    memset( iEndOffsetRecord, 0x00, sizeof(iEndOffsetRecord) );
	  
	  	/*如果是右开*/
	    if (!strcmp( unloadShare->pcValueType, KEY_OPEN_OPEN ) != !strcmp( unloadShare->pcValueType, KEY_CLOSE_OPEN ))
      {
    	  iRet = managerPositionRightOpenMain( iEndOffsetRecord, unloadShare->pcKeyEnd, llDisc, llSize, 1, llLoops, iIndepentId );
    	  
    	  /*如果没有满足条件*/
    	  if (iRet == NOT_FOUND)
    	  {
    	  	continue ;
    	  }
      }
      else
      {
    	  iRet = managerPositionRightCloseMain( iEndOffsetRecord, unloadShare->pcKeyEnd, llDisc, llSize, 1, llLoops, iIndepentId );
    	  
    	  /*如果没有满足条件*/
    	  if (iRet == NOT_FOUND)
    	  {
    	  	continue ;
    	  }
      }
	  }
    
	  /*导出数据*/
	  iRet = managerUnloadDealUnloadDataMain( unloadShare, iStartOffsetRecord, iEndOffsetRecord, llDisc, llSize, 1, llLoops, iIndepentId );
	  
	  if (iRet == INTERRUPT)
	  {
	    break;
	  }
	}
	
	for (iIndepentFileId = 0; iIndepentFileId < INDEPENDENT_FILE_SIZE; iIndepentFileId++)
	{
	  llDisc = sgBase[iIndepentFileId].llFirstDisc;
	  llSize = sgBase[iIndepentFileId].llFirstSize;
	  llLoops = sgBase[iIndepentFileId].llLoops;
	
	  /*定位开始位置*/
	  {
	  	/*初始化开始位置记录变量*/
	    memset( &iStartOffsetRecord, 0x00, sizeof(iStartOffsetRecord) );
	  
	  	/*如果是左开*/
	    if (!strcmp( unloadShare->pcValueType, KEY_OPEN_OPEN ) != !strcmp( unloadShare->pcValueType, KEY_OPEN_CLOSE ))
      {
    	  iRet = managerPositionLeftOpen( iStartOffsetRecord, unloadShare->pcKeyStart, llDisc, llSize, 1, llLoops, iIndepentFileId );
    	  
    	  /*如果没有满足条件*/
    	  if (iRet == NOT_FOUND)
    	  {
          continue ;
    	  }
      }
      else
      {
    	  iRet = managerPositionLeftClose( iStartOffsetRecord, unloadShare->pcKeyStart, llDisc, llSize, 1, llLoops, iIndepentFileId );
    	  
    	  /*如果没有满足条件*/
    	  if (iRet == NOT_FOUND)
    	  {
          continue ;
    	  }
      }
	  }
	  
	  /*定位结束位置*/
	  {
	  	/*初始化结束位置记录变量*/
	    memset( iEndOffsetRecord, 0x00, sizeof(iEndOffsetRecord) );
	  
	  	/*如果是右开*/
	    if (!strcmp( unloadShare->pcValueType, KEY_OPEN_OPEN ) != !strcmp( unloadShare->pcValueType, KEY_CLOSE_OPEN ))
      {
    	  iRet = managerPositionRightOpen( iEndOffsetRecord, unloadShare->pcKeyEnd, llDisc, llSize, 1, llLoops, iIndepentFileId );
    	  
    	  /*如果没有满足条件*/
    	  if (iRet == NOT_FOUND)
    	  {
    	  	continue ;
    	  }
      }
      else
      {
    	  iRet = managerPositionRightClose( iEndOffsetRecord, unloadShare->pcKeyEnd, llDisc, llSize, 1, llLoops, iIndepentFileId );
    	  
    	  /*如果没有满足条件*/
    	  if (iRet == NOT_FOUND)
    	  {
    	  	continue ;
    	  }
      }
	  }
    
	  /*导出数据*/
	  iRet = managerUnloadDealUnloadData( unloadShare, iStartOffsetRecord, iEndOffsetRecord, llDisc, llSize, 1, llLoops, iIndepentFileId );
	  
	  if (iRet == INTERRUPT)
	  {
	    break;
	  }
	}
	unloadShare->iState = RUN_STATE_FINISHED;
	return;
}

/**********************************************************************
函数名称: managerUnloadDealUnloadData
函数功能: 管理端处理 unload 中的数据导出
参    数：
第    一：线程共享变量     I/O
第    二：开始位置         I
第    三：结束位置         I
第    四：数据地址         I
第    五：数量             I
第    六：级数             I
第    七：总级数           I
返    回：
作    者: 李高文
建立时间: 20161106
**********************************************************************/
int managerUnloadDealUnloadData( UNLOAD_SHARE *unloadShare, int iaStartOffset[], int iaEndOffset[], __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId )
{
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	KEY_POINT *key;                /*其他级数据中的数据结构*/
	KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
  char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
  __int64 iAddr;
  int  iSize;
  int  iIndex;
  __int64 llTotalRows;
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
  	
  	/*从 VALUE 文件中获取 KEY 的值*/
    valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
    pcKey[keyData->llKeyLen] = 0x00;
    
    /*从 VALUE 文件中获取 VALUE 的值*/
    valueToBuf( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentFileId );
    pcValue[keyData->llValueLen] = 0x00;
  	
  	fprintf( unloadShare->fp, "\'%s\' \'%s\'\n", pcKey, pcValue );
  	
  	(unloadShare->llTotalRows)++;
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
      
  /*获取数据*/
  {
    /*处理开始位置的级*/
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iIndex] * sizeof(KEY_POINT));
    iRet = managerUnloadDealGetDataLeft( unloadShare, key->llChildAddr, key->llChildSize, iaStartOffset, iIndex + 1, iaTotalLoops, iaIndepentFileId );
    /*如果被中断结束*/
    if (iRet)
    {
    	return iRet;
    }
    
    /*处理其他位置的级*/
    for (iTmp3 = iaStartOffset[iIndex] + 1; iTmp3 < iaEndOffset[iIndex]; iTmp3++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp3 * sizeof(KEY_POINT));
      iRet = managerUnloadDealGetDataOther( unloadShare, key->llChildAddr, key->llChildSize, iIndex + 1, iaTotalLoops, iaIndepentFileId );
      if (iRet)
      {
    	  return iRet;
      }
    }
    
    /*处理结束位置的级*/
    key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iIndex] * sizeof(KEY_POINT));
    iRet = managerUnloadDealGetDataRight( unloadShare, key->llChildAddr, key->llChildSize, iaEndOffset, iIndex + 1, iaTotalLoops, iaIndepentFileId );
    if (iRet)
    {
    	return iRet;
    }
  }
  
  return SUCCESS;
}

/**********************************************************************
函数名称: managerUnloadDealGetDataLeft
函数功能: 管理端处理 unload 中的导出数据
参    数：
第    一：线程共享变量     I
第    二：数据地址         I
第    三：下一级数量       I
第    四：开始位置         I
第    五：级数             I
第    六：总级数           I
返    回：
作    者: 李高文
建立时间: 20161112
**********************************************************************/
int managerUnloadDealGetDataLeft( UNLOAD_SHARE *unloadShare, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], int iaLoop, int iaTotalLoops, int iaIndepentFileId )
{
	char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
	KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
	KEY_POINT *key;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iTmp;
	int  iRet;
	
	if (unloadShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}
	
	/*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
		for (iTmp = iaStartOffset[iaLoop]; iTmp < iaSize; iTmp++ )
	  {
	    keyData = (KEY_VALUE_POINT*)(pcDataBuf + iTmp * sizeof(KEY_VALUE_POINT));
	    
	    /*从 VALUE 文件中获取 KEY 的值*/
      valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
      pcKey[keyData->llKeyLen] = 0x00;
      
      /*从 VALUE 文件中获取 VALUE 的值*/
      valueToBuf( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentFileId );
      pcValue[keyData->llValueLen] = 0x00;
  	        
      fprintf( unloadShare->fp, "\'%s\' \'%s\'\n", pcKey, pcValue );
      
      (unloadShare->llTotalRows)++;
	  }
	  
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iaLoop] * sizeof(KEY_POINT));
    
    iRet = managerUnloadDealGetDataLeft( unloadShare, key->llChildAddr, key->llChildSize, iaStartOffset, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
    if (iRet)
    {
    	return INTERRUPT;
    }
    
    for (iTmp = iaStartOffset[iaLoop] + 1; iTmp < iaSize; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerUnloadDealGetDataOther( unloadShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
    }
	}
	return SUCCESS;
}

/**********************************************************************
函数名称: managerUnloadDealGetDataOther
函数功能: 管理端处理 unload 中的导出数据
参    数：
第    一：线程共享变量     I
第    二：数据地址         I
第    三：下一级数量       I
第    四：级数             I
第    五：总级数           I
返    回：
作    者: 李高文
建立时间: 20161112
**********************************************************************/
int managerUnloadDealGetDataOther( UNLOAD_SHARE *unloadShare,  __int64 iaAddr, __int64 iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId )
{
	char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
	KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
	KEY_POINT *key;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iTmp;
	int  iRet;
	
	if (unloadShare->iCmd != RUN_CMD_RUN)
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
	    
	    /*从 VALUE 文件中获取 KEY 的值*/
      valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
      pcKey[keyData->llKeyLen] = 0x00;
      
      /*从 VALUE 文件中获取 VALUE 的值*/
      valueToBuf( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentFileId );
      pcValue[keyData->llValueLen] = 0x00;
      
      fprintf( unloadShare->fp, "\'%s\' \'%s\'\n", pcKey, pcValue );
      
      (unloadShare->llTotalRows)++;
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
      iRet = managerUnloadDealGetDataOther( unloadShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
    }
	}
	return SUCCESS;
}

/**********************************************************************
函数名称: managerUnloadDealGetDataRight
函数功能: 管理端处理 unload 中的导出数据
参    数：
第    一：线程共享变量     I
第    二：数据地址         I
第    三：下一级数量       I
第    四：开始位置         I
第    五：级数             I
第    六：总级数           I
返    回：
作    者: 李高文
建立时间: 20161112
**********************************************************************/
int managerUnloadDealGetDataRight( UNLOAD_SHARE *unloadShare, __int64 iaAddr, __int64 iaSize, int iaEndOffset[], int iaLoop, int iaTotalLoops, int iaIndepentFileId )
{
	char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
	KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
	KEY_POINT *key;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iTmp;
	int  iRet;
	
	if (unloadShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}
	
	/*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
	  memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
		for (iTmp = 0; iTmp <= iaEndOffset[iaLoop]; iTmp++ )
	  {
	    keyData = (KEY_VALUE_POINT*)(pcDataBuf + iTmp * sizeof(KEY_VALUE_POINT));
	      	
      /*从 VALUE 文件中获取 KEY 的值*/
      valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
      pcKey[keyData->llKeyLen] = 0x00;
      
      /*从 VALUE 文件中获取 VALUE 的值*/
      valueToBuf( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentFileId );
      pcValue[keyData->llValueLen] = 0x00;
      
      fprintf( unloadShare->fp, "\'%s\' \'%s\'\n", pcKey, pcValue );
      
      (unloadShare->llTotalRows)++;
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
      iRet = managerUnloadDealGetDataOther( unloadShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
    }
    
    key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iaLoop] * sizeof(KEY_POINT));
    iRet = managerUnloadDealGetDataRight( unloadShare, key->llChildAddr, key->llChildSize, iaEndOffset, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
    if (iRet)
    {
    	return INTERRUPT;
    }
	}
	return SUCCESS;
}

/**********************************************************************
函数名称: managerUnloadDealUnloadDataMain
函数功能: 管理端处理 unload 中的数据导出
参    数：
第    一：线程共享变量     I/O
第    二：开始位置         I
第    三：结束位置         I
第    四：数据地址         I
第    五：数量             I
第    六：级数             I
第    七：总级数           I
返    回：
作    者: 李高文
建立时间: 20161106
**********************************************************************/
int managerUnloadDealUnloadDataMain( UNLOAD_SHARE *unloadShare, int iaStartOffset[], int iaEndOffset[], __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId )
{
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	KEY_POINT *key;                /*其他级数据中的数据结构*/
	KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
  char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
  __int64 iAddr;
  int  iSize;
  int  iIndex;
  __int64 llTotalRows;
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
  	
  	/*从 VALUE 文件中获取 KEY 的值*/
    valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
    pcKey[keyData->llKeyLen] = 0x00;
    
    /*从 VALUE 文件中获取 VALUE 的值*/
    valueToBufMain( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentId );
    pcValue[keyData->llValueLen] = 0x00;
  	
  	fprintf( unloadShare->fp, "\'%s\' \'%s\'\n", pcKey, pcValue );
  	
  	(unloadShare->llTotalRows)++;
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
      
  /*获取数据*/
  {
    /*处理开始位置的级*/
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iIndex] * sizeof(KEY_POINT));
    iRet = managerUnloadDealGetDataLeftMain( unloadShare, key->llChildAddr, key->llChildSize, iaStartOffset, iIndex + 1, iaTotalLoops, iaIndepentId );
    /*如果被中断结束*/
    if (iRet)
    {
    	return iRet;
    }
    
    /*处理其他位置的级*/
    for (iTmp3 = iaStartOffset[iIndex] + 1; iTmp3 < iaEndOffset[iIndex]; iTmp3++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp3 * sizeof(KEY_POINT));
      iRet = managerUnloadDealGetDataOtherMain( unloadShare, key->llChildAddr, key->llChildSize, iIndex + 1, iaTotalLoops, iaIndepentId );
      if (iRet)
      {
    	  return iRet;
      }
    }
    
    /*处理结束位置的级*/
    key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iIndex] * sizeof(KEY_POINT));
    iRet = managerUnloadDealGetDataRightMain( unloadShare, key->llChildAddr, key->llChildSize, iaEndOffset, iIndex + 1, iaTotalLoops, iaIndepentId );
    if (iRet)
    {
    	return iRet;
    }
  }
  
  return SUCCESS;
}

/**********************************************************************
函数名称: managerUnloadDealGetDataLeftMain
函数功能: 管理端处理 unload 中的导出数据
参    数：
第    一：线程共享变量     I
第    二：数据地址         I
第    三：下一级数量       I
第    四：开始位置         I
第    五：级数             I
第    六：总级数           I
返    回：
作    者: 李高文
建立时间: 20161112
**********************************************************************/
int managerUnloadDealGetDataLeftMain( UNLOAD_SHARE *unloadShare, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], int iaLoop, int iaTotalLoops, int iaIndepentId )
{
	char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
	KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
	KEY_POINT *key;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iTmp;
	int  iRet;
	
	if (unloadShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}
	
	/*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
		for (iTmp = iaStartOffset[iaLoop]; iTmp < iaSize; iTmp++ )
	  {
	    keyData = (KEY_VALUE_POINT*)(pcDataBuf + iTmp * sizeof(KEY_VALUE_POINT));
	    
	    /*从 VALUE 文件中获取 KEY 的值*/
      valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
      pcKey[keyData->llKeyLen] = 0x00;
      
      /*从 VALUE 文件中获取 VALUE 的值*/
      valueToBufMain( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentId );
      pcValue[keyData->llValueLen] = 0x00;
  	        
      fprintf( unloadShare->fp, "\'%s\' \'%s\'\n", pcKey, pcValue );
      
      (unloadShare->llTotalRows)++;
	  }
	  
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iaLoop] * sizeof(KEY_POINT));
    
    iRet = managerUnloadDealGetDataLeftMain( unloadShare, key->llChildAddr, key->llChildSize, iaStartOffset, iaLoop + 1, iaTotalLoops, iaIndepentId );
    if (iRet)
    {
    	return INTERRUPT;
    }
    
    for (iTmp = iaStartOffset[iaLoop] + 1; iTmp < iaSize; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerUnloadDealGetDataOtherMain( unloadShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
    }
	}
	return SUCCESS;
}

/**********************************************************************
函数名称: managerUnloadDealGetDataOtherMain
函数功能: 管理端处理 unload 中的导出数据
参    数：
第    一：线程共享变量     I
第    二：数据地址         I
第    三：下一级数量       I
第    四：级数             I
第    五：总级数           I
返    回：
作    者: 李高文
建立时间: 20161112
**********************************************************************/
int managerUnloadDealGetDataOtherMain( UNLOAD_SHARE *unloadShare,  __int64 iaAddr, __int64 iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId )
{
	char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
	KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
	KEY_POINT *key;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iTmp;
	int  iRet;
	
	if (unloadShare->iCmd != RUN_CMD_RUN)
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
	    
	    /*从 VALUE 文件中获取 KEY 的值*/
      valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
      pcKey[keyData->llKeyLen] = 0x00;
      
      /*从 VALUE 文件中获取 VALUE 的值*/
      valueToBufMain( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentId );
      pcValue[keyData->llValueLen] = 0x00;
      
      fprintf( unloadShare->fp, "\'%s\' \'%s\'\n", pcKey, pcValue );
      
      (unloadShare->llTotalRows)++;
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
      iRet = managerUnloadDealGetDataOtherMain( unloadShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
    }
	}
	return SUCCESS;
}

/**********************************************************************
函数名称: managerUnloadDealGetDataRightMain
函数功能: 管理端处理 unload 中的导出数据
参    数：
第    一：线程共享变量     I
第    二：数据地址         I
第    三：下一级数量       I
第    四：开始位置         I
第    五：级数             I
第    六：总级数           I
返    回：
作    者: 李高文
建立时间: 20161112
**********************************************************************/
int managerUnloadDealGetDataRightMain( UNLOAD_SHARE *unloadShare, __int64 iaAddr, __int64 iaSize, int iaEndOffset[], int iaLoop, int iaTotalLoops, int iaIndepentId )
{
	char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
	KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
	KEY_POINT *key;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iTmp;
	int  iRet;
	
	if (unloadShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}
	
	/*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
	  memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
		for (iTmp = 0; iTmp <= iaEndOffset[iaLoop]; iTmp++ )
	  {
	    keyData = (KEY_VALUE_POINT*)(pcDataBuf + iTmp * sizeof(KEY_VALUE_POINT));
	      	
      /*从 VALUE 文件中获取 KEY 的值*/
      valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
      pcKey[keyData->llKeyLen] = 0x00;
      
      /*从 VALUE 文件中获取 VALUE 的值*/
      valueToBufMain( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentId );
      pcValue[keyData->llValueLen] = 0x00;
      
      fprintf( unloadShare->fp, "\'%s\' \'%s\'\n", pcKey, pcValue );
      
      (unloadShare->llTotalRows)++;
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
      iRet = managerUnloadDealGetDataOtherMain( unloadShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
    }
    
    key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iaLoop] * sizeof(KEY_POINT));
    iRet = managerUnloadDealGetDataRightMain( unloadShare, key->llChildAddr, key->llChildSize, iaEndOffset, iaLoop + 1, iaTotalLoops, iaIndepentId );
    if (iRet)
    {
    	return INTERRUPT;
    }
	}
	return SUCCESS;
}

/**********************************************************************
函数名称: managerLoadDeal
函数功能: 管理端处理 load 操作
参    数：
第    一：SOCKET               I
第    二：数据开始地址         I
第    三：数据结束地址         I
作    者: 李高文
建立时间: 20161022
**********************************************************************/
void managerLoadDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd )
{
/*
接收数据结构：长度+操作+文件路径+用户名+密码
返回数据结构：操作结果+导入数据总数量+导入数量+相同key数量+失败数量
*/
#define MANAGER_LOAD_DEAL_SEND_ERROR( value ) \
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
	char pcFilePath[FILE_PATH_LEN + 1];
	char pcUser[NAME_LEN + 1];
	char pcPass[NAME_LEN + 1];
	char pcKeysTotal[INT64_LEN + 1];
	char pcKeysLoaded[INT64_LEN + 1];
	char pcKeysUpdate[INT64_LEN + 1];
	char pcKeysFailed[INT64_LEN + 1];
	char pcSendBuf[1 + INT64_LEN + INT64_LEN + INT64_LEN + INT64_LEN + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	char pcRecvLen[DATA_BUF_LEN + 1];
	FILE *fp;
	__int64 llKeyValueSize;
	__int64 llKeysTotal;
	__int64 llKeysLoaded;
	__int64 llKeysUpdate;
	__int64 llKeysFailed;
	__int64 llSize;
	int  iKeyLen;
	int  iValueLen;
	int  iSendLen;
	int  iRet;
	int  iTmp;
	char *pcBufStart;
	char *pcBufEnd;
	LOAD_SHARE loadStruct;
	char pcOperate[OPERATE_LEN + 1];
	int iRecvLen;
	int iTotalLen;
	int iDataLen;
	int iTime;
	#ifdef SYSTEM_WINDOWS_64
	  void *threadHandle;                /*windows下的线程句柄*/
	  unsigned long ulWindowsThreadId;
	#else/*#ifdef SYSTEM_UBUNTU*/
	  pthread_t threadId;                /*其他OS下的线程ID*/
	#endif
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcBufEnd;
	
	/*取文件路径*/
	memset( pcFilePath, 0x00, sizeof(pcFilePath) );
	memcpy( pcFilePath, pcBufStart, NAME_LEN );
	AllTrim( pcFilePath );
	pcBufStart += FILE_PATH_LEN;
	
	/*取用户名*/
	memset( pcUser, 0x00, sizeof(pcUser) );
	memcpy( pcUser, pcBufStart, NAME_LEN );
	AllTrim( pcUser );
	pcBufStart += NAME_LEN;
	
	/*取密码*/
	memset( pcPass, 0x00, sizeof(pcPass) );
	memcpy( pcPass, pcBufStart, NAME_LEN );
	AllTrim( pcPass );
	pcBufStart += NAME_LEN;
	
	MANAGER_WRLOCK();
	
	/*取得当前key的数量*/  
  llKeyValueSize = 0;
  for (iTmp = 0; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
  {
  	llKeyValueSize += sgBase[iTmp].llKeyValueSize;
  }
	
	MANAGER_WRUNLOCK();
	
	/*检查用户名和密码是否匹配*/
	if (strcmp( pcgUser, pcUser ) || strcmp( pcgPass, pcPass ))
	{
		MANAGER_LOAD_DEAL_SEND_ERROR( ERROR );
		MANAGER_WRUNLOCK();
		return;
	}
	
	/*打开文件*/
	fp = fopen64( pcFilePath, "rb" );
	if (fp == NULL)
	{
		MANAGER_LOAD_DEAL_SEND_ERROR( ERROR );
		return;
	}
	
	/*设置与子线程共享数据*/
	memset( &loadStruct, 0x00, sizeof(loadStruct) );
	loadStruct.fp = fp;                        /*文件指针*/
	loadStruct.iState = RUN_STATE_UNFINISHED;  /*子线程运行状态*/
	loadStruct.iCmd = RUN_CMD_RUN;             /*子线程运行命令*/
	
	#ifdef SYSTEM_WINDOWS_64
    threadHandle = CreateThread( NULL, 0, (void *)(&managerLoadDealChildThread), &loadStruct, 0, &ulWindowsThreadId );
    if (sgManagerThread.threadHandle == NULL)
    {
    	MANAGER_SELECT_COUNT_DEAL_SEND_ERROR( ERROR );
    	MANAGER_WRUNLOCK();
      return;
    }
  #else/*#ifdef SYSTEM_UBUNTU*/
    if (pthread_create(&threadId, NULL, (void *)(&managerLoadDealChildThread), &loadStruct) != 0)
    {
  	  MANAGER_SELECT_COUNT_DEAL_SEND_ERROR( ERROR );
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
		if (loadStruct.iState == RUN_STATE_FINISHED)
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
	loadStruct.iCmd = RUN_CMD_EXIT;
	
	/*等待子线程执行结束*/
	while (loadStruct.iState != RUN_STATE_FINISHED)
	{
		THREAD_THREAD_SLEEP();
	}
	
	fclose(fp);
	
	llKeysTotal = loadStruct.llTotalRows;
	llKeysFailed = loadStruct.llTotalFail;
	
	/*计算导入数量*/
	llSize = 0;
  for (iTmp = 0; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
  {
  	llSize += sgBase[iTmp].llKeyValueSize;
  }
	llKeysLoaded = llSize - llKeyValueSize;
	
	/*计算更新数量*/
	llKeysUpdate = llKeysTotal - llKeysLoaded - llKeysFailed;
	
	MANAGER_WRUNLOCK();
	
	memset( pcKeysTotal, 0x00, sizeof(pcKeysTotal) );
  sprintf( pcKeysTotal, "%0*lld", INT64_LEN, llKeysTotal );
  
  memset( pcKeysLoaded, 0x00, sizeof(pcKeysLoaded) );
  sprintf( pcKeysLoaded, "%0*lld", INT64_LEN, llKeysLoaded );
  
  memset( pcKeysUpdate, 0x00, sizeof(pcKeysUpdate) );
  sprintf( pcKeysUpdate, "%0*lld", INT64_LEN, llKeysUpdate );
  
  memset( pcKeysFailed, 0x00, sizeof(pcKeysFailed) );
  sprintf( pcKeysFailed, "%0*lld", INT64_LEN, llKeysFailed );
  
  /*组发送数据*/
  memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
  
  sprintf( &cResult, "%d", SUCCESS );
  memcpy( pcSendBuf, &cResult, 1 );
  memcpy( pcSendBuf + 1, pcKeysTotal, INT64_LEN );
  memcpy( pcSendBuf + 1 + INT64_LEN, pcKeysLoaded, INT64_LEN );
  memcpy( pcSendBuf + 1 + INT64_LEN + INT64_LEN, pcKeysUpdate, INT64_LEN );
  memcpy( pcSendBuf + 1 + INT64_LEN + INT64_LEN + INT64_LEN, pcKeysFailed, INT64_LEN );
  
  iSendLen = 1 + INT64_LEN + INT64_LEN + INT64_LEN + INT64_LEN;
  
  memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
  
  send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
  
  send( iaSocket, pcSendBuf, iSendLen, 0 );
  
  return;
}

/**********************************************************************
函数名称: managerLoadDeal
函数功能: 管理端处理 load 操作
参    数：
第    一：共享结构体               I/O
作    者: 李高文
建立时间: 20161030
**********************************************************************/
void managerLoadDealChildThread( LOAD_SHARE *loadShare )
{
	FILE *fp;
	char pcLines[MAX_LINE_BUF];   /*每行的最大值*/
	char pcKey[MAX_KEY_LEN + 1];
	char pcValue[MAX_VALUE_LEN + 1];
	char *pcComma;
	int  iKeyLen;
	int  iValueLen;
	char *pcBufStart;
	char *pcBufEnd;
	__int64 llKeysTotal;
	__int64 llKeysFailed;
	int  iIndepentId;
	
	
	fp = loadShare->fp;
	llKeysTotal = 0;
	llKeysFailed = 0;
	
	/*循环导入数据*/
	while (loadShare->iCmd == RUN_CMD_RUN)
	{
	  memset( pcLines, 0x00, sizeof(pcLines) );
    if (fgets( pcLines, sizeof(pcLines), fp ) == NULL)
    {
      break;
    }
    AllTrim( pcLines );
    if (strlen(pcLines) == 0)
    {
    	continue;
    }
    
    llKeysTotal++;
    
    pcBufStart = pcLines;
    pcBufEnd = pcLines + strlen(pcLines);
    
    /*处理KEY*/
    {
      /*忽略开始空格*/
      IGNORE_BLANK_START( pcBufStart, pcBufEnd );
      
      /*如果空格后面第一个字符不是单引号*/
      if (*(pcBufStart) != '\'')
      {
      	llKeysFailed++;
      	continue;
      }
      
      /*寻找下一个单引号*/
      pcComma = strchr( pcBufStart + 1, '\'' );
      if (pcComma == NULL || pcComma > pcBufEnd)
      {
      	llKeysFailed++;
      	continue;
      }
      
      /*如果KEY值过长*/
      iKeyLen = pcComma - (pcBufStart + 1);
      if (iKeyLen > MAX_KEY_LEN)
      {
      	llKeysFailed++;
      	continue;
      }
      	
      /*取KEY*/
      memset( pcKey, 0x00, sizeof(pcKey) );
      memcpy( pcKey, pcBufStart + 1, pcComma - (pcBufStart + 1) );
    }
    pcBufStart = pcComma + 1;
    
    if (pcBufStart >= pcBufEnd)
    {
    	llKeysFailed++;
      continue;
    }
    
    /*处理VALUE*/
    {
      /*忽略开始空格*/
      IGNORE_BLANK_START( pcBufStart, pcBufEnd );
      if (pcBufStart == pcBufEnd)
      {
    	  llKeysFailed++;
        continue;
      }
      
      /*如果空格后面第一个字符不是单引号*/
      if (*(pcBufStart) != '\'')
      {
      	llKeysFailed++;
      	continue;
      }
      
      /*寻找下一个单引号*/
      pcComma = strchr( pcBufStart + 1, '\'' );
      if (pcComma == NULL || pcComma > pcBufEnd)
      {
      	llKeysFailed++;
      	continue;
      }
      
      /*如果VALUE值过长*/
      iValueLen = pcComma - (pcBufStart + 1);
      if (iValueLen > MAX_VALUE_LEN)
      {
      	llKeysFailed++;
      	continue;
      }
      	
      /*取KEY*/
      memset( pcValue, 0x00, sizeof(pcValue) );
      memcpy( pcValue, pcBufStart + 1, pcComma - (pcBufStart + 1) );
    }
    pcBufStart = pcComma + 1;
    
    /*忽略开始空格*/
    IGNORE_BLANK_START( pcBufStart, pcBufEnd );
    if (pcBufStart != pcBufEnd)
    {
      llKeysFailed++;
      continue;
    }
        
    /*将KEY插入*/
    iIndepentId = calculateKeyMain( pcKey, iKeyLen );
    if (updateNodeMain( pcKey, (__int64)(iKeyLen), pcValue, (__int64)(iValueLen), iIndepentId ) != SUCCESS)
    {
    	llKeysFailed++;
      continue;
    }
	}
	
	loadShare->llTotalRows = llKeysTotal;
	loadShare->llTotalFail = llKeysFailed;
	loadShare->iState = RUN_STATE_FINISHED;
	return;
}

