#define __KEY_VALUE_THREAD_MANAGER_SELECT_KEY_VALUE_C

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

void managerSelectKeyValueDeal( int iaSocket, char *pcaOperate, char *pcaBufStart, char *pcaBufEnd );
void managerSelectKeyValueDealFreeSelectId( __int64 llaSelectId );
void managerSelectKeyValueDealFreeSelectIdData( __int64 llaSelectId );
void managerSelectKeyValueDealFreeResultBuf( __int64 llaSelectId, int iaSize );
void managerSelectKeyValueDealChildThread( SELECT_SHARE *selectShare );
int  managerSelectKeyValueDealScanfValue( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId );
int  managerSelectKeyValueDealResult( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, int iaStartOffset[], int iaEndOffset[], __int64 iaAddr, __int64 iaTotalLoops, int iaIndepentFileId );
int  managerSelectKeyValueDealGetDataLeft( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId );
int  managerSelectKeyValueDealGetDataOther( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId );
int  managerSelectKeyValueDealGetDataRight( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex,  __int64 iaAddr, __int64 iaSize, int iaEndOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId );
int  managerSelectKeyValueInsertKeyValuePoint( __int64 llaSelectId, int iaKeyLen, char *pcaKey, int iaValueLen, char *pcaValue );

int  managerSelectKeyValueDealScanfValueMain( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId );
int  managerSelectKeyValueDealResultMain( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, int iaStartOffset[], int iaEndOffset[], __int64 iaAddr, __int64 iaTotalLoops, int iaIndepentId );
int  managerSelectKeyValueDealGetDataLeftMain( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId );
int  managerSelectKeyValueDealGetDataOtherMain( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId );
int  managerSelectKeyValueDealGetDataRightMain( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex,  __int64 iaAddr, __int64 iaSize, int iaEndOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId );

#define SELECT_MALLOC( buf, selectid, size ) \
{\
  /*����Ƿ����㹻�Ĳ�ѯ����ռ䣬������㣬�ͷ�һ����*//*�ڲ�ѯ�����ڴ�ǰ*/\
  if (llgResultBufOffset + size > MAX_SELECT_BUF)\
  {\
    managerSelectKeyValueDealFreeResultBuf( selectid, size );\
  }\
  buf = malloc(size);\
  if (buf != NULL)\
  {\
    memset( buf, 0x00, size );\
    llgResultBufOffset += size;\
  }\
}

#define SELECT_FREE( buf, size ) \
{\
  free( buf );\
  buf = NULL;\
  llgResultBufOffset -= size;\
}


/**********************************************************************
��������: managerSelectKeyValueDeal
��������: ����˴��� select key, value ����
��    ����
��    һ��SOCKET               I
��    �������ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20161022
**********************************************************************/
void managerSelectKeyValueDeal( int iaSocket, char *pcaOperate, char *pcaBufStart, char *pcaBufEnd )
{
/*
�������ݽṹ������+����+��ȡ����+��ȡ��ʼλ��+ֵ���䣨���ҿ������ұա�����ҿ�������ұգ�+KEY��ʼֵ����+KEY��ʼֵ+KEY����ֵ����+KEY����ֵ
�������ݽṹ���������+������+��ǰ��������+(KEY��VALUE�ĳ��ȼ�ֵ)
�������ݽṹ������+��ȡ����+��ȡ��ʼλ��
�������ݽṹ���������+������+��ǰ��������+(KEY��VALUE�ĳ��ȼ�ֵ)
*/
#define MANAGER_SELECT_KEY_VALUE_DEAL_SEND_ERROR( value ) \
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
	return;\
}
  char pcSysDate[DATE_LEN + 1];
	char pcSysTime[TIME_LEN + 1];
	char pcSysDateTime[DATE_TIME + 1];
	char pcResult[INT_LEN + 1];
	char cResult;
	char *pcSendBuf;
	char pcRows[INT64_LEN + 1];
	char pcTotalRows[INT64_LEN + 1];
	char pcReturnRows[INT64_LEN + 1];
  char pcRowStartIndex[INT64_LEN + 1];
	char pcKeyValueStartLen[KEY_VALUE_LENGTH + 1];
  char pcKeyValueEndLen[KEY_VALUE_LENGTH + 1];
  char pcKeyValueStart[MAX_VALUE_LEN + 1];
  char pcKeyValueEnd[MAX_VALUE_LEN + 1];
  char pcValueType[KEY_VALUE_LENGTH + 1];
  char pcKeyValueLen[KEY_VALUE_LENGTH + 1];
  char pcSendLen[DATA_BUF_LEN + 1];
  char pcRecvLen[DATA_BUF_LEN + 1];
  char pcSelectResultPath[PATH_LEN];
  char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iKeyValueLen;
  int  iKeyValueStartLen;
	int  iKeyValueEndLen;
	int  iKeyValueFlag;                     /*��ȡKEY����VALUE����KEY VALUE*/
	__int64 llStartIndex;
	__int64 llStartIndexTmp;
	__int64 llSelectRows;
	__int64 llReturnRows;
	__int64 llTmp;
	__int64 llTmp2;
	int  iRet;
	char *pcBufStart;
	char *pcBufEnd;
	SELECT_SHARE selectStruct;
	char pcOperate[OPERATE_LEN + 1];
	char pcSelectId[INT64_LEN+1];
	int iRecvLen;
	int iTotalLen;
	int iDataLen;
	int iSendLen;
	int iTmp;
	int iIndex;
	int iTime;
	int iKeyLen;
	int iValueLen;
	__int64 llSelectId;
	SELECT_RESULT_BUF *sResultBufTmp;
	SELECT_KEY_VALUE_RESULT *skvr;
	SELECT_KEY_VALUE_RESULT *skvrTmp;
	FILE *fp;
	
	
	#ifdef SYSTEM_WINDOWS_64
	  void *threadHandle;                /*windows�µ��߳̾��*/
	  unsigned long ulWindowsThreadId;
	#else/*#ifdef SYSTEM_UBUNTU*/
	  pthread_t threadId;                /*����OS�µ��߳�ID*/
	#endif
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
	/*��ȡϵͳ��ʱ��*/
	getDateTime( pcSysDate, pcSysTime );
	
	memset( pcSysDateTime, 0x00, sizeof(pcSysDateTime) );
	sprintf( pcSysDateTime, "%s%s", pcSysDate, pcSysTime );
	
	/*��ȡ��ѯID*/
	memset( pcSelectId, 0x00, sizeof(pcSelectId) );
	memcpy( pcSelectId, pcBufStart, INT64_LEN );
	pcBufStart += INT64_LEN;
	
	/*��ȡ��������*//*ֵΪ0ʱ����ʾ�����ѯID��Ӧ�Ļ�����*/
	memset( pcRows, 0x00, sizeof(pcRows) );
	memcpy( pcRows, pcBufStart, INT64_LEN );
	pcBufStart += INT64_LEN;
	llSelectRows = atoll( pcRows );
	
	/*���ֵΪ0*//*������������*/
	if (llSelectRows == 0)
	{
		llSelectId = atoll( pcSelectId );
		
		MANAGER_SELECT_LOCK();
		{
		  managerSelectKeyValueDealFreeSelectId( llSelectId );
		}
		MANAGER_SELECT_UNLOCK();
		
		/*���ؽ��*/
		MANAGER_SELECT_KEY_VALUE_DEAL_SEND_ERROR( SUCCESS );
		return;
	}
	
	/*����ǳ�ʼֵ�Ĳ�ѯID*/
	if (!strcmp( pcSelectId, NULL_SELECT_ID ))
  {
  	/*��ȡ��ʼλ��*/
	  memset( pcRowStartIndex, 0x00, sizeof(pcRowStartIndex) );
	  memcpy( pcRowStartIndex, pcBufStart, INT64_LEN );
	  pcBufStart += INT64_LEN;
	  llStartIndex = atoll( pcRowStartIndex );
	  llStartIndexTmp = llStartIndex;
	  
	  /*ȡֵ����*/
	  memset( pcValueType, 0x00, sizeof(pcValueType) );
	  memcpy( pcValueType, pcBufStart, KEY_VALUE_LENGTH );
	  pcBufStart += KEY_VALUE_LENGTH;
	  
	  /*ȡKEY��VALUE��ʼֵ����*/
	  memset( pcKeyValueStartLen, 0x00, sizeof(pcKeyValueStartLen) );
	  memcpy( pcKeyValueStartLen, pcBufStart, KEY_VALUE_LENGTH );
	  pcBufStart += KEY_VALUE_LENGTH;
	  
	  /*ȡKEY��VALUE��ʼֵ*/
	  iKeyValueStartLen = atoi( pcKeyValueStartLen );
	  memset( pcKeyValueStart, 0x00, sizeof(pcKeyValueStart) );
	  memcpy( pcKeyValueStart, pcBufStart, iKeyValueStartLen );
	  pcBufStart += iKeyValueStartLen;
	  
	  /*ȡKEY��VALUE����ֵ����*/
	  memset( pcKeyValueEndLen, 0x00, sizeof(pcKeyValueEndLen) );
	  memcpy( pcKeyValueEndLen, pcBufStart, KEY_VALUE_LENGTH );
	  pcBufStart += KEY_VALUE_LENGTH;
	  
	  /*ȡKEY��VALUE����ֵ*/
	  iKeyValueEndLen = atoi( pcKeyValueEndLen );
	  memset( pcKeyValueEnd, 0x00, sizeof(pcKeyValueEnd) );
	  memcpy( pcKeyValueEnd, pcBufStart, iKeyValueEndLen );
	  pcBufStart += iKeyValueEndLen;
	  
	  if (!strcmp( pcaOperate, MANAGER_SELECT_KEY_CNT ))
    {
      iKeyValueFlag = SELECT_KEY;
    }
    else
    if (!strcmp( pcaOperate, MANAGER_SELECT_VALUE_CNT ))
    {
      iKeyValueFlag = SELECT_VALUE;
    }
    else
    /*if (!strcmp( pcaOperate, MANAGER_SELECT_KEY_VALUE_CNT ))*/
    {
      iKeyValueFlag = SELECT_KEY_VALUE;
    }
    
    /*ȡ�õ�ǰ��ѯID*/
    MANAGER_SELECT_LOCK();
      llSelectId = llgSelectId;
    	llgSelectId++;
  	MANAGER_SELECT_UNLOCK();
  	
  	/*����ѯ�������뵽������*/
  	MANAGER_SELECT_LOCK();
  	{
  	  if (sgResultBuf == NULL)
  	  {
  	    SELECT_MALLOC( sgResultBuf, llSelectId, sizeof(SELECT_RESULT_BUF) );
  	  	if (sgResultBuf == NULL)
  	  	{
  	  		MANAGER_SELECT_KEY_VALUE_DEAL_SEND_ERROR( ERROR );
  	  	}
  	  	
  	  	memcpy( sgResultBuf->pcDateTime, pcSysDateTime, DATE_TIME );                /*��һ��ʹ�ò�ѯID��ʱ��*/
  	  	sgResultBuf->iKeyValueFlag = iKeyValueFlag;                                 /*��ȡKEY����VALUE����KEY VALUE*/
	      memcpy( sgResultBuf->pcValueType, pcValueType, KEY_VALUE_LENGTH );          /*����������ֵ���䣨KEY��VALUE ���ҿ������ұա�����ҿ�������ұ�*/
	      memcpy( sgResultBuf->pcKeyValueStart, pcKeyValueStart, iKeyValueStartLen ); /*KEY VALUE�Ŀ�ʼֵ*/
	      sgResultBuf->iKeyValueStartLen = iKeyValueStartLen;                         /*KEY VALUE �Ŀ�ʼֵ����*/
	      memcpy( sgResultBuf->pcKeyValueEnd, pcKeyValueEnd, iKeyValueEndLen );       /*KEY VALUE �Ľ���ֵ*/
	      sgResultBuf->iKeyValueEndLen = iKeyValueEndLen;                             /*KEY VALUE �Ľ���ֵ����*/
	      
	      sgResultBuf->llSelectId = llSelectId;                                       /*��ѯID*/
  	  }
  	  else
  	  {
  	  	sResultBufTmp = sgResultBuf;
  	  	
  	  	while (sResultBufTmp ->next != NULL)
  	  	{
  	  		sResultBufTmp = sResultBufTmp->next;
  	  	}
  	  	
  	  	SELECT_MALLOC( sResultBufTmp->next, llSelectId, sizeof(SELECT_RESULT_BUF) );
  	  	if (sResultBufTmp->next == NULL)
  	  	{
  	  		MANAGER_SELECT_KEY_VALUE_DEAL_SEND_ERROR( ERROR );
  	  	}
  	  	
  	  	memcpy( sResultBufTmp->next->pcDateTime, pcSysDateTime, DATE_TIME );                /*��һ��ʹ�ò�ѯID��ʱ��*/
  	  	sResultBufTmp->next->iKeyValueFlag = iKeyValueFlag;                                 /*��ȡKEY����VALUE����KEY VALUE*/
	      memcpy( sResultBufTmp->next->pcValueType, pcValueType, KEY_VALUE_LENGTH );          /*����������ֵ���䣨KEY��VALUE ���ҿ������ұա�����ҿ�������ұ�*/
	      memcpy( sResultBufTmp->next->pcKeyValueStart, pcKeyValueStart, iKeyValueStartLen ); /*KEY VALUE�Ŀ�ʼֵ*/
	      sResultBufTmp->next->iKeyValueStartLen = iKeyValueStartLen;                         /*KEY VALUE �Ŀ�ʼֵ����*/
	      memcpy( sResultBufTmp->next->pcKeyValueEnd, pcKeyValueEnd, iKeyValueEndLen );       /*KEY VALUE �Ľ���ֵ*/
	      sResultBufTmp->next->iKeyValueEndLen = iKeyValueEndLen;                             /*KEY VALUE �Ľ���ֵ����*/
	      
	      sResultBufTmp->next->llSelectId = llSelectId;                                       /*��ѯID*/
	      sResultBufTmp->next->llStartIndex = llStartIndex;                                   /*��ǰ����еĲ�ѯ�����λ��ƫ����*/
  	  }  	  
    }
    MANAGER_SELECT_UNLOCK();
  	
  	/*���������̹߳�������*/
	  memset( &selectStruct, 0x00, sizeof(selectStruct) );
	  selectStruct.llSelectId = llSelectId;                                       /*��ѯID*/
	  selectStruct.iState = RUN_STATE_UNFINISHED;                                 /*���߳�����״̬*/
	  selectStruct.iCmd = RUN_CMD_RUN;                                            /*���߳���������*/
  	
  	/*ִ������*/
  	MANAGER_RDLOCK();
  	{
	    #ifdef SYSTEM_WINDOWS_64
        threadHandle = CreateThread( NULL, 0, (void *)(&managerSelectKeyValueDealChildThread), &selectStruct, 0, &ulWindowsThreadId );
        if (sgManagerThread.threadHandle == NULL)
        {
        	MANAGER_SELECT_KEY_VALUE_DEAL_SEND_ERROR( ERROR );
        	MANAGER_WRUNLOCK();
          return;
        }
      #else/*#ifdef SYSTEM_UBUNTU*/
        if (pthread_create(&threadId, NULL, (void *)(&managerSelectKeyValueDealChildThread), &selectStruct) != 0)
        {
      	  MANAGER_SELECT_KEY_VALUE_DEAL_SEND_ERROR( ERROR );
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
	    	if (selectStruct.iState == RUN_STATE_FINISHED)
	    	{
	    		logFmtSyn( LVNOR, "selectStruct.iState[%d]", selectStruct.iState );
	    		break;
	    	}
	    	else
	    	{
	    		usleep( 100*1000 );
	    	}
	    	iTime++;
	    }
	    
	    if (selectStruct.iState != RUN_STATE_FINISHED)
      {
      	logFmtSyn( LVNOR, "�����̷߳�����ֹ����" );
	      /*�����̷߳�����ֹ����*/
	      selectStruct.iCmd = RUN_CMD_EXIT;
	      
	      /*�ȴ����߳�ִ�н���*/
	      while (selectStruct.iState != RUN_STATE_FINISHED)
	      {
	      	THREAD_THREAD_SLEEP();
	      }
      }
  	}
  	MANAGER_RDUNLOCK();
  	  	
  	/*���ò�ѯID*/
  	memset( pcSelectId, 0x00, sizeof(pcSelectId) );
  	sprintf( pcSelectId, "%0*lld", INT64_LEN, llSelectId );
  }
  /*��ѯID�ļ���ִ��*/
  else
  {
  	/*��ȡ��ʼλ��*/
	  memset( pcRowStartIndex, 0x00, sizeof(pcRowStartIndex) );
	  memcpy( pcRowStartIndex, pcBufStart, INT64_LEN );
	  pcBufStart += INT64_LEN;
	  llStartIndex = atoll( pcRowStartIndex );
	  llStartIndexTmp = llStartIndex;
	  
	  /*���²�ѯIDʹ��ʱ�䡢��ǰ����еĲ�ѯ�����λ��ƫ����*/
  	llSelectId = atoll( pcSelectId );
  	sResultBufTmp = sgResultBuf;
  	
  	MANAGER_SELECT_LOCK();
  	logFmtSyn( LVNOR, "sResultBufTmp[%lld]", sResultBufTmp );

  	  while (sResultBufTmp != NULL)
  	  {
  	  	if (sResultBufTmp->llSelectId == llSelectId)
  	  	{
  	  		memcpy( sResultBufTmp->pcDateTime, pcSysDateTime, DATE_TIME );                /*��һ��ʹ�ò�ѯID��ʱ��*/
  	  		break;
  	  	}
  	  	else
  	  	{
  	  		sResultBufTmp = sResultBufTmp->next;
  	  	}
  	  }
  	MANAGER_SELECT_UNLOCK();
  	
  	/*�����㱻����*/
  	if (sResultBufTmp == NULL)
  	{
  	  MANAGER_SELECT_KEY_VALUE_DEAL_SEND_ERROR( ERROR );
  	}
  	
  	/*�Ƿ����㹻������*//*�������е�����λ��С�ڴ�ȡ���ݵ�λ�ã��������㹻������*/
  	if ((sResultBufTmp->llStartIndex <= llStartIndex) && (sResultBufTmp->llStartIndex + sResultBufTmp->llRows >= llStartIndex + llSelectRows))
  	{
  	  /*���㹻�����ݣ�����Ҫ����ֱ����������ӻ�����ִ��ȡ����*/
  	}
  	/*���������������ִ�в�ѯ*/
  	else
  	{
  		logFmtSyn( LVNOR, "sResultBufTmp->llStartIndex[%lld]sResultBufTmp->llRows[%lld]llStartIndex[%lld]llSelectRows[%lld]", sResultBufTmp->llStartIndex, sResultBufTmp->llRows, llStartIndex, llSelectRows );
  		
  	  /*�ͷŻ�����������*/
  	  managerSelectKeyValueDealFreeSelectIdData( llSelectId );
  	  
  	  /*���õ�ǰ����еĲ�ѯ�����λ��ƫ�����͵�ǰ����*/
  	  sResultBufTmp->llStartIndex = llStartIndex;
  	  sResultBufTmp->llRows = 0;
  	  
  	  /*���������̹߳�������*/
	    memset( &selectStruct, 0x00, sizeof(selectStruct) );
	    selectStruct.llSelectId = llSelectId;                                       /*��ѯID*/
	    selectStruct.iState = RUN_STATE_UNFINISHED;                                 /*���߳�����״̬*/
	    selectStruct.iCmd = RUN_CMD_RUN;                                            /*���߳���������*/
	    
	    /*ִ������*/
	    MANAGER_RDLOCK();
  	  {
	      #ifdef SYSTEM_WINDOWS_64
          threadHandle = CreateThread( NULL, 0, (void *)(&managerSelectKeyValueDealChildThread), &selectStruct, 0, &ulWindowsThreadId );
          if (sgManagerThread.threadHandle == NULL)
          {
          	MANAGER_SELECT_KEY_VALUE_DEAL_SEND_ERROR( ERROR );
          	MANAGER_WRUNLOCK();
            return;
          }
        #else/*#ifdef SYSTEM_UBUNTU*/
          if (pthread_create(&threadId, NULL, (void *)(&managerSelectKeyValueDealChildThread), &selectStruct) != 0)
          {
        	  MANAGER_SELECT_KEY_VALUE_DEAL_SEND_ERROR( ERROR );
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
	      	if (selectStruct.iState == RUN_STATE_FINISHED)
	      	{
	      		logFmtSyn( LVNOR, "selectStruct.iState[%d]", selectStruct.iState );
	      		break;
	      	}
	      	else
	      	{
	      		usleep( 100*1000 );
	      	}
	      	iTime++;
	      }
	      
	      if (selectStruct.iState != RUN_STATE_FINISHED)
        {
        	logFmtSyn( LVNOR, "�����̷߳�����ֹ����" );
	        /*�����̷߳�����ֹ����*/
	        selectStruct.iCmd = RUN_CMD_EXIT;
	        
	        /*�ȴ����߳�ִ�н���*/
	        while (selectStruct.iState != RUN_STATE_FINISHED)
	        {
	        	THREAD_THREAD_SLEEP();
	        }
        }
  	  }
  	  MANAGER_RDUNLOCK();
  	}
  }
  
  logFmtSyn( LVNOR, "��ʼ�ӻ�������ȡ����" );
  
  MANAGER_SELECT_LOCK();
  {
    /*�ӻ�������������*/
    sResultBufTmp = sgResultBuf;
    	
    while (sResultBufTmp != NULL)
    {
    	if (sResultBufTmp->llSelectId == llSelectId)
    	{
    		memcpy( sResultBufTmp->pcDateTime, pcSysDateTime, DATE_TIME );                /*��һ��ʹ�ò�ѯID��ʱ��*/
    		break;
    	}
    	else
    	{
    		sResultBufTmp = sResultBufTmp->next;
    	}
    }
    
    /*�����㱻����*/
  	if (sResultBufTmp == NULL)
  	{
  	  MANAGER_SELECT_KEY_VALUE_DEAL_SEND_ERROR( ERROR );
  	  MANAGER_SELECT_UNLOCK();
  	  return;
  	}
  	logFmtSyn( LVNOR, "sResultBufTmp->llStartIndex[%lld]sResultBufTmp->llRows[%lld]llStartIndex[%lld]", sResultBufTmp->llStartIndex, sResultBufTmp->llRows, llStartIndex );
    /*�����������������*/
    if ((sResultBufTmp->llStartIndex <= llStartIndex) && (sResultBufTmp->llStartIndex + sResultBufTmp->llRows > llStartIndex))
  	{
  		/*������ݱ��ͷ�*//*���ļ���ȡ����*/
    	if (sResultBufTmp->skvr == NULL)
    	{
    		/*���ѯ��������ļ�·��*/
	      memset( pcSelectResultPath, 0x00, sizeof(pcSelectResultPath) );
	      
	      #ifdef SYSTEM_WINDOWS_64
	        if (strlen(pcgSelectTempPath) == 0)
          {
            sprintf( pcSelectResultPath, "\"%lld\"", llSelectId );
          }
          else
          {
          	sprintf( pcSelectResultPath, "\"%s\\%lld\"", pcgSelectTempPath, llSelectId );
          }
	      #else
	        if (strlen(pcgSelectTempPath) == 0)
          {
            sprintf( pcSelectResultPath, "\"%lld\"", llSelectId );
          }
          else
          {
          	sprintf( pcSelectResultPath, "%s/%lld", pcgSelectTempPath, llSelectId );
          }
	      #endif
	      
	      /*���ļ�*/
	      fp = fopen( pcSelectResultPath, "rb" );
	      if (fp == NULL)
	      {
	      	MANAGER_SELECT_KEY_VALUE_DEAL_SEND_ERROR( ERROR );
  	      MANAGER_SELECT_UNLOCK();
  	      return;
	      }
	      
	      while (1)
	      {
	      	memset( pcKeyValueLen, 0x00, sizeof(pcKeyValueLen) );
          fread( pcKeyValueLen, 1, KEY_VALUE_LENGTH, fp );
          
          iKeyValueLen = atoi(pcKeyValueLen);
          
          if (iKeyValueLen == 0)
          {
          	break;
          }
            
	      	if (sResultBufTmp->iKeyValueFlag == SELECT_KEY)
          {
            fread( pcKey, 1, iKeyValueLen, fp );
            pcKey[iKeyValueLen] = 0x00;
            
            managerSelectKeyValueInsertKeyValuePoint( llSelectId, iKeyValueLen, pcKey, 0, NULL );
          }
          else
          if (sResultBufTmp->iKeyValueFlag == SELECT_VALUE)
          {
            fread( pcValue, 1, iKeyValueLen, fp );
            pcValue[iKeyValueLen] = 0x00;
            
            managerSelectKeyValueInsertKeyValuePoint( llSelectId, 0, NULL, iKeyValueLen, pcValue );
          }
          else
          /*if (sResultBufTmp->iKeyValueFlag = SELECT_KEY_VALUE)*/
          {
            fread( pcKey, 1, iKeyValueLen, fp );
            pcKey[iKeyValueLen] = 0x00;
            
            memset( pcKeyValueLen, 0x00, sizeof(pcKeyValueLen) );
            fread( pcKeyValueLen, 1, KEY_VALUE_LENGTH, fp );
            iValueLen = atoi(pcKeyValueLen);
            
            fread( pcValue, 1, iValueLen, fp );
            pcValue[iValueLen] = 0x00;
            
            managerSelectKeyValueInsertKeyValuePoint( llSelectId, iKeyValueLen, pcKey, iValueLen, pcValue );
          }
	      }
	      fclose(fp);
  	  }
  	
  	  llTmp = sResultBufTmp->llStartIndex;
  	  skvr = sResultBufTmp->skvr;
  	  
  	  /*�л������ݵ�λ��*/
	    while (1)
	    {
	      if (llTmp == llStartIndex)
	      {
	        break;
	      }
	      else
	      {
	        skvr = skvr->next;
	        llTmp++;
	      }
	    }

	    /*����Ҫ���͵���������*/
	    iSendLen = 1 + INT64_LEN + INT64_LEN;
	    skvrTmp = skvr;
	    
	    llReturnRows = 0;
	    for (llTmp2 = llTmp; llTmp2 <= (sResultBufTmp->llStartIndex) + (sResultBufTmp->llRows), llTmp2 < llStartIndex + llSelectRows; llTmp2++)
	    {
	      if (sResultBufTmp->iKeyValueFlag == SELECT_KEY)
        {
          iSendLen += skvrTmp->iKeyLen;
      	  iSendLen += KEY_VALUE_LENGTH;
        }
        else
        if (sResultBufTmp->iKeyValueFlag == SELECT_VALUE)
        {
          iSendLen += skvrTmp->iValueLen;
	    	  iSendLen += KEY_VALUE_LENGTH;
        }
        else
        /*if (sResultBufTmp->iKeyValueFlag = SELECT_KEY_VALUE)*/
        {
          iSendLen += skvrTmp->iKeyLen;
      	  iSendLen += KEY_VALUE_LENGTH;
      	
	    	  iSendLen += skvrTmp->iValueLen;
	    	  iSendLen += KEY_VALUE_LENGTH;
        }
        llReturnRows++;
        skvrTmp = skvrTmp->next;
	    }
	    
	    skvrTmp = skvr;
	    
	    /*�鷢������*/
	    pcSendBuf = malloc( iSendLen + 1 );
	    if (pcSendBuf == NULL)
	    {
	    	MANAGER_SELECT_KEY_VALUE_DEAL_SEND_ERROR( ERROR );
	    	MANAGER_SELECT_UNLOCK();
	      return;
	    }
	    memset( pcSendBuf, 0x00, iSendLen + 1 );
      
      memset( pcReturnRows, 0x00, sizeof(pcReturnRows) );
      sprintf( pcReturnRows, "%0*lld", INT64_LEN, llReturnRows );
	    
	    sprintf( &cResult, "%d", SUCCESS );
      memcpy( pcSendBuf, &cResult, 1 );
      memcpy( pcSendBuf + 1, pcSelectId, INT64_LEN );
      memcpy( pcSendBuf + 1 + INT64_LEN, pcReturnRows, INT64_LEN );
      
      iIndex = 1 + INT64_LEN + INT64_LEN;
      
      for (llTmp2 = 0; llTmp2 < llReturnRows; llTmp2++)
	    {
	      if (sResultBufTmp->iKeyValueFlag == SELECT_KEY)
	      {
	        memset( pcKeyValueLen, 0x00, sizeof(pcKeyValueLen) );
	    	  sprintf( pcKeyValueLen, "%0*d", KEY_VALUE_LENGTH, skvrTmp->iKeyLen );
	      	memcpy( pcSendBuf + iIndex, pcKeyValueLen, KEY_VALUE_LENGTH );
	    	  iIndex += KEY_VALUE_LENGTH;
	    	
	    	  memcpy( pcSendBuf + iIndex, skvrTmp->pcKey, skvrTmp->iKeyLen );
	        iIndex += skvrTmp->iKeyLen;
	      }
	      else
        if (sResultBufTmp->iKeyValueFlag == SELECT_VALUE)
        {
          memset( pcKeyValueLen, 0x00, sizeof(pcKeyValueLen) );
          sprintf( pcKeyValueLen, "%0*d", KEY_VALUE_LENGTH, skvrTmp->iValueLen );
	    	  memcpy( pcSendBuf + iIndex, pcKeyValueLen, KEY_VALUE_LENGTH );
	    	  iIndex += KEY_VALUE_LENGTH;
	    	
	        memcpy( pcSendBuf + iIndex, skvrTmp->pcValue, skvrTmp->iValueLen );
	        iIndex += skvrTmp->iValueLen;
        }
        else
        /*if (sResultBufTmp->iKeyValueFlag = SELECT_KEY_VALUE)*/
        {
          memset( pcKeyValueLen, 0x00, sizeof(pcKeyValueLen) );
	    	  sprintf( pcKeyValueLen, "%0*d", KEY_VALUE_LENGTH, skvrTmp->iKeyLen );
	    	  memcpy( pcSendBuf + iIndex, pcKeyValueLen, KEY_VALUE_LENGTH );
	    	  iIndex += KEY_VALUE_LENGTH;
	    	  
	    	  memcpy( pcSendBuf + iIndex, skvrTmp->pcKey, skvrTmp->iKeyLen );
	        iIndex += skvrTmp->iKeyLen;
	        
	    	  
	    	  memset( pcKeyValueLen, 0x00, sizeof(pcKeyValueLen) );
          sprintf( pcKeyValueLen, "%0*d", KEY_VALUE_LENGTH, skvrTmp->iValueLen );
	    	  memcpy( pcSendBuf + iIndex, pcKeyValueLen, KEY_VALUE_LENGTH );
	    	  iIndex += KEY_VALUE_LENGTH;
	    	  
	        memcpy( pcSendBuf + iIndex, skvrTmp->pcValue, skvrTmp->iValueLen );
	        iIndex += skvrTmp->iValueLen;
        }
        
        skvrTmp = skvrTmp->next;
	    }
	    
	    memset( pcSendLen, 0x00, sizeof(pcSendLen) );
      sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
      
      send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
      send( iaSocket, pcSendBuf, iSendLen, 0 );
      free( pcSendBuf );
  	}
  	/*��������0*/
  	else
  	{
  	  /*����Ҫ���͵���������*/
	    iSendLen = 1 + INT64_LEN + INT64_LEN;
	    
	    pcSendBuf = malloc( iSendLen + 1 );
	    if (pcSendBuf == NULL)
	    {
	    	MANAGER_SELECT_KEY_VALUE_DEAL_SEND_ERROR( ERROR );logFmtSyn( LVNOR, "[aaa]" );
	      MANAGER_SELECT_UNLOCK();
  	    return;
	    }
	    memset( pcSendBuf, 0x00, iSendLen + 1 );
	    
	    sprintf( &cResult, "%d", SUCCESS );
	    
	    memset( pcReturnRows, 0x00, sizeof(pcReturnRows) );
	    sprintf( pcReturnRows, "%0*d", INT64_LEN, 0 );
	    
      memcpy( pcSendBuf, &cResult, 1 );
      memcpy( pcSendBuf + 1, pcSelectId, INT64_LEN );
      memcpy( pcSendBuf + 1 + INT64_LEN, pcReturnRows, INT64_LEN );
	    
	    memset( pcSendLen, 0x00, sizeof(pcSendLen) );
      sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
      
      send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
      send( iaSocket, pcSendBuf, iSendLen, 0 );
      
      free( pcSendBuf );
  	}
  }
  MANAGER_SELECT_UNLOCK();
  return;
}

/**********************************************************************
��������: managerSelectKeyValueDealFreeSelectId
��������: �ͷŲ�ѯID
��    ����
��    һ����ѯID               I
��    ��: �����
����ʱ��: 20170108
**********************************************************************/
void managerSelectKeyValueDealFreeSelectId( __int64 llaSelectId )
{
	char pcSelectResultPath[PATH_LEN];
	__int64 llSelectId;
	SELECT_RESULT_BUF *sResultBufTmp;
	SELECT_RESULT_BUF *sResultBufTmp2;
	SELECT_KEY_VALUE_RESULT *skvr;
	SELECT_KEY_VALUE_RESULT *skvrTmp;
	
	llSelectId = llaSelectId;
	
	/*�ͷŽ��*//*sgResultBuf��ֵΪNULL����ʾδ���������*/
	if (sgResultBuf != NULL)
	{
		/*�����һ��������*/
		if (sgResultBuf->llSelectId == llSelectId)
		{
			/*��¼��һ������ֵ*/
			sResultBufTmp = sgResultBuf->next;
			
			/*ָ��ý��ļ�ֵ������*/
			skvr = sgResultBuf->skvr;
			
			/*�ͷŽ��*/
			while (skvr != NULL)
			{
				skvrTmp = skvr;
				skvr = skvr->next;
				
				/*���������ռ䣬�ͷŽ�㣬����¼��ǰ���������*/
				if (skvrTmp->pcKey != NULL)
				{
					SELECT_FREE( skvrTmp->pcKey, skvrTmp->iKeyLen + 1 );
				}
				
				if (skvrTmp->pcValue != NULL)
				{
					SELECT_FREE( skvrTmp->pcValue, skvrTmp->iValueLen + 1 );
				}
				
				SELECT_FREE( skvrTmp, sizeof(SELECT_KEY_VALUE_RESULT) );
			}
			
			SELECT_FREE( sgResultBuf, sizeof(SELECT_RESULT_BUF) );
			sgResultBuf = sResultBufTmp;
		}
		else
		{
			/*�������*/
			sResultBufTmp = sgResultBuf;
			
			while (sResultBufTmp != NULL)
			{
				/*����ҵ����*/
				if (sResultBufTmp->llSelectId == llSelectId)
				{
					/*ָ��ý��ļ�ֵ������*/
			    skvr = sResultBufTmp->skvr;
			    
			    /*�ͷŽ��*/
			    while (skvr != NULL)
			    {
			    	skvrTmp = skvr;
			    	skvr = skvr->next;
			    	
			    	/*���������ռ䣬�ͷŽ�㣬����¼��ǰ���������*/
			    	if (skvrTmp->pcKey != NULL)
			    	{
			    		SELECT_FREE( skvrTmp->pcKey, skvrTmp->iKeyLen + 1 );
			    	}
			    	
			    	if (skvrTmp->pcValue != NULL)
			    	{
			    		SELECT_FREE( skvrTmp->pcValue, skvrTmp->iValueLen + 1 );
			    	}
			    	
			    	SELECT_FREE( skvrTmp, sizeof(SELECT_KEY_VALUE_RESULT) );
			    }
			    
			    sResultBufTmp2 = sResultBufTmp;
			    while (sResultBufTmp2 ->next != sResultBufTmp)
			    {
			    	sResultBufTmp2 = sResultBufTmp2->next;
			    }
			    sResultBufTmp2->next = sResultBufTmp->next;
			    
			    SELECT_FREE( sResultBufTmp, sizeof(SELECT_RESULT_BUF) );
			    
			    break;
				}
				else
				{
					sResultBufTmp = sResultBufTmp->next;
				}
			}
		}
	}
	
	/*���ѯ��������ļ�·��*/
	memset( pcSelectResultPath, 0x00, sizeof(pcSelectResultPath) );
	
	#ifdef SYSTEM_WINDOWS_64
	  if (strlen(pcgSelectTempPath) == 0)
    {
      sprintf( pcSelectResultPath, "\"%lld\"", llSelectId );
    }
    else
    {
    	sprintf( pcSelectResultPath, "\"%s\\%lld\"", pcgSelectTempPath, llSelectId );
    }
	#else
	  if (strlen(pcgSelectTempPath) == 0)
    {
      sprintf( pcSelectResultPath, "\"%lld\"", llSelectId );
    }
    else
    {
    	sprintf( pcSelectResultPath, "%s/%lld", pcgSelectTempPath, llSelectId );
    }
	#endif
	
	/*����������ļ����ڣ�ɾ��*/
	if (!access( pcSelectResultPath, F_OK ))
	{
		unlink( pcSelectResultPath );
	}
	
	return;
}

/**********************************************************************
��������: managerSelectKeyValueDealFreeSelectIdData
��������: �����ͷŲ�ѯID�е�����
��    ����
��    һ����ѯID               I
��    ��: �����
����ʱ��: 20170110
**********************************************************************/
void managerSelectKeyValueDealFreeSelectIdData( __int64 llaSelectId )
{
	char pcSelectResultPath[PATH_LEN];
	__int64 llSelectId;
	SELECT_RESULT_BUF *sResultBufTmp;
	SELECT_KEY_VALUE_RESULT *skvr;
	SELECT_KEY_VALUE_RESULT *skvrTmp;
	
	llSelectId = llaSelectId;
	
	/*�ͷŽ��*//*sgResultBuf��ֵΪNULL����ʾδ���������*/
	if (sgResultBuf != NULL)
	{
		/*�����һ��������*/
		if (sgResultBuf->llSelectId == llSelectId)
		{
			/*ָ��ý��ļ�ֵ������*/
			skvr = sgResultBuf->skvr;
			
			/*�ͷŽ��*/
			while (skvr != NULL)
			{
				skvrTmp = skvr;
				skvr = skvr->next;
				
				/*���������ռ䣬�ͷŽ�㣬����¼��ǰ���������*/
				if (skvrTmp->pcKey != NULL)
				{
					SELECT_FREE( skvrTmp->pcKey, skvrTmp->iKeyLen + 1 );
				}
				
				if (skvrTmp->pcValue != NULL)
				{
					SELECT_FREE( skvrTmp->pcValue, skvrTmp->iValueLen + 1 );
				}
				
				SELECT_FREE( skvrTmp, sizeof(SELECT_KEY_VALUE_RESULT) );
			}
			
			sgResultBuf->skvr = NULL;
			sgResultBuf->skvrLast = NULL;
		}
		else
		{
			/*�������*/
			sResultBufTmp = sgResultBuf;
			
			while (sResultBufTmp != NULL)
			{
				/*����ҵ����*/
				if (sResultBufTmp->llSelectId == llSelectId)
				{
					/*ָ��ý��ļ�ֵ������*/
			    skvr = sResultBufTmp->skvr;
			    
			    /*�ͷŽ��*/
			    while (skvr != NULL)
			    {
			    	skvrTmp = skvr;
			    	skvr = skvr->next;
			    	
			    	/*���������ռ䣬�ͷŽ�㣬����¼��ǰ���������*/
			    	if (skvrTmp->pcKey != NULL)
			    	{
			    		SELECT_FREE( skvrTmp->pcKey, skvrTmp->iKeyLen + 1 );
			    	}
			    	
			    	if (skvrTmp->pcValue != NULL)
			    	{
			    		SELECT_FREE( skvrTmp->pcValue, skvrTmp->iValueLen + 1 );
			    	}
			    	
			    	SELECT_FREE( skvrTmp, sizeof(SELECT_KEY_VALUE_RESULT) );
			    }
			    
			    sResultBufTmp->skvr = NULL;
			    sResultBufTmp->skvrLast = NULL;
			    break;
				}
				else
				{
					sResultBufTmp = sResultBufTmp->next;
				}
			}
		}
	}
	
	return;
}

/**********************************************************************
��������: managerSelectKeyValueDealFreeResultBuf
��������: �ͷŲ�ѯ����еĲ������ݣ�������д�뵽Ӳ����
��    ����
��    һ������Ĳ�ѯID
��    ����Ҫ�ͷŵĿռ�
��    ��: �����
����ʱ��: 20170108
**********************************************************************/
void managerSelectKeyValueDealFreeResultBuf( __int64 llaSelectId, int iaSize )
{
	char pcSysDate[DATE_LEN + 1];
	char pcSysTime[TIME_LEN + 1];
	char pcSysDateTime[DATE_TIME + 1];
	char pcSelectResultPath[PATH_LEN];
	FILE *fp;
	__int64 llSelectId;
	int iSize;
	SELECT_RESULT_BUF *sResultBufTmp;
	SELECT_RESULT_BUF *sResultBufTmp2;
	SELECT_KEY_VALUE_RESULT *skvr;
	SELECT_KEY_VALUE_RESULT *skvrTmp;
	
	memset( pcSysDate, 0x00, sizeof(pcSysDate) );
	memset( pcSysTime, 0x00, sizeof(pcSysTime) );
	
	/*��ȡϵͳ��ʱ��*/
	getDateTime( pcSysDate, pcSysTime );
	
	memset( pcSysDateTime, 0x00, sizeof(pcSysDateTime) );
	sprintf( pcSysDateTime, "%s%s", pcSysDate, pcSysTime );
	
	llSelectId = llaSelectId;
	
	iSize = 0;
	sResultBufTmp = sgResultBuf;
	while (sResultBufTmp != NULL)
	{
		/*����Ǳ���㣬ִ����һ��*/
		if (sResultBufTmp->llSelectId == llSelectId)
		{
			sResultBufTmp = sResultBufTmp->next;
			continue;
		}
		else
		{
			/*���ѯ��������ļ�·��*/
	    memset( pcSelectResultPath, 0x00, sizeof(pcSelectResultPath) );
	    
	    #ifdef SYSTEM_WINDOWS_64
	      if (strlen(pcgSelectTempPath) == 0)
        {
          sprintf( pcSelectResultPath, "\"%lld\"", llSelectId );
        }
        else
        {
        	sprintf( pcSelectResultPath, "\"%s\\%lld\"", pcgSelectTempPath, llSelectId );
        }
	    #else
	      if (strlen(pcgSelectTempPath) == 0)
        {
          sprintf( pcSelectResultPath, "\"%lld\"", llSelectId );
        }
        else
        {
        	sprintf( pcSelectResultPath, "%s/%lld", pcgSelectTempPath, llSelectId );
        }
	    #endif
	    
	    /*�ͷŽ�����ݣ������ͷŵ�����д�뵽�����ļ���*/
	    fp = fopen( pcSelectResultPath, "wb+" );
	    if (fp == NULL)
	    {
	    	return;
	    }
	    
	    /*ָ��ý��ļ�ֵ������*/
			skvr = sResultBufTmp->skvr;
			
			/*�ͷŽ��*/
			while (skvr != NULL)
			{
				skvrTmp = skvr;
				skvr = skvr->next;
				
				/*���������ռ䣬�ͷŽ�㣬����¼��ǰ���������*/
				if (skvrTmp->pcKey != NULL)
				{
					fprintf( fp, "%0*d%s", KEY_VALUE_LENGTH, skvrTmp->iKeyLen, skvrTmp->pcKey );
					SELECT_FREE( skvrTmp->pcKey, skvrTmp->iKeyLen + 1);
					iSize += skvrTmp->iKeyLen + 1;
				}
				
				if (skvrTmp->pcValue != NULL)
				{
					fprintf( fp, "%0*d%s", KEY_VALUE_LENGTH, skvrTmp->iValueLen, skvrTmp->pcValue );
					SELECT_FREE( skvrTmp->pcValue, skvrTmp->iValueLen + 1 );
					iSize += skvrTmp->iValueLen + 1;
				}
				
				SELECT_FREE( skvrTmp, sizeof(SELECT_KEY_VALUE_RESULT) );
				iSize += sizeof(SELECT_KEY_VALUE_RESULT);
			}
			
			/*д�볤��0��ʾ����*/
			fprintf( fp, "%0*d", KEY_VALUE_LENGTH, 0 );
			fclose(fp);
			
			sResultBufTmp->skvr = NULL;
			sResultBufTmp->skvrLast = NULL;
	    
	    /*�����ǰ��ѯ��㳬ʱ���ѽ��Ҳ˳���ͷ�*/
	    if (atoi(pcSysDateTime) - atoi(sResultBufTmp->pcDateTime) > SELECT_ID_OVER_TIME)
	    {
		    sResultBufTmp2 = sResultBufTmp;
			  while (sResultBufTmp2 ->next != sResultBufTmp)
			  {
			  	sResultBufTmp2 = sResultBufTmp2->next;
			  }
			  sResultBufTmp2->next = sResultBufTmp->next;
			  
			  SELECT_FREE( sResultBufTmp, sizeof(SELECT_RESULT_BUF) );
			  iSize += sizeof(SELECT_RESULT_BUF);
			  
			  /*�����ָ��ǰ���*/
			  sResultBufTmp = sResultBufTmp2;
	    }
	    
	    /*����ͷŵĿռ���������*/
	    if (iSize >= iaSize)
	    {
	    	break;
	    }
			sResultBufTmp = sResultBufTmp->next;
		}
	}
	
	return;
}

/**********************************************************************
��������: managerSelectKeyValueDealChildThread
��������: ����˴��� select key ����
��    ����
��    һ������ṹ��               I/O
��    ��: �����
����ʱ��: 20161108
**********************************************************************/
void managerSelectKeyValueDealChildThread( SELECT_SHARE *selectShare )
{
  int  iStartOffsetRecord[20];   /*��ʼλ�ü�¼*/
	int  iEndOffsetRecord[20];     /*����λ�ü�¼*/
	__int64  llDisc;
	__int64  llSize;
	__int64  llLoops;
	__int64  llSelectIndex;
	int  iLoop;
	int  iKeyValueFlag;             /*0--WHERE�����Ƕ�KEY��1--��VALUE*/
	int  iRet;
	SELECT_RESULT_BUF *srb;
	int  iIndepentId;
	int  iIndepentFileId;
		
	llSelectIndex = 0;
	
	/*�������*/
	srb = sgResultBuf;
	logFmtSyn( LVNOR, "%lld", srb );
	while (srb != NULL)
	{
		if (srb->llSelectId == selectShare->llSelectId)
		{
			break;
		}
		else
		{
			srb = srb->next;
		}
	}
	logFmtSyn( LVNOR, "%lld", srb );
	/*������ݳ����쳣����������Ϊ�ͻ��˳�ʱ��û�ͷ����ͨѶ���ֻ����ǿͻ���û�������������˵�ͨѶ����������������ͻ���Ӧ�����·����ѯ����ȡ��ѯID*/
	if (srb == NULL)
	{
    selectShare->iState = RUN_STATE_FINISHED;
    return;
	}
	
	/*ȷ����KEY����VALUE��Ϊ�������������Ҷ�λ*/
	iKeyValueFlag = 0;
	
	/*����Ƕ�VALUE����*//*����������0*/
	if (!strcmp( srb->pcValueType, VALUE_OPEN_OPEN ) || 
		  !strcmp( srb->pcValueType, VALUE_OPEN_CLOSE ) || 
		  !strcmp( srb->pcValueType, VALUE_CLOSE_OPEN ) ||
		  !strcmp( srb->pcValueType, VALUE_CLOSE_CLOSE )
		 )
	{
		/*���� VALUE ������������Ϊ0 */
		for (iIndepentId = 0; iIndepentId < INDEPENDENT_SIZE; iIndepentId++)
	  {
	    llDisc = sgGlobalVariableMain[iIndepentId].llgFirstDiscMain;
	    llSize = sgGlobalVariableMain[iIndepentId].llgFirstSizeMain;
	    llLoops = sgGlobalVariableMain[iIndepentId].llgLoopsMain;
	  
		  iRet = managerSelectKeyValueDealScanfValueMain( selectShare, srb, &llSelectIndex, llDisc, llSize, 1, llLoops, iIndepentId );
		  if (iRet == INTERRUPT)
		  {
		  	break;
		  }
		  if (srb->llRows >= EACH_SELECT_SIZE)
      {
        break;
      }
	  }
		for (iIndepentFileId = 0; iIndepentFileId < INDEPENDENT_FILE_SIZE; iIndepentFileId++)
	  {
	  	if (iRet == INTERRUPT)
		  {
		  	break;
		  }
		  if (srb->llRows >= EACH_SELECT_SIZE)
      {
        break;
      }
	    llDisc = sgBase[iIndepentFileId].llFirstDisc;
	    llSize = sgBase[iIndepentFileId].llFirstSize;
	    llLoops = sgBase[iIndepentFileId].llLoops;
		  iRet = managerSelectKeyValueDealScanfValue( selectShare, srb, &llSelectIndex, llDisc, llSize, 1, llLoops, iIndepentFileId );
	  }
		selectShare->iState = RUN_STATE_FINISHED;
	  return;
	}
	else
	{
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
	      if (!strcmp( srb->pcValueType, KEY_OPEN_OPEN ) != !strcmp( srb->pcValueType, KEY_OPEN_CLOSE ))
        {
      	  iRet = managerPositionLeftOpenMain( iStartOffsetRecord, srb->pcKeyValueStart, llDisc, llSize, 1, llLoops, iIndepentId );
      	  
      	  /*���û����������*/
      	  if (iRet == NOT_FOUND)
      	  {
            selectShare->iState = RUN_STATE_FINISHED;
            continue;
      	  }
        }
        else
        {
      	  iRet = managerPositionLeftCloseMain( iStartOffsetRecord, srb->pcKeyValueStart, llDisc, llSize, 1, llLoops, iIndepentId );
      	  
      	  /*���û����������*/
      	  if (iRet == NOT_FOUND)
      	  {
            selectShare->iState = RUN_STATE_FINISHED;
            continue;
      	  }
        }
	    }

	    //logFmtSyn( LVNOR, "��λ��ʼλ�ý���" );
	    
	    /*��λ����λ��*/
	    {
	    	/*��ʼ������λ�ü�¼����*/
	      memset( &iEndOffsetRecord, 0x00, sizeof(iEndOffsetRecord) );
	    
	    	/*������ҿ�*/
	      if (!strcmp( srb->pcValueType, KEY_OPEN_OPEN ) != !strcmp( srb->pcValueType, KEY_CLOSE_OPEN ))
        {
      	  iRet = managerPositionRightOpenMain( iEndOffsetRecord, srb->pcKeyValueEnd, llDisc, llSize, 1, llLoops, iIndepentId );
      	  
      	  /*���û����������*/
      	  if (iRet == NOT_FOUND)
      	  {
            selectShare->iState = RUN_STATE_FINISHED;
            //logFmtSyn( LVNOR, "��������" );
            continue;
      	  }
        }
        else
        {
      	  iRet = managerPositionRightCloseMain( iEndOffsetRecord, srb->pcKeyValueEnd, llDisc, llSize, 1, llLoops, iIndepentId );
      	  
      	  /*���û����������*/
      	  if (iRet == NOT_FOUND)
      	  {
            selectShare->iState = RUN_STATE_FINISHED;
            //logFmtSyn( LVNOR, "��������" );
            continue;
      	  }
        }
	    }

	    //logFmtSyn( LVNOR, "��λ����λ�ý���" );
	    
	    /*��ȡִ�н��*/
	    iRet = managerSelectKeyValueDealResultMain( selectShare, srb, &llSelectIndex, iStartOffsetRecord, iEndOffsetRecord, llDisc, llLoops, iIndepentId );
	    
	    if (iRet == INTERRUPT)
		  {
		  	break;
		  }
	    if (srb->llRows >= EACH_SELECT_SIZE)
      {
      	break;
      }
    }

		for (iIndepentFileId = 0; iIndepentFileId < INDEPENDENT_FILE_SIZE; iIndepentFileId++)
	  {
	  	if (iRet == INTERRUPT)
		  {
		  	break;
		  }
	    if (srb->llRows >= EACH_SELECT_SIZE)
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
	      if (!strcmp( srb->pcValueType, KEY_OPEN_OPEN ) != !strcmp( srb->pcValueType, KEY_OPEN_CLOSE ))
        {
      	  iRet = managerPositionLeftOpen( iStartOffsetRecord, srb->pcKeyValueStart, llDisc, llSize, 1, llLoops, iIndepentFileId );
      	  
      	  /*���û����������*/
      	  if (iRet == NOT_FOUND)
      	  {
            selectShare->iState = RUN_STATE_FINISHED;
            continue;
      	  }
        }
        else
        {
      	  iRet = managerPositionLeftClose( iStartOffsetRecord, srb->pcKeyValueStart, llDisc, llSize, 1, llLoops, iIndepentFileId );
      	  
      	  /*���û����������*/
      	  if (iRet == NOT_FOUND)
      	  {
            selectShare->iState = RUN_STATE_FINISHED;
            continue;
      	  }
        }
	    }
	    
	    //logFmtSyn( LVNOR, "��λ��ʼλ�ý���" );
	    
	    /*��λ����λ��*/
	    {
	    	/*��ʼ������λ�ü�¼����*/
	      memset( &iEndOffsetRecord, 0x00, sizeof(iEndOffsetRecord) );
	    
	    	/*������ҿ�*/
	      if (!strcmp( srb->pcValueType, KEY_OPEN_OPEN ) != !strcmp( srb->pcValueType, KEY_CLOSE_OPEN ))
        {
      	  iRet = managerPositionRightOpen( iEndOffsetRecord, srb->pcKeyValueEnd, llDisc, llSize, 1, llLoops, iIndepentFileId );
      	  
      	  /*���û����������*/
      	  if (iRet == NOT_FOUND)
      	  {
            selectShare->iState = RUN_STATE_FINISHED;
            //logFmtSyn( LVNOR, "��������" );
            continue;
      	  }
        }
        else
        {
      	  iRet = managerPositionRightClose( iEndOffsetRecord, srb->pcKeyValueEnd, llDisc, llSize, 1, llLoops, iIndepentFileId );
      	  
      	  /*���û����������*/
      	  if (iRet == NOT_FOUND)
      	  {
            selectShare->iState = RUN_STATE_FINISHED;
            //logFmtSyn( LVNOR, "��������" );
            continue;
      	  }
        }
	    }
	    
	    //logFmtSyn( LVNOR, "��λ����λ�ý���" );
	    
	    /*��ȡִ�н��*/
	    iRet = managerSelectKeyValueDealResult( selectShare, srb, &llSelectIndex, iStartOffsetRecord, iEndOffsetRecord, llDisc, llLoops, iIndepentFileId );
    }
	}
	selectShare->iState = RUN_STATE_FINISHED;
	return;
}

/**********************************************************************
��������: managerSelectKeyValueDealScanfValue
��������: ����˴��� select key where value[] �еĽ��
��    ����
��    һ���̹߳������     I/O
��    �������ַ           I
��    ����������           I
��    �ģ�����             I
��    �壺�ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161109
**********************************************************************/
int managerSelectKeyValueDealScanfValue( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId )
{
	char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
	KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
	KEY_POINT *key;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iTmp;
	int  iRet;
	
	if (selectShare->iCmd != RUN_CMD_RUN)
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
	      	
	    /*�� VALUE �ļ��л�ȡ VALUE ��ֵ*/
      valueToBuf( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentFileId );
      pcValue[keyData->llValueLen] = 0x00;
	    
	    if (!strcmp( selectResult->pcValueType, VALUE_OPEN_OPEN ))
	    {
	    	/*�������������*/
	    	if (!(strcmp( pcValue, selectResult->pcKeyValueStart ) > 0 && strcmp( pcValue, selectResult->pcKeyValueEnd ) < 0))
	    	{
	    		continue;
	    	}
	    }
	    else
	    if (!strcmp( selectResult->pcValueType, VALUE_OPEN_CLOSE ))
	    {
	    	/*�������������*/
	    	if (!(strcmp( pcValue, selectResult->pcKeyValueStart ) > 0 && strcmp( pcValue, selectResult->pcKeyValueEnd ) <= 0))
	    	{
	    		continue;
	    	}
	    }
	    else
	    if (!strcmp( selectResult->pcValueType, VALUE_CLOSE_OPEN ))
	    {
	    	/*�������������*/
	    	if (!(strcmp( pcValue, selectResult->pcKeyValueStart ) >= 0 && strcmp( pcValue, selectResult->pcKeyValueEnd ) < 0))
	    	{
	    		continue;
	    	}
	    }
	    else
	    /*if (!strcmp( selectResult->pcValueType, VALUE_CLOSE_CLOSE ))*/
	    {
	    	/*�������������*/
	    	if (strcmp( pcValue, selectResult->pcKeyValueStart ) >= 0 && strcmp( pcValue, selectResult->pcKeyValueEnd ) <= 0)
	    	{
	    		continue;
	    	}
	    }
	      	
	  	/*�������ָ����λ��*/
	  	if (*llaSelectIndex >= selectResult->llStartIndex )
	  	{
		    /*���ȡ���㹻�Ĳ�ѯ����*/
        if (selectResult->llRows >= EACH_SELECT_SIZE)
        {
        	return SUCCESS;
        }
	      else
	      {
	      	if (selectResult->iKeyValueFlag == SELECT_KEY)
	      	{
	      	  /*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
            valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
            pcKey[keyData->llKeyLen] = 0x00;
            
            MANAGER_SELECT_LOCK();
              iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, 0, NULL );
            MANAGER_SELECT_UNLOCK();
            /*��������쳣*/
            if (iRet)
            {
            	return INTERRUPT;
            }
          }
          else
          if (selectResult->iKeyValueFlag == SELECT_VALUE)
	      	{
	      	  /*���� VALUE ��ֵ*/
	      	  MANAGER_SELECT_LOCK();
	      	    iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, 0, NULL, keyData->llValueLen, pcValue );
	      	  MANAGER_SELECT_UNLOCK();
	      	  /*��������쳣*/
            if (iRet)
            {
            	return INTERRUPT;
            }
          }
          else
          /*if (selectResult->iKeyValueFlag == SELECT_KEY_VALUE)*/
	      	{
	      		/*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
            valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
            pcKey[keyData->llKeyLen] = 0x00;
            
	      	  /*���� KEY VALUE ��ֵ*/
	      	  MANAGER_SELECT_LOCK();
              iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, keyData->llValueLen, pcValue );
            MANAGER_SELECT_UNLOCK();
            /*��������쳣*/
            if (iRet)
            {
            	return INTERRUPT;
            }
          }
          selectResult->llRows++;
	      }
	    }
	    
	    (*llaSelectIndex)++;
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
      iRet = managerSelectKeyValueDealScanfValue( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
    }
	}
	
	return SUCCESS;
}

/**********************************************************************
��������: managerSelectKeyValueDealResult
��������: ����˴��� select key �еĽ��
��    ����
��    һ���̹߳������     I/O
��    ������ʼλ��         I
��    ��������λ��         I
��    �ģ����ݵ�ַ         I
��    �壺�ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161108
**********************************************************************/
int managerSelectKeyValueDealResult( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, int iaStartOffset[], int iaEndOffset[], __int64 iaAddr, __int64 iaTotalLoops, int iaIndepentFileId )
{
	SELECT_COUNT_SHARE *selectCountShare;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	KEY_POINT *key;                /*�����������е����ݽṹ*/
	KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
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
  	/*�����ѯ��ʼλ��С�ڻ���ڲ�ѯ���������������ݷ��뵽������*/
  	if (selectResult->llStartIndex <= 1)/*����� 1 ��ָ��ǰ�Ĳ�ѯ���ֻ��1��*/
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
  	  
  	  /*��������*/
      if (selectResult->iKeyValueFlag == SELECT_KEY)
	    {
	      /*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
        valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
        pcKey[keyData->llKeyLen] = 0x00;
        
        MANAGER_SELECT_LOCK();
          iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, 0, NULL );
        MANAGER_SELECT_UNLOCK();
        /*��������쳣*/
        if (iRet)
        {
        	return iRet;
        }
      }
      else
      if (selectResult->iKeyValueFlag == SELECT_VALUE)
	    {
	    	/*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
        valueToBuf( pcKey, keyData->llValueAddr, keyData->llValueLen, iaIndepentFileId );
        pcValue[keyData->llValueLen] = 0x00;
        
	      /*���� VALUE ��ֵ*/
	      MANAGER_SELECT_LOCK();
	        iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, 0, NULL, keyData->llValueLen, pcValue );
	      MANAGER_SELECT_UNLOCK();
	      /*��������쳣*/
        if (iRet)
        {
        	return iRet;
        }
      }
      else
      /*if (selectResult->iKeyValueFlag == SELECT_KEY_VALUE)*/
	    {
	    	/*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
        valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
        pcKey[keyData->llKeyLen] = 0x00;
        
        /*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
        valueToBuf( pcKey, keyData->llValueAddr, keyData->llValueLen, iaIndepentFileId );
        pcValue[keyData->llValueLen] = 0x00;
        
	      /*���� KEY VALUE ��ֵ*/
	      MANAGER_SELECT_LOCK();
          iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, keyData->llValueLen, pcValue );
        MANAGER_SELECT_UNLOCK();
        /*��������쳣*/
        if (iRet)
        {
        	return iRet;
        }
      }
      
      selectResult->llRows += 1;
  	}
  	return SUCCESS;
  }
  
  //logFmtSyn( LVNOR, "��ʼ��ȡ����" );
  
  /*ȡ����ǰ��������*/
  iAddr = iaAddr;
  memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
  getKeyBlockData( pcDataBuf, iAddr, iaIndepentFileId );

  for (iTmp = 2; iTmp < iIndex; iTmp++)
  {
  	memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iAddr, iaIndepentFileId );
    
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iTmp] * sizeof(KEY_POINT));
    
    iAddr = key->llChildAddr;
  }

  /*�Ӹü���ʼ����ʼλ�úͽ���λ�ò�ͬ*/
    
  /*��ȡ����*/
  {
    /*����ʼλ�õļ�*/
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iIndex] * sizeof(KEY_POINT));
    iRet = managerSelectKeyValueDealGetDataLeft( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaStartOffset, iIndex + 1, iaTotalLoops, iaIndepentFileId );
    /*������жϽ���*/
    if (iRet)
    {
    	return iRet;
    }

    /*������������ݶ�ȡ����*/
    if (selectResult->llRows >= EACH_SELECT_SIZE)
    {
    	return SUCCESS;
    }
    
    /*��������λ�õļ�*/
    for (iTmp3 = iaStartOffset[iIndex] + 1; iTmp3 < iaEndOffset[iIndex]; iTmp3++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp3 * sizeof(KEY_POINT));
      iRet = managerSelectKeyValueDealGetDataOther( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iIndex + 1, iaTotalLoops, iaIndepentFileId );
      if (iRet)
      {
    	  return iRet;
      }
      logFmtSyn( LVNOR, "selectResult->llRows[%lld]llSelectIndex[%lld]", selectResult->llRows, llaSelectIndex );
      /*������������ݶ�ȡ����*/
      if (selectResult->llRows == EACH_SELECT_SIZE)
      {
    	  return SUCCESS;
      }
    }
    
    /*�������λ�õļ�*/
    key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iIndex] * sizeof(KEY_POINT));
    iRet = managerSelectKeyValueDealGetDataRight( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaEndOffset, iIndex + 1, iaTotalLoops, iaIndepentFileId );
    if (iRet)
    {
    	return iRet;
    }
    //logFmtSyn( LVNOR, "selectResult->llRows[%lld]llSelectIndex[%lld]", selectResult->llRows, llSelectIndex );
  }
  
  return SUCCESS;
}

/**********************************************************************
��������: managerSelectKeyValueDealGetDataLeft
��������: ����˴��� select key �еĻ�ȡ����
��    ����
��    һ���̹߳������     I
��    �������ݵ�ַ         I
��    ������һ������       I
��    �ģ���ʼλ��         I
��    �壺����             I
��    �����ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161109
**********************************************************************/
int managerSelectKeyValueDealGetDataLeft( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId )
{
	char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
	KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
	KEY_POINT *key;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iTmp;
	int  iRet;
	
	if (selectShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}
		
	/*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
	  /*�������ָ����λ��*/
	  if((*llaSelectIndex) + iaSize - iaStartOffset[iaLoop] >= selectResult->llStartIndex)
	  {
	    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
      getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
      
		  for (iTmp = iaStartOffset[iaLoop]; iTmp < iaSize; iTmp++ )
	    {
	    	/*�������ָ����λ��*/
	    	if (*llaSelectIndex >= selectResult->llStartIndex )
	    	{
		      /*���ȡ���㹻�Ĳ�ѯ����*/
          if (selectResult->llRows >= EACH_SELECT_SIZE)
          {
          	return SUCCESS;
          }
	        else
	        {
	        	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iTmp * sizeof(KEY_VALUE_POINT));
	        	if (selectResult->iKeyValueFlag == SELECT_KEY)
	          {
	        	  /*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
              valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
              pcKey[keyData->llKeyLen] = 0x00;
              MANAGER_SELECT_LOCK();
                iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, 0, NULL );
              MANAGER_SELECT_UNLOCK();
              /*��������쳣*/
              if (iRet)
              {
              	return INTERRUPT;
              }
            }
            else
            if (selectResult->iKeyValueFlag == SELECT_VALUE)
	          {
	          	/*�� VALUE �ļ��л�ȡ VALUE ��ֵ*/
              valueToBuf( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentFileId );
              pcValue[keyData->llValueLen] = 0x00;          
              
              /*���� VALUE ��ֵ*/
	      	    MANAGER_SELECT_LOCK();
	      	      iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, 0, NULL, keyData->llValueLen, pcValue );
	      	    MANAGER_SELECT_UNLOCK();
	      	    /*��������쳣*/
              if (iRet)
              {
              	return INTERRUPT;
              }
	          }
	          else
	          /*if (selectResult->iKeyValueFlag == SELECT_KEY_VALUE)*/
	          {
	          	/*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
              valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
              pcKey[keyData->llKeyLen] = 0x00;
              
              /*�� VALUE �ļ��л�ȡ VALUE ��ֵ*/
              valueToBuf( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentFileId );
              pcValue[keyData->llValueLen] = 0x00;
              /*���� KEY VALUE ��ֵ*/
	      	    MANAGER_SELECT_LOCK();
                iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, keyData->llValueLen, pcValue );logFmtSyn( LVNOR, "%d", __LINE__ );
              MANAGER_SELECT_UNLOCK();
              /*��������쳣*/
              if (iRet)
              {
              	return INTERRUPT;
              }
	          }
            selectResult->llRows++;
	        }
	      }
	      else
	      {
	        (*llaSelectIndex)++;
	      }
	    }
	  }
	  else
	  {
	    *llaSelectIndex += iaSize - iaStartOffset[iaLoop];
	  }
	  
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iaLoop] * sizeof(KEY_POINT));
    
    iRet = managerSelectKeyValueDealGetDataLeft( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaStartOffset, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
    if (iRet)
    {
    	return INTERRUPT;
    }
    
    if (selectResult->llRows >= EACH_SELECT_SIZE)
    {
    	return SUCCESS;
    }
    for (iTmp = iaStartOffset[iaLoop] + 1; iTmp < iaSize; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerSelectKeyValueDealGetDataOther( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
      
      if (selectResult->llRows >= EACH_SELECT_SIZE)
      {
      	return SUCCESS;
      }
    }
	}
	
	return SUCCESS;
}

/**********************************************************************
��������: managerSelectKeyValueDealGetDataOther
��������: ����˴��� select key �еĻ�ȡ����
��    ����
��    һ���̹߳������     I
��    �������ݵ�ַ         I
��    ������һ������       I
��    �ģ�����             I
��    �壺�ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161109
**********************************************************************/
int managerSelectKeyValueDealGetDataOther( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId )
{
	char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
	KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
	KEY_POINT *key;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iTmp;
	int  iRet;
	
	if (selectShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}
	
	/*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
	  /*�������ָ����λ��*/
	  if((*llaSelectIndex) + iaSize >= selectResult->llStartIndex)
	  {
	    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
      getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
      
		  for (iTmp = 0; iTmp < iaSize; iTmp++ )
	    {
	    	/*�������ָ����λ��*/
	    	if (*llaSelectIndex >= selectResult->llStartIndex )
	    	{
		      /*���ȡ���㹻�Ĳ�ѯ����*/
          if (selectResult->llRows >= EACH_SELECT_SIZE)
          {
          	return SUCCESS;
          }
	        else
	        {
	        	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iTmp * sizeof(KEY_VALUE_POINT));
	        	
	        	if (selectResult->iKeyValueFlag == SELECT_KEY)
	          {
	        	  /*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
              valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
              pcKey[keyData->llKeyLen] = 0x00;
              
              MANAGER_SELECT_LOCK();
                iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, 0, NULL );
              MANAGER_SELECT_UNLOCK();
              /*��������쳣*/
              if (iRet)
              {
              	return INTERRUPT;
              }
            }
            else
            if (selectResult->iKeyValueFlag == SELECT_VALUE)
	          {
	          	/*�� VALUE �ļ��л�ȡ VALUE ��ֵ*/
              valueToBuf( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentFileId );
              pcValue[keyData->llValueLen] = 0x00;          
              
              /*���� VALUE ��ֵ*/
	      	    MANAGER_SELECT_LOCK();
	      	      iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, 0, NULL, keyData->llValueLen, pcValue );
	      	    MANAGER_SELECT_UNLOCK();
	      	    /*��������쳣*/
              if (iRet)
              {
              	return INTERRUPT;
              }
	          }
	          else
	          /*if (selectResult->iKeyValueFlag == SELECT_KEY_VALUE)*/
	          {
	          	/*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
              valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
              pcKey[keyData->llKeyLen] = 0x00;
              
              /*�� VALUE �ļ��л�ȡ VALUE ��ֵ*/
              valueToBuf( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentFileId );
              pcValue[keyData->llValueLen] = 0x00;
              
              /*���� KEY VALUE ��ֵ*/
	      	    MANAGER_SELECT_LOCK();
                iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, keyData->llValueLen, pcValue );
              MANAGER_SELECT_UNLOCK();
              /*��������쳣*/
              if (iRet)
              {
              	return INTERRUPT;
              }
	          }
            selectResult->llRows++;
	        }
	      }
	      else
	      {
	        (*llaSelectIndex)++;
	      }
	    }
	  }
	  else
	  {
	    *llaSelectIndex += iaSize;
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
      iRet = managerSelectKeyValueDealGetDataOther( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
      
      if (selectResult->llRows >= EACH_SELECT_SIZE)
      {
      	return SUCCESS;
      }
    }
	}
	
	return SUCCESS;
}

/**********************************************************************
��������: managerSelectKeyValueDealGetDataRight
��������: ����˴��� select key �еĻ�ȡ����
��    ����
��    һ���̹߳������     I
��    �������ݵ�ַ         I
��    ������һ������       I
��    �ģ���ʼλ��         I
��    �壺����             I
��    �����ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161109
**********************************************************************/
int managerSelectKeyValueDealGetDataRight( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex,  __int64 iaAddr, __int64 iaSize, int iaEndOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId )
{
	char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
	KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
	KEY_POINT *key;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iTmp;
	int  iRet;
	
	if (selectShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}

	/*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
	  /*�������ָ����λ��*/
	  if( (*llaSelectIndex) + iaEndOffset[iaLoop] + 1 >= selectResult->llStartIndex)
	  {
	    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
      getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
      
		  for (iTmp = 0; iTmp <= iaEndOffset[iaLoop]; iTmp++ )
	    {
	    	/*�������ָ����λ��*/
	    	if (*llaSelectIndex >= selectResult->llStartIndex )
	    	{
		      /*���ȡ���㹻�Ĳ�ѯ����*/
          if (selectResult->llRows >= EACH_SELECT_SIZE)
          {
          	return SUCCESS;
          }
	        else
	        {
	        	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iTmp * sizeof(KEY_VALUE_POINT));
	        	
	        	if (selectResult->iKeyValueFlag == SELECT_KEY)
	          {
	        	  /*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
              valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
              pcKey[keyData->llKeyLen] = 0x00;
              
              MANAGER_SELECT_LOCK();
                iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, 0, NULL );
              MANAGER_SELECT_UNLOCK();
              /*��������쳣*/
              if (iRet)
              {
              	return INTERRUPT;
              }
            }
            else
            if (selectResult->iKeyValueFlag == SELECT_VALUE)
	          {
	          	/*�� VALUE �ļ��л�ȡ VALUE ��ֵ*/
              valueToBuf( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentFileId );
              pcValue[keyData->llValueLen] = 0x00;          
              
              /*���� VALUE ��ֵ*/
	      	    MANAGER_SELECT_LOCK();
	      	      iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, 0, NULL, keyData->llValueLen, pcValue );
	      	    MANAGER_SELECT_UNLOCK();
	      	    /*��������쳣*/
              if (iRet)
              {
              	return INTERRUPT;
              }
	          }
	          else
	          /*if (selectResult->iKeyValueFlag == SELECT_KEY_VALUE)*/
	          {
	          	/*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
              valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
              pcKey[keyData->llKeyLen] = 0x00;
              
              /*�� VALUE �ļ��л�ȡ VALUE ��ֵ*/
              valueToBuf( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentFileId );
              pcValue[keyData->llValueLen] = 0x00;
              
              /*���� KEY VALUE ��ֵ*/
	      	    MANAGER_SELECT_LOCK();
                iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, keyData->llValueLen, pcValue );
              MANAGER_SELECT_UNLOCK();
              /*��������쳣*/
              if (iRet)
              {
              	return INTERRUPT;
              }
	          }
            selectResult->llRows++;
	        }
	      }
	      else
	      {
	        (*llaSelectIndex)++;
	      }
	    }
	  }
	  else
	  {
	    *llaSelectIndex += iaEndOffset[iaLoop] + 1;
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
      iRet = managerSelectKeyValueDealGetDataOther( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
      
      if (selectResult->llRows >= EACH_SELECT_SIZE)
      {
      	return SUCCESS;
      }
    }
    
    key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iaLoop] * sizeof(KEY_POINT));
    
    iRet = managerSelectKeyValueDealGetDataRight( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaEndOffset, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
    if (iRet)
    {
    	return INTERRUPT;
    }
    if (selectResult->llRows >= EACH_SELECT_SIZE)
    {
    	return SUCCESS;
    }
	}
	
	return SUCCESS;
}

/**********************************************************************
��������: managerSelectKeyValueInsertKeyValuePoint
��������: ��������ݲ��뵽���������
��    ����
��    һ����ѯID           I
��    ����KEY ����         I
��    ����KEY ������       I
��    �ģ�VALUE ����       I
��    �壺VALUE ������     I
��    �أ�
��    ��: �����
����ʱ��: 20170108
**********************************************************************/
int managerSelectKeyValueInsertKeyValuePoint( __int64 llaSelectId, int iaKeyLen, char *pcaKey, int iaValueLen, char *pcaValue )
{
  SELECT_KEY_VALUE_RESULT *skvr;             /*��ѯ���*/
  SELECT_RESULT_BUF *srb;
  //logFmtSyn( LVNOR, "pcaKey[%s]pcaValue[%s]", pcaKey, pcaValue );
	/*�������*/
	srb = sgResultBuf;
	while (srb != NULL)
	{
		if (srb->llSelectId == llaSelectId)
		{
			break;
		}
		else
		{
			srb = srb->next;
		}
	}
	/*������ݳ����쳣����������Ϊ�ͻ��˳�ʱ��û�ͷ����ͨѶ���ֻ����ǿͻ���û�������������˵�ͨѶ����������������ͻ���Ӧ�����·����ѯ����ȡ��ѯID*/
	if (srb == NULL)
	{
    return ERROR;
	}
	
	if (srb->skvr == NULL)
	{
	  /*���û���㹻�Ŀռ䣬�ͷŽ���еĽ������*/
  	SELECT_MALLOC( srb->skvr, llaSelectId, sizeof(SELECT_KEY_VALUE_RESULT) );
	  if (srb->skvr == NULL)
	  {
	    return ERROR;
	  }
	  /*����� KEY Ҫ����*/
	  if (iaKeyLen != 0 && pcaKey != NULL)
	  {
	    srb->skvr->iKeyLen = iaKeyLen;
	    
	    SELECT_MALLOC( srb->skvr->pcKey, llaSelectId, iaKeyLen + 1 );
	    if (srb->skvr->pcKey == NULL)
	    {
	      SELECT_FREE( srb->skvr, sizeof(SELECT_KEY_VALUE_RESULT) );
	      if (srb->skvr->pcValue != NULL)
	      {
	        SELECT_FREE( srb->skvr->pcValue, srb->skvr->iValueLen + 1 );
	      }
	      
	      return ERROR;
	    }
	    memcpy( srb->skvr->pcKey, pcaKey, iaKeyLen );
	  }
	  
	  /*����� VALUE Ҫ����*/
	  if (iaValueLen != 0 && pcaValue != NULL)
	  {
	    srb->skvr->iValueLen = iaValueLen;
	    
	    SELECT_MALLOC( srb->skvr->pcValue, llaSelectId, iaValueLen + 1 );
	    if (srb->skvr->pcValue == NULL)
	    {
	      SELECT_FREE( srb->skvr, sizeof(SELECT_KEY_VALUE_RESULT) );
	      if (srb->skvr->pcKey != NULL)
	      {
	        SELECT_FREE( srb->skvr->pcKey, srb->skvr->iKeyLen + 1 );
	      }
	      
	      return ERROR;
	    }
	    memcpy( srb->skvr->pcValue, pcaValue, iaValueLen );
	  }
	  srb->skvrLast = srb->skvr;
	}
	else
	{
		if (srb->skvrLast == srb->skvr)
		{
			SELECT_MALLOC( srb->skvrLast, llaSelectId, sizeof(SELECT_KEY_VALUE_RESULT) );
	    if (srb->skvrLast == NULL)
	    {
	      return ERROR;
	    }
		  
		  /*����� KEY Ҫ����*/
	    if (iaKeyLen != 0 && pcaKey != NULL)
	    {
	      srb->skvrLast->iKeyLen = iaKeyLen;
	      
	      SELECT_MALLOC( srb->skvrLast->pcKey, llaSelectId, iaKeyLen + 1 );
	      if (srb->skvrLast->pcKey == NULL)
	      {
	        SELECT_FREE( srb->skvrLast, sizeof(SELECT_KEY_VALUE_RESULT) );
	        if (srb->skvrLast->pcValue != NULL)
	        {
	          SELECT_FREE( srb->skvrLast->pcValue, srb->skvrLast->iValueLen + 1 );
	        }
	        
	        return ERROR;
	      }
	      memcpy( srb->skvrLast->pcKey, pcaKey, iaKeyLen );
	    }
	    
	    /*����� VALUE Ҫ����*/
	    if (iaValueLen != 0 && pcaValue != NULL)
	    {
	      srb->skvrLast->iValueLen = iaValueLen;
	      
	      SELECT_MALLOC( srb->skvrLast->pcValue, llaSelectId, iaValueLen + 1 );
	      if (srb->skvrLast->pcValue == NULL)
	      {
	        SELECT_FREE( srb->skvrLast, sizeof(SELECT_KEY_VALUE_RESULT) );
	        if (srb->skvrLast->pcKey != NULL)
	        {
	          SELECT_FREE( srb->skvrLast->pcKey, srb->skvrLast->iKeyLen + 1 );
	        }
	        
	        return ERROR;
	      }
	      memcpy( srb->skvrLast->pcValue, pcaValue, iaValueLen );
	    }
	    
	    srb->skvr->next = srb->skvrLast;
		}
		else
		{
			SELECT_MALLOC( srb->skvrLast->next, llaSelectId, sizeof(SELECT_KEY_VALUE_RESULT) );
	    if (srb->skvrLast->next == NULL)
	    {
	      return ERROR;
	    }
		  
		  /*����� KEY Ҫ����*/
	    if (iaKeyLen != 0 && pcaKey != NULL)
	    {
	      srb->skvrLast->next->iKeyLen = iaKeyLen;
	      
	      SELECT_MALLOC( srb->skvrLast->next->pcKey, llaSelectId, iaKeyLen + 1 );
	      if (srb->skvrLast->next->pcKey == NULL)
	      {
	        SELECT_FREE( srb->skvrLast->next, sizeof(SELECT_KEY_VALUE_RESULT) );
	        if (srb->skvrLast->next->pcValue != NULL)
	        {
	          SELECT_FREE( srb->skvrLast->next->pcValue, srb->skvrLast->next->iValueLen + 1 );
	        }
	        
	        return ERROR;
	      }
	      memcpy( srb->skvrLast->next->pcKey, pcaKey, iaKeyLen );
	    }
	    
	    /*����� VALUE Ҫ����*/
	    if (iaValueLen != 0 && pcaValue != NULL)
	    {
	      srb->skvrLast->next->iValueLen = iaValueLen;
	      
	      SELECT_MALLOC( srb->skvrLast->next->pcValue, llaSelectId, iaValueLen + 1 );
	      if (srb->skvrLast->next->pcValue == NULL)
	      {
	        SELECT_FREE( srb->skvrLast->next, sizeof(SELECT_KEY_VALUE_RESULT) );
	        if (srb->skvrLast->next->pcKey != NULL)
	        {
	          SELECT_FREE( srb->skvrLast->next->pcKey, srb->skvrLast->next->iKeyLen + 1 );
	        }
	        
	        return ERROR;
	      }
	      memcpy( srb->skvrLast->next->pcValue, pcaValue, iaValueLen );
	    }
	    
	    srb->skvrLast = srb->skvrLast->next;
		}
	}
	
	return SUCCESS;
}

/**********************************************************************
��������: managerSelectKeyValueDealScanfValueMain
��������: ����˴��� select key where value[] �еĽ��
��    ����
��    һ���̹߳������     I/O
��    �������ַ           I
��    ����������           I
��    �ģ�����             I
��    �壺�ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161109
**********************************************************************/
int managerSelectKeyValueDealScanfValueMain( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId )
{
	char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
	KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
	KEY_POINT *key;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iTmp;
	int  iRet;
	
	if (selectShare->iCmd != RUN_CMD_RUN)
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
	      	
	    /*�� VALUE �ļ��л�ȡ VALUE ��ֵ*/
      valueToBufMain( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentId );
      pcValue[keyData->llValueLen] = 0x00;
	    
	    if (!strcmp( selectResult->pcValueType, VALUE_OPEN_OPEN ))
	    {
	    	/*�������������*/
	    	if (!(strcmp( pcValue, selectResult->pcKeyValueStart ) > 0 && strcmp( pcValue, selectResult->pcKeyValueEnd ) < 0))
	    	{
	    		continue;
	    	}
	    }
	    else
	    if (!strcmp( selectResult->pcValueType, VALUE_OPEN_CLOSE ))
	    {
	    	/*�������������*/
	    	if (!(strcmp( pcValue, selectResult->pcKeyValueStart ) > 0 && strcmp( pcValue, selectResult->pcKeyValueEnd ) <= 0))
	    	{
	    		continue;
	    	}
	    }
	    else
	    if (!strcmp( selectResult->pcValueType, VALUE_CLOSE_OPEN ))
	    {
	    	/*�������������*/
	    	if (!(strcmp( pcValue, selectResult->pcKeyValueStart ) >= 0 && strcmp( pcValue, selectResult->pcKeyValueEnd ) < 0))
	    	{
	    		continue;
	    	}
	    }
	    else
	    /*if (!strcmp( selectResult->pcValueType, VALUE_CLOSE_CLOSE ))*/
	    {
	    	/*�������������*/
	    	if (strcmp( pcValue, selectResult->pcKeyValueStart ) >= 0 && strcmp( pcValue, selectResult->pcKeyValueEnd ) <= 0)
	    	{
	    		continue;
	    	}
	    }
	      	
	  	/*�������ָ����λ��*/
	  	if (*llaSelectIndex >= selectResult->llStartIndex )
	  	{
		    /*���ȡ���㹻�Ĳ�ѯ����*/
        if (selectResult->llRows >= EACH_SELECT_SIZE)
        {
        	return SUCCESS;
        }
	      else
	      {
	      	if (selectResult->iKeyValueFlag == SELECT_KEY)
	      	{
	      	  /*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
            valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
            pcKey[keyData->llKeyLen] = 0x00;
            
            MANAGER_SELECT_LOCK();
              iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, 0, NULL );logFmtSyn(LVNOR,"a");
            MANAGER_SELECT_UNLOCK();
            /*��������쳣*/
            if (iRet)
            {
            	return INTERRUPT;
            }
          }
          else
          if (selectResult->iKeyValueFlag == SELECT_VALUE)
	      	{
	      	  /*���� VALUE ��ֵ*/
	      	  MANAGER_SELECT_LOCK();
	      	    iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, 0, NULL, keyData->llValueLen, pcValue );logFmtSyn(LVNOR,"a");
	      	  MANAGER_SELECT_UNLOCK();
	      	  /*��������쳣*/
            if (iRet)
            {
            	return INTERRUPT;
            }
          }
          else
          /*if (selectResult->iKeyValueFlag == SELECT_KEY_VALUE)*/
	      	{
	      		/*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
            valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
            pcKey[keyData->llKeyLen] = 0x00;
            
	      	  /*���� KEY VALUE ��ֵ*/
	      	  MANAGER_SELECT_LOCK();
              iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, keyData->llValueLen, pcValue );logFmtSyn(LVNOR,"a");
            MANAGER_SELECT_UNLOCK();
            /*��������쳣*/
            if (iRet)
            {
            	return INTERRUPT;
            }
          }
          selectResult->llRows++;
	      }
	    }
	    
	    (*llaSelectIndex)++;
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
      iRet = managerSelectKeyValueDealScanfValueMain( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
    }
	}
	
	return SUCCESS;
}

/**********************************************************************
��������: managerSelectKeyValueDealResultMain
��������: ����˴��� select key �еĽ��
��    ����
��    һ���̹߳������     I/O
��    ������ʼλ��         I
��    ��������λ��         I
��    �ģ����ݵ�ַ         I
��    �壺�ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161108
**********************************************************************/
int managerSelectKeyValueDealResultMain( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, int iaStartOffset[], int iaEndOffset[], __int64 iaAddr, __int64 iaTotalLoops, int iaIndepentId )
{
	SELECT_COUNT_SHARE *selectCountShare;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	KEY_POINT *key;                /*�����������е����ݽṹ*/
	KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
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
  	/*�����ѯ��ʼλ��С�ڻ���ڲ�ѯ���������������ݷ��뵽������*/
  	if (selectResult->llStartIndex <= 1)/*����� 1 ��ָ��ǰ�Ĳ�ѯ���ֻ��1��*/
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
  	  
  	  /*��������*/
      if (selectResult->iKeyValueFlag == SELECT_KEY)
	    {
	      /*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
        valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
        pcKey[keyData->llKeyLen] = 0x00;
        
        MANAGER_SELECT_LOCK();
          iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, 0, NULL );
        MANAGER_SELECT_UNLOCK();
        /*��������쳣*/
        if (iRet)
        {
        	return iRet;
        }
      }
      else
      if (selectResult->iKeyValueFlag == SELECT_VALUE)
	    {
	    	/*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
        valueToBufMain( pcKey, keyData->llValueAddr, keyData->llValueLen, iaIndepentId );
        pcValue[keyData->llValueLen] = 0x00;
        
	      /*���� VALUE ��ֵ*/
	      MANAGER_SELECT_LOCK();
	        iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, 0, NULL, keyData->llValueLen, pcValue );
	      MANAGER_SELECT_UNLOCK();
	      /*��������쳣*/
        if (iRet)
        {
        	return iRet;
        }
      }
      else
      /*if (selectResult->iKeyValueFlag == SELECT_KEY_VALUE)*/
	    {
	    	/*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
        valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
        pcKey[keyData->llKeyLen] = 0x00;
        
        /*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
        valueToBufMain( pcKey, keyData->llValueAddr, keyData->llValueLen, iaIndepentId );
        pcValue[keyData->llValueLen] = 0x00;
        
	      /*���� KEY VALUE ��ֵ*/
	      MANAGER_SELECT_LOCK();
          iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, keyData->llValueLen, pcValue );
        MANAGER_SELECT_UNLOCK();
        /*��������쳣*/
        if (iRet)
        {
        	return iRet;
        }
      }
      
      selectResult->llRows += 1;
  	}
  	return SUCCESS;
  }

  //logFmtSyn( LVNOR, "��ʼ��ȡ����" );

  /*ȡ����ǰ��������*/
  iAddr = iaAddr;
  memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
  getKeyBlockDataMain( pcDataBuf, iAddr, iaIndepentId );

  for (iTmp = 2; iTmp < iIndex; iTmp++)
  {
  	memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iAddr, iaIndepentId );
    
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iTmp] * sizeof(KEY_POINT));
    
    iAddr = key->llChildAddr;
  }
 
  /*�Ӹü���ʼ����ʼλ�úͽ���λ�ò�ͬ*/
  
  /*��ȡ����*/
  {
    /*����ʼλ�õļ�*/
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iIndex] * sizeof(KEY_POINT));
    iRet = managerSelectKeyValueDealGetDataLeftMain( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaStartOffset, iIndex + 1, iaTotalLoops, iaIndepentId );
    /*������жϽ���*/
    if (iRet)
    {
    	return iRet;
    }
//logFmtSyn( LVNOR, "selectResult->llRows[%lld]llSelectIndex[%lld]", selectResult->llRows, *llaSelectIndex );
    /*������������ݶ�ȡ����*/
    if (selectResult->llRows >= EACH_SELECT_SIZE)
    {
    	return SUCCESS;
    }

    /*��������λ�õļ�*/
    for (iTmp3 = iaStartOffset[iIndex] + 1; iTmp3 < iaEndOffset[iIndex]; iTmp3++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp3 * sizeof(KEY_POINT));
      iRet = managerSelectKeyValueDealGetDataOtherMain( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iIndex + 1, iaTotalLoops, iaIndepentId );
      if (iRet)
      {
    	  return iRet;
      }
      /*������������ݶ�ȡ����*/
      if (selectResult->llRows == EACH_SELECT_SIZE)
      {
    	  return SUCCESS;
      }
    }
//logFmtSyn( LVNOR, "selectResult->llRows[%lld]llSelectIndex[%lld]", selectResult->llRows, *llaSelectIndex );
    /*�������λ�õļ�*/
    key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iIndex] * sizeof(KEY_POINT));
    iRet = managerSelectKeyValueDealGetDataRightMain( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaEndOffset, iIndex + 1, iaTotalLoops, iaIndepentId );
    if (iRet)
    {
    	return iRet;
    }
  }
//logFmtSyn( LVNOR, "selectResult->llRows[%lld]llSelectIndex[%lld]", selectResult->llRows, *llaSelectIndex );  
  return SUCCESS;
}

/**********************************************************************
��������: managerSelectKeyValueDealGetDataLeftMain
��������: ����˴��� select key �еĻ�ȡ����
��    ����
��    һ���̹߳������     I
��    �������ݵ�ַ         I
��    ������һ������       I
��    �ģ���ʼλ��         I
��    �壺����             I
��    �����ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161109
**********************************************************************/
int managerSelectKeyValueDealGetDataLeftMain( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId )
{
	char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
	KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
	KEY_POINT *key;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iTmp;
	int  iRet;

	if (selectShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}
//logFmtSyn( LVNOR, "%lld:%lld", iaLoop, iaTotalLoops );
	/*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
		//logFmtSyn( LVNOR, "%lld:%lld", *llaSelectIndex, selectResult->llStartIndex );
	  /*�������ָ����λ��*/
	  if((*llaSelectIndex) + iaSize - iaStartOffset[iaLoop] >= selectResult->llStartIndex)
	  {
	    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
      getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );

		  for (iTmp = iaStartOffset[iaLoop]; iTmp < iaSize; iTmp++ )
	    {
	    	/*�������ָ����λ��*/
	    	if (*llaSelectIndex >= selectResult->llStartIndex )
	    	{
		      /*���ȡ���㹻�Ĳ�ѯ����*/
          if (selectResult->llRows >= EACH_SELECT_SIZE)
          {
          	return SUCCESS;
          }
	        else
	        {
	        	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iTmp * sizeof(KEY_VALUE_POINT));
	        	
	        	if (selectResult->iKeyValueFlag == SELECT_KEY)
	          {
	        	  /*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
              valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
              pcKey[keyData->llKeyLen] = 0x00;
              MANAGER_SELECT_LOCK();
                iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, 0, NULL );
              MANAGER_SELECT_UNLOCK();
              /*��������쳣*/
              if (iRet)
              {
              	return INTERRUPT;
              }
            }
            else
            if (selectResult->iKeyValueFlag == SELECT_VALUE)
	          {
	          	/*�� VALUE �ļ��л�ȡ VALUE ��ֵ*/
              valueToBufMain( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentId );
              pcValue[keyData->llValueLen] = 0x00;     
              
              //logFmtSyn( LVNOR, "pcValue[%s][%d]", pcValue, keyData->llValueLen );     
              
              /*���� VALUE ��ֵ*/
	      	    MANAGER_SELECT_LOCK();
	      	      iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, 0, NULL, keyData->llValueLen, pcValue );
	      	    MANAGER_SELECT_UNLOCK();
	      	    /*��������쳣*/
              if (iRet)
              {
              	return INTERRUPT;
              }
	          }
	          else
	          /*if (selectResult->iKeyValueFlag == SELECT_KEY_VALUE)*/
	          {
	          	/*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
              valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
              pcKey[keyData->llKeyLen] = 0x00;
              
              /*�� VALUE �ļ��л�ȡ VALUE ��ֵ*/
              valueToBufMain( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentId );
              pcValue[keyData->llValueLen] = 0x00;
              /*���� KEY VALUE ��ֵ*/
	      	    MANAGER_SELECT_LOCK();
                iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, keyData->llValueLen, pcValue );
              MANAGER_SELECT_UNLOCK();
              /*��������쳣*/
              if (iRet)
              {
              	return INTERRUPT;
              }
	          }
            selectResult->llRows++;
	        }
	      }
	      else
	      {
	        (*llaSelectIndex)++;
	      }
	    }
	  }
	  else
	  {
	    *llaSelectIndex += iaSize - iaStartOffset[iaLoop];
	  }
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iaLoop] * sizeof(KEY_POINT));
    
    iRet = managerSelectKeyValueDealGetDataLeftMain( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaStartOffset, iaLoop + 1, iaTotalLoops, iaIndepentId );
    if (iRet)
    {
    	return INTERRUPT;
    }
    if (selectResult->llRows >= EACH_SELECT_SIZE)
    {
    	return SUCCESS;
    }
    for (iTmp = iaStartOffset[iaLoop] + 1; iTmp < iaSize; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerSelectKeyValueDealGetDataOtherMain( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
      
      if (selectResult->llRows >= EACH_SELECT_SIZE)
      {
      	return SUCCESS;
      }
    }
	}
	
	return SUCCESS;
}

/**********************************************************************
��������: managerSelectKeyValueDealGetDataOtherMain
��������: ����˴��� select key �еĻ�ȡ����
��    ����
��    һ���̹߳������     I
��    �������ݵ�ַ         I
��    ������һ������       I
��    �ģ�����             I
��    �壺�ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161109
**********************************************************************/
int managerSelectKeyValueDealGetDataOtherMain( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex, __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId )
{
	char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
	KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
	KEY_POINT *key;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iTmp;
	int  iRet;
	
	//logFmtSyn( LVNOR, "selectResult->llRows[%lld]llSelectIndex[%lld]", selectResult->llRows, *llaSelectIndex );
	if (selectShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}
	
	/*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
		//logFmtSyn( LVNOR, "selectResult->llRows[%lld]llSelectIndex[%lld]", selectResult->llRows, *llaSelectIndex );
	  /*�������ָ����λ��*/
	  if((*llaSelectIndex) + iaSize >= selectResult->llStartIndex)
	  {
	    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
      getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
      
		  for (iTmp = 0; iTmp < iaSize; iTmp++ )
	    {
	    	/*�������ָ����λ��*/
	    	if (*llaSelectIndex >= selectResult->llStartIndex )
	    	{
		      /*���ȡ���㹻�Ĳ�ѯ����*/
          if (selectResult->llRows >= EACH_SELECT_SIZE)
          {
          	return SUCCESS;
          }
	        else
	        {
	        	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iTmp * sizeof(KEY_VALUE_POINT));
	        	
	        	if (selectResult->iKeyValueFlag == SELECT_KEY)
	          {
	        	  /*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
              valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
              pcKey[keyData->llKeyLen] = 0x00;
              
              MANAGER_SELECT_LOCK();
                iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, 0, NULL );
              MANAGER_SELECT_UNLOCK();
              /*��������쳣*/
              if (iRet)
              {
              	return INTERRUPT;
              }
            }
            else
            if (selectResult->iKeyValueFlag == SELECT_VALUE)
	          {
	          	/*�� VALUE �ļ��л�ȡ VALUE ��ֵ*/
              valueToBufMain( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentId );
              pcValue[keyData->llValueLen] = 0x00;          
              
              /*���� VALUE ��ֵ*/
	      	    MANAGER_SELECT_LOCK();
	      	      iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, 0, NULL, keyData->llValueLen, pcValue );
	      	    MANAGER_SELECT_UNLOCK();
	      	    /*��������쳣*/
              if (iRet)
              {
              	return INTERRUPT;
              }
	          }
	          else
	          /*if (selectResult->iKeyValueFlag == SELECT_KEY_VALUE)*/
	          {
	          	/*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
              valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
              pcKey[keyData->llKeyLen] = 0x00;
              
              /*�� VALUE �ļ��л�ȡ VALUE ��ֵ*/
              valueToBufMain( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentId );
              pcValue[keyData->llValueLen] = 0x00;
              
              /*���� KEY VALUE ��ֵ*/
	      	    MANAGER_SELECT_LOCK();
                iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, keyData->llValueLen, pcValue );
              MANAGER_SELECT_UNLOCK();
              /*��������쳣*/
              if (iRet)
              {
              	return INTERRUPT;
              }
	          }
            selectResult->llRows++;
	        }
	      }
	      else
	      {
	        (*llaSelectIndex)++;
	      }
	    }
	  }
	  else
	  {
	    *llaSelectIndex += iaSize;
	  }
	  
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    for (iTmp = 0; iTmp < iaSize; iTmp++)
    {
    	//logFmtSyn( LVNOR, "selectResult->llRows[%lld]llSelectIndex[%lld]", selectResult->llRows, *llaSelectIndex );
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      iRet = managerSelectKeyValueDealGetDataOtherMain( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
      
      if (selectResult->llRows >= EACH_SELECT_SIZE)
      {
      	return SUCCESS;
      }
      //logFmtSyn( LVNOR, "selectResult->llRows[%lld]llSelectIndex[%lld]", selectResult->llRows, *llaSelectIndex );
    }
	}
	
	return SUCCESS;
}

/**********************************************************************
��������: managerSelectKeyValueDealGetDataRightMain
��������: ����˴��� select key �еĻ�ȡ����
��    ����
��    һ���̹߳������     I
��    �������ݵ�ַ         I
��    ������һ������       I
��    �ģ���ʼλ��         I
��    �壺����             I
��    �����ܼ���           I
��    �أ�
��    ��: �����
����ʱ��: 20161109
**********************************************************************/
int managerSelectKeyValueDealGetDataRightMain( SELECT_SHARE *selectShare, SELECT_RESULT_BUF *selectResult, __int64 *llaSelectIndex,  __int64 iaAddr, __int64 iaSize, int iaEndOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId )
{
	char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
	KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
	KEY_POINT *key;
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	int  iTmp;
	int  iRet;
	
	if (selectShare->iCmd != RUN_CMD_RUN)
	{
		return INTERRUPT;
	}

	/*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
	  /*�������ָ����λ��*/
	  if( (*llaSelectIndex) + iaEndOffset[iaLoop] + 1 >= selectResult->llStartIndex)
	  {
	    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
      getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
      
		  for (iTmp = 0; iTmp <= iaEndOffset[iaLoop]; iTmp++ )
	    {
	    	/*�������ָ����λ��*/
	    	if (*llaSelectIndex >= selectResult->llStartIndex )
	    	{
		      /*���ȡ���㹻�Ĳ�ѯ����*/
          if (selectResult->llRows >= EACH_SELECT_SIZE)
          {
          	return SUCCESS;
          }
	        else
	        {
	        	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iTmp * sizeof(KEY_VALUE_POINT));
	        	
	        	if (selectResult->iKeyValueFlag == SELECT_KEY)
	          {
	        	  /*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
              valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
              pcKey[keyData->llKeyLen] = 0x00;
              
              MANAGER_SELECT_LOCK();
                iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, 0, NULL );
              MANAGER_SELECT_UNLOCK();
              /*��������쳣*/
              if (iRet)
              {
              	return INTERRUPT;
              }
            }
            else
            if (selectResult->iKeyValueFlag == SELECT_VALUE)
	          {
	          	/*�� VALUE �ļ��л�ȡ VALUE ��ֵ*/
              valueToBufMain( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentId );
              pcValue[keyData->llValueLen] = 0x00;          
              
              /*���� VALUE ��ֵ*/
	      	    MANAGER_SELECT_LOCK();
	      	      iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, 0, NULL, keyData->llValueLen, pcValue );
	      	    MANAGER_SELECT_UNLOCK();
	      	    /*��������쳣*/
              if (iRet)
              {
              	return INTERRUPT;
              }
	          }
	          else
	          /*if (selectResult->iKeyValueFlag == SELECT_KEY_VALUE)*/
	          {
	          	/*�� VALUE �ļ��л�ȡ KEY ��ֵ*/
              valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
              pcKey[keyData->llKeyLen] = 0x00;
              
              /*�� VALUE �ļ��л�ȡ VALUE ��ֵ*/
              valueToBufMain( pcValue, keyData->llValueAddr, keyData->llValueLen, iaIndepentId );
              pcValue[keyData->llValueLen] = 0x00;
              
              /*���� KEY VALUE ��ֵ*/
	      	    MANAGER_SELECT_LOCK();
                iRet = managerSelectKeyValueInsertKeyValuePoint( selectResult->llSelectId, keyData->llKeyLen, pcKey, keyData->llValueLen, pcValue );
              MANAGER_SELECT_UNLOCK();
              /*��������쳣*/
              if (iRet)
              {
              	return INTERRUPT;
              }
	          }
            selectResult->llRows++;
	        }
	      }
	      else
	      {
	        (*llaSelectIndex)++;
	      }
	    }
	  }
	  else
	  {
	    *llaSelectIndex += iaEndOffset[iaLoop] + 1;
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
      iRet = managerSelectKeyValueDealGetDataOtherMain( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
      if (iRet)
      {
  	    return INTERRUPT;
      }
      
      if (selectResult->llRows >= EACH_SELECT_SIZE)
      {
      	return SUCCESS;
      }
    }
    
    key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iaLoop] * sizeof(KEY_POINT));
    
    iRet = managerSelectKeyValueDealGetDataRightMain( selectShare, selectResult, llaSelectIndex, key->llChildAddr, key->llChildSize, iaEndOffset, iaLoop + 1, iaTotalLoops, iaIndepentId );
    if (iRet)
    {
    	return INTERRUPT;
    }
    if (selectResult->llRows >= EACH_SELECT_SIZE)
    {
    	return SUCCESS;
    }
	}
	
	return SUCCESS;
}

