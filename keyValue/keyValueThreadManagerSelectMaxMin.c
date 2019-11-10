#define __KEY_VALUE_THREAD_MANAGER_SELECT_MAX_MIN_C

/*
������ MANAGER �е� SELECT ���
*/

#ifndef __COMMON_HEAD_H
  #include "commonHead.h"
#endif

void managerSelectMinDeal( int iaSocket );
void managerSelectMinDealResult( __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, KEY_VALUE_POINT *keyDataOut, int iaIndepentFileId );
void managerSelectMinDealResultMain( __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, KEY_VALUE_POINT *keyDataOut, int iaIndepentId );
void managerSelectMaxDeal( int iaSocket );
void managerSelectMaxDealResult( __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, KEY_VALUE_POINT *keyDataOut, int iaIndepentFileId );
void managerSelectMaxDealResultMain( __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, KEY_VALUE_POINT *keyDataOut, int iaIndepentId );

/**********************************************************************
��������: managerSelectMinDeal
��������: ����˴��� select min ����
��    ����
��    һ��SOCKET               I
��    �������ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20161022
**********************************************************************/
void managerSelectMinDeal( int iaSocket )
{
/*
�������ݽṹ������+����
�������ݽṹ���������+KEYֵ����+KEYֵ+VALUEֵ����+VALUEֵ
*/
#define MANAGER_SELECT_MIN_DEAL_SEND_ERROR( value ) \
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
	char pcSendBuf[1 + MAX_KEY_LEN + KEY_VALUE_LENGTH + MAX_VALUE_LEN + KEY_VALUE_LENGTH + 1];
	char cResult;
	KEY_VALUE_POINT keyData;
	char pcKey[MAX_KEY_LEN + 1];
	char pcValue[MAX_VALUE_LEN + 1];
	char pcKeyMain[MAX_KEY_LEN + 1];
	char pcValueMain[MAX_VALUE_LEN + 1];
	char pcKeyLen[KEY_VALUE_LENGTH + 1];
	char pcValueLen[KEY_VALUE_LENGTH + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	char pcMinKey[INDEPENDENT_FILE_SIZE][MAX_KEY_LEN + 1];
	char pcMinValue[INDEPENDENT_FILE_SIZE][MAX_VALUE_LEN + 1];
	//char pcMinKeyMain[INDEPENDENT_SIZE][MAX_KEY_LEN + 1];��windows���棬���ܾ�̬����̫���ڴ棬�ͻ���ֺ���ִ�в��ˡ�
	//char pcMinValueMain[INDEPENDENT_SIZE][MAX_VALUE_LEN + 1];
	char *pcMinKeyMain[INDEPENDENT_SIZE];
	char *pcMinValueMain[INDEPENDENT_SIZE];
	__int64  llDisc;
	__int64  llSize;
	__int64  llLoops;
	int  iKeyLen;
	int  iValueLen;
	int  iSendLen;
	int  iRet;
	int  iTmp;
	int  iIndepentId;
	int  iIndepentFileId;
  
  llSize = 0;
  for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
  {
  	llSize += sgBase[iTmp].llKeyValueSize;
  }
	
	if (llSize == 0)
	{
		MANAGER_SELECT_MIN_DEAL_SEND_ERROR( ERROR );
		return;
	}
	
	for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
	{
	  pcMinKeyMain[iTmp] = malloc( MAX_KEY_LEN + 1 );
	  if (pcMinKeyMain[iTmp] == NULL)
	  {
	  	return;
	  }
	  memset( pcMinKeyMain[iTmp], 0x00, MAX_KEY_LEN + 1 );
	  
	  pcMinValueMain[iTmp] = malloc( MAX_VALUE_LEN + 1 );
	  if (pcMinValueMain[iTmp] == NULL)
	  {
	  	return;
	  }
	  memset( pcMinValueMain[iTmp], 0x00, MAX_VALUE_LEN + 1 );
  }
  
	/*�Ӷ�������ļ�ȡ�������Сֵ*/
	memset( pcMinKey, 0x00, sizeof(pcMinKey) );
	memset( pcMinValue, 0x00, sizeof(pcMinValue) );

	MANAGER_RDLOCK();
	//logFmtSyn( LVNOR, "a" );
	for (iIndepentId = 0; iIndepentId < INDEPENDENT_SIZE; iIndepentId++)
	{
		
	  llDisc = sgGlobalVariableMain[iIndepentId].llgFirstDiscMain;
	  llSize = sgGlobalVariableMain[iIndepentId].llgFirstSizeMain;
	  llLoops = sgGlobalVariableMain[iIndepentId].llgLoopsMain;
	  //logFmtSyn( LVNOR, "%d:%d:%d", iIndepentId, sgGlobalVariableMain[iIndepentId].llgFirstDiscMain, sgGlobalVariableMain[iIndepentId].llgFirstSizeMain, sgGlobalVariableMain[iIndepentId].llgLoopsMain );
	  memset( &keyData, 0x00, sizeof(keyData) );	
	  managerSelectMinDealResultMain( llDisc, llSize, 1, llLoops, &keyData, iIndepentId );
	  
    valueToBufMain( pcMinKeyMain[iIndepentId], keyData.llKeyAddr, keyData.llKeyLen, iIndepentId );
    valueToBufMain( pcMinValueMain[iIndepentId], keyData.llValueAddr, keyData.llValueLen, iIndepentId );
    
    //logFmtSyn( LVNOR, "key[%s]value[%s]", pcMinKeyMain[iIndepentId], pcMinValueMain[iIndepentId] );
  }
  //logFmtSyn( LVNOR, "a" );
	for (iIndepentFileId = 0; iIndepentFileId < INDEPENDENT_FILE_SIZE; iIndepentFileId++)
	{
	  llDisc = sgBase[iIndepentFileId].llFirstDisc;
	  llSize = sgBase[iIndepentFileId].llFirstSize;
	  llLoops = sgBase[iIndepentFileId].llLoops;
	  
	  memset( &keyData, 0x00, sizeof(keyData) );	
	  managerSelectMinDealResult( llDisc, llSize, 1, llLoops, &keyData, iIndepentFileId );

    valueToBuf( pcMinKey[iIndepentFileId], keyData.llKeyAddr, keyData.llKeyLen, iIndepentFileId );
    valueToBuf( pcMinValue[iIndepentFileId], keyData.llValueAddr, keyData.llValueLen, iIndepentFileId );
  }
  //logFmtSyn( LVNOR, "a" );
  MANAGER_RDUNLOCK();
  
  memset( pcKeyMain, 0x00, sizeof(pcKeyMain) );
  strcpy( pcKeyMain, pcMinKeyMain[0] );
  
  memset( pcValueMain, 0x00, sizeof(pcValueMain) );
  strcpy( pcValueMain, pcMinValueMain[0] );
  
  for (iTmp = 1; iTmp < INDEPENDENT_SIZE; iTmp++)
	{
		if (strcmp( pcMinKeyMain[iTmp], pcKeyMain ) < 0)
		{
			memset( pcKeyMain, 0x00, sizeof(pcKeyMain) );
      strcpy( pcKeyMain, pcMinKeyMain[iTmp] );
      
      memset( pcValueMain, 0x00, sizeof(pcValueMain) );
      strcpy( pcValueMain, pcMinValueMain[iTmp] );
		}
	}
	
	//logFmtSyn( LVNOR, "key[%s]value[%s]", pcKeyMain, pcValueMain );
  
  /*�ٴӶ������ȡ����С��һ��*/
  memset( pcKey, 0x00, sizeof(pcKey) );
  strcpy( pcKey, pcMinKey[0] );
  
  memset( pcValue, 0x00, sizeof(pcValue) );
  strcpy( pcValue, pcMinValue[0] );
  
  for (iTmp = 1; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
	{
		if (strcmp( pcMinKey[iTmp], pcKey ) < 0)
		{
			memset( pcKey, 0x00, sizeof(pcKey) );
      strcpy( pcKey, pcMinKey[iTmp] );
      
      memset( pcValue, 0x00, sizeof(pcValue) );
      strcpy( pcValue, pcMinValue[iTmp] );
		}
	}
	
	//logFmtSyn( LVNOR, "key[%s]value[%s]", pcKey, pcValue );
	
	if (strlen( pcKey ) != 0 && strlen( pcKeyMain ) != 0)
	{
		if (strcmp( pcKeyMain, pcKey ) < 0)
	  {
	  	memset( pcKey, 0x00, sizeof(pcKey) );
      strcpy( pcKey, pcKeyMain );
      
      memset( pcValue, 0x00, sizeof(pcValue) );
      strcpy( pcValue, pcValueMain );
	  }
	}
	else
	if (strlen( pcKey ) == 0 && strlen( pcKeyMain ) != 0)
	{
		memset( pcKey, 0x00, sizeof(pcKey) );
    strcpy( pcKey, pcKeyMain );
      
    memset( pcValue, 0x00, sizeof(pcValue) );
    strcpy( pcValue, pcValueMain );
  }
  
	//logFmtSyn( LVNOR, "key[%s]value[%s]", pcKey, pcValue );
	
	for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
	{
	  free( pcMinKeyMain[iTmp] );
	  free( pcMinValueMain[iTmp] );
  }
	
	/*�鷢������*/
  memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
  iKeyLen = strlen( pcKey );
  memset( pcKeyLen, 0x00, sizeof(pcKeyLen) );
  sprintf( pcKeyLen, "%0*d", KEY_VALUE_LENGTH, iKeyLen );
  
  iValueLen = strlen( pcValue );
  memset( pcValueLen, 0x00, sizeof(pcValueLen) );
  sprintf( pcValueLen, "%0*d", KEY_VALUE_LENGTH, iValueLen );
  
  sprintf( &cResult, "%d", SUCCESS );
  memcpy( pcSendBuf, &cResult, 1 );
  memcpy( pcSendBuf + 1, pcKeyLen, KEY_VALUE_LENGTH );
  memcpy( pcSendBuf + 1 + KEY_VALUE_LENGTH, pcKey, iKeyLen );
  memcpy( pcSendBuf + 1 + KEY_VALUE_LENGTH + iKeyLen, pcValueLen, KEY_VALUE_LENGTH );
  memcpy( pcSendBuf + 1 + KEY_VALUE_LENGTH + iKeyLen + KEY_VALUE_LENGTH, pcValue, iValueLen );
  
  iSendLen = 1 + KEY_VALUE_LENGTH + iValueLen + KEY_VALUE_LENGTH + iValueLen;
  
  memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
  
  send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
  send( iaSocket, pcSendBuf, iSendLen, 0 );
}

/**********************************************************************
��������: managerSelectMinDealResult
��������: ����˴��� select min �е�����
��    ����
��    һ�����ݵ�ַ     I
��    ��������         I
��    ��������         I
��    �ģ��ܼ���       I
��    �壺���ݽṹ��   O
��    �أ�
��    ��: �����
����ʱ��: 20161106
**********************************************************************/
void managerSelectMinDealResult( __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, KEY_VALUE_POINT *keyDataOut, int iaIndepentFileId )
{
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	KEY_POINT *key;                /*�����������е����ݽṹ*/
  KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
  char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
  int  iRet;
  
  if (iaSize == 0)
  {
  	return;
  }
  
  /*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
		/*ȡ�����һ����������*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
    memcpy( keyDataOut, pcDataBuf, sizeof(KEY_VALUE_POINT) );
    
    return;
	}
  /*�������һ��*/
	else
	{
		/*ȡ����������������*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
    key = (KEY_POINT*)(pcDataBuf);
    
    return managerSelectMinDealResult( key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, keyDataOut, iaIndepentFileId );
  }
}

/**********************************************************************
��������: managerSelectMinDealResultMain
��������: ����˴��� select min �е�����
��    ����
��    һ�����ݵ�ַ     I
��    ��������         I
��    ��������         I
��    �ģ��ܼ���       I
��    �壺���ݽṹ��   O
��    �أ�
��    ��: �����
����ʱ��: 20161106
**********************************************************************/
void managerSelectMinDealResultMain( __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, KEY_VALUE_POINT *keyDataOut, int iaIndepentId )
{
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	KEY_POINT *key;                /*�����������е����ݽṹ*/
  KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
  char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
  int  iRet;
  
  if (iaSize == 0)
  {
  	return;
  }
  //logFmtSyn( LVNOR, "a" );
  /*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
		/*ȡ�����һ����������*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    memcpy( keyDataOut, pcDataBuf, sizeof(KEY_VALUE_POINT) );
    
    return;
	}
  /*�������һ��*/
	else
	{
		/*ȡ����������������*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    key = (KEY_POINT*)(pcDataBuf);
    //logFmtSyn( LVNOR, "a" );
    return managerSelectMinDealResultMain( key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, keyDataOut, iaIndepentId );
  }
}

/**********************************************************************
��������: managerSelectMaxDeal
��������: ����˴��� select max ����
��    ����
��    һ��SOCKET               I
��    �������ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20161022
**********************************************************************/
void managerSelectMaxDeal( int iaSocket )
{
#define MANAGER_SELECT_MAX_DEAL_SEND_ERROR( value ) \
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
	char pcSendBuf[1 + MAX_KEY_LEN + KEY_VALUE_LENGTH + MAX_VALUE_LEN + KEY_VALUE_LENGTH + 1];
	char cResult;
	KEY_VALUE_POINT keyData;
	char pcKey[MAX_KEY_LEN + 1];
	char pcValue[MAX_KEY_LEN + 1];
	char pcKeyMain[MAX_KEY_LEN + 1];
	char pcValueMain[MAX_KEY_LEN + 1];
	char pcKeyLen[KEY_VALUE_LENGTH + 1];
	char pcValueLen[KEY_VALUE_LENGTH + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	char pcMaxKey[INDEPENDENT_FILE_SIZE][MAX_KEY_LEN + 1];
	char pcMaxValue[INDEPENDENT_FILE_SIZE][MAX_VALUE_LEN + 1];
	//char pcMaxKeyMain[INDEPENDENT_SIZE][MAX_KEY_LEN + 1];
	//char pcMaxValueMain[INDEPENDENT_SIZE][MAX_VALUE_LEN + 1];
	char *pcMaxKeyMain[INDEPENDENT_SIZE];
	char *pcMaxValueMain[INDEPENDENT_SIZE];
	__int64  llDisc;
	__int64  llSize;
	__int64  llLoops;
	int  iKeyLen;
	int  iValueLen;
	int  iRet;
	int  iTmp;
	int  iSendLen;
	int  iIndepentId;
	int  iIndepentFileId;
  
  llSize = 0;
  for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
  {
  	llSize += sgBase[iTmp].llKeyValueSize;
  }
	
	if (llSize == 0)
	{
		MANAGER_SELECT_MAX_DEAL_SEND_ERROR( ERROR );
		return;
	}

	for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
	{
	  pcMaxKeyMain[iTmp] = malloc( MAX_KEY_LEN + 1 );
	  if (pcMaxKeyMain[iTmp] == NULL)
	  {
	  	return;
	  }
	  memset( pcMaxKeyMain[iTmp], 0x00, MAX_KEY_LEN + 1 );
	  
	  pcMaxValueMain[iTmp] = malloc( MAX_VALUE_LEN + 1 );
	  if (pcMaxValueMain[iTmp] == NULL)
	  {
	  	return;
	  }
	  memset( pcMaxValueMain[iTmp], 0x00, MAX_VALUE_LEN + 1 );
  }

	/*�Ӷ�������ļ�ȡ��������ֵ*/
	memset( pcMaxKey, 0x00, sizeof(pcMaxKey) );
	memset( pcMaxValue, 0x00, sizeof(pcMaxValue) );
	MANAGER_RDLOCK();

	for (iIndepentId = 0; iIndepentId < INDEPENDENT_SIZE; iIndepentId++)
	{
	  llDisc = sgGlobalVariableMain[iIndepentId].llgFirstDiscMain;
	  llSize = sgGlobalVariableMain[iIndepentId].llgFirstSizeMain;
	  llLoops = sgGlobalVariableMain[iIndepentId].llgLoopsMain;

	  memset( &keyData, 0x00, sizeof(keyData) );	
	  managerSelectMaxDealResultMain( llDisc, llSize, 1, llLoops, &keyData, iIndepentId );

    valueToBufMain( pcMaxKeyMain[iIndepentId], keyData.llKeyAddr, keyData.llKeyLen, iIndepentId );
    valueToBufMain( pcMaxValueMain[iIndepentId], keyData.llValueAddr, keyData.llValueLen, iIndepentId );
  }

	for (iIndepentFileId = 0; iIndepentFileId < INDEPENDENT_FILE_SIZE; iIndepentFileId++)
	{
	  llDisc = sgBase[iIndepentFileId].llFirstDisc;
	  llSize = sgBase[iIndepentFileId].llFirstSize;
	  llLoops = sgBase[iIndepentFileId].llLoops;
	  
	  memset( &keyData, 0x00, sizeof(keyData) );	
	  managerSelectMaxDealResult( llDisc, llSize, 1, llLoops, &keyData, iIndepentFileId );
	  
    valueToBuf( pcMaxKey[iIndepentFileId], keyData.llKeyAddr, keyData.llKeyLen, iIndepentFileId );
    valueToBuf( pcMaxValue[iIndepentFileId], keyData.llValueAddr, keyData.llValueLen, iIndepentFileId );
  }
  MANAGER_RDUNLOCK();

  memset( pcKeyMain, 0x00, sizeof(pcKeyMain) );
  strcpy( pcKeyMain, pcMaxKeyMain[0] );
  
  memset( pcValueMain, 0x00, sizeof(pcValueMain) );
  strcpy( pcValueMain, pcMaxValueMain[0] );
  
  for (iTmp = 1; iTmp < INDEPENDENT_SIZE; iTmp++)
	{
		if (strcmp( pcMaxKeyMain[iTmp], pcKeyMain ) > 0)
		{
			memset( pcKeyMain, 0x00, sizeof(pcKeyMain) );
      strcpy( pcKeyMain, pcMaxKeyMain[iTmp] );
      
      memset( pcValueMain, 0x00, sizeof(pcValueMain) );
      strcpy( pcValueMain, pcMaxValueMain[iTmp] );
		}
	}

  /*�ٴӶ������ȡ������һ��*/
  memset( pcKey, 0x00, sizeof(pcKey) );
  strcpy( pcKey, pcMaxKey[0] );
  
  memset( pcValue, 0x00, sizeof(pcValue) );
  strcpy( pcValue, pcMaxValue[0] );

  for (iTmp = 1; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
	{
		if (strcmp( pcMaxKey[iTmp], pcKey ) > 0)
		{
			memset( pcKey, 0x00, sizeof(pcKey) );
      strcpy( pcKey, pcMaxKey[iTmp] );
      
      memset( pcValue, 0x00, sizeof(pcValue) );
      strcpy( pcValue, pcMaxValue[iTmp] );
		}
	}

	if (strlen( pcKey ) != 0 && strlen( pcKeyMain ) != 0)
	{
		if (strcmp( pcKeyMain, pcKey ) > 0)
	  {
	  	memset( pcKey, 0x00, sizeof(pcKey) );
      strcpy( pcKey, pcKeyMain );
      
      memset( pcValue, 0x00, sizeof(pcValue) );
      strcpy( pcValue, pcValueMain );
	  }
	}
	else
	if (strlen( pcKey ) == 0 && strlen( pcKeyMain ) != 0)
	{
		memset( pcKey, 0x00, sizeof(pcKey) );
    strcpy( pcKey, pcKeyMain );
      
    memset( pcValue, 0x00, sizeof(pcValue) );
    strcpy( pcValue, pcValueMain );
  }

	for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
	{
	  free( pcMaxKeyMain[iTmp] );
	  free( pcMaxValueMain[iTmp] );
  }
	
	/*�鷢������*/
  memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
  iKeyLen = strlen( pcKey );
  memset( pcKeyLen, 0x00, sizeof(pcKeyLen) );
  sprintf( pcKeyLen, "%0*d", KEY_VALUE_LENGTH, iKeyLen );
  
  iValueLen = strlen( pcValue );
  memset( pcValueLen, 0x00, sizeof(pcValueLen) );
  sprintf( pcValueLen, "%0*d", KEY_VALUE_LENGTH, iValueLen );
  
  sprintf( &cResult, "%d", SUCCESS );
  memcpy( pcSendBuf, &cResult, 1 );
  memcpy( pcSendBuf + 1, pcKeyLen, KEY_VALUE_LENGTH );
  memcpy( pcSendBuf + 1 + KEY_VALUE_LENGTH, pcKey, iKeyLen );
  memcpy( pcSendBuf + 1 + KEY_VALUE_LENGTH + iKeyLen, pcValueLen, KEY_VALUE_LENGTH );
  memcpy( pcSendBuf + 1 + KEY_VALUE_LENGTH + iKeyLen + KEY_VALUE_LENGTH, pcValue, iValueLen );
  
  iSendLen = 1 + KEY_VALUE_LENGTH + iValueLen + KEY_VALUE_LENGTH + iValueLen;
  
  memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
  
  send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
  send( iaSocket, pcSendBuf, iSendLen, 0 );
}

/**********************************************************************
��������: managerSelectMaxDealResult
��������: ����˴��� select max �е�����
��    ����
��    һ�����ݵ�ַ     I
��    ��������         I
��    ��������         I
��    �ģ��ܼ���       I
��    �壺���ݽṹ��   O
��    �أ�
��    ��: �����
����ʱ��: 20161106
**********************************************************************/
void managerSelectMaxDealResult( __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, KEY_VALUE_POINT *keyDataOut, int iaIndepentFileId )
{
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	KEY_POINT *key;                /*�����������е����ݽṹ*/
  KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
  char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
  int  iRet;
  
  if (iaSize == 0)
  {
  	return;
  }
  
  /*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
		/*ȡ�����һ����������*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
    memcpy( keyDataOut, pcDataBuf + (iaSize  - 1)* sizeof(KEY_VALUE_POINT) , sizeof(KEY_VALUE_POINT) );
    
    return;
	}
  /*�������һ��*/
	else
	{
		/*ȡ����������������*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockData( pcDataBuf, iaAddr, iaIndepentFileId );
    
    key = (KEY_POINT*)(pcDataBuf + (iaSize  - 1)* sizeof(KEY_POINT));
    
    return managerSelectMaxDealResult( key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, keyDataOut, iaIndepentFileId );
  }
}

/**********************************************************************
��������: managerSelectMaxDealResultMain
��������: ����˴��� select max �е�����
��    ����
��    һ�����ݵ�ַ     I
��    ��������         I
��    ��������         I
��    �ģ��ܼ���       I
��    �壺���ݽṹ��   O
��    �أ�
��    ��: �����
����ʱ��: 20170217
**********************************************************************/
void managerSelectMaxDealResultMain( __int64 iaAddr, __int64 iaSize, __int64 iaLoop, __int64 iaTotalLoops, KEY_VALUE_POINT *keyDataOut, int iaIndepentId )
{
	char pcKey[MAX_KEY_LEN];
	char pcValue[MAX_VALUE_LEN];
	KEY_POINT *key;                /*�����������е����ݽṹ*/
  KEY_VALUE_POINT *keyData;      /*��������е����ݽṹ*/
  char pcDataBuf[BLOCK_SIZE];    /*�����е�����*/
  int  iRet;
  
  if (iaSize == 0)
  {
  	return;
  }
  
  /*��������һ��*/
	if (iaLoop == iaTotalLoops)
	{
		/*ȡ�����һ����������*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    memcpy( keyDataOut, pcDataBuf + (iaSize  - 1)* sizeof(KEY_VALUE_POINT) , sizeof(KEY_VALUE_POINT) );
    
    return;
	}
  /*�������һ��*/
	else
	{
		/*ȡ����������������*/
    memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );
    getKeyBlockDataMain( pcDataBuf, iaAddr, iaIndepentId );
    
    key = (KEY_POINT*)(pcDataBuf + (iaSize  - 1)* sizeof(KEY_POINT));
    
    return managerSelectMaxDealResultMain( key->llChildAddr, key->llChildSize, iaLoop + 1, iaTotalLoops, keyDataOut, iaIndepentId );
  }
}

