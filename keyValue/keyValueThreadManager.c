#define __KEY_VALUE_THREAD_MANAGER_C

/*
������ MANAGER ���
*/

#ifndef __COMMON_HEAD_H
  #include "commonHead.h"
#endif

#ifndef __KEY_VALUE_THREAD_MANAGER_DELETE_C
  #include "keyValueThreadManagerDelete.c"
#endif

#ifndef __KEY_VALUE_THREAD_MANAGER_EXIT_FLUSH_ALTER_C
  #include "keyValueThreadManagerExitFlushAlter.c"
#endif

#ifndef __KEY_VALUE_THREAD_MANAGER_SELECT_COUNT_C
  #include "keyValueThreadManagerSelectCount.c"
#endif

#ifndef __KEY_VALUE_THREAD_MANAGER_SELECT_KEY_VALUE_C
  #include "keyValueThreadManagerSelectKeyValue.c"
#endif

#ifndef __KEY_VALUE_THREAD_MANAGER_SELECT_MAX_MIN_C
  #include "keyValueThreadManagerSelectMaxMin.c"
#endif

#ifndef __KEY_VALUE_THREAD_MANAGER_SHOW_C
  #include "keyValueThreadManagerShow.c"
#endif

#ifndef __KEY_VALUE_THREAD_MANAGER_UNLOAD_LOAD_C
  #include "keyValueThreadManagerUnloadLoad.c"
#endif

void managerSocket( );
void managerDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd );
int  managerPositionLeftOpen( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId );
int  managerPositionLeftClose( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId );
int  managerPositionRightOpen( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId );
int  managerPositionRightClose( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId );
int  managerPositionLeftOpenMain( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId );
int  managerPositionLeftCloseMain( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId );
int  managerPositionRightOpenMain( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId );
int  managerPositionRightCloseMain( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId );


/**********************************************************************
��������: managerSocket
��������: �����̴߳�����
��    ����
��    һ��SOCKET         I
��    ��: �����
����ʱ��: 20161018
**********************************************************************/
void managerSocket( )
{
	struct sockaddr_in server;   /*��������ַ��Ϣ�ṹ��*/
  struct sockaddr_in addr;     /*���յ��Ŀͻ�����Ϣ�ṹ��*/
  int sin_size;                /*��ַ��Ϣ�ṹ���С*/
  int iSocket;
  int iNewSocket;
  
	char pcRecvLen[DATA_BUF_LEN + 1];
  int iRecvLen;
	int iTotalLen;
	int iDataLen;
	int iRet;
	char *pcRecvBuf;
	int iSize = 0;

  iSocket = sgManagerThread.iSocket;
  sin_size = sizeof(struct sockaddr);
  
  
  while (sgManagerThread.iRunState == THREAD_RUN_STATE_RUN)
  {
  	/*ACCPET ����ִ��*/
  	sgManagerThread.iAcceptState = ACCEPT_STATE_YES;
  	
    /*����accept�����ط�������ͻ������ӵ�socket������*/
    iNewSocket = accept(iSocket, (struct sockaddr *)&addr, &sin_size);
    if (iNewSocket == -1)
    {
      continue;   
    }
    
    /*ACCPET ִ�����*/
    sgManagerThread.iAcceptState = ACCEPT_STATE_NO;

	  /*����10B����*/
	  iTotalLen = 0;
	  iRecvLen = 0;
	  memset( pcRecvLen, 0x00, sizeof(pcRecvLen) );
	  	
	  while (sgManagerThread.iRunState == THREAD_RUN_STATE_RUN)
	  {
	  	iRecvLen = recv( iNewSocket, pcRecvLen + iTotalLen, DATA_BUF_LEN - iTotalLen, 0 );
	  	if (iRecvLen <= 0)
	  	{
	  		closesocket(iNewSocket);
	  		break;
	  	}
	  	iTotalLen += iRecvLen;
	  	if (iTotalLen == DATA_BUF_LEN)
	  	{
	  		break;
	  	}
	  }
	  if (sgManagerThread.iRunState == THREAD_RUN_STATE_STOP)
	  {
	  	break;
	  }
	  if (iTotalLen != DATA_BUF_LEN)
	  {
	  	closesocket(iNewSocket);
	  	continue;
	  }
	  	  
	  /*���㳤��*/
	  iDataLen = atoi( pcRecvLen );
	  if (iDataLen < OPERATE_LEN)
	  {
	  	closesocket(iNewSocket);
	  	continue;
	  }
	  
	  /*�����ڴ�*/
	  pcRecvBuf = malloc( iDataLen + 1 );
	  if (pcRecvBuf == NULL)
	  {
	  	closesocket(iNewSocket);
	  	continue;
	  }
	  memset( pcRecvBuf, 0x00, iDataLen + 1 );
	  
	  /*��������*/
	  iTotalLen = 0;
	  iRecvLen = 0;
	 
	  while (igConnectionErrFlag == SUCCESS)
	  {
	  	iRecvLen = recv( iNewSocket, pcRecvBuf + iTotalLen, iDataLen - iTotalLen, 0 );
	  	if (iRecvLen <= 0)
	  	{
	  		break;
	  	}
	  		  	
	  	iTotalLen += iRecvLen;
	  	if (iTotalLen == iDataLen)
	  	{
	  		break;
	  	}
	  }
	  if (sgManagerThread.iRunState == THREAD_RUN_STATE_STOP)
	  {
	  	free(pcRecvBuf);
	  	break;
	  }
	  if (iTotalLen != iDataLen)
	  {
	  	closesocket(iNewSocket);
	  	free(pcRecvBuf);
	  	continue;
	  }
	  
	  managerDeal( iNewSocket, pcRecvBuf, pcRecvBuf + iTotalLen );

	  closesocket(iNewSocket);
	  free( pcRecvBuf );	  
  }
}

/**********************************************************************
��������: managerDeal
��������: �������˽��յ�������
��    ����
��    һ��SOCKET               I
��    �������ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20161018
**********************************************************************/
void managerDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd )
{
#define MANAGER_DEAL_SEND_RESULT( value ) memset( pcResult, 0x00, sizeof(pcResult) );sprintf( pcResult, "%d", value ); cResult = pcResult[0];send( iaSocket, &cResult, 1, 0 );
	char pcResult[INT_LEN + 1];
	char pcOperate[OPERATE_LEN + 1];
	char cResult;
	char *pcBufStart;
	char *pcBufEnd;
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcBufEnd;
	
	/*ȡ��������*/
	memset( pcOperate, 0x00, sizeof(pcOperate) );
	memcpy( pcOperate, pcBufStart, OPERATE_LEN );
	pcBufStart += OPERATE_LEN;
	
	if (!strcmp( pcOperate, MANAGER_SELECT_KEY_CNT ) ||
	    !strcmp( pcOperate, MANAGER_SELECT_VALUE_CNT )||
	    !strcmp( pcOperate, MANAGER_SELECT_KEY_VALUE_CNT ))
	{
		managerSelectKeyValueDeal( iaSocket, pcOperate, pcBufStart, pcBufEnd );
	}
	else
	if (!strcmp( pcOperate, MANAGER_SELECT_MIN_CNT ))
	{
		managerSelectMinDeal( iaSocket );
	}
	else
	if (!strcmp( pcOperate, MANAGER_SELECT_MAX_CNT ))
	{
		managerSelectMaxDeal( iaSocket );
	}
	else
	if (!strcmp( pcOperate, MANAGER_SELECT_COUNT_CNT ))
	{
		managerSelectCountDeal( iaSocket, pcBufStart, pcBufEnd );
	}
	else
	if (!strcmp( pcOperate, MANAGER_DELETE_CNT ))
	{
		managerDeleteDeal( iaSocket, pcBufStart, pcBufEnd );
	}
	else
	if (!strcmp( pcOperate, MANAGER_UNLOAD_CNT ))
	{
		managerUnloadDeal( iaSocket, pcBufStart, pcBufEnd );
	}
	else
	if (!strcmp( pcOperate, MANAGER_LOAD_CNT ))
	{
		managerLoadDeal( iaSocket, pcBufStart, pcBufEnd );
	}
	else
	if (!strcmp( pcOperate, MANAGER_EXIT_CNT ))
	{
		managerExitDeal( iaSocket, pcBufStart, pcBufEnd );
	}
	else
	if (!strcmp( pcOperate, MANAGER_FLUSH_CNT ))
	{
		managerFlushDeal( iaSocket, pcBufStart, pcBufEnd );
	}
	else
	if (!strcmp( pcOperate, MANAGER_ALTER_DISCS_CNT ))
	{
		managerAlterDiscsDeal( iaSocket, pcBufStart, pcBufEnd );
	}
	else
	if (!strcmp( pcOperate, MANAGER_ALTER_UPPERS_CNT ))
	{
		managerAlterUppersDeal( iaSocket, pcBufStart, pcBufEnd );
	}
	else
	if (!strcmp( pcOperate, MANAGER_SHOW_RAMS_CNT ))
	{
		managerShowRamsDeal( iaSocket );
	}
	else
	if (!strcmp( pcOperate, MANAGER_SHOW_DISCS_CNT ))
	{
		managerShowDiscsDeal( iaSocket );
	}
	else
	if (!strcmp( pcOperate, MANAGER_SHOW_THREADS_CNT ))
	{
		managerShowThreadsDeal( iaSocket );
	}
	else
	if (!strcmp( pcOperate, MANAGER_SHOW_KEYS_CNT ))
	{
		managerShowKeysDeal( iaSocket );
	}
	else
	{
		MANAGER_DEAL_SEND_RESULT( 1 );
	  return;
	}
	
	return;
}

/**********************************************************************
��������: managerPositionLeftOpen
��������: ����˴����еĶ�λ��ʼλ�ã�������
��    ����
��    һ�����λ�ñ���     O
��    ����KEY              I
��    ������ַ             I
��    �ģ�����             I
��    �壺����             I
��    �����ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161105
**********************************************************************/
int managerPositionLeftOpen( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId )
{
	char  pcKey[MAX_KEY_LEN];
	KEY_POINT *key;                /*�����������е����ݽṹ*/
  KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
  char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
  int  iIndex;
  int  iTmp;
  int  iRet;
    
  /*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
		if (iaAddr == 0)
		{
			return NOT_FOUND;
		}
		
		/*ȡ�����һ����������*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
    for (iIndex = 0; iIndex < iaSize; iIndex++)
    {
    	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iIndex * sizeof(KEY_VALUE_POINT));
    	
    	/*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
      valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
      pcKey[keyData->llKeyLen] = 0x00;
      
      /*�����Ŀ��ֵ�󣬽���*/
      if (strcmp( pcKey, pcaKey ) > 0)
      {
      	break;
      }
    }
    
    /*�����*/
    if (iIndex < iaSize)
    {
    	/*���õ�ǰ���Ľ��*/
    	iaOutResult[iaLoop] = iIndex;
    	
    	return SUCCESS;
    }
    else
    {
    	return NOT_FOUND;
    }
	}
  /*�������һ��*/
	else
	{
		if (iaAddr == 0)
		{
			return NOT_FOUND;
		}
		/*ȡ����������������*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
    /*������һ����Ŀ��ֵ�������λ��*/
    for (iIndex = 0; iIndex < iaSize; iIndex++)
    {
    	key = (KEY_POINT*)(pcDataBuf + iIndex * sizeof(KEY_POINT));
    	
    	/*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
      valueToBuf( pcKey, key->llKeyAddr, key->llKeyLen, iaIndepentFileId );
      pcKey[key->llKeyLen] = 0x00;
      
      /*�����Ŀ��ֵ�󣬽���*/
      if (strcmp( pcKey, pcaKey ) > 0)
      {
      	break;
      }
    }
    
    /*�����һ��λ�þ��ǣ����õ�ǰ������λ�ã��Լ����漶����λ��*/
    if (iIndex == 0 && strcmp( pcKey, pcaKey ) == 0)
    {
    	/*���õ�ǰ���Ľ��*/
    	iaOutResult[iaLoop] = iIndex;
    	
    	/*����ʣ�¼��Ľ��*/
    	for (iTmp = iaLoop + 1; iTmp <= iaTotalLoops; iTmp++)
    	{
    		iaOutResult[iTmp] = 0;
    	}
    	
    	return SUCCESS;
    }
    /*���ǵ�һ��λ��*/
    else
    {
    	/*��������ڣ���ǰ key ������������*/
    	if (iIndex == iaSize)
    	{
    		/*���õ�ǰ���Ľ��*/
    	  iaOutResult[iaLoop] = iIndex - 1;
    	  
    	  /*ָ��ǰֵ*/
    		key = (KEY_POINT*)(pcDataBuf + (iIndex - 1) * sizeof(KEY_POINT));
    		
    		/*������һ��*/
    		iRet = managerPositionLeftOpen( iaOutResult, pcaKey, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
    		
    		/*����ҵ�����������*/
    		if (iRet == SUCCESS)
    		{
    			return SUCCESS;
    		}
    		/*����¼�����û���ҵ�*/
    		else
    		/*if (iRet == NOT_FOUND)*/
    		{
    			return NOT_FOUND;
    		}
    	}
    	/*�������*/
    	else
    	{
    		/*���õ�ǰ���Ľ��*/
    	  iaOutResult[iaLoop] = iIndex - 1;
    		
    		/*ָ��ǰֵ*/
    		key = (KEY_POINT*)(pcDataBuf + (iIndex - 1) * sizeof(KEY_POINT));
    		
    		/*������һ��*/
    		iRet = managerPositionLeftOpen( iaOutResult, pcaKey, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
    		
    		/*����ҵ�����������*/
    		if (iRet == SUCCESS)
    		{
    			return SUCCESS;
    		}
    		else
    		/*if (iRet == NOT_FOUND)*/
    		{
    			/*�������õ�ǰ���Ľ��*/
    	    iaOutResult[iaLoop] = iIndex;
    	    
    	    /*����ʣ�¼��Ľ��*/
        	for (iTmp = iaLoop + 1; iTmp <= iaTotalLoops; iTmp++)
        	{
    		    iaOutResult[iTmp] = 0;
    	    }
    	  
    			return SUCCESS;
    		}
    	}
    }
	}
}

/**********************************************************************
��������: managerPositionLeftClose
��������: ����˴����еĶ�λ��ʼλ�ã��������
��    ����
��    һ�����λ�ñ���     O
��    ����KEY              I
��    ������ַ             I
��    �ģ�����             I
��    �壺����             I
��    �����ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161105
**********************************************************************/
int managerPositionLeftClose( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId )
{
	char  pcKey[MAX_KEY_LEN];
	KEY_POINT *key;                /*�����������е����ݽṹ*/
  KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
  char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
  int  iIndex;
  int  iTmp;
  int  iRet;
  
  if (iaSize == 0)
  {
		return NOT_FOUND;
	}
  /*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
		/*ȡ�����һ����������*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );

    for (iIndex = 0; iIndex < iaSize; iIndex++)
    {
    	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iIndex * sizeof(KEY_VALUE_POINT));
    	
    	/*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
      valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
      pcKey[keyData->llKeyLen] = 0x00;
      
      /*�����Ŀ��ֵ�󣬽���*/
      if (strcmp( pcKey, pcaKey ) >= 0)
      {
      	break;
      }
    }

    /*�����*/
    if (iIndex < iaSize)
    {
    	/*���õ�ǰ���Ľ��*/
    	iaOutResult[iaLoop] = iIndex;
    	
    	return SUCCESS;
    }
    else
    {
    	return NOT_FOUND;
    }
	}
  /*�������һ��*/
	else
	{
		if (iaAddr == 0)
		{
			return NOT_FOUND;
		}
		/*ȡ����������������*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
    /*������һ����Ŀ��ֵ�������λ��*/
    for (iIndex = 0; iIndex < iaSize; iIndex++)
    {
    	key = (KEY_POINT*)(pcDataBuf + iIndex * sizeof(KEY_POINT));
    	
    	/*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
      valueToBuf( pcKey, key->llKeyAddr, key->llKeyLen, iaIndepentFileId );
      pcKey[key->llKeyLen] = 0x00;
            
      /*�����Ŀ��ֵ�󣬽���*/
      if (strcmp( pcKey, pcaKey ) >= 0)
      {
      	break;
      }
    }

    /*�����һ��λ�þ��ǣ����߸�ֵ�պ���Ŀ��ֵ��ȣ����õ�ǰ������λ�ã��Լ����漶����λ��*/
    if (iIndex == 0 && strcmp( pcKey, pcaKey ) >= 0)/*������󿪵��㷨��ͬ*/
    {
    	/*���õ�ǰ���Ľ��*/
    	iaOutResult[iaLoop] = iIndex;

    	/*����ʣ�¼��Ľ��*/
    	for (iTmp = iaLoop + 1; iTmp <= iaTotalLoops; iTmp++)
    	{
    		iaOutResult[iTmp] = 0;
    	}

    	return SUCCESS;
    }
    /*���ǵ�һ��λ��*/
    else
    {
    	/*��������ڣ���ǰ key ������������*/
    	if (iIndex == iaSize)
    	{
    		/*���õ�ǰ���Ľ��*/
    	  iaOutResult[iaLoop] = iIndex - 1;
    	  
    	  /*ָ��ǰֵ*/
    		key = (KEY_POINT*)(pcDataBuf + (iIndex - 1) * sizeof(KEY_POINT));
    		
    		/*������һ��*/
    		iRet = managerPositionLeftClose( iaOutResult, pcaKey, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
    		
    		/*����ҵ�����������*/
    		if (iRet == SUCCESS)
    		{
    			return SUCCESS;
    		}
    		/*����¼�����û���ҵ�*/
    		else
    		/*if (iRet == NOT_FOUND)*/
    		{
    			return NOT_FOUND;
    		}
    	}
    	/*�������*/
    	else
    	{
    		/*���õ�ǰ����ǰһ��ֵ*/
    	  iaOutResult[iaLoop] = iIndex - 1;
    		
    		/*ָ��ǰֵ*/
    		key = (KEY_POINT*)(pcDataBuf + (iIndex - 1) * sizeof(KEY_POINT));
    		
    		/*������һ��*/
    		iRet = managerPositionLeftClose( iaOutResult, pcaKey, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
    		
    		/*����ҵ�����������*/
    		if (iRet == SUCCESS)
    		{
    			return SUCCESS;
    		}
    		else
    		/*if (iRet == NOT_FOUND)*/
    		{
    			/*�������õ�ǰ���Ľ��*/
    	    iaOutResult[iaLoop] = iIndex;
    	    
    	    /*����ʣ�¼��Ľ��*/
        	for (iTmp = iaLoop + 1; iTmp <= iaTotalLoops; iTmp++)
        	{
    		    iaOutResult[iTmp] = 0;
    	    }
    	  
    			return SUCCESS;
    		}
    	}
    }
	}
}

/**********************************************************************
��������: managerPositionRightOpen
��������: ����˴����еĶ�λ��ʼλ�ã��ҿ�����
��    ����
��    һ�����λ�ñ���     O
��    ����KEY              I
��    ������ַ             I
��    �ģ�����             I
��    �壺����             I
��    �����ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161106
**********************************************************************/
int managerPositionRightOpen( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId )
{
	char  pcKey[MAX_KEY_LEN];
	KEY_POINT *key;                /*�����������е����ݽṹ*/
  KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
  char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
  int  iIndex;
  int  iTmp;
  int  iRet;
  
  if (iaSize == 0)
  {
		return NOT_FOUND;
	}
	
  /*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
		/*ȡ�����һ����������*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
    for (iIndex = iaSize - 1; iIndex >= 0; iIndex--)
    {
    	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iIndex * sizeof(KEY_VALUE_POINT));
    	
    	/*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
      valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
      pcKey[keyData->llKeyLen] = 0x00;
      
      /*�����Ŀ��ֵС������*/
      if (strcmp( pcKey, pcaKey ) < 0)
      {
      	break;
      }
    }
    
    /*���û���ҵ�*/
    if (iIndex == -1)
    {
    	return NOT_FOUND;
    }
    else
    {
    	/*���õ�ǰ���Ľ��*/
    	iaOutResult[iaLoop] = iIndex;
    	
    	return SUCCESS;
    }
	}
  /*�������һ��*/
	else
	{
		if (iaAddr == 0)
		{
			return NOT_FOUND;
		}
		
		/*ȡ����������������*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
    /*������һ����Ŀ��ֵС������λ��*/
    for (iIndex = iaSize - 1; iIndex >= 0; iIndex--)
    {
    	key = (KEY_POINT*)(pcDataBuf + iIndex * sizeof(KEY_POINT));
    	
    	/*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
      valueToBuf( pcKey, key->llKeyAddr, key->llKeyLen, iaIndepentFileId );
      pcKey[key->llKeyLen] = 0x00;
      
      /*�����Ŀ��ֵС������*/
      if (strcmp( pcKey, pcaKey ) < 0)
      {
      	break;
      }
    }
    
    /*���û���ҵ�*/
    if (iIndex == -1)
    {
    	return NOT_FOUND;
    }
    else
    /*����ҵ�*/
    {
    	/*���õ�ǰ���Ľ��*/
    	iaOutResult[iaLoop] = iIndex;
    	
    	/*ָ��ǰֵ*/
    	key = (KEY_POINT*)(pcDataBuf + iIndex * sizeof(KEY_POINT));
    	
    	/*������һ��*/
    	iRet = managerPositionRightOpen( iaOutResult, pcaKey, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
    	
    	/*����ҵ�����������*/
    	if (iRet == SUCCESS)
    	{
    		return SUCCESS;
    	}
    	/*����¼�����û���ҵ�*/
    	else
    	/*if (iRet == NOT_FOUND)*/
    	{
    		return NOT_FOUND;
    	}
    }
	}
}

/**********************************************************************
��������: managerPositionRightClose
��������: ����˴����еĶ�λ��ʼλ�ã��ұ�����
��    ����
��    һ�����λ�ñ���     O
��    ����KEY              I
��    ������ַ             I
��    �ģ�����             I
��    �壺����             I
��    �����ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161106
**********************************************************************/
int managerPositionRightClose( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId )
{
	char  pcKey[MAX_KEY_LEN];
	KEY_POINT *key;                /*�����������е����ݽṹ*/
  KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
  char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
  int  iIndex;
  int  iTmp;
  int  iRet;
  
  if (iaSize == 0)
  {
		return NOT_FOUND;
	}
	
  /*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
		/*ȡ�����һ����������*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
    for (iIndex = iaSize - 1; iIndex >= 0; iIndex--)
    {
    	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iIndex * sizeof(KEY_VALUE_POINT));
    	
    	/*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
      valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
      pcKey[keyData->llKeyLen] = 0x00;
            
      /*�����Ŀ��ֵС������*/
      if (strcmp( pcKey, pcaKey ) <= 0)
      {
      	break;
      }
    }
    
    /*���û���ҵ�*/
    if (iIndex == -1)
    {
    	return NOT_FOUND;
    }
    else
    {
    	/*���õ�ǰ���Ľ��*/
    	iaOutResult[iaLoop] = iIndex;
    	
    	return SUCCESS;
    }
	}
  /*�������һ��*/
	else
	{
		if (iaAddr == 0)
		{
			return NOT_FOUND;
		}
		
		/*ȡ����������������*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
    /*������һ����Ŀ��ֵС������λ��*/
    for (iIndex = iaSize - 1; iIndex >= 0; iIndex--)
    {
    	key = (KEY_POINT*)(pcDataBuf + iIndex * sizeof(KEY_POINT));
    	
    	/*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
      valueToBuf( pcKey, key->llKeyAddr, key->llKeyLen, iaIndepentFileId );
      pcKey[key->llKeyLen] = 0x00;
      
      /*�����Ŀ��ֵС������*/
      if (strcmp( pcKey, pcaKey ) <= 0)
      {
      	break;
      }
    }
    
    /*���û���ҵ�*/
    if (iIndex == -1)
    {
    	return NOT_FOUND;
    }
    else
    /*����ҵ�*/
    {
    	/*���õ�ǰ���Ľ��*/
    	iaOutResult[iaLoop] = iIndex;
    	
    	/*ָ��ǰֵ*/
    	key = (KEY_POINT*)(pcDataBuf + iIndex * sizeof(KEY_POINT));
    	
    	/*������һ��*/
    	iRet = managerPositionRightClose( iaOutResult, pcaKey, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
    	
    	/*����ҵ�����������*/
    	if (iRet == SUCCESS)
    	{
    		return SUCCESS;
    	}
    	/*����¼�����û���ҵ�*/
    	else
    	/*if (iRet == NOT_FOUND)*/
    	{
    		return NOT_FOUND;
    	}
    }
	}
}

/**********************************************************************
��������: managerPositionLeftOpenMain
��������: ����˴����еĶ�λ��ʼλ�ã�������
��    ����
��    һ�����λ�ñ���     O
��    ����KEY              I
��    ������ַ             I
��    �ģ�����             I
��    �壺����             I
��    �����ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161105
**********************************************************************/
int managerPositionLeftOpenMain( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId )
{
	char  pcKey[MAX_KEY_LEN];
	KEY_POINT *key;                /*�����������е����ݽṹ*/
  KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
  char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
  int  iIndex;
  int  iTmp;
  int  iRet;
  
  if (iaSize == 0)
  {
		return NOT_FOUND;
	}
	
  /*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
		/*ȡ�����һ����������*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    for (iIndex = 0; iIndex < iaSize; iIndex++)
    {
    	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iIndex * sizeof(KEY_VALUE_POINT));
    	
    	/*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
      valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
      pcKey[keyData->llKeyLen] = 0x00;
      
      /*�����Ŀ��ֵ�󣬽���*/
      if (strcmp( pcKey, pcaKey ) > 0)
      {
      	break;
      }
    }
    
    /*�����*/
    if (iIndex < iaSize)
    {
    	/*���õ�ǰ���Ľ��*/
    	iaOutResult[iaLoop] = iIndex;
    	
    	return SUCCESS;
    }
    else
    {
    	return NOT_FOUND;
    }
	}
  /*�������һ��*/
	else
	{
		if (iaAddr == 0)
		{
			return NOT_FOUND;
		}
		/*ȡ����������������*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    /*������һ����Ŀ��ֵ�������λ��*/
    for (iIndex = 0; iIndex < iaSize; iIndex++)
    {
    	key = (KEY_POINT*)(pcDataBuf + iIndex * sizeof(KEY_POINT));
    	
    	/*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
      valueToBufMain( pcKey, key->llKeyAddr, key->llKeyLen, iaIndepentId );
      pcKey[key->llKeyLen] = 0x00;
      
      /*�����Ŀ��ֵ�󣬽���*/
      if (strcmp( pcKey, pcaKey ) > 0)
      {
      	break;
      }
    }
    
    /*�����һ��λ�þ��ǣ����õ�ǰ������λ�ã��Լ����漶����λ��*/
    if (iIndex == 0 && strcmp( pcKey, pcaKey ) == 0)
    {
    	/*���õ�ǰ���Ľ��*/
    	iaOutResult[iaLoop] = iIndex;
    	
    	/*����ʣ�¼��Ľ��*/
    	for (iTmp = iaLoop + 1; iTmp <= iaTotalLoops; iTmp++)
    	{
    		iaOutResult[iTmp] = 0;
    	}
    	
    	return SUCCESS;
    }
    /*���ǵ�һ��λ��*/
    else
    {
    	/*��������ڣ���ǰ key ������������*/
    	if (iIndex == iaSize)
    	{
    		/*���õ�ǰ���Ľ��*/
    	  iaOutResult[iaLoop] = iIndex - 1;
    	  
    	  /*ָ��ǰֵ*/
    		key = (KEY_POINT*)(pcDataBuf + (iIndex - 1) * sizeof(KEY_POINT));
    		
    		/*������һ��*/
    		iRet = managerPositionLeftOpenMain( iaOutResult, pcaKey, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
    		
    		/*����ҵ�����������*/
    		if (iRet == SUCCESS)
    		{
    			return SUCCESS;
    		}
    		/*����¼�����û���ҵ�*/
    		else
    		/*if (iRet == NOT_FOUND)*/
    		{
    			return NOT_FOUND;
    		}
    	}
    	/*�������*/
    	else
    	{
    		/*���õ�ǰ���Ľ��*/
    	  iaOutResult[iaLoop] = iIndex - 1;
    		
    		/*ָ��ǰֵ*/
    		key = (KEY_POINT*)(pcDataBuf + (iIndex - 1) * sizeof(KEY_POINT));
    		
    		/*������һ��*/
    		iRet = managerPositionLeftOpenMain( iaOutResult, pcaKey, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
    		
    		/*����ҵ�����������*/
    		if (iRet == SUCCESS)
    		{
    			return SUCCESS;
    		}
    		else
    		/*if (iRet == NOT_FOUND)*/
    		{
    			/*�������õ�ǰ���Ľ��*/
    	    iaOutResult[iaLoop] = iIndex;
    	    
    	    /*����ʣ�¼��Ľ��*/
        	for (iTmp = iaLoop + 1; iTmp <= iaTotalLoops; iTmp++)
        	{
    		    iaOutResult[iTmp] = 0;
    	    }
    	  
    			return SUCCESS;
    		}
    	}
    }
	}
}

/**********************************************************************
��������: managerPositionLeftCloseMain
��������: ����˴����еĶ�λ��ʼλ�ã��������
��    ����
��    һ�����λ�ñ���     O
��    ����KEY              I
��    ������ַ             I
��    �ģ�����             I
��    �壺����             I
��    �����ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161105
**********************************************************************/
int managerPositionLeftCloseMain( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId )
{
	char  pcKey[MAX_KEY_LEN];
	KEY_POINT *key;                /*�����������е����ݽṹ*/
  KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
  char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
  int  iIndex;
  int  iTmp;
  int  iRet;

  if (iaSize == 0)
  {
		return NOT_FOUND;
	}
  /*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
		/*ȡ�����һ����������*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );

    for (iIndex = 0; iIndex < iaSize; iIndex++)
    {
    	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iIndex * sizeof(KEY_VALUE_POINT));
    	
    	/*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
      valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
      pcKey[keyData->llKeyLen] = 0x00;
      
      /*�����Ŀ��ֵ�󣬽���*/
      if (strcmp( pcKey, pcaKey ) >= 0)
      {
      	break;
      }
    }

    /*�����*/
    if (iIndex < iaSize)
    {
    	/*���õ�ǰ���Ľ��*/
    	iaOutResult[iaLoop] = iIndex;
    	
    	return SUCCESS;
    }
    else
    {
    	return NOT_FOUND;
    }
	}
  /*�������һ��*/
	else
	{
		if (iaAddr == 0)
		{
			return NOT_FOUND;
		}
		/*ȡ����������������*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    /*������һ����Ŀ��ֵ�������λ��*/
    for (iIndex = 0; iIndex < iaSize; iIndex++)
    {
    	key = (KEY_POINT*)(pcDataBuf + iIndex * sizeof(KEY_POINT));
    	
    	/*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
      valueToBufMain( pcKey, key->llKeyAddr, key->llKeyLen, iaIndepentId );
      pcKey[key->llKeyLen] = 0x00;
            
      /*�����Ŀ��ֵ�󣬽���*/
      if (strcmp( pcKey, pcaKey ) >= 0)
      {
      	break;
      }
    }
    
    /*�����һ��λ�þ��ǣ����߸�ֵ�պ���Ŀ��ֵ��ȣ����õ�ǰ������λ�ã��Լ����漶����λ��*/
    if (iIndex == 0 && strcmp( pcKey, pcaKey ) >= 0)/*������󿪵��㷨��ͬ*/
    {
    	/*���õ�ǰ���Ľ��*/
    	iaOutResult[iaLoop] = iIndex;

    	/*����ʣ�¼��Ľ��*/
    	for (iTmp = iaLoop + 1; iTmp <= iaTotalLoops; iTmp++)
    	{
    		iaOutResult[iTmp] = 0;
    	}

    	return SUCCESS;
    }
    /*���ǵ�һ��λ��*/
    else
    {
    	/*��������ڣ���ǰ key ������������*/
    	if (iIndex == iaSize)
    	{
    		/*���õ�ǰ���Ľ��*/
    	  iaOutResult[iaLoop] = iIndex - 1;
    	  
    	  /*ָ��ǰֵ*/
    		key = (KEY_POINT*)(pcDataBuf + (iIndex - 1) * sizeof(KEY_POINT));

    		/*������һ��*/
    		iRet = managerPositionLeftCloseMain( iaOutResult, pcaKey, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
    		
    		/*����ҵ�����������*/
    		if (iRet == SUCCESS)
    		{
    			return SUCCESS;
    		}
    		/*����¼�����û���ҵ�*/
    		else
    		/*if (iRet == NOT_FOUND)*/
    		{
    			return NOT_FOUND;
    		}
    	}
    	/*�������*/
    	else
    	{
    		/*���õ�ǰ����ǰһ��ֵ*/
    	  iaOutResult[iaLoop] = iIndex - 1;
    		
    		/*ָ��ǰֵ*/
    		key = (KEY_POINT*)(pcDataBuf + (iIndex - 1) * sizeof(KEY_POINT));

    		/*������һ��*/
    		iRet = managerPositionLeftCloseMain( iaOutResult, pcaKey, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
    		
    		/*����ҵ�����������*/
    		if (iRet == SUCCESS)
    		{
    			return SUCCESS;
    		}
    		else
    		/*if (iRet == NOT_FOUND)*/
    		{
    			/*�������õ�ǰ���Ľ��*/
    	    iaOutResult[iaLoop] = iIndex;
    	    
    	    /*����ʣ�¼��Ľ��*/
        	for (iTmp = iaLoop + 1; iTmp <= iaTotalLoops; iTmp++)
        	{
    		    iaOutResult[iTmp] = 0;
    	    }
    	  
    			return SUCCESS;
    		}
    	}
    }
	}
}

/**********************************************************************
��������: managerPositionRightOpenMain
��������: ����˴����еĶ�λ��ʼλ�ã��ҿ�����
��    ����
��    һ�����λ�ñ���     O
��    ����KEY              I
��    ������ַ             I
��    �ģ�����             I
��    �壺����             I
��    �����ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161106
**********************************************************************/
int managerPositionRightOpenMain( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId )
{
	char  pcKey[MAX_KEY_LEN];
	KEY_POINT *key;                /*�����������е����ݽṹ*/
  KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
  char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
  int  iIndex;
  int  iTmp;
  int  iRet;
  
  if (iaSize == 0)
  {
		return NOT_FOUND;
	}
	
  /*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
		/*ȡ�����һ����������*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    for (iIndex = iaSize - 1; iIndex >= 0; iIndex--)
    {
    	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iIndex * sizeof(KEY_VALUE_POINT));
    	
    	/*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
      valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
      pcKey[keyData->llKeyLen] = 0x00;
      
      /*�����Ŀ��ֵС������*/
      if (strcmp( pcKey, pcaKey ) < 0)
      {
      	break;
      }
    }
    
    /*���û���ҵ�*/
    if (iIndex == -1)
    {
    	return NOT_FOUND;
    }
    else
    {
    	/*���õ�ǰ���Ľ��*/
    	iaOutResult[iaLoop] = iIndex;
    	
    	return SUCCESS;
    }
	}
  /*�������һ��*/
	else
	{
		if (iaAddr == 0)
		{
			return NOT_FOUND;
		}
		
		/*ȡ����������������*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    /*������һ����Ŀ��ֵС������λ��*/
    for (iIndex = iaSize - 1; iIndex >= 0; iIndex--)
    {
    	key = (KEY_POINT*)(pcDataBuf + iIndex * sizeof(KEY_POINT));
    	
    	/*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
      valueToBufMain( pcKey, key->llKeyAddr, key->llKeyLen, iaIndepentId );
      pcKey[key->llKeyLen] = 0x00;
      
      /*�����Ŀ��ֵС������*/
      if (strcmp( pcKey, pcaKey ) < 0)
      {
      	break;
      }
    }
    
    /*���û���ҵ�*/
    if (iIndex == -1)
    {
    	return NOT_FOUND;
    }
    else
    /*����ҵ�*/
    {
    	/*���õ�ǰ���Ľ��*/
    	iaOutResult[iaLoop] = iIndex;
    	
    	/*ָ��ǰֵ*/
    	key = (KEY_POINT*)(pcDataBuf + iIndex * sizeof(KEY_POINT));
    	
    	/*������һ��*/
    	iRet = managerPositionRightOpenMain( iaOutResult, pcaKey, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
    	
    	/*����ҵ�����������*/
    	if (iRet == SUCCESS)
    	{
    		return SUCCESS;
    	}
    	/*����¼�����û���ҵ�*/
    	else
    	/*if (iRet == NOT_FOUND)*/
    	{
    		return NOT_FOUND;
    	}
    }
	}
}

/**********************************************************************
��������: managerPositionRightCloseMain
��������: ����˴����еĶ�λ��ʼλ�ã��ұ�����
��    ����
��    һ�����λ�ñ���     O
��    ����KEY              I
��    ������ַ             I
��    �ģ�����             I
��    �壺����             I
��    �����ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161106
**********************************************************************/
int managerPositionRightCloseMain( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId )
{
	char  pcKey[MAX_KEY_LEN];
	KEY_POINT *key;                /*�����������е����ݽṹ*/
  KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
  char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
  int  iIndex;
  int  iTmp;
  int  iRet;
  
  if (iaSize == 0)
  {
		return NOT_FOUND;
	}
	
  /*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
		/*ȡ�����һ����������*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    for (iIndex = iaSize - 1; iIndex >= 0; iIndex--)
    {
    	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iIndex * sizeof(KEY_VALUE_POINT));
    	
    	/*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
      valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
      pcKey[keyData->llKeyLen] = 0x00;
            
      /*�����Ŀ��ֵС������*/
      if (strcmp( pcKey, pcaKey ) <= 0)
      {
      	break;
      }
    }
    
    /*���û���ҵ�*/
    if (iIndex == -1)
    {
    	return NOT_FOUND;
    }
    else
    {
    	/*���õ�ǰ���Ľ��*/
    	iaOutResult[iaLoop] = iIndex;
    	
    	return SUCCESS;
    }
	}
  /*�������һ��*/
	else
	{
		if (iaAddr == 0)
		{
			return NOT_FOUND;
		}
		
		/*ȡ����������������*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    /*������һ����Ŀ��ֵС������λ��*/
    for (iIndex = iaSize - 1; iIndex >= 0; iIndex--)
    {
    	key = (KEY_POINT*)(pcDataBuf + iIndex * sizeof(KEY_POINT));
    	
    	/*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
      valueToBufMain( pcKey, key->llKeyAddr, key->llKeyLen, iaIndepentId );
      pcKey[key->llKeyLen] = 0x00;
      
      /*�����Ŀ��ֵС������*/
      if (strcmp( pcKey, pcaKey ) <= 0)
      {
      	break;
      }
    }
    
    /*���û���ҵ�*/
    if (iIndex == -1)
    {
    	return NOT_FOUND;
    }
    else
    /*����ҵ�*/
    {
    	/*���õ�ǰ���Ľ��*/
    	iaOutResult[iaLoop] = iIndex;
    	
    	/*ָ��ǰֵ*/
    	key = (KEY_POINT*)(pcDataBuf + iIndex * sizeof(KEY_POINT));
    	
    	/*������һ��*/
    	iRet = managerPositionRightCloseMain( iaOutResult, pcaKey, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
    	
    	/*����ҵ�����������*/
    	if (iRet == SUCCESS)
    	{
    		return SUCCESS;
    	}
    	/*����¼�����û���ҵ�*/
    	else
    	/*if (iRet == NOT_FOUND)*/
    	{
    		return NOT_FOUND;
    	}
    }
	}
}
