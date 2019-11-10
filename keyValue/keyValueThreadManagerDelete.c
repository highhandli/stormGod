#define __KEY_VALUE_THREAD_MANAGER_DELETE_C

/*
负责处理 MANAGER 中的 DELETE 语句
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

void managerDeleteDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd );
void managerDeleteDealChild( DELETE_SHARE *deleteShare, int iaIndepentFileId );
int  managerDeleteDealDeleteData( DELETE_SHARE *deleteShare, int iaStartOffset[], int iaEndOffset[], int iaIndepentFileId );
int managerDeleteDealDeleteDataLeft( DELETE_SHARE *deleteShare, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId );
int managerDeleteDealDeleteDataOther( DELETE_SHARE *deleteShare, __int64 iaAddr, __int64 iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId );
int managerDeleteDealDeleteDataRight( DELETE_SHARE *deleteShare, KEY_POINT *saKey, int iaEndOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId );
int managerDeleteNode( char *pcaKey, int iaKeyLen, int iaIndepentFileId );

void managerDeleteDealChildMain( DELETE_SHARE *deleteShare, int iaIndepentId );
int  managerDeleteDealDeleteDataMain( DELETE_SHARE *deleteShare, int iaStartOffset[], int iaEndOffset[], int iaIndepentId );
int managerDeleteDealDeleteDataLeftMain( DELETE_SHARE *deleteShare, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId );
int managerDeleteDealDeleteDataOtherMain( DELETE_SHARE *deleteShare, __int64 iaAddr, __int64 iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId );
int managerDeleteDealDeleteDataRightMain( DELETE_SHARE *deleteShare, KEY_POINT *saKey, int iaEndOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId );
int managerDeleteNodeMain( char *pcaKey, int iaKeyLen, int iaIndepentId );

/**********************************************************************
函数名称: managerDeleteDeal
函数功能: 管理端处理 delete 操作
参    数：
第    一：SOCKET               I
第    二：数据开始地址         I
第    三：数据结束地址         I
作    者: 李高文
建立时间: 20161022
**********************************************************************/
void managerDeleteDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd )
{
/*
接收数据结构：长度+操作+值区间（左开右开、左开右闭、左闭右开、左闭右闭）+KEY开始值长度+KEY开始值+KEY结束值长度+KEY结束值+用户名+密码
返回数据结构：操作结果+删除数据总数量
删除不响应中断，因为要兼顾中断，效率会慢很多。
*/
#define MANAGER_DELETE_VALUE_DEAL_SEND_ERROR( value ) \
{\
	memset( pcResult, 0x00, sizeof(pcResult) );\
	sprintf( pcResult, "%d", value ); \
	cResult = pcResult[0];\
	\
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );\
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, 1 );\
  \
  send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );\
	send( iaSocket, &cResult, 1, 0 );\
}
	char pcResult[INT_LEN + 1];
	char cResult;
	char pcSendBuf[INT_LEN + 1];
	char pcRows[INT64_LEN + 1];
	char pcTotalRows[INT64_LEN + 1];
	char pcKeyStartLen[KEY_VALUE_LENGTH + 1];
  char pcKeyEndLen[KEY_VALUE_LENGTH + 1];
  char pcKeyStart[MAX_KEY_LEN + 1];
  char pcKeyEnd[MAX_KEY_LEN + 1];
  char pcValueType[KEY_VALUE_LENGTH + 1];
  char pcKeyLen[KEY_VALUE_LENGTH + 1];
  char pcUser[NAME_LEN + 1];
	char pcPass[NAME_LEN + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
  int  iKeyStartLen;
	int  iKeyEndLen;
	__int64 llStartIndex;
	__int64 llStartIndexTmp;
	__int64 llSelectRows;
	int  iReturnRows;
	int  iRet;
	char *pcBufStart;
	char *pcBufEnd;
	DELETE_SHARE deleteStruct;
	pthread_t  childThreadId;
	char pcOperate[OPERATE_LEN + 1];
	int iRecvLen;
	int iTotalLen;
	int iDataLen;
	int iSendLen;
	int iTmp;
	int iIndex;
	
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
	
	/*取KEY或VALUE结束值长度*/
	memset( pcKeyEndLen, 0x00, sizeof(pcKeyEndLen) );
	memcpy( pcKeyEndLen, pcBufStart, KEY_VALUE_LENGTH );
	pcBufStart += KEY_VALUE_LENGTH;
	
	/*取KEY或VALUE结束值*/
	iKeyEndLen = atoi( pcKeyEndLen );
	memset( pcKeyEnd, 0x00, sizeof(pcKeyEnd) );
	memcpy( pcKeyEnd, pcBufStart, iKeyEndLen );
	pcBufStart += iKeyEndLen;
	
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
	
	logFmtSyn( LVNOR, "pcValueType[%s]pcKeyStart[%s]pcKeyEnd[%s]", pcValueType, pcKeyStart, pcKeyEnd );
	
	MANAGER_WRLOCK();
	
	/*检查用户名和密码是否匹配*/
	if (strcmp( pcgUser, pcUser ) || strcmp( pcgPass, pcPass ))
	{
		MANAGER_DELETE_VALUE_DEAL_SEND_ERROR( ERROR );
		MANAGER_WRUNLOCK();
		return;
	}
	
	/*设置共享数据*/
	memset( &deleteStruct, 0x00, sizeof(deleteStruct) );
	memcpy( deleteStruct.pcValueType, pcValueType, KEY_VALUE_LENGTH ); /*搜索条件的值区间（KEY或VALUE 左开右开、左开右闭、左闭右开、左闭右闭*/
	memcpy( deleteStruct.pcKeyStart, pcKeyStart, iKeyStartLen );      /*KEY 的开始值*/
	deleteStruct.iKeyStartLen = iKeyStartLen;                         /*KEY 的开始值长度*/
	memcpy( deleteStruct.pcKeyEnd, pcKeyEnd, iKeyEndLen );            /*KEY VALUE 的结束值*/
	deleteStruct.iKeyEndLen = iKeyEndLen;                             /*KEY VALUE 的结束值长度*/

  #if 1
	/*将内存中的所有数据写入到硬盘中*/
	for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
	{
		memset( deleteStruct.pcConnerKey, 0x00, sizeof(deleteStruct.pcConnerKey) );
    memset( &(deleteStruct.iConnerKeyLen), 0x00, sizeof(deleteStruct.iConnerKeyLen) );
    deleteStruct.iCornerSize = 0;
    managerDeleteDealChildMain( &deleteStruct, iTmp );
  }
  #endif
  
	/*调用函数来处理*/
	for (iTmp = 0; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
	{
		memset( deleteStruct.pcConnerKey, 0x00, sizeof(deleteStruct.pcConnerKey) );
    memset( &(deleteStruct.iConnerKeyLen), 0x00, sizeof(deleteStruct.iConnerKeyLen) );
    deleteStruct.iCornerSize = 0;
	  managerDeleteDealChild( &deleteStruct, iTmp );
  }
	
	MANAGER_WRUNLOCK();
	
	memset( pcTotalRows, 0x00, sizeof(pcTotalRows) );
  sprintf( pcTotalRows, "%0*lld", INT64_LEN, deleteStruct.llTotalRows );
  
  logFmtSyn( LVNOR, "%s", pcTotalRows );
  
  /*组发送数据*/
  memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
  
  sprintf( &cResult, "%d", SUCCESS );
  memcpy( pcSendBuf, &cResult, 1 );
  memcpy( pcSendBuf + 1, pcTotalRows, INT64_LEN );
  
  iSendLen = 1 + INT64_LEN;
  send( iaSocket, pcSendBuf, iSendLen, 0 );
	
}

/**********************************************************************
函数名称: managerDeleteDealChild
函数功能: 管理端处理 delete 操作
参    数：
第    一：共享结构体               I/O
作    者: 李高文
建立时间: 20161111
**********************************************************************/
void managerDeleteDealChild( DELETE_SHARE *deleteShare, int iaIndepentFileId )
{
  int  iStartOffsetRecord[20];   /*开始位置记录*/
	int  iEndOffsetRecord[20];     /*结束位置记录*/
	KEY_POINT key;                 /*其他级数据中的数据结构*/
	__int64  llDisc;
	__int64  llSize;
	__int64  llLoops;
	int  iRet;
	int  iTmp;
	
	llDisc = sgBase[iaIndepentFileId].llFirstDisc;
	llSize = sgBase[iaIndepentFileId].llFirstSize;
	llLoops = sgBase[iaIndepentFileId].llLoops;
	
	/*定位开始位置*/
	{
		/*初始化开始位置记录变量*/
	  for (iTmp = 0; iTmp < 20; iTmp++)
	  {
	  	iStartOffsetRecord[iTmp] = 0x00;
	  }
	
		/*如果是左开*/
	  if (!strcmp( deleteShare->pcValueType, KEY_OPEN_OPEN ) != !strcmp( deleteShare->pcValueType, KEY_OPEN_CLOSE ))
    {
  	  iRet = managerPositionLeftOpen( iStartOffsetRecord, deleteShare->pcKeyStart, llDisc, llSize, 1, llLoops, iaIndepentFileId );
  	  
  	  /*如果没有满足条件*/
  	  if (iRet == NOT_FOUND)
  	  {
  	  	logFmtSyn( LVNOR, "没有找到数据" );
        return ;
  	  }
    }
    else
    {
  	  iRet = managerPositionLeftClose( iStartOffsetRecord, deleteShare->pcKeyStart, llDisc, llSize, 1, llLoops, iaIndepentFileId );
  	  
  	  /*如果没有满足条件*/
  	  if (iRet == NOT_FOUND)
  	  {
  	  	logFmtSyn( LVNOR, "没有找到数据" );
        return ;
  	  }
    }
	}
	
	/*定位结束位置*/
	{
		/*初始化结束位置记录变量*/
	  for (iTmp = 0; iTmp < 20; iTmp++)
	  {
	  	iEndOffsetRecord[iTmp] = 0x00;
	  }
	
		/*如果是右开*/
	  if (!strcmp( deleteShare->pcValueType, KEY_OPEN_OPEN ) != !strcmp( deleteShare->pcValueType, KEY_CLOSE_OPEN ))
    {
  	  iRet = managerPositionRightOpen( iEndOffsetRecord, deleteShare->pcKeyEnd, llDisc, llSize, 1, llLoops, iaIndepentFileId );
  	  
  	  /*如果没有满足条件*/
  	  if (iRet == NOT_FOUND)
  	  {
  	  	logFmtSyn( LVNOR, "没有找到数据" );
        return ;
  	  }
    }
    else
    {
  	  iRet = managerPositionRightClose( iEndOffsetRecord, deleteShare->pcKeyEnd, llDisc, llSize, 1, llLoops, iaIndepentFileId );
  	  
  	  /*如果没有满足条件*/
  	  if (iRet == NOT_FOUND)
  	  {
  	  	logFmtSyn( LVNOR, "没有找到数据" );
        return ;
  	  }
    }
	}
		
	/*删除数据*/
	key.llChildAddr = sgBase[iaIndepentFileId].llFirstDisc;
	key.llChildSize = sgBase[iaIndepentFileId].llFirstSize;
	
	/*删除数据*/
	managerDeleteDealDeleteData( deleteShare,  iStartOffsetRecord, iEndOffsetRecord, iaIndepentFileId );
	
	return;
}

/**********************************************************************
函数名称: managerDeleteDealDeleteData
函数功能: 管理端处理 delete 中的数据删除
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
建立时间: 20161111
**********************************************************************/
int  managerDeleteDealDeleteData( DELETE_SHARE *deleteShare, int iaStartOffset[], int iaEndOffset[], int iaIndepentFileId )
{
	KEY_POINT *key;                /*其他级数据中的数据结构*/
	KEY_POINT *keyTmp;             /*其他级数据中的数据结构*/
	KEY_POINT keyOut;              /*其他级数据中的数据结构*/
  KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
  char pcKey[MAX_KEY_LEN];
  char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
  char pcDataBufTmp[BLOCK_SIZE]; /*级区中的数据临时变量*/
  char pcDataBufTmp2[BLOCK_SIZE]; /*级区中的数据临时变量*/
  int  iReplaceFlag = 0;
  int  iReplaceLeftFlag;
  int  iReplaceRightFlag;
  int  iStartOffsetRecord[20];   /*开始位置记录*/
  int  iEndOffsetRecord[20];     /*结束位置记录*/
  int  iIndex;
  __int64 iAddr;
  __int64 iAddrTmp;
  __int64 iSize;
  int  iTotalLoops;
  
  __int64 llChildSize;
  int  iTmp;
  int  iRet;
  
   /*取出当前级的数据*/
  iAddr = sgBase[iaIndepentFileId].llFirstDisc;
  iSize = sgBase[iaIndepentFileId].llFirstSize;
  iTotalLoops = sgBase[iaIndepentFileId].llLoops;
  
  /*搜索开始位置和结束位置不同的级*/
  for (iIndex = 1; iIndex <= iTotalLoops; iIndex++)
  {
  	if (iaStartOffset[iIndex] < iaEndOffset[iIndex] )
  	{
  		break;
  	}
  }
  
  /*如果最后的位置都相同，表示只有一个数据满足条件*/
  if (iIndex > iTotalLoops)
  {
  	/*取出该数据*/
  	{
  	  /*取出最后级的数据*/
      iAddr = sgBase[iaIndepentFileId].llFirstDisc;
      for (iTmp = 1; iTmp < iTotalLoops; iTmp++)
      {
      	memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
        getKeyBlockData( pcDataBuf, iAddr, iaIndepentFileId );
      
        key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iTmp] * sizeof(KEY_POINT));
      
        iAddr = key->llChildAddr;
      }
      
      memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
      getKeyBlockData( pcDataBuf, iAddr, iaIndepentFileId );
      keyData = (KEY_VALUE_POINT*)(pcDataBuf + iaStartOffset[iTotalLoops] * sizeof(KEY_VALUE_POINT));
      
      valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
      pcKey[keyData->llKeyLen] = 0x00;
  	}
  	
  	iRet = managerDeleteNode( pcKey, keyData->llKeyLen, iaIndepentFileId );
		if (iRet == SUCCESS)
		{
			(deleteShare->llTotalRows)++;
		}
		
		return SUCCESS;
  }
  
  memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
  getKeyBlockData( pcDataBuf, iAddr, iaIndepentFileId );
  
  /*取出当前级的数据*/
  for (iTmp = 2; iTmp < iIndex; iTmp++)
  {
  	memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iAddr, iaIndepentFileId );
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iTmp] * sizeof(KEY_POINT));
    
    memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
    
    /*备份上一级数据*/
    memcpy( pcDataBufTmp, pcDataBuf, BLOCK_SIZE );
    keyTmp = key;
    iAddrTmp = iAddr;
    
    iAddr = key->llChildAddr;
    iSize = key->llChildSize;
  }
  
  /*从该级开始，开始位置和结束位置不同*/

  /*处理开始位置的级，删除非边数据*/
  key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iIndex] * sizeof(KEY_POINT));
  managerDeleteDealDeleteDataLeft( deleteShare, key->llChildAddr, key->llChildSize, iaStartOffset, iIndex + 1, iTotalLoops, iaIndepentFileId );


  /*重新设置该结点*/
  key->llChildSize = iaStartOffset[iIndex + 1] + 1;
  setKeyBlockData( pcDataBuf, iAddr, iaIndepentFileId );
  
  /*处理其他位置的级，删除非边数据*/
  for (iTmp = iaStartOffset[iIndex] + 1; iTmp < iaEndOffset[iIndex]; iTmp++)
  {
    key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
    managerDeleteDealDeleteDataOther( deleteShare, key->llChildAddr, key->llChildSize, iIndex + 1, iTotalLoops, iaIndepentFileId );
  }


  /*处理结束位置的级，删除非边数据*/
  key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iIndex] * sizeof(KEY_POINT));
  managerDeleteDealDeleteDataRight( deleteShare, key, iaEndOffset, iIndex + 1, iTotalLoops, iaIndepentFileId );

  if (key->llChildSize == 0)
  {
  	freeKeyBlock( key->llChildAddr, iaIndepentFileId );

  	/*重新设置该区的值*/
    memset( pcDataBufTmp2, 0x00, sizeof(pcDataBufTmp) );
    memcpy( pcDataBufTmp2, pcDataBuf, ( iaStartOffset[iIndex] + 1 ) * sizeof(KEY_POINT) );
    memcpy( pcDataBufTmp2 + ( iaStartOffset[iIndex] + 1 ) * sizeof(KEY_POINT), pcDataBuf + ( iaEndOffset[iIndex] + 1 ) * sizeof(KEY_POINT), ( iSize - iaEndOffset[iIndex] - 1 )* sizeof(KEY_POINT) );
    setKeyBlockData( pcDataBufTmp2, iAddr, iaIndepentFileId );

  	/*如果是第一级*/
    if (iAddr == sgBase[iaIndepentFileId].llFirstDisc)
    {
    	sgBase[iaIndepentFileId].llFirstSize -= iaEndOffset[iIndex] - iaStartOffset[iIndex];
    }
    else
    {
    	keyTmp->llChildSize -= iaEndOffset[iIndex] - iaStartOffset[iIndex];
    	setKeyBlockData( pcDataBufTmp, iAddrTmp, iaIndepentFileId );
    }
  }
  else
  {
  	/*重新设置该区的值*/
    memset( pcDataBufTmp2, 0x00, sizeof(pcDataBufTmp) );
    memcpy( pcDataBufTmp2, pcDataBuf,  ( iaStartOffset[iIndex] + 1 ) * sizeof(KEY_POINT) );
    memcpy( pcDataBufTmp2 + ( iaStartOffset[iIndex] + 1 ) * sizeof(KEY_POINT), pcDataBuf + iaEndOffset[iIndex] * sizeof(KEY_POINT), ( iSize - iaEndOffset[iIndex] )* sizeof(KEY_POINT) );
    setKeyBlockData( pcDataBufTmp2, iAddr, iaIndepentFileId );
  
  	/*如果是第一级*/
    if (iAddr == sgBase[iaIndepentFileId].llFirstDisc)
    {
    	sgBase[iaIndepentFileId].llFirstSize -= iaEndOffset[iIndex] - 1 - iaStartOffset[iIndex];
    }
    else
    {
    	keyTmp->llChildSize -= iaEndOffset[iIndex] - 1 - iaStartOffset[iIndex];
    	setKeyBlockData( pcDataBufTmp, iAddrTmp, iaIndepentFileId );
    }
  }
    
  /*清理级数*/
	for (iIndex = 1; iIndex <= iTotalLoops; iIndex++)
  {
  	iAddr = sgBase[iaIndepentFileId].llFirstDisc;
    iSize = sgBase[iaIndepentFileId].llFirstSize;
    iTotalLoops = sgBase[iaIndepentFileId].llLoops;
  
  	if (iSize == 1)
  	{
  		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
      getKeyBlockData( pcDataBuf, iAddr, iaIndepentFileId );
      
      key = (KEY_POINT*)(pcDataBuf);
      
      freeKeyBlock( sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );
      
      /*重新设置第一个结点*/
      sgBase[iaIndepentFileId].llFirstDisc = key->llChildAddr;
      sgBase[iaIndepentFileId].llFirstSize = key->llChildSize;
      sgBase[iaIndepentFileId].llLoops--;
  	}
  	else
  	{
  		break;
  	}
  }

  /*删除边角数据*/
	for (iTmp = 0; iTmp < deleteShare->iCornerSize; iTmp++)
	{
		memset( pcKey, 0x00, sizeof(pcKey) );
		strcpy( pcKey, deleteShare->pcConnerKey[iTmp] );
		
		iRet = managerDeleteNode( pcKey, deleteShare->iConnerKeyLen[iTmp], iaIndepentFileId );

		if (iRet == SUCCESS)
		{
			(deleteShare->llTotalRows)++;
		}
	}
	
  return SUCCESS;
}

/**********************************************************************
函数名称: managerDeleteDealDeleteDataLeft
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
建立时间: 20170114
**********************************************************************/
int managerDeleteDealDeleteDataLeft( DELETE_SHARE *deleteShare, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId )
{
	char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
	char pcDataBufTmp[BLOCK_SIZE]; /*级区中的数据*/
	char pcKey[MAX_KEY_LEN];
	KEY_POINT *key;
	KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
	int  iTmp;
	int  iRet;
		
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
      
      strcpy( deleteShare->pcConnerKey[deleteShare->iCornerSize], pcKey );
      deleteShare->iConnerKeyLen[deleteShare->iCornerSize] = keyData->llKeyLen;
  	  
      (deleteShare->iCornerSize)++;
      
    }
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iaLoop] * sizeof(KEY_POINT));
    managerDeleteDealDeleteDataLeft( deleteShare, key->llChildAddr, key->llChildSize, iaStartOffset, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
    
    /*如果下一级不是最后一级，重新设置该结点*/
    
    if (iaLoop + 1 != iaTotalLoops)
    {
      key->llChildSize = iaStartOffset[iaLoop + 1] + 1;
    }
    
    for (iTmp = iaStartOffset[iaLoop] + 1; iTmp < iaSize; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      managerDeleteDealDeleteDataOther( deleteShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
      
      freeKeyBlock( key->llChildAddr, iaIndepentFileId );
    }
    
    /*重新设置下级数据*/
    memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
    memcpy( pcDataBufTmp, pcDataBuf, ( iaStartOffset[iaLoop] + 1 ) * sizeof(KEY_POINT) );
    setKeyBlockData( pcDataBufTmp, iaAddr, iaIndepentFileId );
	}
	
	return SUCCESS;
}

/**********************************************************************
函数名称: managerDeleteDealDeleteDataOther
函数功能: 管理端处理 delete 中的删除数据
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
int managerDeleteDealDeleteDataOther( DELETE_SHARE *deleteShare, __int64 iaAddr, __int64 iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId )
{
	KEY_POINT *key;                /*其他级数据中的数据结构*/
  KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
  char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
  int  iIndex;
  
	memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
  getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
  
  /*如果到了最后一级*/
  if (iaLoop == iaTotalLoops)
  {
    /*删除结点*/
    for (iIndex = 0; iIndex < iaSize; iIndex++)
    {
    	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iIndex * sizeof(KEY_VALUE_POINT));
    	
    	/*释放结点在 VALUE 文件中的空间*/
	    freeValueBlock( keyData->llKeyAddr, iaIndepentFileId );
	    freeValueBlock( keyData->llValueAddr, iaIndepentFileId );
	    (deleteShare->llTotalRows)++;
	    (sgBase[iaIndepentFileId].llKeyValueSize)--;
    }
  }
  else
  {
    /*删除结点*/
    for (iIndex = 0; iIndex < iaSize; iIndex++)
    {
    	key = (KEY_POINT*)(pcDataBuf + iIndex * sizeof(KEY_POINT));
    	
    	managerDeleteDealDeleteDataOther( deleteShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );    	
    	freeKeyBlock( key->llChildAddr, iaIndepentFileId );
    }
  }
  
  return SUCCESS;
}

/**********************************************************************
函数名称: managerDeleteDealDeleteDataRight
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
建立时间: 20170114
**********************************************************************/
int managerDeleteDealDeleteDataRight( DELETE_SHARE *deleteShare, KEY_POINT *saKey, int iaEndOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId )
{
	char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
	char pcDataBufTmp[BLOCK_SIZE]; /*级区中的数据*/
	char pcKey[MAX_KEY_LEN];
	KEY_POINT *key;
	KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
	int  iTmp;
	int  iRet;
	__int64 llChildSize;
	
	llChildSize = saKey->llChildSize;
	
	/*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, saKey->llChildAddr, iaIndepentFileId );
    
		for (iTmp = 0; iTmp <= iaEndOffset[iaLoop]; iTmp++ )
	  {
	  	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iTmp * sizeof(KEY_VALUE_POINT));
      
      /*释放结点在 VALUE 文件中的空间*/
	    freeValueBlock( keyData->llKeyAddr, iaIndepentFileId );
	    freeValueBlock( keyData->llValueAddr, iaIndepentFileId );
	    (deleteShare->llTotalRows)++;
	    (sgBase[iaIndepentFileId].llKeyValueSize)--;
      (saKey->llChildSize)--;
    }
    
    if (saKey->llChildSize > 0)
    {
    	memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
    	memcpy( pcDataBufTmp, pcDataBuf + (iaEndOffset[iaLoop] + 1) * sizeof(KEY_VALUE_POINT), ( llChildSize - iaEndOffset[iaLoop] - 1) * sizeof(KEY_VALUE_POINT)  );
      setKeyBlockData( pcDataBufTmp, saKey->llChildAddr, iaIndepentFileId );
      
      /*将最后的结点挤出*/
      saKey->llKeyLen = keyData->llKeyLen;
      saKey->llKeyAddr = keyData->llKeyAddr;
    }
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, saKey->llChildAddr, iaIndepentFileId );
    for (iTmp = 0; iTmp < iaEndOffset[iaLoop]; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      managerDeleteDealDeleteDataOther( deleteShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
      freeKeyBlock( key->llChildAddr, iaIndepentFileId );
    }
    
    saKey->llChildSize -= iaEndOffset[iaLoop];
    
    key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iaLoop] * sizeof(KEY_POINT));
    iRet = managerDeleteDealDeleteDataRight( deleteShare, key, iaEndOffset, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
    
    /*如果下一级的数据为0，释放*/
    if (key->llChildSize == 0)
    {
    	freeKeyBlock( key->llChildAddr, iaIndepentFileId );
    	(saKey->llChildSize)--;
    }
    
    /*如果当前级还有数据*/
    if (saKey->llChildSize > 0)
    {      
      /*将最后的结点挤出*/
      saKey->llKeyLen = key->llKeyLen;
      saKey->llKeyAddr = key->llKeyAddr;
      
      /*重新设置下级数据*/
      memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
      memcpy( pcDataBufTmp, pcDataBuf + iaEndOffset[iaLoop] * sizeof(KEY_POINT), ( llChildSize - iaEndOffset[iaLoop] ) * sizeof(KEY_POINT) );
      setKeyBlockData( pcDataBufTmp, saKey->llChildAddr, iaIndepentFileId );
    }
    
	}
	
	return SUCCESS;
}

/********************************************************
函数名称：managerDeleteNode
函数功能：删除索引数据 
入口参数：
第    一: 索引值                 I
返    回：0表示成功，其他表示失败 
作    者：李高文 
创建时间：20170114
修改时间： 
*********************************************************/
int managerDeleteNode( char *pcaKey, int iaKeyLen, int iaIndepentFileId )
{
	int iRet;
  
	if (sgBase[iaIndepentFileId].llFirstSize == 0)
  {
  	return NOT_FOUND;
  }
  
  /*如果三级以上*/
  if (sgBase[iaIndepentFileId].llLoops >= 3)
  {
  	iRet = deleteIndexNode( NULL, NULL, 1, pcaKey, iaKeyLen, iaIndepentFileId );
  }
  else
  /*如果只有两级*/
  if (sgBase[iaIndepentFileId].llLoops == 2)
  {
  	iRet = deleteIndexNodeTwoLoops( pcaKey, iaKeyLen, iaIndepentFileId );
  }
  else
  /*如果只有一级*/
  if (sgBase[iaIndepentFileId].llLoops <= 1)
  {
  	iRet = deleteIndexNodeOneLoops( pcaKey, iaKeyLen, iaIndepentFileId );
  }
  else
  {  
    iRet = ERROR;
  }
  
  if (iRet == SUCCESS)
  {
  	(sgBase[iaIndepentFileId].llKeyValueSize)--;
  }
  
  return iRet;
}

/**********************************************************************
函数名称: managerDeleteDealChild
函数功能: 管理端处理 delete 操作
参    数：
第    一：共享结构体               I/O
作    者: 李高文
建立时间: 20161111
**********************************************************************/
void managerDeleteDealChildMain( DELETE_SHARE *deleteShare, int iaIndepentId )
{
  int  iStartOffsetRecord[20];   /*开始位置记录*/
	int  iEndOffsetRecord[20];     /*结束位置记录*/
	KEY_POINT key;                 /*其他级数据中的数据结构*/
	__int64  llDisc;
	__int64  llSize;
	__int64  llLoops;
	int  iRet;
	int  iTmp;
	
	llDisc = sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain;
	llSize = sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain;
	llLoops = sgGlobalVariableMain[iaIndepentId].llgLoopsMain;
	
	/*定位开始位置*/
	{
		/*初始化开始位置记录变量*/
	  for (iTmp = 0; iTmp < 20; iTmp++)
	  {
	  	iStartOffsetRecord[iTmp] = 0x00;
	  }
	
		/*如果是左开*/
	  if (!strcmp( deleteShare->pcValueType, KEY_OPEN_OPEN ) != !strcmp( deleteShare->pcValueType, KEY_OPEN_CLOSE ))
    {
  	  iRet = managerPositionLeftOpenMain( iStartOffsetRecord, deleteShare->pcKeyStart, llDisc, llSize, 1, llLoops, iaIndepentId );
  	  
  	  /*如果没有满足条件*/
  	  if (iRet == NOT_FOUND)
  	  {
  	  	//logFmtSyn( LVNOR, "没有找到数据" );
        return ;
  	  }
    }
    else
    {
  	  iRet = managerPositionLeftCloseMain( iStartOffsetRecord, deleteShare->pcKeyStart, llDisc, llSize, 1, llLoops, iaIndepentId );
  	  
  	  /*如果没有满足条件*/
  	  if (iRet == NOT_FOUND)
  	  {
  	  	//logFmtSyn( LVNOR, "没有找到数据" );
        return ;
  	  }
    }
	}
	
	/*定位结束位置*/
	{
		/*初始化结束位置记录变量*/
	  for (iTmp = 0; iTmp < 20; iTmp++)
	  {
	  	iEndOffsetRecord[iTmp] = 0x00;
	  }
	
		/*如果是右开*/
	  if (!strcmp( deleteShare->pcValueType, KEY_OPEN_OPEN ) != !strcmp( deleteShare->pcValueType, KEY_CLOSE_OPEN ))
    {
  	  iRet = managerPositionRightOpenMain( iEndOffsetRecord, deleteShare->pcKeyEnd, llDisc, llSize, 1, llLoops, iaIndepentId );
  	  
  	  /*如果没有满足条件*/
  	  if (iRet == NOT_FOUND)
  	  {
  	  	//logFmtSyn( LVNOR, "没有找到数据" );
        return ;
  	  }
    }
    else
    {
  	  iRet = managerPositionRightCloseMain( iEndOffsetRecord, deleteShare->pcKeyEnd, llDisc, llSize, 1, llLoops, iaIndepentId );
  	  
  	  /*如果没有满足条件*/
  	  if (iRet == NOT_FOUND)
  	  {
  	  	//logFmtSyn( LVNOR, "没有找到数据" );
        return ;
  	  }
    }
	}
		
	/*删除数据*/
	key.llChildAddr = sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain;
	key.llChildSize = sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain;
	
	/*删除数据*/
	managerDeleteDealDeleteDataMain( deleteShare,  iStartOffsetRecord, iEndOffsetRecord, iaIndepentId );
	
	return;
}

/**********************************************************************
函数名称: managerDeleteDealDeleteData
函数功能: 管理端处理 delete 中的数据删除
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
建立时间: 20161111
**********************************************************************/
int  managerDeleteDealDeleteDataMain( DELETE_SHARE *deleteShare, int iaStartOffset[], int iaEndOffset[], int iaIndepentId )
{
	KEY_POINT *key;                /*其他级数据中的数据结构*/
	KEY_POINT *keyTmp;             /*其他级数据中的数据结构*/
	KEY_POINT keyOut;              /*其他级数据中的数据结构*/
  KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
  char pcKey[MAX_KEY_LEN];
  char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
  char pcDataBufTmp[BLOCK_SIZE]; /*级区中的数据临时变量*/
  char pcDataBufTmp2[BLOCK_SIZE]; /*级区中的数据临时变量*/
  int  iReplaceFlag = 0;
  int  iReplaceLeftFlag;
  int  iReplaceRightFlag;
  int  iStartOffsetRecord[20];   /*开始位置记录*/
  int  iEndOffsetRecord[20];     /*结束位置记录*/
  int  iIndex;
  __int64 iAddr;
  __int64 iAddrTmp;
  __int64 iSize;
  int  iTotalLoops;
  
  __int64 llChildSize;
  int  iTmp;
  int  iRet;
  
   /*取出当前级的数据*/
  iAddr = sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain;
  iSize = sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain;
  iTotalLoops = sgGlobalVariableMain[iaIndepentId].llgLoopsMain;
  
  /*搜索开始位置和结束位置不同的级*/
  for (iIndex = 1; iIndex <= iTotalLoops; iIndex++)
  {
  	if (iaStartOffset[iIndex] < iaEndOffset[iIndex] )
  	{
  		break;
  	}
  }
  
  /*如果最后的位置都相同，表示只有一个数据满足条件*/
  if (iIndex > iTotalLoops)
  {
  	/*取出该数据*/
  	{
  	  /*取出最后级的数据*/
      iAddr = sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain;
      for (iTmp = 1; iTmp < iTotalLoops; iTmp++)
      {
      	memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
        getKeyBlockDataMain( pcDataBuf, iAddr, iaIndepentId );
      
        key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iTmp] * sizeof(KEY_POINT));
      
        iAddr = key->llChildAddr;
      }
      
      memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
      getKeyBlockDataMain( pcDataBuf, iAddr, iaIndepentId );
      keyData = (KEY_VALUE_POINT*)(pcDataBuf + iaStartOffset[iTotalLoops] * sizeof(KEY_VALUE_POINT));
      
      valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
      pcKey[keyData->llKeyLen] = 0x00;
  	}
  	
  	iRet = managerDeleteNodeMain( pcKey, keyData->llKeyLen, iaIndepentId );
		if (iRet == SUCCESS)
		{
			(deleteShare->llTotalRows)++;
		}
		
		return SUCCESS;
  }
  
  memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
  getKeyBlockDataMain( pcDataBuf, iAddr, iaIndepentId );
  
  /*取出当前级的数据*/
  for (iTmp = 2; iTmp < iIndex; iTmp++)
  {
  	memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iAddr, iaIndepentId );
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iTmp] * sizeof(KEY_POINT));
    
    memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
    
    /*备份上一级数据*/
    memcpy( pcDataBufTmp, pcDataBuf, BLOCK_SIZE );
    keyTmp = key;
    iAddrTmp = iAddr;
    
    iAddr = key->llChildAddr;
    iSize = key->llChildSize;
  }
  
  /*从该级开始，开始位置和结束位置不同*/

  /*处理开始位置的级，删除非边数据*/
  key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iIndex] * sizeof(KEY_POINT));
  managerDeleteDealDeleteDataLeftMain( deleteShare, key->llChildAddr, key->llChildSize, iaStartOffset, iIndex + 1, iTotalLoops, iaIndepentId );


  /*重新设置该结点*/
  key->llChildSize = iaStartOffset[iIndex + 1] + 1;
  setKeyBlockDataMain( pcDataBuf, iAddr, iaIndepentId );
  
  /*处理其他位置的级，删除非边数据*/
  for (iTmp = iaStartOffset[iIndex] + 1; iTmp < iaEndOffset[iIndex]; iTmp++)
  {
    key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
    managerDeleteDealDeleteDataOtherMain( deleteShare, key->llChildAddr, key->llChildSize, iIndex + 1, iTotalLoops, iaIndepentId );
  }


  /*处理结束位置的级，删除非边数据*/
  key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iIndex] * sizeof(KEY_POINT));
  managerDeleteDealDeleteDataRightMain( deleteShare, key, iaEndOffset, iIndex + 1, iTotalLoops, iaIndepentId );

  if (key->llChildSize == 0)
  {
  	freeKeyBlockMain( key->llChildAddr, iaIndepentId );

  	/*重新设置该区的值*/
    memset( pcDataBufTmp2, 0x00, sizeof(pcDataBufTmp) );
    memcpy( pcDataBufTmp2, pcDataBuf, ( iaStartOffset[iIndex] + 1 ) * sizeof(KEY_POINT) );
    memcpy( pcDataBufTmp2 + ( iaStartOffset[iIndex] + 1 ) * sizeof(KEY_POINT), pcDataBuf + ( iaEndOffset[iIndex] + 1 ) * sizeof(KEY_POINT), ( iSize - iaEndOffset[iIndex] - 1 )* sizeof(KEY_POINT) );
    setKeyBlockDataMain( pcDataBufTmp2, iAddr, iaIndepentId );

  	/*如果是第一级*/
    if (iAddr == sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain)
    {
    	sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain -= iaEndOffset[iIndex] - iaStartOffset[iIndex];
    }
    else
    {
    	keyTmp->llChildSize -= iaEndOffset[iIndex] - iaStartOffset[iIndex];
    	setKeyBlockDataMain( pcDataBufTmp, iAddrTmp, iaIndepentId );
    }
  }
  else
  {
  	/*重新设置该区的值*/
    memset( pcDataBufTmp2, 0x00, sizeof(pcDataBufTmp) );
    memcpy( pcDataBufTmp2, pcDataBuf,  ( iaStartOffset[iIndex] + 1 ) * sizeof(KEY_POINT) );
    memcpy( pcDataBufTmp2 + ( iaStartOffset[iIndex] + 1 ) * sizeof(KEY_POINT), pcDataBuf + iaEndOffset[iIndex] * sizeof(KEY_POINT), ( iSize - iaEndOffset[iIndex] )* sizeof(KEY_POINT) );
    setKeyBlockDataMain( pcDataBufTmp2, iAddr, iaIndepentId );
  
  	/*如果是第一级*/
    if (iAddr == sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain)
    {
    	sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain -= iaEndOffset[iIndex] - 1 - iaStartOffset[iIndex];
    }
    else
    {
    	keyTmp->llChildSize -= iaEndOffset[iIndex] - 1 - iaStartOffset[iIndex];
    	setKeyBlockDataMain( pcDataBufTmp, iAddrTmp, iaIndepentId );
    }
  }
    
  /*清理级数*/
  #if 1
  /*取得第一级索引区所有数据*/
  GET_BLOCK_DATA_MAIN( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
  while (sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain == 1 && sgGlobalVariableMain[iaIndepentId].llgLoopsMain > 1)
  {
    /*取得当前唯一的数据*/
    key = (KEY_POINT*)(pcDataBuf);
  
    /*释放第一个结点*/
    freeKeyBlockMain( sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
  
    /*重新设置第一个结点*/
    sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain = key->llChildAddr;
    sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain = key->llChildSize;
    (sgGlobalVariableMain[iaIndepentId].llgLoopsMain)--;
    GET_BLOCK_DATA_MAIN( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
  }
  #else
	for (iIndex = 1; iIndex <= iTotalLoops; iIndex++)
  {
  	iAddr = sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain;
    iSize = sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain;
    iTotalLoops = sgGlobalVariableMain[iaIndepentId].llgLoopsMain;
  
  	if (iSize == 1 && iTotalLoops > 1)
  	{
  		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
      getKeyBlockDataMain( pcDataBuf, iAddr, iaIndepentId );
      
      key = (KEY_POINT*)(pcDataBuf);
      
      freeKeyBlockMain( sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
      
      /*重新设置第一个结点*/
      sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain = key->llChildAddr;
      sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain = key->llChildSize;
      sgGlobalVariableMain[iaIndepentId].llgLoopsMain--;
  	}
  	else
  	{
  		break;
  	}
  }
  #endif

  //logFmtSyn( LVNOR, "[%lld]", deleteShare->llTotalRows );
  //logFmtSyn( LVNOR, "[%lld][%lld][%lld]", sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain, sgGlobalVariableMain[iaIndepentId].llgLoopsMain );
  
  /*删除边角数据*/
	for (iTmp = 0; iTmp < deleteShare->iCornerSize; iTmp++)
	{
		memset( pcKey, 0x00, sizeof(pcKey) );
		strcpy( pcKey, deleteShare->pcConnerKey[iTmp] );
		//logFmtSyn( LVNOR, "pcKey[%s][%d]", pcKey, deleteShare->iCornerSize );
		iRet = managerDeleteNodeMain( pcKey, deleteShare->iConnerKeyLen[iTmp], iaIndepentId );
    //logFmtSyn( LVNOR, "iRet[%d]", iRet );
    
		if (iRet == SUCCESS)
		{
			(deleteShare->llTotalRows)++;
		}
	}
	
  return SUCCESS;
}

/**********************************************************************
函数名称: managerDeleteDealDeleteDataLeftMain
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
建立时间: 20170114
**********************************************************************/
int managerDeleteDealDeleteDataLeftMain( DELETE_SHARE *deleteShare, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId )
{
	char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
	char pcDataBufTmp[BLOCK_SIZE]; /*级区中的数据*/
	char pcKey[MAX_KEY_LEN];
	KEY_POINT *key;
	KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
	int  iTmp;
	int  iRet;
		
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
      
      strcpy( deleteShare->pcConnerKey[deleteShare->iCornerSize], pcKey );
      deleteShare->iConnerKeyLen[deleteShare->iCornerSize] = keyData->llKeyLen;
  	  
      (deleteShare->iCornerSize)++;
      
    }
    //logFmtSyn( LVNOR, "[%lld][%lld]", iaAddr, iaSize );
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iaLoop] * sizeof(KEY_POINT));
    managerDeleteDealDeleteDataLeftMain( deleteShare, key->llChildAddr, key->llChildSize, iaStartOffset, iaLoop + 1, iaTotalLoops, iaIndepentId );
    
    /*如果下一级不是最后一级，重新设置该结点*/
    #if 1
    if (iaLoop + 1 != iaTotalLoops)
    {
      key->llChildSize = iaStartOffset[iaLoop + 1] + 1;
    }
    #endif
    
    for (iTmp = iaStartOffset[iaLoop] + 1; iTmp < iaSize; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      managerDeleteDealDeleteDataOtherMain( deleteShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
      
      freeKeyBlockMain( key->llChildAddr, iaIndepentId );
    }
    
    /*重新设置下级数据*/
    memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
    memcpy( pcDataBufTmp, pcDataBuf, ( iaStartOffset[iaLoop] + 1 ) * sizeof(KEY_POINT) );
    setKeyBlockDataMain( pcDataBufTmp, iaAddr, iaIndepentId );
	}
	
	return SUCCESS;
}

/**********************************************************************
函数名称: managerDeleteDealDeleteDataOtherMain
函数功能: 管理端处理 delete 中的删除数据
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
int managerDeleteDealDeleteDataOtherMain( DELETE_SHARE *deleteShare, __int64 iaAddr, __int64 iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId )
{
	KEY_POINT *key;                /*其他级数据中的数据结构*/
  KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
  char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
  int  iIndex;
  
	memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
  getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
  
  /*如果到了最后一级*/
  if (iaLoop == iaTotalLoops)
  {
    /*删除结点*/
    for (iIndex = 0; iIndex < iaSize; iIndex++)
    {
    	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iIndex * sizeof(KEY_VALUE_POINT));
    	
    	/*释放结点在 VALUE 文件中的空间*/
	    freeValueBlockMain( keyData->llKeyAddr, iaIndepentId );
	    freeValueBlockMain( keyData->llValueAddr, iaIndepentId );
	    (deleteShare->llTotalRows)++;
	    (sgGlobalVariableMain[iaIndepentId].llgKeyValueSizeMain)--;
    }
  }
  else
  {
    /*删除结点*/
    for (iIndex = 0; iIndex < iaSize; iIndex++)
    {
    	key = (KEY_POINT*)(pcDataBuf + iIndex * sizeof(KEY_POINT));
    	
    	managerDeleteDealDeleteDataOtherMain( deleteShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );    	
    	freeKeyBlockMain( key->llChildAddr, iaIndepentId );
    }
  }
  
  return SUCCESS;
}

/**********************************************************************
函数名称: managerDeleteDealDeleteDataRightMain
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
建立时间: 20170114
**********************************************************************/
int managerDeleteDealDeleteDataRightMain( DELETE_SHARE *deleteShare, KEY_POINT *saKey, int iaEndOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId )
{
	char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
	char pcDataBufTmp[BLOCK_SIZE]; /*级区中的数据*/
	char pcKey[MAX_KEY_LEN];
	KEY_POINT *key;
	KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
	int  iTmp;
	int  iRet;
	__int64 llChildSize;
	
	llChildSize = saKey->llChildSize;
	
	/*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, saKey->llChildAddr, iaIndepentId );
    
		for (iTmp = 0; iTmp <= iaEndOffset[iaLoop]; iTmp++ )
	  {
	  	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iTmp * sizeof(KEY_VALUE_POINT));
      
      /*释放结点在 VALUE 文件中的空间*/
	    freeValueBlockMain( keyData->llKeyAddr, iaIndepentId );
	    freeValueBlockMain( keyData->llValueAddr, iaIndepentId );
	    (deleteShare->llTotalRows)++;
	    (sgGlobalVariableMain[iaIndepentId].llgKeyValueSizeMain)--;
      (saKey->llChildSize)--;
    }
    
    if (saKey->llChildSize > 0)
    {
    	memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
    	memcpy( pcDataBufTmp, pcDataBuf + (iaEndOffset[iaLoop] + 1) * sizeof(KEY_VALUE_POINT), ( llChildSize - iaEndOffset[iaLoop] - 1) * sizeof(KEY_VALUE_POINT)  );
      setKeyBlockDataMain( pcDataBufTmp, saKey->llChildAddr, iaIndepentId );
      
      /*将最后的结点挤出*/
      saKey->llKeyLen = keyData->llKeyLen;
      saKey->llKeyAddr = keyData->llKeyAddr;
    }
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, saKey->llChildAddr, iaIndepentId );
    for (iTmp = 0; iTmp < iaEndOffset[iaLoop]; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      managerDeleteDealDeleteDataOtherMain( deleteShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
      freeKeyBlockMain( key->llChildAddr, iaIndepentId );
    }
    
    saKey->llChildSize -= iaEndOffset[iaLoop];
    
    key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iaLoop] * sizeof(KEY_POINT));
    iRet = managerDeleteDealDeleteDataRightMain( deleteShare, key, iaEndOffset, iaLoop + 1, iaTotalLoops, iaIndepentId );
    
    /*如果下一级的数据为0，释放*/
    if (key->llChildSize == 0)
    {
    	freeKeyBlockMain( key->llChildAddr, iaIndepentId );
    	(saKey->llChildSize)--;
    }
    
    /*如果当前级还有数据*/
    if (saKey->llChildSize > 0)
    {      
      /*将最后的结点挤出*/
      saKey->llKeyLen = key->llKeyLen;
      saKey->llKeyAddr = key->llKeyAddr;
      
      /*重新设置下级数据*/
      memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
      memcpy( pcDataBufTmp, pcDataBuf + iaEndOffset[iaLoop] * sizeof(KEY_POINT), ( llChildSize - iaEndOffset[iaLoop] ) * sizeof(KEY_POINT) );
      setKeyBlockDataMain( pcDataBufTmp, saKey->llChildAddr, iaIndepentId );
    }
    
	}
	
	return SUCCESS;
}

/********************************************************
函数名称：managerDeleteNodeMain
函数功能：删除索引数据 
入口参数：
第    一: 索引值                 I
返    回：0表示成功，其他表示失败 
作    者：李高文 
创建时间：20170114
修改时间： 
*********************************************************/
int managerDeleteNodeMain( char *pcaKey, int iaKeyLen, int iaIndepentId )
{
	int iRet;
  
	if (sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain == 0)
  {
  	return NOT_FOUND;
  }
  
  /*如果三级以上*/
  if (sgGlobalVariableMain[iaIndepentId].llgLoopsMain >= 3)
  {
  	iRet = deleteIndexNode( NULL, NULL, 1, pcaKey, iaKeyLen, iaIndepentId );
  }
  else
  /*如果只有两级*/
  if (sgGlobalVariableMain[iaIndepentId].llgLoopsMain == 2)
  {
  	iRet = deleteIndexNodeTwoLoops( pcaKey, iaKeyLen, iaIndepentId );
  }
  else
  /*如果只有一级*/
  if (sgGlobalVariableMain[iaIndepentId].llgLoopsMain <= 1)
  {
  	iRet = deleteIndexNodeOneLoopsMain( pcaKey, iaKeyLen, iaIndepentId );
  }
  else
  {  
    iRet = ERROR;
  }
  
  if (iRet == SUCCESS)
  {
  	(sgGlobalVariableMain[iaIndepentId].llgKeyValueSizeMain)--;
  }
  
  return iRet;
}
