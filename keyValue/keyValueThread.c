#define __KEY_VALUE_THREAD_C

#ifndef __COMMON_HEAD_H
  #include "commonHead.h"
#endif

void shortSocket( int *iaIndex );
void updateDeleteSingle( char *pcaDataBuf, int iaDataLen, int iaSocket, char *pcaOperateType );
void selectSingle( char *pcaDataBuf, int iDataLen, int iaSocket );
void updateDeleteBat( char *pcaDataBuf, int iaDataLen, int iaSocket, char *pcaOperateType );
void selectBat( char *pcaDataBuf, int iaDataLen, int iaSocket );
void updateDeleteSelectBat( char *pcaDataBuf, int iaDataLen, int iaSocket );
void longSocket( int *iaIndex );
int checkConnectOverTime( char *pcaLastTime );

/**********************************************************************
��������: createServerSocket 
��������: ����SOCKET
��    ����
��    һ���˿�         I
��    ��: �����
����ʱ��: 20160818
**********************************************************************/
int createServerSocket( int iaPort )
{
  struct sockaddr_in server;   /*��������ַ��Ϣ�ṹ��*/
  int    iOpt;
  int    iSocket;
  
  #ifdef SYSTEM_WINDOWS_64
    /*����socket*/
    iSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP); 
    if (iSocket == INVALID_SOCKET)
    {
  	  fprintf( stderr, CREATE_SOCKET_ERRMSG, GetLastError() );
  	  fprintf( stderr, "%d\n", __LINE__ );
  	  return -1;
    }
    
    /*����socket����*/
    iOpt = SO_REUSEADDR;
    setsockopt(iSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&iOpt, sizeof(iOpt));
    
    /*����socket*/
    memset( &server, 0x00, sizeof(struct sockaddr) );
    server.sin_family = AF_INET;
    server.sin_port = htons(iaPort);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    
    /*�󶨶˿�*/
    if (bind( iSocket, (struct sockaddr *)&server, sizeof(server) ))
    {
      fprintf( stderr, BIND_SOCKET_ERRMSG , GetLastError() );
      closesocket(iSocket);
      fprintf( stderr, "%d\n", __LINE__ );
      return -1;
    }
    
    /*��������*/
    if (listen( iSocket, MAX_LINKS ) == -1)
    {
      fprintf( stderr, LISTEN_SOCKET_ERRMSG, GetLastError() );
      closesocket(iSocket);
      fprintf( stderr, "%d\n", __LINE__ );
      return -1;
    }
    
  #else
    /*����socket*/
    iSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (iSocket == -1)
    {
      fprintf( stderr, CREATE_SOCKET_ERRMSG_UNIX, errno, strerror(errno) );   
  	  return -1;
    }

    /*����socket����*/
    iOpt = SO_REUSEADDR;
    setsockopt(iSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&iOpt, sizeof(iOpt));

    memset(&server, 0x00, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(iaPort);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    /*����bind�󶨵�ַ*/
    if (bind(iSocket, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
      fprintf( stderr, BIND_SOCKET_ERRMSG_UNIX, errno, strerror(errno) );
      return -1;
    }

    /*����listen��ʼ����*/
    if (listen(iSocket, MAX_LINKS) == -1)
    {
      fprintf( stderr, LISTEN_SOCKET_ERRMSG_UNIX, errno, strerror(errno) );
      return -1;
    }
  #endif
  
  return iSocket;
}

/**********************************************************************
��������: waitFd 
��������: �ȴ�SOCKET
��    ����
��    һ��SOCKET      I
��    ��������        I
��    ������ʱʱ��    I
��    �أ�-1ʧ�ܣ������ɹ�
��    ��: �����
����ʱ��: 20160818
**********************************************************************/
int waitFd( int iFd, int iFlag, int iTimeOut )
{
	int iRc;
  fd_set fds;
  struct timeval sTimeval;
  fd_set *fds_read = NULL;
  fd_set *fds_write = NULL;
  struct timeval *tv = NULL;
  
  memset( &sTimeval, 0x00, sizeof(sTimeval) );
  sTimeval.tv_sec = iTimeOut;
  
  FD_ZERO( &fds );
  FD_SET( iFd, &fds );
  
  if (iFlag == WAIT_OUT)
  {
  	fds_write = &fds;
  }
  else
  {
  	fds_read = &fds;
  }
  
  if (iTimeOut >= 0)
  {
  	tv = &sTimeval;
  }
  else
  {
  	tv = NULL;
  }
  
  return select( iFd + 1, fds_read, fds_write, NULL, tv );
}

/**********************************************************************
��������: shortSocket
��������: �����Ӵ�����
��    ����
��    һ��SOCKET         I
��    ��: �����
����ʱ��: 20160917
**********************************************************************/
void shortSocket( int *iaIndex )
{
	struct sockaddr_in server;   /*��������ַ��Ϣ�ṹ��*/
  struct sockaddr_in addr;     /*���յ��Ŀͻ�����Ϣ�ṹ��*/
  int sin_size;                /*��ַ��Ϣ�ṹ���С*/
  int iSocket;
  int iNewSocket;
  int iIndex;
  
	char pcRecvLen[DATA_BUF_LEN + 1];
	char pcOperate[OPERATE_LEN + 1];
  int iRecvLen;
	int iTotalLen;
	int iDataLen;
	int iRet;
	char *pcRecvBuf;
	int iSize = 0;
	
  iIndex = *iaIndex;
  iSocket = sgShortThreadArray[iIndex].iSocket;
  sin_size = sizeof(struct sockaddr);
  
  /*�����̶߳� pthread_cancel �ķ�Ӧ*/
  #ifdef SYSTEM_WINDOWS_64
    /*WINDOWS�²���Ҫ*/
  #else/*#ifdef SYSTEM_UBUNTU*/
    pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL );
    pthread_setcanceltype( PTHREAD_CANCEL_ASYNCHRONOUS, NULL );
  #endif
  
  while (sgShortThreadArray[iIndex].iRunState == THREAD_RUN_STATE_RUN)
  {
  	sgShortThreadArray[iIndex].iAcceptState = ACCEPT_STATE_NO;
  	
    /*����accept�����ط�������ͻ������ӵ�socket������*/
    iNewSocket = accept(iSocket, (struct sockaddr *)&addr, &sin_size);
    
    sgShortThreadArray[iIndex].iAcceptState = ACCEPT_STATE_YES;
    if (iNewSocket == -1)
    {
      break;
    }

	  /*�ܽṹ��������+��������������+����ɾ����ѯ����+KEY����+KEY+VALUE����+VALUE*/
	  /*����10B����*/
	  iTotalLen = 0;
	  iRecvLen = 0;
	  memset( pcRecvLen, 0x00, sizeof(pcRecvLen) );
	  	
	  while (sgShortThreadArray[iIndex].iRunState == THREAD_RUN_STATE_RUN)
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
	  if (sgShortThreadArray[iIndex].iRunState == THREAD_RUN_STATE_STOP)
	  {
	  	closesocket( iNewSocket );
	  	break;
	  }
	  if (iRecvLen <= 0)
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
	 
	  while (sgShortThreadArray[iIndex].iRunState == THREAD_RUN_STATE_RUN)
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
	  if (sgShortThreadArray[iIndex].iRunState == THREAD_RUN_STATE_STOP)
	  {
	  	closesocket( iNewSocket );
	  	free(pcRecvBuf);
	  	break;
	  }
	  if (iRecvLen <= 0)
	  {
	  	closesocket(iNewSocket);
	  	free(pcRecvBuf);
	  	continue;
	  }
	  
	  /*���յ��������У���ʼ4BΪ�������ͣ�UPDATE������DELETE������SELECT������UPDATE������DELETE������SELECT�������������*/
	  /*
	  �������ݽṹ��
	      KEY���ȣ�4B��+KEY+VALUE���ȣ�4B��+VALUE
	  ��������ṹ��
	      UPDATE��DELETE��SELECT
	      KEY���ȣ�4B��+KEY+VALUE���ȣ�4B��+VALUE
	  */
	  
	  /*ȡ��������*/
	  memset( pcOperate, 0x00, sizeof(pcOperate) );
	  memcpy( pcOperate, pcRecvBuf, OPERATE_LEN );
	  
	  /*������������Ӧ�Ĵ���*/
	  if (!strcmp( pcOperate, UPDATE_SINGLE_CNT ) || !strcmp( pcOperate, DELETE_SINGLE_CNT ))
	  {
	  	/*��������+KEY����+KEY+VALUE����+VALUE*/
	  	updateDeleteSingle( pcRecvBuf + OPERATE_LEN, iDataLen - OPERATE_LEN, iNewSocket, pcOperate );
	  }
	  else
	  if (!strcmp( pcOperate, SELECT_SINGLE_CNT ))
	  {
	  	/*KEY����+KEY*/
	  	selectSingle( pcRecvBuf + OPERATE_LEN, iDataLen - OPERATE_LEN, iNewSocket );
	  }
	  else
	  if (!strcmp( pcOperate, UPDATE_BAT_CNT ) || !strcmp( pcOperate, DELETE_BAT_CNT ))
	  {
	  	/*��������+KEY����+KEY+VALUE����+VALUE*/
	  	updateDeleteBat( pcRecvBuf + OPERATE_LEN, iDataLen - OPERATE_LEN, iNewSocket, pcOperate );
	  }
	  else
	  if (!strcmp( pcOperate, SELECT_BAT_CNT ))
	  {
	  	/*KEY����+KEY*/
	  	selectBat( pcRecvBuf + OPERATE_LEN, iDataLen - OPERATE_LEN, iNewSocket );
	  }
	  else
	  if (!strcmp( pcOperate, UP_DE_SE_BAT_CNT ))
	  {
	  	/*��������+KEY����+KEY+VALUE����+VALUE*/
	  	updateDeleteSelectBat( pcRecvBuf + OPERATE_LEN, iDataLen - OPERATE_LEN, iNewSocket );
	  }
	  
	  closesocket(iNewSocket);
	  free( pcRecvBuf );
  }
  
  sgShortThreadArray[iIndex].iRunState = THREAD_RUN_STATE_STOP;
  
  return;
}

/**********************************************************************
��������: longSocket
��������: �����Ӵ�����
��    ����
��    һ��SOCKET         I
��    ��: �����
����ʱ��: 20160918
**********************************************************************/
void longSocket( int *iaIndex )
{
#define LONG_SOCKET_SEND_RESULT( value ) \
{\
	memset( pcResult, 0x00, sizeof(pcResult) );\
	sprintf( pcResult, "%d", value ); \
	cResult = pcResult[0];\
	send( iNewSocket, &cResult, 1, 0 );\
}

	struct sockaddr_in server;   /*��������ַ��Ϣ�ṹ��*/
  struct sockaddr_in addr;     /*���յ��Ŀͻ�����Ϣ�ṹ��*/
  int sin_size;                /*��ַ��Ϣ�ṹ���С*/
  int iSocket;
  int iNewSocket;
  int iIndex;
  
  char cResult;
	char pcResult[INT_LEN + 1];
  
	char pcDateTime[DATE_TIME];
	char pcDate[DATE_LEN + 1];
  char pcTime[TIME_LEN + 1];
	char pcRecvLen[DATA_BUF_LEN + 1];
	char pcOperate[OPERATE_LEN + 1];
  int iRecvLen;
	int iTotalLen;
	int iDataLen;
	char *pcRecvBuf;
	int iRet;
	
	iIndex = *iaIndex;
	iSocket = sgLongThreadArray[iIndex].iSocket;
  sin_size = sizeof(struct sockaddr);
  
  
  while (sgLongThreadArray[iIndex].iRunState == THREAD_RUN_STATE_RUN)
  {
  	sgLongThreadArray[iIndex].iAcceptState = ACCEPT_STATE_NO;
  	
    /*����accept�����ط�������ͻ������ӵ�socket������*/
    iNewSocket = accept(iSocket, (struct sockaddr *)&addr, &sin_size);
    
    sgLongThreadArray[iIndex].iAcceptState = ACCEPT_STATE_YES;
    if (iNewSocket == -1)
    {
      break;
    }

    memset( pcDate, 0x00, sizeof(pcDate) );
	  memset( pcTime, 0x00, sizeof(pcTime) );
	  getDateTime( pcDate, pcTime );
	  
	  memset( pcDateTime, 0x00, sizeof(pcDateTime) );
	  sprintf( pcDateTime, "%s%s", pcDate, pcTime );
	  	    	
    while (sgLongThreadArray[iIndex].iRunState == THREAD_RUN_STATE_RUN)
    {
	    /*�ܽṹ��������+��������������+����ɾ����ѯ����+KEY����+KEY+VALUE����+VALUE*/
		  /*����Ƿ����ӳ�ʱ*/
		  #if 1
		  if (checkConnectOverTime( pcDateTime ))
		  {
		  	closesocket(iNewSocket);
	    	break;
		  }
		  #endif
		  
	  	iRet = waitFd( iNewSocket, WAIT_IN, OVER_TIME );
	  	if (iRet == 0)
	  	{
	  		closesocket(iNewSocket);
	  		break;
	  	}

	    /*����10B����*/
	    iTotalLen = 0;
	    iRecvLen = 0;
	    memset( pcRecvLen, 0x00, sizeof(pcRecvLen) );
	    while (sgLongThreadArray[iIndex].iRunState == THREAD_RUN_STATE_RUN)
	    {
	    	iRecvLen = recv( iNewSocket, pcRecvLen + iTotalLen, DATA_BUF_LEN - iTotalLen, 0 );
	    	if (iRecvLen <= 0)
	    	{
	    		break;
	    	}
	    	
	    	iTotalLen += iRecvLen;
	    	if (iTotalLen == DATA_BUF_LEN)
	    	{
	    		break;
	    	}
	    }
	    if (sgLongThreadArray[iIndex].iRunState == THREAD_RUN_STATE_STOP)
	    {
	    	closesocket( iNewSocket );
	  	  break;
	    }
	    if (iRecvLen <= 0)
	    {
	    	break;
	    }
	    	    
	    /*���㳤��*/
	    iDataLen = atoi( pcRecvLen );

	    /*���������0����ʾ��������*/
	    if (iDataLen == 0)
	    {
	    	memset( pcDate, 0x00, sizeof(pcDate) );
	    	memset( pcTime, 0x00, sizeof(pcTime) );
	    	getDateTime( pcDate, pcTime );
	    	
	    	memset( pcDateTime, 0x00, sizeof(pcDateTime) );
	    	sprintf( pcDateTime, "%s%s", pcDate, pcTime );
	    	LONG_SOCKET_SEND_RESULT(0);
	    	continue;
	    }
      if (sgLongThreadArray[iIndex].iRunState == THREAD_RUN_STATE_STOP)
	    {
	    	closesocket( iNewSocket );
	  	  break;
	    }
	    if (iDataLen <= OPERATE_LEN)
	    {
	    	closesocket( iNewSocket );
	    	break;
	    }
	    
	    /*�����ڴ�*/
	    pcRecvBuf = malloc( iDataLen + 1 );
	    if (pcRecvBuf == NULL)
	    {
	    	closesocket( iNewSocket );
	    	break;
	    }
	    memset( pcRecvBuf, 0x00, iDataLen + 1 );
	    
	    iRet = waitFd( iNewSocket, WAIT_IN, OVER_TIME );
	  	if (iRet == 0)
	  	{
	  		closesocket(iNewSocket);
	  		free( pcRecvBuf );
	  		break;
	  	}
	  	
	    /*��������*/
	    iTotalLen = 0;
	    iRecvLen = 0;
	    while (sgLongThreadArray[iIndex].iRunState == THREAD_RUN_STATE_RUN)
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
	    if (sgLongThreadArray[iIndex].iRunState == THREAD_RUN_STATE_STOP)
	    {
	    	closesocket( iNewSocket );
	    	free( pcRecvBuf );
	  	  break;
	    }
	    if (iRecvLen <= 0)
	    {
	    	closesocket(iNewSocket);
	    	free( pcRecvBuf );
	    	break;
	    }
	  
	    
	    /*���յ��������У���ʼ4BΪ�������ͣ�UPDATE������DELETE������SELECT������UPDATE������DELETE������SELECT�������������*/
	    /*
	    �������ݽṹ��
	        KEY���ȣ�4B��+KEY+VALUE���ȣ�4B��+VALUE
	    ��������ṹ��
	        UPDATE��DELETE��SELECT
	        KEY���ȣ�4B��+KEY+VALUE���ȣ�4B��+VALUE
	    */
	    
	    /*ȡ��������*/
	    memset( pcOperate, 0x00, sizeof(pcOperate) );
	    memcpy( pcOperate, pcRecvBuf, OPERATE_LEN );
	    
	    /*������������Ӧ�Ĵ���*/
	    if (!strcmp( pcOperate, UPDATE_SINGLE_CNT ) || !strcmp( pcOperate, DELETE_SINGLE_CNT ))
	    {
	    	/*��������+KEY����+KEY+VALUE����+VALUE*/
	    	updateDeleteSingle( pcRecvBuf + OPERATE_LEN, iDataLen - OPERATE_LEN, iNewSocket, pcOperate );
	    }
	    else
	    if (!strcmp( pcOperate, SELECT_SINGLE_CNT ))
	    {
	    	/*KEY����+KEY*/
	    	selectSingle( pcRecvBuf + OPERATE_LEN, iDataLen - OPERATE_LEN, iNewSocket );
	    }
	    else
	    if (!strcmp( pcOperate, UPDATE_BAT_CNT ) || !strcmp( pcOperate, DELETE_BAT_CNT ))
	    {
	    	/*��������+KEY����+KEY+VALUE����+VALUE*/
	    	updateDeleteBat( pcRecvBuf + OPERATE_LEN, iDataLen - OPERATE_LEN, iNewSocket, pcOperate );
	    }
	    else
	    if (!strcmp( pcOperate, SELECT_BAT_CNT ))
	    {
	    	/*KEY����+KEY*/
	    	selectBat( pcRecvBuf + OPERATE_LEN, iDataLen - OPERATE_LEN, iNewSocket );
	    }
	    else
	    if (!strcmp( pcOperate, UP_DE_SE_BAT_CNT ))
	    {
	    	/*��������+KEY����+KEY+VALUE����+VALUE*/
	    	updateDeleteSelectBat( pcRecvBuf + OPERATE_LEN, iDataLen - OPERATE_LEN, iNewSocket );
	    }
	  
	    free( pcRecvBuf );
	  }
	  
	  closesocket(iNewSocket);
  }
  
  sgLongThreadArray[iIndex].iRunState = THREAD_RUN_STATE_STOP;
}


/**********************************************************************
��������: updateDeleteSingle
��������: ���������»�ɾ��
��    ����
��    һ�����ݿ�ʼ��ַ      I
��    �������ݳ���          I
��    ����SOCKET            I
��    �ģ���������          I
��    ��: �����
����ʱ��: 20160917
**********************************************************************/
void updateDeleteSingle( char *pcaDataBuf, int iaDataLen, int iaSocket, char *pcaOperateType )
{
#define UPDATE_DELETE_SINGLE_SEND_RESULT( value ) memset( pcResult, 0x00, sizeof(pcResult) );sprintf( pcResult, "%d", value ); cResult = pcResult[0];send( iaSocket, &cResult, 1, 0 );

	char pcKeyValueLen[KEY_VALUE_LENGTH + 1];
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	char cResult;
	char pcResult[INT_LEN + 1];
	int  iIndex;
	int  iKeyLen;
	int  iValueLen;
	int  iResult;
	char *pcBuf;
	int  iIndepentId;
	int  iIndepentFileId;
	
	pcBuf = pcaDataBuf;
	
	iIndex = 0;
		
	/*ȡKEY����*/
	if (iaDataLen - iIndex < KEY_VALUE_LENGTH)
	{
		UPDATE_DELETE_SINGLE_SEND_RESULT( ERROR );
	  return;
	}
	memset( pcKeyValueLen, 0x00, sizeof(pcKeyValueLen) );
	memcpy( pcKeyValueLen, pcBuf + iIndex, KEY_VALUE_LENGTH );
	iKeyLen = atoi( pcKeyValueLen );
	iIndex += KEY_VALUE_LENGTH;
	  
	/*ȡKEY*/
	if (iaDataLen - iIndex < iKeyLen || iKeyLen > MAX_KEY_LEN)
	{
		UPDATE_DELETE_SINGLE_SEND_RESULT( ERROR );
	  return;
	}
	
	/*����KEYֵ*/
  *(pcKey + iKeyLen) = 0x00;
  memcpy( pcKey, pcBuf + iIndex, iKeyLen );
	iIndex += iKeyLen;
	
	iIndepentId = calculateKeyMain( pcKey, iKeyLen );
  
	/*UPDATE������ȡVALUE*/
	if (!strcmp( pcaOperateType, UPDATE_SINGLE_CNT ))
	{
		/*ȡVALUE����*/
		if (iaDataLen - iIndex < KEY_VALUE_LENGTH)
	  {
	  	UPDATE_DELETE_SINGLE_SEND_RESULT( ERROR );
	    return;
	  }
	  memset( pcKeyValueLen, 0x00, sizeof(pcKeyValueLen) );
	  memcpy( pcKeyValueLen, pcBuf + iIndex, KEY_VALUE_LENGTH );
	  iValueLen = atoi( pcKeyValueLen );
	  iIndex += KEY_VALUE_LENGTH;
	  
	  /*ȡVALUE*/
	  if (iaDataLen - iIndex < iValueLen || iValueLen > MAX_VALUE_LEN)
	  {
	  	UPDATE_DELETE_SINGLE_SEND_RESULT( ERROR );
	    return;
	  }

    /*����VALUEֵ*/
    *(pcValue + iValueLen) = 0x00;
    memcpy( pcValue, pcBuf + iIndex, iValueLen );
	  iIndex += iValueLen;
	  
	  iResult = updateNodeMain( pcKey, (__int64)(iKeyLen), pcValue, (__int64)(iValueLen), iIndepentId );
	}
	/*DELETE����*/
	else
	{
		iResult = deleteNodeMain( pcKey, (__int64)(iKeyLen), iIndepentId );
		if (iResult == NOT_FOUND)
    {
    	iIndepentFileId = calculateKey( pcKey, strlen(pcKey) );
    	iResult = deleteNode( pcKey, (__int64)(iKeyLen), iIndepentFileId );
    }
	}
	
	UPDATE_DELETE_SINGLE_SEND_RESULT( iResult );
	
	return;
}

/**********************************************************************
��������: selectSingle
��������: ��������ѯ
��    ����
��    һ�����ݿ�ʼ��ַ      I
��    �������ݳ���          I
��    ����SOCKET            I
��    ��: �����
����ʱ��: 20160917
**********************************************************************/
void selectSingle( char *pcaDataBuf, int iaDataLen, int iaSocket )
{
	#define SELECT_SINGLE_SEND_RESULT( value ) memset( pcResult, 0x00, sizeof(pcResult) );sprintf( pcResult, "%d", value ); cResult = pcResult[0];send( iaSocket, &cResult, 1, 0 );
	
	char pcKeyValueLen[KEY_VALUE_LENGTH + 1];
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	char cResult;
	char pcResult[INT_LEN + 1];
	int  iIndex;
	int  iKeyLen;
	int  iValueLen;
	int  iResult;
	int  iTmp;
	char *pcBuf;
	int  iIndepentId;
	int  iIndepentFileId;
	
	pcBuf = pcaDataBuf;
	
	iIndex = 0;

	/*ȡKEY����*/
	if (iaDataLen - iIndex < KEY_VALUE_LENGTH)
	{
		SELECT_SINGLE_SEND_RESULT( ERROR );
	  return;
	}
	memset( pcKeyValueLen, 0x00, sizeof(pcKeyValueLen) );
	memcpy( pcKeyValueLen, pcBuf + iIndex, KEY_VALUE_LENGTH );
	iKeyLen = atoi( pcKeyValueLen );
	iIndex += KEY_VALUE_LENGTH;
	  
	/*ȡKEY*/
	if (iaDataLen - iIndex < iKeyLen || iKeyLen > MAX_KEY_LEN)
	{
		SELECT_SINGLE_SEND_RESULT( ERROR );
	  return;
	}
	
  *(pcKey + iKeyLen) = 0x00;
  memcpy( pcKey, pcBuf + iIndex, iKeyLen );
	iIndex += iKeyLen;
	
	iIndepentId = calculateKeyMain( pcKey, iKeyLen );
	
	memset( pcValue, 0x00, sizeof(pcValue) );
	
	iResult = selectNodeMain( pcKey, iKeyLen, pcValue, iIndepentId );
  
  /*������ݲ����ڣ���Ӳ��������*/
  if (iResult == NOT_FOUND)
  {
  	iIndepentFileId = calculateKey( pcKey, strlen(pcKey) );
	  iResult = selectNode( pcKey, iKeyLen, pcValue, iIndepentFileId );
  }

	SELECT_SINGLE_SEND_RESULT( iResult );
	
	iValueLen = strlen( pcValue );
	memset( pcKeyValueLen, 0x00, sizeof(pcKeyValueLen) );
	sprintf( pcKeyValueLen, "%0*d", KEY_VALUE_LENGTH, iValueLen );
	
	/*����VALUE����*/
	send( iaSocket, pcKeyValueLen, KEY_VALUE_LENGTH, 0 );
	
	/*����VALUE*/
	send( iaSocket, pcValue, iValueLen, 0 );
		
	return;
}

/**********************************************************************
��������: updateDeleteBat
��������: ������������
��    ����
��    һ�����ݿ�ʼ��ַ      I
��    �������ݳ���          I
��    ����SOCKET            I
��    �ģ���������          I
��    ��: �����
����ʱ��: 20160917
**********************************************************************/
void updateDeleteBat( char *pcaDataBuf, int iaDataLen, int iaSocket, char *pcaOperateType )
{
#define UPDATE_DELETE_BAT_SEND() \
{\
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );\
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendSize );\
	send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );\
	send( iaSocket, pcSendBuf, iSendSize, 0 );\
	memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );\
	iSendSize = 0;\
}

#define UPDATE_DELETE_BAT_SEND_ERROR(value) \
{\
	if (iSendSize + 1 > sizeof(pcSendBuf))\
	{\
	  UPDATE_DELETE_BAT_SEND();\
	}\
	memset( pcResult, 0x00, sizeof(pcResult) );\
	sprintf( pcResult, "%d", value );\
	cResult = pcResult[0];\
	memcpy( pcSendBuf + iSendSize, &cResult, 1 );\
	iSendSize++;\
	\
	UPDATE_DELETE_BAT_SEND();\
	\
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );\
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, 0 );\
	send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );\
}

	char pcKeyValueLen[KEY_VALUE_LENGTH + 1];
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	char pcSendBuf[BAT_RETURN];
	char pcSendLen[DATA_BUF_LEN + 1];
	char cResult;
	char pcResult[INT_LEN + 1];
	int  iOperateType;
	int  iResult;
	int  iIndex;
	int  iKeyLen;
	int  iValueLen;
	int  iType;
	int  iSendSize;
	char *pcBuf;
	int  iIndepentId;
	int  iIndepentFileId;
	
	struct timeb  tp;
	
	if (!strcmp( pcaOperateType, UPDATE_BAT_CNT ))
	{
		iType = UPDATE_OPERATE;
	}
	else
	{
		iType = DELETE_OPERATE;
	}
	
	pcBuf = pcaDataBuf;
	iIndex = 0;
	iSendSize = 0;

	while (1)
  {
  	if (igConnectionErrFlag != SUCCESS)
  	{
  		break;
  	}
    /*����KEY*/
    {
      /*ȡKEY����*/
      if (iaDataLen == iIndex)
      {
      	break;
      }
      if (iaDataLen - iIndex < KEY_VALUE_LENGTH)
	    {
		    UPDATE_DELETE_BAT_SEND_ERROR( ERROR );
	      return;
	    }
	    memset( pcKeyValueLen, 0x00, sizeof(pcKeyValueLen) );
	    memcpy( pcKeyValueLen, pcBuf + iIndex, KEY_VALUE_LENGTH );
	    iKeyLen = atoi( pcKeyValueLen );
	    iIndex += KEY_VALUE_LENGTH;
	    
	    /*ȡKEY*/
	    if (iaDataLen - iIndex < iKeyLen || iKeyLen > MAX_KEY_LEN)
	    {
	    	UPDATE_DELETE_BAT_SEND_ERROR( ERROR );
	      return;
	    }
	    
	    /*����KEYֵ*/
      *(pcKey + iKeyLen) = 0x00;
      memcpy( pcKey, pcBuf + iIndex, iKeyLen );
	    iIndex += iKeyLen;
    }
    iIndepentId = calculateKeyMain( pcKey, iKeyLen );
    
    /*����VALUE*/
    if (iType == UPDATE_OPERATE)
    {
      /*ȡVALUE����*/
      if (iaDataLen - iIndex < KEY_VALUE_LENGTH)
	    {
		    UPDATE_DELETE_BAT_SEND_ERROR( ERROR );
	      return;
	    }
	    memset( pcKeyValueLen, 0x00, sizeof(pcKeyValueLen) );
	    memcpy( pcKeyValueLen, pcBuf + iIndex, KEY_VALUE_LENGTH );
	    iValueLen = atoi( pcKeyValueLen );
	    iIndex += KEY_VALUE_LENGTH;
	    
	    /*ȡVALUE*/
	    if (iaDataLen - iIndex < iValueLen || iValueLen > MAX_VALUE_LEN)
	    {
	    	UPDATE_DELETE_BAT_SEND_ERROR( ERROR );
	      return;
	    }
	    
	    /*����VALUEֵ*/
      *(pcValue + iValueLen) = 0x00;
      memcpy( pcValue, pcBuf + iIndex, iValueLen );
	    iIndex += iValueLen;
	    
	    iResult = updateNodeMain( pcKey, (__int64)(iKeyLen), pcValue, (__int64)(iValueLen), iIndepentId );
	    if (iResult != SUCCESS)
	    {
	    	logFmtSyn( LVBUG, "[%d]", iResult );
	    }
  
	    if (iSendSize + 1 > sizeof(pcSendBuf))
	    {
	    	UPDATE_DELETE_BAT_SEND();
	    }
	    sprintf( pcResult, "%d", iResult );
	    cResult = pcResult[0];
	    memcpy( pcSendBuf + iSendSize, &cResult, 1 );
	    iSendSize++;
    }
    else
    {
    	iResult = deleteNodeMain( pcKey, (__int64)(iKeyLen), iIndepentId );
    	if (iResult == NOT_FOUND)
    	{
    	  iIndepentFileId = calculateKey( pcKey, strlen(pcKey) );
    	  iResult = deleteNode( pcKey, (__int64)(iKeyLen), iIndepentFileId );
      }

    	if (iSendSize + 1 > sizeof(pcSendBuf))
	    {
	    	UPDATE_DELETE_BAT_SEND();
	    }
	    sprintf( pcResult, "%d", iResult );
	    cResult = pcResult[0];
	    memcpy( pcSendBuf + iSendSize, &cResult, 1 );
	    iSendSize++;
    }
  }
    	
	if (iSendSize > 0)
	{
		memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendSize );
  	send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
		send( iaSocket, pcSendBuf, iSendSize, 0 );
	}

  memset( pcSendLen, 0x00, sizeof(pcSendLen) );
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, 0 );
	send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
	return;
}

/**********************************************************************
��������: selectBat
��������: ������������
��    ����
��    һ�����ݿ�ʼ��ַ      I
��    �������ݳ���          I
��    ����SOCKET            I
��    ��: �����
����ʱ��: 20160918
**********************************************************************/
void selectBat( char *pcaDataBuf, int iaDataLen, int iaSocket )
{
	/*
	���գ������ݳ���+�� KEY���ȣ�4B��+KEY��*N
  ���ͣ����ݳ��� + �� KEY���ȣ�4B��+KEY + VALUE���� + VALUE��*N��ֱ�����յ�����Ϊ0����
	*/
#define SELECT_BAT_SEND() \
{\
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );\
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendSize );\
	send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );\
	send( iaSocket, pcSendBuf, iSendSize, 0 );\
	memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );\
	iSendSize = 0;\
}

#define SELECT_BAT_SEND_ERROR(value) \
{\
	if (iSendSize + 1 > sizeof(pcSendBuf))\
	{\
	  SELECT_BAT_SEND();\
	}\
	memset( pcResult, 0x00, sizeof(pcResult) );\
	sprintf( pcResult, "%d", value );\
	cResult = pcResult[0];\
	memcpy( pcSendBuf + iSendSize, &cResult, 1 );\
	iSendSize++;\
	\
	SELECT_BAT_SEND();\
	\
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );\
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, 0 );\
	send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );\
}

	char pcKeyValueLen[KEY_VALUE_LENGTH + 1];
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	char pcSendBuf[BAT_RETURN];
	char pcSendLen[DATA_BUF_LEN + 1];
	char cResult;
	char pcResult[INT_LEN+1];
	int  iIndex;
	int  iKeyLen;
	int  iValueLen;
	int  iResult;
	int  iTmp;
	int  iSendSize;
	char *pcBuf;
	int  iIndepentId;
	int  iIndepentFileId;
	
	pcBuf = pcaDataBuf;
	iIndex = 0;
	
	memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
	iSendSize = 0;
	
	while (1)
  {
  	if (igConnectionErrFlag != SUCCESS)
  	{
  		break;
  	}
    /*����KEY*/
    {
      /*ȡKEY����*/
      if (iaDataLen == iIndex)
      {
      	break;
      }
      if (iaDataLen - iIndex < KEY_VALUE_LENGTH)
	    {
		    SELECT_BAT_SEND_ERROR(ERROR);
		    return;
	    }
	    memset( pcKeyValueLen, 0x00, sizeof(pcKeyValueLen) );
	    memcpy( pcKeyValueLen, pcBuf + iIndex, KEY_VALUE_LENGTH );
	    iKeyLen = atoi( pcKeyValueLen );
	    iIndex += KEY_VALUE_LENGTH;
	    
	    /*ȡKEY*/
	    if (iaDataLen - iIndex < iKeyLen || iKeyLen > MAX_KEY_LEN)
	    {
	    	SELECT_BAT_SEND_ERROR(ERROR);
	      return;
	    }
	    
	    /*����KEYֵ*/
      *(pcKey + iKeyLen) = 0x00;
      memcpy( pcKey, pcBuf + iIndex, iKeyLen );
	    iIndex += iKeyLen;
    }
    
    iIndepentId = calculateKeyMain( pcKey, iKeyLen );
    
    memset( pcValue, 0x00, sizeof(pcValue) );
    
	  iResult = selectNodeMain( pcKey, iKeyLen, pcValue, iIndepentId );
	  
	  /*������ݲ����ڣ���Ӳ��������*/
    if (iResult == NOT_FOUND)
    {
    	iIndepentFileId = calculateKey( pcKey, strlen(pcKey) );
	    iResult = selectNode( pcKey, iKeyLen, pcValue, iIndepentFileId );
    }
	  
	  iValueLen = strlen( pcValue );
	  memset( pcKeyValueLen, 0x00, sizeof(pcKeyValueLen) );
	  sprintf( pcKeyValueLen, "%0*d", KEY_VALUE_LENGTH, iValueLen );

	  if (iSendSize + 1 + KEY_VALUE_LENGTH + iValueLen > sizeof(pcSendBuf))
	  {
	  	SELECT_BAT_SEND();
	  }

	  sprintf( pcResult, "%d", iResult );
	  cResult = pcResult[0];
	  memcpy( pcSendBuf + iSendSize, &cResult, 1 );
	  iSendSize++;

	  memcpy( pcSendBuf + iSendSize, pcKeyValueLen, KEY_VALUE_LENGTH );
	  iSendSize += KEY_VALUE_LENGTH;

	  memcpy( pcSendBuf + iSendSize, pcValue, iValueLen );
	  iSendSize += iValueLen;
  }
  
  if (iSendSize > 0)
	{
		memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendSize );
  	send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
		send( iaSocket, pcSendBuf, iSendSize, 0 );
	}

  memset( pcSendLen, 0x00, sizeof(pcSendLen) );
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, 0 );
	send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
	return;
}

/**********************************************************************
��������: updateDeleteSelectBat
��������: ����������
��    ����
��    һ�����ݿ�ʼ��ַ      I
��    �������ݳ���          I
��    ����SOCKET            I
��    ��: �����
����ʱ��: 20160918
**********************************************************************/
void updateDeleteSelectBat( char *pcaDataBuf, int iaDataLen, int iaSocket )
{
#define UPDATE_DELETE_SELECT_BAT_SEND() \
{\
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );\
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendSize );\
	send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );\
	send( iaSocket, pcSendBuf, iSendSize, 0 );\
	memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );\
	iSendSize = 0;\
}

#define UPDATE_DELETE_SELECT_BAT_ERROR(value) \
{\
	if (iSendSize + 1 > sizeof(pcSendBuf))\
	{\
	  UPDATE_DELETE_SELECT_BAT_SEND();\
	}\
	memset( pcResult, 0x00, sizeof(pcResult) );\
	sprintf( pcResult, "%d", value );\
	cResult = pcResult[0];\
	memcpy( pcSendBuf + iSendSize, &cResult, 1 );\
	iSendSize++;\
	\
	UPDATE_DELETE_SELECT_BAT_SEND();\
	\
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );\
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, 0 );\
	send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );\
}

#define UPDATE_DELETE_SELECT_BAT_RESULT(value) \
{\
	if (iSendSize + 1 > sizeof(pcSendBuf))\
	{\
	  UPDATE_DELETE_SELECT_BAT_SEND();\
	}\
	memset( pcResult, 0x00, sizeof(pcResult) );\
	sprintf( pcResult, "%d", value );\
	cResult = pcResult[0];\
	memcpy( pcSendBuf + iSendSize, &cResult, 1 );\
	iSendSize++;\
}

  char pcOperate[OPERATE_LEN + 1];
	char pcKeyValueLen[KEY_VALUE_LENGTH + 1];
	char pcKey[MAX_KEY_LEN + 1];
	char pcValue[MAX_VALUE_LEN + 1];
	char pcSendBuf[BAT_RETURN];
	char pcSendLen[DATA_BUF_LEN + 1];
	char cResult;
	char pcResult[INT_LEN+1];
	int  iOperateType;
	int  iIndex;
	int  iKeyLen;
	int  iValueLen;
	int  iType;
	int  iResult;
	int  iSendSize;
	char *pcBuf;
	int  iIndepentId;
	int  iIndepentFileId;
	
	pcBuf = pcaDataBuf;
	iIndex = 0;
	
	memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
	iSendSize = 0;
	
	while (1)
  {
  	if (igConnectionErrFlag != SUCCESS)
  	{
  		break;
  	}
  	/*ȡ��������*/
    if (iaDataLen == iIndex)
    {
    	break;
    }
    if (iaDataLen - iIndex < OPERATE_LEN)
	  {
		  UPDATE_DELETE_SELECT_BAT_RESULT( ERROR );
	    return;
	  }
	  memset( pcOperate, 0x00, sizeof(pcOperate) );
	  memcpy( pcOperate, pcBuf + iIndex, OPERATE_LEN );
	  iIndex += OPERATE_LEN;
	  	  
	  /*����KEY*/
    {
      /*ȡKEY����*/
      if (iaDataLen - iIndex < KEY_VALUE_LENGTH)
	    {
		    UPDATE_DELETE_SELECT_BAT_ERROR( ERROR );
	      return;
	    }
	    memset( pcKeyValueLen, 0x00, sizeof(pcKeyValueLen) );
	    memcpy( pcKeyValueLen, pcBuf + iIndex, KEY_VALUE_LENGTH );
	    iKeyLen = atoi( pcKeyValueLen );
	    iIndex += KEY_VALUE_LENGTH;
	    
	    /*ȡKEY*/
	    if (iaDataLen - iIndex < iKeyLen || iKeyLen > MAX_KEY_LEN)
	    {
	    	UPDATE_DELETE_SELECT_BAT_ERROR( ERROR );
	      return;
	    }
	    
	    /*����KEYֵ*/
      *(pcKey + iKeyLen) = 0x00;
      memcpy( pcKey, pcBuf + iIndex, iKeyLen );
	    iIndex += iKeyLen;
    }
    
    iIndepentId = calculateKeyMain( pcKey, iKeyLen );
        	  
	  if (!strcmp( pcOperate, UPDATE_OPERATE_CNT ) || !strcmp( pcOperate, DELETE_OPERATE_CNT ))
	  {
      if (!strcmp( pcOperate, UPDATE_OPERATE_CNT ))
      {
	  	  /*����VALUE*/
        {
          /*ȡVALUE����*/
          if (iaDataLen - iIndex < KEY_VALUE_LENGTH)
	        {
		        UPDATE_DELETE_SELECT_BAT_ERROR( ERROR );
	          return;
	        }
	        memset( pcKeyValueLen, 0x00, sizeof(pcKeyValueLen) );
	        memcpy( pcKeyValueLen, pcBuf + iIndex, KEY_VALUE_LENGTH );
	        iValueLen = atoi( pcKeyValueLen );
	        iIndex += KEY_VALUE_LENGTH;
	        
	        /*ȡVALUE*/
	        if (iaDataLen - iIndex < iValueLen || iValueLen > MAX_VALUE_LEN)
	        {
	        	UPDATE_DELETE_SELECT_BAT_ERROR( ERROR );
	          return;
	        }
	        
	        /*����VALUEֵ*/
          *(pcValue + iValueLen) = 0x00;
          memcpy( pcValue, pcBuf + iIndex, iValueLen );
	        iIndex += iValueLen;
        }
        
        iResult = updateNodeMain( pcKey, (__int64)(iKeyLen), pcValue, (__int64)(iValueLen), iIndepentId );
      }
      else
      {
      	iResult = deleteNodeMain( pcKey, (__int64)(iKeyLen), iIndepentId );
      	if (iResult == NOT_FOUND)
    	  {
    	    iIndepentFileId = calculateKey( pcKey, strlen(pcKey) );
    	    iResult = deleteNode( pcKey, (__int64)(iKeyLen), iIndepentFileId );
        }
      }
            
      UPDATE_DELETE_SELECT_BAT_RESULT( iResult );
	  }
	  else
	  if (!strcmp( pcOperate, SELECT_OPERATE_CNT ))
	  {
	  	memset( pcValue, 0x00, sizeof(pcValue) );
	  	
	  	iResult = selectNodeMain( pcKey, iKeyLen, pcValue, iIndepentId );
      
      /*������ݲ����ڣ���Ӳ��������*/
      if (iResult == NOT_FOUND)
      {
      	iIndepentFileId = calculateKey( pcKey, strlen(pcKey) );
	      iResult = selectNode( pcKey, iKeyLen, pcValue, iIndepentFileId );
      }
	  
	    iValueLen = strlen( pcValue );
	    memset( pcKeyValueLen, 0x00, sizeof(pcKeyValueLen) );
	    sprintf( pcKeyValueLen, "%0*d", KEY_VALUE_LENGTH, iValueLen );
      
	    if (iSendSize + 1 + KEY_VALUE_LENGTH + iValueLen > sizeof(pcSendBuf))
	    {
	    	UPDATE_DELETE_SELECT_BAT_SEND();
	    }
 
	    sprintf( pcResult, "%d", iResult );
	    cResult = pcResult[0];
	    memcpy( pcSendBuf + iSendSize, &cResult, 1 );
	    iSendSize++;
      
	    memcpy( pcSendBuf + iSendSize, pcKeyValueLen, KEY_VALUE_LENGTH );
	    iSendSize += KEY_VALUE_LENGTH;
      
	    memcpy( pcSendBuf + iSendSize, pcValue, iValueLen );
	    iSendSize += iValueLen;
    }
    else
    {
    	UPDATE_DELETE_SELECT_BAT_ERROR( ERROR );
    }
  }
  
  if (iSendSize > 0)
	{
		memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendSize );
  	send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
		send( iaSocket, pcSendBuf, iSendSize, 0 );
	}

  memset( pcSendLen, 0x00, sizeof(pcSendLen) );
	sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, 0 );
	send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
  
  return;
}

/********************************************************
�������ƣ�checkConnectOverTime
�������ܣ����ʱ���Ƿ�ʱ 
��ڲ�����
��    һ��ʱ�� 
��    �أ�1��ʾ��ʱ��0��ʾδ��ʱ 
��    �ߣ������
����ʱ�䣺20140318
*********************************************************/
int checkConnectOverTime( char *pcaLastDateTime )
{
	char pcSysDate[DATE_LEN + 1];
	char pcSysTime[TIME_LEN + 1];
	char pcSysDateTime[DATE_TIME + 1];
	
	memset( pcSysDate, 0x00, sizeof(pcSysDate) );
	memset( pcSysTime, 0x00, sizeof(pcSysTime) );
	
	/*��ȡϵͳ��ʱ��*/
	getDateTime( pcSysDate, pcSysTime );
	
	memset( pcSysDateTime, 0x00, sizeof(pcSysDateTime) );
	sprintf( pcSysDateTime, "%s%s", pcSysDate, pcSysTime );
	
	if (atoi(pcSysDateTime) - atoi(pcaLastDateTime) > OVER_TIME)
	{
		return 1;
	}
	return 0;
}
