#define __KEY_VALUE_THREAD_MANAGER_UNLOAD_LOAD_C

/*
������ MANAGER �е� UNLOAD LOAD ���
*/

#ifndef __COMMON_HEAD_H
  #include "commonHead.h"
#endif

void managerUnloadDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd );
void managerUnloadDealChildThread( UNLOAD_SHARE *unloadShare );
int  managerUnloadDealUnloadData( UNLOAD_SHARE *unloadShare, int iaStartOffset[], int iaEndOffset[], __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId );
int  managerUnloadDealGetDataLeft( UNLOAD_SHARE *unloadShare, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], int iaLoop, int iaTotalLoops, int iaIndepentFileId );
int  managerUnloadDealGetDataOther( UNLOAD_SHARE *unloadShare,  __int64 iaAddr, __int64 iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId );
int  managerUnloadDealGetDataRight( UNLOAD_SHARE *unloadShare, __int64 iaAddr, __int64 iaSize, int iaEndOffset[], int iaLoop, int iaTotalLoops, int iaIndepentFileId );
int  managerUnloadDealUnloadDataMain( UNLOAD_SHARE *unloadShare, int iaStartOffset[], int iaEndOffset[], __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId );
int  managerUnloadDealGetDataLeftMain( UNLOAD_SHARE *unloadShare, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], int iaLoop, int iaTotalLoops, int iaIndepentId );
int  managerUnloadDealGetDataOtherMain( UNLOAD_SHARE *unloadShare,  __int64 iaAddr, __int64 iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId );
int  managerUnloadDealGetDataRightMain( UNLOAD_SHARE *unloadShare, __int64 iaAddr, __int64 iaSize, int iaEndOffset[], int iaLoop, int iaTotalLoops, int iaIndepentId );
void managerLoadDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd );
void managerLoadDealChildThread( LOAD_SHARE *loadShare );

/**********************************************************************
��������: managerUnloadDeal
��������: ����˴��� unload ����
��    ����
��    һ��SOCKET               I
��    �������ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20161022
**********************************************************************/
void managerUnloadDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd )
{
/*
�������ݽṹ������+����+�ļ�����+ֵ���䣨���ҿ������ұա�����ҿ�������ұգ�+KEY��ʼֵ����+KEY��ʼֵ+KEY����ֵ����+KEY����ֵ
�������ݽṹ���������+��������������
*/
/*
�������10����������ô��
*/
#define MANAGER_UNLOAD_DEAL_SEND_ERROR( value ) \
{\
	memset( pcResult, 0x00, sizeof(pcResult) );\
	sprintf( pcResult, "%d", value ); \
	cResult = pcResult[0];\
	\
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );\
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, 1 );\
  \
  send( iaSocket, pcSendLen, DATA_BUF_LEN,0 );\
	send( iaSocket, &cResult, 1, 0 );\
}
	char pcResult[INT_LEN + 1];
	char cResult;
	char pcSendBuf[INT64_LEN + 1 + 1];
	char pcFilePath[FILE_PATH_LEN + 1];
	char pcKeyStartLen[KEY_VALUE_LENGTH + 1];
  char pcKeyEndLen[KEY_VALUE_LENGTH + 1];
  char pcKeyStart[MAX_KEY_LEN + 1];
  char pcKeyEnd[MAX_KEY_LEN + 1];
  char pcValueType[KEY_VALUE_LENGTH + 1];
  char pcSendLen[DATA_BUF_LEN + 1];
  char pcRecvLen[DATA_BUF_LEN + 1];
  int  iKeyStartLen;
	int  iKeyEndLen;
	FILE *fp;
	int  iRet;
	char *pcBufStart;
	char *pcBufEnd;
	UNLOAD_SHARE unloadStruct;
	char pcOperate[OPERATE_LEN + 1];
	char pcTotalRows[INT64_LEN + 1];
	int iRecvLen;
	int iTotalLen;
	int iDataLen;
	int iSendLen;
	int iTime;
	#ifdef SYSTEM_WINDOWS_64
	  void *threadHandle;                /*windows�µ��߳̾��*/
	  unsigned long ulWindowsThreadId;
	#else/*#ifdef SYSTEM_UBUNTU*/
	  pthread_t threadId;                /*����OS�µ��߳�ID*/
	#endif
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcBufEnd;
	
	/*ȡ�ļ�·��*/
	memset( pcFilePath, 0x00, sizeof(pcFilePath) );
	memcpy( pcFilePath, pcBufStart, NAME_LEN );
	AllTrim( pcFilePath );
	pcBufStart += FILE_PATH_LEN;
	
	/*ȡֵ����*/
	memset( pcValueType, 0x00, sizeof(pcValueType) );
	memcpy( pcValueType, pcBufStart, KEY_VALUE_LENGTH );
	pcBufStart += KEY_VALUE_LENGTH;
	
	/*ȡKEY��ʼֵ����*/
	memset( pcKeyStartLen, 0x00, sizeof(pcKeyStartLen) );
	memcpy( pcKeyStartLen, pcBufStart, KEY_VALUE_LENGTH );
	pcBufStart += KEY_VALUE_LENGTH;
	
	/*ȡKEY��ʼֵ*/
	iKeyStartLen = atoi( pcKeyStartLen );
	memset( pcKeyStart, 0x00, sizeof(pcKeyStart) );
	memcpy( pcKeyStart, pcBufStart, iKeyStartLen );
	pcBufStart += iKeyStartLen;
	
	/*ȡKEY����ֵ����*/
	memset( pcKeyEndLen, 0x00, sizeof(pcKeyEndLen) );
	memcpy( pcKeyEndLen, pcBufStart, KEY_VALUE_LENGTH );
	pcBufStart += KEY_VALUE_LENGTH;
	
	/*ȡKEY����ֵ*/
	iKeyEndLen = atoi( pcKeyEndLen );
	memset( pcKeyEnd, 0x00, sizeof(pcKeyEnd) );
	memcpy( pcKeyEnd, pcBufStart, iKeyEndLen );
	pcBufStart += iKeyEndLen;
	
	logFmtSyn( LVNOR, "pcFilePath[%s]pcValueType[%s]pcKeyStart[%s]pcKeyEnd[%s]", pcFilePath, pcValueType, pcKeyStart, pcKeyEnd );
	
	/*�½��ļ�*/
	fp = fopen64( pcFilePath, "wb+" );
	if (fp == NULL)
	{
		MANAGER_UNLOAD_DEAL_SEND_ERROR( ERROR );
		return;
	}
	
	/*���������̹߳�������*/
	memset( &unloadStruct, 0x00, sizeof(unloadStruct) );
	unloadStruct.fp = fp;                                              /*�ļ�ָ��*/
	memcpy( unloadStruct.pcValueType, pcValueType, KEY_VALUE_LENGTH ); /*����������ֵ���䣨KEY��VALUE ���ҿ������ұա�����ҿ�������ұ�*/
	memcpy( unloadStruct.pcKeyStart, pcKeyStart, iKeyStartLen );       /*KEY �Ŀ�ʼֵ*/
	unloadStruct.iKeyStartLen = iKeyStartLen;                          /*KEY �Ŀ�ʼֵ����*/
	memcpy( unloadStruct.pcKeyEnd, pcKeyEnd, iKeyEndLen );             /*KEY �Ľ���ֵ*/
	unloadStruct.iKeyEndLen = iKeyEndLen;                              /*KEY �Ľ���ֵ����*/
	unloadStruct.iState = RUN_STATE_UNFINISHED;                        /*���߳�����״̬*/
	unloadStruct.iCmd = RUN_CMD_RUN;                                   /*���߳���������*/
	
	MANAGER_RDLOCK();
	
	#ifdef SYSTEM_WINDOWS_64
    threadHandle = CreateThread( NULL, 0, (void *)(&managerUnloadDealChildThread), &unloadStruct, 0, &ulWindowsThreadId );
    if (sgManagerThread.threadHandle == NULL)
    {
    	MANAGER_UNLOAD_DEAL_SEND_ERROR( ERROR );
    	MANAGER_RDUNLOCK();
      return;
    }
  #else/*#ifdef SYSTEM_UBUNTU*/
    if (pthread_create(&threadId, NULL, (void *)(&managerUnloadDealChildThread), &unloadStruct) != 0)
    {
  	  MANAGER_UNLOAD_DEAL_SEND_ERROR( ERROR );
  	  MANAGER_RDUNLOCK();
      return;
    }
  
    /*�̷߳��룬����ʱ�Զ�������Դ*/
    pthread_detach(threadId);
  #endif
  
  /*ѭ���ȴ�������߼���߳�ִ�����*/
  iTime = 0;
	while (iTime < EACH_JOB_TIME * 10) /*EACH_JOB_TIME �ĵ�λ���룬ÿ��˯��0.1��*/
	{		
		/*������߳�����ɣ���ʾ���߳�����ִ����ɣ����˳�*/
		if (unloadStruct.iState == RUN_STATE_FINISHED)
		{
			break;
		}
		else
		{
			usleep( 100*1000 );
		}
		iTime++;
	}
	
	/*�����̷߳�����ֹ����*/
	unloadStruct.iCmd = RUN_CMD_EXIT;
	
	/*�ȴ����߳�ִ�н���*/
	while (unloadStruct.iState != RUN_STATE_FINISHED)
	{
		THREAD_THREAD_SLEEP();
	}
	
	fclose(fp);
		
	MANAGER_RDUNLOCK();
	
	memset( pcTotalRows, 0x00, sizeof(pcTotalRows) );
  sprintf( pcTotalRows, "%0*lld", INT64_LEN, unloadStruct.llTotalRows );
  
  /*�鷢������*/
  memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
  
  sprintf( &cResult, "%d", SUCCESS );
  memcpy( pcSendBuf, &cResult, 1 );
  memcpy( pcSendBuf + 1, pcTotalRows, INT64_LEN );
  
  iSendLen = 1 + INT64_LEN;
  
  memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
  
  send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
  
  send( iaSocket, pcSendBuf, iSendLen, 0 );
	
	return;
}

/**********************************************************************
��������: managerUnloadDealChildThread
��������: ����˴��� unload ����
��    ����
��    һ������ṹ��               I/O
��    ��: �����
����ʱ��: 20161031
**********************************************************************/
void managerUnloadDealChildThread( UNLOAD_SHARE *unloadShare )
{
	FILE *fp;
  int  iStartOffsetRecord[20];   /*��ʼλ�ü�¼*/
	int  iEndOffsetRecord[20];     /*����λ�ü�¼*/
	__int64  llDisc;
	__int64  llSize;
	__int64  llLoops;
	int  iRet;
	int  iIndepentId;
	int  iIndepentFileId;
	
	/*ָ������ļ�ָ��*/
	fp = unloadShare->fp;
	
	for (iIndepentId = 0; iIndepentId < INDEPENDENT_SIZE; iIndepentId++)
	{
	  llDisc = sgGlobalVariableMain[iIndepentId].llgFirstDiscMain;
	  llSize = sgGlobalVariableMain[iIndepentId].llgFirstSizeMain;
	  llLoops = sgGlobalVariableMain[iIndepentId].llgLoopsMain;
	
	  /*��λ��ʼλ��*/
	  {
	  	/*��ʼ����ʼλ�ü�¼����*/
	    memset( &iStartOffsetRecord, 0x00, sizeof(iStartOffsetRecord) );
	  
	  	/*�������*/
	    if (!strcmp( unloadShare->pcValueType, KEY_OPEN_OPEN ) != !strcmp( unloadShare->pcValueType, KEY_OPEN_CLOSE ))
      {
    	  iRet = managerPositionLeftOpenMain( iStartOffsetRecord, unloadShare->pcKeyStart, llDisc, llSize, 1, llLoops, iIndepentId );
    	  
    	  /*���û����������*/
    	  if (iRet == NOT_FOUND)
    	  {
          continue ;
    	  }
      }
      else
      {
    	  iRet = managerPositionLeftCloseMain( iStartOffsetRecord, unloadShare->pcKeyStart, llDisc, llSize, 1, llLoops, iIndepentId );
    	  
    	  /*���û����������*/
    	  if (iRet == NOT_FOUND)
    	  {
          continue ;
    	  }
      }
	  }
	  
	  /*��λ����λ��*/
	  {
	  	/*��ʼ������λ�ü�¼����*/
	    memset( iEndOffsetRecord, 0x00, sizeof(iEndOffsetRecord) );
	  
	  	/*������ҿ�*/
	    if (!strcmp( unloadShare->pcValueType, KEY_OPEN_OPEN ) != !strcmp( unloadShare->pcValueType, KEY_CLOSE_OPEN ))
      {
    	  iRet = managerPositionRightOpenMain( iEndOffsetRecord, unloadShare->pcKeyEnd, llDisc, llSize, 1, llLoops, iIndepentId );
    	  
    	  /*���û����������*/
    	  if (iRet == NOT_FOUND)
    	  {
    	  	continue ;
    	  }
      }
      else
      {
    	  iRet = managerPositionRightCloseMain( iEndOffsetRecord, unloadShare->pcKeyEnd, llDisc, llSize, 1, llLoops, iIndepentId );
    	  
    	  /*���û����������*/
    	  if (iRet == NOT_FOUND)
    	  {
    	  	continue ;
    	  }
      }
	  }
    
	  /*��������*/
	  iRet = managerUnloadDealUnloadDataMain( unloadShare, iStartOffsetRecord, iEndOffsetRecord, llDisc, llSize, 1, llLoops, iIndepentId );
	  
	  if (iRet == INTERRUPT)
	  {
	    break;
	  }
	}
	
	for (iIndepentFileId = 0; iIndepentFileId < INDEPENDENT_FILE_SIZE; iIndepentFileId++)
	{
	  llDisc = sgBase[iIndepentFileId].llFirstDisc;
	  llSize = sgBase[iIndepentFileId].llFirstSize;
	  llLoops = sgBase[iIndepentFileId].llLoops;
	
	  /*��λ��ʼλ��*/
	  {
	  	/*��ʼ����ʼλ�ü�¼����*/
	    memset( &iStartOffsetRecord, 0x00, sizeof(iStartOffsetRecord) );
	  
	  	/*�������*/
	    if (!strcmp( unloadShare->pcValueType, KEY_OPEN_OPEN ) != !strcmp( unloadShare->pcValueType, KEY_OPEN_CLOSE ))
      {
    	  iRet = managerPositionLeftOpen( iStartOffsetRecord, unloadShare->pcKeyStart, llDisc, llSize, 1, llLoops, iIndepentFileId );
    	  
    	  /*���û����������*/
    	  if (iRet == NOT_FOUND)
    	  {
          continue ;
    	  }
      }
      else
      {
    	  iRet = managerPositionLeftClose( iStartOffsetRecord, unloadShare->pcKeyStart, llDisc, llSize, 1, llLoops, iIndepentFileId );
    	  
    	  /*���û����������*/
    	  if (iRet == NOT_FOUND)
    	  {
          continue ;
    	  }
      }
	  }
	  
	  /*��λ����λ��*/
	  {
	  	/*��ʼ������λ�ü�¼����*/
	    memset( iEndOffsetRecord, 0x00, sizeof(iEndOffsetRecord) );
	  
	  	/*������ҿ�*/
	    if (!strcmp( unloadShare->pcValueType, KEY_OPEN_OPEN ) != !strcmp( unloadShare->pcValueType, KEY_CLOSE_OPEN ))
      {
    	  iRet = managerPositionRightOpen( iEndOffsetRecord, unloadShare->pcKeyEnd, llDisc, llSize, 1, llLoops, iIndepentFileId );
    	  
    	  /*���û����������*/
    	  if (iRet == NOT_FOUND)
    	  {
    	  	continue ;
    	  }
      }
      else
      {
    	  iRet = managerPositionRightClose( iEndOffsetRecord, unloadShare->pcKeyEnd, llDisc, llSize, 1, llLoops, iIndepentFileId );
    	  
    	  /*���û����������*/
    	  if (iRet == NOT_FOUND)
    	  {
    	  	continue ;
    	  }
      }
	  }
    
	  /*��������*/
	  iRet = managerUnloadDealUnloadData( unloadShare, iStartOffsetRecord, iEndOffsetRecord, llDisc, llSize, 1, llLoops, iIndepentFileId );
	  
	  if (iRet == INTERRUPT)
	  {
	    break;
	  }
	}
	unloadShare->iState = RUN_STATE_FINISHED;
	return;
}

/**********************************************************************
��������: managerUnloadDealUnloadData
��������: ����˴��� unload �е����ݵ���
��    ����
��    һ���̹߳������     I/O
��    ������ʼλ��         I
��    ��������λ��         I
��    �ģ����ݵ�ַ         I
��    �壺����             I
��    ��������             I
��    �ߣ��ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161106
**********************************************************************/
int managerUnloadDealUnloadData( UNLOAD_SHARE *unloadShare, int iaStartOffset[], int iaEndOffset[], __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId )
{
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	KEY_POINT *key;                /*�����������е����ݽṹ*/
	KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
  char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
  __int64 iAddr;
  int  iSize;
  int  iIndex;
  __int64 llTotalRows;
  int  iTmp;
  int  iTmp2;
  int  iTmp3;
  int  iRet;
  
  /*������ʼλ�úͽ���λ�ò�ͬ�ļ�*/
  for (iIndex = 1; iIndex <= iaTotalLoops; iIndex++)
  {
  	if (iaStartOffset[iIndex] < iaEndOffset[iIndex] )
  	{
  		break;
  	}
  }
    
  /*�������λ�ö���ͬ����ʾֻ��һ��������������*/
  if (iIndex > iaTotalLoops)
  {
  	/*ȡ��������*/
  	{
  	  /*ȡ����󼶵�����*/
      iAddr = iaAddr;
      for (iTmp2 = 1; iTmp2 < iaTotalLoops; iTmp2++)
      {
      	memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
        getKeyBlockData( pcDataBuf, iAddr, iaIndepentFileId );
      
        key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iTmp2] * sizeof(KEY_POINT));
      
        iAddr = key->llChildAddr;
      }
      
      memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
      getKeyBlockData( pcDataBuf, iAddr, iaIndepentFileId );
      keyData = (KEY_VALUE_POINT*)(pcDataBuf + iaStartOffset[iaTotalLoops] * sizeof(KEY_VALUE_POINT));
  	}
  	
  	/*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
    valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
    pcKey[keyData->llKeyLen] = 0x00;
    
    /*�� VALUE �ļ��л�ȡ VALUE ��ֵ*/
    valueToBuf( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentFileId );
    pcValue[keyData->llValueLen] = 0x00;
  	
  	fprintf( unloadShare->fp, "\'%s\' \'%s\'\n", pcKey, pcValue );
  	
  	(unloadShare->llTotalRows)++;
  	return SUCCESS;
  }
    
    /*ȡ����ǰ��������*/
  iAddr = iaAddr;
  for (iTmp = 1; iTmp < iIndex; iTmp++)
  {
  	memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iAddr, iaIndepentFileId );
    
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iTmp] * sizeof(KEY_POINT));
    
    iAddr = key->llChildAddr;
  }

  memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
  getKeyBlockData( pcDataBuf, iAddr, iaIndepentFileId );
      
  /*��ȡ����*/
  {
    /*����ʼλ�õļ�*/
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iIndex] * sizeof(KEY_POINT));
    iRet = managerUnloadDealGetDataLeft( unloadShare, key->llChildAddr, key->llChildSize, iaStartOffset, iIndex + 1, iaTotalLoops, iaIndepentFileId );
    /*������жϽ���*/
    if (iRet)
    {
    	return iRet;
    }
    
    /*��������λ�õļ�*/
    for (iTmp3 = iaStartOffset[iIndex] + 1; iTmp3 < iaEndOffset[iIndex]; iTmp3++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp3 * sizeof(KEY_POINT));
      iRet = managerUnloadDealGetDataOther( unloadShare, key->llChildAddr, key->llChildSize, iIndex + 1, iaTotalLoops, iaIndepentFileId );
      if (iRet)
      {
    	  return iRet;
      }
    }
    
    /*�������λ�õļ�*/
    key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iIndex] * sizeof(KEY_POINT));
    iRet = managerUnloadDealGetDataRight( unloadShare, key->llChildAddr, key->llChildSize, iaEndOffset, iIndex + 1, iaTotalLoops, iaIndepentFileId );
    if (iRet)
    {
    	return iRet;
    }
  }
  
  return SUCCESS;
}

/**********************************************************************
��������: managerUnloadDealGetDataLeft
��������: ����˴��� unload �еĵ�������
��    ����
��    һ���̹߳������     I
��    �������ݵ�ַ         I
��    ������һ������       I
��    �ģ���ʼλ��         I
��    �壺����             I
��    �����ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161112
**********************************************************************/
int managerUnloadDealGetDataLeft( UNLOAD_SHARE *unloadShare, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], int iaLoop, int iaTotalLoops, int iaIndepentFileId )
{
	char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
	KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
	KEY_POINT *key;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iTmp;
	int  iRet;
	
	if (unloadShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}
	
	/*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
		for (iTmp = iaStartOffset[iaLoop]; iTmp < iaSize; iTmp++ )
	  {
	    keyData = (KEY_VALUE_POINT*)(pcDataBuf + iTmp * sizeof(KEY_VALUE_POINT));
	    
	    /*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
      valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
      pcKey[keyData->llKeyLen] = 0x00;
      
      /*�� VALUE �ļ��л�ȡ VALUE ��ֵ*/
      valueToBuf( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentFileId );
      pcValue[keyData->llValueLen] = 0x00;
  	        
      fprintf( unloadShare->fp, "\'%s\' \'%s\'\n", pcKey, pcValue );
      
      (unloadShare->llTotalRows)++;
	  }
	  
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iaLoop] * sizeof(KEY_POINT));
    
    iRet = managerUnloadDealGetDataLeft( unloadShare, key->llChildAddr, key->llChildSize, iaStartOffset, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
    if (iRet)
    {
    	return INTERRUPT;
    }
    
    for (iTmp = iaStartOffset[iaLoop] + 1; iTmp < iaSize; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerUnloadDealGetDataOther( unloadShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
    }
	}
	return SUCCESS;
}

/**********************************************************************
��������: managerUnloadDealGetDataOther
��������: ����˴��� unload �еĵ�������
��    ����
��    һ���̹߳������     I
��    �������ݵ�ַ         I
��    ������һ������       I
��    �ģ�����             I
��    �壺�ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161112
**********************************************************************/
int managerUnloadDealGetDataOther( UNLOAD_SHARE *unloadShare,  __int64 iaAddr, __int64 iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId )
{
	char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
	KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
	KEY_POINT *key;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iTmp;
	int  iRet;
	
	if (unloadShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}
	
	/*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
		for (iTmp = 0; iTmp < iaSize; iTmp++ )
	  {
	    keyData = (KEY_VALUE_POINT*)(pcDataBuf + iTmp * sizeof(KEY_VALUE_POINT));
	    
	    /*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
      valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
      pcKey[keyData->llKeyLen] = 0x00;
      
      /*�� VALUE �ļ��л�ȡ VALUE ��ֵ*/
      valueToBuf( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentFileId );
      pcValue[keyData->llValueLen] = 0x00;
      
      fprintf( unloadShare->fp, "\'%s\' \'%s\'\n", pcKey, pcValue );
      
      (unloadShare->llTotalRows)++;
	  }
	  
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
    for (iTmp = 0; iTmp < iaSize; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerUnloadDealGetDataOther( unloadShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
    }
	}
	return SUCCESS;
}

/**********************************************************************
��������: managerUnloadDealGetDataRight
��������: ����˴��� unload �еĵ�������
��    ����
��    һ���̹߳������     I
��    �������ݵ�ַ         I
��    ������һ������       I
��    �ģ���ʼλ��         I
��    �壺����             I
��    �����ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161112
**********************************************************************/
int managerUnloadDealGetDataRight( UNLOAD_SHARE *unloadShare, __int64 iaAddr, __int64 iaSize, int iaEndOffset[], int iaLoop, int iaTotalLoops, int iaIndepentFileId )
{
	char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
	KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
	KEY_POINT *key;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iTmp;
	int  iRet;
	
	if (unloadShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}
	
	/*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
	  memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
		for (iTmp = 0; iTmp <= iaEndOffset[iaLoop]; iTmp++ )
	  {
	    keyData = (KEY_VALUE_POINT*)(pcDataBuf + iTmp * sizeof(KEY_VALUE_POINT));
	      	
      /*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
      valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
      pcKey[keyData->llKeyLen] = 0x00;
      
      /*�� VALUE �ļ��л�ȡ VALUE ��ֵ*/
      valueToBuf( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentFileId );
      pcValue[keyData->llValueLen] = 0x00;
      
      fprintf( unloadShare->fp, "\'%s\' \'%s\'\n", pcKey, pcValue );
      
      (unloadShare->llTotalRows)++;
	  }
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
    for (iTmp = 0; iTmp < iaEndOffset[iaLoop]; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerUnloadDealGetDataOther( unloadShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
    }
    
    key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iaLoop] * sizeof(KEY_POINT));
    iRet = managerUnloadDealGetDataRight( unloadShare, key->llChildAddr, key->llChildSize, iaEndOffset, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
    if (iRet)
    {
    	return INTERRUPT;
    }
	}
	return SUCCESS;
}

/**********************************************************************
��������: managerUnloadDealUnloadDataMain
��������: ����˴��� unload �е����ݵ���
��    ����
��    һ���̹߳������     I/O
��    ������ʼλ��         I
��    ��������λ��         I
��    �ģ����ݵ�ַ         I
��    �壺����             I
��    ��������             I
��    �ߣ��ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161106
**********************************************************************/
int managerUnloadDealUnloadDataMain( UNLOAD_SHARE *unloadShare, int iaStartOffset[], int iaEndOffset[], __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId )
{
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	KEY_POINT *key;                /*�����������е����ݽṹ*/
	KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
  char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
  __int64 iAddr;
  int  iSize;
  int  iIndex;
  __int64 llTotalRows;
  int  iTmp;
  int  iTmp2;
  int  iTmp3;
  int  iRet;
  
  /*������ʼλ�úͽ���λ�ò�ͬ�ļ�*/
  for (iIndex = 1; iIndex <= iaTotalLoops; iIndex++)
  {
  	if (iaStartOffset[iIndex] < iaEndOffset[iIndex] )
  	{
  		break;
  	}
  }
    
  /*�������λ�ö���ͬ����ʾֻ��һ��������������*/
  if (iIndex > iaTotalLoops)
  {
  	/*ȡ��������*/
  	{
  	  /*ȡ����󼶵�����*/
      iAddr = iaAddr;
      for (iTmp2 = 1; iTmp2 < iaTotalLoops; iTmp2++)
      {
      	memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
        getKeyBlockDataMain( pcDataBuf, iAddr, iaIndepentId );
      
        key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iTmp2] * sizeof(KEY_POINT));
      
        iAddr = key->llChildAddr;
      }
      
      memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
      getKeyBlockDataMain( pcDataBuf, iAddr, iaIndepentId );
      keyData = (KEY_VALUE_POINT*)(pcDataBuf + iaStartOffset[iaTotalLoops] * sizeof(KEY_VALUE_POINT));
  	}
  	
  	/*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
    valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
    pcKey[keyData->llKeyLen] = 0x00;
    
    /*�� VALUE �ļ��л�ȡ VALUE ��ֵ*/
    valueToBufMain( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentId );
    pcValue[keyData->llValueLen] = 0x00;
  	
  	fprintf( unloadShare->fp, "\'%s\' \'%s\'\n", pcKey, pcValue );
  	
  	(unloadShare->llTotalRows)++;
  	return SUCCESS;
  }
    
    /*ȡ����ǰ��������*/
  iAddr = iaAddr;
  for (iTmp = 1; iTmp < iIndex; iTmp++)
  {
  	memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iAddr, iaIndepentId );
    
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iTmp] * sizeof(KEY_POINT));
    
    iAddr = key->llChildAddr;
  }

  memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
  getKeyBlockDataMain( pcDataBuf, iAddr, iaIndepentId );
      
  /*��ȡ����*/
  {
    /*����ʼλ�õļ�*/
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iIndex] * sizeof(KEY_POINT));
    iRet = managerUnloadDealGetDataLeftMain( unloadShare, key->llChildAddr, key->llChildSize, iaStartOffset, iIndex + 1, iaTotalLoops, iaIndepentId );
    /*������жϽ���*/
    if (iRet)
    {
    	return iRet;
    }
    
    /*��������λ�õļ�*/
    for (iTmp3 = iaStartOffset[iIndex] + 1; iTmp3 < iaEndOffset[iIndex]; iTmp3++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp3 * sizeof(KEY_POINT));
      iRet = managerUnloadDealGetDataOtherMain( unloadShare, key->llChildAddr, key->llChildSize, iIndex + 1, iaTotalLoops, iaIndepentId );
      if (iRet)
      {
    	  return iRet;
      }
    }
    
    /*�������λ�õļ�*/
    key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iIndex] * sizeof(KEY_POINT));
    iRet = managerUnloadDealGetDataRightMain( unloadShare, key->llChildAddr, key->llChildSize, iaEndOffset, iIndex + 1, iaTotalLoops, iaIndepentId );
    if (iRet)
    {
    	return iRet;
    }
  }
  
  return SUCCESS;
}

/**********************************************************************
��������: managerUnloadDealGetDataLeftMain
��������: ����˴��� unload �еĵ�������
��    ����
��    һ���̹߳������     I
��    �������ݵ�ַ         I
��    ������һ������       I
��    �ģ���ʼλ��         I
��    �壺����             I
��    �����ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161112
**********************************************************************/
int managerUnloadDealGetDataLeftMain( UNLOAD_SHARE *unloadShare, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], int iaLoop, int iaTotalLoops, int iaIndepentId )
{
	char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
	KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
	KEY_POINT *key;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iTmp;
	int  iRet;
	
	if (unloadShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}
	
	/*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
		for (iTmp = iaStartOffset[iaLoop]; iTmp < iaSize; iTmp++ )
	  {
	    keyData = (KEY_VALUE_POINT*)(pcDataBuf + iTmp * sizeof(KEY_VALUE_POINT));
	    
	    /*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
      valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
      pcKey[keyData->llKeyLen] = 0x00;
      
      /*�� VALUE �ļ��л�ȡ VALUE ��ֵ*/
      valueToBufMain( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentId );
      pcValue[keyData->llValueLen] = 0x00;
  	        
      fprintf( unloadShare->fp, "\'%s\' \'%s\'\n", pcKey, pcValue );
      
      (unloadShare->llTotalRows)++;
	  }
	  
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iaLoop] * sizeof(KEY_POINT));
    
    iRet = managerUnloadDealGetDataLeftMain( unloadShare, key->llChildAddr, key->llChildSize, iaStartOffset, iaLoop + 1, iaTotalLoops, iaIndepentId );
    if (iRet)
    {
    	return INTERRUPT;
    }
    
    for (iTmp = iaStartOffset[iaLoop] + 1; iTmp < iaSize; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerUnloadDealGetDataOtherMain( unloadShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
    }
	}
	return SUCCESS;
}

/**********************************************************************
��������: managerUnloadDealGetDataOtherMain
��������: ����˴��� unload �еĵ�������
��    ����
��    һ���̹߳������     I
��    �������ݵ�ַ         I
��    ������һ������       I
��    �ģ�����             I
��    �壺�ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161112
**********************************************************************/
int managerUnloadDealGetDataOtherMain( UNLOAD_SHARE *unloadShare,  __int64 iaAddr, __int64 iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId )
{
	char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
	KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
	KEY_POINT *key;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iTmp;
	int  iRet;
	
	if (unloadShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}
	
	/*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
		for (iTmp = 0; iTmp < iaSize; iTmp++ )
	  {
	    keyData = (KEY_VALUE_POINT*)(pcDataBuf + iTmp * sizeof(KEY_VALUE_POINT));
	    
	    /*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
      valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
      pcKey[keyData->llKeyLen] = 0x00;
      
      /*�� VALUE �ļ��л�ȡ VALUE ��ֵ*/
      valueToBufMain( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentId );
      pcValue[keyData->llValueLen] = 0x00;
      
      fprintf( unloadShare->fp, "\'%s\' \'%s\'\n", pcKey, pcValue );
      
      (unloadShare->llTotalRows)++;
	  }
	  
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    for (iTmp = 0; iTmp < iaSize; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerUnloadDealGetDataOtherMain( unloadShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
    }
	}
	return SUCCESS;
}

/**********************************************************************
��������: managerUnloadDealGetDataRightMain
��������: ����˴��� unload �еĵ�������
��    ����
��    һ���̹߳������     I
��    �������ݵ�ַ         I
��    ������һ������       I
��    �ģ���ʼλ��         I
��    �壺����             I
��    �����ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161112
**********************************************************************/
int managerUnloadDealGetDataRightMain( UNLOAD_SHARE *unloadShare, __int64 iaAddr, __int64 iaSize, int iaEndOffset[], int iaLoop, int iaTotalLoops, int iaIndepentId )
{
	char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
	KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
	KEY_POINT *key;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iTmp;
	int  iRet;
	
	if (unloadShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}
	
	/*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
	  memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
		for (iTmp = 0; iTmp <= iaEndOffset[iaLoop]; iTmp++ )
	  {
	    keyData = (KEY_VALUE_POINT*)(pcDataBuf + iTmp * sizeof(KEY_VALUE_POINT));
	      	
      /*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
      valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
      pcKey[keyData->llKeyLen] = 0x00;
      
      /*�� VALUE �ļ��л�ȡ VALUE ��ֵ*/
      valueToBufMain( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentId );
      pcValue[keyData->llValueLen] = 0x00;
      
      fprintf( unloadShare->fp, "\'%s\' \'%s\'\n", pcKey, pcValue );
      
      (unloadShare->llTotalRows)++;
	  }
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    for (iTmp = 0; iTmp < iaEndOffset[iaLoop]; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerUnloadDealGetDataOtherMain( unloadShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
    }
    
    key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iaLoop] * sizeof(KEY_POINT));
    iRet = managerUnloadDealGetDataRightMain( unloadShare, key->llChildAddr, key->llChildSize, iaEndOffset, iaLoop + 1, iaTotalLoops, iaIndepentId );
    if (iRet)
    {
    	return INTERRUPT;
    }
	}
	return SUCCESS;
}

/**********************************************************************
��������: managerLoadDeal
��������: ����˴��� load ����
��    ����
��    һ��SOCKET               I
��    �������ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20161022
**********************************************************************/
void managerLoadDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd )
{
/*
�������ݽṹ������+����+�ļ�·��+�û���+����
�������ݽṹ���������+��������������+��������+��ͬkey����+ʧ������
*/
#define MANAGER_LOAD_DEAL_SEND_ERROR( value ) \
{\
	memset( pcResult, 0x00, sizeof(pcResult) );\
	sprintf( pcResult, "%d", value ); \
	cResult = pcResult[0];\
	\
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );\
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, 1 );\
  \
  send( iaSocket, pcSendLen, DATA_BUF_LEN,0 );\
	send( iaSocket, &cResult, 1, 0 );\
}
	
	char pcResult[INT_LEN + 1];
	char cResult;
	char pcFilePath[FILE_PATH_LEN + 1];
	char pcUser[NAME_LEN + 1];
	char pcPass[NAME_LEN + 1];
	char pcKeysTotal[INT64_LEN + 1];
	char pcKeysLoaded[INT64_LEN + 1];
	char pcKeysUpdate[INT64_LEN + 1];
	char pcKeysFailed[INT64_LEN + 1];
	char pcSendBuf[1 + INT64_LEN + INT64_LEN + INT64_LEN + INT64_LEN + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	char pcRecvLen[DATA_BUF_LEN + 1];
	FILE *fp;
	__int64 llKeyValueSize;
	__int64 llKeysTotal;
	__int64 llKeysLoaded;
	__int64 llKeysUpdate;
	__int64 llKeysFailed;
	__int64 llSize;
	int  iKeyLen;
	int  iValueLen;
	int  iSendLen;
	int  iRet;
	int  iTmp;
	char *pcBufStart;
	char *pcBufEnd;
	LOAD_SHARE loadStruct;
	char pcOperate[OPERATE_LEN + 1];
	int iRecvLen;
	int iTotalLen;
	int iDataLen;
	int iTime;
	#ifdef SYSTEM_WINDOWS_64
	  void *threadHandle;                /*windows�µ��߳̾��*/
	  unsigned long ulWindowsThreadId;
	#else/*#ifdef SYSTEM_UBUNTU*/
	  pthread_t threadId;                /*����OS�µ��߳�ID*/
	#endif
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcBufEnd;
	
	/*ȡ�ļ�·��*/
	memset( pcFilePath, 0x00, sizeof(pcFilePath) );
	memcpy( pcFilePath, pcBufStart, NAME_LEN );
	AllTrim( pcFilePath );
	pcBufStart += FILE_PATH_LEN;
	
	/*ȡ�û���*/
	memset( pcUser, 0x00, sizeof(pcUser) );
	memcpy( pcUser, pcBufStart, NAME_LEN );
	AllTrim( pcUser );
	pcBufStart += NAME_LEN;
	
	/*ȡ����*/
	memset( pcPass, 0x00, sizeof(pcPass) );
	memcpy( pcPass, pcBufStart, NAME_LEN );
	AllTrim( pcPass );
	pcBufStart += NAME_LEN;
	
	MANAGER_WRLOCK();
	
	/*ȡ�õ�ǰkey������*/  
  llKeyValueSize = 0;
  for (iTmp = 0; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
  {
  	llKeyValueSize += sgBase[iTmp].llKeyValueSize;
  }
	
	MANAGER_WRUNLOCK();
	
	/*����û����������Ƿ�ƥ��*/
	if (strcmp( pcgUser, pcUser ) || strcmp( pcgPass, pcPass ))
	{
		MANAGER_LOAD_DEAL_SEND_ERROR( ERROR );
		MANAGER_WRUNLOCK();
		return;
	}
	
	/*���ļ�*/
	fp = fopen64( pcFilePath, "rb" );
	if (fp == NULL)
	{
		MANAGER_LOAD_DEAL_SEND_ERROR( ERROR );
		return;
	}
	
	/*���������̹߳�������*/
	memset( &loadStruct, 0x00, sizeof(loadStruct) );
	loadStruct.fp = fp;                        /*�ļ�ָ��*/
	loadStruct.iState = RUN_STATE_UNFINISHED;  /*���߳�����״̬*/
	loadStruct.iCmd = RUN_CMD_RUN;             /*���߳���������*/
	
	#ifdef SYSTEM_WINDOWS_64
    threadHandle = CreateThread( NULL, 0, (void *)(&managerLoadDealChildThread), &loadStruct, 0, &ulWindowsThreadId );
    if (sgManagerThread.threadHandle == NULL)
    {
    	MANAGER_SELECT_COUNT_DEAL_SEND_ERROR( ERROR );
    	MANAGER_WRUNLOCK();
      return;
    }
  #else/*#ifdef SYSTEM_UBUNTU*/
    if (pthread_create(&threadId, NULL, (void *)(&managerLoadDealChildThread), &loadStruct) != 0)
    {
  	  MANAGER_SELECT_COUNT_DEAL_SEND_ERROR( ERROR );
  	  MANAGER_WRUNLOCK();
      return;
    }
  
    /*�̷߳��룬����ʱ�Զ�������Դ*/
    pthread_detach(threadId);
  #endif
  
  /*ѭ���ȴ�������߼���߳�ִ�����*/
  iTime = 0;
	while (iTime < EACH_JOB_TIME * 10) /*EACH_JOB_TIME �ĵ�λ���룬ÿ��˯��0.1��*/
	{		
		/*������߳�����ɣ���ʾ���߳�����ִ����ɣ����˳�*/
		if (loadStruct.iState == RUN_STATE_FINISHED)
		{
			break;
		}
		else
		{
			usleep( 100*1000 );
		}
		iTime++;
	}
	
	/*�����̷߳�����ֹ����*/
	loadStruct.iCmd = RUN_CMD_EXIT;
	
	/*�ȴ����߳�ִ�н���*/
	while (loadStruct.iState != RUN_STATE_FINISHED)
	{
		THREAD_THREAD_SLEEP();
	}
	
	fclose(fp);
	
	llKeysTotal = loadStruct.llTotalRows;
	llKeysFailed = loadStruct.llTotalFail;
	
	/*���㵼������*/
	llSize = 0;
  for (iTmp = 0; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
  {
  	llSize += sgBase[iTmp].llKeyValueSize;
  }
	llKeysLoaded = llSize - llKeyValueSize;
	
	/*�����������*/
	llKeysUpdate = llKeysTotal - llKeysLoaded - llKeysFailed;
	
	MANAGER_WRUNLOCK();
	
	memset( pcKeysTotal, 0x00, sizeof(pcKeysTotal) );
  sprintf( pcKeysTotal, "%0*lld", INT64_LEN, llKeysTotal );
  
  memset( pcKeysLoaded, 0x00, sizeof(pcKeysLoaded) );
  sprintf( pcKeysLoaded, "%0*lld", INT64_LEN, llKeysLoaded );
  
  memset( pcKeysUpdate, 0x00, sizeof(pcKeysUpdate) );
  sprintf( pcKeysUpdate, "%0*lld", INT64_LEN, llKeysUpdate );
  
  memset( pcKeysFailed, 0x00, sizeof(pcKeysFailed) );
  sprintf( pcKeysFailed, "%0*lld", INT64_LEN, llKeysFailed );
  
  /*�鷢������*/
  memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
  
  sprintf( &cResult, "%d", SUCCESS );
  memcpy( pcSendBuf, &cResult, 1 );
  memcpy( pcSendBuf + 1, pcKeysTotal, INT64_LEN );
  memcpy( pcSendBuf + 1 + INT64_LEN, pcKeysLoaded, INT64_LEN );
  memcpy( pcSendBuf + 1 + INT64_LEN + INT64_LEN, pcKeysUpdate, INT64_LEN );
  memcpy( pcSendBuf + 1 + INT64_LEN + INT64_LEN + INT64_LEN, pcKeysFailed, INT64_LEN );
  
  iSendLen = 1 + INT64_LEN + INT64_LEN + INT64_LEN + INT64_LEN;
  
  memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
  
  send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
  
  send( iaSocket, pcSendBuf, iSendLen, 0 );
  
  return;
}

/**********************************************************************
��������: managerLoadDeal
��������: ����˴��� load ����
��    ����
��    һ������ṹ��               I/O
��    ��: �����
����ʱ��: 20161030
**********************************************************************/
void managerLoadDealChildThread( LOAD_SHARE *loadShare )
{
	FILE *fp;
	char pcLines[MAX_LINE_BUF];   /*ÿ�е����ֵ*/
	char pcKey[MAX_KEY_LEN + 1];
	char pcValue[MAX_VALUE_LEN + 1];
	char *pcComma;
	int  iKeyLen;
	int  iValueLen;
	char *pcBufStart;
	char *pcBufEnd;
	__int64 llKeysTotal;
	__int64 llKeysFailed;
	int  iIndepentId;
	
	
	fp = loadShare->fp;
	llKeysTotal = 0;
	llKeysFailed = 0;
	
	/*ѭ����������*/
	while (loadShare->iCmd == RUN_CMD_RUN)
	{
	  memset( pcLines, 0x00, sizeof(pcLines) );
    if (fgets( pcLines, sizeof(pcLines), fp ) == NULL)
    {
      break;
    }
    AllTrim( pcLines );
    if (strlen(pcLines) == 0)
    {
    	continue;
    }
    
    llKeysTotal++;
    
    pcBufStart = pcLines;
    pcBufEnd = pcLines + strlen(pcLines);
    
    /*����KEY*/
    {
      /*���Կ�ʼ�ո�*/
      IGNORE_BLANK_START( pcBufStart, pcBufEnd );
      
      /*����ո�����һ���ַ����ǵ�����*/
      if (*(pcBufStart) != '\'')
      {
      	llKeysFailed++;
      	continue;
      }
      
      /*Ѱ����һ��������*/
      pcComma = strchr( pcBufStart + 1, '\'' );
      if (pcComma == NULL || pcComma > pcBufEnd)
      {
      	llKeysFailed++;
      	continue;
      }
      
      /*���KEYֵ����*/
      iKeyLen = pcComma - (pcBufStart + 1);
      if (iKeyLen > MAX_KEY_LEN)
      {
      	llKeysFailed++;
      	continue;
      }
      	
      /*ȡKEY*/
      memset( pcKey, 0x00, sizeof(pcKey) );
      memcpy( pcKey, pcBufStart + 1, pcComma - (pcBufStart + 1) );
    }
    pcBufStart = pcComma + 1;
    
    if (pcBufStart >= pcBufEnd)
    {
    	llKeysFailed++;
      continue;
    }
    
    /*����VALUE*/
    {
      /*���Կ�ʼ�ո�*/
      IGNORE_BLANK_START( pcBufStart, pcBufEnd );
      if (pcBufStart == pcBufEnd)
      {
    	  llKeysFailed++;
        continue;
      }
      
      /*����ո�����һ���ַ����ǵ�����*/
      if (*(pcBufStart) != '\'')
      {
      	llKeysFailed++;
      	continue;
      }
      
      /*Ѱ����һ��������*/
      pcComma = strchr( pcBufStart + 1, '\'' );
      if (pcComma == NULL || pcComma > pcBufEnd)
      {
      	llKeysFailed++;
      	continue;
      }
      
      /*���VALUEֵ����*/
      iValueLen = pcComma - (pcBufStart + 1);
      if (iValueLen > MAX_VALUE_LEN)
      {
      	llKeysFailed++;
      	continue;
      }
      	
      /*ȡKEY*/
      memset( pcValue, 0x00, sizeof(pcValue) );
      memcpy( pcValue, pcBufStart + 1, pcComma - (pcBufStart + 1) );
    }
    pcBufStart = pcComma + 1;
    
    /*���Կ�ʼ�ո�*/
    IGNORE_BLANK_START( pcBufStart, pcBufEnd );
    if (pcBufStart != pcBufEnd)
    {
      llKeysFailed++;
      continue;
    }
        
    /*��KEY����*/
    iIndepentId = calculateKeyMain( pcKey, iKeyLen );
    if (updateNodeMain( pcKey, (__int64)(iKeyLen), pcValue, (__int64)(iValueLen), iIndepentId ) != SUCCESS)
    {
    	llKeysFailed++;
      continue;
    }
	}
	
	loadShare->llTotalRows = llKeysTotal;
	loadShare->llTotalFail = llKeysFailed;
	loadShare->iState = RUN_STATE_FINISHED;
	return;
}

