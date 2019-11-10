#define __KEY_VALUE_UPDATE_C


#ifndef __COMMON_HEAD_H
  #include "commonHead.h"
#endif

int updateNode( char *pcaKey, __int64 llaKeyLen, char *pcaValue, __int64 llaValueLen, int iaIndepentFileId );
int updateIndexNode( char *pcaKey, __int64 llaKeyLen, char *pcaValue, __int64 llaValueLen, KEY_POINT saIndexPoint, int iaLoops, int iaIndepentFileId );


/********************************************************
函数名称：updateNode
函数功能：搜索整型索引数据 
入口参数：
第    一: 索引值                 I
返    回：1表示找到，0表示没找到 
作    者：李高文 
创建时间：20160820
修改时间： 
*********************************************************/
int updateNode( char *pcaKey, __int64 llaKeyLen, char *pcaValue, __int64 llaValueLen, int iaIndepentFileId )
{
  KEY_POINT sIndexPoint;
  int iRet;
  
  /*获取锁*/
  UPDATE_DELETE_LOCK(iaIndepentFileId);
          
  memset( &sIndexPoint, 0x00, sizeof(sIndexPoint) );
  sIndexPoint.llChildAddr = sgBase[iaIndepentFileId].llFirstDisc;
  sIndexPoint.llChildSize = sgBase[iaIndepentFileId].llFirstSize;

  if (sIndexPoint.llChildSize == 0)
  {
  	iRet = insertNode( pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentFileId );
  }
  else
  {
    iRet = updateIndexNode( pcaKey, llaKeyLen, pcaValue, llaValueLen, sIndexPoint, 1, iaIndepentFileId );
  }
  
  /*释放锁*/
  UPDATE_DELETE_UNLOCK(iaIndepentFileId);
          
  return iRet;
}

/********************************************************
函数名称：updateIndexNode
函数功能：搜索整型索引数据 
入口参数：
第    一: 索引值                 I
返    回：1表示找到，0表示没找到 
作    者：李高文 
创建时间：20160820
修改时间： 
*********************************************************/
int updateIndexNode( char *pcaKey, __int64 llaKeyLen, char *pcaValue, __int64 llaValueLen, KEY_POINT saIndexPoint, int iaLoops, int iaIndepentFileId )
{
  int  iStructOffset;                             /*级区中的结构偏移量*/
  __int64  iLeft;
  __int64  iRight;
  __int64  iTmp;
  __int64  iMiddle;
  int iRet;
  char  pcKey[MAX_KEY_LEN];
  KEY_POINT key;                                  /*其他级数据中的数据结构*/
  KEY_VALUE_POINT keyData;                        /*最后级数据中的数据结构*/
  char pcDataBuf[BLOCK_SIZE];                     /*级区中的数据*/
 	
  /*如果是最后一级*/
  if (iaLoops == sgBase[iaIndepentFileId].llLoops)
  {
  	/*搜索最后一级*/
    {
      /*取得最后一级所有数据*/
      memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
      getKeyBlockData( pcDataBuf, saIndexPoint.llChildAddr, iaIndepentFileId );

      iLeft = 0;
      iRight = saIndexPoint.llChildSize - 1;
      iMiddle = 0;

      while (1)
      {
        iMiddle = (iLeft + iRight)>>1;
        
        memset( &keyData, 0x00, sizeof(keyData) );        
        memcpy( &keyData, pcDataBuf + iMiddle * sizeof(KEY_VALUE_POINT), sizeof(KEY_VALUE_POINT) );
        
        /*从 VALUE 文件中获取 KEY 的值*/
        valueToBuf( pcKey, keyData.llKeyAddr, keyData.llKeyLen, iaIndepentFileId );
        pcKey[keyData.llKeyLen] = 0x00;
        
        iRet = strcmp( pcaKey, pcKey );

        if (iRet == 0)
        {
          REPLACE_VALUE_BUF_TO_VALUE( keyData, pcaValue, llaValueLen, iaIndepentFileId )
          SET_KEY_DATA( saIndexPoint.llChildAddr, iMiddle * sizeof(KEY_VALUE_POINT), &keyData, sizeof(KEY_VALUE_POINT), iaIndepentFileId );
          
          return SUCCESS;
        }
        
        /*目标值在中值右边*/
        if (iRet > 0)
        {
        	if (iMiddle == iRight)
        	{
        		iRet = insertNode( pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentFileId );
          
            return iRet;
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
            iRet = insertNode( pcaKey, llaKeyLen, pcaValue, llaValueLen, iaIndepentFileId );
          
            return iRet;
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
    }
  }
  else
  {
  	/*搜索其他级区*/
    {
      /*取得其他索引区所有数据*/
      memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
      getKeyBlockData( pcDataBuf, saIndexPoint.llChildAddr, iaIndepentFileId );
    
      /*在其他级索引区中搜索满足条件的下级索引区*/
      {  
        /*搜索满足条件的结点*/
        iStructOffset = scanfKeyPoint( pcDataBuf, saIndexPoint.llChildSize, pcaKey, llaKeyLen, iaIndepentFileId );    
    
        /*取得当前最后级结点*/
        memset( &key, 0x00, sizeof(key) );
        memcpy( &key, pcDataBuf + iStructOffset * sizeof(KEY_POINT), sizeof(KEY_POINT) );
      }
    }
    return updateIndexNode( pcaKey, llaKeyLen, pcaValue, llaValueLen, key, iaLoops + 1, iaIndepentFileId );
  }
}
