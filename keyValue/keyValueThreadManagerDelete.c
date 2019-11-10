#define __KEY_VALUE_THREAD_MANAGER_DELETE_C

/*
������ MANAGER �е� DELETE ���
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

void managerDeleteDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd );
void managerDeleteDealChild( DELETE_SHARE *deleteShare, int iaIndepentFileId );
int  managerDeleteDealDeleteData( DELETE_SHARE *deleteShare, int iaStartOffset[], int iaEndOffset[], int iaIndepentFileId );
int managerDeleteDealDeleteDataLeft( DELETE_SHARE *deleteShare, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId );
int managerDeleteDealDeleteDataOther( DELETE_SHARE *deleteShare, __int64 iaAddr, __int64 iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId );
int managerDeleteDealDeleteDataRight( DELETE_SHARE *deleteShare, KEY_POINT *saKey, int iaEndOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId );
int managerDeleteNode( char *pcaKey, int iaKeyLen, int iaIndepentFileId );

void managerDeleteDealChildMain( DELETE_SHARE *deleteShare, int iaIndepentId );
int  managerDeleteDealDeleteDataMain( DELETE_SHARE *deleteShare, int iaStartOffset[], int iaEndOffset[], int iaIndepentId );
int managerDeleteDealDeleteDataLeftMain( DELETE_SHARE *deleteShare, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId );
int managerDeleteDealDeleteDataOtherMain( DELETE_SHARE *deleteShare, __int64 iaAddr, __int64 iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId );
int managerDeleteDealDeleteDataRightMain( DELETE_SHARE *deleteShare, KEY_POINT *saKey, int iaEndOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId );
int managerDeleteNodeMain( char *pcaKey, int iaKeyLen, int iaIndepentId );

/**********************************************************************
��������: managerDeleteDeal
��������: ����˴��� delete ����
��    ����
��    һ��SOCKET               I
��    �������ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20161022
**********************************************************************/
void managerDeleteDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd )
{
/*
�������ݽṹ������+����+ֵ���䣨���ҿ������ұա�����ҿ�������ұգ�+KEY��ʼֵ����+KEY��ʼֵ+KEY����ֵ����+KEY����ֵ+�û���+����
�������ݽṹ���������+ɾ������������
ɾ������Ӧ�жϣ���ΪҪ����жϣ�Ч�ʻ����ܶࡣ
*/
#define MANAGER_DELETE_VALUE_DEAL_SEND_ERROR( value ) \
{\
	memset( pcResult, 0x00, sizeof(pcResult) );\
	sprintf( pcResult, "%d", value ); \
	cResult = pcResult[0];\
	\
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );\
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, 1 );\
  \
  send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );\
	send( iaSocket, &cResult, 1, 0 );\
}
	char pcResult[INT_LEN + 1];
	char cResult;
	char pcSendBuf[INT_LEN + 1];
	char pcRows[INT64_LEN + 1];
	char pcTotalRows[INT64_LEN + 1];
	char pcKeyStartLen[KEY_VALUE_LENGTH + 1];
  char pcKeyEndLen[KEY_VALUE_LENGTH + 1];
  char pcKeyStart[MAX_KEY_LEN + 1];
  char pcKeyEnd[MAX_KEY_LEN + 1];
  char pcValueType[KEY_VALUE_LENGTH + 1];
  char pcKeyLen[KEY_VALUE_LENGTH + 1];
  char pcUser[NAME_LEN + 1];
	char pcPass[NAME_LEN + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
  int  iKeyStartLen;
	int  iKeyEndLen;
	__int64 llStartIndex;
	__int64 llStartIndexTmp;
	__int64 llSelectRows;
	int  iReturnRows;
	int  iRet;
	char *pcBufStart;
	char *pcBufEnd;
	DELETE_SHARE deleteStruct;
	pthread_t  childThreadId;
	char pcOperate[OPERATE_LEN + 1];
	int iRecvLen;
	int iTotalLen;
	int iDataLen;
	int iSendLen;
	int iTmp;
	int iIndex;
	
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
	
	/*ȡKEY��VALUE����ֵ����*/
	memset( pcKeyEndLen, 0x00, sizeof(pcKeyEndLen) );
	memcpy( pcKeyEndLen, pcBufStart, KEY_VALUE_LENGTH );
	pcBufStart += KEY_VALUE_LENGTH;
	
	/*ȡKEY��VALUE����ֵ*/
	iKeyEndLen = atoi( pcKeyEndLen );
	memset( pcKeyEnd, 0x00, sizeof(pcKeyEnd) );
	memcpy( pcKeyEnd, pcBufStart, iKeyEndLen );
	pcBufStart += iKeyEndLen;
	
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
	
	logFmtSyn( LVNOR, "pcValueType[%s]pcKeyStart[%s]pcKeyEnd[%s]", pcValueType, pcKeyStart, pcKeyEnd );
	
	MANAGER_WRLOCK();
	
	/*����û����������Ƿ�ƥ��*/
	if (strcmp( pcgUser, pcUser ) || strcmp( pcgPass, pcPass ))
	{
		MANAGER_DELETE_VALUE_DEAL_SEND_ERROR( ERROR );
		MANAGER_WRUNLOCK();
		return;
	}
	
	/*���ù�������*/
	memset( &deleteStruct, 0x00, sizeof(deleteStruct) );
	memcpy( deleteStruct.pcValueType, pcValueType, KEY_VALUE_LENGTH ); /*����������ֵ���䣨KEY��VALUE ���ҿ������ұա�����ҿ�������ұ�*/
	memcpy( deleteStruct.pcKeyStart, pcKeyStart, iKeyStartLen );      /*KEY �Ŀ�ʼֵ*/
	deleteStruct.iKeyStartLen = iKeyStartLen;                         /*KEY �Ŀ�ʼֵ����*/
	memcpy( deleteStruct.pcKeyEnd, pcKeyEnd, iKeyEndLen );            /*KEY VALUE �Ľ���ֵ*/
	deleteStruct.iKeyEndLen = iKeyEndLen;                             /*KEY VALUE �Ľ���ֵ����*/

  #if 1
	/*���ڴ��е���������д�뵽Ӳ����*/
	for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
	{
		memset( deleteStruct.pcConnerKey, 0x00, sizeof(deleteStruct.pcConnerKey) );
    memset( &(deleteStruct.iConnerKeyLen), 0x00, sizeof(deleteStruct.iConnerKeyLen) );
    deleteStruct.iCornerSize = 0;
    managerDeleteDealChildMain( &deleteStruct, iTmp );
  }
  #endif
  
	/*���ú���������*/
	for (iTmp = 0; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
	{
		memset( deleteStruct.pcConnerKey, 0x00, sizeof(deleteStruct.pcConnerKey) );
    memset( &(deleteStruct.iConnerKeyLen), 0x00, sizeof(deleteStruct.iConnerKeyLen) );
    deleteStruct.iCornerSize = 0;
	  managerDeleteDealChild( &deleteStruct, iTmp );
  }
	
	MANAGER_WRUNLOCK();
	
	memset( pcTotalRows, 0x00, sizeof(pcTotalRows) );
  sprintf( pcTotalRows, "%0*lld", INT64_LEN, deleteStruct.llTotalRows );
  
  logFmtSyn( LVNOR, "%s", pcTotalRows );
  
  /*�鷢������*/
  memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
  
  sprintf( &cResult, "%d", SUCCESS );
  memcpy( pcSendBuf, &cResult, 1 );
  memcpy( pcSendBuf + 1, pcTotalRows, INT64_LEN );
  
  iSendLen = 1 + INT64_LEN;
  send( iaSocket, pcSendBuf, iSendLen, 0 );
	
}

/**********************************************************************
��������: managerDeleteDealChild
��������: ����˴��� delete ����
��    ����
��    һ������ṹ��               I/O
��    ��: �����
����ʱ��: 20161111
**********************************************************************/
void managerDeleteDealChild( DELETE_SHARE *deleteShare, int iaIndepentFileId )
{
  int  iStartOffsetRecord[20];   /*��ʼλ�ü�¼*/
	int  iEndOffsetRecord[20];     /*����λ�ü�¼*/
	KEY_POINT key;                 /*�����������е����ݽṹ*/
	__int64  llDisc;
	__int64  llSize;
	__int64  llLoops;
	int  iRet;
	int  iTmp;
	
	llDisc = sgBase[iaIndepentFileId].llFirstDisc;
	llSize = sgBase[iaIndepentFileId].llFirstSize;
	llLoops = sgBase[iaIndepentFileId].llLoops;
	
	/*��λ��ʼλ��*/
	{
		/*��ʼ����ʼλ�ü�¼����*/
	  for (iTmp = 0; iTmp < 20; iTmp++)
	  {
	  	iStartOffsetRecord[iTmp] = 0x00;
	  }
	
		/*�������*/
	  if (!strcmp( deleteShare->pcValueType, KEY_OPEN_OPEN ) != !strcmp( deleteShare->pcValueType, KEY_OPEN_CLOSE ))
    {
  	  iRet = managerPositionLeftOpen( iStartOffsetRecord, deleteShare->pcKeyStart, llDisc, llSize, 1, llLoops, iaIndepentFileId );
  	  
  	  /*���û����������*/
  	  if (iRet == NOT_FOUND)
  	  {
  	  	logFmtSyn( LVNOR, "û���ҵ�����" );
        return ;
  	  }
    }
    else
    {
  	  iRet = managerPositionLeftClose( iStartOffsetRecord, deleteShare->pcKeyStart, llDisc, llSize, 1, llLoops, iaIndepentFileId );
  	  
  	  /*���û����������*/
  	  if (iRet == NOT_FOUND)
  	  {
  	  	logFmtSyn( LVNOR, "û���ҵ�����" );
        return ;
  	  }
    }
	}
	
	/*��λ����λ��*/
	{
		/*��ʼ������λ�ü�¼����*/
	  for (iTmp = 0; iTmp < 20; iTmp++)
	  {
	  	iEndOffsetRecord[iTmp] = 0x00;
	  }
	
		/*������ҿ�*/
	  if (!strcmp( deleteShare->pcValueType, KEY_OPEN_OPEN ) != !strcmp( deleteShare->pcValueType, KEY_CLOSE_OPEN ))
    {
  	  iRet = managerPositionRightOpen( iEndOffsetRecord, deleteShare->pcKeyEnd, llDisc, llSize, 1, llLoops, iaIndepentFileId );
  	  
  	  /*���û����������*/
  	  if (iRet == NOT_FOUND)
  	  {
  	  	logFmtSyn( LVNOR, "û���ҵ�����" );
        return ;
  	  }
    }
    else
    {
  	  iRet = managerPositionRightClose( iEndOffsetRecord, deleteShare->pcKeyEnd, llDisc, llSize, 1, llLoops, iaIndepentFileId );
  	  
  	  /*���û����������*/
  	  if (iRet == NOT_FOUND)
  	  {
  	  	logFmtSyn( LVNOR, "û���ҵ�����" );
        return ;
  	  }
    }
	}
		
	/*ɾ������*/
	key.llChildAddr = sgBase[iaIndepentFileId].llFirstDisc;
	key.llChildSize = sgBase[iaIndepentFileId].llFirstSize;
	
	/*ɾ������*/
	managerDeleteDealDeleteData( deleteShare,  iStartOffsetRecord, iEndOffsetRecord, iaIndepentFileId );
	
	return;
}

/**********************************************************************
��������: managerDeleteDealDeleteData
��������: ����˴��� delete �е�����ɾ��
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
����ʱ��: 20161111
**********************************************************************/
int  managerDeleteDealDeleteData( DELETE_SHARE *deleteShare, int iaStartOffset[], int iaEndOffset[], int iaIndepentFileId )
{
	KEY_POINT *key;                /*�����������е����ݽṹ*/
	KEY_POINT *keyTmp;             /*�����������е����ݽṹ*/
	KEY_POINT keyOut;              /*�����������е����ݽṹ*/
  KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
  char pcKey[MAX_KEY_LEN];
  char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
  char pcDataBufTmp[BLOCK_SIZE]; /*�����е�������ʱ����*/
  char pcDataBufTmp2[BLOCK_SIZE]; /*�����е�������ʱ����*/
  int  iReplaceFlag = 0;
  int  iReplaceLeftFlag;
  int  iReplaceRightFlag;
  int  iStartOffsetRecord[20];   /*��ʼλ�ü�¼*/
  int  iEndOffsetRecord[20];     /*����λ�ü�¼*/
  int  iIndex;
  __int64 iAddr;
  __int64 iAddrTmp;
  __int64 iSize;
  int  iTotalLoops;
  
  __int64 llChildSize;
  int  iTmp;
  int  iRet;
  
   /*ȡ����ǰ��������*/
  iAddr = sgBase[iaIndepentFileId].llFirstDisc;
  iSize = sgBase[iaIndepentFileId].llFirstSize;
  iTotalLoops = sgBase[iaIndepentFileId].llLoops;
  
  /*������ʼλ�úͽ���λ�ò�ͬ�ļ�*/
  for (iIndex = 1; iIndex <= iTotalLoops; iIndex++)
  {
  	if (iaStartOffset[iIndex] < iaEndOffset[iIndex] )
  	{
  		break;
  	}
  }
  
  /*�������λ�ö���ͬ����ʾֻ��һ��������������*/
  if (iIndex > iTotalLoops)
  {
  	/*ȡ��������*/
  	{
  	  /*ȡ����󼶵�����*/
      iAddr = sgBase[iaIndepentFileId].llFirstDisc;
      for (iTmp = 1; iTmp < iTotalLoops; iTmp++)
      {
      	memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
        getKeyBlockData( pcDataBuf, iAddr, iaIndepentFileId );
      
        key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iTmp] * sizeof(KEY_POINT));
      
        iAddr = key->llChildAddr;
      }
      
      memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
      getKeyBlockData( pcDataBuf, iAddr, iaIndepentFileId );
      keyData = (KEY_VALUE_POINT*)(pcDataBuf + iaStartOffset[iTotalLoops] * sizeof(KEY_VALUE_POINT));
      
      valueToBuf( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentFileId );
      pcKey[keyData->llKeyLen] = 0x00;
  	}
  	
  	iRet = managerDeleteNode( pcKey, keyData->llKeyLen, iaIndepentFileId );
		if (iRet == SUCCESS)
		{
			(deleteShare->llTotalRows)++;
		}
		
		return SUCCESS;
  }
  
  memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
  getKeyBlockData( pcDataBuf, iAddr, iaIndepentFileId );
  
  /*ȡ����ǰ��������*/
  for (iTmp = 2; iTmp < iIndex; iTmp++)
  {
  	memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iAddr, iaIndepentFileId );
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iTmp] * sizeof(KEY_POINT));
    
    memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
    
    /*������һ������*/
    memcpy( pcDataBufTmp, pcDataBuf, BLOCK_SIZE );
    keyTmp = key;
    iAddrTmp = iAddr;
    
    iAddr = key->llChildAddr;
    iSize = key->llChildSize;
  }
  
  /*�Ӹü���ʼ����ʼλ�úͽ���λ�ò�ͬ*/

  /*����ʼλ�õļ���ɾ���Ǳ�����*/
  key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iIndex] * sizeof(KEY_POINT));
  managerDeleteDealDeleteDataLeft( deleteShare, key->llChildAddr, key->llChildSize, iaStartOffset, iIndex + 1, iTotalLoops, iaIndepentFileId );


  /*�������øý��*/
  key->llChildSize = iaStartOffset[iIndex + 1] + 1;
  setKeyBlockData( pcDataBuf, iAddr, iaIndepentFileId );
  
  /*��������λ�õļ���ɾ���Ǳ�����*/
  for (iTmp = iaStartOffset[iIndex] + 1; iTmp < iaEndOffset[iIndex]; iTmp++)
  {
    key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
    managerDeleteDealDeleteDataOther( deleteShare, key->llChildAddr, key->llChildSize, iIndex + 1, iTotalLoops, iaIndepentFileId );
  }


  /*�������λ�õļ���ɾ���Ǳ�����*/
  key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iIndex] * sizeof(KEY_POINT));
  managerDeleteDealDeleteDataRight( deleteShare, key, iaEndOffset, iIndex + 1, iTotalLoops, iaIndepentFileId );

  if (key->llChildSize == 0)
  {
  	freeKeyBlock( key->llChildAddr, iaIndepentFileId );

  	/*�������ø�����ֵ*/
    memset( pcDataBufTmp2, 0x00, sizeof(pcDataBufTmp) );
    memcpy( pcDataBufTmp2, pcDataBuf, ( iaStartOffset[iIndex] + 1 ) * sizeof(KEY_POINT) );
    memcpy( pcDataBufTmp2 + ( iaStartOffset[iIndex] + 1 ) * sizeof(KEY_POINT), pcDataBuf + ( iaEndOffset[iIndex] + 1 ) * sizeof(KEY_POINT), ( iSize - iaEndOffset[iIndex] - 1 )* sizeof(KEY_POINT) );
    setKeyBlockData( pcDataBufTmp2, iAddr, iaIndepentFileId );

  	/*����ǵ�һ��*/
    if (iAddr == sgBase[iaIndepentFileId].llFirstDisc)
    {
    	sgBase[iaIndepentFileId].llFirstSize -= iaEndOffset[iIndex] - iaStartOffset[iIndex];
    }
    else
    {
    	keyTmp->llChildSize -= iaEndOffset[iIndex] - iaStartOffset[iIndex];
    	setKeyBlockData( pcDataBufTmp, iAddrTmp, iaIndepentFileId );
    }
  }
  else
  {
  	/*�������ø�����ֵ*/
    memset( pcDataBufTmp2, 0x00, sizeof(pcDataBufTmp) );
    memcpy( pcDataBufTmp2, pcDataBuf,  ( iaStartOffset[iIndex] + 1 ) * sizeof(KEY_POINT) );
    memcpy( pcDataBufTmp2 + ( iaStartOffset[iIndex] + 1 ) * sizeof(KEY_POINT), pcDataBuf + iaEndOffset[iIndex] * sizeof(KEY_POINT), ( iSize - iaEndOffset[iIndex] )* sizeof(KEY_POINT) );
    setKeyBlockData( pcDataBufTmp2, iAddr, iaIndepentFileId );
  
  	/*����ǵ�һ��*/
    if (iAddr == sgBase[iaIndepentFileId].llFirstDisc)
    {
    	sgBase[iaIndepentFileId].llFirstSize -= iaEndOffset[iIndex] - 1 - iaStartOffset[iIndex];
    }
    else
    {
    	keyTmp->llChildSize -= iaEndOffset[iIndex] - 1 - iaStartOffset[iIndex];
    	setKeyBlockData( pcDataBufTmp, iAddrTmp, iaIndepentFileId );
    }
  }
    
  /*������*/
	for (iIndex = 1; iIndex <= iTotalLoops; iIndex++)
  {
  	iAddr = sgBase[iaIndepentFileId].llFirstDisc;
    iSize = sgBase[iaIndepentFileId].llFirstSize;
    iTotalLoops = sgBase[iaIndepentFileId].llLoops;
  
  	if (iSize == 1)
  	{
  		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
      getKeyBlockData( pcDataBuf, iAddr, iaIndepentFileId );
      
      key = (KEY_POINT*)(pcDataBuf);
      
      freeKeyBlock( sgBase[iaIndepentFileId].llFirstDisc, iaIndepentFileId );
      
      /*�������õ�һ�����*/
      sgBase[iaIndepentFileId].llFirstDisc = key->llChildAddr;
      sgBase[iaIndepentFileId].llFirstSize = key->llChildSize;
      sgBase[iaIndepentFileId].llLoops--;
  	}
  	else
  	{
  		break;
  	}
  }

  /*ɾ���߽�����*/
	for (iTmp = 0; iTmp < deleteShare->iCornerSize; iTmp++)
	{
		memset( pcKey, 0x00, sizeof(pcKey) );
		strcpy( pcKey, deleteShare->pcConnerKey[iTmp] );
		
		iRet = managerDeleteNode( pcKey, deleteShare->iConnerKeyLen[iTmp], iaIndepentFileId );

		if (iRet == SUCCESS)
		{
			(deleteShare->llTotalRows)++;
		}
	}
	
  return SUCCESS;
}

/**********************************************************************
��������: managerDeleteDealDeleteDataLeft
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
����ʱ��: 20170114
**********************************************************************/
int managerDeleteDealDeleteDataLeft( DELETE_SHARE *deleteShare, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId )
{
	char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
	char pcDataBufTmp[BLOCK_SIZE]; /*�����е�����*/
	char pcKey[MAX_KEY_LEN];
	KEY_POINT *key;
	KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
	int  iTmp;
	int  iRet;
		
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
      
      strcpy( deleteShare->pcConnerKey[deleteShare->iCornerSize], pcKey );
      deleteShare->iConnerKeyLen[deleteShare->iCornerSize] = keyData->llKeyLen;
  	  
      (deleteShare->iCornerSize)++;
      
    }
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iaLoop] * sizeof(KEY_POINT));
    managerDeleteDealDeleteDataLeft( deleteShare, key->llChildAddr, key->llChildSize, iaStartOffset, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
    
    /*�����һ���������һ�����������øý��*/
    
    if (iaLoop + 1 != iaTotalLoops)
    {
      key->llChildSize = iaStartOffset[iaLoop + 1] + 1;
    }
    
    for (iTmp = iaStartOffset[iaLoop] + 1; iTmp < iaSize; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      managerDeleteDealDeleteDataOther( deleteShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
      
      freeKeyBlock( key->llChildAddr, iaIndepentFileId );
    }
    
    /*���������¼�����*/
    memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
    memcpy( pcDataBufTmp, pcDataBuf, ( iaStartOffset[iaLoop] + 1 ) * sizeof(KEY_POINT) );
    setKeyBlockData( pcDataBufTmp, iaAddr, iaIndepentFileId );
	}
	
	return SUCCESS;
}

/**********************************************************************
��������: managerDeleteDealDeleteDataOther
��������: ����˴��� delete �е�ɾ������
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
int managerDeleteDealDeleteDataOther( DELETE_SHARE *deleteShare, __int64 iaAddr, __int64 iaSize, int iaLoop, int iaTotalLoops, int iaIndepentFileId )
{
	KEY_POINT *key;                /*�����������е����ݽṹ*/
  KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
  char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
  int  iIndex;
  
	memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
  getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
  
  /*����������һ��*/
  if (iaLoop == iaTotalLoops)
  {
    /*ɾ�����*/
    for (iIndex = 0; iIndex < iaSize; iIndex++)
    {
    	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iIndex * sizeof(KEY_VALUE_POINT));
    	
    	/*�ͷŽ���� VALUE �ļ��еĿռ�*/
	    freeValueBlock( keyData->llKeyAddr, iaIndepentFileId );
	    freeValueBlock( keyData->llValueAddr, iaIndepentFileId );
	    (deleteShare->llTotalRows)++;
	    (sgBase[iaIndepentFileId].llKeyValueSize)--;
    }
  }
  else
  {
    /*ɾ�����*/
    for (iIndex = 0; iIndex < iaSize; iIndex++)
    {
    	key = (KEY_POINT*)(pcDataBuf + iIndex * sizeof(KEY_POINT));
    	
    	managerDeleteDealDeleteDataOther( deleteShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );    	
    	freeKeyBlock( key->llChildAddr, iaIndepentFileId );
    }
  }
  
  return SUCCESS;
}

/**********************************************************************
��������: managerDeleteDealDeleteDataRight
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
����ʱ��: 20170114
**********************************************************************/
int managerDeleteDealDeleteDataRight( DELETE_SHARE *deleteShare, KEY_POINT *saKey, int iaEndOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentFileId )
{
	char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
	char pcDataBufTmp[BLOCK_SIZE]; /*�����е�����*/
	char pcKey[MAX_KEY_LEN];
	KEY_POINT *key;
	KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
	int  iTmp;
	int  iRet;
	__int64 llChildSize;
	
	llChildSize = saKey->llChildSize;
	
	/*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, saKey->llChildAddr, iaIndepentFileId );
    
		for (iTmp = 0; iTmp <= iaEndOffset[iaLoop]; iTmp++ )
	  {
	  	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iTmp * sizeof(KEY_VALUE_POINT));
      
      /*�ͷŽ���� VALUE �ļ��еĿռ�*/
	    freeValueBlock( keyData->llKeyAddr, iaIndepentFileId );
	    freeValueBlock( keyData->llValueAddr, iaIndepentFileId );
	    (deleteShare->llTotalRows)++;
	    (sgBase[iaIndepentFileId].llKeyValueSize)--;
      (saKey->llChildSize)--;
    }
    
    if (saKey->llChildSize > 0)
    {
    	memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
    	memcpy( pcDataBufTmp, pcDataBuf + (iaEndOffset[iaLoop] + 1) * sizeof(KEY_VALUE_POINT), ( llChildSize - iaEndOffset[iaLoop] - 1) * sizeof(KEY_VALUE_POINT)  );
      setKeyBlockData( pcDataBufTmp, saKey->llChildAddr, iaIndepentFileId );
      
      /*�����Ľ�㼷��*/
      saKey->llKeyLen = keyData->llKeyLen;
      saKey->llKeyAddr = keyData->llKeyAddr;
    }
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, saKey->llChildAddr, iaIndepentFileId );
    for (iTmp = 0; iTmp < iaEndOffset[iaLoop]; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      managerDeleteDealDeleteDataOther( deleteShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
      freeKeyBlock( key->llChildAddr, iaIndepentFileId );
    }
    
    saKey->llChildSize -= iaEndOffset[iaLoop];
    
    key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iaLoop] * sizeof(KEY_POINT));
    iRet = managerDeleteDealDeleteDataRight( deleteShare, key, iaEndOffset, iaLoop + 1, iaTotalLoops, iaIndepentFileId );
    
    /*�����һ��������Ϊ0���ͷ�*/
    if (key->llChildSize == 0)
    {
    	freeKeyBlock( key->llChildAddr, iaIndepentFileId );
    	(saKey->llChildSize)--;
    }
    
    /*�����ǰ����������*/
    if (saKey->llChildSize > 0)
    {      
      /*�����Ľ�㼷��*/
      saKey->llKeyLen = key->llKeyLen;
      saKey->llKeyAddr = key->llKeyAddr;
      
      /*���������¼�����*/
      memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
      memcpy( pcDataBufTmp, pcDataBuf + iaEndOffset[iaLoop] * sizeof(KEY_POINT), ( llChildSize - iaEndOffset[iaLoop] ) * sizeof(KEY_POINT) );
      setKeyBlockData( pcDataBufTmp, saKey->llChildAddr, iaIndepentFileId );
    }
    
	}
	
	return SUCCESS;
}

/********************************************************
�������ƣ�managerDeleteNode
�������ܣ�ɾ���������� 
��ڲ�����
��    һ: ����ֵ                 I
��    �أ�0��ʾ�ɹ���������ʾʧ�� 
��    �ߣ������ 
����ʱ�䣺20170114
�޸�ʱ�䣺 
*********************************************************/
int managerDeleteNode( char *pcaKey, int iaKeyLen, int iaIndepentFileId )
{
	int iRet;
  
	if (sgBase[iaIndepentFileId].llFirstSize == 0)
  {
  	return NOT_FOUND;
  }
  
  /*�����������*/
  if (sgBase[iaIndepentFileId].llLoops >= 3)
  {
  	iRet = deleteIndexNode( NULL, NULL, 1, pcaKey, iaKeyLen, iaIndepentFileId );
  }
  else
  /*���ֻ������*/
  if (sgBase[iaIndepentFileId].llLoops == 2)
  {
  	iRet = deleteIndexNodeTwoLoops( pcaKey, iaKeyLen, iaIndepentFileId );
  }
  else
  /*���ֻ��һ��*/
  if (sgBase[iaIndepentFileId].llLoops <= 1)
  {
  	iRet = deleteIndexNodeOneLoops( pcaKey, iaKeyLen, iaIndepentFileId );
  }
  else
  {  
    iRet = ERROR;
  }
  
  if (iRet == SUCCESS)
  {
  	(sgBase[iaIndepentFileId].llKeyValueSize)--;
  }
  
  return iRet;
}

/**********************************************************************
��������: managerDeleteDealChild
��������: ����˴��� delete ����
��    ����
��    һ������ṹ��               I/O
��    ��: �����
����ʱ��: 20161111
**********************************************************************/
void managerDeleteDealChildMain( DELETE_SHARE *deleteShare, int iaIndepentId )
{
  int  iStartOffsetRecord[20];   /*��ʼλ�ü�¼*/
	int  iEndOffsetRecord[20];     /*����λ�ü�¼*/
	KEY_POINT key;                 /*�����������е����ݽṹ*/
	__int64  llDisc;
	__int64  llSize;
	__int64  llLoops;
	int  iRet;
	int  iTmp;
	
	llDisc = sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain;
	llSize = sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain;
	llLoops = sgGlobalVariableMain[iaIndepentId].llgLoopsMain;
	
	/*��λ��ʼλ��*/
	{
		/*��ʼ����ʼλ�ü�¼����*/
	  for (iTmp = 0; iTmp < 20; iTmp++)
	  {
	  	iStartOffsetRecord[iTmp] = 0x00;
	  }
	
		/*�������*/
	  if (!strcmp( deleteShare->pcValueType, KEY_OPEN_OPEN ) != !strcmp( deleteShare->pcValueType, KEY_OPEN_CLOSE ))
    {
  	  iRet = managerPositionLeftOpenMain( iStartOffsetRecord, deleteShare->pcKeyStart, llDisc, llSize, 1, llLoops, iaIndepentId );
  	  
  	  /*���û����������*/
  	  if (iRet == NOT_FOUND)
  	  {
  	  	//logFmtSyn( LVNOR, "û���ҵ�����" );
        return ;
  	  }
    }
    else
    {
  	  iRet = managerPositionLeftCloseMain( iStartOffsetRecord, deleteShare->pcKeyStart, llDisc, llSize, 1, llLoops, iaIndepentId );
  	  
  	  /*���û����������*/
  	  if (iRet == NOT_FOUND)
  	  {
  	  	//logFmtSyn( LVNOR, "û���ҵ�����" );
        return ;
  	  }
    }
	}
	
	/*��λ����λ��*/
	{
		/*��ʼ������λ�ü�¼����*/
	  for (iTmp = 0; iTmp < 20; iTmp++)
	  {
	  	iEndOffsetRecord[iTmp] = 0x00;
	  }
	
		/*������ҿ�*/
	  if (!strcmp( deleteShare->pcValueType, KEY_OPEN_OPEN ) != !strcmp( deleteShare->pcValueType, KEY_CLOSE_OPEN ))
    {
  	  iRet = managerPositionRightOpenMain( iEndOffsetRecord, deleteShare->pcKeyEnd, llDisc, llSize, 1, llLoops, iaIndepentId );
  	  
  	  /*���û����������*/
  	  if (iRet == NOT_FOUND)
  	  {
  	  	//logFmtSyn( LVNOR, "û���ҵ�����" );
        return ;
  	  }
    }
    else
    {
  	  iRet = managerPositionRightCloseMain( iEndOffsetRecord, deleteShare->pcKeyEnd, llDisc, llSize, 1, llLoops, iaIndepentId );
  	  
  	  /*���û����������*/
  	  if (iRet == NOT_FOUND)
  	  {
  	  	//logFmtSyn( LVNOR, "û���ҵ�����" );
        return ;
  	  }
    }
	}
		
	/*ɾ������*/
	key.llChildAddr = sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain;
	key.llChildSize = sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain;
	
	/*ɾ������*/
	managerDeleteDealDeleteDataMain( deleteShare,  iStartOffsetRecord, iEndOffsetRecord, iaIndepentId );
	
	return;
}

/**********************************************************************
��������: managerDeleteDealDeleteData
��������: ����˴��� delete �е�����ɾ��
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
����ʱ��: 20161111
**********************************************************************/
int  managerDeleteDealDeleteDataMain( DELETE_SHARE *deleteShare, int iaStartOffset[], int iaEndOffset[], int iaIndepentId )
{
	KEY_POINT *key;                /*�����������е����ݽṹ*/
	KEY_POINT *keyTmp;             /*�����������е����ݽṹ*/
	KEY_POINT keyOut;              /*�����������е����ݽṹ*/
  KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
  char pcKey[MAX_KEY_LEN];
  char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
  char pcDataBufTmp[BLOCK_SIZE]; /*�����е�������ʱ����*/
  char pcDataBufTmp2[BLOCK_SIZE]; /*�����е�������ʱ����*/
  int  iReplaceFlag = 0;
  int  iReplaceLeftFlag;
  int  iReplaceRightFlag;
  int  iStartOffsetRecord[20];   /*��ʼλ�ü�¼*/
  int  iEndOffsetRecord[20];     /*����λ�ü�¼*/
  int  iIndex;
  __int64 iAddr;
  __int64 iAddrTmp;
  __int64 iSize;
  int  iTotalLoops;
  
  __int64 llChildSize;
  int  iTmp;
  int  iRet;
  
   /*ȡ����ǰ��������*/
  iAddr = sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain;
  iSize = sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain;
  iTotalLoops = sgGlobalVariableMain[iaIndepentId].llgLoopsMain;
  
  /*������ʼλ�úͽ���λ�ò�ͬ�ļ�*/
  for (iIndex = 1; iIndex <= iTotalLoops; iIndex++)
  {
  	if (iaStartOffset[iIndex] < iaEndOffset[iIndex] )
  	{
  		break;
  	}
  }
  
  /*�������λ�ö���ͬ����ʾֻ��һ��������������*/
  if (iIndex > iTotalLoops)
  {
  	/*ȡ��������*/
  	{
  	  /*ȡ����󼶵�����*/
      iAddr = sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain;
      for (iTmp = 1; iTmp < iTotalLoops; iTmp++)
      {
      	memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
        getKeyBlockDataMain( pcDataBuf, iAddr, iaIndepentId );
      
        key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iTmp] * sizeof(KEY_POINT));
      
        iAddr = key->llChildAddr;
      }
      
      memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
      getKeyBlockDataMain( pcDataBuf, iAddr, iaIndepentId );
      keyData = (KEY_VALUE_POINT*)(pcDataBuf + iaStartOffset[iTotalLoops] * sizeof(KEY_VALUE_POINT));
      
      valueToBufMain( pcKey, keyData->llKeyAddr, keyData->llKeyLen, iaIndepentId );
      pcKey[keyData->llKeyLen] = 0x00;
  	}
  	
  	iRet = managerDeleteNodeMain( pcKey, keyData->llKeyLen, iaIndepentId );
		if (iRet == SUCCESS)
		{
			(deleteShare->llTotalRows)++;
		}
		
		return SUCCESS;
  }
  
  memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
  getKeyBlockDataMain( pcDataBuf, iAddr, iaIndepentId );
  
  /*ȡ����ǰ��������*/
  for (iTmp = 2; iTmp < iIndex; iTmp++)
  {
  	memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iAddr, iaIndepentId );
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iTmp] * sizeof(KEY_POINT));
    
    memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
    
    /*������һ������*/
    memcpy( pcDataBufTmp, pcDataBuf, BLOCK_SIZE );
    keyTmp = key;
    iAddrTmp = iAddr;
    
    iAddr = key->llChildAddr;
    iSize = key->llChildSize;
  }
  
  /*�Ӹü���ʼ����ʼλ�úͽ���λ�ò�ͬ*/

  /*����ʼλ�õļ���ɾ���Ǳ�����*/
  key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iIndex] * sizeof(KEY_POINT));
  managerDeleteDealDeleteDataLeftMain( deleteShare, key->llChildAddr, key->llChildSize, iaStartOffset, iIndex + 1, iTotalLoops, iaIndepentId );


  /*�������øý��*/
  key->llChildSize = iaStartOffset[iIndex + 1] + 1;
  setKeyBlockDataMain( pcDataBuf, iAddr, iaIndepentId );
  
  /*��������λ�õļ���ɾ���Ǳ�����*/
  for (iTmp = iaStartOffset[iIndex] + 1; iTmp < iaEndOffset[iIndex]; iTmp++)
  {
    key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
    managerDeleteDealDeleteDataOtherMain( deleteShare, key->llChildAddr, key->llChildSize, iIndex + 1, iTotalLoops, iaIndepentId );
  }


  /*�������λ�õļ���ɾ���Ǳ�����*/
  key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iIndex] * sizeof(KEY_POINT));
  managerDeleteDealDeleteDataRightMain( deleteShare, key, iaEndOffset, iIndex + 1, iTotalLoops, iaIndepentId );

  if (key->llChildSize == 0)
  {
  	freeKeyBlockMain( key->llChildAddr, iaIndepentId );

  	/*�������ø�����ֵ*/
    memset( pcDataBufTmp2, 0x00, sizeof(pcDataBufTmp) );
    memcpy( pcDataBufTmp2, pcDataBuf, ( iaStartOffset[iIndex] + 1 ) * sizeof(KEY_POINT) );
    memcpy( pcDataBufTmp2 + ( iaStartOffset[iIndex] + 1 ) * sizeof(KEY_POINT), pcDataBuf + ( iaEndOffset[iIndex] + 1 ) * sizeof(KEY_POINT), ( iSize - iaEndOffset[iIndex] - 1 )* sizeof(KEY_POINT) );
    setKeyBlockDataMain( pcDataBufTmp2, iAddr, iaIndepentId );

  	/*����ǵ�һ��*/
    if (iAddr == sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain)
    {
    	sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain -= iaEndOffset[iIndex] - iaStartOffset[iIndex];
    }
    else
    {
    	keyTmp->llChildSize -= iaEndOffset[iIndex] - iaStartOffset[iIndex];
    	setKeyBlockDataMain( pcDataBufTmp, iAddrTmp, iaIndepentId );
    }
  }
  else
  {
  	/*�������ø�����ֵ*/
    memset( pcDataBufTmp2, 0x00, sizeof(pcDataBufTmp) );
    memcpy( pcDataBufTmp2, pcDataBuf,  ( iaStartOffset[iIndex] + 1 ) * sizeof(KEY_POINT) );
    memcpy( pcDataBufTmp2 + ( iaStartOffset[iIndex] + 1 ) * sizeof(KEY_POINT), pcDataBuf + iaEndOffset[iIndex] * sizeof(KEY_POINT), ( iSize - iaEndOffset[iIndex] )* sizeof(KEY_POINT) );
    setKeyBlockDataMain( pcDataBufTmp2, iAddr, iaIndepentId );
  
  	/*����ǵ�һ��*/
    if (iAddr == sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain)
    {
    	sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain -= iaEndOffset[iIndex] - 1 - iaStartOffset[iIndex];
    }
    else
    {
    	keyTmp->llChildSize -= iaEndOffset[iIndex] - 1 - iaStartOffset[iIndex];
    	setKeyBlockDataMain( pcDataBufTmp, iAddrTmp, iaIndepentId );
    }
  }
    
  /*������*/
  #if 1
  /*ȡ�õ�һ����������������*/
  GET_BLOCK_DATA_MAIN( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
  while (sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain == 1 && sgGlobalVariableMain[iaIndepentId].llgLoopsMain > 1)
  {
    /*ȡ�õ�ǰΨһ������*/
    key = (KEY_POINT*)(pcDataBuf);
  
    /*�ͷŵ�һ�����*/
    freeKeyBlockMain( sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
  
    /*�������õ�һ�����*/
    sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain = key->llChildAddr;
    sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain = key->llChildSize;
    (sgGlobalVariableMain[iaIndepentId].llgLoopsMain)--;
    GET_BLOCK_DATA_MAIN( pcDataBuf, sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
  }
  #else
	for (iIndex = 1; iIndex <= iTotalLoops; iIndex++)
  {
  	iAddr = sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain;
    iSize = sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain;
    iTotalLoops = sgGlobalVariableMain[iaIndepentId].llgLoopsMain;
  
  	if (iSize == 1 && iTotalLoops > 1)
  	{
  		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
      getKeyBlockDataMain( pcDataBuf, iAddr, iaIndepentId );
      
      key = (KEY_POINT*)(pcDataBuf);
      
      freeKeyBlockMain( sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, iaIndepentId );
      
      /*�������õ�һ�����*/
      sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain = key->llChildAddr;
      sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain = key->llChildSize;
      sgGlobalVariableMain[iaIndepentId].llgLoopsMain--;
  	}
  	else
  	{
  		break;
  	}
  }
  #endif

  //logFmtSyn( LVNOR, "[%lld]", deleteShare->llTotalRows );
  //logFmtSyn( LVNOR, "[%lld][%lld][%lld]", sgGlobalVariableMain[iaIndepentId].llgFirstDiscMain, sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain, sgGlobalVariableMain[iaIndepentId].llgLoopsMain );
  
  /*ɾ���߽�����*/
	for (iTmp = 0; iTmp < deleteShare->iCornerSize; iTmp++)
	{
		memset( pcKey, 0x00, sizeof(pcKey) );
		strcpy( pcKey, deleteShare->pcConnerKey[iTmp] );
		//logFmtSyn( LVNOR, "pcKey[%s][%d]", pcKey, deleteShare->iCornerSize );
		iRet = managerDeleteNodeMain( pcKey, deleteShare->iConnerKeyLen[iTmp], iaIndepentId );
    //logFmtSyn( LVNOR, "iRet[%d]", iRet );
    
		if (iRet == SUCCESS)
		{
			(deleteShare->llTotalRows)++;
		}
	}
	
  return SUCCESS;
}

/**********************************************************************
��������: managerDeleteDealDeleteDataLeftMain
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
����ʱ��: 20170114
**********************************************************************/
int managerDeleteDealDeleteDataLeftMain( DELETE_SHARE *deleteShare, __int64 iaAddr, __int64 iaSize, int iaStartOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId )
{
	char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
	char pcDataBufTmp[BLOCK_SIZE]; /*�����е�����*/
	char pcKey[MAX_KEY_LEN];
	KEY_POINT *key;
	KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
	int  iTmp;
	int  iRet;
		
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
      
      strcpy( deleteShare->pcConnerKey[deleteShare->iCornerSize], pcKey );
      deleteShare->iConnerKeyLen[deleteShare->iCornerSize] = keyData->llKeyLen;
  	  
      (deleteShare->iCornerSize)++;
      
    }
    //logFmtSyn( LVNOR, "[%lld][%lld]", iaAddr, iaSize );
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    key = (KEY_POINT*)(pcDataBuf + iaStartOffset[iaLoop] * sizeof(KEY_POINT));
    managerDeleteDealDeleteDataLeftMain( deleteShare, key->llChildAddr, key->llChildSize, iaStartOffset, iaLoop + 1, iaTotalLoops, iaIndepentId );
    
    /*�����һ���������һ�����������øý��*/
    #if 1
    if (iaLoop + 1 != iaTotalLoops)
    {
      key->llChildSize = iaStartOffset[iaLoop + 1] + 1;
    }
    #endif
    
    for (iTmp = iaStartOffset[iaLoop] + 1; iTmp < iaSize; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      managerDeleteDealDeleteDataOtherMain( deleteShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
      
      freeKeyBlockMain( key->llChildAddr, iaIndepentId );
    }
    
    /*���������¼�����*/
    memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
    memcpy( pcDataBufTmp, pcDataBuf, ( iaStartOffset[iaLoop] + 1 ) * sizeof(KEY_POINT) );
    setKeyBlockDataMain( pcDataBufTmp, iaAddr, iaIndepentId );
	}
	
	return SUCCESS;
}

/**********************************************************************
��������: managerDeleteDealDeleteDataOtherMain
��������: ����˴��� delete �е�ɾ������
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
int managerDeleteDealDeleteDataOtherMain( DELETE_SHARE *deleteShare, __int64 iaAddr, __int64 iaSize, int iaLoop, int iaTotalLoops, int iaIndepentId )
{
	KEY_POINT *key;                /*�����������е����ݽṹ*/
  KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
  char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
  int  iIndex;
  
	memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
  getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
  
  /*����������һ��*/
  if (iaLoop == iaTotalLoops)
  {
    /*ɾ�����*/
    for (iIndex = 0; iIndex < iaSize; iIndex++)
    {
    	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iIndex * sizeof(KEY_VALUE_POINT));
    	
    	/*�ͷŽ���� VALUE �ļ��еĿռ�*/
	    freeValueBlockMain( keyData->llKeyAddr, iaIndepentId );
	    freeValueBlockMain( keyData->llValueAddr, iaIndepentId );
	    (deleteShare->llTotalRows)++;
	    (sgGlobalVariableMain[iaIndepentId].llgKeyValueSizeMain)--;
    }
  }
  else
  {
    /*ɾ�����*/
    for (iIndex = 0; iIndex < iaSize; iIndex++)
    {
    	key = (KEY_POINT*)(pcDataBuf + iIndex * sizeof(KEY_POINT));
    	
    	managerDeleteDealDeleteDataOtherMain( deleteShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );    	
    	freeKeyBlockMain( key->llChildAddr, iaIndepentId );
    }
  }
  
  return SUCCESS;
}

/**********************************************************************
��������: managerDeleteDealDeleteDataRightMain
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
����ʱ��: 20170114
**********************************************************************/
int managerDeleteDealDeleteDataRightMain( DELETE_SHARE *deleteShare, KEY_POINT *saKey, int iaEndOffset[], __int64 iaLoop, __int64 iaTotalLoops, int iaIndepentId )
{
	char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
	char pcDataBufTmp[BLOCK_SIZE]; /*�����е�����*/
	char pcKey[MAX_KEY_LEN];
	KEY_POINT *key;
	KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
	int  iTmp;
	int  iRet;
	__int64 llChildSize;
	
	llChildSize = saKey->llChildSize;
	
	/*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, saKey->llChildAddr, iaIndepentId );
    
		for (iTmp = 0; iTmp <= iaEndOffset[iaLoop]; iTmp++ )
	  {
	  	keyData = (KEY_VALUE_POINT*)(pcDataBuf + iTmp * sizeof(KEY_VALUE_POINT));
      
      /*�ͷŽ���� VALUE �ļ��еĿռ�*/
	    freeValueBlockMain( keyData->llKeyAddr, iaIndepentId );
	    freeValueBlockMain( keyData->llValueAddr, iaIndepentId );
	    (deleteShare->llTotalRows)++;
	    (sgGlobalVariableMain[iaIndepentId].llgKeyValueSizeMain)--;
      (saKey->llChildSize)--;
    }
    
    if (saKey->llChildSize > 0)
    {
    	memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
    	memcpy( pcDataBufTmp, pcDataBuf + (iaEndOffset[iaLoop] + 1) * sizeof(KEY_VALUE_POINT), ( llChildSize - iaEndOffset[iaLoop] - 1) * sizeof(KEY_VALUE_POINT)  );
      setKeyBlockDataMain( pcDataBufTmp, saKey->llChildAddr, iaIndepentId );
      
      /*�����Ľ�㼷��*/
      saKey->llKeyLen = keyData->llKeyLen;
      saKey->llKeyAddr = keyData->llKeyAddr;
    }
	  return SUCCESS;
	}
	else
	{
		memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, saKey->llChildAddr, iaIndepentId );
    for (iTmp = 0; iTmp < iaEndOffset[iaLoop]; iTmp++)
    {
      key = (KEY_POINT*)(pcDataBuf + iTmp * sizeof(KEY_POINT));
      managerDeleteDealDeleteDataOtherMain( deleteShare, key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, iaIndepentId );
      freeKeyBlockMain( key->llChildAddr, iaIndepentId );
    }
    
    saKey->llChildSize -= iaEndOffset[iaLoop];
    
    key = (KEY_POINT*)(pcDataBuf + iaEndOffset[iaLoop] * sizeof(KEY_POINT));
    iRet = managerDeleteDealDeleteDataRightMain( deleteShare, key, iaEndOffset, iaLoop + 1, iaTotalLoops, iaIndepentId );
    
    /*�����һ��������Ϊ0���ͷ�*/
    if (key->llChildSize == 0)
    {
    	freeKeyBlockMain( key->llChildAddr, iaIndepentId );
    	(saKey->llChildSize)--;
    }
    
    /*�����ǰ����������*/
    if (saKey->llChildSize > 0)
    {      
      /*�����Ľ�㼷��*/
      saKey->llKeyLen = key->llKeyLen;
      saKey->llKeyAddr = key->llKeyAddr;
      
      /*���������¼�����*/
      memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );
      memcpy( pcDataBufTmp, pcDataBuf + iaEndOffset[iaLoop] * sizeof(KEY_POINT), ( llChildSize - iaEndOffset[iaLoop] ) * sizeof(KEY_POINT) );
      setKeyBlockDataMain( pcDataBufTmp, saKey->llChildAddr, iaIndepentId );
    }
    
	}
	
	return SUCCESS;
}

/********************************************************
�������ƣ�managerDeleteNodeMain
�������ܣ�ɾ���������� 
��ڲ�����
��    һ: ����ֵ                 I
��    �أ�0��ʾ�ɹ���������ʾʧ�� 
��    �ߣ������ 
����ʱ�䣺20170114
�޸�ʱ�䣺 
*********************************************************/
int managerDeleteNodeMain( char *pcaKey, int iaKeyLen, int iaIndepentId )
{
	int iRet;
  
	if (sgGlobalVariableMain[iaIndepentId].llgFirstSizeMain == 0)
  {
  	return NOT_FOUND;
  }
  
  /*�����������*/
  if (sgGlobalVariableMain[iaIndepentId].llgLoopsMain >= 3)
  {
  	iRet = deleteIndexNode( NULL, NULL, 1, pcaKey, iaKeyLen, iaIndepentId );
  }
  else
  /*���ֻ������*/
  if (sgGlobalVariableMain[iaIndepentId].llgLoopsMain == 2)
  {
  	iRet = deleteIndexNodeTwoLoops( pcaKey, iaKeyLen, iaIndepentId );
  }
  else
  /*���ֻ��һ��*/
  if (sgGlobalVariableMain[iaIndepentId].llgLoopsMain <= 1)
  {
  	iRet = deleteIndexNodeOneLoopsMain( pcaKey, iaKeyLen, iaIndepentId );
  }
  else
  {  
    iRet = ERROR;
  }
  
  if (iRet == SUCCESS)
  {
  	(sgGlobalVariableMain[iaIndepentId].llgKeyValueSizeMain)--;
  }
  
  return iRet;
}
