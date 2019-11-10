#define __KEY_VALUE_THREAD_MANAGER_C

/*
负责处理 MANAGER 语句
*/

#ifndef __COMMON_HEAD_H
  #include "commonHead.h"
#endif

#ifndef __KEY_VALUE_THREAD_MANAGER_DELETE_C
  #include "keyValueThreadManagerDelete.c"
#endif

#ifndef __KEY_VALUE_THREAD_MANAGER_EXIT_FLUSH_ALTER_C
  #include "keyValueThreadManagerExitFlushAlter.c"
#endif

#ifndef __KEY_VALUE_THREAD_MANAGER_SELECT_COUNT_C
  #include "keyValueThreadManagerSelectCount.c"
#endif

#ifndef __KEY_VALUE_THREAD_MANAGER_SELECT_KEY_VALUE_C
  #include "keyValueThreadManagerSelectKeyValue.c"
#endif

#ifndef __KEY_VALUE_THREAD_MANAGER_SELECT_MAX_MIN_C
  #include "keyValueThreadManagerSelectMaxMin.c"
#endif

#ifndef __KEY_VALUE_THREAD_MANAGER_SHOW_C
  #include "keyValueThreadManagerShow.c"
#endif

#ifndef __KEY_VALUE_THREAD_MANAGER_UNLOAD_LOAD_C
  #include "keyValueThreadManagerUnloadLoad.c"
#endif

void managerSocket( );
void managerDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd );
int  managerPositionLeftOpen( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId );
int  managerPositionLeftClose( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId );
int  managerPositionRightOpen( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId );
int  managerPositionRightClose( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId );
int  managerPositionLeftOpenMain( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId );
int  managerPositionLeftCloseMain( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId );
int  managerPositionRightOpenMain( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId );
int  managerPositionRightCloseMain( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId );


/**********************************************************************
函数名称: managerSocket
函数功能: 管理线程处理函数
参    数：
第    一：SOCKET         I
作    者: 李高文
建立时间: 20161018
**********************************************************************/
void managerSocket( )
{
	struct sockaddr_in server;   /*服务器地址信息结构体*/
  struct sockaddr_in addr;     /*接收到的客户端信息结构体*/
  int sin_size;                /*地址信息结构体大小*/
  int iSocket;
  int iNewSocket;
  
	char pcRecvLen[DATA_BUF_LEN + 1];
  int iRecvLen;
	int iTotalLen;
	int iDataLen;
	int iRet;
	char *pcRecvBuf;
	int iSize = 0;

  iSocket = sgManagerThread.iSocket;
  sin_size = sizeof(struct sockaddr);
  
  
  while (sgManagerThread.iRunState == THREAD_RUN_STATE_RUN)
  {
  	/*ACCPET 正在执行*/
  	sgManagerThread.iAcceptState = ACCEPT_STATE_YES;
  	
    /*调用accept，返回服务器与客户端连接的socket描述符*/
    iNewSocket = accept(iSocket, (struct sockaddr *)&addr, &sin_size);
    if (iNewSocket == -1)
    {
      continue;   
    }
    
    /*ACCPET 执行完成*/
    sgManagerThread.iAcceptState = ACCEPT_STATE_NO;

	  /*接收10B长度*/
	  iTotalLen = 0;
	  iRecvLen = 0;
	  memset( pcRecvLen, 0x00, sizeof(pcRecvLen) );
	  	
	  while (sgManagerThread.iRunState == THREAD_RUN_STATE_RUN)
	  {
	  	iRecvLen = recv( iNewSocket, pcRecvLen + iTotalLen, DATA_BUF_LEN - iTotalLen, 0 );
	  	if (iRecvLen <= 0)
	  	{
	  		closesocket(iNewSocket);
	  		break;
	  	}
	  	iTotalLen += iRecvLen;
	  	if (iTotalLen == DATA_BUF_LEN)
	  	{
	  		break;
	  	}
	  }
	  if (sgManagerThread.iRunState == THREAD_RUN_STATE_STOP)
	  {
	  	break;
	  }
	  if (iTotalLen != DATA_BUF_LEN)
	  {
	  	closesocket(iNewSocket);
	  	continue;
	  }
	  	  
	  /*计算长度*/
	  iDataLen = atoi( pcRecvLen );
	  if (iDataLen < OPERATE_LEN)
	  {
	  	closesocket(iNewSocket);
	  	continue;
	  }
	  
	  /*分配内存*/
	  pcRecvBuf = malloc( iDataLen + 1 );
	  if (pcRecvBuf == NULL)
	  {
	  	closesocket(iNewSocket);
	  	continue;
	  }
	  memset( pcRecvBuf, 0x00, iDataLen + 1 );
	  
	  /*接收数据*/
	  iTotalLen = 0;
	  iRecvLen = 0;
	 
	  while (igConnectionErrFlag == SUCCESS)
	  {
	  	iRecvLen = recv( iNewSocket, pcRecvBuf + iTotalLen, iDataLen - iTotalLen, 0 );
	  	if (iRecvLen <= 0)
	  	{
	  		break;
	  	}
	  		  	
	  	iTotalLen += iRecvLen;
	  	if (iTotalLen == iDataLen)
	  	{
	  		break;
	  	}
	  }
	  if (sgManagerThread.iRunState == THREAD_RUN_STATE_STOP)
	  {
	  	free(pcRecvBuf);
	  	break;
	  }
	  if (iTotalLen != iDataLen)
	  {
	  	closesocket(iNewSocket);
	  	free(pcRecvBuf);
	  	continue;
	  }
	  
	  managerDeal( iNewSocket, pcRecvBuf, pcRecvBuf + iTotalLen );

	  closesocket(iNewSocket);
	  free( pcRecvBuf );	  
  }
}

/**********************************************************************
函数名称: managerDeal
函数功能: 处理管理端接收到的数据
参    数：
第    一：SOCKET               I
第    二：数据开始地址         I
第    三：数据结束地址         I
作    者: 李高文
建立时间: 20161018
**********************************************************************/
void managerDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd )
{
#define MANAGER_DEAL_SEND_RESULT( value ) memset( pcResult, 0x00, sizeof(pcResult) );sprintf( pcResult, "%d", value ); cResult = pcResult[0];send( iaSocket, &cResult, 1, 0 );
	char pcResult[INT_LEN + 1];
	char pcOperate[OPERATE_LEN + 1];
	char cResult;
	char *pcBufStart;
	char *pcBufEnd;
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcBufEnd;
	
	/*取操作类型*/
	memset( pcOperate, 0x00, sizeof(pcOperate) );
	memcpy( pcOperate, pcBufStart, OPERATE_LEN );
	pcBufStart += OPERATE_LEN;
	
	if (!strcmp( pcOperate, MANAGER_SELECT_KEY_CNT ) ||
	    !strcmp( pcOperate, MANAGER_SELECT_VALUE_CNT )||
	    !strcmp( pcOperate, MANAGER_SELECT_KEY_VALUE_CNT ))
	{
		managerSelectKeyValueDeal( iaSocket, pcOperate, pcBufStart, pcBufEnd );
	}
	else
	if (!strcmp( pcOperate, MANAGER_SELECT_MIN_CNT ))
	{
		managerSelectMinDeal( iaSocket );
	}
	else
	if (!strcmp( pcOperate, MANAGER_SELECT_MAX_CNT ))
	{
		managerSelectMaxDeal( iaSocket );
	}
	else
	if (!strcmp( pcOperate, MANAGER_SELECT_COUNT_CNT ))
	{
		managerSelectCountDeal( iaSocket, pcBufStart, pcBufEnd );
	}
	else
	if (!strcmp( pcOperate, MANAGER_DELETE_CNT ))
	{
		managerDeleteDeal( iaSocket, pcBufStart, pcBufEnd );
	}
	else
	if (!strcmp( pcOperate, MANAGER_UNLOAD_CNT ))
	{
		managerUnloadDeal( iaSocket, pcBufStart, pcBufEnd );
	}
	else
	if (!strcmp( pcOperate, MANAGER_LOAD_CNT ))
	{
		managerLoadDeal( iaSocket, pcBufStart, pcBufEnd );
	}
	else
	if (!strcmp( pcOperate, MANAGER_EXIT_CNT ))
	{
		managerExitDeal( iaSocket, pcBufStart, pcBufEnd );
	}
	else
	if (!strcmp( pcOperate, MANAGER_FLUSH_CNT ))
	{
		managerFlushDeal( iaSocket, pcBufStart, pcBufEnd );
	}
	else
	if (!strcmp( pcOperate, MANAGER_ALTER_DISCS_CNT ))
	{
		managerAlterDiscsDeal( iaSocket, pcBufStart, pcBufEnd );
	}
	else
	if (!strcmp( pcOperate, MANAGER_ALTER_UPPERS_CNT ))
	{
		managerAlterUppersDeal( iaSocket, pcBufStart, pcBufEnd );
	}
	else
	if (!strcmp( pcOperate, MANAGER_SHOW_RAMS_CNT ))
	{
		managerShowRamsDeal( iaSocket );
	}
	else
	if (!strcmp( pcOperate, MANAGER_SHOW_DISCS_CNT ))
	{
		managerShowDiscsDeal( iaSocket );
	}
	else
	if (!strcmp( pcOperate, MANAGER_SHOW_THREADS_CNT ))
	{
		managerShowThreadsDeal( iaSocket );
	}
	else
	if (!strcmp( pcOperate, MANAGER_SHOW_KEYS_CNT ))
	{
		managerShowKeysDeal( iaSocket );
	}
	else
	{
		MANAGER_DEAL_SEND_RESULT( 1 );
	  return;
	}
	
	return;
}

/**********************************************************************
函数名称: managerPositionLeftOpen
函数功能: 管理端处理中的定位开始位置，左开区间
参    数：
第    一：结果位置变量     O
第    二：KEY              I
第    三：地址             I
第    四：数量             I
第    五：级数             I
第    六：总级数           I
返    回：
作    者: 李高文
建立时间: 20161105
**********************************************************************/
int managerPositionLeftOpen( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId )
{
	char  pcKey[MAX_KEY_LEN];
	KEY_POINT *key;                /*其他级数据中的数据结构*/
  KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
  char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
  int  iIndex;
  int  iTmp;
  int  iRet;
    
  /*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
		if (iaAddr == 0)
		{
			return NOT_FOUND;
		}
		
		/*取得最后一级所有数据*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
    for (iIndex = 0; iIndex < iaSize; iIndex++)
    {
    	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iIndex * sizeof(KEY_VALUE_POINT));
    	
    	/*从 VALUE 文件中获取 KEY 的值*/
      valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
      pcKey[keyData->llKeyLen] = 0x00;
      
      /*如果比目标值大，结束*/
      if (strcmp( pcKey, pcaKey ) > 0)
      {
      	break;
      }
    }
    
    /*如果有*/
    if (iIndex < iaSize)
    {
    	/*设置当前级的结果*/
    	iaOutResult[iaLoop] = iIndex;
    	
    	return SUCCESS;
    }
    else
    {
    	return NOT_FOUND;
    }
	}
  /*不是最后一级*/
	else
	{
		if (iaAddr == 0)
		{
			return NOT_FOUND;
		}
		/*取得其他级所有数据*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
    /*搜索第一个比目标值大的数的位置*/
    for (iIndex = 0; iIndex < iaSize; iIndex++)
    {
    	key = (KEY_POINT*)(pcDataBuf + iIndex * sizeof(KEY_POINT));
    	
    	/*从 VALUE 文件中获取 KEY 的值*/
      valueToBuf( pcKey, key->llKeyAddr, key->llKeyLen, iaIndepentFileId );
      pcKey[key->llKeyLen] = 0x00;
      
      /*如果比目标值大，结束*/
      if (strcmp( pcKey, pcaKey ) > 0)
      {
      	break;
      }
    }
    
    /*如果第一个位置就是，设置当前级数的位置，以及下面级数的位置*/
    if (iIndex == 0 && strcmp( pcKey, pcaKey ) == 0)
    {
    	/*设置当前级的结果*/
    	iaOutResult[iaLoop] = iIndex;
    	
    	/*设置剩下级的结果*/
    	for (iTmp = iaLoop + 1; iTmp <= iaTotalLoops; iTmp++)
    	{
    		iaOutResult[iTmp] = 0;
    	}
    	
    	return SUCCESS;
    }
    /*不是第一个位置*/
    else
    {
    	/*如果不存在，当前 key 就是最后的数据*/
    	if (iIndex == iaSize)
    	{
    		/*设置当前级的结果*/
    	  iaOutResult[iaLoop] = iIndex - 1;
    	  
    	  /*指向当前值*/
    		key = (KEY_POINT*)(pcDataBuf + (iIndex - 1) * sizeof(KEY_POINT));
    		
    		/*调用下一级*/
    		iRet = managerPositionLeftOpen( iaOutResult, pcaKey, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
    		
    		/*如果找到满足条件的*/
    		if (iRet == SUCCESS)
    		{
    			return SUCCESS;
    		}
    		/*如果下级返回没有找到*/
    		else
    		/*if (iRet == NOT_FOUND)*/
    		{
    			return NOT_FOUND;
    		}
    	}
    	/*如果存在*/
    	else
    	{
    		/*设置当前级的结果*/
    	  iaOutResult[iaLoop] = iIndex - 1;
    		
    		/*指向当前值*/
    		key = (KEY_POINT*)(pcDataBuf + (iIndex - 1) * sizeof(KEY_POINT));
    		
    		/*调用下一级*/
    		iRet = managerPositionLeftOpen( iaOutResult, pcaKey, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
    		
    		/*如果找到满足条件的*/
    		if (iRet == SUCCESS)
    		{
    			return SUCCESS;
    		}
    		else
    		/*if (iRet == NOT_FOUND)*/
    		{
    			/*重新设置当前级的结果*/
    	    iaOutResult[iaLoop] = iIndex;
    	    
    	    /*设置剩下级的结果*/
        	for (iTmp = iaLoop + 1; iTmp <= iaTotalLoops; iTmp++)
        	{
    		    iaOutResult[iTmp] = 0;
    	    }
    	  
    			return SUCCESS;
    		}
    	}
    }
	}
}

/**********************************************************************
函数名称: managerPositionLeftClose
函数功能: 管理端处理中的定位开始位置，左闭区间
参    数：
第    一：结果位置变量     O
第    二：KEY              I
第    三：地址             I
第    四：数量             I
第    五：级数             I
第    六：总级数           I
返    回：
作    者: 李高文
建立时间: 20161105
**********************************************************************/
int managerPositionLeftClose( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId )
{
	char  pcKey[MAX_KEY_LEN];
	KEY_POINT *key;                /*其他级数据中的数据结构*/
  KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
  char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
  int  iIndex;
  int  iTmp;
  int  iRet;
  
  if (iaSize == 0)
  {
		return NOT_FOUND;
	}
  /*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
		/*取得最后一级所有数据*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );

    for (iIndex = 0; iIndex < iaSize; iIndex++)
    {
    	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iIndex * sizeof(KEY_VALUE_POINT));
    	
    	/*从 VALUE 文件中获取 KEY 的值*/
      valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
      pcKey[keyData->llKeyLen] = 0x00;
      
      /*如果比目标值大，结束*/
      if (strcmp( pcKey, pcaKey ) >= 0)
      {
      	break;
      }
    }

    /*如果有*/
    if (iIndex < iaSize)
    {
    	/*设置当前级的结果*/
    	iaOutResult[iaLoop] = iIndex;
    	
    	return SUCCESS;
    }
    else
    {
    	return NOT_FOUND;
    }
	}
  /*不是最后一级*/
	else
	{
		if (iaAddr == 0)
		{
			return NOT_FOUND;
		}
		/*取得其他级所有数据*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
    /*搜索第一个比目标值大的数的位置*/
    for (iIndex = 0; iIndex < iaSize; iIndex++)
    {
    	key = (KEY_POINT*)(pcDataBuf + iIndex * sizeof(KEY_POINT));
    	
    	/*从 VALUE 文件中获取 KEY 的值*/
      valueToBuf( pcKey, key->llKeyAddr, key->llKeyLen, iaIndepentFileId );
      pcKey[key->llKeyLen] = 0x00;
            
      /*如果比目标值大，结束*/
      if (strcmp( pcKey, pcaKey ) >= 0)
      {
      	break;
      }
    }

    /*如果第一个位置就是，或者该值刚好与目标值相等，设置当前级数的位置，以及下面级数的位置*/
    if (iIndex == 0 && strcmp( pcKey, pcaKey ) >= 0)/*这里，与左开的算法不同*/
    {
    	/*设置当前级的结果*/
    	iaOutResult[iaLoop] = iIndex;

    	/*设置剩下级的结果*/
    	for (iTmp = iaLoop + 1; iTmp <= iaTotalLoops; iTmp++)
    	{
    		iaOutResult[iTmp] = 0;
    	}

    	return SUCCESS;
    }
    /*不是第一个位置*/
    else
    {
    	/*如果不存在，当前 key 就是最后的数据*/
    	if (iIndex == iaSize)
    	{
    		/*设置当前级的结果*/
    	  iaOutResult[iaLoop] = iIndex - 1;
    	  
    	  /*指向当前值*/
    		key = (KEY_POINT*)(pcDataBuf + (iIndex - 1) * sizeof(KEY_POINT));
    		
    		/*调用下一级*/
    		iRet = managerPositionLeftClose( iaOutResult, pcaKey, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
    		
    		/*如果找到满足条件的*/
    		if (iRet == SUCCESS)
    		{
    			return SUCCESS;
    		}
    		/*如果下级返回没有找到*/
    		else
    		/*if (iRet == NOT_FOUND)*/
    		{
    			return NOT_FOUND;
    		}
    	}
    	/*如果存在*/
    	else
    	{
    		/*设置当前级的前一个值*/
    	  iaOutResult[iaLoop] = iIndex - 1;
    		
    		/*指向当前值*/
    		key = (KEY_POINT*)(pcDataBuf + (iIndex - 1) * sizeof(KEY_POINT));
    		
    		/*调用下一级*/
    		iRet = managerPositionLeftClose( iaOutResult, pcaKey, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
    		
    		/*如果找到满足条件的*/
    		if (iRet == SUCCESS)
    		{
    			return SUCCESS;
    		}
    		else
    		/*if (iRet == NOT_FOUND)*/
    		{
    			/*重新设置当前级的结果*/
    	    iaOutResult[iaLoop] = iIndex;
    	    
    	    /*设置剩下级的结果*/
        	for (iTmp = iaLoop + 1; iTmp <= iaTotalLoops; iTmp++)
        	{
    		    iaOutResult[iTmp] = 0;
    	    }
    	  
    			return SUCCESS;
    		}
    	}
    }
	}
}

/**********************************************************************
函数名称: managerPositionRightOpen
函数功能: 管理端处理中的定位开始位置，右开区间
参    数：
第    一：结果位置变量     O
第    二：KEY              I
第    三：地址             I
第    四：数量             I
第    五：级数             I
第    六：总级数           I
返    回：
作    者: 李高文
建立时间: 20161106
**********************************************************************/
int managerPositionRightOpen( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId )
{
	char  pcKey[MAX_KEY_LEN];
	KEY_POINT *key;                /*其他级数据中的数据结构*/
  KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
  char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
  int  iIndex;
  int  iTmp;
  int  iRet;
  
  if (iaSize == 0)
  {
		return NOT_FOUND;
	}
	
  /*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
		/*取得最后一级所有数据*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
    for (iIndex = iaSize - 1; iIndex >= 0; iIndex--)
    {
    	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iIndex * sizeof(KEY_VALUE_POINT));
    	
    	/*从 VALUE 文件中获取 KEY 的值*/
      valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
      pcKey[keyData->llKeyLen] = 0x00;
      
      /*如果比目标值小，结束*/
      if (strcmp( pcKey, pcaKey ) < 0)
      {
      	break;
      }
    }
    
    /*如果没有找到*/
    if (iIndex == -1)
    {
    	return NOT_FOUND;
    }
    else
    {
    	/*设置当前级的结果*/
    	iaOutResult[iaLoop] = iIndex;
    	
    	return SUCCESS;
    }
	}
  /*不是最后一级*/
	else
	{
		if (iaAddr == 0)
		{
			return NOT_FOUND;
		}
		
		/*取得其他级所有数据*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
    /*搜索第一个比目标值小的数的位置*/
    for (iIndex = iaSize - 1; iIndex >= 0; iIndex--)
    {
    	key = (KEY_POINT*)(pcDataBuf + iIndex * sizeof(KEY_POINT));
    	
    	/*从 VALUE 文件中获取 KEY 的值*/
      valueToBuf( pcKey, key->llKeyAddr, key->llKeyLen, iaIndepentFileId );
      pcKey[key->llKeyLen] = 0x00;
      
      /*如果比目标值小，结束*/
      if (strcmp( pcKey, pcaKey ) < 0)
      {
      	break;
      }
    }
    
    /*如果没有找到*/
    if (iIndex == -1)
    {
    	return NOT_FOUND;
    }
    else
    /*如果找到*/
    {
    	/*设置当前级的结果*/
    	iaOutResult[iaLoop] = iIndex;
    	
    	/*指向当前值*/
    	key = (KEY_POINT*)(pcDataBuf + iIndex * sizeof(KEY_POINT));
    	
    	/*调用下一级*/
    	iRet = managerPositionRightOpen( iaOutResult, pcaKey, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
    	
    	/*如果找到满足条件的*/
    	if (iRet == SUCCESS)
    	{
    		return SUCCESS;
    	}
    	/*如果下级返回没有找到*/
    	else
    	/*if (iRet == NOT_FOUND)*/
    	{
    		return NOT_FOUND;
    	}
    }
	}
}

/**********************************************************************
函数名称: managerPositionRightClose
函数功能: 管理端处理中的定位开始位置，右闭区间
参    数：
第    一：结果位置变量     O
第    二：KEY              I
第    三：地址             I
第    四：数量             I
第    五：级数             I
第    六：总级数           I
返    回：
作    者: 李高文
建立时间: 20161106
**********************************************************************/
int managerPositionRightClose( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId )
{
	char  pcKey[MAX_KEY_LEN];
	KEY_POINT *key;                /*其他级数据中的数据结构*/
  KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
  char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
  int  iIndex;
  int  iTmp;
  int  iRet;
  
  if (iaSize == 0)
  {
		return NOT_FOUND;
	}
	
  /*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
		/*取得最后一级所有数据*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
    for (iIndex = iaSize - 1; iIndex >= 0; iIndex--)
    {
    	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iIndex * sizeof(KEY_VALUE_POINT));
    	
    	/*从 VALUE 文件中获取 KEY 的值*/
      valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
      pcKey[keyData->llKeyLen] = 0x00;
            
      /*如果比目标值小，结束*/
      if (strcmp( pcKey, pcaKey ) <= 0)
      {
      	break;
      }
    }
    
    /*如果没有找到*/
    if (iIndex == -1)
    {
    	return NOT_FOUND;
    }
    else
    {
    	/*设置当前级的结果*/
    	iaOutResult[iaLoop] = iIndex;
    	
    	return SUCCESS;
    }
	}
  /*不是最后一级*/
	else
	{
		if (iaAddr == 0)
		{
			return NOT_FOUND;
		}
		
		/*取得其他级所有数据*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
    /*搜索第一个比目标值小的数的位置*/
    for (iIndex = iaSize - 1; iIndex >= 0; iIndex--)
    {
    	key = (KEY_POINT*)(pcDataBuf + iIndex * sizeof(KEY_POINT));
    	
    	/*从 VALUE 文件中获取 KEY 的值*/
      valueToBuf( pcKey, key->llKeyAddr, key->llKeyLen, iaIndepentFileId );
      pcKey[key->llKeyLen] = 0x00;
      
      /*如果比目标值小，结束*/
      if (strcmp( pcKey, pcaKey ) <= 0)
      {
      	break;
      }
    }
    
    /*如果没有找到*/
    if (iIndex == -1)
    {
    	return NOT_FOUND;
    }
    else
    /*如果找到*/
    {
    	/*设置当前级的结果*/
    	iaOutResult[iaLoop] = iIndex;
    	
    	/*指向当前值*/
    	key = (KEY_POINT*)(pcDataBuf + iIndex * sizeof(KEY_POINT));
    	
    	/*调用下一级*/
    	iRet = managerPositionRightClose( iaOutResult, pcaKey, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
    	
    	/*如果找到满足条件的*/
    	if (iRet == SUCCESS)
    	{
    		return SUCCESS;
    	}
    	/*如果下级返回没有找到*/
    	else
    	/*if (iRet == NOT_FOUND)*/
    	{
    		return NOT_FOUND;
    	}
    }
	}
}

/**********************************************************************
函数名称: managerPositionLeftOpenMain
函数功能: 管理端处理中的定位开始位置，左开区间
参    数：
第    一：结果位置变量     O
第    二：KEY              I
第    三：地址             I
第    四：数量             I
第    五：级数             I
第    六：总级数           I
返    回：
作    者: 李高文
建立时间: 20161105
**********************************************************************/
int managerPositionLeftOpenMain( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId )
{
	char  pcKey[MAX_KEY_LEN];
	KEY_POINT *key;                /*其他级数据中的数据结构*/
  KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
  char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
  int  iIndex;
  int  iTmp;
  int  iRet;
  
  if (iaSize == 0)
  {
		return NOT_FOUND;
	}
	
  /*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
		/*取得最后一级所有数据*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    for (iIndex = 0; iIndex < iaSize; iIndex++)
    {
    	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iIndex * sizeof(KEY_VALUE_POINT));
    	
    	/*从 VALUE 文件中获取 KEY 的值*/
      valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
      pcKey[keyData->llKeyLen] = 0x00;
      
      /*如果比目标值大，结束*/
      if (strcmp( pcKey, pcaKey ) > 0)
      {
      	break;
      }
    }
    
    /*如果有*/
    if (iIndex < iaSize)
    {
    	/*设置当前级的结果*/
    	iaOutResult[iaLoop] = iIndex;
    	
    	return SUCCESS;
    }
    else
    {
    	return NOT_FOUND;
    }
	}
  /*不是最后一级*/
	else
	{
		if (iaAddr == 0)
		{
			return NOT_FOUND;
		}
		/*取得其他级所有数据*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    /*搜索第一个比目标值大的数的位置*/
    for (iIndex = 0; iIndex < iaSize; iIndex++)
    {
    	key = (KEY_POINT*)(pcDataBuf + iIndex * sizeof(KEY_POINT));
    	
    	/*从 VALUE 文件中获取 KEY 的值*/
      valueToBufMain( pcKey, key->llKeyAddr, key->llKeyLen, iaIndepentId );
      pcKey[key->llKeyLen] = 0x00;
      
      /*如果比目标值大，结束*/
      if (strcmp( pcKey, pcaKey ) > 0)
      {
      	break;
      }
    }
    
    /*如果第一个位置就是，设置当前级数的位置，以及下面级数的位置*/
    if (iIndex == 0 && strcmp( pcKey, pcaKey ) == 0)
    {
    	/*设置当前级的结果*/
    	iaOutResult[iaLoop] = iIndex;
    	
    	/*设置剩下级的结果*/
    	for (iTmp = iaLoop + 1; iTmp <= iaTotalLoops; iTmp++)
    	{
    		iaOutResult[iTmp] = 0;
    	}
    	
    	return SUCCESS;
    }
    /*不是第一个位置*/
    else
    {
    	/*如果不存在，当前 key 就是最后的数据*/
    	if (iIndex == iaSize)
    	{
    		/*设置当前级的结果*/
    	  iaOutResult[iaLoop] = iIndex - 1;
    	  
    	  /*指向当前值*/
    		key = (KEY_POINT*)(pcDataBuf + (iIndex - 1) * sizeof(KEY_POINT));
    		
    		/*调用下一级*/
    		iRet = managerPositionLeftOpenMain( iaOutResult, pcaKey, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
    		
    		/*如果找到满足条件的*/
    		if (iRet == SUCCESS)
    		{
    			return SUCCESS;
    		}
    		/*如果下级返回没有找到*/
    		else
    		/*if (iRet == NOT_FOUND)*/
    		{
    			return NOT_FOUND;
    		}
    	}
    	/*如果存在*/
    	else
    	{
    		/*设置当前级的结果*/
    	  iaOutResult[iaLoop] = iIndex - 1;
    		
    		/*指向当前值*/
    		key = (KEY_POINT*)(pcDataBuf + (iIndex - 1) * sizeof(KEY_POINT));
    		
    		/*调用下一级*/
    		iRet = managerPositionLeftOpenMain( iaOutResult, pcaKey, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
    		
    		/*如果找到满足条件的*/
    		if (iRet == SUCCESS)
    		{
    			return SUCCESS;
    		}
    		else
    		/*if (iRet == NOT_FOUND)*/
    		{
    			/*重新设置当前级的结果*/
    	    iaOutResult[iaLoop] = iIndex;
    	    
    	    /*设置剩下级的结果*/
        	for (iTmp = iaLoop + 1; iTmp <= iaTotalLoops; iTmp++)
        	{
    		    iaOutResult[iTmp] = 0;
    	    }
    	  
    			return SUCCESS;
    		}
    	}
    }
	}
}

/**********************************************************************
函数名称: managerPositionLeftCloseMain
函数功能: 管理端处理中的定位开始位置，左闭区间
参    数：
第    一：结果位置变量     O
第    二：KEY              I
第    三：地址             I
第    四：数量             I
第    五：级数             I
第    六：总级数           I
返    回：
作    者: 李高文
建立时间: 20161105
**********************************************************************/
int managerPositionLeftCloseMain( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId )
{
	char  pcKey[MAX_KEY_LEN];
	KEY_POINT *key;                /*其他级数据中的数据结构*/
  KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
  char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
  int  iIndex;
  int  iTmp;
  int  iRet;

  if (iaSize == 0)
  {
		return NOT_FOUND;
	}
  /*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
		/*取得最后一级所有数据*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );

    for (iIndex = 0; iIndex < iaSize; iIndex++)
    {
    	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iIndex * sizeof(KEY_VALUE_POINT));
    	
    	/*从 VALUE 文件中获取 KEY 的值*/
      valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
      pcKey[keyData->llKeyLen] = 0x00;
      
      /*如果比目标值大，结束*/
      if (strcmp( pcKey, pcaKey ) >= 0)
      {
      	break;
      }
    }

    /*如果有*/
    if (iIndex < iaSize)
    {
    	/*设置当前级的结果*/
    	iaOutResult[iaLoop] = iIndex;
    	
    	return SUCCESS;
    }
    else
    {
    	return NOT_FOUND;
    }
	}
  /*不是最后一级*/
	else
	{
		if (iaAddr == 0)
		{
			return NOT_FOUND;
		}
		/*取得其他级所有数据*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    /*搜索第一个比目标值大的数的位置*/
    for (iIndex = 0; iIndex < iaSize; iIndex++)
    {
    	key = (KEY_POINT*)(pcDataBuf + iIndex * sizeof(KEY_POINT));
    	
    	/*从 VALUE 文件中获取 KEY 的值*/
      valueToBufMain( pcKey, key->llKeyAddr, key->llKeyLen, iaIndepentId );
      pcKey[key->llKeyLen] = 0x00;
            
      /*如果比目标值大，结束*/
      if (strcmp( pcKey, pcaKey ) >= 0)
      {
      	break;
      }
    }
    
    /*如果第一个位置就是，或者该值刚好与目标值相等，设置当前级数的位置，以及下面级数的位置*/
    if (iIndex == 0 && strcmp( pcKey, pcaKey ) >= 0)/*这里，与左开的算法不同*/
    {
    	/*设置当前级的结果*/
    	iaOutResult[iaLoop] = iIndex;

    	/*设置剩下级的结果*/
    	for (iTmp = iaLoop + 1; iTmp <= iaTotalLoops; iTmp++)
    	{
    		iaOutResult[iTmp] = 0;
    	}

    	return SUCCESS;
    }
    /*不是第一个位置*/
    else
    {
    	/*如果不存在，当前 key 就是最后的数据*/
    	if (iIndex == iaSize)
    	{
    		/*设置当前级的结果*/
    	  iaOutResult[iaLoop] = iIndex - 1;
    	  
    	  /*指向当前值*/
    		key = (KEY_POINT*)(pcDataBuf + (iIndex - 1) * sizeof(KEY_POINT));

    		/*调用下一级*/
    		iRet = managerPositionLeftCloseMain( iaOutResult, pcaKey, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
    		
    		/*如果找到满足条件的*/
    		if (iRet == SUCCESS)
    		{
    			return SUCCESS;
    		}
    		/*如果下级返回没有找到*/
    		else
    		/*if (iRet == NOT_FOUND)*/
    		{
    			return NOT_FOUND;
    		}
    	}
    	/*如果存在*/
    	else
    	{
    		/*设置当前级的前一个值*/
    	  iaOutResult[iaLoop] = iIndex - 1;
    		
    		/*指向当前值*/
    		key = (KEY_POINT*)(pcDataBuf + (iIndex - 1) * sizeof(KEY_POINT));

    		/*调用下一级*/
    		iRet = managerPositionLeftCloseMain( iaOutResult, pcaKey, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
    		
    		/*如果找到满足条件的*/
    		if (iRet == SUCCESS)
    		{
    			return SUCCESS;
    		}
    		else
    		/*if (iRet == NOT_FOUND)*/
    		{
    			/*重新设置当前级的结果*/
    	    iaOutResult[iaLoop] = iIndex;
    	    
    	    /*设置剩下级的结果*/
        	for (iTmp = iaLoop + 1; iTmp <= iaTotalLoops; iTmp++)
        	{
    		    iaOutResult[iTmp] = 0;
    	    }
    	  
    			return SUCCESS;
    		}
    	}
    }
	}
}

/**********************************************************************
函数名称: managerPositionRightOpenMain
函数功能: 管理端处理中的定位开始位置，右开区间
参    数：
第    一：结果位置变量     O
第    二：KEY              I
第    三：地址             I
第    四：数量             I
第    五：级数             I
第    六：总级数           I
返    回：
作    者: 李高文
建立时间: 20161106
**********************************************************************/
int managerPositionRightOpenMain( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId )
{
	char  pcKey[MAX_KEY_LEN];
	KEY_POINT *key;                /*其他级数据中的数据结构*/
  KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
  char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
  int  iIndex;
  int  iTmp;
  int  iRet;
  
  if (iaSize == 0)
  {
		return NOT_FOUND;
	}
	
  /*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
		/*取得最后一级所有数据*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    for (iIndex = iaSize - 1; iIndex >= 0; iIndex--)
    {
    	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iIndex * sizeof(KEY_VALUE_POINT));
    	
    	/*从 VALUE 文件中获取 KEY 的值*/
      valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
      pcKey[keyData->llKeyLen] = 0x00;
      
      /*如果比目标值小，结束*/
      if (strcmp( pcKey, pcaKey ) < 0)
      {
      	break;
      }
    }
    
    /*如果没有找到*/
    if (iIndex == -1)
    {
    	return NOT_FOUND;
    }
    else
    {
    	/*设置当前级的结果*/
    	iaOutResult[iaLoop] = iIndex;
    	
    	return SUCCESS;
    }
	}
  /*不是最后一级*/
	else
	{
		if (iaAddr == 0)
		{
			return NOT_FOUND;
		}
		
		/*取得其他级所有数据*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    /*搜索第一个比目标值小的数的位置*/
    for (iIndex = iaSize - 1; iIndex >= 0; iIndex--)
    {
    	key = (KEY_POINT*)(pcDataBuf + iIndex * sizeof(KEY_POINT));
    	
    	/*从 VALUE 文件中获取 KEY 的值*/
      valueToBufMain( pcKey, key->llKeyAddr, key->llKeyLen, iaIndepentId );
      pcKey[key->llKeyLen] = 0x00;
      
      /*如果比目标值小，结束*/
      if (strcmp( pcKey, pcaKey ) < 0)
      {
      	break;
      }
    }
    
    /*如果没有找到*/
    if (iIndex == -1)
    {
    	return NOT_FOUND;
    }
    else
    /*如果找到*/
    {
    	/*设置当前级的结果*/
    	iaOutResult[iaLoop] = iIndex;
    	
    	/*指向当前值*/
    	key = (KEY_POINT*)(pcDataBuf + iIndex * sizeof(KEY_POINT));
    	
    	/*调用下一级*/
    	iRet = managerPositionRightOpenMain( iaOutResult, pcaKey, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
    	
    	/*如果找到满足条件的*/
    	if (iRet == SUCCESS)
    	{
    		return SUCCESS;
    	}
    	/*如果下级返回没有找到*/
    	else
    	/*if (iRet == NOT_FOUND)*/
    	{
    		return NOT_FOUND;
    	}
    }
	}
}

/**********************************************************************
函数名称: managerPositionRightCloseMain
函数功能: 管理端处理中的定位开始位置，右闭区间
参    数：
第    一：结果位置变量     O
第    二：KEY              I
第    三：地址             I
第    四：数量             I
第    五：级数             I
第    六：总级数           I
返    回：
作    者: 李高文
建立时间: 20161106
**********************************************************************/
int managerPositionRightCloseMain( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId )
{
	char  pcKey[MAX_KEY_LEN];
	KEY_POINT *key;                /*其他级数据中的数据结构*/
  KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
  char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
  int  iIndex;
  int  iTmp;
  int  iRet;
  
  if (iaSize == 0)
  {
		return NOT_FOUND;
	}
	
  /*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
		/*取得最后一级所有数据*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    for (iIndex = iaSize - 1; iIndex >= 0; iIndex--)
    {
    	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iIndex * sizeof(KEY_VALUE_POINT));
    	
    	/*从 VALUE 文件中获取 KEY 的值*/
      valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
      pcKey[keyData->llKeyLen] = 0x00;
            
      /*如果比目标值小，结束*/
      if (strcmp( pcKey, pcaKey ) <= 0)
      {
      	break;
      }
    }
    
    /*如果没有找到*/
    if (iIndex == -1)
    {
    	return NOT_FOUND;
    }
    else
    {
    	/*设置当前级的结果*/
    	iaOutResult[iaLoop] = iIndex;
    	
    	return SUCCESS;
    }
	}
  /*不是最后一级*/
	else
	{
		if (iaAddr == 0)
		{
			return NOT_FOUND;
		}
		
		/*取得其他级所有数据*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    /*搜索第一个比目标值小的数的位置*/
    for (iIndex = iaSize - 1; iIndex >= 0; iIndex--)
    {
    	key = (KEY_POINT*)(pcDataBuf + iIndex * sizeof(KEY_POINT));
    	
    	/*从 VALUE 文件中获取 KEY 的值*/
      valueToBufMain( pcKey, key->llKeyAddr, key->llKeyLen, iaIndepentId );
      pcKey[key->llKeyLen] = 0x00;
      
      /*如果比目标值小，结束*/
      if (strcmp( pcKey, pcaKey ) <= 0)
      {
      	break;
      }
    }
    
    /*如果没有找到*/
    if (iIndex == -1)
    {
    	return NOT_FOUND;
    }
    else
    /*如果找到*/
    {
    	/*设置当前级的结果*/
    	iaOutResult[iaLoop] = iIndex;
    	
    	/*指向当前值*/
    	key = (KEY_POINT*)(pcDataBuf + iIndex * sizeof(KEY_POINT));
    	
    	/*调用下一级*/
    	iRet = managerPositionRightCloseMain( iaOutResult, pcaKey, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
    	
    	/*如果找到满足条件的*/
    	if (iRet == SUCCESS)
    	{
    		return SUCCESS;
    	}
    	/*如果下级返回没有找到*/
    	else
    	/*if (iRet == NOT_FOUND)*/
    	{
    		return NOT_FOUND;
    	}
    }
	}
}
