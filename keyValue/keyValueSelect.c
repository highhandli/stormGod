#define __KEY_VALUE_SELECT_C

#ifndef __COMMON_HEAD_H
  #include "commonHead.h"
#endif

int selectNode( char *pcaKey, int iaKeyLen, char *pcaValue, int iaIndepentFileId );
int selectIndexNode( char *pcaKey, int iaKeyLen, char *pcaValue, KEY_POINT *saIndexPoint, int iaLoops, int iaIndepentFileId );

/********************************************************
函数名称：selectNode
函数功能：搜索整型索引数据 
入口参数：
第    一: 索引值                 I
返    回：1表示找到，0表示没找到 
作    者：李高文 
创建时间：20160820
修改时间： 
*********************************************************/
int selectNode( char *pcaKey, int iaKeyLen, char *pcaValue, int iaIndepentFileId )
{
  KEY_POINT sIndexPoint;
  int iRet;
	
	/*获取锁*/
  SELECT_LOCK(iaIndepentFileId);
  
  if (sgBase[iaIndepentFileId].llFirstSize == 0)
  {
    iRet = NOT_FOUND;
  }
  else
  {
    memset( &sIndexPoint, 0x00, sizeof(sIndexPoint) );
    sIndexPoint.llChildAddr = sgBase[iaIndepentFileId].llFirstDisc;
    sIndexPoint.llChildSize = sgBase[iaIndepentFileId].llFirstSize;
    
    iRet = selectIndexNode( pcaKey, iaKeyLen, pcaValue, &sIndexPoint, 1, iaIndepentFileId );
  }
  
  /*释放锁*/
  SELECT_UNLOCK(iaIndepentFileId);
          
  return iRet;
}

/********************************************************
函数名称：selectIndexNode
函数功能：搜索整型索引数据 
入口参数：
第    一: 索引值                 I
返    回：1表示找到，0表示没找到 
作    者：李高文 
创建时间：20160820
修改时间： 
*********************************************************/
int selectIndexNode( char *pcaKey, int iaKeyLen, char *pcaValue, KEY_POINT *saIndexPoint, int iaLoops, int iaIndepentFileId )
{
  int   iStructOffset;                                      /*级区中的结构偏移量*/
  __int64  iLeft;
  __int64  iRight;
  __int64  iMiddle;
  char  pcKey[MAX_KEY_LEN];
  KEY_POINT *key;                                           /*其他级数据中的数据结构*/
  KEY_VALUE_POINT *keyData;                                 /*最后级数据中的数据结构*/
  char pcDataBuf[BLOCK_SIZE];                               /*级区中的数据*/
  
  /*如果是最后一级*/
  if (iaLoops == sgBase[iaIndepentFileId].llLoops)
  {
  	/*搜索最后一级*/
    {
      /*取得最后一级所有数据*/
      memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
      getKeyBlockData( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentFileId );

      iLeft = 0;
      iRight = saIndexPoint->llChildSize - 1;
      iMiddle = 0;

      while (1)
      {
        iMiddle = (iLeft + iRight)>>1;
        keyData = (KEY_VALUE_POINT*)(pcDataBuf + iMiddle * sizeof(KEY_VALUE_POINT));
        
        /*从 VALUE 文件中获取 KEY 的值*/
        valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
        pcKey[keyData->llKeyLen] = 0x00; 

        if (!strcmp( pcaKey, pcKey ))
        {
          /*从 VALUE 文件中获取 VALUE 的值*/
          valueToBuf( pcaValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentFileId );
          return SUCCESS;
        }
        
        /*目标值在中值右边*/
        if (strcmp( pcaKey , pcKey ) > 0)
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
    }

    return NOT_FOUND;
  }
  else
  {
  	/*搜索其他级区*/
    {
      /*取得索引区所有数据*/
      memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
      getKeyBlockData( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentFileId );
    
      /*在其他级索引区中搜索满足条件的下级索引区*/
      {  
        /*搜索满足条件的结点*/
        iStructOffset = scanfKeyPoint( pcDataBuf, saIndexPoint->llChildSize, pcaKey, iaKeyLen, iaIndepentFileId );    
    
        /*取得当前最后级结点*/
        key = (KEY_POINT *)(pcDataBuf + iStructOffset * sizeof(KEY_POINT));
      }
    }
    return selectIndexNode( pcaKey, iaKeyLen, pcaValue, key, iaLoops + 1, iaIndepentFileId );
  }
}
