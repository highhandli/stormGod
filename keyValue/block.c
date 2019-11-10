#define __BLOCK_C

#ifndef __COMMON_HEAD_H
  #include "commonHead.h"
#endif

int bufToValue( __int64 llaValue, void *pcaBuf, int iaSize, int iaIndepentFileId );
int valueToBuf( void *pcaBuf, __int64 llaValue, int iaSize, int iaIndepentFileId );
int  scanfKeyPoint( char *pcaBlockDiscData, int  iaPointSize, char  *pcaKey, int iaKeyLen, int iaIndepentFileId );
int  scanfkeyValuePoint( char *pcaBlockDiscData, int  iaPointSize, char  *pcaKey, int iaKeyLen, int iaIndepentFileId );
__int64 mallocKeyBlock(int iaIndepentFileId);
int freeKeyBlock( __int64 laBlock, int iaIndepentFileId );
__int64 mallocValueBlock( __int64 llaSize, int iaIndepentFileId );
int freeValueBlock( __int64 laBlock, int iaIndepentFileId );


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
#define getKeyBlockData( pcaBlock, laBlock, independentFileId ) \
{\
  fseeko64( fpgKey[independentFileId], laBlock<<BLOCK_LEFT_MOVE, SEEK_SET );\
  fread( pcaBlock, 1, BLOCK_SIZE, fpgKey[independentFileId] );\
}

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
#define setKeyBlockData( pcaBlock, laBlock, independentFileId ) \
{\
  fseeko64( fpgKey[independentFileId], laBlock<< BLOCK_LEFT_MOVE, SEEK_SET );\
  fwrite( pcaBlock, 1, BLOCK_SIZE, fpgKey[independentFileId] );\
}



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
int bufToValue( __int64 llaValue, void *pcaBuf, int iaSize, int iaIndepentFileId )
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
	    SET_VALUE_DATA( llValue, 0, pcBuf, MIN_RAM_UNIT, iaIndepentFileId );
	    pcBuf += MIN_RAM_UNIT;
	    iLeftSize -= MIN_RAM_UNIT;
	    GET_VALUE_DATA( llValue, MIN_RAM_UNIT, &llValueTmp, ADDRESS_SIZE, iaIndepentFileId );
	    llValue = llValueTmp;
	  }
	  else
	  {
  		SET_VALUE_DATA( llValue, 0, pcBuf, iLeftSize, iaIndepentFileId );
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
int valueToBuf( void *pcaBuf, __int64 llaValue, int iaSize, int iaIndepentFileId )
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
	    GET_VALUE_DATA( llValue, 0, pcBuf, MIN_RAM_UNIT, iaIndepentFileId );
	    pcBuf += MIN_RAM_UNIT;
	    iLeftSize -= MIN_RAM_UNIT;
	    GET_VALUE_DATA( llValue, MIN_RAM_UNIT, &llValueTmp, ADDRESS_SIZE, iaIndepentFileId );
	    llValue = llValueTmp;
	  }
	  else
	  {
  		GET_VALUE_DATA( llValue, 0, pcBuf, iLeftSize, iaIndepentFileId );
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
int  scanfKeyPoint( char *pcaBlockDiscData, int  iaPointSize, char  *pcaKey, int iaKeyLen, int iaIndepentFileId )
{
  char pcKey[MAX_KEY_LEN];
  __int64  iLeft;                                            /*二分法中的左值位置*/
  __int64  iRight;                                           /*二分法中的右值位置*/
  __int64  iMiddle;                                          /*二分法中的中间位置*/
  __int64  iTmp;
  int iRet;
  char *pcBuf;
  KEY_POINT *skp;
  
  pcBuf = pcaBlockDiscData;
  iLeft = 0;
  iRight = iaPointSize;
  
  for (iTmp = iLeft; iTmp < iRight; iTmp++)
  {
    /*取得结点*/
    skp = (KEY_POINT*)(pcBuf + iTmp * sizeof(KEY_POINT));
    
    /*从 VALUE 文件中获取 KEY 的值*/
    valueToBuf( pcKey, skp->llKeyAddr, skp->llKeyLen, iaIndepentFileId );
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
    valueToBuf( pcKey, skp->llKeyAddr, skp->llKeyLen, iaIndepentFileId );
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
int  scanfkeyValuePoint( char *pcaBlockDiscData, int  iaPointSize, char  *pcaKey, int iaKeyLen, int iaIndepentFileId )
{
  char pcKey[MAX_KEY_LEN];
  __int64  iLeft;
  __int64  iRight;
  __int64  iMiddle;
  __int64  iTmp;
  int iRet;
  char *pcBuf;
  KEY_VALUE_POINT *skvp;
  
  pcBuf = pcaBlockDiscData;
  iLeft = 0;
  iRight = iaPointSize;
 
  for (iTmp = iLeft; iTmp < iRight; iTmp++)
  {
    /*取得结点*/
    skvp = (KEY_VALUE_POINT *)(pcBuf + iTmp * sizeof(KEY_VALUE_POINT) );
    
    /*从 VALUE 文件中获取 KEY 的值*/
    valueToBuf( pcKey, skvp->llKeyAddr, skvp->llKeyLen, iaIndepentFileId );
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
    valueToBuf( pcKey, skvp->llKeyAddr, skvp->llKeyLen, iaIndepentFileId );
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
__int64 mallocKeyBlock(int iaIndepentFileId)
{
	__int64 llBlock;
	__int64 llNextBlock;
	char c;
	
	/*从空闲区找*/
	while (1)
	{
		/*当前空闲区有*/
	  if (sgBase[iaIndepentFileId].llKeyFreeOffset > 0)
	  {
	  	llBlock = *((__int64*)(pcgKeyFree[iaIndepentFileId] + sgBase[iaIndepentFileId].llKeyFreeOffset - sizeof(__int64)));
	  	sgBase[iaIndepentFileId].llKeyFreeOffset -= sizeof(__int64);
	  	return llBlock;
	  }
	  else
	  {
	  	/*指向空闲块的最后位置*/
	  	llNextBlock = *((__int64*)(pcgKeyFree[iaIndepentFileId] + BLOCK_SIZE - sizeof(__int64)));
	  	
	  	/*如果没有*/
	  	if (llNextBlock == NO_BLOCK)
	  	{
	  		break;
	  	}
	  	else
	  	{
	  		getKeyBlockData( pcgKeyFree[iaIndepentFileId], llNextBlock, iaIndepentFileId );
	  		sgBase[iaIndepentFileId].llKeyFreeOffset = BLOCK_SIZE - sizeof(__int64);
	  		return llNextBlock;
	  	}
	  }
	}
		
	/*如果当前文件有空闲空间*/
	if (sgBase[iaIndepentFileId].llKeyFileOffset + 1 < llgKeyFileDisc[iaIndepentFileId])
	{
		return sgBase[iaIndepentFileId].llKeyFileOffset++;
	}
	/*当前文件没有空闲空间，先从空闲区找*/
	else
	{
		/*只有在空闲区执行到break才会到这里，表示空闲区没有*/
		/*检查是否有足够的文件扩展空间*/
		if (llgKeyFileSize[iaIndepentFileId] + (llgFileUpperSize>>INDEPENDENT_FILE_LEFT_MOVE) < (llgFileMaxSize>>INDEPENDENT_FILE_LEFT_MOVE))
		{
			/*将文件扩展*/
			llgKeyFileSize[iaIndepentFileId] += (llgFileUpperSize>>INDEPENDENT_FILE_LEFT_MOVE);
			
			/*设置 KEY 文件大小为步增值*/
			INCREASE_FILE( fpgKey[iaIndepentFileId], llgKeyFileSize[iaIndepentFileId], c );
      
      /*重新设置KEY文件的块数*/
      llgKeyFileDisc[iaIndepentFileId] = llgKeyFileSize[iaIndepentFileId] >> BLOCK_LEFT_MOVE;
      		
      return sgBase[iaIndepentFileId].llKeyFileOffset++;
      
		}
		else
		{
			return NO_BLOCK;
		}
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
int freeKeyBlock( __int64 laBlock, int iaIndepentFileId )
{
	char pcBlock[BLOCK_SIZE];
	
	memset( pcBlock, 0x00, BLOCK_SIZE );
	setKeyBlockData( pcBlock, laBlock, iaIndepentFileId );
	
	/*如果刚好是当前 KEY 文件使用偏移量*/
	if (laBlock == (sgBase[iaIndepentFileId].llKeyFileOffset) - 1)
	{
		sgBase[iaIndepentFileId].llKeyFileOffset--;
		return SUCCESS;
	}

  /*如果当前块的空间未满*/
  if (sgBase[iaIndepentFileId].llKeyFreeOffset < BLOCK_SIZE - sizeof(__int64))
  {
  	*((__int64 *)(pcgKeyFree[iaIndepentFileId] + sgBase[iaIndepentFileId].llKeyFreeOffset)) = laBlock;
  	sgBase[iaIndepentFileId].llKeyFreeOffset += sizeof(__int64);
  }
  /*如果已经满了*/
  else
  {
  	/*将新加入的空闲块数据设置为当前块数据*/
  	setKeyBlockData( pcgKeyFree[iaIndepentFileId], laBlock, iaIndepentFileId );
  	
  	/*将当前块最后8B数据设置为新加入的空闲块*/
  	*((__int64 *)(pcgKeyFree[iaIndepentFileId] + BLOCK_SIZE - sizeof(__int64))) = laBlock;
  	sgBase[iaIndepentFileId].llKeyFreeOffset = 0;
  }
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
__int64 mallocValueBlock( __int64 llaSize, int iaIndepentFileId )
{
	__int64 llBlock;
	__int64 llNextBlock;
	__int64 llNextBlockTmp;
	__int64 llBlockTmp;
	__int64 llTmp;
	__int64 llLeftSize;
	char c;
	int  iFd;
		
	llBlock = 0;
	llNextBlock = 0;
	llBlockTmp = 0;
	
	/*计算实际要分配的内存大小*/
	llLeftSize = ((llaSize%MIN_RAM_UNIT) == 0 ? (int)(llaSize/MIN_RAM_UNIT):(int)(llaSize/MIN_RAM_UNIT) + 1) * MIN_UNIT;
		
	
	/*先分配第一个块*/
	{
		/*当前空闲区有*/
		if (sgBase[iaIndepentFileId].llValueFreeOffset > 0)
		{
			llBlock = *((__int64*)(pcgValueFree[iaIndepentFileId] + sgBase[iaIndepentFileId].llValueFreeOffset - sizeof(__int64)));
			sgBase[iaIndepentFileId].llValueFreeOffset -= sizeof(__int64);
						
			if (sgBase[iaIndepentFileId].llValueFreeOffset == 0)
			{
				/*指向空闲块的最后位置*/
		  	llNextBlock = *((__int64*)(pcgValueFree[iaIndepentFileId] + BLOCK_SIZE - sizeof(__int64)));
		  	
		  	/*如果没有*/
		  	if (llNextBlock != NO_BLOCK)
			  {
				  getKeyBlockData( pcgValueFree[iaIndepentFileId], llNextBlock, iaIndepentFileId );
			  	freeKeyBlock(llNextBlock, iaIndepentFileId);
			  	sgBase[iaIndepentFileId].llValueFreeOffset = BLOCK_SIZE - sizeof(__int64);
			  }
			}
		}

		
		/*如果空闲区没有*/
		if (llBlock == 0)
		{
		  if (sgBase[iaIndepentFileId].llValueFileOffset + MIN_UNIT < llgValueFileSize[iaIndepentFileId])
		  {
		    llBlock = (sgBase[iaIndepentFileId].llValueFileOffset) >> VALUE_LEFT_MOVE;
		    sgBase[iaIndepentFileId].llValueFileOffset += MIN_UNIT;
		  }
		  else
		  {
		    /*检查是否有足够的文件扩展空间*/
		    if (llgValueFileSize[iaIndepentFileId] + (llgFileUpperSize>>INDEPENDENT_FILE_LEFT_MOVE) < (llgFileMaxSize>>INDEPENDENT_FILE_LEFT_MOVE))
		    {
		    	/*将文件扩展*/
		    	llgValueFileSize[iaIndepentFileId] += (llgFileUpperSize>>INDEPENDENT_FILE_LEFT_MOVE);
		    	
		    	/*设置 KEY 文件大小为步增值*/
          INCREASE_FILE( fpgValue[iaIndepentFileId], llgValueFileSize[iaIndepentFileId], c );
          
          llBlock = (sgBase[iaIndepentFileId].llValueFileOffset) >> VALUE_LEFT_MOVE;;
		      sgBase[iaIndepentFileId].llValueFileOffset += MIN_UNIT;
		      
		      /*数据同步*/
		      //__sync_synchronize();
		    }
		    else
		    {
		    	return NO_BLOCK;
		    }
		  }
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
		  llTmp = 0;

			SET_VALUE_DATA( llBlockTmp, MIN_RAM_UNIT, &llTmp, sizeof(__int64), iaIndepentFileId );
			
			return llBlock;
		}
		
		/*当前空闲区有*/
		if (sgBase[iaIndepentFileId].llValueFreeOffset > 0)
		{
		  llNextBlock = *((__int64*)(pcgValueFree[iaIndepentFileId] + sgBase[iaIndepentFileId].llValueFreeOffset - sizeof(__int64)));
		  sgBase[iaIndepentFileId].llValueFreeOffset -= sizeof(__int64);
		  		  
		  if (sgBase[iaIndepentFileId].llValueFreeOffset == 0)
		  {
		  	/*指向空闲块的最后位置*/
			  llNextBlockTmp = *((__int64*)(pcgValueFree[iaIndepentFileId] + BLOCK_SIZE - sizeof(__int64)));
			  
			  /*如果没有*/
			  if (llNextBlockTmp > 0)
			  {
			  	getKeyBlockData( pcgValueFree[iaIndepentFileId], llNextBlockTmp, iaIndepentFileId );
			  	freeKeyBlock(llNextBlockTmp, iaIndepentFileId);
			  	sgBase[iaIndepentFileId].llValueFreeOffset = BLOCK_SIZE - sizeof(__int64);
			  }
		  }
		}

		if (llNextBlock == 0)
		{
		  if (sgBase[iaIndepentFileId].llValueFileOffset < llgValueFileSize[iaIndepentFileId])
		  {
		    llNextBlock = (sgBase[iaIndepentFileId].llValueFileOffset) >> VALUE_LEFT_MOVE;
		    sgBase[iaIndepentFileId].llValueFileOffset += MIN_UNIT;
		  }
		  else
		  {
		    /*检查是否有足够的文件扩展空间*/
		    if (llgValueFileSize[iaIndepentFileId] + (llgFileUpperSize>>INDEPENDENT_FILE_LEFT_MOVE) < (llgFileMaxSize>>INDEPENDENT_FILE_LEFT_MOVE))
		    {
		    	/*将文件扩展*/
		    	llgValueFileSize[iaIndepentFileId] += (llgFileUpperSize>>INDEPENDENT_FILE_LEFT_MOVE);
		    	
		    	/*设置 KEY 文件大小为步增值*/
          INCREASE_FILE( fpgValue[iaIndepentFileId], llgValueFileSize[iaIndepentFileId], c );
          
          llNextBlock = (sgBase[iaIndepentFileId].llValueFileOffset) >> VALUE_LEFT_MOVE;
		      sgBase[iaIndepentFileId].llValueFileOffset += MIN_UNIT;
		    }
		    else
		    {
		      freeValueBlock( llBlock, iaIndepentFileId );
		    	return NO_BLOCK;
		    }
		  }
	  }

	  SET_VALUE_DATA( llBlockTmp, MIN_RAM_UNIT, &llNextBlock, sizeof(__int64), iaIndepentFileId );
		llBlockTmp = llNextBlock;
		llNextBlock = 0;
		
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
int freeValueBlock( __int64 laBlock, int iaIndepentFileId )
{
  __int64 llBlock;
  __int64 llBlockTmp;
  __int64 llNextBlock;
  char pcBuf[MIN_UNIT];
  
  memset( pcBuf, 0x00, MIN_UNIT );
  
  llBlock = laBlock;

  while (llBlock != 0)
  {
  	GET_VALUE_DATA( llBlock, MIN_RAM_UNIT, &llNextBlock, ADDRESS_SIZE, iaIndepentFileId );
  	
    /*如果当前块的空间未满*/
    if (sgBase[iaIndepentFileId].llValueFreeOffset < BLOCK_SIZE - sizeof(__int64))
    {
    	*((__int64 *)(pcgValueFree[iaIndepentFileId] + sgBase[iaIndepentFileId].llValueFreeOffset)) = llBlock;
    	sgBase[iaIndepentFileId].llValueFreeOffset += sizeof(__int64);
    }
    /*如果已经满了*/
    else
    {
      /*向 KEY 文件申请空间*/
      llBlockTmp = mallocKeyBlock(iaIndepentFileId);
      if (llBlockTmp == NO_BLOCK)
      {
        return ERROR;
      }
      
    	/*将刚申请的空闲块数据设置为当前块数据*/
    	setKeyBlockData( pcgValueFree[iaIndepentFileId], llBlockTmp, iaIndepentFileId );
    	
    	/*将当前块最后8B数据设置为刚申请的空闲块*/
    	*((__int64 *)(pcgValueFree[iaIndepentFileId] + BLOCK_SIZE - sizeof(__int64))) = llBlockTmp;
    	
    	*((__int64 *)(pcgValueFree[iaIndepentFileId])) = llBlock;
    	sgBase[iaIndepentFileId].llValueFreeOffset = sizeof(__int64);
    }
    
    SET_VALUE_DATA( llBlock, 0, pcBuf, MIN_UNIT, iaIndepentFileId );
    llBlock = llNextBlock;
  }

  return SUCCESS;
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
int calculateKey( char *pcaKey, int iaKeyLen )
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
	  	//memset( pcKey, 0x00, sizeof(pcKey) );
	  	memcpy( pcKey, pcaKey + iIndex, iaKeyLen - iIndex );
	  	llKey += *((__int64*)(pcKey));
	  	return (int)(llKey%INDEPENDENT_FILE_SIZE);
	  }
	  else
	  {
	  	llKey += *((__int64*)(pcaKey + iIndex));
	  	iIndex += sizeof(__int64);
	  }
  }
}
