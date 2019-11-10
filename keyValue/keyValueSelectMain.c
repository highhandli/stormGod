#define __KEY_VALUE_SELECT_MAIN_C

#ifndef __COMMON_HEAD_H
  #include "commonHead.h"
#endif

int selectNodeMain( char *pcaKey, int iaKeyLen, char *pcaValue, int iaIndepentId );
int selectIndexNodeMain( char *pcaKey, int iaKeyLen, char *pcaValue, KEY_POINT *saIndexPoint, int iaLoops, int iaIndepentId );

/********************************************************
�������ƣ�selectNodeMain
�������ܣ����������������� 
��ڲ�����
��    һ: ����ֵ                 I
��    �أ�1��ʾ�ҵ���0��ʾû�ҵ� 
��    �ߣ������ 
����ʱ�䣺20160820
�޸�ʱ�䣺 
*********************************************************/
int selectNodeMain( char *pcaKey, int iaKeyLen, char *pcaValue, int iaIndepentId )
{
  KEY_POINT sIndexPoint;
  int iRet;
	
	/*��ȡ��*/
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
  
  /*�ͷ���*/
  SELECT_MAIN_UNLOCK(iaIndepentId);
          
  return iRet;
}

/********************************************************
�������ƣ�selectIndexNodeMain
�������ܣ����������������� 
��ڲ�����
��    һ: ����ֵ                 I
��    �أ�1��ʾ�ҵ���0��ʾû�ҵ� 
��    �ߣ������ 
����ʱ�䣺20160820
�޸�ʱ�䣺 
*********************************************************/
int selectIndexNodeMain( char *pcaKey, int iaKeyLen, char *pcaValue, KEY_POINT *saIndexPoint, int iaLoops, int iaIndepentId )
{
  int   iStructOffset;                                      /*�����еĽṹƫ����*/
  __int64  iLeft;
  __int64  iRight;
  __int64  iMiddle;
  int iRet;
  char  pcKey[MAX_KEY_LEN];
  KEY_POINT *key;                                           /*�����������е����ݽṹ*/
  KEY_VALUE_POINT *keyData;                                 /*��������е����ݽṹ*/
  char pcDataBuf[BLOCK_SIZE];                               /*�����е�����*/
  
  /*��������һ��*/
  if (iaLoops == sgGlobalVariableMain[iaIndepentId].llgLoopsMain)
  {
  	/*�������һ��*/
    {
      /*ȡ�����һ����������*/
      memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
      getKeyBlockDataMain( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentId );

      iLeft = 0;
      iRight = saIndexPoint->llChildSize - 1;
      iMiddle = 0;

      while (1)
      {
        iMiddle = (iLeft + iRight)>>1;
        keyData = (KEY_VALUE_POINT*)(pcDataBuf + iMiddle * sizeof(KEY_VALUE_POINT));
        
        /*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
        valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
        pcKey[keyData->llKeyLen] = 0x00; 

        iRet = strcmp( pcaKey, pcKey );
        if (iRet == 0)
        {
          /*�� VALUE �ļ��л�ȡ VALUE ��ֵ*/
          valueToBufMain( pcaValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentId );
          return SUCCESS;
        }
        /*Ŀ��ֵ����ֵ�ұ�*/
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
        /*Ŀ��ֵ����ֵ���*/
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
  	/*������������*/
    {
      /*ȡ����������������*/
      memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
      getKeyBlockDataMain( pcDataBuf, saIndexPoint->llChildAddr, iaIndepentId );
    
      /*�������������������������������¼�������*/
      {  
        /*�������������Ľ��*/
        iStructOffset = scanfKeyPointMain( pcDataBuf, saIndexPoint->llChildSize, pcaKey, iaKeyLen, iaIndepentId );    
    
        /*ȡ�õ�ǰ��󼶽��*/
        key = (KEY_POINT *)(pcDataBuf + iStructOffset * sizeof(KEY_POINT));
      }
    }
    return selectIndexNodeMain( pcaKey, iaKeyLen, pcaValue, key, iaLoops + 1, iaIndepentId );
  }
}
