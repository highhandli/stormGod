#define __KEY_VALUE_THREAD_MANAGER_EXIT_FLUSH_ALTER_C

/*
负责处理 MANAGER 中的 EXIT FLUSH ALTER 语句
*/

#ifndef __COMMON_HEAD_H
  #include "commonHead.h"
#endif

void managerExitDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd );
void managerFlushDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd );
void managerRamDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd );
void managerAlterThreadsDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd );
void managerAlterDiscsDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd );
void managerAlterUppersDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd );

/**********************************************************************
函数名称: managerExitDeal
函数功能: 管理端处理 exit 操作
参    数：
第    一：SOCKET               I
第    二：数据开始地址         I
第    三：数据结束地址         I
作    者: 李高文
建立时间: 20161022
**********************************************************************/
void managerExitDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd )
{
/*
接收数据结构：长度+操作+用户名+密码
返回数据结构：操作结果
*/
	#define MANAGER_EXIT_DEAL_SEND_RESULT( value ) \
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
	char pcUser[NAME_LEN + 1];
	char pcPass[NAME_LEN + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	int  iTmp;
	int  iRet;
	char *pcBufStart;
	char *pcBufEnd;
	
	logFmtSyn( LVNOR, "收到管理端的退出请求" );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
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
	
	/*检查用户名和密码是否匹配*/
	if (strcmp( pcgUser, pcUser ) || strcmp( pcgPass, pcPass ))
	{
		logFmtSyn( LVNOR, "用户名[%s]密码[%s]不匹配", pcUser, pcPass );
		MANAGER_EXIT_DEAL_SEND_RESULT( ERROR );
		MANAGER_WRUNLOCK();
		return;
	}
	
	/*先把结果返回去*/
	sprintf( &cResult, "%d", SUCCESS );
	
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, 1 );
  
  send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
  send( iaSocket, &cResult, 1, 0 );
	
	/*释放文件和内存资源*/
	//keyValueRelease( );
  igExitFlag = SERVER_EXIT;
	//exit(0);
  return;
}

/**********************************************************************
函数名称: managerFlushDeal
函数功能: 管理端处理 fflush 操作
参    数：
第    一：SOCKET               I
第    二：数据开始地址         I
第    三：数据结束地址         I
作    者: 李高文
建立时间: 20161022
**********************************************************************/
void managerFlushDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd )
{
/*
接收数据结构：长度+操作+用户名+密码
返回数据结构：操作结果
*/
	
	#define MANAGER_FFLUSH_DEAL_SEND_ERROR( value ) \
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
	char pcUser[NAME_LEN + 1];
	char pcPass[NAME_LEN + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	char *pcBufStart;
	char *pcBufEnd;
	char *pcTmp;
	int iTmp;
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
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
  
 
  
  /*将文件缓冲区的数据写入硬盘*/
  for (iTmp = 0; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
	{
    rewind( fpgKey[iTmp] );
	  fwrite( &(sgBase[iTmp]), 1, sizeof(KEY_VALUE_BASE_INFO), fpgKey[iTmp] );
	  
	  fflush( fpgKey[iTmp] );
	  fflush( fpgValue[iTmp] );
  }
  
  /*将内存中的数据写入到硬盘*/
  rewind( fpgMappingKey );
  rewind( fpgMappingValue );
	fwrite( &sgGlobalVariableMain, 1, sizeof(sgGlobalVariableMain), fpgMappingKey );

  /*写入文件数据*/
  for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
	{
		//logFmtSyn( LVNOR, "%d:%d", sgGlobalVariableMain[iTmp].llgKeyRamOffsetMain, sgGlobalVariableMain[iTmp].llgValueRamOffsetMain );
		fwrite( sgGlobalVariableMain[iTmp].pcgKeyBufMain, 1, sgGlobalVariableMain[iTmp].llgKeyRamOffsetMain, fpgMappingKey );
    fwrite( sgGlobalVariableMain[iTmp].pcgValueBufMain, 1, sgGlobalVariableMain[iTmp].llgValueRamOffsetMain, fpgMappingValue );
  }
  fflush( fpgMappingKey );
  fflush( fpgMappingValue );

	MANAGER_WRUNLOCK();
	
	sprintf( &cResult, "%d", SUCCESS );
	
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, 1 );
  
  send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
  send( iaSocket, &cResult, 1, 0 );
  return;
}

/**********************************************************************
函数名称: managerRamDeal
函数功能: 管理端处理 ram to disc 操作
参    数：
第    一：SOCKET               I
第    二：数据开始地址         I
第    三：数据结束地址         I
作    者: 李高文
建立时间: 20170217
**********************************************************************/
void managerRamDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd )
{
/*
接收数据结构：长度+操作+用户名+密码
返回数据结构：操作结果
*/
	
	#define MANAGER_RAM_DEAL_SEND_ERROR( value ) \
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
	char pcUser[NAME_LEN + 1];
	char pcPass[NAME_LEN + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	char *pcBufStart;
	char *pcBufEnd;
	char *pcTmp;
	int iTmp;
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
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

	/*将内存中的所有数据写入到硬盘中*/
	for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
	{
    releaseBlockMainAll(iTmp);
  }
  
  #if 1
  /*将内存中的数据写入到硬盘*/
  rewind( fpgMappingKey );
  rewind( fpgMappingValue );
	fwrite( &sgGlobalVariableMain, 1, sizeof(sgGlobalVariableMain), fpgMappingKey );
  for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
	{
		//logFmtSyn( LVNOR, "%d", iTmp );
    fwrite( sgGlobalVariableMain[iTmp].pcgKeyBufMain, 1, sgGlobalVariableMain[iTmp].llgKeyRamOffsetMain, fpgMappingKey );
    fwrite( sgGlobalVariableMain[iTmp].pcgValueBufMain, 1, sgGlobalVariableMain[iTmp].llgValueRamOffsetMain, fpgMappingValue );
  }
  fflush( fpgMappingKey );
  fflush( fpgMappingValue );
  #endif
  
  #if 1
  for (iTmp = 0; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
	{
    rewind( fpgKey[iTmp] );
	  fwrite( &(sgBase[iTmp]), 1, sizeof(KEY_VALUE_BASE_INFO), fpgKey[iTmp] );
	  
	  fflush( fpgKey[iTmp] );
	  fflush( fpgValue[iTmp] );
  }
	#endif
	
	MANAGER_WRUNLOCK();
	
	sprintf( &cResult, "%d", SUCCESS );
	
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, 1 );
  
  send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
  send( iaSocket, &cResult, 1, 0 );
  return;
}

/**********************************************************************
函数名称: managerAlterDiscsDeal
函数功能: 管理端处理 alter discs 操作
参    数：
第    一：SOCKET               I
第    二：数据开始地址         I
第    三：数据结束地址         I
作    者: 李高文
建立时间: 20161022
**********************************************************************/
void managerAlterDiscsDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd )
{
/*
发送数据结构：长度+操作+文件最大值+用户名+密码
接收数据结构：操作结果
*/
	#define MANAGER_ALTER_DISCS_DEAL_SEND_ERROR( value ) \
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
	char pcMax[INT64_LEN + 1];
	char pcUser[NAME_LEN + 1];
	char pcPass[NAME_LEN + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	__int64 llFileMaxSize;
	char *pcBufStart;
	char *pcBufEnd;
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
	/*取文件最大值*/
	memset( pcMax, 0x00, sizeof(pcMax) );
	memcpy( pcMax, pcBufStart, INT64_LEN );
	pcBufStart += INT64_LEN;
	llFileMaxSize = (atoll( pcMax )) << 20;
	
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
	
	/*检查用户名和密码是否匹配*/
	if (strcmp( pcgUser, pcUser ) || strcmp( pcgPass, pcPass ))
	{
		logFmtSyn( LVNOR, "用户名[%s]密码[%s]不匹配", pcUser, pcPass );
		MANAGER_ALTER_DISCS_DEAL_SEND_ERROR( ERROR );
		MANAGER_WRUNLOCK();
		return;
	}
	
	#if 0
	if (llFileMaxSize > llgFileMaxSize)
  {
  	logFmtSyn( LVNOR, "文件大小[%lld]不能超出最大值[%lld]", llFileMaxSize, llgFileMaxSize );
  	MANAGER_ALTER_DISCS_DEAL_SEND_ERROR( ERROR );
		MANAGER_WRUNLOCK();
		return;
  }
  #endif
	
	llgFileMaxSize = llFileMaxSize;
	
	MANAGER_WRUNLOCK();
	
	sprintf( &cResult, "%d", SUCCESS );
	
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, 1 );
  
  send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
  send( iaSocket, &cResult, 1, 0 );
  return;
}

/**********************************************************************
函数名称: managerAlterUppersDeal
函数功能: 管理端处理 alter uppers 操作
参    数：
第    一：SOCKET               I
第    二：数据开始地址         I
第    三：数据结束地址         I
作    者: 李高文
建立时间: 20161022
**********************************************************************/
void managerAlterUppersDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd )
{
/*
接收数据结构：长度+操作+文件步增值+用户名+密码
返回数据结构：操作结果
*/
	#define MANAGER_ALTER_UPPERS_DEAL_SEND_ERROR( value ) \
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
	char pcUppers[INT64_LEN + 1];
	char pcUser[NAME_LEN + 1];
	char pcPass[NAME_LEN + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	__int64 llFileUpperSize;
	char *pcBufStart;
	char *pcBufEnd;
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
	/*取步增值*/
	memset( pcUppers, 0x00, sizeof(pcUppers) );
	memcpy( pcUppers, pcBufStart, INT64_LEN );
	pcBufStart += INT64_LEN;
	
	llFileUpperSize = (atoll( pcUppers )) << 20;
	
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
	
	/*检查用户名和密码是否匹配*/
	if (strcmp( pcgUser, pcUser ) || strcmp( pcgPass, pcPass ))
	{
		logFmtSyn( LVNOR, "用户名[%s]密码[%s]不匹配", pcUser, pcPass );
		MANAGER_ALTER_UPPERS_DEAL_SEND_ERROR( ERROR );
		MANAGER_WRUNLOCK();
		return;
	}
		
	if (llFileUpperSize > llgFileMaxSize)
  {
  	logFmtSyn( LVNOR, "文件步增大小[%lld]不能超出最大值[%lld]", llFileUpperSize, llgFileMaxSize );
  	MANAGER_ALTER_UPPERS_DEAL_SEND_ERROR( ERROR );
		MANAGER_WRUNLOCK();
		return;
  }
  
  llgFileUpperSize = llFileUpperSize;
	
	MANAGER_WRUNLOCK();
	
	sprintf( &cResult, "%d", SUCCESS );
	
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, 1 );
  
  send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
  send( iaSocket, &cResult, 1, 0 );
  return;
}

/**********************************************************************
函数名称: managerAlterDiscsDeal
函数功能: 管理端处理 alter discs 操作
参    数：
第    一：SOCKET               I
第    二：数据开始地址         I
第    三：数据结束地址         I
作    者: 李高文
建立时间: 20161022
**********************************************************************/
void managerCheckDeal( int iaSocket )
{
/*
发送数据结构：
接收数据结构：操作结果
*/
	#define MANAGER_ALTER_DISCS_DEAL_SEND_ERROR( value ) \
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
	char pcSendLen[DATA_BUF_LEN + 1];
	int  iRet;
	int  iTmp;
	int  iTmp2;
	char pcBlock[BLOCK_SIZE];
	__int64 llBlock;
	__int64 llKeyFreeOffsetMain;
	__int64 llValueFreeOffsetMain;
	
	
	MANAGER_WRLOCK();
	
	#if 0
	/*检查变量 sgGlobalVariableMain */
	{
		/*打印 llgKeyRamOffsetMain 和 pcgKeyFreeMain*/
		for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
		{
			memcpy( pcBlock, sgGlobalVariableMain[iTmp].pcgKeyFreeMain, BLOCK_SIZE );
			llKeyFreeOffsetMain = sgGlobalVariableMain[iTmp].llgKeyFreeOffsetMain;
			while (1)
			{
			  if (llKeyFreeOffsetMain == 0)
		    {
		    	/*指向空闲块的最后位置*/
		      llBlock = *((__int64*)( pcBlock + BLOCK_SIZE - sizeof(__int64)));
		    
	      	/*如果有*/
	      	if (llBlock > 0)
	      	{
		    	  getKeyBlockDataMain( pcBlock, llBlock, iTmp );
		    	  llKeyFreeOffsetMain = BLOCK_SIZE - sizeof(__int64);
		    	  logFmtSyn( LVSYS, "%lld", llBlock );
		      }
		      else
		      {
		      	break;
		      }
		    }
		    else
	      if (llKeyFreeOffsetMain > 0)
	      {
	      	llKeyFreeOffsetMain -= sizeof(__int64);
	      	llBlock = *((__int64*)(pcBlock + llKeyFreeOffsetMain));

		      logFmtSyn( LVSYS, "%lld", llBlock );
		    }
	    }
	    
	    logFmtSyn( LVSYS, "%lld:%lld:%lld:%lld", sgGlobalVariableMain[iTmp].llgKeyValueSizeMain, sgGlobalVariableMain[iTmp].llgFirstDiscMain, sgGlobalVariableMain[iTmp].llgFirstSizeMain, sgGlobalVariableMain[iTmp].llgLoopsMain );
		}
		
		/*打印 llgValueRamOffsetMain 和 pcgKeyFreeMain*/
		for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
		{
			memcpy( pcBlock, sgGlobalVariableMain[iTmp].pcgValueFreeMain, BLOCK_SIZE );
			llValueFreeOffsetMain = sgGlobalVariableMain[iTmp].llgValueFreeOffsetMain;
			while (1)
			{
			  if (llValueFreeOffsetMain > 0)
		    {
		    	llBlock = *((__int64*)(pcBlock+ llValueFreeOffsetMain - sizeof(__int64)));
		    	llValueFreeOffsetMain -= sizeof(__int64);
		    	
		    	logFmtSyn( LVERR, "%lld", llBlock );
		    	
		    	if (llValueFreeOffsetMain == 0)
		    	{
		    		llBlock = *((__int64*)(pcBlock + BLOCK_SIZE - sizeof(__int64)));
		    		if (llBlock > 0)
		    		{
		    			getKeyBlockDataMain( pcBlock, llBlock, iTmp );
		    	  	llValueFreeOffsetMain = BLOCK_SIZE - sizeof(__int64);
		    		}
		    		else
		    		{
		    			break;
		    		}
		    	}
		    }
		    else
		    {
		    	break;
		    }
	    }
		}
	}
	#else
	for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
  {
  	logFmtSyn( LVWAR, "[%lld][%lld][%lld][%lld][%lld][%lld][%lld][%lld][%lld][%lld][%lld]", 
  	  iTmp,                                              /*1*/
  	  sgGlobalVariableMain[iTmp].llgKeyRamOffsetMain,    /*2*/
  	  sgGlobalVariableMain[iTmp].llgValueRamOffsetMain,  /*3*/
  	  sgGlobalVariableMain[iTmp].llgKeyRamUsedMain,      /*4*/
  	  sgGlobalVariableMain[iTmp].llgValueRamUsedMain,    /*5*/
  	  sgGlobalVariableMain[iTmp].llgKeyFreeOffsetMain,   /*6*/
  	  sgGlobalVariableMain[iTmp].llgValueFreeOffsetMain, /*7*/
  	  sgGlobalVariableMain[iTmp].llgKeyValueSizeMain,    /*8*/
  	  sgGlobalVariableMain[iTmp].llgFirstDiscMain,       /*9*/
  	  sgGlobalVariableMain[iTmp].llgFirstSizeMain,       /*10*/
  	  sgGlobalVariableMain[iTmp].llgLoopsMain );         /*11*/
	  //memset( sgGlobalVariableMain[iTmp].pcgKeyBufMain, 0x00, sgGlobalVariableMain[iTmp].llgKeyRamOffsetMain );
    //memset( sgGlobalVariableMain[iTmp].pcgValueBufMain, 0x00, sgGlobalVariableMain[iTmp].llgValueRamOffsetMain );
    //logFmtSyn( LVNOR, "%d", llTmp );
    //sgGlobalVariableMain[iTmp].llgKeyRamOffsetMain = 0;                           /* KEY 文件缓冲区的使用偏移量*/
    //sgGlobalVariableMain[iTmp].llgValueRamOffsetMain = 0;                         /* VALUE 文件缓冲区的使用偏移量*/
    //sgGlobalVariableMain[iTmp].llgKeyRamUsedMain = 0;                             /* KEY 文件缓冲区的使用数量，这里还包括释放过的*/
    //sgGlobalVariableMain[iTmp].llgValueRamUsedMain = 0;                           /* VALUE 文件缓冲区的使用数量，这里还包括释放过的*/
    //sgGlobalVariableMain[iTmp].llgKeyFreeOffsetMain = 0;                          /*当前文件中的 KEY 文件空闲区使用偏移量，单位：B*/
    //sgGlobalVariableMain[iTmp].llgValueFreeOffsetMain = 0;                        /*当前文件中的 VALUE 文件空闲区使用偏移量，单位：B*/
    //sgGlobalVariableMain[iTmp].llgKeyValueSizeMain = 0;                           /* KEY 的总数量*/
    //sgGlobalVariableMain[iTmp].llgFirstDiscMain = 0;                              /* KEY 文件中的 KEY 的一级数据块地址，单位：BLOCK_SIZE*/
    //sgGlobalVariableMain[iTmp].llgFirstSizeMain = 0;                              /* KEY 文件中的 KEY 的一级数据数量*/
    //sgGlobalVariableMain[iTmp].llgLoopsMain = 0;                                  /* KEY 文件中的 KEY 的当前级数*/
    //memset( sgGlobalVariableMain[iTmp].pcgKeyFreeMain, 0x00, BLOCK_SIZE );        /* KEY 文件空闲记录块*/
    //memset( sgGlobalVariableMain[iTmp].pcgValueFreeMain, 0x00, BLOCK_SIZE );      /* VALUE 文件空闲记录块*/
  }
	#endif
	MANAGER_WRUNLOCK();
	
	sprintf( &cResult, "%d", SUCCESS );
	
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, 1 );
  
  send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
  send( iaSocket, &cResult, 1, 0 );
  return;
}
