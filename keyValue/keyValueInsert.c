#define __KEY_VALUE_INSERT_C

#ifndef __COMMON_HEAD_H
  #include "commonHead.h"
#endif

int insertNode( char *pcaKey, __int64 llaKeyLen, char *pcaValue, __int64 llaValueLen, int iaIndepentFileId );
int insertIndexNodeOneLoops( char *pcaKey, __int64 llaKeyLen, char *pcaValue, __int64 llaValueLen, int iaIndepentFileId );
int insertIndexNodeLastLoop( int *iaReplaceFlag, int *iaOutFlag, KEY_VALUE_POINT *saOutIndexPoint, KEY_POINT *saIndexPoint,
char *pcaKey, __int64 llaKeyLen, char *pcaValue, __int64 llaValueLen, int iaIndepentFileId );
int insertIndexNodeTwoLoops( char *pcaKey, __int64 llaKeyLen, char *pcaValue, __int64 llaValueLen, int iaIndepentFileId );
int  insertIndexNode( int *iaReplaceFlag, int *iaOutFlag, KEY_POINT *saOutIndexPoint, KEY_POINT *saIndexPoint, int iaLoops, 
char *pcaKey, __int64 llaKeyLen, char *pcaValue, __int64 llaValueLen, int iaIndepentFileId );

/********************************************************
函数名称：insertNode
函数功能：插入索引数据 
入口参数：
第    一: 索引值                 I
返    回：0表示成功，其他表示失败 
作    者：李高文 
创建时间：20150818
修改时间： 
*********************************************************/
int insertNode( char *pcaKey, __int64 llaKeyLen, char *pcaValue, __int64 llaValueLen, int iaIndepentFileId )
{ 
  int iRet;

  /*如果三级以上*/
  if (sgBase[iaIndepentFileId].llLoops >= 3)
  {
  	iRet = insertIndexNode( NULL, NULL, NULL, NULL, 1, pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentFileId );
  }
  else
  /*如果只有两级*/
  if (sgBase[iaIndepentFileId].llLoops == 2)
  {
  	iRet = insertIndexNodeTwoLoops( pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentFileId );
  }
  else
  /*如果只有一级*/
  if (sgBase[iaIndepentFileId].llLoops <= 1)
  {
  	iRet = insertIndexNodeOneLoops( pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentFileId );
  }
  else
  {
    iRet = ERROR;
  }
  
  if (iRet == SUCCESS)
  {
  	sgBase[iaIndepentFileId].llKeyValueSize++;
  }

  return iRet;
}


int insertIndexNodeOneLoops( char *pcaKey, __int64 llaKeyLen, char *pcaValue, __int64 llaValueLen, int iaIndepentFileId )
{
  int  iStructOffset;                                   /*级区中的结构偏移量*/
  __int64 llFirstDiscNew;
  KEY_POINT key;                                        /*其他级数据中的数据结构*/
  KEY_POINT keyRight;                                   /*其他级数据中的数据结构（右结点）*/
  
  KEY_VALUE_POINT keyData;                              /*最后级数据中的数据结构*/
  KEY_VALUE_POINT keyDataMax;                           /*最后级数据中的数据结构*/
  KEY_VALUE_POINT keyDataMin;

  char pcDataBuf[BLOCK_SIZE];                           /*一个块的数据*/
  char pcDataBufTmp[BLOCK_SIZE];                        /*一个块的数据临时变量*/
  
  /*如果第一次插入数据*/
  if (sgBase[iaIndepentFileId].llFirstSize == 0)
  {
    /*申请索引块空间*/
    sgBase[iaIndepentFileId].llFirstDisc = mallocKeyBlock(iaIndepentFileId);
    if (sgBase[iaIndepentFileId].llFirstDisc == NO_BLOCK)
    {
      return ERROR;
    }

    /*设置数据*/
    {
    	/*设置结点*/
      SET_KEY_VALUE_DATA( keyData, pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentFileId );
      
      /*将数据写入到 KEY 文件中*/
      memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
      memcpy( pcDataBufTmp, &keyData, sizeof(KEY_VALUE_POINT) );
      setKeyBlockData( pcDataBufTmp, sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );

	    sgBase[iaIndepentFileId].llFirstSize = 1;
	    sgBase[iaIndepentFileId].llLoops = 1;
	    return SUCCESS;
    }
  }
  /*已经有数据*/
  else
  {
    /*如果当前级有足够的空间*/
    if (sgBase[iaIndepentFileId].llFirstSize < MAX_AERA_SIZE)
    {
      /*取得当前级所有数据*/
      GET_BLOCK_DATA( pcDataBuf, sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );
    
      /*搜索满足条件的结点*/  
      iStructOffset = scanfkeyValuePoint( pcDataBuf, sgBase[iaIndepentFileId].llFirstSize, pcaKey, llaKeyLen, iaIndepentFileId );          

      /*设置数据*/
      SET_KEY_VALUE_DATA( keyData, pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentFileId );
      
      memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
      memcpy( pcDataBufTmp, pcDataBuf + iStructOffset * sizeof(KEY_VALUE_POINT), ((sgBase[iaIndepentFileId].llFirstSize) - iStructOffset) * sizeof(KEY_VALUE_POINT) );
      memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_VALUE_POINT), pcDataBufTmp, ((sgBase[iaIndepentFileId].llFirstSize) - iStructOffset) * sizeof(KEY_VALUE_POINT) );
      memcpy( pcDataBuf + iStructOffset * sizeof(KEY_VALUE_POINT), &keyData, sizeof(KEY_VALUE_POINT) );
    
      /*级数量+1*/
      sgBase[iaIndepentFileId].llFirstSize++;
			    
      /*设置当前级数据*/
      setKeyBlockData( pcDataBuf, sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );

      return SUCCESS;
    }
    /*当前级没有足够的空间*/
    else
    {
      /*取得当前级所有数据*/
      GET_BLOCK_DATA( pcDataBuf, sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );
    
      /*取出当前的最大值，并设置级区结点*/
      {      
        /*搜索满足条件的结点*/  
        iStructOffset = scanfkeyValuePoint( pcDataBuf, MAX_AERA_SIZE, pcaKey, llaKeyLen, iaIndepentFileId );
      
        /*如果插入的就是最大值*/
        if (iStructOffset == MAX_AERA_SIZE)
        {
        	/*设置数据*/
        	SET_KEY_VALUE_DATA( keyDataMax, pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentFileId );
        }
        /*插入的值不是最大值*/
        else
        {
          /*取出当前的最大值*/
          GET_MAX_KEY( keyDataMax, pcDataBuf );
        
          /*设置结点*/
          SET_KEY_VALUE_DATA( keyData, pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentFileId );
      
          /*设置数据*/
          memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
          memcpy( pcDataBufTmp, pcDataBuf + iStructOffset * sizeof(KEY_VALUE_POINT), ((sgBase[iaIndepentFileId].llFirstSize) - 1 - iStructOffset) * sizeof(KEY_VALUE_POINT) );
          memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_VALUE_POINT), pcDataBufTmp, ((sgBase[iaIndepentFileId].llFirstSize) - 1 - iStructOffset) * sizeof(KEY_VALUE_POINT) );
              
          memcpy( pcDataBuf + iStructOffset * sizeof(KEY_VALUE_POINT), &keyData, sizeof(KEY_VALUE_POINT) );
          
          /*设置数据*/
          setKeyBlockData( pcDataBuf, sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );
        }
      }

      /*新建第一级*/
	    CREATE_NEW_FIRST_BLOCK( llFirstDiscNew, pcDataBuf, key, keyDataMin, iaIndepentFileId );

	    /*新建右结点，放下上面被挤出的最大值*/
	    CREATE_CHILD_POINT( pcDataBufTmp, keyRight, keyDataMax, iaIndepentFileId );

	    /*将新建区中的两个结点写入*/
	    WRITE_NEW_POINT( pcDataBufTmp, key, keyRight, llFirstDiscNew, iaIndepentFileId );
	    
	    return SUCCESS;
    }	
  }
  
  return ERROR;
}

int insertIndexNodeLastLoop( int *iaReplaceFlag, int *iaOutFlag, KEY_VALUE_POINT *saOutIndexPoint, KEY_POINT *saIndexPoint,
char *pcaKey, __int64 llaKeyLen, char *pcaValue, __int64 llaValueLen, int iaIndepentFileId )
{
  int  iStructOffset;                                      /*级区中的结构偏移量*/
  char pcKey[MAX_KEY_LEN];
  KEY_VALUE_POINT keyData;                                 /*最后级数据中的数据结构*/
  KEY_VALUE_POINT keyDataMax;                              /*最后级数据中的数据结构*/
  char pcDataBuf[BLOCK_SIZE];                              /*一个块的数据*/
  char pcDataBufTmp[BLOCK_SIZE];                           /*级区中的数据临时变量*/
    
	/*如果最后一级有足够的空间*/
	if (saIndexPoint->llChildSize < MAX_AERA_SIZE )
	{
		/*取得当前级索引区所有数据*/
		GET_BLOCK_DATA( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentFileId );
    
    /*搜索满足条件的结点*/  
    iStructOffset = scanfkeyValuePoint( pcDataBuf, saIndexPoint->llChildSize, pcaKey, llaKeyLen, iaIndepentFileId );          

    /*设置数据*/
    SET_KEY_VALUE_DATA( keyData, pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentFileId );
    
    memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
    memcpy( pcDataBufTmp, pcDataBuf + iStructOffset * sizeof(KEY_VALUE_POINT), ((saIndexPoint->llChildSize) - iStructOffset) * sizeof(KEY_VALUE_POINT) );
    memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_VALUE_POINT), pcDataBufTmp, ((saIndexPoint->llChildSize) - iStructOffset) * sizeof(KEY_VALUE_POINT) );
    memcpy( pcDataBuf + iStructOffset * sizeof(KEY_VALUE_POINT), &keyData, sizeof(KEY_VALUE_POINT) );
   
    /*修改当前级数量+1*/
    saIndexPoint->llChildSize++;
    
    /*设置数据*/
    setKeyBlockData( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentFileId );
  
    /*如果当前级的最小值有变*/
    memset( pcKey, 0x00, sizeof(pcKey) );
    valueToBuf( pcKey, saIndexPoint->llKeyAddr, saIndexPoint->llKeyLen, iaIndepentFileId );
    
    if (strcmp( pcaKey, pcKey ) < 0)
    {
      /*替换值*/
      saIndexPoint->llKeyAddr = keyData.llKeyAddr;
      saIndexPoint->llKeyLen = keyData.llKeyLen;
      *iaReplaceFlag = REPLACE_0;
    }
    else
    {
    	*iaReplaceFlag = REPLACE_2;
    }
    *iaOutFlag = NOT_OUT;
    
    return SUCCESS;
	}
	/*当前级没有足够的空间*/
	else
	{
		/*取得当前级索引区所有数据*/
		GET_BLOCK_DATA( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentFileId );
    
    /*取出当前的最大值，并设置当前级区结点*/
    {
      /*搜索满足条件的结点*/  
      iStructOffset = scanfkeyValuePoint( pcDataBuf, saIndexPoint->llChildSize, pcaKey, llaKeyLen, iaIndepentFileId );

      /*如果插入的就是最大值*/
      if (iStructOffset == saIndexPoint->llChildSize)
      {
      	/*设置值*/
        SET_KEY_VALUE_DATA( keyDataMax, pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentFileId );
      }
      /*插入的值不是最大值*/
      else
      {
        /*取出当前的最大值*/
        GET_MAX_KEY( keyDataMax, pcDataBuf );
        
        /*设置当前级区结点*/
        SET_KEY_VALUE_DATA( keyData, pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentFileId );
      
        /*设置数据*/
        memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
        memcpy( pcDataBufTmp, pcDataBuf + iStructOffset * sizeof(KEY_VALUE_POINT), (MAX_AERA_SIZE - 1 - iStructOffset) * sizeof(KEY_VALUE_POINT) );
        memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_VALUE_POINT), pcDataBufTmp, (MAX_AERA_SIZE - 1 - iStructOffset) * sizeof(KEY_VALUE_POINT) );
              
        memcpy( pcDataBuf + iStructOffset * sizeof(KEY_VALUE_POINT), &keyData, sizeof(KEY_VALUE_POINT) );
          
        /*设置当前级区的数据*/
        setKeyBlockData( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentFileId );
      }
    }
    
    memcpy( saOutIndexPoint, &keyDataMax, sizeof(KEY_VALUE_POINT) );
    
    /*如果当前级区的最小值有变*/
    memset( pcKey, 0x00, sizeof(pcKey) );
    valueToBuf( pcKey, saIndexPoint->llKeyAddr, saIndexPoint->llKeyLen, iaIndepentFileId );
    if (strcmp( pcaKey, pcKey ) < 0)
    {
    	/*替换值注意此处的 keyData 是上面的*/
    	saIndexPoint->llKeyAddr = keyData.llKeyAddr;
      saIndexPoint->llKeyLen = keyData.llKeyLen;
      *iaReplaceFlag = REPLACE_1;
    }
    else
    {
    	*iaReplaceFlag = REPLACE_3;
    }
    *iaOutFlag = OUT;
    return SUCCESS;
	} 
}

int insertIndexNodeTwoLoops( char *pcaKey, __int64 llaKeyLen, char *pcaValue, __int64 llaValueLen, int iaIndepentFileId )
{
  int  iStructOffset;                                      /*级区中的结构偏移量*/
  __int64 llFirstDiscNew;
  KEY_POINT key;                                           /*其他级数据中的数据结构*/
  KEY_POINT keyRight;                                      /*其他级数据中的数据结构（右结点）*/
  KEY_POINT keyMax;                                        /*其他级数据中的数据结构（最大结点）*/
  KEY_POINT keyMin;                                        /*其他级数据中的数据结构（最小结点）*/
  KEY_VALUE_POINT keyData;                                 /*最后级数据中的数据结构*/
  KEY_VALUE_POINT keyDataMax;                              /*最后级数据中的数据结构*/
  char pcDataBuf[BLOCK_SIZE];                              /*级区中的数据*/
  char pcDataBufTmp[BLOCK_SIZE];                           /*级区中的数据临时变量*/
  char pcDataBufRight[BLOCK_SIZE];                         /*级区中的数据*/
  char pcDataBufTmpRight[BLOCK_SIZE];                      /*级区中的数据*/
  int iReplaceFlag = 0;
  int iOutFlag = 0;
  	
  /*取得第一级索引区所有数据*/
  GET_BLOCK_DATA( pcDataBuf, sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );
  
  /*在当前级索引区中搜索满足条件的下级索引区结点*/
  iStructOffset = scanfKeyPoint( pcDataBuf, sgBase[iaIndepentFileId].llFirstSize, pcaKey, llaKeyLen, iaIndepentFileId );
  
  /*取得当前下级结点*/
  GET_POINT_DATA( key, pcDataBuf, iStructOffset );
  
  memset( &keyDataMax, 0x00, sizeof(keyDataMax) );

  /*执行最后一级*/
  insertIndexNodeLastLoop( &iReplaceFlag, &iOutFlag, &keyDataMax, &key, pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentFileId );

	/*如果关键值或数量有变化*/
  if (iReplaceFlag == REPLACE_0 || iReplaceFlag == REPLACE_1 || iReplaceFlag == REPLACE_2)
  {
	  /*设置数据*/
    memcpy( pcDataBuf + iStructOffset * sizeof(KEY_POINT), &key, sizeof(KEY_POINT) );
    
    /*设置一级区的数据*/
    setKeyBlockData( pcDataBuf, sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );
  }
  /*如果有数据被挤出*/
  if (iOutFlag == OUT)
  {
  	/*如果当前级没有右结点*/
  	if (iStructOffset == MAX_AERA_SIZE - 1)
  	{
      /*新建第一级*/
	    CREATE_NEW_FIRST_BLOCK( llFirstDiscNew, pcDataBuf, key, keyMin, iaIndepentFileId );
      
      /*新建右结点，放下上面被挤出的最大值*/
      CREATE_CHILD_POINT( pcDataBufTmp, keyMax, keyDataMax, iaIndepentFileId );
      
      /*新建右结点，放下上面新建的结点*/
      CREATE_CHILD_POINT( pcDataBufTmp, keyRight, keyMax, iaIndepentFileId );
      
      /*将新建区中的两个结点写入*/
      WRITE_NEW_POINT( pcDataBufTmp, key, keyRight, llFirstDiscNew, iaIndepentFileId );
      
  		return SUCCESS;
    }
    /*有右结点*/
    else
    {
   	  /*取出满足条件的右结点*/
   	  GET_POINT_DATA( keyRight, pcDataBuf, iStructOffset + 1 );
   	  
      /*如果右边的结点为空值*/
      if (keyRight.llChildSize == 0)
      {
      	CREATE_CHILD_POINT( pcDataBufTmp, keyRight, keyDataMax, iaIndepentFileId );
        
        /*修改当前级数量+1*/
        sgBase[iaIndepentFileId].llFirstSize++;
        
        /*设置当前级区的数据*/
        memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &keyRight, sizeof(KEY_POINT) );
        setKeyBlockData( pcDataBuf, sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );

        return SUCCESS;
      }
      /*如果右边的结点有数据，并且有足够的空间*/
      else
      if (keyRight.llChildSize < MAX_AERA_SIZE)
      {
      	/*取得兄弟下级索引区所有数据*/
      	GET_BLOCK_DATA( pcDataBufRight, keyRight.llChildAddr, iaIndepentFileId );
              
        /*设置兄弟下级区的结点*/
        SET_BROTHER_POINT( pcDataBufTmpRight, pcDataBufRight, keyRight, keyDataMax, iaIndepentFileId )

        /*设置当前级区中的下级区数量*/
        memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &keyRight, sizeof(KEY_POINT) );
        
        /*设置数据*/
        setKeyBlockData( pcDataBuf, sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );
        
        return SUCCESS;
      }
      /*如果右边的结点已满，但当前级有多余的结点空间*/
      else
      if (keyRight.llChildSize == MAX_AERA_SIZE && sgBase[iaIndepentFileId].llFirstSize < MAX_AERA_SIZE)
      {
      	/*新建下级区，将刚才数据插入*/
      	CREATE_CHILD_POINT( pcDataBufTmp, keyRight, keyDataMax, iaIndepentFileId );
        
        memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
        memcpy( pcDataBufTmp, pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), ((sgBase[iaIndepentFileId].llFirstSize) - iStructOffset - 1) * sizeof(KEY_POINT) );
        memcpy( pcDataBuf + (iStructOffset + 2) * sizeof(KEY_POINT), pcDataBufTmp, ((sgBase[iaIndepentFileId].llFirstSize) - iStructOffset - 1) * sizeof(KEY_POINT) );
                
        /*设置新建下级区的结点*/
        memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &keyRight, sizeof(KEY_POINT) );
                               
        sgBase[iaIndepentFileId].llFirstSize++;
                      
        /*设置一级区的数据*/
        setKeyBlockData( pcDataBuf, sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );
        
        return SUCCESS;
      }
      /*右边的结点已满，并且当前级区没有多余的结点空间*/
      else
      {
      	/*取出当前的最大值*/
        GET_MAX_KEY( keyMax, pcDataBuf );
        
        /*新建右结点，放下上面被挤出的最大值*/
        CREATE_CHILD_POINT( pcDataBufTmp, keyRight, keyDataMax, iaIndepentFileId );
        
        /*设置数据*/
        memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
        memcpy( pcDataBufTmp, pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), (MAX_AERA_SIZE - 1 - iStructOffset - 1) * sizeof(KEY_POINT) );
        memcpy( pcDataBuf + (iStructOffset + 2) * sizeof(KEY_POINT), pcDataBufTmp, (MAX_AERA_SIZE - 1 - iStructOffset - 1) * sizeof(KEY_POINT) );
            
        memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &keyRight, sizeof(KEY_POINT) );
        
        /*设置数据*/
        setKeyBlockData( pcDataBuf, sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );
        
        /*新建第一级*/
	      CREATE_NEW_FIRST_BLOCK( llFirstDiscNew, pcDataBuf, key, keyMin, iaIndepentFileId );
        
        /*新建区用来装这个最大值*/
        CREATE_CHILD_POINT( pcDataBufTmp, keyRight, keyMax, iaIndepentFileId );
  		  
  		  /*将新建区中的两个结点写入*/
  		  WRITE_NEW_POINT( pcDataBufTmp, key, keyRight, llFirstDiscNew, iaIndepentFileId );
  		  
  		  return SUCCESS;
      }
    }
  }
  else
  {
    return SUCCESS;
  }
}

/********************************************************
函数名称：insertIndexNode
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
int  insertIndexNode( int *iaReplaceFlag, int *iaOutFlag, KEY_POINT *saOutIndexPoint, KEY_POINT *saIndexPoint, int iaLoops, 
char *pcaKey, __int64 llaKeyLen, char *pcaValue, __int64 llaValueLen, int iaIndepentFileId )
{
  int  iStructOffset;                                      /*级区中的结构偏移量*/
  __int64 llFirstDiscNew;
  KEY_POINT key;                                           /*其他级数据中的数据结构*/
  KEY_POINT keyTmp;                                        /*其他级数据中的数据结构*/
  KEY_POINT keyRight;                                      /*其他级数据中的数据结构（右结点）*/
  KEY_POINT keyMax;                                        /*其他级数据中的数据结构（最大结点）*/
  KEY_POINT keyMin;                                        /*其他级数据中的数据结构（最小结点）*/
  KEY_VALUE_POINT keyData;                                 /*最后级数据中的数据结构*/
  KEY_VALUE_POINT keyDataMax;                              /*最后级数据中的数据结构*/
  KEY_VALUE_POINT keyDataMin;
  char pcDataBuf[BLOCK_SIZE];                              /*级区中的数据*/
  char pcDataBufTmp[BLOCK_SIZE];                           /*级区中的数据临时变量*/
  char pcDataBufRight[BLOCK_SIZE];                         /*级区中的数据*/
  char pcDataBufTmpRight[BLOCK_SIZE];                      /*级区中的数据临时变量*/
  int iReplaceFlag = 0;
  int iOutFlag = 0;
  
  /*如果是第一级*/
  if (iaLoops == 1)
  {
  	/*取得第一级索引区所有数据*/
  	GET_BLOCK_DATA( pcDataBuf, sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );
    
    /*在当前级索引区中搜索满足条件的下级索引区结点*/
    iStructOffset = scanfKeyPoint( pcDataBuf, sgBase[iaIndepentFileId].llFirstSize, pcaKey, llaKeyLen, iaIndepentFileId );
    
    /*取得当前下级结点*/
    GET_POINT_DATA( key, pcDataBuf, iStructOffset );
    
    memset( &keyMax, 0x00, sizeof(keyMax) );
    
    /*往下一级执行*/
    insertIndexNode( &iReplaceFlag, &iOutFlag, &keyMax, &key, iaLoops + 1, pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentFileId );
    
    /*如果关键值或数量有变化*/
    if (iReplaceFlag == REPLACE_0 || iReplaceFlag == REPLACE_1 || iReplaceFlag == REPLACE_2)
    {
      /*设置数据*/
      memcpy( pcDataBuf + iStructOffset * sizeof(KEY_POINT), &key, sizeof(KEY_POINT) );
      
      /*设置数据*/
      setKeyBlockData( pcDataBuf, sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );
    }
//logFmtAsy( LVNOR, "iStructOffset[%d]iOutFlag[%d]", iStructOffset, iOutFlag );
    
    /*如果有数据被挤出*/
    if (iOutFlag == OUT)
    {
    	/*如果当前级没有右结点*/
  	  if (iStructOffset == MAX_AERA_SIZE - 1)
  	  {
        /*新建第一级*/
	      CREATE_NEW_FIRST_BLOCK( llFirstDiscNew, pcDataBuf, key, keyMin, iaIndepentFileId );
        
        /*新建右结点，放下上面被挤出的最大值*/
        CREATE_CHILD_POINT( pcDataBufTmp, keyRight, keyMax, iaIndepentFileId );
      
        /*新建右结点，放下上面新建的结点*/
        CREATE_CHILD_POINT( pcDataBufTmp, keyTmp, keyRight, iaIndepentFileId );
      
        /*将新建区中的两个结点写入*/
        WRITE_NEW_POINT( pcDataBufTmp, key, keyTmp, llFirstDiscNew, iaIndepentFileId );
        
  	  	return SUCCESS;
      }
      /*有右结点*/
      else
      {
   	    /*取出满足条件的右结点*/
   	    GET_POINT_DATA( keyRight, pcDataBuf, iStructOffset + 1 );

//logFmtAsy( LVNOR, "keyRight.llChildAddr[%lld]keyRight.llChildSize[%lld]", keyRight.llChildAddr, keyRight.llChildSize );
              
        /*如果右边的结点为空值*/
        if (keyRight.llChildSize == 0)
        {
        	CREATE_CHILD_POINT( pcDataBufTmp, keyRight, keyMax, iaIndepentFileId );
          
          /*修改当前级数量+1*/
          sgBase[iaIndepentFileId].llFirstSize++;
          
          /*设置当前级区的数据*/
          memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &keyRight, sizeof(KEY_POINT) );
          setKeyBlockData( pcDataBuf, sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );
      
          return SUCCESS;
        }
        /*如果右边的结点有数据，并且有足够的空间*/
        else
        if (keyRight.llChildSize < MAX_AERA_SIZE)
        {
        	/*取得兄弟下级索引区所有数据*/
        	GET_BLOCK_DATA( pcDataBufRight, keyRight.llChildAddr, iaIndepentFileId );
                
          /*设置兄弟下级区的结点*/
          SET_BROTHER_POINT( pcDataBufTmpRight, pcDataBufRight, keyRight, keyMax, iaIndepentFileId )
      
          /*设置当前级区中的下级区数量*/
          memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &keyRight, sizeof(KEY_POINT) );
          
          /*设置数据*/
          setKeyBlockData( pcDataBuf, sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );
          
          return SUCCESS;
        }
        /*如果右边的结点已满，但当前级有多余的结点空间*/
        else
        if (keyRight.llChildSize == MAX_AERA_SIZE && sgBase[iaIndepentFileId].llFirstSize < MAX_AERA_SIZE)
        {
        	/*新建下级区，将刚才数据插入*/
        	CREATE_CHILD_POINT( pcDataBufTmp, keyRight, keyMax, iaIndepentFileId );
          
          memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
          memcpy( pcDataBufTmp, pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), ((sgBase[iaIndepentFileId].llFirstSize) - iStructOffset - 1) * sizeof(KEY_POINT) );
          memcpy( pcDataBuf + (iStructOffset + 2) * sizeof(KEY_POINT), pcDataBufTmp, ((sgBase[iaIndepentFileId].llFirstSize) - iStructOffset - 1) * sizeof(KEY_POINT) );
                  
          /*设置新建下级区的结点*/
          memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &keyRight, sizeof(KEY_POINT) );
                                 
          sgBase[iaIndepentFileId].llFirstSize++;
                        
          /*设置一级区的数据*/
          setKeyBlockData( pcDataBuf, sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );
          
          return SUCCESS;
        }
        /*右边的结点已满，并且当前级区没有多余的结点空间*/
        else
        {
          /*新建右结点，放下上面被挤出的最大值*/
          CREATE_CHILD_POINT( pcDataBufTmp, keyRight, keyMax, iaIndepentFileId );
          
          /*取出当前的最大值*//*这里复用变量 keyMax */
          GET_MAX_KEY( keyMax, pcDataBuf );
          
          /*设置数据*/
          memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
          memcpy( pcDataBufTmp, pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), ((sgBase[iaIndepentFileId].llFirstSize) - 1 - iStructOffset - 1) * sizeof(KEY_POINT) );
          memcpy( pcDataBuf + (iStructOffset + 2) * sizeof(KEY_POINT), pcDataBufTmp, ((sgBase[iaIndepentFileId].llFirstSize) - 1 - iStructOffset - 1) * sizeof(KEY_POINT) );
              
          memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &keyRight, sizeof(KEY_POINT) );
          
          /*设置数据*/
          setKeyBlockData( pcDataBuf, sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );
          
          /*新建第一级*/
	        CREATE_NEW_FIRST_BLOCK( llFirstDiscNew, pcDataBuf, key, keyMin, iaIndepentFileId );
	        
	        /*新建区用来装这个最大值*/
          CREATE_CHILD_POINT( pcDataBufTmp, keyRight, keyMax, iaIndepentFileId );
  	  	  
  	  	  /*将新建区中的两个结点写入*/
  	  	  WRITE_NEW_POINT( pcDataBufTmp, key, keyRight, llFirstDiscNew, iaIndepentFileId );
  	  	  
  	  	  return SUCCESS;
        }
      }
    }
    /*没必要操作*/
    else
    {
    	return SUCCESS;
    }
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
      iStructOffset = scanfKeyPoint( pcDataBuf, saIndexPoint->llChildSize, pcaKey, llaKeyLen, iaIndepentFileId );
      
      /*取得当前下级结点*/
      GET_POINT_DATA( key, pcDataBuf, iStructOffset );
      
      memset( &keyDataMax, 0x00, sizeof(keyDataMax) );
    
//logFmtAsy( LVNOR, "saIndexPoint->llChildAddr[%lld] iStructOffset[%d] key.llChildAddr[%lld] key.llChildSize[%lld]", saIndexPoint->llChildAddr, iStructOffset, key.llChildAddr, key.llChildSize );
      	
      /*执行最后一级*/
      insertIndexNodeLastLoop( &iReplaceFlag, &iOutFlag, &keyDataMax, &key, pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentFileId );
        
    	/*如果关键值或数量有变化*/
      if (iReplaceFlag == REPLACE_0 || iReplaceFlag == REPLACE_1 || iReplaceFlag == REPLACE_2)
      {
    	  /*设置数据*/
        memcpy( pcDataBuf + iStructOffset * sizeof(KEY_POINT), &key, sizeof(KEY_POINT) );

        /*设置一级区的数据*/
        setKeyBlockData( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentFileId );
      }
      
      /*如果刚好是第一数据的关键值变化*/
      if (iStructOffset == 0 && (iReplaceFlag == REPLACE_0 || iReplaceFlag == REPLACE_1))
      {
      	*iaReplaceFlag = REPLACE_1;
      	saIndexPoint->llKeyLen = key.llKeyLen;
        saIndexPoint->llKeyAddr = key.llKeyAddr;
      }
      else
      {
      	*iaReplaceFlag = REPLACE_3;
      }

//logFmtAsy( LVNOR, "iStructOffset[%d]iOutFlag[%d]", iStructOffset, iOutFlag );
      
      /*如果有数据被挤出*/
      if (iOutFlag == OUT)
      {
      	/*如果当前级没有右结点*/
      	if (iStructOffset == MAX_AERA_SIZE - 1)
      	{
      		CREATE_CHILD_POINT( pcDataBufTmp, keyRight, keyDataMax, iaIndepentFileId );
      		
      		/*将该结点挤出*/
      		memcpy( saOutIndexPoint, &keyRight, sizeof(KEY_POINT) );
      		*iaOutFlag = OUT;
      		
      		return SUCCESS;
	      }
	      /*有右结点*/
	      else
	      {
	      	/*取出满足条件的右结点*/
	      	GET_POINT_DATA( keyRight, pcDataBuf, iStructOffset + 1 );
	      	
//logFmtAsy( LVNOR, "keyRight.llChildAddr[%lld]keyRight.llChildSize[%lld]*iaReplaceFlag[%d]", keyRight.llChildAddr, keyRight.llChildSize, *iaReplaceFlag );
          
          /*如果右边的结点为空值*/
          if (keyRight.llChildSize == 0)
          {
          	CREATE_CHILD_POINT( pcDataBufTmp, keyRight, keyDataMax, iaIndepentFileId );
            
            /*修改当前级数量+1*/
            saIndexPoint->llChildSize++;
            
            /*设置当前级区的数据*/
            memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &keyRight, sizeof(KEY_POINT) );
            setKeyBlockData( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentFileId );
            
            if (*iaReplaceFlag == REPLACE_1)
            {
            	*iaReplaceFlag = REPLACE_0;
            }
            else
            {
            	*iaReplaceFlag = REPLACE_2;
            }
            
            *iaOutFlag = NOT_OUT;
            return SUCCESS;
          }
          /*如果右边的结点有数据，并且有足够的空间*/
          else
          if (keyRight.llChildSize < MAX_AERA_SIZE)
          {
          	/*取得兄弟下级索引区所有数据*/
          	GET_BLOCK_DATA( pcDataBufRight, keyRight.llChildAddr, iaIndepentFileId );
                 
            /*设置兄弟下级区的结点*/
            SET_BROTHER_POINT( pcDataBufTmpRight, pcDataBufRight, keyRight, keyDataMax, iaIndepentFileId );
                                      
            /*设置当前级区中的下级区数量*/
            memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &keyRight, sizeof(KEY_POINT) );
            setKeyBlockData( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentFileId );
            
            *iaOutFlag = NOT_OUT;
            return SUCCESS;
          }
          /*如果右边的结点已满，但当前级有多余的结点空间*/
          else
          if (keyRight.llChildSize == MAX_AERA_SIZE && saIndexPoint->llChildSize < MAX_AERA_SIZE)
          {
          	CREATE_CHILD_POINT( pcDataBufTmp, keyRight, keyDataMax, iaIndepentFileId );
            
            memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
            memcpy( pcDataBufTmp, pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), ((saIndexPoint->llChildSize) - iStructOffset - 1) * sizeof(KEY_POINT) );
            memcpy( pcDataBuf + (iStructOffset + 2) * sizeof(KEY_POINT), pcDataBufTmp, ((saIndexPoint->llChildSize) - iStructOffset - 1) * sizeof(KEY_POINT) );
                    
            /*设置新建下级区的结点*/
            memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &keyRight, sizeof(KEY_POINT) );
                                     
            saIndexPoint->llChildSize++;
                          
            /*设置当前级区的数据*/
            setKeyBlockData( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentFileId );
            
            if (*iaReplaceFlag == REPLACE_0)
            {
            	*iaReplaceFlag = REPLACE_1;
            }
            else
            {
            	*iaReplaceFlag = REPLACE_2;
            }
            
            *iaOutFlag = NOT_OUT;
            return SUCCESS;
          }
          /*右边的结点已满，并且当前级区没有多余的结点空间*/
          else
          {
          	CREATE_CHILD_POINT( pcDataBufTmp, key, keyDataMax, iaIndepentFileId );
            
            /*取出当前的最大值*/
            GET_MAX_KEY( keyMax, pcDataBuf );
            
            /*设置数据*/
            memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
            memcpy( pcDataBufTmp, pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), ((saIndexPoint->llChildSize) - 1 - iStructOffset - 1) * sizeof(KEY_POINT) );
            memcpy( pcDataBuf + (iStructOffset + 2) * sizeof(KEY_POINT), pcDataBufTmp, ((saIndexPoint->llChildSize) - 1 - iStructOffset - 1) * sizeof(KEY_POINT) );
                
            memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &key, sizeof(KEY_POINT) );
            
            /*设置数据*/
            setKeyBlockData( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentFileId );
      		  
      		  /*将该结点挤出*/
      		  memcpy( saOutIndexPoint, &keyMax, sizeof(KEY_POINT) );
      		  *iaOutFlag = OUT;
      		  
      		  return SUCCESS;
          }
	      }
      }
      /*没必要操作*/
      else
      {
      	return SUCCESS;
      }
  	}
  	/*其他级*/
  	else
  	{
  	  /*取得当前级索引区所有数据*/
  	  GET_BLOCK_DATA( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentFileId );
      
      /*在当前级索引区中搜索满足条件的下级索引区结点*/
      iStructOffset = scanfKeyPoint( pcDataBuf, saIndexPoint->llChildSize, pcaKey, llaKeyLen, iaIndepentFileId );
      
      /*取得当前下级结点*/
      GET_POINT_DATA( key, pcDataBuf, iStructOffset );
      
      memset( &keyMax, 0x00, sizeof(keyMax) );
      
      /*往下一级执行*/
      insertIndexNode( &iReplaceFlag, &iOutFlag, &keyMax, &key, iaLoops + 1, pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentFileId );
      
      /*如果关键值或数量有变化*/
      if (iReplaceFlag == REPLACE_0 || iReplaceFlag == REPLACE_1 || iReplaceFlag == REPLACE_2)
      {
        /*设置数据*/
        memcpy( pcDataBuf + iStructOffset * sizeof(KEY_POINT), &key, sizeof(KEY_POINT) );
        
        /*设置数据*/
        setKeyBlockData( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentFileId );
      }
      
      /*如果刚好是第一数据的关键值变化*/
      if (iStructOffset == 0 && (iReplaceFlag == REPLACE_0 || iReplaceFlag == REPLACE_1))
      {
      	*iaReplaceFlag = REPLACE_1;
      	saIndexPoint->llKeyLen = key.llKeyLen;
        saIndexPoint->llKeyAddr = key.llKeyAddr;
      }
      else
      {
      	*iaReplaceFlag = REPLACE_3;
      }
      
      /*如果有数据被挤出*/
      if (iOutFlag == OUT)
      {
      	/*如果当前级没有右结点*/
      	if (iStructOffset == MAX_AERA_SIZE - 1)
      	{
      		CREATE_CHILD_POINT( pcDataBufTmp, keyRight, keyMax, iaIndepentFileId );
      		
      		/*将该结点挤出*/
      		memcpy( saOutIndexPoint, &keyRight, sizeof(KEY_POINT) );
      		*iaOutFlag = OUT;
      		
      		return SUCCESS;
	      }
	      /*有右结点*/
	      else
	      {
	      	/*取出满足条件的右结点*/
	      	GET_POINT_DATA( keyRight, pcDataBuf, iStructOffset + 1 );
          
          /*如果右边的结点为空值*/
          if (keyRight.llChildSize == 0)
          {
          	CREATE_CHILD_POINT( pcDataBufTmp, keyRight, keyMax, iaIndepentFileId );
            
            /*修改当前级数量+1*/
            saIndexPoint->llChildSize++;
            
            /*设置当前级区的数据*/
            memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &keyRight, sizeof(KEY_POINT) );
            setKeyBlockData( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentFileId );
            
            if (*iaReplaceFlag == REPLACE_1)
            {
            	*iaReplaceFlag = REPLACE_0;
            }
            else
            {
            	*iaReplaceFlag = REPLACE_2;
            }
            
            *iaOutFlag = NOT_OUT;
            return SUCCESS;
          }
          /*如果右边的结点有数据，并且有足够的空间*/
          else
          if (keyRight.llChildSize < MAX_AERA_SIZE)
          {
          	/*取得兄弟下级索引区所有数据*/
          	GET_BLOCK_DATA( pcDataBufRight, keyRight.llChildAddr, iaIndepentFileId );
                  
            /*设置兄弟下级区的结点*/
            SET_BROTHER_POINT( pcDataBufTmpRight, pcDataBufRight, keyRight, keyMax, iaIndepentFileId );
                                      
            /*设置当前级区中的下级区数量*/
            memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &keyRight, sizeof(KEY_POINT) );
            setKeyBlockData( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentFileId );
            
            *iaOutFlag = NOT_OUT;
            return SUCCESS;
          }
          /*如果右边的结点已满，但当前级有多余的结点空间*/
          else
          if (keyRight.llChildSize == MAX_AERA_SIZE && saIndexPoint->llChildSize < MAX_AERA_SIZE )
          {
          	CREATE_CHILD_POINT( pcDataBufTmp, keyRight, keyMax, iaIndepentFileId );
            
            memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
            memcpy( pcDataBufTmp, pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), ((saIndexPoint->llChildSize) - iStructOffset - 1) * sizeof(KEY_POINT) );
            memcpy( pcDataBuf + (iStructOffset + 2) * sizeof(KEY_POINT), pcDataBufTmp, ((saIndexPoint->llChildSize) - iStructOffset - 1) * sizeof(KEY_POINT) );
                    
            /*设置新建下级区的结点*/
            memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &keyRight, sizeof(KEY_POINT) );
                                     
            saIndexPoint->llChildSize++;
                          
            /*设置当前级区的数据*/
            setKeyBlockData( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentFileId );
            
            if (*iaReplaceFlag == REPLACE_0)
            {
            	*iaReplaceFlag = REPLACE_1;
            }
            else
            {
            	*iaReplaceFlag = REPLACE_2;
            }
            
            *iaOutFlag = NOT_OUT;
            return SUCCESS;
          }
          /*右边的结点已满，并且当前级区没有多余的结点空间*/
          else
          {
          	CREATE_CHILD_POINT( pcDataBufTmp, key, keyMax, iaIndepentFileId );
            
            /*取出当前的最大值*/
            GET_MAX_KEY( keyMax, pcDataBuf );
            
            /*设置数据*/
            memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
            memcpy( pcDataBufTmp, pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), ((saIndexPoint->llChildSize) - 1 - iStructOffset - 1) * sizeof(KEY_POINT) );
            memcpy( pcDataBuf + (iStructOffset + 2) * sizeof(KEY_POINT), pcDataBufTmp, ((saIndexPoint->llChildSize) - 1 - iStructOffset - 1) * sizeof(KEY_POINT) );
                
            memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &key, sizeof(KEY_POINT) );
            
            /*设置数据*/
            setKeyBlockData( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentFileId );
      		  
      		  /*将该结点挤出*/
      		  memcpy( saOutIndexPoint, &keyMax, sizeof(KEY_POINT) );
      		  *iaOutFlag = OUT;
      		  
      		  return SUCCESS;
          }
	      }
      }
      /*没必要操作*/
      else
      {
      	return SUCCESS;
      }
    }
  }
  return ERROR;
}

