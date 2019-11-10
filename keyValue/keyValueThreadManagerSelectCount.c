#define __KEY_VALUE_THREAD_MANAGER_SELECT_COUNT_C

/*
������ MANAGER �е� SELECT ���
*/

#ifndef __COMMON_HEAD_H
  #include "commonHead.h"
#endif

int  managerPositionLeftOpen( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId );
int  managerPositionLeftClose( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId );
int  managerPositionRightOpen( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId );
int  managerPositionRightClose( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId );
int  managerPositionLeftOpenMain( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId );
int  managerPositionLeftCloseMain( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId );
int  managerPositionRightOpenMain( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId );
int  managerPositionRightCloseMain( int iaOutResult[], char *pcaKey, __int64 iaAddr, int iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId );

void managerSelectCountDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd );
void managerSelectCountDealChildThread( SELECT_COUNT_SHARE *selectCountShare );
int  managerSelectCountDealResult( SELECT_COUNT_SHARE *selectCountShare, int iaStartOffset[], int iaEndOffset[], __int64 iaAddr, __int64 iaTotalLoops, int iaIndepentFileId );
int  managerSelectCountDealResultLeft( SELECT_COUNT_SHARE *selectCountShare, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId );
int  managerSelectCountDealResultOther( SELECT_COUNT_SHARE *selectCountShare,  __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId );
int  managerSelectCountDealResultRight( SELECT_COUNT_SHARE *selectCountShare, __int64 iaAddr, __int64 iaSize, int iaEndOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId );
int  managerSelectCountDealResultMain( SELECT_COUNT_SHARE *selectCountShare, int iaStartOffset[], int iaEndOffset[], __int64 iaAddr, __int64 iaTotalLoops, int iaIndepentId );
int  managerSelectCountDealResultLeftMain( SELECT_COUNT_SHARE *selectCountShare, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId );
int  managerSelectCountDealResultOtherMain( SELECT_COUNT_SHARE *selectCountShare,  __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId );
int  managerSelectCountDealResultRightMain( SELECT_COUNT_SHARE *selectCountShare, __int64 iaAddr, __int64 iaSize, int iaEndOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId );

/**********************************************************************
��������: managerSelectCountDeal
��������: ����˴��� select count ����
��    ����
��    һ��SOCKET               I
��    �������ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20161022
**********************************************************************/
void managerSelectCountDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd )
{
/*
�������ݽṹ������+����+ֵ���䣨�Ƿ������������ҿ������ұա�����ҿ�������ұգ�+KEY��ʼֵ����+KEY��ʼֵ+KEY����ֵ����+KEY����ֵ
�������ݽṹ���������+COUNTֵ
*/
#define MANAGER_SELECT_COUNT_DEAL_SEND_ERROR( value ) \
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
	char pcKeyStartLen[KEY_VALUE_LENGTH + 1];
  char pcKeyEndLen[KEY_VALUE_LENGTH + 1];
  char pcKeyStart[MAX_KEY_LEN + 1];
  char pcKeyEnd[MAX_KEY_LEN + 1];
  char pcValueType[KEY_VALUE_LENGTH + 1];
  char pcSendLen[DATA_BUF_LEN + 1];
  char pcRecvLen[DATA_BUF_LEN + 1];
  int  iKeyStartLen;
	int  iKeyEndLen;
	int  iRet;
	char *pcBufStart;
	char *pcBufEnd;
	SELECT_COUNT_SHARE selectStruct;
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
	pcBufEnd = pcaBufEnd;
	
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
	
	//logFmtSyn( LVNOR, "pcKeyStart[%s]", pcKeyStart );
	
	/*ȡKEY����ֵ����*/
	memset( pcKeyEndLen, 0x00, sizeof(pcKeyEndLen) );
	memcpy( pcKeyEndLen, pcBufStart, KEY_VALUE_LENGTH );
	pcBufStart += KEY_VALUE_LENGTH;
	
	//logFmtSyn( LVNOR, "pcKeyEndLen[%s]", pcKeyEndLen );
	
	/*ȡKEY����ֵ*/
	iKeyEndLen = atoi( pcKeyEndLen );
	memset( pcKeyEnd, 0x00, sizeof(pcKeyEnd) );
	memcpy( pcKeyEnd, pcBufStart, iKeyEndLen );
	pcBufStart += iKeyEndLen;
	
	//logFmtSyn( LVNOR, "pcKeyEnd[%s]", pcKeyEnd );
	
	/*���������̹߳�������*/
	memset( &selectStruct, 0x00, sizeof(selectStruct) );
	memcpy( selectStruct.pcValueType, pcValueType, KEY_VALUE_LENGTH ); /*����������ֵ���䣨KEY��VALUE ���ҿ������ұա�����ҿ�������ұ�*/
	memcpy( selectStruct.pcKeyStart, pcKeyStart, iKeyStartLen );       /*KEY �Ŀ�ʼֵ*/
	selectStruct.iKeyStartLen = iKeyStartLen;                          /*KEY �Ŀ�ʼֵ����*/
	memcpy( selectStruct.pcKeyEnd, pcKeyEnd, iKeyEndLen );             /*KEY �Ľ���ֵ*/
	selectStruct.iKeyEndLen = iKeyEndLen;                              /*KEY �Ľ���ֵ����*/
	selectStruct.iState = RUN_STATE_UNFINISHED;                        /*���߳�����״̬*/
	selectStruct.iCmd = RUN_CMD_RUN;                                   /*���߳���������*/
	
	MANAGER_RDLOCK();

	#ifdef SYSTEM_WINDOWS_64
    threadHandle = CreateThread( NULL, 0, (void *)(&managerSelectCountDealChildThread), &selectStruct, 0, &ulWindowsThreadId );
    if (sgManagerThread.threadHandle == NULL)
    {
    	MANAGER_SELECT_COUNT_DEAL_SEND_ERROR( ERROR );
    	MANAGER_RDUNLOCK();
      return;
    }
  #else/*#ifdef SYSTEM_UBUNTU*/
    if (pthread_create(&threadId, NULL, (void *)(&managerSelectCountDealChildThread), &selectStruct) != 0)
    {
  	  MANAGER_SELECT_COUNT_DEAL_SEND_ERROR( ERROR );
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
		//logFmtSyn( LVNOR, "selectStruct.iState[%d]", selectStruct.iState );
		
		/*������߳�����ɣ���ʾ���߳�����ִ����ɣ����˳�*/
		if (selectStruct.iState == RUN_STATE_FINISHED)
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
	selectStruct.iCmd = RUN_CMD_EXIT;
	
	/*�ȴ����߳�ִ�н���*/
	while (selectStruct.iState != RUN_STATE_FINISHED)
	{
		THREAD_THREAD_SLEEP();
	}
	
	MANAGER_RDUNLOCK();
	
	memset( pcTotalRows, 0x00, sizeof(pcTotalRows) );
  sprintf( pcTotalRows, "%0*lld", INT64_LEN, selectStruct.llTotalRows );
  
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
  
  logFmtSyn( LVNOR, "�������ݳɹ�[%s][%s]", pcSendLen, pcSendBuf );
}

/**********************************************************************
��������: managerSelectCountDealChildThread
��������: ����˴��� select count ����
��    ����
��    һ������ṹ��               I/O
��    ��: �����
����ʱ��: 20161106
**********************************************************************/
void managerSelectCountDealChildThread( SELECT_COUNT_SHARE *selectCountShare )
{
  int  iStartOffsetRecord[20];   /*��ʼλ�ü�¼*/
	int  iEndOffsetRecord[20];     /*����λ�ü�¼*/
	__int64  llDisc;
	__int64  llSize;
	__int64  llLoops;
	int  iRet;
	int  iIndepentId;
	int  iIndepentFileId;
		
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
	    if (!strcmp( selectCountShare->pcValueType, KEY_OPEN_OPEN ) != !strcmp( selectCountShare->pcValueType, KEY_OPEN_CLOSE ))
      {
    	  iRet = managerPositionLeftOpenMain( iStartOffsetRecord, selectCountShare->pcKeyStart, llDisc, llSize, 1, llLoops, iIndepentId );

    	  /*���û����������*/
    	  if (iRet == NOT_FOUND)
    	  {
    	  	//logFmtSyn( LVNOR, "û��������λ�õ�����" );
          continue ;
    	  }
      }
      else
      {
    	  iRet = managerPositionLeftCloseMain( iStartOffsetRecord, selectCountShare->pcKeyStart, llDisc, llSize, 1, llLoops, iIndepentId );

    	  /*���û����������*/
    	  if (iRet == NOT_FOUND)
    	  {
    	  	//logFmtSyn( LVNOR, "û��������λ�õ�����" );
          continue ;
    	  }
      }
	  }

	  /*��λ����λ��*/
	  {
	  	/*��ʼ������λ�ü�¼����*/
	    memset( &iEndOffsetRecord, 0x00, sizeof(iEndOffsetRecord) );
	  
	  	/*������ҿ�*/
	    if (!strcmp( selectCountShare->pcValueType, KEY_OPEN_OPEN ) != !strcmp( selectCountShare->pcValueType, KEY_CLOSE_OPEN ))
      {
    	  iRet = managerPositionRightOpenMain( iEndOffsetRecord, selectCountShare->pcKeyEnd, llDisc, llSize, 1, llLoops, iIndepentId );
    	  
    	  /*���û����������*/
    	  if (iRet == NOT_FOUND)
    	  {
    	  	
    	  	//logFmtSyn( LVNOR, "û��������λ�õ�����" );
          continue ;
    	  }
      }
      else
      {
    	  iRet = managerPositionRightCloseMain( iEndOffsetRecord, selectCountShare->pcKeyEnd, llDisc, llSize, 1, llLoops, iIndepentId );
    	  
    	  /*���û����������*/
    	  if (iRet == NOT_FOUND)
    	  {
    	  	//logFmtSyn( LVNOR, "û��������λ�õ�����" );
          continue ;
    	  }
      }
	  }

	  /*��������*/
	  iRet = managerSelectCountDealResultMain( selectCountShare, iStartOffsetRecord, iEndOffsetRecord, llDisc, llLoops, iIndepentId );
	  
	  if (iRet == INTERRUPT)
	  {
	  	break;
	  }
  }

	for (iIndepentFileId = 0; iIndepentFileId < INDEPENDENT_FILE_SIZE; iIndepentFileId++)
	{
		/*�������������*/
		if (iRet == INTERRUPT)
	  {
	  	break;
	  }
	  llDisc = sgBase[iIndepentFileId].llFirstDisc;
	  llSize = sgBase[iIndepentFileId].llFirstSize;
	  llLoops = sgBase[iIndepentFileId].llLoops;

	  /*��λ��ʼλ��*/
	  {
	  	/*��ʼ����ʼλ�ü�¼����*/
	    memset( &iStartOffsetRecord, 0x00, sizeof(iStartOffsetRecord) );
	  
	  	/*�������*/
	    if (!strcmp( selectCountShare->pcValueType, KEY_OPEN_OPEN ) != !strcmp( selectCountShare->pcValueType, KEY_OPEN_CLOSE ))
      {
    	  iRet = managerPositionLeftOpen( iStartOffsetRecord, selectCountShare->pcKeyStart, llDisc, llSize, 1, llLoops, iIndepentFileId );
    	  
    	  /*���û����������*/
    	  if (iRet == NOT_FOUND)
    	  {
    	  	//logFmtSyn( LVNOR, "û��������λ�õ�����" );
          continue ;
    	  }
      }
      else
      {
    	  iRet = managerPositionLeftClose( iStartOffsetRecord, selectCountShare->pcKeyStart, llDisc, llSize, 1, llLoops, iIndepentFileId );
    	  
    	  /*���û����������*/
    	  if (iRet == NOT_FOUND)
    	  {
    	  	//logFmtSyn( LVNOR, "û��������λ�õ�����" );
          continue ;
    	  }
      }
	  }

	  /*��λ����λ��*/
	  {
	  	/*��ʼ������λ�ü�¼����*/
	    memset( &iEndOffsetRecord, 0x00, sizeof(iEndOffsetRecord) );
	  
	  	/*������ҿ�*/
	    if (!strcmp( selectCountShare->pcValueType, KEY_OPEN_OPEN ) != !strcmp( selectCountShare->pcValueType, KEY_CLOSE_OPEN ))
      {
    	  iRet = managerPositionRightOpen( iEndOffsetRecord, selectCountShare->pcKeyEnd, llDisc, llSize, 1, llLoops, iIndepentFileId );
    	  
    	  /*���û����������*/
    	  if (iRet == NOT_FOUND)
    	  {
    	  	
    	  	//logFmtSyn( LVNOR, "û��������λ�õ�����" );
          continue ;
    	  }
      }
      else
      {
    	  iRet = managerPositionRightClose( iEndOffsetRecord, selectCountShare->pcKeyEnd, llDisc, llSize, 1, llLoops, iIndepentFileId );
    	  
    	  /*���û����������*/
    	  if (iRet == NOT_FOUND)
    	  {
    	  	//logFmtSyn( LVNOR, "û��������λ�õ�����" );
          continue ;
    	  }
      }
	  }

	  /*��������*/
	  iRet = managerSelectCountDealResult( selectCountShare, iStartOffsetRecord, iEndOffsetRecord, llDisc, llLoops, iIndepentFileId );
  }
	selectCountShare->iState = RUN_STATE_FINISHED;
	return;
}

/**********************************************************************
��������: managerSelectCountDealResultOther
��������: ����˴��� select count �е���������
��    ����
��    һ���̹߳������     I/O
��    ������ʼλ��         I
��    ��������λ��         I
��    �ģ����ݵ�ַ         I
��    �壺�ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161106
**********************************************************************/
int managerSelectCountDealResult( SELECT_COUNT_SHARE *selectCountShare, int iaStartOffset[], int iaEndOffset[], __int64 iaAddr, __int64 iaTotalLoops, int iaIndepentFileId )
{
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	KEY_POINT *key;                /*�����������е����ݽṹ*/
  char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
  __int64 iAddr;
  int  iSize;
  int  iIndex;
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
  if (iIndex >= iaTotalLoops)
  {
  	selectCountShare->llTotalRows += 1;
  	return SUCCESS;
  }
  
  /*���ֻ�����һ�����ݲ�ͬ������������������*/
  if (iIndex == iaTotalLoops)
  {
  	selectCountShare->llTotalRows += iaEndOffset[iIndex] - iaStartOffset[iIndex] + 1;
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

  /*�Ӹü���ʼ����ʼλ�úͽ���λ�ò�ͬ*/

  /*����ʼλ�õļ�*/
  key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iIndex] * sizeof(KEY_POINT));
  iRet = managerSelectCountDealResultLeft( selectCountShare, key->llChildAddr, key->llChildSize, iaStartOffset, iIndex + 1, iaTotalLoops, iaIndepentFileId );
  /*������жϽ���*/
  if (iRet)
  {
  	return iRet;
  }

  /*��������λ�õļ�*/
  for (iTmp3 = iaStartOffset[iIndex] + 1; iTmp3 < iaEndOffset[iIndex]; iTmp3++)
  {
    key = (KEY_POINT*)(pcDataBuf + iTmp3 * sizeof(KEY_POINT));
    iRet = managerSelectCountDealResultOther( selectCountShare, key->llChildAddr, key->llChildSize, iIndex + 1, iaTotalLoops, iaIndepentFileId );
    if (iRet)
    {
  	  return iRet;
    }
  }

  /*�������λ�õļ�*/
  key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iIndex] * sizeof(KEY_POINT));
  iRet = managerSelectCountDealResultRight( selectCountShare, key->llChildAddr, key->llChildSize, iaEndOffset, iIndex + 1, iaTotalLoops, iaIndepentFileId );

  return SUCCESS;
}

/**********************************************************************
��������: managerSelectCountDealResultLeft
��������: ����˴��� select count �е���������
��    ����
��    һ���̹߳������     I
��    �������ݵ�ַ         I
��    ������һ������       I
��    �ģ���ʼλ��         I
��    �壺����             I
��    �����ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161106
**********************************************************************/
int managerSelectCountDealResultLeft( SELECT_COUNT_SHARE *selectCountShare, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId )
{
	char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
	KEY_POINT *key;
	int  iTmp;
	int  iRet;

	if (selectCountShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}

	/*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
		selectCountShare->llTotalRows += iaSize - (__int64)iaStartOffset[iaLoop];
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iaLoop] * sizeof(KEY_POINT));
    iRet = managerSelectCountDealResultLeft( selectCountShare, key->llChildAddr, key->llChildSize, iaStartOffset, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
    if (iRet)
    {
    	return INTERRUPT;
    }

    for (iTmp = iaStartOffset[iaLoop] + 1; iTmp < iaSize; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerSelectCountDealResultOther( selectCountShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
    }
	}

	return SUCCESS;
}

/**********************************************************************
��������: managerSelectCountDealResultOther
��������: ����˴��� select count �е���������
��    ����
��    һ���̹߳������     I
��    �������ݵ�ַ         I
��    ������һ������       I
��    �ģ�����             I
��    �壺�ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161106
**********************************************************************/
int managerSelectCountDealResultOther( SELECT_COUNT_SHARE *selectCountShare,  __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId )
{
	char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
	KEY_POINT *key;
	int  iTmp;
	int  iRet;
	
	if (selectCountShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}

	/*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
		selectCountShare->llTotalRows += iaSize;
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
    for (iTmp = 0; iTmp < iaSize; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerSelectCountDealResultOther( selectCountShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
    }
	}
	return SUCCESS;
}

/**********************************************************************
��������: managerSelectCountDealResultRight
��������: ����˴��� select count �е���������
��    ����
��    һ���̹߳������     I
��    �������ݵ�ַ         I
��    ������һ������       I
��    �ģ���ʼλ��         I
��    �壺����             I
��    �����ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161106
**********************************************************************/
int managerSelectCountDealResultRight( SELECT_COUNT_SHARE *selectCountShare, __int64 iaAddr, __int64 iaSize, int iaEndOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId )
{
	char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
	KEY_POINT *key;
	int  iTmp;
	int  iRet;
	
	//logFmtSyn( LVNOR, "��ʼִ�к���[%s]", __FUNCTION__ );
	
	if (selectCountShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}
	
	/*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
		selectCountShare->llTotalRows += iaEndOffset[iaLoop] + 1;
	  return SUCCESS;
	}
	
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iaLoop] * sizeof(KEY_POINT));
    
    iRet = managerSelectCountDealResultRight( selectCountShare, key->llChildAddr, key->llChildSize, iaEndOffset, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
    if (iRet)
    {
    	return INTERRUPT;
    }
    
    for (iTmp = 0; iTmp < iaEndOffset[iaLoop]; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerSelectCountDealResultOther( selectCountShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
    }
	}
	
	return SUCCESS;
}

/**********************************************************************
��������: managerSelectCountDealResultMain
��������: ����˴��� select count �е���������
��    ����
��    һ���̹߳������     I/O
��    ������ʼλ��         I
��    ��������λ��         I
��    �ģ����ݵ�ַ         I
��    �壺�ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161106
**********************************************************************/
int managerSelectCountDealResultMain( SELECT_COUNT_SHARE *selectCountShare, int iaStartOffset[], int iaEndOffset[], __int64 iaAddr, __int64 iaTotalLoops, int iaIndepentId )
{
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	KEY_POINT *key;                /*�����������е����ݽṹ*/
  char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
  __int64 iAddr;
  int  iSize;
  int  iIndex;
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
  	selectCountShare->llTotalRows += 1;
  	return SUCCESS;
  }
  
  /*���ֻ�����һ�����ݲ�ͬ������������������*/
  if (iIndex == iaTotalLoops)
  {
  	selectCountShare->llTotalRows += iaEndOffset[iIndex] - iaStartOffset[iIndex] + 1;
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
  
  /*�Ӹü���ʼ����ʼλ�úͽ���λ�ò�ͬ*/

  /*����ʼλ�õļ�*/
  key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iIndex] * sizeof(KEY_POINT));
  iRet = managerSelectCountDealResultLeftMain( selectCountShare, key->llChildAddr, key->llChildSize, iaStartOffset, iIndex + 1, iaTotalLoops, iaIndepentId );
  /*������жϽ���*/
  if (iRet)
  {
  	return iRet;
  }
  
  /*��������λ�õļ�*/
  for (iTmp3 = iaStartOffset[iIndex] + 1; iTmp3 < iaEndOffset[iIndex]; iTmp3++)
  {
    key = (KEY_POINT*)(pcDataBuf + iTmp3 * sizeof(KEY_POINT));
    iRet = managerSelectCountDealResultOtherMain( selectCountShare, key->llChildAddr, key->llChildSize, iIndex + 1, iaTotalLoops, iaIndepentId );
    if (iRet)
    {
  	  return iRet;
    }
  }
  
  /*�������λ�õļ�*/
  key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iIndex] * sizeof(KEY_POINT));
  iRet = managerSelectCountDealResultRightMain( selectCountShare, key->llChildAddr, key->llChildSize, iaEndOffset, iIndex + 1, iaTotalLoops, iaIndepentId );
  
  return SUCCESS;
}

/**********************************************************************
��������: managerSelectCountDealResultLeftMain
��������: ����˴��� select count �е���������
��    ����
��    һ���̹߳������     I
��    �������ݵ�ַ         I
��    ������һ������       I
��    �ģ���ʼλ��         I
��    �壺����             I
��    �����ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161106
**********************************************************************/
int managerSelectCountDealResultLeftMain( SELECT_COUNT_SHARE *selectCountShare, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId )
{
	char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
	KEY_POINT *key;
	int  iTmp;
	int  iRet;
	
	if (selectCountShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}
	
	/*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
		selectCountShare->llTotalRows += iaSize - (__int64)iaStartOffset[iaLoop];
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iaLoop] * sizeof(KEY_POINT));
    
    iRet = managerSelectCountDealResultLeftMain( selectCountShare, key->llChildAddr, key->llChildSize, iaStartOffset, iaLoop + 1, iaTotalLoops, iaIndepentId );
    if (iRet)
    {
    	return INTERRUPT;
    }
    
    for (iTmp = iaStartOffset[iaLoop] + 1; iTmp < iaSize; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerSelectCountDealResultOtherMain( selectCountShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
    }
	}
	
	return SUCCESS;
}

/**********************************************************************
��������: managerSelectCountDealResultOtherMain
��������: ����˴��� select count �е���������
��    ����
��    һ���̹߳������     I
��    �������ݵ�ַ         I
��    ������һ������       I
��    �ģ�����             I
��    �壺�ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161106
**********************************************************************/
int managerSelectCountDealResultOtherMain( SELECT_COUNT_SHARE *selectCountShare,  __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId )
{
	char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
	KEY_POINT *key;
	int  iTmp;
	int  iRet;
	
	if (selectCountShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}
	
	/*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
		selectCountShare->llTotalRows += iaSize;
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    for (iTmp = 0; iTmp < iaSize; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerSelectCountDealResultOtherMain( selectCountShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
    }
	}
	
	return SUCCESS;
}

/**********************************************************************
��������: managerSelectCountDealResultRightMain
��������: ����˴��� select count �е���������
��    ����
��    һ���̹߳������     I
��    �������ݵ�ַ         I
��    ������һ������       I
��    �ģ���ʼλ��         I
��    �壺����             I
��    �����ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161106
**********************************************************************/
int managerSelectCountDealResultRightMain( SELECT_COUNT_SHARE *selectCountShare, __int64 iaAddr, __int64 iaSize, int iaEndOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId )
{
	char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
	KEY_POINT *key;
	int  iTmp;
	int  iRet;
	
	//logFmtSyn( LVNOR, "��ʼִ�к���[%s]", __FUNCTION__ );
	
	if (selectCountShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}
	
	/*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
		selectCountShare->llTotalRows += iaEndOffset[iaLoop] + 1;
	  return SUCCESS;
	}
	
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iaLoop] * sizeof(KEY_POINT));
    
    iRet = managerSelectCountDealResultRightMain( selectCountShare, key->llChildAddr, key->llChildSize, iaEndOffset, iaLoop + 1, iaTotalLoops, iaIndepentId );
    if (iRet)
    {
    	return INTERRUPT;
    }
    
    for (iTmp = 0; iTmp < iaEndOffset[iaLoop]; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerSelectCountDealResultOtherMain( selectCountShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
    }
	}
	
	return SUCCESS;
}
