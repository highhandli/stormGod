#define __KEY_VALUE_THREAD_MANAGER_SHOW_C

/*
������ MANAGER �е� SHOW ���
*/

#ifndef __COMMON_HEAD_H
  #include "commonHead.h"
#endif

void managerShowRamsDeal( int iaSocket );
void managerShowDiscsDeal( int iaSocket );
void managerShowThreadsDeal( int iaSocket );
void managerShowKeysDeal( int iaSocket );
void managerShowLoopsDeal( int iaSocket );

/**********************************************************************
��������: managerShowRamsDeal
��������: ����˴��� show rams ����
��    ����
��    һ��SOCKET               I
��    �������ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20161022
**********************************************************************/
void managerShowRamsDeal( int iaSocket )
{
/*
�������ݽṹ������+����
�������ݽṹ���������+KEY�ļ������������� + KEY�ļ���������ʹ��ƫ��������λ B�� + VALUE�ļ������������� + VALUE�ļ���������ʹ��ƫ��������λ B��
*/
	char cResult;
	char pcSendBuf[1 + INT64_LEN + INT64_LEN + INT64_LEN + INT64_LEN + 1];
	char pcKeyTotal[INT64_LEN + 1];
	char pcKeyUsed[INT64_LEN + 1];
	char pcValueTotal[INT64_LEN + 1];
	char pcValueUsed[INT64_LEN + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	int  iSendLen;
	__int64 llRam;
  __int64 llValue;
  int iTmp;
	
	MANAGER_RDLOCK();

  /*ȡKEY�ļ�������������*/
  memset( pcKeyTotal, 0x00, sizeof(pcKeyTotal) );
  sprintf( pcKeyTotal, "%0*lld", INT64_LEN, llgKeyRamSizeMain );
  
  llRam = 0;
  llValue = 0;
  for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
  {
  	llRam += sgGlobalVariableMain[iTmp].llgKeyRamUsedMain;
  	llValue += sgGlobalVariableMain[iTmp].llgValueRamUsedMain;
  }
  
  /*ȡKEY�ļ���������ʹ��ƫ����*/
  memset( pcKeyUsed, 0x00, sizeof(pcKeyUsed) );
  sprintf( pcKeyUsed, "%0*lld", INT64_LEN, llRam );
  
  /*ȡVALUE�ļ�������������*/
  memset( pcValueTotal, 0x00, sizeof(pcValueTotal) );
  sprintf( pcValueTotal, "%0*lld", INT64_LEN, llgValueRamSizeMain );
  
  /*ȡVALUE�ļ���������ʹ��ƫ����*/
  memset( pcValueUsed, 0x00, sizeof(pcValueUsed) );
  sprintf( pcValueUsed, "%0*lld", INT64_LEN, llValue );
  
  MANAGER_RDUNLOCK();
  
  /*�鷢������*/
  memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
  
  sprintf( &cResult, "%d", SUCCESS );
  memcpy( pcSendBuf, &cResult, 1 );
  memcpy( pcSendBuf + 1, pcKeyTotal, INT64_LEN );
  memcpy( pcSendBuf + 1 + INT64_LEN, pcKeyUsed, INT64_LEN );
  memcpy( pcSendBuf + 1 + INT64_LEN + INT64_LEN, pcValueTotal, INT64_LEN );
  memcpy( pcSendBuf + 1 + INT64_LEN + INT64_LEN + INT64_LEN, pcValueUsed, INT64_LEN );
  
  iSendLen = 1 + INT64_LEN + INT64_LEN + INT64_LEN + INT64_LEN;
  
  memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
  
  send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
  send( iaSocket, pcSendBuf, iSendLen, 0 );
  
  logFmtSyn( LVNOR, "send[%s]", pcSendBuf );
  
  return;
}

/**********************************************************************
��������: managerShowDiscsDeal
��������: ����˴��� show discs ����
��    ����
��    һ��SOCKET               I
��    �������ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20161022
**********************************************************************/
void managerShowDiscsDeal( int iaSocket )
{
/*
�������ݽṹ������+����
�������ݽṹ���������+KEY�ļ����ֵ+KEY�ļ���ǰֵ+KEY�ļ�����ֵ+ KEY�ļ�ʹ��ƫ���� + KEY�ļ���ʹ�ô�С+VALUE�ļ����ֵ+VALUE�ļ���ǰֵ+VALUE�ļ�����ֵ+ VALUE�ļ�ʹ��ƫ����+VALUE�ļ���ʹ�ô�С
*/
  char pcKeyFreeBlock[BLOCK_SIZE];        /* KEY �ļ����м�¼��*/
  char pcValueFreeBlock[BLOCK_SIZE];      /* VALUE �ļ����м�¼��*/
	char cResult;
	char pcSendBuf[1 + INT64_LEN * 10 + 1];
	char pcKeyMax[INT64_LEN + 1];
	char pcKeyTotal[INT64_LEN + 1];
	char pcKeyUpper[INT64_LEN + 1];
	char pcKeyUsed[INT64_LEN + 1];
	char pcKeyFree[INT64_LEN + 1];
	char pcValueMax[INT64_LEN + 1];
	char pcValueTotal[INT64_LEN + 1];
	char pcValueUpper[INT64_LEN + 1];
	char pcValueUsed[INT64_LEN + 1];
	char pcValueFree[INT64_LEN + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	int  iSendLen;
	int  iTmp;
	__int64 llKeyFree;
	__int64 llValueFree;
	__int64 llBlock;
	__int64 llKeyFileOffset;
	__int64 llValueFileOffset;
	__int64 llKeyFileSize;
	__int64 llValueFileSize;
	
	MANAGER_RDLOCK();
	
	/*ȡKEY�ļ����ֵ*/
  memset( pcKeyMax, 0x00, sizeof(pcKeyMax) );
  sprintf( pcKeyMax, "%0*lld", INT64_LEN, llgFileMaxSize );
  
  /*ȡKEY�ļ���ǰֵ*/
  llKeyFileSize = 0;
  for (iTmp = 0; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
  {
  	llKeyFileSize += llgKeyFileSize[iTmp];
  }
  memset( pcKeyTotal, 0x00, sizeof(pcKeyTotal) );
  sprintf( pcKeyTotal, "%0*lld", INT64_LEN, llKeyFileSize );
  
  /*ȡKEY�ļ�����ֵ*/
  memset( pcKeyUpper, 0x00, sizeof(pcKeyUpper) );
  sprintf( pcKeyUpper, "%0*lld", INT64_LEN, llgFileUpperSize );
  
  /*ȡKEY�ļ�ʹ��ƫ����*/
  llKeyFileOffset = 0;
  for (iTmp = 1; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
	{
		llKeyFileOffset += sgBase[iTmp].llKeyFileOffset;
	}
	llKeyFileOffset <<= BLOCK_LEFT_MOVE;
	
  memset( pcKeyUsed, 0x00, sizeof(pcKeyUsed) );
  sprintf( pcKeyUsed, "%0*lld", INT64_LEN, llKeyFileOffset );
	
	/*����KEY�ļ���ʹ�ô�С*/
	llKeyFree = 0;
	for (iTmp = 0; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
	{
		/*��ȫ�ֱ���copy������*/
    memset( pcKeyFreeBlock, 0x00, sizeof(pcKeyFreeBlock) );
    memcpy( pcKeyFreeBlock, pcgKeyFree[iTmp], BLOCK_SIZE );
    
    /*ȡ�õ�һ�����п������*/
    llKeyFree += (__int64)(sgBase[iTmp].llKeyFreeOffset/sizeof(__int64));
    
    /*ָ����һ����*/
    llBlock = *((__int64*)(pcKeyFreeBlock + BLOCK_SIZE - sizeof(__int64)));
    
    /*ѭ������*/
    while (llBlock != 0)
    {
    	llKeyFree += (__int64)(BLOCK_SIZE/sizeof(__int64));
    	llBlock = *((__int64*)(pcKeyFreeBlock + BLOCK_SIZE - sizeof(__int64)));
    	memset( pcKeyFreeBlock, 0x00, sizeof(pcKeyFreeBlock) );
  	  getKeyBlockData( pcKeyFreeBlock, llBlock, iTmp );
    }
  }
  memset( pcKeyFree, 0x00, sizeof(pcKeyFree) );
  sprintf( pcKeyFree, "%0*lld", INT64_LEN, llKeyFree << BLOCK_LEFT_MOVE );
  
  
  /*ȡVALUE�ļ����ֵ*/
  memset( pcValueMax, 0x00, sizeof(pcValueMax) );
  sprintf( pcValueMax, "%0*lld", INT64_LEN, llgFileMaxSize );
  
  /*ȡVALUE�ļ���ǰֵ*/
  llValueFileSize = 0;
  for (iTmp = 0; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
	{
		llValueFileSize += llgValueFileSize[iTmp];
	}
  memset( pcValueTotal, 0x00, sizeof(pcValueTotal) );
  sprintf( pcValueTotal, "%0*lld", INT64_LEN, llValueFileSize );
  
  /*ȡVALUE�ļ�����ֵ*/
  memset( pcValueUpper, 0x00, sizeof(pcValueUpper) );
  sprintf( pcValueUpper, "%0*lld", INT64_LEN, llgFileUpperSize );
  
  /*ȡVALUE�ļ�ʹ��ƫ����*/
  llValueFileOffset = 0;
  for (iTmp = 0; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
	{
		llValueFileOffset += sgBase[iTmp].llValueFileOffset;
	}
  memset( pcValueUsed, 0x00, sizeof(pcValueUsed) );
  sprintf( pcValueUsed, "%0*lld", INT64_LEN, llValueFileOffset );
	
	/*����VALUE�ļ���ʹ�ô�С*/
	llValueFree = 0;
	for (iTmp = 0; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
	{
		/*��ȫ�ֱ���copy������*/
    memset( pcValueFreeBlock, 0x00, sizeof(pcValueFreeBlock) );
    memcpy( pcValueFreeBlock, pcgValueFree[iTmp], BLOCK_SIZE );
    
    /*ȡ�õ�һ�����п������*/
    llValueFree += (__int64)(sgBase[iTmp].llValueFreeOffset/sizeof(__int64));
    
    /*ָ����һ����*/
    llBlock = *((__int64*)(pcValueFreeBlock + BLOCK_SIZE - sizeof(__int64)));
    
    /*ѭ������*/
    while (llBlock != 0)
    {
    	llValueFree += (__int64)(BLOCK_SIZE/sizeof(__int64));
    	llBlock = *((__int64*)(pcValueFreeBlock + BLOCK_SIZE - sizeof(__int64)));
    	memset( pcValueFreeBlock, 0x00, sizeof(pcValueFreeBlock) );
  	  getKeyBlockData( pcValueFreeBlock, llBlock, iTmp );
    }
  }
  memset( pcValueFree, 0x00, sizeof(pcValueFree) );
  sprintf( pcValueFree, "%0*lld", INT64_LEN, llValueFree << VALUE_LEFT_MOVE );
  
  MANAGER_RDUNLOCK();
  
  /*�鷢������*/
  memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
  
  sprintf( &cResult, "%d", SUCCESS );
  memcpy( pcSendBuf, &cResult, 1 );
  memcpy( pcSendBuf + 1, pcKeyMax, INT64_LEN );
  memcpy( pcSendBuf + 1 + INT64_LEN, pcKeyTotal, INT64_LEN );
  memcpy( pcSendBuf + 1 + INT64_LEN * 2, pcKeyUpper, INT64_LEN );
  memcpy( pcSendBuf + 1 + INT64_LEN * 3, pcKeyUsed, INT64_LEN );
  memcpy( pcSendBuf + 1 + INT64_LEN * 4, pcKeyFree, INT64_LEN );
  
  memcpy( pcSendBuf + 1 + INT64_LEN * 5, pcValueMax, INT64_LEN );
  memcpy( pcSendBuf + 1 + INT64_LEN * 6, pcValueTotal, INT64_LEN );
  memcpy( pcSendBuf + 1 + INT64_LEN * 7, pcValueUpper, INT64_LEN );
  memcpy( pcSendBuf + 1 + INT64_LEN * 8, pcValueUsed, INT64_LEN );
  memcpy( pcSendBuf + 1 + INT64_LEN * 9, pcValueFree, INT64_LEN );
  
  iSendLen = 1 + INT64_LEN * 10;
  
  memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
  
  send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
  send( iaSocket, pcSendBuf, iSendLen, 0 );
  
  return;
}

/**********************************************************************
��������: managerShowThreadsDeal
��������: ����˴��� show threads ����
��    ����
��    һ��SOCKET               I
��    �������ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20161022
**********************************************************************/
void managerShowThreadsDeal( int iaSocket )
{
/*
�������ݽṹ������+����
�������ݽṹ���������+�������߳�����+�������߳�����
*/
	char cResult;                                   /*ִ�н��*/
	char pcSendBuf[1 + INT64_LEN + INT64_LEN + 1];  /*���ػ�����*/
	char pcShortThreads[INT64_LEN + 1];             /*���ض������߳�����*/
	char pcLongThreads[INT64_LEN + 1];              /*���س������߳�����*/
	char pcSendLen[DATA_BUF_LEN + 1];
	int  iSendLen;                                  /*���س���*/
	int  iTmp;
	
	//MANAGER_RDLOCK();

  /*ȡ�������߳�����*/
  memset( pcShortThreads, 0x00, sizeof(pcShortThreads) );
  sprintf( pcShortThreads, "%0*d", INT64_LEN, igShortThreads );
  
  /*ȡ�������߳�����*/
  memset( pcLongThreads, 0x00, sizeof(pcLongThreads) );
  sprintf( pcLongThreads, "%0*d", INT64_LEN, igLongThreads );
  
  //MANAGER_RDUNLOCK();
  
  /*�鷢������*/
  memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
  
  sprintf( &cResult, "%d", SUCCESS );
  memcpy( pcSendBuf, &cResult, 1 );
  memcpy( pcSendBuf + 1, pcShortThreads, INT64_LEN );
  memcpy( pcSendBuf + 1 + INT64_LEN, pcLongThreads, INT64_LEN );
  
  iSendLen = 1 + INT64_LEN + INT64_LEN;
  
  memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
  
  send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
  send( iaSocket, pcSendBuf, iSendLen, 0 );
  
  /*���������־���鿴����״̬*/
  for (iTmp = 0; iTmp  < INDEPENDENT_SIZE; iTmp++)
  {
  	logFmtSyn( LVBUG, "main:id[%d]state[%d]", iTmp, igMainMutexState[iTmp] );
  }
  for (iTmp = 0; iTmp  < INDEPENDENT_FILE_SIZE; iTmp++)
  {
  	logFmtSyn( LVBUG, "     id[%d]state[%d]", iTmp, igMutexState[iTmp] );
  }
  return;
}

/**********************************************************************
��������: managerShowKeysDeal
��������: ����˴��� show keys ����
��    ����
��    һ��SOCKET               I
��    �������ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20161022
**********************************************************************/
void managerShowKeysDeal( int iaSocket )
{
/*
�������ݽṹ������+����
�������ݽṹ���������+KEY������
*/
	char cResult;                       /*ִ�н��*/
	char pcSendBuf[1 + INT64_LEN + 1];  /*���ػ�����*/
	char pcTotalKeys[INT64_LEN + 1];    /*����KEY������*/
	char pcSendLen[DATA_BUF_LEN + 1];
	int  iSendLen;                      /*���س���*/
	int  iTmp;
	__int64 llSize;
  
  llSize = 0;
  
	MANAGER_RDLOCK();
	
  for (iTmp = 0; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
  {
  	llSize += sgBase[iTmp].llKeyValueSize;
  }
  for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
  {
  	llSize += sgGlobalVariableMain[iTmp].llgKeyValueSizeMain;
  }
  
  /*ȡKEY������*/
  memset( pcTotalKeys, 0x00, sizeof(pcTotalKeys) );
  sprintf( pcTotalKeys, "%0*lld", INT64_LEN, llSize );
  
  MANAGER_RDUNLOCK();
  
  /*�鷢������*/
  memset( pcSendBuf, 0x00, sizeof(pcSendBuf) );
  
  sprintf( &cResult, "%d", SUCCESS );
  memcpy( pcSendBuf, &cResult, 1 );
  memcpy( pcSendBuf + 1, pcTotalKeys, INT64_LEN );
  
  iSendLen = 1 + INT64_LEN;
  
  memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, iSendLen );
  
  send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
  send( iaSocket, pcSendBuf, iSendLen, 0 );
  
  return;
}
