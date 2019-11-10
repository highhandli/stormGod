#define __KEY_VALUE_THREAD_MANAGER_EXIT_FLUSH_ALTER_C

/*
������ MANAGER �е� EXIT FLUSH ALTER ���
*/

#ifndef __COMMON_HEAD_H
  #include "commonHead.h"
#endif

void managerExitDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd );
void managerFlushDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd );
void managerRamDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd );
void managerAlterThreadsDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd );
void managerAlterDiscsDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd );
void managerAlterUppersDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd );

/**********************************************************************
��������: managerExitDeal
��������: ����˴��� exit ����
��    ����
��    һ��SOCKET               I
��    �������ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20161022
**********************************************************************/
void managerExitDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd )
{
/*
�������ݽṹ������+����+�û���+����
�������ݽṹ���������
*/
	#define MANAGER_EXIT_DEAL_SEND_RESULT( value ) \
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
	char pcUser[NAME_LEN + 1];
	char pcPass[NAME_LEN + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	int  iTmp;
	int  iRet;
	char *pcBufStart;
	char *pcBufEnd;
	
	logFmtSyn( LVNOR, "�յ�����˵��˳�����" );
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
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
	
	/*����û����������Ƿ�ƥ��*/
	if (strcmp( pcgUser, pcUser ) || strcmp( pcgPass, pcPass ))
	{
		logFmtSyn( LVNOR, "�û���[%s]����[%s]��ƥ��", pcUser, pcPass );
		MANAGER_EXIT_DEAL_SEND_RESULT( ERROR );
		MANAGER_WRUNLOCK();
		return;
	}
	
	/*�Ȱѽ������ȥ*/
	sprintf( &cResult, "%d", SUCCESS );
	
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, 1 );
  
  send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
  send( iaSocket, &cResult, 1, 0 );
	
	/*�ͷ��ļ����ڴ���Դ*/
	//keyValueRelease( );
  igExitFlag = SERVER_EXIT;
	//exit(0);
  return;
}

/**********************************************************************
��������: managerFlushDeal
��������: ����˴��� fflush ����
��    ����
��    һ��SOCKET               I
��    �������ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20161022
**********************************************************************/
void managerFlushDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd )
{
/*
�������ݽṹ������+����+�û���+����
�������ݽṹ���������
*/
	
	#define MANAGER_FFLUSH_DEAL_SEND_ERROR( value ) \
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
	char pcUser[NAME_LEN + 1];
	char pcPass[NAME_LEN + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	char *pcBufStart;
	char *pcBufEnd;
	char *pcTmp;
	int iTmp;
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
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
  
 
  
  /*���ļ�������������д��Ӳ��*/
  for (iTmp = 0; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
	{
    rewind( fpgKey[iTmp] );
	  fwrite( &(sgBase[iTmp]), 1, sizeof(KEY_VALUE_BASE_INFO), fpgKey[iTmp] );
	  
	  fflush( fpgKey[iTmp] );
	  fflush( fpgValue[iTmp] );
  }
  
  /*���ڴ��е�����д�뵽Ӳ��*/
  rewind( fpgMappingKey );
  rewind( fpgMappingValue );
	fwrite( &sgGlobalVariableMain, 1, sizeof(sgGlobalVariableMain), fpgMappingKey );

  /*д���ļ�����*/
  for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
	{
		//logFmtSyn( LVNOR, "%d:%d", sgGlobalVariableMain[iTmp].llgKeyRamOffsetMain, sgGlobalVariableMain[iTmp].llgValueRamOffsetMain );
		fwrite( sgGlobalVariableMain[iTmp].pcgKeyBufMain, 1, sgGlobalVariableMain[iTmp].llgKeyRamOffsetMain, fpgMappingKey );
    fwrite( sgGlobalVariableMain[iTmp].pcgValueBufMain, 1, sgGlobalVariableMain[iTmp].llgValueRamOffsetMain, fpgMappingValue );
  }
  fflush( fpgMappingKey );
  fflush( fpgMappingValue );

	MANAGER_WRUNLOCK();
	
	sprintf( &cResult, "%d", SUCCESS );
	
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, 1 );
  
  send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
  send( iaSocket, &cResult, 1, 0 );
  return;
}

/**********************************************************************
��������: managerRamDeal
��������: ����˴��� ram to disc ����
��    ����
��    һ��SOCKET               I
��    �������ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20170217
**********************************************************************/
void managerRamDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd )
{
/*
�������ݽṹ������+����+�û���+����
�������ݽṹ���������
*/
	
	#define MANAGER_RAM_DEAL_SEND_ERROR( value ) \
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
	char pcUser[NAME_LEN + 1];
	char pcPass[NAME_LEN + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	char *pcBufStart;
	char *pcBufEnd;
	char *pcTmp;
	int iTmp;
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
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

	/*���ڴ��е���������д�뵽Ӳ����*/
	for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
	{
    releaseBlockMainAll(iTmp);
  }
  
  #if 1
  /*���ڴ��е�����д�뵽Ӳ��*/
  rewind( fpgMappingKey );
  rewind( fpgMappingValue );
	fwrite( &sgGlobalVariableMain, 1, sizeof(sgGlobalVariableMain), fpgMappingKey );
  for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
	{
		//logFmtSyn( LVNOR, "%d", iTmp );
    fwrite( sgGlobalVariableMain[iTmp].pcgKeyBufMain, 1, sgGlobalVariableMain[iTmp].llgKeyRamOffsetMain, fpgMappingKey );
    fwrite( sgGlobalVariableMain[iTmp].pcgValueBufMain, 1, sgGlobalVariableMain[iTmp].llgValueRamOffsetMain, fpgMappingValue );
  }
  fflush( fpgMappingKey );
  fflush( fpgMappingValue );
  #endif
  
  #if 1
  for (iTmp = 0; iTmp < INDEPENDENT_FILE_SIZE; iTmp++)
	{
    rewind( fpgKey[iTmp] );
	  fwrite( &(sgBase[iTmp]), 1, sizeof(KEY_VALUE_BASE_INFO), fpgKey[iTmp] );
	  
	  fflush( fpgKey[iTmp] );
	  fflush( fpgValue[iTmp] );
  }
	#endif
	
	MANAGER_WRUNLOCK();
	
	sprintf( &cResult, "%d", SUCCESS );
	
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, 1 );
  
  send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
  send( iaSocket, &cResult, 1, 0 );
  return;
}

/**********************************************************************
��������: managerAlterDiscsDeal
��������: ����˴��� alter discs ����
��    ����
��    һ��SOCKET               I
��    �������ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20161022
**********************************************************************/
void managerAlterDiscsDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd )
{
/*
�������ݽṹ������+����+�ļ����ֵ+�û���+����
�������ݽṹ���������
*/
	#define MANAGER_ALTER_DISCS_DEAL_SEND_ERROR( value ) \
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
	char pcMax[INT64_LEN + 1];
	char pcUser[NAME_LEN + 1];
	char pcPass[NAME_LEN + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	__int64 llFileMaxSize;
	char *pcBufStart;
	char *pcBufEnd;
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
	/*ȡ�ļ����ֵ*/
	memset( pcMax, 0x00, sizeof(pcMax) );
	memcpy( pcMax, pcBufStart, INT64_LEN );
	pcBufStart += INT64_LEN;
	llFileMaxSize = (atoll( pcMax )) << 20;
	
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
	
	/*����û����������Ƿ�ƥ��*/
	if (strcmp( pcgUser, pcUser ) || strcmp( pcgPass, pcPass ))
	{
		logFmtSyn( LVNOR, "�û���[%s]����[%s]��ƥ��", pcUser, pcPass );
		MANAGER_ALTER_DISCS_DEAL_SEND_ERROR( ERROR );
		MANAGER_WRUNLOCK();
		return;
	}
	
	#if 0
	if (llFileMaxSize > llgFileMaxSize)
  {
  	logFmtSyn( LVNOR, "�ļ���С[%lld]���ܳ������ֵ[%lld]", llFileMaxSize, llgFileMaxSize );
  	MANAGER_ALTER_DISCS_DEAL_SEND_ERROR( ERROR );
		MANAGER_WRUNLOCK();
		return;
  }
  #endif
	
	llgFileMaxSize = llFileMaxSize;
	
	MANAGER_WRUNLOCK();
	
	sprintf( &cResult, "%d", SUCCESS );
	
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, 1 );
  
  send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
  send( iaSocket, &cResult, 1, 0 );
  return;
}

/**********************************************************************
��������: managerAlterUppersDeal
��������: ����˴��� alter uppers ����
��    ����
��    һ��SOCKET               I
��    �������ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20161022
**********************************************************************/
void managerAlterUppersDeal( int iaSocket, char *pcaBufStart, char *pcaBufEnd )
{
/*
�������ݽṹ������+����+�ļ�����ֵ+�û���+����
�������ݽṹ���������
*/
	#define MANAGER_ALTER_UPPERS_DEAL_SEND_ERROR( value ) \
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
	char pcUppers[INT64_LEN + 1];
	char pcUser[NAME_LEN + 1];
	char pcPass[NAME_LEN + 1];
	char pcSendLen[DATA_BUF_LEN + 1];
	__int64 llFileUpperSize;
	char *pcBufStart;
	char *pcBufEnd;
	
	pcBufStart = pcaBufStart;
	pcBufEnd = pcaBufEnd;
	
	/*ȡ����ֵ*/
	memset( pcUppers, 0x00, sizeof(pcUppers) );
	memcpy( pcUppers, pcBufStart, INT64_LEN );
	pcBufStart += INT64_LEN;
	
	llFileUpperSize = (atoll( pcUppers )) << 20;
	
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
	
	/*����û����������Ƿ�ƥ��*/
	if (strcmp( pcgUser, pcUser ) || strcmp( pcgPass, pcPass ))
	{
		logFmtSyn( LVNOR, "�û���[%s]����[%s]��ƥ��", pcUser, pcPass );
		MANAGER_ALTER_UPPERS_DEAL_SEND_ERROR( ERROR );
		MANAGER_WRUNLOCK();
		return;
	}
		
	if (llFileUpperSize > llgFileMaxSize)
  {
  	logFmtSyn( LVNOR, "�ļ�������С[%lld]���ܳ������ֵ[%lld]", llFileUpperSize, llgFileMaxSize );
  	MANAGER_ALTER_UPPERS_DEAL_SEND_ERROR( ERROR );
		MANAGER_WRUNLOCK();
		return;
  }
  
  llgFileUpperSize = llFileUpperSize;
	
	MANAGER_WRUNLOCK();
	
	sprintf( &cResult, "%d", SUCCESS );
	
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, 1 );
  
  send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
  send( iaSocket, &cResult, 1, 0 );
  return;
}

/**********************************************************************
��������: managerAlterDiscsDeal
��������: ����˴��� alter discs ����
��    ����
��    һ��SOCKET               I
��    �������ݿ�ʼ��ַ         I
��    �������ݽ�����ַ         I
��    ��: �����
����ʱ��: 20161022
**********************************************************************/
void managerCheckDeal( int iaSocket )
{
/*
�������ݽṹ��
�������ݽṹ���������
*/
	#define MANAGER_ALTER_DISCS_DEAL_SEND_ERROR( value ) \
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
	char pcSendLen[DATA_BUF_LEN + 1];
	int  iRet;
	int  iTmp;
	int  iTmp2;
	char pcBlock[BLOCK_SIZE];
	__int64 llBlock;
	__int64 llKeyFreeOffsetMain;
	__int64 llValueFreeOffsetMain;
	
	
	MANAGER_WRLOCK();
	
	#if 0
	/*������ sgGlobalVariableMain */
	{
		/*��ӡ llgKeyRamOffsetMain �� pcgKeyFreeMain*/
		for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
		{
			memcpy( pcBlock, sgGlobalVariableMain[iTmp].pcgKeyFreeMain, BLOCK_SIZE );
			llKeyFreeOffsetMain = sgGlobalVariableMain[iTmp].llgKeyFreeOffsetMain;
			while (1)
			{
			  if (llKeyFreeOffsetMain == 0)
		    {
		    	/*ָ����п�����λ��*/
		      llBlock = *((__int64*)( pcBlock + BLOCK_SIZE - sizeof(__int64)));
		    
	      	/*�����*/
	      	if (llBlock > 0)
	      	{
		    	  getKeyBlockDataMain( pcBlock, llBlock, iTmp );
		    	  llKeyFreeOffsetMain = BLOCK_SIZE - sizeof(__int64);
		    	  logFmtSyn( LVSYS, "%lld", llBlock );
		      }
		      else
		      {
		      	break;
		      }
		    }
		    else
	      if (llKeyFreeOffsetMain > 0)
	      {
	      	llKeyFreeOffsetMain -= sizeof(__int64);
	      	llBlock = *((__int64*)(pcBlock + llKeyFreeOffsetMain));

		      logFmtSyn( LVSYS, "%lld", llBlock );
		    }
	    }
	    
	    logFmtSyn( LVSYS, "%lld:%lld:%lld:%lld", sgGlobalVariableMain[iTmp].llgKeyValueSizeMain, sgGlobalVariableMain[iTmp].llgFirstDiscMain, sgGlobalVariableMain[iTmp].llgFirstSizeMain, sgGlobalVariableMain[iTmp].llgLoopsMain );
		}
		
		/*��ӡ llgValueRamOffsetMain �� pcgKeyFreeMain*/
		for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
		{
			memcpy( pcBlock, sgGlobalVariableMain[iTmp].pcgValueFreeMain, BLOCK_SIZE );
			llValueFreeOffsetMain = sgGlobalVariableMain[iTmp].llgValueFreeOffsetMain;
			while (1)
			{
			  if (llValueFreeOffsetMain > 0)
		    {
		    	llBlock = *((__int64*)(pcBlock+ llValueFreeOffsetMain - sizeof(__int64)));
		    	llValueFreeOffsetMain -= sizeof(__int64);
		    	
		    	logFmtSyn( LVERR, "%lld", llBlock );
		    	
		    	if (llValueFreeOffsetMain == 0)
		    	{
		    		llBlock = *((__int64*)(pcBlock + BLOCK_SIZE - sizeof(__int64)));
		    		if (llBlock > 0)
		    		{
		    			getKeyBlockDataMain( pcBlock, llBlock, iTmp );
		    	  	llValueFreeOffsetMain = BLOCK_SIZE - sizeof(__int64);
		    		}
		    		else
		    		{
		    			break;
		    		}
		    	}
		    }
		    else
		    {
		    	break;
		    }
	    }
		}
	}
	#else
	for (iTmp = 0; iTmp < INDEPENDENT_SIZE; iTmp++)
  {
  	logFmtSyn( LVWAR, "[%lld][%lld][%lld][%lld][%lld][%lld][%lld][%lld][%lld][%lld][%lld]", 
  	  iTmp,                                              /*1*/
  	  sgGlobalVariableMain[iTmp].llgKeyRamOffsetMain,    /*2*/
  	  sgGlobalVariableMain[iTmp].llgValueRamOffsetMain,  /*3*/
  	  sgGlobalVariableMain[iTmp].llgKeyRamUsedMain,      /*4*/
  	  sgGlobalVariableMain[iTmp].llgValueRamUsedMain,    /*5*/
  	  sgGlobalVariableMain[iTmp].llgKeyFreeOffsetMain,   /*6*/
  	  sgGlobalVariableMain[iTmp].llgValueFreeOffsetMain, /*7*/
  	  sgGlobalVariableMain[iTmp].llgKeyValueSizeMain,    /*8*/
  	  sgGlobalVariableMain[iTmp].llgFirstDiscMain,       /*9*/
  	  sgGlobalVariableMain[iTmp].llgFirstSizeMain,       /*10*/
  	  sgGlobalVariableMain[iTmp].llgLoopsMain );         /*11*/
	  //memset( sgGlobalVariableMain[iTmp].pcgKeyBufMain, 0x00, sgGlobalVariableMain[iTmp].llgKeyRamOffsetMain );
    //memset( sgGlobalVariableMain[iTmp].pcgValueBufMain, 0x00, sgGlobalVariableMain[iTmp].llgValueRamOffsetMain );
    //logFmtSyn( LVNOR, "%d", llTmp );
    //sgGlobalVariableMain[iTmp].llgKeyRamOffsetMain = 0;                           /* KEY �ļ���������ʹ��ƫ����*/
    //sgGlobalVariableMain[iTmp].llgValueRamOffsetMain = 0;                         /* VALUE �ļ���������ʹ��ƫ����*/
    //sgGlobalVariableMain[iTmp].llgKeyRamUsedMain = 0;                             /* KEY �ļ���������ʹ�����������ﻹ�����ͷŹ���*/
    //sgGlobalVariableMain[iTmp].llgValueRamUsedMain = 0;                           /* VALUE �ļ���������ʹ�����������ﻹ�����ͷŹ���*/
    //sgGlobalVariableMain[iTmp].llgKeyFreeOffsetMain = 0;                          /*��ǰ�ļ��е� KEY �ļ�������ʹ��ƫ��������λ��B*/
    //sgGlobalVariableMain[iTmp].llgValueFreeOffsetMain = 0;                        /*��ǰ�ļ��е� VALUE �ļ�������ʹ��ƫ��������λ��B*/
    //sgGlobalVariableMain[iTmp].llgKeyValueSizeMain = 0;                           /* KEY ��������*/
    //sgGlobalVariableMain[iTmp].llgFirstDiscMain = 0;                              /* KEY �ļ��е� KEY ��һ�����ݿ��ַ����λ��BLOCK_SIZE*/
    //sgGlobalVariableMain[iTmp].llgFirstSizeMain = 0;                              /* KEY �ļ��е� KEY ��һ����������*/
    //sgGlobalVariableMain[iTmp].llgLoopsMain = 0;                                  /* KEY �ļ��е� KEY �ĵ�ǰ����*/
    //memset( sgGlobalVariableMain[iTmp].pcgKeyFreeMain, 0x00, BLOCK_SIZE );        /* KEY �ļ����м�¼��*/
    //memset( sgGlobalVariableMain[iTmp].pcgValueFreeMain, 0x00, BLOCK_SIZE );      /* VALUE �ļ����м�¼��*/
  }
	#endif
	MANAGER_WRUNLOCK();
	
	sprintf( &cResult, "%d", SUCCESS );
	
	memset( pcSendLen, 0x00, sizeof(pcSendLen) );
  sprintf( pcSendLen, "%0*d", DATA_BUF_LEN, 1 );
  
  send( iaSocket, pcSendLen, DATA_BUF_LEN, 0 );
  send( iaSocket, &cResult, 1, 0 );
  return;
}
