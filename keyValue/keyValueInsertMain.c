#define __KEY_VALUE_INSERT_MAIN_C

#ifndef __COMMON_HEAD_H
  #include "commonHead.h"
#endif

int insertNodeMain( char *pcaKey, __int64 llaKeyLen, char *pcaValue, __int64 llaValueLen, int iaIndepentId );
int insertIndexNodeOneLoopsMain( char *pcaKey, __int64 llaKeyLen, char *pcaValue, __int64 llaValueLen, int iaIndepentId );
int insertIndexNodeLastLoopMain( int *iaReplaceFlag, int *iaOutFlag, KEY_VALUE_POINT *saOutIndexPoint, KEY_POINT *saIndexPoint,
char *pcaKey, __int64 llaKeyLen, char *pcaValue, __int64 llaValueLen, int iaIndepentId );
int insertIndexNodeTwoLoopsMain( char *pcaKey, __int64 llaKeyLen, char *pcaValue, __int64 llaValueLen, int iaIndepentId );
int  insertIndexNodeMain( int *iaReplaceFlag, int *iaOutFlag, KEY_POINT *saOutIndexPoint, KEY_POINT *saIndexPoint, int iaLoops, 
char *pcaKey, __int64 llaKeyLen, char *pcaValue, __int64 llaValueLen, int iaIndepentId );

/********************************************************
函数名称：insertNodeMain
函数功能：插入索引数据 
入口参数：
第    一: 索引值                 I
返    回：0表示成功，其他表示失败 
作    者：李高文 
创建时间：20150818
修改时间： 
*********************************************************/
int insertNodeMain( char *pcaKey, __int64 llaKeyLen, char *pcaValue, __int64 llaValueLen, int iaIndepentId )
{ 
  int iRet;
  
  //logFmtSyn( LVBUG, "pcaKey[%s]pcaValue[%s]sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain[%lld]sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain[%lld]sgGlobalVariableMain[iaIndepentId].llgLoopsMain[%lld]sgGlobalVariableMain[iaIndepentId].llgKeyValueSizeMain[%lld]", pcaKey, pcaValue, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain, sgGlobalVariableMain[iaIndepentId].llgLoopsMain, sgGlobalVariableMain[iaIndepentId].llgKeyValueSizeMain );

  /*如果三级以上*/
  if (sgGlobalVariableMain[iaIndepentId].llgLoopsMain >= 3)
  {
  	iRet = insertIndexNodeMain( NULL, NULL, NULL, NULL, 1, pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentId );
  }
  else
  /*如果只有两级*/
  if (sgGlobalVariableMain[iaIndepentId].llgLoopsMain == 2)
  {
  	iRet = insertIndexNodeTwoLoopsMain( pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentId );
  }
  else
  /*如果只有一级*/
  if (sgGlobalVariableMain[iaIndepentId].llgLoopsMain <= 1)
  {
  	iRet = insertIndexNodeOneLoopsMain( pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentId );
  }
  else
  {
    iRet = ERROR;
  }
  
  if (iRet == SUCCESS)
  {
  	sgGlobalVariableMain[iaIndepentId].llgKeyValueSizeMain++;
  }
  
  return iRet;
}


int insertIndexNodeOneLoopsMain( char *pcaKey, __int64 llaKeyLen, char *pcaValue, __int64 llaValueLen, int iaIndepentId )
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
  if (sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain == 0)
  {
    /*申请索引块空间*/
    sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain = mallocKeyBlockMain(iaIndepentId);
    if (sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain == NO_BLOCK)
    {
      return ERROR;
    }

    /*设置数据*/
    {
    	/*设置结点*/
      SET_KEY_VALUE_DATA_MAIN( keyData, pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentId );
      
      /*将数据写入到 KEY 文件中*/
      memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
      memcpy( pcDataBufTmp, &keyData, sizeof(KEY_VALUE_POINT) );
      setKeyBlockDataMain( pcDataBufTmp, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );

	    sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain = 1;
	    sgGlobalVariableMain[iaIndepentId].llgLoopsMain = 1;
	    return SUCCESS;
    }
  }
  /*已经有数据*/
  else
  {
    /*如果当前级有足够的空间*/
    if (sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain < MAX_AERA_SIZE)
    {
      /*取得当前级所有数据*/
      GET_BLOCK_DATA_MAIN( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
    
      /*搜索满足条件的结点*/  
      iStructOffset = scanfkeyValuePointMain( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain, pcaKey, llaKeyLen, iaIndepentId );          

      /*设置数据*/
      SET_KEY_VALUE_DATA_MAIN( keyData, pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentId );
      
      memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
      memcpy( pcDataBufTmp, pcDataBuf + iStructOffset * sizeof(KEY_VALUE_POINT), ((sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain) - iStructOffset) * sizeof(KEY_VALUE_POINT) );
      memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_VALUE_POINT), pcDataBufTmp, ((sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain) - iStructOffset) * sizeof(KEY_VALUE_POINT) );
      memcpy( pcDataBuf + iStructOffset * sizeof(KEY_VALUE_POINT), &keyData, sizeof(KEY_VALUE_POINT) );
    
      /*级数量+1*/
      sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain++;
			    
      /*设置当前级数据*/
      setKeyBlockDataMain( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );

      return SUCCESS;
    }
    /*当前级没有足够的空间*/
    else
    {
      /*取得当前级所有数据*/
      GET_BLOCK_DATA_MAIN( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
    
      /*取出当前的最大值，并设置级区结点*/
      {      
        /*搜索满足条件的结点*/  
        iStructOffset = scanfkeyValuePointMain( pcDataBuf, MAX_AERA_SIZE, pcaKey, llaKeyLen, iaIndepentId );
      
        /*如果插入的就是最大值*/
        if (iStructOffset == MAX_AERA_SIZE)
        {
        	/*设置数据*/
        	SET_KEY_VALUE_DATA_MAIN( keyDataMax, pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentId );
        }
        /*插入的值不是最大值*/
        else
        {
          /*取出当前的最大值*/
          GET_MAX_KEY_MAIN( keyDataMax, pcDataBuf );
        
          /*设置结点*/
          SET_KEY_VALUE_DATA_MAIN( keyData, pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentId );
      
          /*设置数据*/
          memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
          memcpy( pcDataBufTmp, pcDataBuf + iStructOffset * sizeof(KEY_VALUE_POINT), ((sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain) - 1 - iStructOffset) * sizeof(KEY_VALUE_POINT) );
          memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_VALUE_POINT), pcDataBufTmp, ((sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain) - 1 - iStructOffset) * sizeof(KEY_VALUE_POINT) );
              
          memcpy( pcDataBuf + iStructOffset * sizeof(KEY_VALUE_POINT), &keyData, sizeof(KEY_VALUE_POINT) );
          
          /*设置数据*/
          setKeyBlockDataMain( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
        }
      }

      /*新建第一级*/
	    CREATE_NEW_FIRST_BLOCK_MAIN( llFirstDiscNew, pcDataBuf, key, keyDataMin, iaIndepentId );

	    /*新建右结点，放下上面被挤出的最大值*/
	    CREATE_CHILD_POINT_MAIN( pcDataBufTmp, keyRight, keyDataMax, iaIndepentId );

	    /*将新建区中的两个结点写入*/
	    WRITE_NEW_POINT_MAIN( pcDataBufTmp, key, keyRight, llFirstDiscNew, iaIndepentId );
	    
	    return SUCCESS;
    }	
  }
  
  return ERROR;
}

int insertIndexNodeLastLoopMain( int *iaReplaceFlag, int *iaOutFlag, KEY_VALUE_POINT *saOutIndexPoint, KEY_POINT *saIndexPoint,
char *pcaKey, __int64 llaKeyLen, char *pcaValue, __int64 llaValueLen, int iaIndepentId )
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
		GET_BLOCK_DATA_MAIN( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentId );
    
    /*搜索满足条件的结点*/  
    iStructOffset = scanfkeyValuePointMain( pcDataBuf, saIndexPoint->llChildSize, pcaKey, llaKeyLen, iaIndepentId );          

    /*设置数据*/
    SET_KEY_VALUE_DATA_MAIN( keyData, pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentId );
    
    memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
    memcpy( pcDataBufTmp, pcDataBuf + iStructOffset * sizeof(KEY_VALUE_POINT), ((saIndexPoint->llChildSize) - iStructOffset) * sizeof(KEY_VALUE_POINT) );
    memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_VALUE_POINT), pcDataBufTmp, ((saIndexPoint->llChildSize) - iStructOffset) * sizeof(KEY_VALUE_POINT) );
    memcpy( pcDataBuf + iStructOffset * sizeof(KEY_VALUE_POINT), &keyData, sizeof(KEY_VALUE_POINT) );
   
    /*修改当前级数量+1*/
    saIndexPoint->llChildSize++;
    
    /*设置数据*/
    setKeyBlockDataMain( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentId );
  
    /*如果当前级的最小值有变*/
    memset( pcKey, 0x00, sizeof(pcKey) );
    valueToBufMain( pcKey, saIndexPoint->llKeyAddr, saIndexPoint->llKeyLen, iaIndepentId );
    
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
		GET_BLOCK_DATA_MAIN( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentId );
    
    /*取出当前的最大值，并设置当前级区结点*/
    {
      /*搜索满足条件的结点*/  
      iStructOffset = scanfkeyValuePointMain( pcDataBuf, saIndexPoint->llChildSize, pcaKey, llaKeyLen, iaIndepentId );

      /*如果插入的就是最大值*/
      if (iStructOffset == saIndexPoint->llChildSize)
      {
      	/*设置值*/
        SET_KEY_VALUE_DATA_MAIN( keyDataMax, pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentId );
      }
      /*插入的值不是最大值*/
      else
      {
        /*取出当前的最大值*/
        GET_MAX_KEY_MAIN( keyDataMax, pcDataBuf );
        
        /*设置当前级区结点*/
        SET_KEY_VALUE_DATA_MAIN( keyData, pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentId );
      
        /*设置数据*/
        memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
        memcpy( pcDataBufTmp, pcDataBuf + iStructOffset * sizeof(KEY_VALUE_POINT), (MAX_AERA_SIZE - 1 - iStructOffset) * sizeof(KEY_VALUE_POINT) );
        memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_VALUE_POINT), pcDataBufTmp, (MAX_AERA_SIZE - 1 - iStructOffset) * sizeof(KEY_VALUE_POINT) );
              
        memcpy( pcDataBuf + iStructOffset * sizeof(KEY_VALUE_POINT), &keyData, sizeof(KEY_VALUE_POINT) );
          
        /*设置当前级区的数据*/
        setKeyBlockDataMain( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentId );
      }
    }
    
    memcpy( saOutIndexPoint, &keyDataMax, sizeof(KEY_VALUE_POINT) );
    
    /*如果当前级区的最小值有变*/
    memset( pcKey, 0x00, sizeof(pcKey) );
    valueToBufMain( pcKey, saIndexPoint->llKeyAddr, saIndexPoint->llKeyLen, iaIndepentId );
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

int insertIndexNodeTwoLoopsMain( char *pcaKey, __int64 llaKeyLen, char *pcaValue, __int64 llaValueLen, int iaIndepentId )
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
  GET_BLOCK_DATA_MAIN( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
  
  /*在当前级索引区中搜索满足条件的下级索引区结点*/
  iStructOffset = scanfKeyPointMain( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain, pcaKey, llaKeyLen, iaIndepentId );
  
  /*取得当前下级结点*/
  GET_POINT_DATA_MAIN( key, pcDataBuf, iStructOffset );
  
  memset( &keyDataMax, 0x00, sizeof(keyDataMax) );

  /*执行最后一级*/
  insertIndexNodeLastLoopMain( &iReplaceFlag, &iOutFlag, &keyDataMax, &key, pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentId );

	/*如果关键值或数量有变化*/
  if (iReplaceFlag == REPLACE_0 || iReplaceFlag == REPLACE_1 || iReplaceFlag == REPLACE_2)
  {
	  /*设置数据*/
    memcpy( pcDataBuf + iStructOffset * sizeof(KEY_POINT), &key, sizeof(KEY_POINT) );
    
    /*设置一级区的数据*/
    setKeyBlockDataMain( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
  }
  /*如果有数据被挤出*/
  if (iOutFlag == OUT)
  {
  	/*如果当前级没有右结点*/
  	if (iStructOffset == MAX_AERA_SIZE - 1)
  	{
      /*新建第一级*/
	    CREATE_NEW_FIRST_BLOCK_MAIN( llFirstDiscNew, pcDataBuf, key, keyMin, iaIndepentId );
      
      /*新建右结点，放下上面被挤出的最大值*/
      CREATE_CHILD_POINT_MAIN( pcDataBufTmp, keyMax, keyDataMax, iaIndepentId );
      
      /*新建右结点，放下上面新建的结点*/
      CREATE_CHILD_POINT_MAIN( pcDataBufTmp, keyRight, keyMax, iaIndepentId );
      
      /*将新建区中的两个结点写入*/
      WRITE_NEW_POINT_MAIN( pcDataBufTmp, key, keyRight, llFirstDiscNew, iaIndepentId );
      
  		return SUCCESS;
    }
    /*有右结点*/
    else
    {
   	  /*取出满足条件的右结点*/
   	  GET_POINT_DATA_MAIN( keyRight, pcDataBuf, iStructOffset + 1 );
   	  
      /*如果右边的结点为空值*/
      if (keyRight.llChildSize == 0)
      {
      	CREATE_CHILD_POINT_MAIN( pcDataBufTmp, keyRight, keyDataMax, iaIndepentId );
        
        /*修改当前级数量+1*/
        sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain++;
        
        /*设置当前级区的数据*/
        memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &keyRight, sizeof(KEY_POINT) );
        setKeyBlockDataMain( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );

        return SUCCESS;
      }
      /*如果右边的结点有数据，并且有足够的空间*/
      else
      if (keyRight.llChildSize < MAX_AERA_SIZE)
      {
      	/*取得兄弟下级索引区所有数据*/
      	GET_BLOCK_DATA_MAIN( pcDataBufRight, keyRight.llChildAddr, iaIndepentId );
              
        /*设置兄弟下级区的结点*/
        SET_BROTHER_POINT_MAIN( pcDataBufTmpRight, pcDataBufRight, keyRight, keyDataMax, iaIndepentId )

        /*设置当前级区中的下级区数量*/
        memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &keyRight, sizeof(KEY_POINT) );
        
        /*设置数据*/
        setKeyBlockDataMain( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
        
        return SUCCESS;
      }
      /*如果右边的结点已满，但当前级有多余的结点空间*/
      else
      if (keyRight.llChildSize == MAX_AERA_SIZE && sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain < MAX_AERA_SIZE)
      {
      	/*新建下级区，将刚才数据插入*/
      	CREATE_CHILD_POINT_MAIN( pcDataBufTmp, keyRight, keyDataMax, iaIndepentId );
        
        memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
        memcpy( pcDataBufTmp, pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), ((sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain) - iStructOffset - 1) * sizeof(KEY_POINT) );
        memcpy( pcDataBuf + (iStructOffset + 2) * sizeof(KEY_POINT), pcDataBufTmp, ((sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain) - iStructOffset - 1) * sizeof(KEY_POINT) );
                
        /*设置新建下级区的结点*/
        memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &keyRight, sizeof(KEY_POINT) );
                               
        sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain++;
                      
        /*设置一级区的数据*/
        setKeyBlockDataMain( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
        
        return SUCCESS;
      }
      /*右边的结点已满，并且当前级区没有多余的结点空间*/
      else
      {
      	/*取出当前的最大值*/
        GET_MAX_KEY_MAIN( keyMax, pcDataBuf );
        
        /*新建右结点，放下上面被挤出的最大值*/
        CREATE_CHILD_POINT_MAIN( pcDataBufTmp, keyRight, keyDataMax, iaIndepentId );
        
        /*设置数据*/
        memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
        memcpy( pcDataBufTmp, pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), (MAX_AERA_SIZE - 1 - iStructOffset - 1) * sizeof(KEY_POINT) );
        memcpy( pcDataBuf + (iStructOffset + 2) * sizeof(KEY_POINT), pcDataBufTmp, (MAX_AERA_SIZE - 1 - iStructOffset - 1) * sizeof(KEY_POINT) );
            
        memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &keyRight, sizeof(KEY_POINT) );
        
        /*设置数据*/
        setKeyBlockDataMain( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
        
        /*新建第一级*/
	      CREATE_NEW_FIRST_BLOCK_MAIN( llFirstDiscNew, pcDataBuf, key, keyMin, iaIndepentId );
        
        /*新建区用来装这个最大值*/
        CREATE_CHILD_POINT_MAIN( pcDataBufTmp, keyRight, keyMax, iaIndepentId );
  		  
  		  /*将新建区中的两个结点写入*/
  		  WRITE_NEW_POINT_MAIN( pcDataBufTmp, key, keyRight, llFirstDiscNew, iaIndepentId );
  		  
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
函数名称：insertIndexNodeMain
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
int  insertIndexNodeMain( int *iaReplaceFlag, int *iaOutFlag, KEY_POINT *saOutIndexPoint, KEY_POINT *saIndexPoint, int iaLoops, 
char *pcaKey, __int64 llaKeyLen, char *pcaValue, __int64 llaValueLen, int iaIndepentId )
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
  	GET_BLOCK_DATA_MAIN( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
    
    /*在当前级索引区中搜索满足条件的下级索引区结点*/
    iStructOffset = scanfKeyPointMain( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain, pcaKey, llaKeyLen, iaIndepentId );
    
    /*取得当前下级结点*/
    GET_POINT_DATA_MAIN( key, pcDataBuf, iStructOffset );
    
    memset( &keyMax, 0x00, sizeof(keyMax) );
    
    /*往下一级执行*/
    insertIndexNodeMain( &iReplaceFlag, &iOutFlag, &keyMax, &key, iaLoops + 1, pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentId );
    
    /*如果关键值或数量有变化*/
    if (iReplaceFlag == REPLACE_0 || iReplaceFlag == REPLACE_1 || iReplaceFlag == REPLACE_2)
    {
      /*设置数据*/
      memcpy( pcDataBuf + iStructOffset * sizeof(KEY_POINT), &key, sizeof(KEY_POINT) );
      
      /*设置数据*/
      setKeyBlockDataMain( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
    }
//logFmtAsy( LVNOR, "iStructOffset[%d]iOutFlag[%d]", iStructOffset, iOutFlag );
    
    /*如果有数据被挤出*/
    if (iOutFlag == OUT)
    {
    	/*如果当前级没有右结点*/
  	  if (iStructOffset == MAX_AERA_SIZE - 1)
  	  {
        /*新建第一级*/
	      CREATE_NEW_FIRST_BLOCK_MAIN( llFirstDiscNew, pcDataBuf, key, keyMin, iaIndepentId );
        
        /*新建右结点，放下上面被挤出的最大值*/
        CREATE_CHILD_POINT_MAIN( pcDataBufTmp, keyRight, keyMax, iaIndepentId );
      
        /*新建右结点，放下上面新建的结点*/
        CREATE_CHILD_POINT_MAIN( pcDataBufTmp, keyTmp, keyRight, iaIndepentId );
      
        /*将新建区中的两个结点写入*/
        WRITE_NEW_POINT_MAIN( pcDataBufTmp, key, keyTmp, llFirstDiscNew, iaIndepentId );
        
  	  	return SUCCESS;
      }
      /*有右结点*/
      else
      {
   	    /*取出满足条件的右结点*/
   	    GET_POINT_DATA_MAIN( keyRight, pcDataBuf, iStructOffset + 1 );

//logFmtAsy( LVNOR, "keyRight.llChildAddr[%lld]keyRight.llChildSize[%lld]", keyRight.llChildAddr, keyRight.llChildSize );
              
        /*如果右边的结点为空值*/
        if (keyRight.llChildSize == 0)
        {
        	CREATE_CHILD_POINT_MAIN( pcDataBufTmp, keyRight, keyMax, iaIndepentId );
          
          /*修改当前级数量+1*/
          sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain++;
          
          /*设置当前级区的数据*/
          memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &keyRight, sizeof(KEY_POINT) );
          setKeyBlockDataMain( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
      
          return SUCCESS;
        }
        /*如果右边的结点有数据，并且有足够的空间*/
        else
        if (keyRight.llChildSize < MAX_AERA_SIZE)
        {
        	/*取得兄弟下级索引区所有数据*/
        	GET_BLOCK_DATA_MAIN( pcDataBufRight, keyRight.llChildAddr, iaIndepentId );
                
          /*设置兄弟下级区的结点*/
          SET_BROTHER_POINT_MAIN( pcDataBufTmpRight, pcDataBufRight, keyRight, keyMax, iaIndepentId )
      
          /*设置当前级区中的下级区数量*/
          memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &keyRight, sizeof(KEY_POINT) );
          
          /*设置数据*/
          setKeyBlockDataMain( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
          
          return SUCCESS;
        }
        /*如果右边的结点已满，但当前级有多余的结点空间*/
        else
        if (keyRight.llChildSize == MAX_AERA_SIZE && sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain < MAX_AERA_SIZE)
        {
        	/*新建下级区，将刚才数据插入*/
        	CREATE_CHILD_POINT_MAIN( pcDataBufTmp, keyRight, keyMax, iaIndepentId );
          
          memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
          memcpy( pcDataBufTmp, pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), ((sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain) - iStructOffset - 1) * sizeof(KEY_POINT) );
          memcpy( pcDataBuf + (iStructOffset + 2) * sizeof(KEY_POINT), pcDataBufTmp, ((sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain) - iStructOffset - 1) * sizeof(KEY_POINT) );
                  
          /*设置新建下级区的结点*/
          memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &keyRight, sizeof(KEY_POINT) );
                                 
          sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain++;
                        
          /*设置一级区的数据*/
          setKeyBlockDataMain( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
          
          return SUCCESS;
        }
        /*右边的结点已满，并且当前级区没有多余的结点空间*/
        else
        {
          /*新建右结点，放下上面被挤出的最大值*/
          CREATE_CHILD_POINT_MAIN( pcDataBufTmp, keyRight, keyMax, iaIndepentId );
          
          /*取出当前的最大值*//*这里复用变量 keyMax */
          GET_MAX_KEY_MAIN( keyMax, pcDataBuf );
          
          /*设置数据*/
          memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
          memcpy( pcDataBufTmp, pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), ((sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain) - 1 - iStructOffset - 1) * sizeof(KEY_POINT) );
          memcpy( pcDataBuf + (iStructOffset + 2) * sizeof(KEY_POINT), pcDataBufTmp, ((sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain) - 1 - iStructOffset - 1) * sizeof(KEY_POINT) );
              
          memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &keyRight, sizeof(KEY_POINT) );
          
          /*设置数据*/
          setKeyBlockDataMain( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
          
          /*新建第一级*/
	        CREATE_NEW_FIRST_BLOCK_MAIN( llFirstDiscNew, pcDataBuf, key, keyMin, iaIndepentId );
	        
	        /*新建区用来装这个最大值*/
          CREATE_CHILD_POINT_MAIN( pcDataBufTmp, keyRight, keyMax, iaIndepentId );
  	  	  
  	  	  /*将新建区中的两个结点写入*/
  	  	  WRITE_NEW_POINT_MAIN( pcDataBufTmp, key, keyRight, llFirstDiscNew, iaIndepentId );
  	  	  
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
  	if (iaLoops == sgGlobalVariableMain[iaIndepentId].llgLoopsMain - 1)
  	{
  		/*取得第一级索引区所有数据*/
  		GET_BLOCK_DATA_MAIN( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentId );
      
      /*在当前级索引区中搜索满足条件的下级索引区结点*/
      iStructOffset = scanfKeyPointMain( pcDataBuf, saIndexPoint->llChildSize, pcaKey, llaKeyLen, iaIndepentId );
      
      /*取得当前下级结点*/
      GET_POINT_DATA_MAIN( key, pcDataBuf, iStructOffset );
      
      memset( &keyDataMax, 0x00, sizeof(keyDataMax) );
    
//logFmtAsy( LVNOR, "saIndexPoint->llChildAddr[%lld] iStructOffset[%d] key.llChildAddr[%lld] key.llChildSize[%lld]", saIndexPoint->llChildAddr, iStructOffset, key.llChildAddr, key.llChildSize );
      	
      /*执行最后一级*/
      insertIndexNodeLastLoopMain( &iReplaceFlag, &iOutFlag, &keyDataMax, &key, pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentId );
        
    	/*如果关键值或数量有变化*/
      if (iReplaceFlag == REPLACE_0 || iReplaceFlag == REPLACE_1 || iReplaceFlag == REPLACE_2)
      {
    	  /*设置数据*/
        memcpy( pcDataBuf + iStructOffset * sizeof(KEY_POINT), &key, sizeof(KEY_POINT) );

        /*设置一级区的数据*/
        setKeyBlockDataMain( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentId );
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
      		CREATE_CHILD_POINT_MAIN( pcDataBufTmp, keyRight, keyDataMax, iaIndepentId );
      		
      		/*将该结点挤出*/
      		memcpy( saOutIndexPoint, &keyRight, sizeof(KEY_POINT) );
      		*iaOutFlag = OUT;
      		
      		return SUCCESS;
	      }
	      /*有右结点*/
	      else
	      {
	      	/*取出满足条件的右结点*/
	      	GET_POINT_DATA_MAIN( keyRight, pcDataBuf, iStructOffset + 1 );
	      	
//logFmtAsy( LVNOR, "keyRight.llChildAddr[%lld]keyRight.llChildSize[%lld]*iaReplaceFlag[%d]", keyRight.llChildAddr, keyRight.llChildSize, *iaReplaceFlag );
          
          /*如果右边的结点为空值*/
          if (keyRight.llChildSize == 0)
          {
          	CREATE_CHILD_POINT_MAIN( pcDataBufTmp, keyRight, keyDataMax, iaIndepentId );
            
            /*修改当前级数量+1*/
            saIndexPoint->llChildSize++;
            
            /*设置当前级区的数据*/
            memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &keyRight, sizeof(KEY_POINT) );
            setKeyBlockDataMain( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentId );
            
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
          	GET_BLOCK_DATA_MAIN( pcDataBufRight, keyRight.llChildAddr, iaIndepentId );
                 
            /*设置兄弟下级区的结点*/
            SET_BROTHER_POINT_MAIN( pcDataBufTmpRight, pcDataBufRight, keyRight, keyDataMax, iaIndepentId );
                                      
            /*设置当前级区中的下级区数量*/
            memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &keyRight, sizeof(KEY_POINT) );
            setKeyBlockDataMain( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentId );
            
            *iaOutFlag = NOT_OUT;
            return SUCCESS;
          }
          /*如果右边的结点已满，但当前级有多余的结点空间*/
          else
          if (keyRight.llChildSize == MAX_AERA_SIZE && saIndexPoint->llChildSize < MAX_AERA_SIZE)
          {
          	CREATE_CHILD_POINT_MAIN( pcDataBufTmp, keyRight, keyDataMax, iaIndepentId );
            
            memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
            memcpy( pcDataBufTmp, pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), ((saIndexPoint->llChildSize) - iStructOffset - 1) * sizeof(KEY_POINT) );
            memcpy( pcDataBuf + (iStructOffset + 2) * sizeof(KEY_POINT), pcDataBufTmp, ((saIndexPoint->llChildSize) - iStructOffset - 1) * sizeof(KEY_POINT) );
                    
            /*设置新建下级区的结点*/
            memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &keyRight, sizeof(KEY_POINT) );
                                     
            saIndexPoint->llChildSize++;
                          
            /*设置当前级区的数据*/
            setKeyBlockDataMain( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentId );
            
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
          	CREATE_CHILD_POINT_MAIN( pcDataBufTmp, key, keyDataMax, iaIndepentId );
            
            /*取出当前的最大值*/
            GET_MAX_KEY_MAIN( keyMax, pcDataBuf );
            
            /*设置数据*/
            memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
            memcpy( pcDataBufTmp, pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), ((saIndexPoint->llChildSize) - 1 - iStructOffset - 1) * sizeof(KEY_POINT) );
            memcpy( pcDataBuf + (iStructOffset + 2) * sizeof(KEY_POINT), pcDataBufTmp, ((saIndexPoint->llChildSize) - 1 - iStructOffset - 1) * sizeof(KEY_POINT) );
                
            memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &key, sizeof(KEY_POINT) );
            
            /*设置数据*/
            setKeyBlockDataMain( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentId );
      		  
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
  	  GET_BLOCK_DATA_MAIN( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentId );
      
      /*在当前级索引区中搜索满足条件的下级索引区结点*/
      iStructOffset = scanfKeyPointMain( pcDataBuf, saIndexPoint->llChildSize, pcaKey, llaKeyLen, iaIndepentId );
      
      /*取得当前下级结点*/
      GET_POINT_DATA_MAIN( key, pcDataBuf, iStructOffset );
      
      memset( &keyMax, 0x00, sizeof(keyMax) );
      
      /*往下一级执行*/
      insertIndexNodeMain( &iReplaceFlag, &iOutFlag, &keyMax, &key, iaLoops + 1, pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentId );
      
      /*如果关键值或数量有变化*/
      if (iReplaceFlag == REPLACE_0 || iReplaceFlag == REPLACE_1 || iReplaceFlag == REPLACE_2)
      {
        /*设置数据*/
        memcpy( pcDataBuf + iStructOffset * sizeof(KEY_POINT), &key, sizeof(KEY_POINT) );
        
        /*设置数据*/
        setKeyBlockDataMain( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentId );
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
      		CREATE_CHILD_POINT_MAIN( pcDataBufTmp, keyRight, keyMax, iaIndepentId );
      		
      		/*将该结点挤出*/
      		memcpy( saOutIndexPoint, &keyRight, sizeof(KEY_POINT) );
      		*iaOutFlag = OUT;
      		
      		return SUCCESS;
	      }
	      /*有右结点*/
	      else
	      {
	      	/*取出满足条件的右结点*/
	      	GET_POINT_DATA_MAIN( keyRight, pcDataBuf, iStructOffset + 1 );
          
          /*如果右边的结点为空值*/
          if (keyRight.llChildSize == 0)
          {
          	CREATE_CHILD_POINT_MAIN( pcDataBufTmp, keyRight, keyMax, iaIndepentId );
            
            /*修改当前级数量+1*/
            saIndexPoint->llChildSize++;
            
            /*设置当前级区的数据*/
            memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &keyRight, sizeof(KEY_POINT) );
            setKeyBlockDataMain( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentId );
            
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
          	GET_BLOCK_DATA_MAIN( pcDataBufRight, keyRight.llChildAddr, iaIndepentId );
                  
            /*设置兄弟下级区的结点*/
            SET_BROTHER_POINT_MAIN( pcDataBufTmpRight, pcDataBufRight, keyRight, keyMax, iaIndepentId );
                                      
            /*设置当前级区中的下级区数量*/
            memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &keyRight, sizeof(KEY_POINT) );
            setKeyBlockDataMain( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentId );
            
            *iaOutFlag = NOT_OUT;
            return SUCCESS;
          }
          /*如果右边的结点已满，但当前级有多余的结点空间*/
          else
          if (keyRight.llChildSize == MAX_AERA_SIZE && saIndexPoint->llChildSize < MAX_AERA_SIZE )
          {
          	CREATE_CHILD_POINT_MAIN( pcDataBufTmp, keyRight, keyMax, iaIndepentId );
            
            memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
            memcpy( pcDataBufTmp, pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), ((saIndexPoint->llChildSize) - iStructOffset - 1) * sizeof(KEY_POINT) );
            memcpy( pcDataBuf + (iStructOffset + 2) * sizeof(KEY_POINT), pcDataBufTmp, ((saIndexPoint->llChildSize) - iStructOffset - 1) * sizeof(KEY_POINT) );
                    
            /*设置新建下级区的结点*/
            memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &keyRight, sizeof(KEY_POINT) );
                                     
            saIndexPoint->llChildSize++;
                          
            /*设置当前级区的数据*/
            setKeyBlockDataMain( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentId );
            
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
          	CREATE_CHILD_POINT_MAIN( pcDataBufTmp, key, keyMax, iaIndepentId );
            
            /*取出当前的最大值*/
            GET_MAX_KEY_MAIN( keyMax, pcDataBuf );
            
            /*设置数据*/
            memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
            memcpy( pcDataBufTmp, pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), ((saIndexPoint->llChildSize) - 1 - iStructOffset - 1) * sizeof(KEY_POINT) );
            memcpy( pcDataBuf + (iStructOffset + 2) * sizeof(KEY_POINT), pcDataBufTmp, ((saIndexPoint->llChildSize) - 1 - iStructOffset - 1) * sizeof(KEY_POINT) );
                
            memcpy( pcDataBuf + (iStructOffset + 1) * sizeof(KEY_POINT), &key, sizeof(KEY_POINT) );
            
            /*设置数据*/
            setKeyBlockDataMain( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentId );
      		  
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

