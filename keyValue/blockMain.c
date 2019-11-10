#define __BLOCK_MAIN_C

#ifndef __COMMON_HEAD_H
  #include "commonHead.h"
#endif

int bufToValueMain( __int64 llaValue, void *pcaBuf, int iaSize, int iaIndepentId );
int valueToBufMain( void *pcaBuf, __int64 llaValue, int iaSize, int iaIndepentId );
int  scanfKeyPointMain( char *pcaBlockDiscData, int  iaPointSize, char  *pcaKey, int iaKeyLen, int iaIndepentId );
int  scanfkeyValuePointMain( char *pcaBlockDiscData, int  iaPointSize, char  *pcaKey, int iaKeyLen, int iaIndepentId );
__int64 mallocKeyBlockMain(int iaIndepentId);
int freeKeyBlockMain( __int64 laBlock, int iaIndepentId );
__int64 mallocValueBlockMain( __int64 llaSize, int iaIndepentId );
int freeValueBlockMain( __int64 laBlock, int iaIndepentId );

int  calculateKeyMain( char *pcaKey, int iaKeyLen );
__int64 calculateKeyMain2( char *pcaKey, __int64 iaKeyLen );

void releaseBlockMainAll(int iaIndepentId);
void releaseBlockMainAllData( __int64 iaAddr, __int64 iaSize, __int64 iaLoop, int iaTotalLoops, int iaIndepentId );
void releaseBlockMain(int iaIndepentId);
void releaseBlockMainData( __int64 iaAddr, __int64 iaSize, __int64 iaLoop, int iaTotalLoops, int iaIndepentId );


/********************************************************
函数名称：getKeyBlockData
函数功能：获取 KEY 文件的数据块数据 
入口参数：
第    一: 输出数据的内存地址                 O
第    二：数据块地址                         I
返    回：0
作    者：李高文 
创建时间：20160820
修改时间： 
*********************************************************/
#define getKeyBlockDataMain( pcaBlock, laBlock, indepentId ) memcpy( pcaBlock, sgGlobalVariableMain[indepentId].pcgKeyBufMain + (laBlock<<BLOCK_LEFT_MOVE), BLOCK_SIZE );

/********************************************************
函数名称：setKeyBlockData
函数功能：设置 KEY 文件的数据块数据 
入口参数：
第    一: 输入数据的内存地址                 I
第    二：数据块地址                         I
返    回：0
作    者：李高文 
创建时间：20160820
修改时间： 
*********************************************************/
#define setKeyBlockDataMain( pcaBlock, laBlock, indepentId ) memcpy( sgGlobalVariableMain[indepentId].pcgKeyBufMain + (laBlock<<BLOCK_LEFT_MOVE), pcaBlock, BLOCK_SIZE );

/**********************************************************************
函数名称: bufToValue
函数功能: 常规内存复制到 VALUE 文件 。注意，如果目标没有足够的空间，死机。
入口参数
第    一：目标内存
第    二：源内存
第    三：数量 
返    回：
作    者: 李高文
建立时间: 20160809
修改时间: 
**********************************************************************/
int bufToValueMain( __int64 llaValue, void *pcaBuf, int iaSize, int iaIndepentId )
{
	__int64 llValue;
	__int64 llValueTmp;
	void *pcBuf;
	int  iLeftSize;
		
	llValue = llaValue;
	pcBuf = pcaBuf;
	
	iLeftSize = iaSize;

	while (1)
	{
	  if (iLeftSize > MIN_RAM_UNIT)
	  {
	    SET_VALUE_DATA_MAIN( llValue, 0, pcBuf, MIN_RAM_UNIT, iaIndepentId );
	    pcBuf += MIN_RAM_UNIT;
	    iLeftSize -= MIN_RAM_UNIT;
	    GET_VALUE_DATA_MAIN( llValue, MIN_RAM_UNIT, &llValueTmp, ADDRESS_SIZE, iaIndepentId );
	    llValue = llValueTmp;
	  }
	  else
	  {
  		SET_VALUE_DATA_MAIN( llValue, 0, pcBuf, iLeftSize, iaIndepentId );
  		break;
  	}
	}

	return SUCCESS;
}

/**********************************************************************
函数名称: valueToBuf
函数功能: VALUE 文件数据复制到常规内存。注意，如果目标没有足够的空间，死机。
入口参数
第    一：目标内存
第    二：源内存
第    三：数量 
返    回：
作    者: 李高文
建立时间: 20160819
修改时间: 
**********************************************************************/
int valueToBufMain( void *pcaBuf, __int64 llaValue, int iaSize, int iaIndepentId )
{
	__int64 llValue;
	__int64 llValueTmp;
	void *pcBuf;
	int  iLeftSize;
	
	llValue = llaValue;
	pcBuf = pcaBuf;
	
	iLeftSize = iaSize;
	
	while (1)
	{
	  if (iLeftSize > MIN_RAM_UNIT)
	  {
	    GET_VALUE_DATA_MAIN( llValue, 0, pcBuf, MIN_RAM_UNIT, iaIndepentId );
	    pcBuf += MIN_RAM_UNIT;
	    iLeftSize -= MIN_RAM_UNIT;
	    GET_VALUE_DATA_MAIN( llValue, MIN_RAM_UNIT, &llValueTmp, ADDRESS_SIZE, iaIndepentId );
	    llValue = llValueTmp;
	  }
	  else
	  {
  		GET_VALUE_DATA_MAIN( llValue, 0, pcBuf, iLeftSize, iaIndepentId );
  		break;
  	}
	}
  
	return SUCCESS;
}

/********************************************************
函数名称：scanfKeyPoint
函数功能：搜索满足条件的结点位置 
入口参数：
第    一：输入数据                     I
第    二：区的结点的最大数量           I 
第    三：关键值                       I 
返    回：结点的位置，-1为结点不存在 
作    者：李高文 
创建时间：20160820
修改时间： 
*********************************************************/
int  scanfKeyPointMain( char *pcaBlockDiscData, int  iaPointSize, char  *pcaKey, int iaKeyLen, int iaIndepentId )
{
  char pcKey[MAX_KEY_LEN];
  __int64  iLeft;                                            /*二分法中的左值位置*/
  __int64  iRight;                                           /*二分法中的右值位置*/
  __int64  iMiddle;                                          /*二分法中的中间位置*/
  __int64  iTmp;
  char *pcBuf;
  KEY_POINT *skp;
  int iRet;
    
  pcBuf = pcaBlockDiscData;
  iLeft = 0;
  iRight = iaPointSize;
  
  for (iTmp = iLeft; iTmp < iRight; iTmp++)
  {
    /*取得结点*/
    skp = (KEY_POINT*)(pcBuf + iTmp * sizeof(KEY_POINT));
    
    /*从 VALUE 文件中获取 KEY 的值*/
    valueToBufMain( pcKey, skp->llKeyAddr, skp->llKeyLen, iaIndepentId );
    pcKey[(skp->llKeyLen)] = 0x00;
    
    if (iaKeyLen >= skp->llKeyLen)
    {
    	iRet = memcmp( pcaKey, pcKey, iaKeyLen );
    }
    else
    {
    	iRet = memcmp( pcaKey, pcKey, skp->llKeyLen );
    }
    if (iRet == 0)
    {
      return iTmp;
    }
      
    if (iRet < 0)
    {
    	if (iTmp == 0)
    	{
    		return 0;
    	}
      return iTmp - 1;
    }
        
    iMiddle = (iLeft + iRight)>>1;

    skp = (KEY_POINT*)(pcBuf + iMiddle * sizeof(KEY_POINT));
    
    /*从 VALUE 文件中获取 KEY 的值*/
    valueToBufMain( pcKey, skp->llKeyAddr, skp->llKeyLen, iaIndepentId );
    pcKey[(skp->llKeyLen)] = 0x00;
    
    if (iaKeyLen >= skp->llKeyLen)
    {
    	iRet = memcmp( pcaKey, pcKey, iaKeyLen );
    }
    else
    {
    	iRet = memcmp( pcaKey, pcKey, skp->llKeyLen );
    }
    if (iRet == 0)
    {
      return iMiddle;
    }

    if (iRet > 0)
    {
      iTmp = iMiddle;
      iLeft = iMiddle + 1;
    }
    else
    {
      iRight = iMiddle + 1;
    }   
  }

  return iTmp - 1;
}

/********************************************************
函数名称：scanfkeyValuePoint
函数功能：搜索满足条件的结点位置 
入口参数：
第    一：输入数据                     I
第    二：区的结点的最大数量           I 
第    三：关键值                       I 
返    回：结点的位置，-1为结点不存在 
作    者：李高文 
创建时间：20160820
修改时间： 
*********************************************************/
int  scanfkeyValuePointMain( char *pcaBlockDiscData, int  iaPointSize, char  *pcaKey, int iaKeyLen, int iaIndepentId )
{
  char pcKey[MAX_KEY_LEN];
  __int64  iLeft;
  __int64  iRight;
  __int64  iMiddle;
  __int64  iTmp;
  char *pcBuf;
  KEY_VALUE_POINT *skvp;
  int iRet;
  
  pcBuf = pcaBlockDiscData;
  iLeft = 0;
  iRight = iaPointSize;
   
  for (iTmp = iLeft; iTmp < iRight; iTmp++)
  {
    /*取得结点*/
    skvp = (KEY_VALUE_POINT *)(pcBuf + iTmp * sizeof(KEY_VALUE_POINT) );
    
    /*从 VALUE 文件中获取 KEY 的值*/
    valueToBufMain( pcKey, skvp->llKeyAddr, skvp->llKeyLen, iaIndepentId );
    pcKey[(skvp->llKeyLen)] = 0x00;
    
    if (iaKeyLen >= skvp->llKeyLen)
    {
      if (memcmp( pcaKey, pcKey, iaKeyLen ) <= 0)
      {
        return iTmp;
      }
    }
    else
    {
    	if (memcmp( pcaKey, pcKey, skvp->llKeyLen ) <= 0)
      {
        return iTmp;
      }
    }

    iMiddle = (iLeft + iRight)>>1;
    skvp = (KEY_VALUE_POINT *)(pcBuf + iMiddle * sizeof(KEY_VALUE_POINT) );
    
    /*从 VALUE 文件中获取 KEY 的值*/
    valueToBufMain( pcKey, skvp->llKeyAddr, skvp->llKeyLen, iaIndepentId );
    pcKey[(skvp->llKeyLen)] = 0x00;
    
    if (iaKeyLen >= skvp->llKeyLen)
    {
    	iRet = memcmp( pcaKey, pcKey, iaKeyLen );
    }
    else
    {
    	iRet = memcmp( pcaKey, pcKey, skvp->llKeyLen );
    }
    if (iRet == 0)
    {
      return iMiddle;
    }
    if (iRet > 0)
    {
      iTmp = iMiddle;
      iLeft = iMiddle + 1;
    }
    else
    {
      iRight = iMiddle + 1;
    }
  }
  return iTmp;
}

/********************************************************
函数名称：mallocKeyBlock
函数功能：申请 KEY 文件数据块地址 
入口参数：
返    回：数据块地址 
作    者：李高文 
创建时间：20160818
修改时间： 
*********************************************************/
__int64 mallocKeyBlockMain(int iaIndepentId)
{
	__int64 llBlock;
		
	#if 1
		if (sgGlobalVariableMain[iaIndepentId].llgKeyFreeOffsetMain == 0)
		{
			/*指向空闲块的最后位置*/
		  llBlock = *((__int64*)(sgGlobalVariableMain[iaIndepentId].pcgKeyFreeMain + BLOCK_SIZE - sizeof(__int64)));
		  //*((__int64*)(sgGlobalVariableMain[iaIndepentId].llgKeyFreeOffsetMain + BLOCK_SIZE - sizeof(__int64))) = 0;
		  
	  	/*如果有*/
	  	if (llBlock != NO_BLOCK)
	  	{
			  getKeyBlockDataMain( sgGlobalVariableMain[iaIndepentId].pcgKeyFreeMain, llBlock, iaIndepentId );
			  sgGlobalVariableMain[iaIndepentId].llgKeyFreeOffsetMain = (__int64)(BLOCK_SIZE - sizeof(__int64));
			  
			  sgGlobalVariableMain[iaIndepentId].llgKeyRamUsedMain += (__int64)BLOCK_SIZE;
			  //logFmtSyn( LVDET, "%lld", sgGlobalVariableMain[iaIndepentId].llgKeyFreeOffsetMain );
			  //logFmtSyn( LVNOR, "%lld", llBlock );
			  return llBlock;
		  }
		}
		else
	  if (sgGlobalVariableMain[iaIndepentId].llgKeyFreeOffsetMain > 0)
	  {
	  	sgGlobalVariableMain[iaIndepentId].llgKeyFreeOffsetMain -= (__int64)sizeof(__int64);
	  	llBlock = *((__int64*)(sgGlobalVariableMain[iaIndepentId].pcgKeyFreeMain + sgGlobalVariableMain[iaIndepentId].llgKeyFreeOffsetMain));
		  
		  sgGlobalVariableMain[iaIndepentId].llgKeyRamUsedMain += (__int64)BLOCK_SIZE;
		  //logFmtSyn( LVNOR, "%lld", sgGlobalVariableMain[iaIndepentId].llgKeyFreeOffsetMain );
		  //logFmtSyn( LVNOR, "%lld", llBlock );
		  return llBlock;
		}
	#endif	
	/*如果当前有空闲空间*/
	if (sgGlobalVariableMain[iaIndepentId].llgKeyRamOffsetMain + BLOCK_SIZE < sgGlobalVariableMain[iaIndepentId].llgKeyRamSizeMain)
	{
		llBlock = sgGlobalVariableMain[iaIndepentId].llgKeyRamOffsetMain>>BLOCK_LEFT_MOVE;
		sgGlobalVariableMain[iaIndepentId].llgKeyRamOffsetMain += (__int64)BLOCK_SIZE;
		sgGlobalVariableMain[iaIndepentId].llgKeyRamUsedMain += (__int64)BLOCK_SIZE;
		//logFmtSyn( LVNOR, "%lld", llBlock );
		return llBlock;
	}
	else
	{
		return NO_BLOCK;
	}
}


/********************************************************
函数名称：freeKeyBlock
函数功能：释放 KEY 文件的数据块
入口参数：
返    回：0 成功；其他失败
作    者：李高文 
创建时间：20160818
修改时间： 
*********************************************************/
int freeKeyBlockMain( __int64 laBlock, int iaIndepentId )
{	
	//return SUCCESS;
	char pcBlock[BLOCK_SIZE];
	
	//logFmtSyn( LVDET, "%lld", laBlock );
	
	memset( pcBlock, 0x00, BLOCK_SIZE );
	setKeyBlockDataMain( pcBlock, laBlock, iaIndepentId );
	
  /*如果当前块的空间未满*/
  if (sgGlobalVariableMain[iaIndepentId].llgKeyFreeOffsetMain < (__int64)(BLOCK_SIZE - sizeof(__int64)))
  {
  	*((__int64 *)(sgGlobalVariableMain[iaIndepentId].pcgKeyFreeMain + sgGlobalVariableMain[iaIndepentId].llgKeyFreeOffsetMain)) = laBlock;
  	sgGlobalVariableMain[iaIndepentId].llgKeyFreeOffsetMain += (__int64)sizeof(__int64);
  	//logFmtSyn( LVDET, "%lld", sgGlobalVariableMain[iaIndepentId].llgKeyFreeOffsetMain );
  }
  /*如果已经满了*/
  else
  {
  	/*将新加入的空闲块数据设置为当前块数据*/
  	setKeyBlockDataMain( sgGlobalVariableMain[iaIndepentId].pcgKeyFreeMain, laBlock, iaIndepentId );
  	
  	/*将当前块最后8B数据设置为新加入的空闲块*/
  	*((__int64 *)(sgGlobalVariableMain[iaIndepentId].pcgKeyFreeMain + BLOCK_SIZE - sizeof(__int64))) = laBlock;
  	sgGlobalVariableMain[iaIndepentId].llgKeyFreeOffsetMain = 0;
  	//logFmtSyn( LVDET, "%lld", sgGlobalVariableMain[iaIndepentId].llgKeyFreeOffsetMain );
  }
  sgGlobalVariableMain[iaIndepentId].llgKeyRamUsedMain -= (__int64)BLOCK_SIZE;

  return SUCCESS;
}

/********************************************************
函数名称：mallocValueBlock
函数功能：申请 VALUE 文件数据块地址 
入口参数：
返    回：数据块地址 
作    者：李高文 
创建时间：20160819
修改时间： 
*********************************************************/
__int64 mallocValueBlockMain( __int64 llaSize, int iaIndepentId )
{
	__int64 llBlock;
	__int64 llNextBlock;
	__int64 llBlockTmp;
	__int64 llBlockTmp2;
	__int64 llTmp;
	__int64 llLeftSize;
	//char pcBuf[MIN_UNIT];
		
	llBlock = NO_BLOCK;
	llNextBlock = 0;
	llBlockTmp = 0;
	//memset( pcBuf, 0x00, sizeof(pcBuf) );
	
	/*计算实际要分配的内存大小*/
	llLeftSize = ((llaSize%MIN_RAM_UNIT) == 0 ? (int)(llaSize/MIN_RAM_UNIT):(int)(llaSize/MIN_RAM_UNIT) + 1) * MIN_UNIT;
		
	
	#if 1
	/*先分配第一个块*/
	{
		/*当前空闲区有*/
		if (sgGlobalVariableMain[iaIndepentId].llgValueFreeOffsetMain > 0)
		{
			sgGlobalVariableMain[iaIndepentId].llgValueFreeOffsetMain -= sizeof(__int64);
			llBlock = *((__int64*)(sgGlobalVariableMain[iaIndepentId].pcgValueFreeMain + sgGlobalVariableMain[iaIndepentId].llgValueFreeOffsetMain));
			//SET_VALUE_DATA_MAIN( llBlock, 0, pcBuf, MIN_UNIT, iaIndepentId );
			sgGlobalVariableMain[iaIndepentId].llgValueRamUsedMain += MIN_UNIT;
			
			if (sgGlobalVariableMain[iaIndepentId].llgValueFreeOffsetMain == 0)
			{
				llBlockTmp2 = *((__int64*)(sgGlobalVariableMain[iaIndepentId].pcgValueFreeMain + BLOCK_SIZE - sizeof(__int64)));
				if (llBlockTmp2 != NO_BLOCK)
				{
					getKeyBlockDataMain( sgGlobalVariableMain[iaIndepentId].pcgValueFreeMain, llBlockTmp2, iaIndepentId );
			    freeKeyBlockMain(llBlockTmp2, iaIndepentId);
			  	sgGlobalVariableMain[iaIndepentId].llgValueFreeOffsetMain = BLOCK_SIZE - sizeof(__int64);
				}
			}
		}
		else
		{				
			llBlock = NO_BLOCK;
		}
  }
  #endif
  
	/*如果空闲区没有*/
	if (llBlock == NO_BLOCK)
	{
	  /*如果 VALUE 文件比缓冲区小，在缓冲区分配*/
	  if (sgGlobalVariableMain[iaIndepentId].llgValueRamOffsetMain + MIN_UNIT < sgGlobalVariableMain[iaIndepentId].llgValueRamSizeMain)
	  {
	    /*将当前位置作为申请块*/
	    llBlock = (sgGlobalVariableMain[iaIndepentId].llgValueRamOffsetMain) >> VALUE_LEFT_MOVE;
	    sgGlobalVariableMain[iaIndepentId].llgValueRamOffsetMain += MIN_UNIT;
	    sgGlobalVariableMain[iaIndepentId].llgValueRamUsedMain += MIN_UNIT;
	  }
	  else
	  {
	    return NO_BLOCK;
	  }
	}
	
	llLeftSize -= MIN_UNIT;
	
	llBlockTmp = llBlock;
	llNextBlock = 0;
	while (1)
	{
		/*如果分配完了*/
		if (llLeftSize == 0)
		{
		  llTmp = NO_BLOCK;

			SET_VALUE_DATA_MAIN( llBlockTmp, MIN_RAM_UNIT, &llTmp, sizeof(__int64), iaIndepentId );
			
			return llBlock;
		}
		
		#if 1
		/*当前空闲区有*/
		{
		  if (sgGlobalVariableMain[iaIndepentId].llgValueFreeOffsetMain > 0)
		  {
		    llNextBlock = *((__int64*)(sgGlobalVariableMain[iaIndepentId].pcgValueFreeMain + sgGlobalVariableMain[iaIndepentId].llgValueFreeOffsetMain - sizeof(__int64)));
		    //SET_VALUE_DATA_MAIN( llNextBlock, 0, pcBuf, MIN_UNIT, iaIndepentId );
		    sgGlobalVariableMain[iaIndepentId].llgValueFreeOffsetMain -= sizeof(__int64);
		    sgGlobalVariableMain[iaIndepentId].llgValueRamUsedMain += MIN_UNIT;
		    if (sgGlobalVariableMain[iaIndepentId].llgValueFreeOffsetMain == 0)
			  {
			  	llBlockTmp2 = *((__int64*)(sgGlobalVariableMain[iaIndepentId].pcgValueFreeMain + BLOCK_SIZE - sizeof(__int64)));
			  	if (llBlockTmp2 != NO_BLOCK)
			  	{
			  		getKeyBlockDataMain( sgGlobalVariableMain[iaIndepentId].pcgValueFreeMain, llBlockTmp2, iaIndepentId );
			      freeKeyBlockMain(llBlockTmp2, iaIndepentId);
			    	sgGlobalVariableMain[iaIndepentId].llgValueFreeOffsetMain = BLOCK_SIZE - sizeof(__int64);
			  	}
			  }
		  }
		  else
		  {	  
		    llNextBlock = NO_BLOCK;
		  }
    }
    #endif
		if (llNextBlock == NO_BLOCK)
		{
		  /*如果 VALUE 文件比缓冲区小，在缓冲区分配*/
	    if (sgGlobalVariableMain[iaIndepentId].llgValueRamOffsetMain + MIN_UNIT < sgGlobalVariableMain[iaIndepentId].llgValueRamSizeMain)
	    {
	      /*将当前位置作为申请块*/
	      llNextBlock = sgGlobalVariableMain[iaIndepentId].llgValueRamOffsetMain >> VALUE_LEFT_MOVE;
	      sgGlobalVariableMain[iaIndepentId].llgValueRamOffsetMain += MIN_UNIT;
	      sgGlobalVariableMain[iaIndepentId].llgValueRamUsedMain += MIN_UNIT;
	    }
	    else
	    {
		    freeValueBlockMain( llBlock, iaIndepentId );
		    return NO_BLOCK;
	    }
	  }

	  SET_VALUE_DATA_MAIN( llBlockTmp, MIN_RAM_UNIT, &llNextBlock, sizeof(__int64), iaIndepentId );
		llBlockTmp = llNextBlock;
		
		llLeftSize -= MIN_UNIT;
	}

	return llBlock;
}

/********************************************************
函数名称：freeValueBlock
函数功能：释放 VALUE 文件的数据块
入口参数：
返    回：0 成功；其他失败
作    者：李高文 
创建时间：20160819
修改时间： 
*********************************************************/
int freeValueBlockMain( __int64 laBlock, int iaIndepentId )
{
  __int64 llBlock;
  __int64 llBlockTmp;
  __int64 llNextBlock;
  char pcBuf[MIN_UNIT];
  
  //return SUCCESS;
  
  memset( pcBuf, 0x00, MIN_UNIT );
  
  llBlock = laBlock;
  
  while (llBlock != NO_BLOCK)
  {
  	GET_VALUE_DATA_MAIN( llBlock, MIN_RAM_UNIT, &llNextBlock, ADDRESS_SIZE, iaIndepentId );

    /*如果当前块的空间未满*/
    if (sgGlobalVariableMain[iaIndepentId].llgValueFreeOffsetMain < (__int64)(BLOCK_SIZE - sizeof(__int64)))
    {
    	*((__int64 *)(sgGlobalVariableMain[iaIndepentId].pcgValueFreeMain + sgGlobalVariableMain[iaIndepentId].llgValueFreeOffsetMain)) = llBlock;
    	sgGlobalVariableMain[iaIndepentId].llgValueFreeOffsetMain += (__int64)sizeof(__int64);
    }
    /*如果已经满了*/
    else
    {
      /*向 KEY 文件申请空间*/
      llBlockTmp = mallocKeyBlockMain(iaIndepentId);
      if (llBlockTmp == NO_BLOCK)
      {
        return ERROR;
      }
      
    	/*将刚申请的空闲块数据设置为当前块数据*/
    	setKeyBlockDataMain( sgGlobalVariableMain[iaIndepentId].pcgValueFreeMain, llBlockTmp, iaIndepentId );

    	/*将当前块最后8B数据设置为刚申请的空闲块*/
    	*((__int64 *)(sgGlobalVariableMain[iaIndepentId].pcgValueFreeMain + BLOCK_SIZE - sizeof(__int64))) = llBlockTmp;
    	
    	*((__int64 *)(sgGlobalVariableMain[iaIndepentId].pcgValueFreeMain)) = llBlock;
    	sgGlobalVariableMain[iaIndepentId].llgValueFreeOffsetMain = (__int64)sizeof(__int64);
    }
    sgGlobalVariableMain[iaIndepentId].llgValueRamUsedMain -= MIN_UNIT;
    SET_VALUE_DATA_MAIN( llBlock, 0, pcBuf, MIN_UNIT, iaIndepentId );
    llBlock = llNextBlock;
  }

  return SUCCESS;
}

/********************************************************
函数名称：checkBlock
函数功能：检查是否有足够的空间让插入动作能执行
KEY_BLOCK至少有 LOOPS 个，VALUE_BLOCK 至少有 MAX_KEY_LEN+MAX_VALUE_LEN
入口参数：
返    回：BOOL
作    者：李高文 
创建时间：20170120
修改时间：
*********************************************************/
int checkBlockMain(int iaIndepentId)
{
	/*检查是否有足够的 KEY_BLOCK*/
	if (sgGlobalVariableMain[iaIndepentId].llgKeyRamUsedMain + sgGlobalVariableMain[iaIndepentId].llgLoopsMain * BLOCK_SIZE > sgGlobalVariableMain[iaIndepentId].llgKeyRamSizeMain)
	{
		return 0;
	}
	
	/*检查是否有足够的 VALUE_BLOCK*//*通常实际需要的内存比 MAX_KEY_LEN + MAX_VALUE_LEN 要大一点点*/
	if (sgGlobalVariableMain[iaIndepentId].llgValueRamUsedMain + (MAX_KEY_LEN + MAX_VALUE_LEN + MAX_KEY_LEN) > sgGlobalVariableMain[iaIndepentId].llgValueRamSizeMain)
	{
		return 0;
	}
	
	return 1;
}

/********************************************************
函数名称：releaseBlockMainAllData
函数功能：释放内存中的所有数据到硬盘
入口参数：
作    者：李高文 
创建时间：20170120
修改时间：
*********************************************************/
void releaseBlockMainAll(int iaIndepentId)
{
	/*
	生成随机数，释放其中一个数量。
	*/
	char pcDataBuf[BLOCK_SIZE];     /*级区中的数据*/
	char pcDataBufTmp[BLOCK_SIZE];  /*级区中的数据*/
	KEY_POINT *key;                 /*其他级数据中的数据结构*/
  KEY_VALUE_POINT *keyData;       /*最后级数据中的数据结构*/
  char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	__int64 llTmp;
	int iIndepentFileId;
	int iRet;
	
	if (sgGlobalVariableMain[iaIndepentId].llgKeyValueSizeMain == 0)
	{
		return;
	}
	
	/*取得第一级数据*/
	memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
	getKeyBlockDataMain( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
	
	for (llTmp = 0; llTmp < sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain; llTmp++)
  {
  	//logFmtSyn( LVNOR, "%d:%d", llTmp, sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain );
  	if (sgGlobalVariableMain[iaIndepentId].llgLoopsMain == 1)
  	{
  		keyData = (KEY_VALUE_POINT*)(pcDataBuf + llTmp * sizeof(KEY_VALUE_POINT));
	  	
	  	/*从 VALUE 文件中获取 KEY 的值*/
      valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
      pcKey[keyData->llKeyLen] = 0x00;
      
      /*从 VALUE 文件中获取 VALUE 的值*/
      valueToBufMain( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentId );
      pcValue[keyData->llValueLen] = 0x00;
      
      /*将数据插入到硬盘中*/
      iIndepentFileId = calculateKey( pcKey, keyData->llKeyLen );
      iRet = updateNode( pcKey, keyData->llKeyLen, pcValue, keyData->llValueLen, iIndepentFileId );
  	}
  	else
  	{
	    key = (KEY_POINT*)(pcDataBuf + llTmp * sizeof(KEY_POINT));
	    releaseBlockMainAllData( key->llChildAddr, key->llChildSize, 2, sgGlobalVariableMain[iaIndepentId].llgLoopsMain, iaIndepentId );
	  }
  }
  //logFmtSyn( LVNOR, "%d", llTmp );
  memset( sgGlobalVariableMain[iaIndepentId].pcgKeyBufMain, 0x00, sgGlobalVariableMain[iaIndepentId].llgKeyRamOffsetMain );
  memset( sgGlobalVariableMain[iaIndepentId].pcgValueBufMain, 0x00, sgGlobalVariableMain[iaIndepentId].llgValueRamOffsetMain );
  //logFmtSyn( LVNOR, "%d", llTmp );
  sgGlobalVariableMain[iaIndepentId].llgKeyRamOffsetMain = 0;                           /* KEY 文件缓冲区的使用偏移量*/
  sgGlobalVariableMain[iaIndepentId].llgValueRamOffsetMain = 0;                         /* VALUE 文件缓冲区的使用偏移量*/
  sgGlobalVariableMain[iaIndepentId].llgKeyRamUsedMain = 0;                             /* KEY 文件缓冲区的使用数量，这里还包括释放过的*/
  sgGlobalVariableMain[iaIndepentId].llgValueRamUsedMain = 0;                           /* VALUE 文件缓冲区的使用数量，这里还包括释放过的*/
  sgGlobalVariableMain[iaIndepentId].llgKeyFreeOffsetMain = 0;                          /*当前文件中的 KEY 文件空闲区使用偏移量，单位：B*/
  sgGlobalVariableMain[iaIndepentId].llgValueFreeOffsetMain = 0;                        /*当前文件中的 VALUE 文件空闲区使用偏移量，单位：B*/
  sgGlobalVariableMain[iaIndepentId].llgKeyValueSizeMain = 0;                           /* KEY 的总数量*/
  sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain = 0;                              /* KEY 文件中的 KEY 的一级数据块地址，单位：BLOCK_SIZE*/
  sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain = 0;                              /* KEY 文件中的 KEY 的一级数据数量*/
  sgGlobalVariableMain[iaIndepentId].llgLoopsMain = 0;                                  /* KEY 文件中的 KEY 的当前级数*/
  memset( sgGlobalVariableMain[iaIndepentId].pcgKeyFreeMain, 0x00, BLOCK_SIZE );        /* KEY 文件空闲记录块*/
  memset( sgGlobalVariableMain[iaIndepentId].pcgValueFreeMain, 0x00, BLOCK_SIZE );      /* VALUE 文件空闲记录块*/
  //logFmtSyn( LVNOR, "%d", llTmp );
  return;
}

/**********************************************************************
函数名称: releaseBlockMainData
函数功能: 释放数据
参    数：
第    一：线程共享变量     I
第    二：数据地址         I
第    三：下一级数量       I
第    四：级数             I
第    五：总级数           I
返    回：
作    者: 李高文
建立时间: 20170120
**********************************************************************/
void releaseBlockMainAllData( __int64 iaAddr, __int64 iaSize, __int64 iaLoop, int iaTotalLoops, int iaIndepentId )
{
	char pcDataBuf[BLOCK_SIZE];    /*级区中的数据*/
	KEY_VALUE_POINT *keyData;      /*最后级数据中的数据结构*/
	KEY_POINT *key;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iTmp;
	int  iRet;
	int iIndepentFileId;
	
	memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
  getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
//logFmtSyn( LVNOR, "iaSize[%d]", iaSize );
	/*如果是最后一级*/
	if (iaLoop == iaTotalLoops)
	{
		for (iTmp = 0; iTmp < iaSize; iTmp++ )
	  {
	  	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iTmp * sizeof(KEY_VALUE_POINT));
	  	
	  	/*从 VALUE 文件中获取 KEY 的值*/
      valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
      pcKey[keyData->llKeyLen] = 0x00;
      
      /*从 VALUE 文件中获取 VALUE 的值*/
      valueToBufMain( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentId );
      pcValue[keyData->llValueLen] = 0x00;
      logFmtSyn( LVNOR, "pcKey[%s]pcValue[%s]", pcKey, pcValue );
      /*将数据插入到硬盘中*/
      iIndepentFileId = calculateKey( pcKey, keyData->llKeyLen );
      iRet = updateNode( pcKey, keyData->llKeyLen, pcValue, keyData->llValueLen, iIndepentFileId );
      //logFmtSyn( LVNOR, "iTmp[%d]", iTmp );
	  }
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    for (iTmp = 0; iTmp < iaSize; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      releaseBlockMainAllData( key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
    }
	}
	
	return;
}

/**********************************************************************
函数名称: calculateKeyMain
函数功能: 计算KEY的下标
参    数：
第    一：KEY             I
第    二：KEY长度         I
返    回：
作    者: 李高文
建立时间: 20170202
**********************************************************************/
int calculateKeyMain( char *pcaKey, int iaKeyLen )
{
	char pcKey[sizeof(__int64)];
	__int64 llKey;
	int  iIndex;
	
	iIndex = 0;
	llKey = 0;
	
	while (1)
	{
	  if (iIndex + sizeof(__int64) >= iaKeyLen)
	  {
	  	*((__int64*)pcKey) = 0x00;
	  	memcpy( pcKey, pcaKey + iIndex, iaKeyLen - iIndex );
	  	llKey += *((__int64*)(pcKey));
	  	return (int)(llKey%(__int64)INDEPENDENT_SIZE);
	  }
	  else
	  {
	  	llKey += *((__int64*)(pcaKey + iIndex));
	  	iIndex += sizeof(__int64);
	  }
  }
}

