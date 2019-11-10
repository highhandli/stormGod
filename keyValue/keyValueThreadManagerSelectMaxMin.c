#define __KEY_VALUE_THREAD_MANAGER_SELECT_MAX_MIN_C

/*
负责处理 MANAGER 中的 SELECT 语句
*/

#ifndef __COMMON_HEAD_H
  #include "commonHead.h"
#endif

void managerSelectMinDeal( int iaSocket );
void managerSelectMinDealResult( __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, KEY_VALUE_POINT *keyDataOut, int iaIndepentFileId );
void managerSelectMinDealResultMain( __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, KEY_VALUE_POINT *keyDataOut, int iaIndepentId );
void managerSelectMaxDeal( int iaSocket );
void managerSelectMaxDealResult( __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, KEY_VALUE_POINT *keyDataOut, int iaIndepentFileId );
void managerSelectMaxDealResultMain( __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, KEY_VALUE_POINT *keyDataOut, int iaIndepentId );

/**********************************************************************
函数名称: managerSelectMinDeal
函数功能: 管理端处理 select min 操作
参    数：
第    一：SOCKET               I
第    二：数据开始地址         I
第    三：数据结束地址         I
作    者: 李高文
建立时间: 20161022
**********************************************************************/
void managerSelectMinDeal( int iaSocket )
{
/*
接收数据结构：长度+操作
返回数据结构：操作结果+KEY值长度+KEY值+VALUE值长度+VALUE值
*/
#define MANAGER_SELECT_MIN_DEAL_SEND_ERROR( value ) \
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
	char pcSendBuf[1 + MAX_KEY_LEN + KEY_VALUE_LENGTH + MAX_VALUE_LEN + KEY_VALUE_LENGTH + 1];
	char cResult;
	KEY_VALUE_POINT keyData;
	char pcKey[MAX_KEY_LEN + 1];
	char pcValue[MAX_VALUE_LEN + 1];
	char pcKeyMain[MAX_KEY_LEN + 1];
	char pcValueMain[MAX_VALUE_LEN + 1];
	char pcKeyLen[KEY_VALUE_LENGTH + 1];
	char pcValueLen[KEY_VALUE_LENGTH + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	char pcMinKey[INDEPENDENT_FILE_SIZE][MAX_KEY_LEN + 1];
	char pcMinValue[INDEPENDENT_FILE_SIZE][MAX_VALUE_LEN + 1];
	//char pcMinKeyMain[INDEPENDENT_SIZE][MAX_KEY_LEN + 1];在windows下面，不能静态分配太多内存，就会出现函数执行不了。
	//char pcMinValueMain[INDEPENDENT_SIZE][MAX_VALUE_LEN + 1];
	char *pcMinKeyMain[INDEPENDENT_SIZE];
	char *pcMinValueMain[INDEPENDENT_SIZE];
	__int64  llDisc;
	__int64  llSize;
	__int64  llLoops;
	int  iKeyLen;
	int  iValueLen;
	int  iSendLen;
	int  iRet;
	int  iTmp;
	int  iIndepentId;
	int  iIndepentFileId;
  
  llSize = 0;
  for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
  {
  	llSize += sgBase[iTmp].llKeyValueSize;
  }
	
	if (llSize == 0)
	{
		MANAGER_SELECT_MIN_DEAL_SEND_ERROR( ERROR );
		return;
	}
	
	for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
	{
	  pcMinKeyMain[iTmp] = malloc( MAX_KEY_LEN + 1 );
	  if (pcMinKeyMain[iTmp] == NULL)
	  {
	  	return;
	  }
	  memset( pcMinKeyMain[iTmp], 0x00, MAX_KEY_LEN + 1 );
	  
	  pcMinValueMain[iTmp] = malloc( MAX_VALUE_LEN + 1 );
	  if (pcMinValueMain[iTmp] == NULL)
	  {
	  	return;
	  }
	  memset( pcMinValueMain[iTmp], 0x00, MAX_VALUE_LEN + 1 );
  }
  
	/*从多个数据文件取出多个最小值*/
	memset( pcMinKey, 0x00, sizeof(pcMinKey) );
	memset( pcMinValue, 0x00, sizeof(pcMinValue) );

	MANAGER_RDLOCK();
	//logFmtSyn( LVNOR, "a" );
	for (iIndepentId = 0; iIndepentId < INDEPENDENT_SIZE; iIndepentId++)
	{
		
	  llDisc = sgGlobalVariableMain[iIndepentId].llgFirstDiscMain;
	  llSize = sgGlobalVariableMain[iIndepentId].llgFirstSizeMain;
	  llLoops = sgGlobalVariableMain[iIndepentId].llgLoopsMain;
	  //logFmtSyn( LVNOR, "%d:%d:%d", iIndepentId, sgGlobalVariableMain[iIndepentId].llgFirstDiscMain, sgGlobalVariableMain[iIndepentId].llgFirstSizeMain, sgGlobalVariableMain[iIndepentId].llgLoopsMain );
	  memset( &keyData, 0x00, sizeof(keyData) );	
	  managerSelectMinDealResultMain( llDisc, llSize, 1, llLoops, &keyData, iIndepentId );
	  
    valueToBufMain( pcMinKeyMain[iIndepentId], keyData.llKeyAddr, keyData.llKeyLen, iIndepentId );
    valueToBufMain( pcMinValueMain[iIndepentId], keyData.llValueAddr, keyData.llValueLen, iIndepentId );
    
    //logFmtSyn( LVNOR, "key[%s]value[%s]", pcMinKeyMain[iIndepentId], pcMinValueMain[iIndepentId] );
  }
  //logFmtSyn( LVNOR, "a" );
	for (iIndepentFileId = 0; iIndepentFileId < INDEPENDENT_FILE_SIZE; iIndepentFileId++)
	{
	  llDisc = sgBase[iIndepentFileId].llFirstDisc;
	  llSize = sgBase[iIndepentFileId].llFirstSize;
	  llLoops = sgBase[iIndepentFileId].llLoops;
	  
	  memset( &keyData, 0x00, sizeof(keyData) );	
	  managerSelectMinDealResult( llDisc, llSize, 1, llLoops, &keyData, iIndepentFileId );

    valueToBuf( pcMinKey[iIndepentFileId], keyData.llKeyAddr, keyData.llKeyLen, iIndepentFileId );
    valueToBuf( pcMinValue[iIndepentFileId], keyData.llValueAddr, keyData.llValueLen, iIndepentFileId );
  }
  //logFmtSyn( LVNOR, "a" );
  MANAGER_RDUNLOCK();
  
  memset( pcKeyMain, 0x00, sizeof(pcKeyMain) );
  strcpy( pcKeyMain, pcMinKeyMain[0] );
  
  memset( pcValueMain, 0x00, sizeof(pcValueMain) );
  strcpy( pcValueMain, pcMinValueMain[0] );
  
  for (iTmp = 1; iTmp < INDEPENDENT_SIZE; iTmp++)
	{
		if (strcmp( pcMinKeyMain[iTmp], pcKeyMain ) < 0)
		{
			memset( pcKeyMain, 0x00, sizeof(pcKeyMain) );
      strcpy( pcKeyMain, pcMinKeyMain[iTmp] );
      
      memset( pcValueMain, 0x00, sizeof(pcValueMain) );
      strcpy( pcValueMain, pcMinValueMain[iTmp] );
		}
	}
	
	//logFmtSyn( LVNOR, "key[%s]value[%s]", pcKeyMain, pcValueMain );
  
  /*再从多个数据取出最小的一个*/
  memset( pcKey, 0x00, sizeof(pcKey) );
  strcpy( pcKey, pcMinKey[0] );
  
  memset( pcValue, 0x00, sizeof(pcValue) );
  strcpy( pcValue, pcMinValue[0] );
  
  for (iTmp = 1; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
	{
		if (strcmp( pcMinKey[iTmp], pcKey ) < 0)
		{
			memset( pcKey, 0x00, sizeof(pcKey) );
      strcpy( pcKey, pcMinKey[iTmp] );
      
      memset( pcValue, 0x00, sizeof(pcValue) );
      strcpy( pcValue, pcMinValue[iTmp] );
		}
	}
	
	//logFmtSyn( LVNOR, "key[%s]value[%s]", pcKey, pcValue );
	
	if (strlen( pcKey ) != 0 && strlen( pcKeyMain ) != 0)
	{
		if (strcmp( pcKeyMain, pcKey ) < 0)
	  {
	  	memset( pcKey, 0x00, sizeof(pcKey) );
      strcpy( pcKey, pcKeyMain );
      
      memset( pcValue, 0x00, sizeof(pcValue) );
      strcpy( pcValue, pcValueMain );
	  }
	}
	else
	if (strlen( pcKey ) == 0 && strlen( pcKeyMain ) != 0)
	{
		memset( pcKey, 0x00, sizeof(pcKey) );
    strcpy( pcKey, pcKeyMain );
      
    memset( pcValue, 0x00, sizeof(pcValue) );
    strcpy( pcValue, pcValueMain );
  }
  
	//logFmtSyn( LVNOR, "key[%s]value[%s]", pcKey, pcValue );
	
	for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
	{
	  free( pcMinKeyMain[iTmp] );
	  free( pcMinValueMain[iTmp] );
  }
	
	/*组发送数据*/
  memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
  iKeyLen = strlen( pcKey );
  memset( pcKeyLen, 0x00, sizeof(pcKeyLen) );
  sprintf( pcKeyLen, "%0*d", KEY_VALUE_LENGTH, iKeyLen );
  
  iValueLen = strlen( pcValue );
  memset( pcValueLen, 0x00, sizeof(pcValueLen) );
  sprintf( pcValueLen, "%0*d", KEY_VALUE_LENGTH, iValueLen );
  
  sprintf( &cResult, "%d", SUCCESS );
  memcpy( pcSendBuf, &cResult, 1 );
  memcpy( pcSendBuf + 1, pcKeyLen, KEY_VALUE_LENGTH );
  memcpy( pcSendBuf + 1 + KEY_VALUE_LENGTH, pcKey, iKeyLen );
  memcpy( pcSendBuf + 1 + KEY_VALUE_LENGTH + iKeyLen, pcValueLen, KEY_VALUE_LENGTH );
  memcpy( pcSendBuf + 1 + KEY_VALUE_LENGTH + iKeyLen + KEY_VALUE_LENGTH, pcValue, iValueLen );
  
  iSendLen = 1 + KEY_VALUE_LENGTH + iValueLen + KEY_VALUE_LENGTH + iValueLen;
  
  memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
  
  send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
  send( iaSocket, pcSendBuf, iSendLen, 0 );
}

/**********************************************************************
函数名称: managerSelectMinDealResult
函数功能: 管理端处理 select min 中的数据
参    数：
第    一：数据地址     I
第    二：数量         I
第    三：级数         I
第    四：总级数       I
第    五：数据结构体   O
返    回：
作    者: 李高文
建立时间: 20161106
**********************************************************************/
void managerSelectMinDealResult( __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, KEY_VALUE_POINT *keyDataOut, int iaIndepentFileId )
{
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	KEY_POINT *key;                /*其他级数据中的数据结构*/
  KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
  char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
  int  iRet;
  
  if (iaSize == 0)
  {
  	return;
  }
  
  /*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
		/*取得最后一级所有数据*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
    memcpy( keyDataOut, pcDataBuf, sizeof(KEY_VALUE_POINT) );
    
    return;
	}
  /*不是最后一级*/
	else
	{
		/*取得其他级所有数据*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
    key = (KEY_POINT*)(pcDataBuf);
    
    return managerSelectMinDealResult( key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, keyDataOut, iaIndepentFileId );
  }
}

/**********************************************************************
函数名称: managerSelectMinDealResultMain
函数功能: 管理端处理 select min 中的数据
参    数：
第    一：数据地址     I
第    二：数量         I
第    三：级数         I
第    四：总级数       I
第    五：数据结构体   O
返    回：
作    者: 李高文
建立时间: 20161106
**********************************************************************/
void managerSelectMinDealResultMain( __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, KEY_VALUE_POINT *keyDataOut, int iaIndepentId )
{
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	KEY_POINT *key;                /*其他级数据中的数据结构*/
  KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
  char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
  int  iRet;
  
  if (iaSize == 0)
  {
  	return;
  }
  //logFmtSyn( LVNOR, "a" );
  /*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
		/*取得最后一级所有数据*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    memcpy( keyDataOut, pcDataBuf, sizeof(KEY_VALUE_POINT) );
    
    return;
	}
  /*不是最后一级*/
	else
	{
		/*取得其他级所有数据*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    key = (KEY_POINT*)(pcDataBuf);
    //logFmtSyn( LVNOR, "a" );
    return managerSelectMinDealResultMain( key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, keyDataOut, iaIndepentId );
  }
}

/**********************************************************************
函数名称: managerSelectMaxDeal
函数功能: 管理端处理 select max 操作
参    数：
第    一：SOCKET               I
第    二：数据开始地址         I
第    三：数据结束地址         I
作    者: 李高文
建立时间: 20161022
**********************************************************************/
void managerSelectMaxDeal( int iaSocket )
{
#define MANAGER_SELECT_MAX_DEAL_SEND_ERROR( value ) \
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
	char pcSendBuf[1 + MAX_KEY_LEN + KEY_VALUE_LENGTH + MAX_VALUE_LEN + KEY_VALUE_LENGTH + 1];
	char cResult;
	KEY_VALUE_POINT keyData;
	char pcKey[MAX_KEY_LEN + 1];
	char pcValue[MAX_KEY_LEN + 1];
	char pcKeyMain[MAX_KEY_LEN + 1];
	char pcValueMain[MAX_KEY_LEN + 1];
	char pcKeyLen[KEY_VALUE_LENGTH + 1];
	char pcValueLen[KEY_VALUE_LENGTH + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	char pcMaxKey[INDEPENDENT_FILE_SIZE][MAX_KEY_LEN + 1];
	char pcMaxValue[INDEPENDENT_FILE_SIZE][MAX_VALUE_LEN + 1];
	//char pcMaxKeyMain[INDEPENDENT_SIZE][MAX_KEY_LEN + 1];
	//char pcMaxValueMain[INDEPENDENT_SIZE][MAX_VALUE_LEN + 1];
	char *pcMaxKeyMain[INDEPENDENT_SIZE];
	char *pcMaxValueMain[INDEPENDENT_SIZE];
	__int64  llDisc;
	__int64  llSize;
	__int64  llLoops;
	int  iKeyLen;
	int  iValueLen;
	int  iRet;
	int  iTmp;
	int  iSendLen;
	int  iIndepentId;
	int  iIndepentFileId;
  
  llSize = 0;
  for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
  {
  	llSize += sgBase[iTmp].llKeyValueSize;
  }
	
	if (llSize == 0)
	{
		MANAGER_SELECT_MAX_DEAL_SEND_ERROR( ERROR );
		return;
	}

	for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
	{
	  pcMaxKeyMain[iTmp] = malloc( MAX_KEY_LEN + 1 );
	  if (pcMaxKeyMain[iTmp] == NULL)
	  {
	  	return;
	  }
	  memset( pcMaxKeyMain[iTmp], 0x00, MAX_KEY_LEN + 1 );
	  
	  pcMaxValueMain[iTmp] = malloc( MAX_VALUE_LEN + 1 );
	  if (pcMaxValueMain[iTmp] == NULL)
	  {
	  	return;
	  }
	  memset( pcMaxValueMain[iTmp], 0x00, MAX_VALUE_LEN + 1 );
  }

	/*从多个数据文件取出多个最大值*/
	memset( pcMaxKey, 0x00, sizeof(pcMaxKey) );
	memset( pcMaxValue, 0x00, sizeof(pcMaxValue) );
	MANAGER_RDLOCK();

	for (iIndepentId = 0; iIndepentId < INDEPENDENT_SIZE; iIndepentId++)
	{
	  llDisc = sgGlobalVariableMain[iIndepentId].llgFirstDiscMain;
	  llSize = sgGlobalVariableMain[iIndepentId].llgFirstSizeMain;
	  llLoops = sgGlobalVariableMain[iIndepentId].llgLoopsMain;

	  memset( &keyData, 0x00, sizeof(keyData) );	
	  managerSelectMaxDealResultMain( llDisc, llSize, 1, llLoops, &keyData, iIndepentId );

    valueToBufMain( pcMaxKeyMain[iIndepentId], keyData.llKeyAddr, keyData.llKeyLen, iIndepentId );
    valueToBufMain( pcMaxValueMain[iIndepentId], keyData.llValueAddr, keyData.llValueLen, iIndepentId );
  }

	for (iIndepentFileId = 0; iIndepentFileId < INDEPENDENT_FILE_SIZE; iIndepentFileId++)
	{
	  llDisc = sgBase[iIndepentFileId].llFirstDisc;
	  llSize = sgBase[iIndepentFileId].llFirstSize;
	  llLoops = sgBase[iIndepentFileId].llLoops;
	  
	  memset( &keyData, 0x00, sizeof(keyData) );	
	  managerSelectMaxDealResult( llDisc, llSize, 1, llLoops, &keyData, iIndepentFileId );
	  
    valueToBuf( pcMaxKey[iIndepentFileId], keyData.llKeyAddr, keyData.llKeyLen, iIndepentFileId );
    valueToBuf( pcMaxValue[iIndepentFileId], keyData.llValueAddr, keyData.llValueLen, iIndepentFileId );
  }
  MANAGER_RDUNLOCK();

  memset( pcKeyMain, 0x00, sizeof(pcKeyMain) );
  strcpy( pcKeyMain, pcMaxKeyMain[0] );
  
  memset( pcValueMain, 0x00, sizeof(pcValueMain) );
  strcpy( pcValueMain, pcMaxValueMain[0] );
  
  for (iTmp = 1; iTmp < INDEPENDENT_SIZE; iTmp++)
	{
		if (strcmp( pcMaxKeyMain[iTmp], pcKeyMain ) > 0)
		{
			memset( pcKeyMain, 0x00, sizeof(pcKeyMain) );
      strcpy( pcKeyMain, pcMaxKeyMain[iTmp] );
      
      memset( pcValueMain, 0x00, sizeof(pcValueMain) );
      strcpy( pcValueMain, pcMaxValueMain[iTmp] );
		}
	}

  /*再从多个数据取出最大的一个*/
  memset( pcKey, 0x00, sizeof(pcKey) );
  strcpy( pcKey, pcMaxKey[0] );
  
  memset( pcValue, 0x00, sizeof(pcValue) );
  strcpy( pcValue, pcMaxValue[0] );

  for (iTmp = 1; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
	{
		if (strcmp( pcMaxKey[iTmp], pcKey ) > 0)
		{
			memset( pcKey, 0x00, sizeof(pcKey) );
      strcpy( pcKey, pcMaxKey[iTmp] );
      
      memset( pcValue, 0x00, sizeof(pcValue) );
      strcpy( pcValue, pcMaxValue[iTmp] );
		}
	}

	if (strlen( pcKey ) != 0 && strlen( pcKeyMain ) != 0)
	{
		if (strcmp( pcKeyMain, pcKey ) > 0)
	  {
	  	memset( pcKey, 0x00, sizeof(pcKey) );
      strcpy( pcKey, pcKeyMain );
      
      memset( pcValue, 0x00, sizeof(pcValue) );
      strcpy( pcValue, pcValueMain );
	  }
	}
	else
	if (strlen( pcKey ) == 0 && strlen( pcKeyMain ) != 0)
	{
		memset( pcKey, 0x00, sizeof(pcKey) );
    strcpy( pcKey, pcKeyMain );
      
    memset( pcValue, 0x00, sizeof(pcValue) );
    strcpy( pcValue, pcValueMain );
  }

	for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
	{
	  free( pcMaxKeyMain[iTmp] );
	  free( pcMaxValueMain[iTmp] );
  }
	
	/*组发送数据*/
  memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
  iKeyLen = strlen( pcKey );
  memset( pcKeyLen, 0x00, sizeof(pcKeyLen) );
  sprintf( pcKeyLen, "%0*d", KEY_VALUE_LENGTH, iKeyLen );
  
  iValueLen = strlen( pcValue );
  memset( pcValueLen, 0x00, sizeof(pcValueLen) );
  sprintf( pcValueLen, "%0*d", KEY_VALUE_LENGTH, iValueLen );
  
  sprintf( &cResult, "%d", SUCCESS );
  memcpy( pcSendBuf, &cResult, 1 );
  memcpy( pcSendBuf + 1, pcKeyLen, KEY_VALUE_LENGTH );
  memcpy( pcSendBuf + 1 + KEY_VALUE_LENGTH, pcKey, iKeyLen );
  memcpy( pcSendBuf + 1 + KEY_VALUE_LENGTH + iKeyLen, pcValueLen, KEY_VALUE_LENGTH );
  memcpy( pcSendBuf + 1 + KEY_VALUE_LENGTH + iKeyLen + KEY_VALUE_LENGTH, pcValue, iValueLen );
  
  iSendLen = 1 + KEY_VALUE_LENGTH + iValueLen + KEY_VALUE_LENGTH + iValueLen;
  
  memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
  
  send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
  send( iaSocket, pcSendBuf, iSendLen, 0 );
}

/**********************************************************************
函数名称: managerSelectMaxDealResult
函数功能: 管理端处理 select max 中的数据
参    数：
第    一：数据地址     I
第    二：数量         I
第    三：级数         I
第    四：总级数       I
第    五：数据结构体   O
返    回：
作    者: 李高文
建立时间: 20161106
**********************************************************************/
void managerSelectMaxDealResult( __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, KEY_VALUE_POINT *keyDataOut, int iaIndepentFileId )
{
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	KEY_POINT *key;                /*其他级数据中的数据结构*/
  KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
  char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
  int  iRet;
  
  if (iaSize == 0)
  {
  	return;
  }
  
  /*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
		/*取得最后一级所有数据*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
    memcpy( keyDataOut, pcDataBuf + (iaSize  - 1)* sizeof(KEY_VALUE_POINT) , sizeof(KEY_VALUE_POINT) );
    
    return;
	}
  /*不是最后一级*/
	else
	{
		/*取得其他级所有数据*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
    key = (KEY_POINT*)(pcDataBuf + (iaSize  - 1)* sizeof(KEY_POINT));
    
    return managerSelectMaxDealResult( key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, keyDataOut, iaIndepentFileId );
  }
}

/**********************************************************************
函数名称: managerSelectMaxDealResultMain
函数功能: 管理端处理 select max 中的数据
参    数：
第    一：数据地址     I
第    二：数量         I
第    三：级数         I
第    四：总级数       I
第    五：数据结构体   O
返    回：
作    者: 李高文
建立时间: 20170217
**********************************************************************/
void managerSelectMaxDealResultMain( __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, KEY_VALUE_POINT *keyDataOut, int iaIndepentId )
{
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	KEY_POINT *key;                /*其他级数据中的数据结构*/
  KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
  char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
  int  iRet;
  
  if (iaSize == 0)
  {
  	return;
  }
  
  /*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
		/*取得最后一级所有数据*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    memcpy( keyDataOut, pcDataBuf + (iaSize  - 1)* sizeof(KEY_VALUE_POINT) , sizeof(KEY_VALUE_POINT) );
    
    return;
	}
  /*不是最后一级*/
	else
	{
		/*取得其他级所有数据*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    key = (KEY_POINT*)(pcDataBuf + (iaSize  - 1)* sizeof(KEY_POINT));
    
    return managerSelectMaxDealResultMain( key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, keyDataOut, iaIndepentId );
  }
}

