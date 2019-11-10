#define __KEY_VALUE_DELETE_MAIN_C

#ifndef __COMMON_HEAD_H
  #include "commonHead.h"
#endif

int deleteNodeMain( char *pcaKey, __int64 llaKeyLen, int iaIndepentId );
int deleteIndexNodeOneLoopsMain( char *pcaKey, __int64 llaKeyLen, int iaIndepentId );
int deleteIndexNodeLastLoopMain( int *iaReplaceFlag,  KEY_POINT *saIndexPoint, char *pcaKey, __int64 llaKeyLen, int iaIndepentId );
int deleteIndexNodeTwoLoopsMain( char *pcaKey, __int64 llaKeyLen, int iaIndepentId );
int deleteIndexNodeMain( int *iaReplaceFlag, KEY_POINT *saIndexPoint, int iaLoops,  char *pcaKey, __int64 llaKeyLen, int iaIndepentId );

//#define freeValueBlockMain( a,b ) freeValueBlockMain( a,b );/*logFmtSyn(LVDET,"%lld",a);*/

/********************************************************
函数名称：deleteNodeMain
函数功能：插入索引数据 
入口参数：
第    一: 索引值                 I
返    回：0表示成功，其他表示失败 
作    者：李高文 
创建时间：20160910
修改时间： 
*********************************************************/
int deleteNodeMain( char *pcaKey, __int64 llaKeyLen, int iaIndepentId )
{
	int iRet;
	
	/*获取锁*/
  UPDATE_DELETE_MAIN_LOCK(iaIndepentId);
  //logFmtSyn( LVDET, "%lld", sgGlobalVariableMain[iaIndepentId].llgLoopsMain );
	if (sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain == 0)
  {
  	/*解锁*/
    UPDATE_DELETE_MAIN_UNLOCK(iaIndepentId);
  	return NOT_FOUND;
  }
  
  /*如果三级以上*/
  if (sgGlobalVariableMain[iaIndepentId].llgLoopsMain >= 3)
  {
  	iRet = deleteIndexNodeMain( NULL, NULL, 1, pcaKey, llaKeyLen, iaIndepentId );
  }
  else
  /*如果只有两级*/
  if (sgGlobalVariableMain[iaIndepentId].llgLoopsMain == 2)
  {
  	iRet = deleteIndexNodeTwoLoopsMain( pcaKey, llaKeyLen, iaIndepentId );
  }
  else
  /*如果只有一级*/
  if (sgGlobalVariableMain[iaIndepentId].llgLoopsMain <= 1)
  {
  	iRet = deleteIndexNodeOneLoopsMain( pcaKey, llaKeyLen, iaIndepentId );
  }
  else
  {  
    iRet = ERROR;
  }
  
  if (iRet == SUCCESS)
  {
  	(sgGlobalVariableMain[iaIndepentId].llgKeyValueSizeMain)--;
  }
  //logFmtSyn( LVDET, "%lld", sgGlobalVariableMain[iaIndepentId].llgValueFreeOffsetMain );
  /*解锁*/
  UPDATE_DELETE_MAIN_UNLOCK(iaIndepentId);
  return iRet;
}

int deleteIndexNodeOneLoopsMain( char *pcaKey, __int64 llaKeyLen, int iaIndepentId )
{
  __int64  iLeft;
  __int64  iRight;
  __int64  iMiddle;
  __int64  iDeleteSite;
  int iRet;
  KEY_VALUE_POINT keyData;                              /*其他级数据中的数据结构*/  
  char  pcValue[MAX_KEY_LEN];

  char pcDataBuf[BLOCK_SIZE];                           /*一个块的数据*/
  char pcDataBufTmp[BLOCK_SIZE];                        /*一个块的数据临时变量*/

  /*取得最后一级所有数据*/
  memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
  getKeyBlockDataMain( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
//logSyn( LVSYS, pcDataBuf, BLOCK_SIZE );
  iLeft = 0;
  iRight = sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain - 1;
  iMiddle = 0;

  while (1)
  {
    iMiddle = (iLeft + iRight)>>1;
    
    memset( &keyData, 0x00, sizeof(keyData) );        
    memcpy( &keyData, pcDataBuf + iMiddle * sizeof(KEY_VALUE_POINT), sizeof(KEY_VALUE_POINT) );
    
    /*从 VALUE 文件中获取 KEY 的值*/
    valueToBufMain( pcValue, keyData.llKeyAddr, keyData.llKeyLen, iaIndepentId );
    pcValue[keyData.llKeyLen] = 0x00; 

    if (llaKeyLen >= keyData.llKeyLen)
    {
    	iRet = memcmp( pcaKey, pcValue, llaKeyLen );
    }
    else
    {
    	iRet = memcmp( pcaKey, pcValue, keyData.llKeyLen );
    }
    
    if (iRet == 0)
    {
    	iDeleteSite = iMiddle;
      break;
    }
    
    /*目标值在中值右边*/
    if (iRet > 0)
    {
    	if (iMiddle == iRight)
    	{
    		return NOT_FOUND;
    	}
    	if (iMiddle == iRight - 1)
    	{
    		iLeft = iRight;
    	}
    	else
    	{
        iLeft = iMiddle + 1;
      }
    }
    /*目标值在中值左边*/
    else
    {
    	if (iMiddle == iLeft)
    	{
    		return NOT_FOUND;
    	}
    	if (iMiddle == iLeft + 1)
    	{
    		iRight = iLeft;
    	}
    	else
    	{
        iRight = iMiddle - 1;
      }
    }
  }
  
  /*释放结点在 VALUE 文件中的空间*/
	freeValueBlockMain( keyData.llKeyAddr, iaIndepentId );
	freeValueBlockMain( keyData.llValueAddr, iaIndepentId );

  /*如果只有一个数据*/
  if (sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain == 1)
  {
		(sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain)--;
		
		/*释放空间*/
		freeKeyBlockMain( sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
		sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain = 0;
    sgGlobalVariableMain[iaIndepentId].llgLoopsMain = 0;

		return SUCCESS;
	}
  /*如果是所在的最后数据：直接删除该数据，全局变量减1，返回*/
  else
  if (iDeleteSite == sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain - 1)
  {
  	(sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain)--;
		memset( pcDataBuf + iDeleteSite * sizeof(KEY_VALUE_POINT), 0x00, sizeof(KEY_VALUE_POINT) );
		setKeyBlockDataMain( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
		
		return SUCCESS;
	}
	/*如果不是最后的数据，是第一个数据：删除该数据，并将后面的数据上移，全局变量减1*/
	else
	if (iDeleteSite == 0)
	{
		memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
		memcpy( pcDataBufTmp, pcDataBuf + sizeof(KEY_VALUE_POINT), (sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain - 1) * sizeof(KEY_VALUE_POINT) );
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
		memcpy( pcDataBuf, pcDataBufTmp, (sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain - 1) * sizeof(KEY_VALUE_POINT) );
		
		(sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain)--;
		setKeyBlockDataMain( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
		
		return SUCCESS;
  }
  /*如果不是最后的数据，也不是第一个数据：删除该数据，并将后面的数据上移，全局变量减1*/
  else
  {
		memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
		memcpy( pcDataBufTmp, pcDataBuf, iDeleteSite * sizeof(KEY_VALUE_POINT) );
		memcpy( pcDataBufTmp + iDeleteSite * sizeof(KEY_VALUE_POINT), pcDataBuf + (iDeleteSite + 1)* sizeof(KEY_VALUE_POINT), (sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain - iDeleteSite - 1) * sizeof(KEY_VALUE_POINT));
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
		memcpy( pcDataBuf, pcDataBufTmp, (sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain - 1) * sizeof(KEY_VALUE_POINT) );
		
		(sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain)--;
		setKeyBlockDataMain( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
		
		return SUCCESS;
	}
	
  return ERROR;
}

int deleteIndexNodeLastLoopMain( int *iaReplaceFlag,  KEY_POINT *saIndexPoint, char *pcaKey, __int64 llaKeyLen, int iaIndepentId )
{
  __int64  iLeft;
  __int64  iRight;
  __int64  iMiddle;
  __int64  iDeleteSite;
  int iRet;
  char  pcValue[MAX_KEY_LEN];
  KEY_VALUE_POINT keyData;                                 /*最后级数据中的数据结构*/
  char pcDataBuf[BLOCK_SIZE];                              /*一个块的数据*/
  char pcDataBufTmp[BLOCK_SIZE];                           /*级区中的数据临时变量*/
  
  /*取得最后一级所有数据*/
  memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
  getKeyBlockDataMain( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentId );

  iLeft = 0;
  iRight = (saIndexPoint->llChildSize) - 1;
  iMiddle = 0;

  while (1)
  {
    iMiddle = (iLeft + iRight)>>1;
    
    memset( &keyData, 0x00, sizeof(keyData) );        
    memcpy( &keyData, pcDataBuf + iMiddle * sizeof(KEY_VALUE_POINT), sizeof(KEY_VALUE_POINT) );
    
    /*从 VALUE 文件中获取 KEY 的值*/
    valueToBufMain( pcValue, keyData.llKeyAddr, keyData.llKeyLen, iaIndepentId );
    pcValue[keyData.llKeyLen] = 0x00; 

    if (llaKeyLen >= keyData.llKeyLen)
    {
    	iRet = memcmp( pcaKey, pcValue, llaKeyLen );
    }
    else
    {
    	iRet = memcmp( pcaKey, pcValue, keyData.llKeyLen );
    }
    
    if (iRet == 0)
    {
    	iDeleteSite = iMiddle;
      break;
    }
    
    /*目标值在中值右边*/
    if (iRet > 0)
    {
    	if (iMiddle == iRight)
    	{
    		//logFmtSyn( LVSYS, "[%d]", iMiddle );
    		return NOT_FOUND;
    	}
    	if (iMiddle == iRight - 1)
    	{
    		iLeft = iRight;
    	}
    	else
    	{
        iLeft = iMiddle + 1;
      }
    }
    /*目标值在中值左边*/
    else
    {
    	if (iMiddle == iLeft)
    	{
    		//logFmtSyn( LVSYS, "[%d]", iMiddle );
    		return NOT_FOUND;
    	}
    	if (iMiddle == iLeft + 1)
    	{
    		iRight = iLeft;
    	}
    	else
    	{
        iRight = iMiddle - 1;
      }
    }
  }
  
  /*释放结点在 VALUE 文件中的空间*/
	freeValueBlockMain( keyData.llKeyAddr, iaIndepentId );
  freeValueBlockMain( keyData.llValueAddr, iaIndepentId );
  
  //logFmtSyn( LVSYS, "[%d][%lld]", iDeleteSite, saIndexPoint->llChildSize );
  
	/*如果最后一级只有一个数据*/
	if (saIndexPoint->llChildSize == 1 )
	{
		*iaReplaceFlag = DELETE_3;
    return SUCCESS;
	}
	/*当前级没有足够的空间*/
	else
	{
		/*如果被删除的数据是第一个数据*/
		if (iDeleteSite == 0)
		{
			
			//logFmtSyn( LVSYS, "[%s][%s]", pcaKey, pcValue );
			/*设置数据*/
      memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
      memcpy( pcDataBufTmp, pcDataBuf + sizeof(KEY_VALUE_POINT), ((saIndexPoint->llChildSize) - 1) * sizeof(KEY_VALUE_POINT) );
      memcpy( pcDataBuf, pcDataBufTmp, ((saIndexPoint->llChildSize) - 1) * sizeof(KEY_VALUE_POINT) );
      memset( pcDataBuf + ((saIndexPoint->llChildSize) - 1) * sizeof(KEY_VALUE_POINT), 0x00, sizeof(KEY_VALUE_POINT) );
      
      /*设置当前级区的数据*/
      setKeyBlockDataMain( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentId );
      
      /*取出当前最小结点*/
      memset( &keyData, 0x00, sizeof(keyData) );
      memcpy( &keyData, pcDataBuf, sizeof(keyData) );
      
      /*设置数据*/
      (saIndexPoint->llChildSize)--;
      saIndexPoint->llKeyAddr = keyData.llKeyAddr;
      saIndexPoint->llKeyLen = keyData.llKeyLen;
      
      *iaReplaceFlag = DELETE_2;
    
      return SUCCESS;
		}
		else
		{			
			(saIndexPoint->llChildSize)--;
			
		  /*设置数据*/
      memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
      memcpy( pcDataBufTmp, pcDataBuf + (iDeleteSite + 1) * sizeof(KEY_VALUE_POINT), ((saIndexPoint->llChildSize) - iDeleteSite) * sizeof(KEY_VALUE_POINT) );
      memcpy( pcDataBuf + iDeleteSite * sizeof(KEY_VALUE_POINT), pcDataBufTmp, ((saIndexPoint->llChildSize) - iDeleteSite) * sizeof(KEY_VALUE_POINT) );
      memset( pcDataBuf + (saIndexPoint->llChildSize) * sizeof(KEY_VALUE_POINT), 0x00, sizeof(KEY_VALUE_POINT) );
      
      /*设置当前级区的数据*/
      setKeyBlockDataMain( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentId );

      *iaReplaceFlag = DELETE_1;
      return SUCCESS;
		}
	} 
}

int deleteIndexNodeTwoLoopsMain( char *pcaKey, __int64 llaKeyLen, int iaIndepentId )
{
  int  iStructOffset;                                      /*级区中的结构偏移量*/
  KEY_POINT key;                                           /*其他级数据中的数据结构*/
  char pcDataBuf[BLOCK_SIZE];                              /*级区中的数据*/
  char pcDataBufTmp[BLOCK_SIZE];                           /*级区中的数据临时变量*/
  int iReplaceFlag = 0;
  int iRet;
  	
  /*取得第一级索引区所有数据*/
  GET_BLOCK_DATA_MAIN( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
  
  /*在当前级索引区中搜索满足条件的下级索引区结点*/
  iStructOffset = scanfKeyPointMain( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain, pcaKey, llaKeyLen, iaIndepentId );
  
  /*取得当前下级结点*/
  GET_POINT_DATA_MAIN( key, pcDataBuf, iStructOffset );
    
  /*执行最后一级*/
  iRet = deleteIndexNodeLastLoopMain( &iReplaceFlag, &key, pcaKey, llaKeyLen, iaIndepentId );
  if (iRet == NOT_FOUND)
  {
  	return NOT_FOUND;
  }

	/*如果只是数量或值有变化*/
  if (iReplaceFlag == DELETE_1 || iReplaceFlag == DELETE_2)
  {
	  /*设置数据*/
    memcpy( pcDataBuf + iStructOffset * sizeof(KEY_POINT), &key, sizeof(KEY_POINT) );
    
    /*设置一级区的数据*/
    setKeyBlockDataMain( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
    
    return SUCCESS;
  }
  
  /*如果要删除该结点*/
  if (iReplaceFlag == DELETE_3)
  {
  	/*释放结点*/
  	freeKeyBlockMain( key.llChildAddr, iaIndepentId );
  	
	  memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
    memcpy( pcDataBufTmp, pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), ((sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain) - iStructOffset - 1) * sizeof(KEY_POINT) );
    memcpy( pcDataBuf + iStructOffset * sizeof(KEY_POINT), pcDataBufTmp, ((sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain) - iStructOffset - 1) * sizeof(KEY_POINT) );
    memset( pcDataBuf + ((sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain) - 1) * sizeof(KEY_POINT), 0x00, sizeof(KEY_POINT) );
    
    /*设置一级区的数据*/
    setKeyBlockDataMain( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
    
    (sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain)--;

    /*如果只剩下一个数据*/
    if( sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain == 1)
    {
      /*取得当前唯一的数据*/
      memset( &key, 0x00, sizeof(key) );
      memcpy( &key, pcDataBuf, sizeof(key) );
      
      /*释放第一个结点*/
      freeKeyBlockMain( sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
      
      /*重新设置第一个结点*/
      sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain = key.llChildAddr;
      sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain = key.llChildSize;
      (sgGlobalVariableMain[iaIndepentId].llgLoopsMain)--;
    }
    
    return SUCCESS;
  }
  
  return SUCCESS;
}

/********************************************************
函数名称：deleteIndexNodeMain
函数功能：插入索引数据 
入口参数：
第    一: 替换key标识            O
第    二：被挤出结点标识         O 
第    三：被挤出的值（第最后级的数据）         O
第    三：被挤出的值（第2到7级的数据）       O
第    四：输入结点               I/O
第    五：循环次数               I
第    六：关键字                 I 
第    七：值                     I
返    回：0表示成功，其他表示失败 
作    者：李高文 
创建时间：20151216
修改时间： 
*********************************************************/
int  deleteIndexNodeMain( int *iaReplaceFlag, KEY_POINT *saIndexPoint, int iaLoops,  char *pcaKey, __int64 llaKeyLen, int iaIndepentId )
{
  int  iStructOffset;                                      /*级区中的结构偏移量*/
  KEY_POINT key;                                           /*其他级数据中的数据结构*/
  char pcDataBuf[BLOCK_SIZE];                              /*级区中的数据*/
  char pcDataBufTmp[BLOCK_SIZE];                           /*级区中的数据临时变量*/
  int iReplaceFlag = 0;
  int iRet;
  int iTmp;


  /*如果是第一级*/
  if (iaLoops == 1)
  {
  	/*取得第一级索引区所有数据*/
  	GET_BLOCK_DATA_MAIN( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
    
    /*在当前级索引区中搜索满足条件的下级索引区结点*/
    iStructOffset = scanfKeyPointMain( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain, pcaKey, llaKeyLen, iaIndepentId );
    
    /*取得当前下级结点*/
    GET_POINT_DATA_MAIN( key, pcDataBuf, iStructOffset );
    
    /*往下一级执行*/
    iRet = deleteIndexNodeMain( &iReplaceFlag, &key, iaLoops + 1, pcaKey, llaKeyLen, iaIndepentId );
    if (iRet == NOT_FOUND)
    {
    	//logFmtSyn( LVNOR, "[%d]", iReplaceFlag );
    	return NOT_FOUND;
    }
        
	  /*如果只是数量或值有变化*/
    if (iReplaceFlag == DELETE_1 || iReplaceFlag == DELETE_2)
    {
	    /*设置数据*/
      memcpy( pcDataBuf + iStructOffset * sizeof(KEY_POINT), &key, sizeof(KEY_POINT) );
      
      /*设置一级区的数据*/
      setKeyBlockDataMain( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
      
      return SUCCESS;
    }
    
    /*如果要删除该结点*/
    if (iReplaceFlag == DELETE_3)
    {
    	/*释放结点*/
    	freeKeyBlockMain( key.llChildAddr, iaIndepentId );
    	
    	memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
      memcpy( pcDataBufTmp, pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), ((sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain) - iStructOffset - 1) * sizeof(KEY_POINT) );
      memcpy( pcDataBuf + iStructOffset * sizeof(KEY_POINT), pcDataBufTmp, ((sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain) - iStructOffset - 1) * sizeof(KEY_POINT) );
      memset( pcDataBuf + ((sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain) - 1) * sizeof(KEY_POINT), 0x00, sizeof(KEY_POINT) );
    
      /*设置一级区的数据*/
      setKeyBlockDataMain( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
    	
    	(sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain)--;

      /*如果只剩下一个数据*//*清理级数*/
      while (sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain == 1 && sgGlobalVariableMain[iaIndepentId].llgLoopsMain > 1)
      {
        /*取得当前唯一的数据*/
        memset( &key, 0x00, sizeof(key) );
        memcpy( &key, pcDataBuf, sizeof(key) );
      
        /*释放第一个结点*/
        freeKeyBlockMain( sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
      
        /*重新设置第一个结点*/
        sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain = key.llChildAddr;
        sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain = key.llChildSize;
        (sgGlobalVariableMain[iaIndepentId].llgLoopsMain)--;
        GET_BLOCK_DATA_MAIN( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
        //logFmtSyn( LVBUG, "[%lld][%lld][%lld]", sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain, sgGlobalVariableMain[iaIndepentId].llgLoopsMain );
      }
     	
      return SUCCESS;
    }
    
    /*这里，值有可能为 DELETE_4*/
    return SUCCESS;
  }
  /*其他级*/
  else
  {
  	/*如果是倒数第二级*/
  	if (iaLoops == sgGlobalVariableMain[iaIndepentId].llgLoopsMain - 1)
  	{
  		/*取得第一级索引区所有数据*/
  		GET_BLOCK_DATA_MAIN( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentId );
      
      /*在当前级索引区中搜索满足条件的下级索引区结点*/
      iStructOffset = scanfKeyPointMain( pcDataBuf, saIndexPoint->llChildSize, pcaKey, llaKeyLen, iaIndepentId );
      
      /*取得当前下级结点*/
      GET_POINT_DATA_MAIN( key, pcDataBuf, iStructOffset );    
      	
      /*执行最后一级*/
      iRet = deleteIndexNodeLastLoopMain( &iReplaceFlag, &key, pcaKey, llaKeyLen, iaIndepentId );
      if (iRet == NOT_FOUND)
      {
      	//logFmtSyn( LVSYS, "[%d]", iReplaceFlag );
      	return NOT_FOUND;
      }
      
	    /*如果只是数量或值有变化*/
      if (iReplaceFlag == DELETE_1 || iReplaceFlag == DELETE_2)
      {
	      /*设置数据*/
        memcpy( pcDataBuf + iStructOffset * sizeof(KEY_POINT), &key, sizeof(KEY_POINT) );
        
        /*设置一级区的数据*/
        setKeyBlockDataMain( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentId );
        
        /*如果是第一个数据，刚好值被替换*/
        if (iStructOffset == 0 && iReplaceFlag == DELETE_2)
        {
        	saIndexPoint->llKeyAddr = key.llKeyAddr;
          saIndexPoint->llKeyLen = key.llKeyLen;
          *iaReplaceFlag = DELETE_2;
        }
        else
        {
        	*iaReplaceFlag = DELETE_4;
        }
        return SUCCESS;
      }
      
      /*如果要删除该结点*/
      if (iReplaceFlag == DELETE_3)
      {
      	/*释放结点*/
      	freeKeyBlockMain( key.llChildAddr, iaIndepentId );
      	
      	/*如果只有一个结点*/
        if (saIndexPoint->llChildSize == 1)
        {
        	*iaReplaceFlag = DELETE_3;
        	return SUCCESS;
        }
      	
	      memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
        memcpy( pcDataBufTmp, pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), ((saIndexPoint->llChildSize) - iStructOffset - 1) * sizeof(KEY_POINT) );
        memcpy( pcDataBuf + iStructOffset * sizeof(KEY_POINT), pcDataBufTmp, ((saIndexPoint->llChildSize) - iStructOffset - 1) * sizeof(KEY_POINT) );
        memset( pcDataBuf + ((saIndexPoint->llChildSize) - 1) * sizeof(KEY_POINT), 0x00, sizeof(KEY_POINT) );
        
        /*设置该级区的数据*/
        setKeyBlockDataMain( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentId );
        
        (saIndexPoint->llChildSize)--;
        
        /*如果是第一个数据，刚好值被替换*/
        if (iStructOffset == 0)
        {
        	/*取出当前最小结点*/
          memset( &key, 0x00, sizeof(key) );
          memcpy( &key, pcDataBuf, sizeof(key) );
      
          /*设置数据*/
          saIndexPoint->llKeyAddr = key.llKeyAddr;
          saIndexPoint->llKeyLen = key.llKeyLen;
      
          *iaReplaceFlag = DELETE_2;
        }
        else
        {
        	*iaReplaceFlag = DELETE_1;
        }
        return SUCCESS;
      }
      
      *iaReplaceFlag = DELETE_4;
      return SUCCESS;
  	}
  	/*其他级*/
  	else
  	{
  	  /*取得当前级索引区所有数据*/
  	  GET_BLOCK_DATA_MAIN( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentId );
      
      /*在当前级索引区中搜索满足条件的下级索引区结点*/
      iStructOffset = scanfKeyPointMain( pcDataBuf, saIndexPoint->llChildSize, pcaKey, llaKeyLen, iaIndepentId );
      
      /*取得当前下级结点*/
      GET_POINT_DATA_MAIN( key, pcDataBuf, iStructOffset );
      
      /*往下一级执行*/
      iRet = deleteIndexNodeMain( &iReplaceFlag, &key, iaLoops + 1, pcaKey, llaKeyLen, iaIndepentId );
      //logFmtSyn( LVSYS, "[%d]", iReplaceFlag );
      if (iRet == NOT_FOUND)
      {
      	//logFmtSyn( LVDET, "[%d]", iReplaceFlag );
      	return NOT_FOUND;
      }
      
	    /*如果只是数量或值有变化*/
      if (iReplaceFlag == DELETE_1 || iReplaceFlag == DELETE_2)
      {
	      /*设置数据*/
        memcpy( pcDataBuf + iStructOffset * sizeof(KEY_POINT), &key, sizeof(KEY_POINT) );
        
        /*设置一级区的数据*/
        setKeyBlockDataMain( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentId );
        
        /*如果是第一个数据，刚好值被替换*/
        if (iStructOffset == 0 && iReplaceFlag == DELETE_2)
        {
        	saIndexPoint->llKeyAddr = key.llKeyAddr;
          saIndexPoint->llKeyLen = key.llKeyLen;
          *iaReplaceFlag = DELETE_2;
        }
        else
        {
        	*iaReplaceFlag = DELETE_4;
        }
        return SUCCESS;
      }
      
      /*如果要删除该结点*/
      if (iReplaceFlag == DELETE_3)
      {
      	/*释放结点*/
      	freeKeyBlockMain( key.llChildAddr, iaIndepentId );
      	
      	/*如果只有一个结点*/
        if (saIndexPoint->llChildSize == 1)
        {
        	*iaReplaceFlag = DELETE_3;
        	return SUCCESS;
        }
      	
	      memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
        memcpy( pcDataBufTmp, pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), ((saIndexPoint->llChildSize) - iStructOffset - 1) * sizeof(KEY_POINT) );
        memcpy( pcDataBuf + iStructOffset * sizeof(KEY_POINT), pcDataBufTmp, ((saIndexPoint->llChildSize) - iStructOffset - 1) * sizeof(KEY_POINT) );
        memset( pcDataBuf + ((saIndexPoint->llChildSize) - 1) * sizeof(KEY_POINT), 0x00, sizeof(KEY_POINT) );
        
        /*设置该级区的数据*/
        setKeyBlockDataMain( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentId );
        
        (saIndexPoint->llChildSize)--;
        
        /*如果是第一个数据，刚好值被替换*/
        if (iStructOffset == 0)
        {
        	/*取出当前最小结点*/
          memset( &key, 0x00, sizeof(key) );
          memcpy( &key, pcDataBuf, sizeof(key) );
      
          /*设置数据*/
          saIndexPoint->llKeyAddr = key.llKeyAddr;
          saIndexPoint->llKeyLen = key.llKeyLen;
      
          *iaReplaceFlag = DELETE_2;
        }
        else
        {
        	*iaReplaceFlag = DELETE_1;
        }
        
        return SUCCESS;
      }
      
      /*这里，值有可能为 DELETE_4*/
      *iaReplaceFlag = DELETE_4;
      return SUCCESS;
    }
  }
  return ERROR;
}
