#define __KEY_VALUE_DELETE_C

#ifndef __COMMON_HEAD_H
  #include "commonHead.h"
#endif

#define REPLACE_KEY_BUF_TO_VALUE2( sIndexPoint, pcaKey, llaKeyLen ) \
{\
  freeValueBlock( sIndexPoint.llKeyAddr );\
  sIndexPoint.llKeyLen = llaKeyLen;\
  sIndexPoint.llKeyAddr = mallocValueBlock( llaKeyLen );\
  bufToValue( sIndexPoint.llKeyAddr, pcaKey, llaKeyLen );\
}

int deleteNode( char *pcaKey, int iaKeyLen, int iaIndepentFileId );
int deleteIndexNodeOneLoops( char *pcaKey, int iaKeyLen, int iaIndepentFileId );
int deleteIndexNodeLastLoop( int *iaReplaceFlag,  KEY_POINT *saIndexPoint, char *pcaKey, int iaKeyLen, int iaIndepentFileId );
int deleteIndexNodeTwoLoops( char *pcaKey, int iaKeyLen, int iaIndepentFileId );
int deleteIndexNode( int *iaReplaceFlag, KEY_POINT *saIndexPoint, int iaLoops,  char *pcaKey, int iaKeyLen, int iaIndepentFileId );


/********************************************************
函数名称：deleteNode
函数功能：插入索引数据 
入口参数：
第    一: 索引值                 I
返    回：0表示成功，其他表示失败 
作    者：李高文 
创建时间：20160910
修改时间： 
*********************************************************/
int deleteNode( char *pcaKey, int iaKeyLen, int iaIndepentFileId )
{
	int iRet;
	
	/*获取锁*/
  UPDATE_DELETE_LOCK(iaIndepentFileId);
	if (sgBase[iaIndepentFileId].llFirstSize == 0)
  {
  	/*解锁*/
    UPDATE_DELETE_UNLOCK(iaIndepentFileId);
  
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
  	sgBase[iaIndepentFileId].llKeyValueSize--;
  }
  
  /*解锁*/
  UPDATE_DELETE_UNLOCK(iaIndepentFileId);
  
  return iRet;
}

int deleteIndexNodeOneLoops( char *pcaKey, int iaKeyLen, int iaIndepentFileId )
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
  getKeyBlockData( pcDataBuf, sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );

  iLeft = 0;
  iRight = sgBase[iaIndepentFileId].llFirstSize - 1;
  iMiddle = 0;

  while (1)
  {
    iMiddle = (iLeft + iRight)>>1;
    
    memset( &keyData, 0x00, sizeof(keyData) );        
    memcpy( &keyData, pcDataBuf + iMiddle * sizeof(KEY_VALUE_POINT), sizeof(KEY_VALUE_POINT) );
    
    /*从 VALUE 文件中获取 KEY 的值*/
    valueToBuf( pcValue, keyData.llKeyAddr, keyData.llKeyLen, iaIndepentFileId );
    pcValue[keyData.llKeyLen] = 0x00; 

    if (iaKeyLen >= keyData.llKeyLen)
    {
    	iRet = memcmp( pcaKey, pcValue, iaKeyLen );
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
	freeValueBlock( keyData.llKeyAddr, iaIndepentFileId );
	freeValueBlock( keyData.llValueAddr, iaIndepentFileId );
  
  /*如果只有一个数据*/
  if (sgBase[iaIndepentFileId].llFirstSize == 1)
  {
		sgBase[iaIndepentFileId].llFirstSize--;
		
		/*释放空间*/
		freeKeyBlock( sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );
		sgBase[iaIndepentFileId].llFirstDisc = 0;
    sgBase[iaIndepentFileId].llLoops = 1;
		
		return SUCCESS;
	}
  /*如果是所在的最后数据：直接删除该数据，全局变量减1，返回*/
  else
  if (iDeleteSite == sgBase[iaIndepentFileId].llFirstSize - 1)
  {
  	sgBase[iaIndepentFileId].llFirstSize--;
		memset( pcDataBuf + iDeleteSite * sizeof(KEY_VALUE_POINT), 0x00, sizeof(KEY_VALUE_POINT) );
		setKeyBlockData( pcDataBuf, sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );
		
		return SUCCESS;
	}
	/*如果不是最后的数据，是第一个数据：删除该数据，并将后面的数据上移，全局变量减1*/
	else
	if (iDeleteSite == 0)
	{
		memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
		memcpy( pcDataBufTmp, pcDataBuf + sizeof(KEY_VALUE_POINT), (sgBase[iaIndepentFileId].llFirstSize - 1) * sizeof(KEY_VALUE_POINT) );
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
		memcpy( pcDataBuf, pcDataBufTmp, (sgBase[iaIndepentFileId].llFirstSize - 1) * sizeof(KEY_VALUE_POINT) );
		
		sgBase[iaIndepentFileId].llFirstSize--;
		setKeyBlockData( pcDataBuf, sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );
		
		return SUCCESS;
  }
  /*如果不是最后的数据，也不是第一个数据：删除该数据，并将后面的数据上移，全局变量减1*/
  else
  {
		memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
		memcpy( pcDataBufTmp, pcDataBuf, iDeleteSite * sizeof(KEY_VALUE_POINT) );
		memcpy( pcDataBufTmp + iDeleteSite * sizeof(KEY_VALUE_POINT), pcDataBuf + (iDeleteSite + 1)* sizeof(KEY_VALUE_POINT), (sgBase[iaIndepentFileId].llFirstSize - iDeleteSite - 1) * sizeof(KEY_VALUE_POINT));
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
		memcpy( pcDataBuf, pcDataBufTmp, (sgBase[iaIndepentFileId].llFirstSize - 1) * sizeof(KEY_VALUE_POINT) );
		
		sgBase[iaIndepentFileId].llFirstSize--;
		setKeyBlockData( pcDataBuf, sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );
		
		return SUCCESS;
	}
	
  return ERROR;
}

int deleteIndexNodeLastLoop( int *iaReplaceFlag,  KEY_POINT *saIndexPoint, char *pcaKey, int iaKeyLen, int iaIndepentFileId )
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
  getKeyBlockData( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentFileId );

  iLeft = 0;
  iRight = (saIndexPoint->llChildSize) - 1;
  iMiddle = 0;

  while (1)
  {
    iMiddle = (iLeft + iRight)>>1;
    
    memset( &keyData, 0x00, sizeof(keyData) );        
    memcpy( &keyData, pcDataBuf + iMiddle * sizeof(KEY_VALUE_POINT), sizeof(KEY_VALUE_POINT) );
    
    /*从 VALUE 文件中获取 KEY 的值*/
    valueToBuf( pcValue, keyData.llKeyAddr, keyData.llKeyLen, iaIndepentFileId );
    pcValue[keyData.llKeyLen] = 0x00; 

    if (iaKeyLen >= keyData.llKeyLen)
    {
    	iRet = memcmp( pcaKey, pcValue, iaKeyLen );
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
	freeValueBlock( keyData.llKeyAddr, iaIndepentFileId );
  freeValueBlock( keyData.llValueAddr, iaIndepentFileId );
    
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
			/*设置数据*/
      memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
      memcpy( pcDataBufTmp, pcDataBuf + sizeof(KEY_VALUE_POINT), (saIndexPoint->llChildSize - 1) * sizeof(KEY_VALUE_POINT) );
      memcpy( pcDataBuf, pcDataBufTmp, (saIndexPoint->llChildSize - 1) * sizeof(KEY_VALUE_POINT) );
      memset( pcDataBuf + (saIndexPoint->llChildSize - 1) * sizeof(KEY_VALUE_POINT), 0x00, sizeof(KEY_VALUE_POINT) );
      
      /*设置当前级区的数据*/
      setKeyBlockData( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentFileId );
      
      /*取出当前最小结点*/
      memset( &keyData, 0x00, sizeof(keyData) );
      memcpy( &keyData, pcDataBuf, sizeof(keyData) );
      
      /*设置数据*/
      saIndexPoint->llChildSize--;
      saIndexPoint->llKeyAddr = keyData.llKeyAddr;
      saIndexPoint->llKeyLen = keyData.llKeyLen;
      
      *iaReplaceFlag = DELETE_2;
    
      return SUCCESS;
		}
		else
		{
		  /*设置数据*/
      memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
      memcpy( pcDataBufTmp, pcDataBuf + (iDeleteSite + 1) * sizeof(KEY_VALUE_POINT), (saIndexPoint->llChildSize - 1 - iDeleteSite) * sizeof(KEY_VALUE_POINT) );
      memcpy( pcDataBuf + iDeleteSite * sizeof(KEY_VALUE_POINT), pcDataBufTmp, (saIndexPoint->llChildSize - 1 - iDeleteSite) * sizeof(KEY_VALUE_POINT) );
      memset( pcDataBuf + (saIndexPoint->llChildSize - 1) * sizeof(KEY_VALUE_POINT), 0x00, sizeof(KEY_VALUE_POINT) );
      
      /*设置当前级区的数据*/
      setKeyBlockData( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentFileId );
      
      saIndexPoint->llChildSize--;
      
      *iaReplaceFlag = DELETE_1;
      return SUCCESS;
		}
	} 
}

int deleteIndexNodeTwoLoops( char *pcaKey, int iaKeyLen, int iaIndepentFileId )
{
  int  iStructOffset;                                      /*级区中的结构偏移量*/
  KEY_POINT key;                                           /*其他级数据中的数据结构*/
  char pcDataBuf[BLOCK_SIZE];                              /*级区中的数据*/
  char pcDataBufTmp[BLOCK_SIZE];                           /*级区中的数据临时变量*/
  int iReplaceFlag = 0;
  int iRet;
  	
  /*取得第一级索引区所有数据*/
  GET_BLOCK_DATA( pcDataBuf, sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );
  
  /*在当前级索引区中搜索满足条件的下级索引区结点*/
  iStructOffset = scanfKeyPoint( pcDataBuf, sgBase[iaIndepentFileId].llFirstSize, pcaKey, iaKeyLen, iaIndepentFileId );
  
  /*取得当前下级结点*/
  GET_POINT_DATA( key, pcDataBuf, iStructOffset );
    
  /*执行最后一级*/
  iRet = deleteIndexNodeLastLoop( &iReplaceFlag, &key, pcaKey, iaKeyLen, iaIndepentFileId );
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
    setKeyBlockData( pcDataBuf, sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );
    
    return SUCCESS;
  }
  
  /*如果要删除该结点*/
  if (iReplaceFlag == DELETE_3)
  {
  	/*释放结点*/
  	freeKeyBlock( key.llChildAddr, iaIndepentFileId );
  	
	  memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
    memcpy( pcDataBufTmp, pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), ((sgBase[iaIndepentFileId].llFirstSize) - iStructOffset - 1) * sizeof(KEY_POINT) );
    memcpy( pcDataBuf + iStructOffset * sizeof(KEY_POINT), pcDataBufTmp, ((sgBase[iaIndepentFileId].llFirstSize) - iStructOffset - 1) * sizeof(KEY_POINT) );
    memset( pcDataBuf + ((sgBase[iaIndepentFileId].llFirstSize) - 1) * sizeof(KEY_POINT), 0x00, sizeof(KEY_POINT) );
    
    /*设置一级区的数据*/
    setKeyBlockData( pcDataBuf, sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );
    
    sgBase[iaIndepentFileId].llFirstSize--;
    
    /*如果只剩下一个数据*/
    if( sgBase[iaIndepentFileId].llFirstSize == 1)
    {
      /*取得当前唯一的数据*/
      memset( &key, 0x00, sizeof(key) );
      memcpy( &key, pcDataBuf, sizeof(key) );
      
      /*释放第一个结点*/
      freeKeyBlock( sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );
      
      /*重新设置第一个结点*/
      sgBase[iaIndepentFileId].llFirstDisc = key.llChildAddr;
      sgBase[iaIndepentFileId].llFirstSize = key.llChildSize;
      sgBase[iaIndepentFileId].llLoops--;
    }
    
    return SUCCESS;
  }
  
  return SUCCESS;
}

/********************************************************
函数名称：deleteIndexNode
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
int  deleteIndexNode( int *iaReplaceFlag, KEY_POINT *saIndexPoint, int iaLoops,  char *pcaKey, int iaKeyLen, int iaIndepentFileId )
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
  	GET_BLOCK_DATA( pcDataBuf, sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );
    
    /*在当前级索引区中搜索满足条件的下级索引区结点*/
    iStructOffset = scanfKeyPoint( pcDataBuf, sgBase[iaIndepentFileId].llFirstSize, pcaKey, iaKeyLen, iaIndepentFileId );
    
    /*取得当前下级结点*/
    GET_POINT_DATA( key, pcDataBuf, iStructOffset );
    
    /*往下一级执行*/
    iRet = deleteIndexNode( &iReplaceFlag, &key, iaLoops + 1, pcaKey, iaKeyLen, iaIndepentFileId );
    
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
      setKeyBlockData( pcDataBuf, sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );
      
      return SUCCESS;
    }
    
    /*如果要删除该结点*/
    if (iReplaceFlag == DELETE_3)
    {
    	/*释放结点*/
    	freeKeyBlock( key.llChildAddr, iaIndepentFileId );
    	
    	memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
      memcpy( pcDataBufTmp, pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), ((sgBase[iaIndepentFileId].llFirstSize) - iStructOffset - 1) * sizeof(KEY_POINT) );
      memcpy( pcDataBuf + iStructOffset * sizeof(KEY_POINT), pcDataBufTmp, ((sgBase[iaIndepentFileId].llFirstSize) - iStructOffset - 1) * sizeof(KEY_POINT) );
      memset( pcDataBuf + ((sgBase[iaIndepentFileId].llFirstSize) - 1) * sizeof(KEY_POINT), 0x00, sizeof(KEY_POINT) );
    
      /*设置一级区的数据*/
      setKeyBlockData( pcDataBuf, sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );
    	
    	sgBase[iaIndepentFileId].llFirstSize--;
    
      /*如果只剩下一个数据*//*清理级数*/
      while( sgBase[iaIndepentFileId].llFirstSize == 1 && sgBase[iaIndepentFileId].llLoops > 1)
      {
        /*取得当前唯一的数据*/
        memset( &key, 0x00, sizeof(key) );
        memcpy( &key, pcDataBuf, sizeof(key) );
      
        /*释放第一个结点*/
        freeKeyBlock( sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );
      
        /*重新设置第一个结点*/
        sgBase[iaIndepentFileId].llFirstDisc = key.llChildAddr;
        sgBase[iaIndepentFileId].llFirstSize = key.llChildSize;
        sgBase[iaIndepentFileId].llLoops--;
        GET_BLOCK_DATA( pcDataBuf, sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );
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
  	if (iaLoops == sgBase[iaIndepentFileId].llLoops - 1)
  	{
  		/*取得第一级索引区所有数据*/
  		GET_BLOCK_DATA( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentFileId );
      
      /*在当前级索引区中搜索满足条件的下级索引区结点*/
      iStructOffset = scanfKeyPoint( pcDataBuf, saIndexPoint->llChildSize, pcaKey, iaKeyLen, iaIndepentFileId );
      
      /*取得当前下级结点*/
      GET_POINT_DATA( key, pcDataBuf, iStructOffset );    
      	
      /*执行最后一级*/
      iRet = deleteIndexNodeLastLoop( &iReplaceFlag, &key, pcaKey, iaKeyLen, iaIndepentFileId );
      
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
        setKeyBlockData( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentFileId );
        
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
      	freeKeyBlock( key.llChildAddr, iaIndepentFileId );
      	
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
        setKeyBlockData( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentFileId );
        
        /*如果是第一个数据，刚好值被替换*/
        if (iStructOffset == 0)
        {
        	/*取出当前最小结点*/
          memset( &key, 0x00, sizeof(key) );
          memcpy( &key, pcDataBuf, sizeof(key) );
      
          /*设置数据*/
          saIndexPoint->llChildSize--;
          saIndexPoint->llKeyAddr = key.llKeyAddr;
          saIndexPoint->llKeyLen = key.llKeyLen;
      
          *iaReplaceFlag = DELETE_2;
        }
        else
        {
        	saIndexPoint->llChildSize--;
        	*iaReplaceFlag = DELETE_1;
        }
        
        return SUCCESS;
      }
        
  	}
  	/*其他级*/
  	else
  	{
  	  /*取得当前级索引区所有数据*/
  	  GET_BLOCK_DATA( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentFileId );
      
      /*在当前级索引区中搜索满足条件的下级索引区结点*/
      iStructOffset = scanfKeyPoint( pcDataBuf, saIndexPoint->llChildSize, pcaKey, iaKeyLen, iaIndepentFileId );
      
      /*取得当前下级结点*/
      GET_POINT_DATA( key, pcDataBuf, iStructOffset );
      
      /*往下一级执行*/
      iRet = deleteIndexNode( &iReplaceFlag, &key, iaLoops + 1, pcaKey, iaKeyLen, iaIndepentFileId );
      
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
        setKeyBlockData( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentFileId );
        
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
      	freeKeyBlock( key.llChildAddr, iaIndepentFileId );
      	
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
        setKeyBlockData( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentFileId );
        
        /*如果是第一个数据，刚好值被替换*/
        if (iStructOffset == 0)
        {
        	/*取出当前最小结点*/
          memset( &key, 0x00, sizeof(key) );
          memcpy( &key, pcDataBuf, sizeof(key) );
      
          /*设置数据*/
          saIndexPoint->llChildSize--;
          saIndexPoint->llKeyAddr = key.llKeyAddr;
          saIndexPoint->llKeyLen = key.llKeyLen;
      
          *iaReplaceFlag = DELETE_2;
        }
        else
        {
        	saIndexPoint->llChildSize--;
        	*iaReplaceFlag = DELETE_1;
        }
        
        return SUCCESS;
      }
      
      /*这里，值有可能为 DELETE_4*/
      return SUCCESS;
    }
  }
  return ERROR;
}
