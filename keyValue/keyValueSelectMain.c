#define __KEY_VALUE_SELECT_MAIN_C

#ifndef __COMMON_HEAD_H
  #include "commonHead.h"
#endif

int selectNodeMain( char *pcaKey, int iaKeyLen, char *pcaValue, int iaIndepentId );
int selectIndexNodeMain( char *pcaKey, int iaKeyLen, char *pcaValue, KEY_POINT *saIndexPoint, int iaLoops, int iaIndepentId );

/********************************************************
函数名称：selectNodeMain
函数功能：搜索整型索引数据 
入口参数：
第    一: 索引值                 I
返    回：1表示找到，0表示没找到 
作    者：李高文 
创建时间：20160820
修改时间： 
*********************************************************/
int selectNodeMain( char *pcaKey, int iaKeyLen, char *pcaValue, int iaIndepentId )
{
  KEY_POINT sIndexPoint;
  int iRet;
	
	/*获取锁*/
  SELECT_MAIN_LOCK(iaIndepentId);
  
  if (sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain == 0)
  {
    iRet = NOT_FOUND;
  }
  else
  {
    memset( &sIndexPoint, 0x00, sizeof(sIndexPoint) );
    sIndexPoint.llChildAddr = sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain;
    sIndexPoint.llChildSize = sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain;
    
    iRet = selectIndexNodeMain( pcaKey, iaKeyLen, pcaValue, &sIndexPoint, 1, iaIndepentId );
  }
  
  /*释放锁*/
  SELECT_MAIN_UNLOCK(iaIndepentId);
          
  return iRet;
}

/********************************************************
函数名称：selectIndexNodeMain
函数功能：搜索整型索引数据 
入口参数：
第    一: 索引值                 I
返    回：1表示找到，0表示没找到 
作    者：李高文 
创建时间：20160820
修改时间： 
*********************************************************/
int selectIndexNodeMain( char *pcaKey, int iaKeyLen, char *pcaValue, KEY_POINT *saIndexPoint, int iaLoops, int iaIndepentId )
{
  int   iStructOffset;                                      /*级区中的结构偏移量*/
  __int64  iLeft;
  __int64  iRight;
  __int64  iMiddle;
  int iRet;
  char  pcKey[MAX_KEY_LEN];
  KEY_POINT *key;                                           /*其他级数据中的数据结构*/
  KEY_VALUE_POINT *keyData;                                 /*最后级数据中的数据结构*/
  char pcDataBuf[BLOCK_SIZE];                               /*级区中的数据*/
  
  /*如果是最后一级*/
  if (iaLoops == sgGlobalVariableMain[iaIndepentId].llgLoopsMain)
  {
  	/*搜索最后一级*/
    {
      /*取得最后一级所有数据*/
      memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
      getKeyBlockDataMain( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentId );

      iLeft = 0;
      iRight = saIndexPoint->llChildSize - 1;
      iMiddle = 0;

      while (1)
      {
        iMiddle = (iLeft + iRight)>>1;
        keyData = (KEY_VALUE_POINT*)(pcDataBuf + iMiddle * sizeof(KEY_VALUE_POINT));
        
        /*从 VALUE 文件中获取 KEY 的值*/
        valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
        pcKey[keyData->llKeyLen] = 0x00; 

        iRet = strcmp( pcaKey, pcKey );
        if (iRet == 0)
        {
          /*从 VALUE 文件中获取 VALUE 的值*/
          valueToBufMain( pcaValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentId );
          return SUCCESS;
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
    }

    return NOT_FOUND;
  }
  else
  {
  	/*搜索其他级区*/
    {
      /*取得索引区所有数据*/
      memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
      getKeyBlockDataMain( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentId );
    
      /*在其他级索引区中搜索满足条件的下级索引区*/
      {  
        /*搜索满足条件的结点*/
        iStructOffset = scanfKeyPointMain( pcDataBuf, saIndexPoint->llChildSize, pcaKey, iaKeyLen, iaIndepentId );    
    
        /*取得当前最后级结点*/
        key = (KEY_POINT *)(pcDataBuf + iStructOffset * sizeof(KEY_POINT));
      }
    }
    return selectIndexNodeMain( pcaKey, iaKeyLen, pcaValue, key, iaLoops + 1, iaIndepentId );
  }
}
