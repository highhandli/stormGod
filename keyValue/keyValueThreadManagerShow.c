#define __KEY_VALUE_THREAD_MANAGER_SHOW_C

/*
负责处理 MANAGER 中的 SHOW 语句
*/

#ifndef __COMMON_HEAD_H
  #include "commonHead.h"
#endif

void managerShowRamsDeal( int iaSocket );
void managerShowDiscsDeal( int iaSocket );
void managerShowThreadsDeal( int iaSocket );
void managerShowKeysDeal( int iaSocket );
void managerShowLoopsDeal( int iaSocket );

/**********************************************************************
函数名称: managerShowRamsDeal
函数功能: 管理端处理 show rams 操作
参    数：
第    一：SOCKET               I
第    二：数据开始地址         I
第    三：数据结束地址         I
作    者: 李高文
建立时间: 20161022
**********************************************************************/
void managerShowRamsDeal( int iaSocket )
{
/*
接收数据结构：长度+操作
返回数据结构：操作结果+KEY文件缓冲区总数量 + KEY文件缓冲区的使用偏移量（单位 B） + VALUE文件缓冲区总数量 + VALUE文件缓冲区的使用偏移量（单位 B）
*/
	char cResult;
	char pcSendBuf[1 + INT64_LEN + INT64_LEN + INT64_LEN + INT64_LEN + 1];
	char pcKeyTotal[INT64_LEN + 1];
	char pcKeyUsed[INT64_LEN + 1];
	char pcValueTotal[INT64_LEN + 1];
	char pcValueUsed[INT64_LEN + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	int  iSendLen;
	__int64 llRam;
  __int64 llValue;
  int iTmp;
	
	MANAGER_RDLOCK();

  /*取KEY文件缓冲区总数量*/
  memset( pcKeyTotal, 0x00, sizeof(pcKeyTotal) );
  sprintf( pcKeyTotal, "%0*lld", INT64_LEN, llgKeyRamSizeMain );
  
  llRam = 0;
  llValue = 0;
  for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
  {
  	llRam += sgGlobalVariableMain[iTmp].llgKeyRamUsedMain;
  	llValue += sgGlobalVariableMain[iTmp].llgValueRamUsedMain;
  }
  
  /*取KEY文件缓冲区的使用偏移量*/
  memset( pcKeyUsed, 0x00, sizeof(pcKeyUsed) );
  sprintf( pcKeyUsed, "%0*lld", INT64_LEN, llRam );
  
  /*取VALUE文件缓冲区总数量*/
  memset( pcValueTotal, 0x00, sizeof(pcValueTotal) );
  sprintf( pcValueTotal, "%0*lld", INT64_LEN, llgValueRamSizeMain );
  
  /*取VALUE文件缓冲区的使用偏移量*/
  memset( pcValueUsed, 0x00, sizeof(pcValueUsed) );
  sprintf( pcValueUsed, "%0*lld", INT64_LEN, llValue );
  
  MANAGER_RDUNLOCK();
  
  /*组发送数据*/
  memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
  
  sprintf( &cResult, "%d", SUCCESS );
  memcpy( pcSendBuf, &cResult, 1 );
  memcpy( pcSendBuf + 1, pcKeyTotal, INT64_LEN );
  memcpy( pcSendBuf + 1 + INT64_LEN, pcKeyUsed, INT64_LEN );
  memcpy( pcSendBuf + 1 + INT64_LEN + INT64_LEN, pcValueTotal, INT64_LEN );
  memcpy( pcSendBuf + 1 + INT64_LEN + INT64_LEN + INT64_LEN, pcValueUsed, INT64_LEN );
  
  iSendLen = 1 + INT64_LEN + INT64_LEN + INT64_LEN + INT64_LEN;
  
  memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
  
  send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
  send( iaSocket, pcSendBuf, iSendLen, 0 );
  
  logFmtSyn( LVNOR, "send[%s]", pcSendBuf );
  
  return;
}

/**********************************************************************
函数名称: managerShowDiscsDeal
函数功能: 管理端处理 show discs 操作
参    数：
第    一：SOCKET               I
第    二：数据开始地址         I
第    三：数据结束地址         I
作    者: 李高文
建立时间: 20161022
**********************************************************************/
void managerShowDiscsDeal( int iaSocket )
{
/*
接收数据结构：长度+操作
返回数据结构：操作结果+KEY文件最大值+KEY文件当前值+KEY文件步增值+ KEY文件使用偏移量 + KEY文件待使用大小+VALUE文件最大值+VALUE文件当前值+VALUE文件步增值+ VALUE文件使用偏移量+VALUE文件待使用大小
*/
  char pcKeyFreeBlock[BLOCK_SIZE];        /* KEY 文件空闲记录块*/
  char pcValueFreeBlock[BLOCK_SIZE];      /* VALUE 文件空闲记录块*/
	char cResult;
	char pcSendBuf[1 + INT64_LEN * 10 + 1];
	char pcKeyMax[INT64_LEN + 1];
	char pcKeyTotal[INT64_LEN + 1];
	char pcKeyUpper[INT64_LEN + 1];
	char pcKeyUsed[INT64_LEN + 1];
	char pcKeyFree[INT64_LEN + 1];
	char pcValueMax[INT64_LEN + 1];
	char pcValueTotal[INT64_LEN + 1];
	char pcValueUpper[INT64_LEN + 1];
	char pcValueUsed[INT64_LEN + 1];
	char pcValueFree[INT64_LEN + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	int  iSendLen;
	int  iTmp;
	__int64 llKeyFree;
	__int64 llValueFree;
	__int64 llBlock;
	__int64 llKeyFileOffset;
	__int64 llValueFileOffset;
	__int64 llKeyFileSize;
	__int64 llValueFileSize;
	
	MANAGER_RDLOCK();
	
	/*取KEY文件最大值*/
  memset( pcKeyMax, 0x00, sizeof(pcKeyMax) );
  sprintf( pcKeyMax, "%0*lld", INT64_LEN, llgFileMaxSize );
  
  /*取KEY文件当前值*/
  llKeyFileSize = 0;
  for (iTmp = 0; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
  {
  	llKeyFileSize += llgKeyFileSize[iTmp];
  }
  memset( pcKeyTotal, 0x00, sizeof(pcKeyTotal) );
  sprintf( pcKeyTotal, "%0*lld", INT64_LEN, llKeyFileSize );
  
  /*取KEY文件步增值*/
  memset( pcKeyUpper, 0x00, sizeof(pcKeyUpper) );
  sprintf( pcKeyUpper, "%0*lld", INT64_LEN, llgFileUpperSize );
  
  /*取KEY文件使用偏移量*/
  llKeyFileOffset = 0;
  for (iTmp = 1; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
	{
		llKeyFileOffset += sgBase[iTmp].llKeyFileOffset;
	}
	llKeyFileOffset <<= BLOCK_LEFT_MOVE;
	
  memset( pcKeyUsed, 0x00, sizeof(pcKeyUsed) );
  sprintf( pcKeyUsed, "%0*lld", INT64_LEN, llKeyFileOffset );
	
	/*计算KEY文件待使用大小*/
	llKeyFree = 0;
	for (iTmp = 0; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
	{
		/*将全局变量copy到函数*/
    memset( pcKeyFreeBlock, 0x00, sizeof(pcKeyFreeBlock) );
    memcpy( pcKeyFreeBlock, pcgKeyFree[iTmp], BLOCK_SIZE );
    
    /*取得第一个空闲块的数量*/
    llKeyFree += (__int64)(sgBase[iTmp].llKeyFreeOffset/sizeof(__int64));
    
    /*指向下一个块*/
    llBlock = *((__int64*)(pcKeyFreeBlock + BLOCK_SIZE - sizeof(__int64)));
    
    /*循环搜索*/
    while (llBlock != 0)
    {
    	llKeyFree += (__int64)(BLOCK_SIZE/sizeof(__int64));
    	llBlock = *((__int64*)(pcKeyFreeBlock + BLOCK_SIZE - sizeof(__int64)));
    	memset( pcKeyFreeBlock, 0x00, sizeof(pcKeyFreeBlock) );
  	  getKeyBlockData( pcKeyFreeBlock, llBlock, iTmp );
    }
  }
  memset( pcKeyFree, 0x00, sizeof(pcKeyFree) );
  sprintf( pcKeyFree, "%0*lld", INT64_LEN, llKeyFree << BLOCK_LEFT_MOVE );
  
  
  /*取VALUE文件最大值*/
  memset( pcValueMax, 0x00, sizeof(pcValueMax) );
  sprintf( pcValueMax, "%0*lld", INT64_LEN, llgFileMaxSize );
  
  /*取VALUE文件当前值*/
  llValueFileSize = 0;
  for (iTmp = 0; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
	{
		llValueFileSize += llgValueFileSize[iTmp];
	}
  memset( pcValueTotal, 0x00, sizeof(pcValueTotal) );
  sprintf( pcValueTotal, "%0*lld", INT64_LEN, llValueFileSize );
  
  /*取VALUE文件步增值*/
  memset( pcValueUpper, 0x00, sizeof(pcValueUpper) );
  sprintf( pcValueUpper, "%0*lld", INT64_LEN, llgFileUpperSize );
  
  /*取VALUE文件使用偏移量*/
  llValueFileOffset = 0;
  for (iTmp = 0; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
	{
		llValueFileOffset += sgBase[iTmp].llValueFileOffset;
	}
  memset( pcValueUsed, 0x00, sizeof(pcValueUsed) );
  sprintf( pcValueUsed, "%0*lld", INT64_LEN, llValueFileOffset );
	
	/*计算VALUE文件待使用大小*/
	llValueFree = 0;
	for (iTmp = 0; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
	{
		/*将全局变量copy到函数*/
    memset( pcValueFreeBlock, 0x00, sizeof(pcValueFreeBlock) );
    memcpy( pcValueFreeBlock, pcgValueFree[iTmp], BLOCK_SIZE );
    
    /*取得第一个空闲块的数量*/
    llValueFree += (__int64)(sgBase[iTmp].llValueFreeOffset/sizeof(__int64));
    
    /*指向下一个块*/
    llBlock = *((__int64*)(pcValueFreeBlock + BLOCK_SIZE - sizeof(__int64)));
    
    /*循环搜索*/
    while (llBlock != 0)
    {
    	llValueFree += (__int64)(BLOCK_SIZE/sizeof(__int64));
    	llBlock = *((__int64*)(pcValueFreeBlock + BLOCK_SIZE - sizeof(__int64)));
    	memset( pcValueFreeBlock, 0x00, sizeof(pcValueFreeBlock) );
  	  getKeyBlockData( pcValueFreeBlock, llBlock, iTmp );
    }
  }
  memset( pcValueFree, 0x00, sizeof(pcValueFree) );
  sprintf( pcValueFree, "%0*lld", INT64_LEN, llValueFree << VALUE_LEFT_MOVE );
  
  MANAGER_RDUNLOCK();
  
  /*组发送数据*/
  memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
  
  sprintf( &cResult, "%d", SUCCESS );
  memcpy( pcSendBuf, &cResult, 1 );
  memcpy( pcSendBuf + 1, pcKeyMax, INT64_LEN );
  memcpy( pcSendBuf + 1 + INT64_LEN, pcKeyTotal, INT64_LEN );
  memcpy( pcSendBuf + 1 + INT64_LEN * 2, pcKeyUpper, INT64_LEN );
  memcpy( pcSendBuf + 1 + INT64_LEN * 3, pcKeyUsed, INT64_LEN );
  memcpy( pcSendBuf + 1 + INT64_LEN * 4, pcKeyFree, INT64_LEN );
  
  memcpy( pcSendBuf + 1 + INT64_LEN * 5, pcValueMax, INT64_LEN );
  memcpy( pcSendBuf + 1 + INT64_LEN * 6, pcValueTotal, INT64_LEN );
  memcpy( pcSendBuf + 1 + INT64_LEN * 7, pcValueUpper, INT64_LEN );
  memcpy( pcSendBuf + 1 + INT64_LEN * 8, pcValueUsed, INT64_LEN );
  memcpy( pcSendBuf + 1 + INT64_LEN * 9, pcValueFree, INT64_LEN );
  
  iSendLen = 1 + INT64_LEN * 10;
  
  memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
  
  send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
  send( iaSocket, pcSendBuf, iSendLen, 0 );
  
  return;
}

/**********************************************************************
函数名称: managerShowThreadsDeal
函数功能: 管理端处理 show threads 操作
参    数：
第    一：SOCKET               I
第    二：数据开始地址         I
第    三：数据结束地址         I
作    者: 李高文
建立时间: 20161022
**********************************************************************/
void managerShowThreadsDeal( int iaSocket )
{
/*
接收数据结构：长度+操作
发送数据结构：操作结果+短连接线程数量+长连接线程数量
*/
	char cResult;                                   /*执行结果*/
	char pcSendBuf[1 + INT64_LEN + INT64_LEN + 1];  /*返回缓冲区*/
	char pcShortThreads[INT64_LEN + 1];             /*返回短连接线程数量*/
	char pcLongThreads[INT64_LEN + 1];              /*返回长连接线程数量*/
	char pcSendLen[DATA_BUF_LEN + 1];
	int  iSendLen;                                  /*返回长度*/
	int  iTmp;
	
	//MANAGER_RDLOCK();

  /*取短连接线程数量*/
  memset( pcShortThreads, 0x00, sizeof(pcShortThreads) );
  sprintf( pcShortThreads, "%0*d", INT64_LEN, igShortThreads );
  
  /*取长连接线程数量*/
  memset( pcLongThreads, 0x00, sizeof(pcLongThreads) );
  sprintf( pcLongThreads, "%0*d", INT64_LEN, igLongThreads );
  
  //MANAGER_RDUNLOCK();
  
  /*组发送数据*/
  memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
  
  sprintf( &cResult, "%d", SUCCESS );
  memcpy( pcSendBuf, &cResult, 1 );
  memcpy( pcSendBuf + 1, pcShortThreads, INT64_LEN );
  memcpy( pcSendBuf + 1 + INT64_LEN, pcLongThreads, INT64_LEN );
  
  iSendLen = 1 + INT64_LEN + INT64_LEN;
  
  memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
  
  send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
  send( iaSocket, pcSendBuf, iSendLen, 0 );
  
  /*这里添加日志，查看锁的状态*/
  for (iTmp = 0; iTmp  < INDEPENDENT_SIZE; iTmp++)
  {
  	logFmtSyn( LVBUG, "main:id[%d]state[%d]", iTmp, igMainMutexState[iTmp] );
  }
  for (iTmp = 0; iTmp  < INDEPENDENT_FILE_SIZE; iTmp++)
  {
  	logFmtSyn( LVBUG, "     id[%d]state[%d]", iTmp, igMutexState[iTmp] );
  }
  return;
}

/**********************************************************************
函数名称: managerShowKeysDeal
函数功能: 管理端处理 show keys 操作
参    数：
第    一：SOCKET               I
第    二：数据开始地址         I
第    三：数据结束地址         I
作    者: 李高文
建立时间: 20161022
**********************************************************************/
void managerShowKeysDeal( int iaSocket )
{
/*
接收数据结构：长度+操作
返回数据结构：操作结果+KEY总数量
*/
	char cResult;                       /*执行结果*/
	char pcSendBuf[1 + INT64_LEN + 1];  /*返回缓冲区*/
	char pcTotalKeys[INT64_LEN + 1];    /*返回KEY总数量*/
	char pcSendLen[DATA_BUF_LEN + 1];
	int  iSendLen;                      /*返回长度*/
	int  iTmp;
	__int64 llSize;
  
  llSize = 0;
  
	MANAGER_RDLOCK();
	
  for (iTmp = 0; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
  {
  	llSize += sgBase[iTmp].llKeyValueSize;
  }
  for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
  {
  	llSize += sgGlobalVariableMain[iTmp].llgKeyValueSizeMain;
  }
  
  /*取KEY总数量*/
  memset( pcTotalKeys, 0x00, sizeof(pcTotalKeys) );
  sprintf( pcTotalKeys, "%0*lld", INT64_LEN, llSize );
  
  MANAGER_RDUNLOCK();
  
  /*组发送数据*/
  memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
  
  sprintf( &cResult, "%d", SUCCESS );
  memcpy( pcSendBuf, &cResult, 1 );
  memcpy( pcSendBuf + 1, pcTotalKeys, INT64_LEN );
  
  iSendLen = 1 + INT64_LEN;
  
  memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
  
  send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
  send( iaSocket, pcSendBuf, iSendLen, 0 );
  
  return;
}
