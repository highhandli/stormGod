#define __KEY_VALUE_DEFINE_H

#ifdef IGNORE_BLANK_START
#undef IGNORE_BLANK_START
#endif
/*从开始忽略空格*/
#define IGNORE_BLANK_START( start, end ) \
  {\
  	while (isBlank( *(start) ) && start < end)\
  	{\
	  	start++;\
	  }\
  }

#ifdef IGNORE_BLANK_END
#undef IGNORE_BLANK_END
#endif
#define IGNORE_BLANK_END( start, end ) \
  {\
  	while (isBlank( *(start) ) && end > start)\
  	{\
	  	end--;\
	  }\
  }

#ifdef REPLACE_VALUE_BUF_TO_VALUE
#undef REPLACE_VALUE_BUF_TO_VALUE
#endif
#define REPLACE_VALUE_BUF_TO_VALUE( keyData, value, valueLen, independentFileId ) \
{\
  freeValueBlock( keyData.llValueAddr, independentFileId );\
  keyData.llValueLen = llaValueLen;\
  keyData.llValueAddr = mallocValueBlock( valueLen, independentFileId );\
  bufToValue( keyData.llValueAddr, value, valueLen, independentFileId );\
}

#ifdef SET_VALUE_DATA
#undef SET_VALUE_DATA
#endif
/*针对一个单元的 buf to value*/
#define SET_VALUE_DATA( llaBlock, llaOffset, pcaValue, iaSize, independentFileId ) \
{\
  fseeko64( fpgValue[independentFileId], (llaBlock << VALUE_LEFT_MOVE) + llaOffset, SEEK_SET );\
  fwrite( (void *)(pcaValue), 1, iaSize, fpgValue[independentFileId] );\
}

#ifdef GET_VALUE_DATA
#undef GET_VALUE_DATA
#endif
/*针对一个单元的 value to buf*/
#define GET_VALUE_DATA( llaBlock, llaOffset, pcaValue, iaSize, independentFileId ) \
{\
  fseeko64( fpgValue[independentFileId], (llaBlock << VALUE_LEFT_MOVE) + llaOffset, SEEK_SET );\
  fread( (void *)(pcaValue), 1, iaSize, fpgValue[independentFileId] );\
}

#ifdef SET_KEY_DATA
#undef SET_KEY_DATA
#endif
#define SET_KEY_DATA( llaBlock, llaOffset, pcaKey, iaSize, independentFileId ) \
{\
  fseeko64( fpgKey[independentFileId], (llaBlock << BLOCK_LEFT_MOVE) + llaOffset, SEEK_SET );\
  fwrite( (void *)(pcaKey), 1, iaSize, fpgKey[independentFileId] );\
}

#ifdef INCREASE_FILE
#undef INCREASE_FILE
#endif
/*设置 KEY 文件大小为步增值*/
#define INCREASE_FILE( fp, size, c ) \
{\
  fseeko64( fp, size - 1, SEEK_SET );\
  c = 0x00;\
  fwrite( &c, 1, sizeof(c), fp );\
}

#ifdef SET_KEY_VALUE_DATA
#undef SET_KEY_VALUE_DATA
#endif
#define SET_KEY_VALUE_DATA( keyData, pcaKey, llaKeyLen, pcaValue, llaValueLen, independentFileId ) \
{\
  memset( &keyData, 0x00, sizeof(keyData) );\
	/*设置结构体中的关键字*/\
	keyData.llKeyLen = llaKeyLen;\
  keyData.llKeyAddr = mallocValueBlock( llaKeyLen, independentFileId );\
  if (keyData.llKeyAddr == NO_BLOCK)\
  {\
  	return ERROR;\
  }\
  \
  /*设置结构体中的值*/\
  keyData.llValueLen = llaValueLen;\
  keyData.llValueAddr = mallocValueBlock( llaValueLen, independentFileId );\
  if (keyData.llValueAddr == NO_BLOCK)\
  {\
  	return ERROR;\
  }\
  \
  /*将值写入到 VALUE 文件中*/\
  bufToValue( keyData.llKeyAddr, pcaKey, llaKeyLen, independentFileId );\
  bufToValue( keyData.llValueAddr, pcaValue, llaValueLen, independentFileId );\
}

#ifdef GET_POINT_DATA
#undef GET_POINT_DATA
#endif
#define GET_POINT_DATA( key, pcDataBuf, iStructOffset ) \
{\
  memset( &key, 0x00, sizeof(key) );\
  memcpy( &key, pcDataBuf + (iStructOffset) * sizeof(key), sizeof(key) );\
}

#ifdef GET_BLOCK_DATA
#undef GET_BLOCK_DATA
#endif
#define GET_BLOCK_DATA( pcDataBuf, llAddr, independentFileId ) \
{\
	memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );\
  getKeyBlockData( pcDataBuf, llAddr, independentFileId );\
}

#ifdef CREATE_CHILD_POINT
#undef CREATE_CHILD_POINT
#endif
#define CREATE_CHILD_POINT( pcDataBufTmp, keyRight, key, independentFileId ) \
{\
	keyRight.llChildAddr = mallocKeyBlock(independentFileId);\
  if (keyRight.llChildAddr == NO_BLOCK)\
  {\
    return ERROR;\
  }\
  keyRight.llKeyLen = key.llKeyLen;\
	keyRight.llKeyAddr = key.llKeyAddr;\
	keyRight.llChildSize = 1;\
  \
	memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );\
  memcpy( pcDataBufTmp, &key, sizeof(key) );\
  \
  /*设置数据*/\
  setKeyBlockData( pcDataBufTmp, keyRight.llChildAddr, independentFileId );\
}

#ifdef CREATE_NEW_FIRST_BLOCK
#undef CREATE_NEW_FIRST_BLOCK
#endif
#define CREATE_NEW_FIRST_BLOCK( llFirstDiscNew, pcDataBuf, key, keyMin, independentFileId ) \
{\
  /*申请索引块空间*/\
  llFirstDiscNew = mallocKeyBlock(independentFileId);\
  if (llFirstDiscNew == NO_BLOCK)\
  {\
    return ERROR;\
  }\
  \
  /*取出当前最小值*/\
  memset( &keyMin, 0x00, sizeof(keyMin) );\
  memcpy( &keyMin, pcDataBuf, sizeof(keyMin) );\
  \
  memset( &key, 0x00, sizeof(key) );\
  key.llKeyLen = keyMin.llKeyLen;\
  key.llKeyAddr = keyMin.llKeyAddr;\
  key.llChildAddr = sgBase[iaIndepentFileId].llFirstDisc;\
  key.llChildSize = sgBase[iaIndepentFileId].llFirstSize;\
  \
  /*重新设置第一级*/\
	sgBase[independentFileId].llFirstDisc = llFirstDiscNew;\
	sgBase[independentFileId].llFirstSize = 2;\
  \
	/*将总级数加1*/\
	sgBase[independentFileId].llLoops++;\
}

#ifdef WRITE_NEW_POINT
#undef WRITE_NEW_POINT
#endif
#define WRITE_NEW_POINT( pcDataBufTmp, key, keyRight, llFirstDiscNew, independentFileId ) \
{\
  memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );\
  memcpy( pcDataBufTmp, &key, sizeof(key) );\
  memcpy( pcDataBufTmp + sizeof(key), &keyRight, sizeof(keyRight) );\
  setKeyBlockData( pcDataBufTmp, llFirstDiscNew, independentFileId );\
}

#ifdef GET_MAX_KEY
#undef GET_MAX_KEY
#endif
#define GET_MAX_KEY( keyMax, pcDataBuf ) \
{\
	memset( &keyMax, 0x00, sizeof(keyMax) );\
  memcpy( &keyMax, pcDataBuf + (MAX_AERA_SIZE - 1) * sizeof(keyMax), sizeof(keyMax) );\
  memset( pcDataBuf + (MAX_AERA_SIZE - 1) * sizeof(keyMax), 0x00, sizeof(keyMax) );\
}

#ifdef SET_BROTHER_POINT
#undef SET_BROTHER_POINT
#endif
#define SET_BROTHER_POINT( pcDataBufTmpRight, pcDataBufRight, keyRight, keyData, independentFileId ) \
{\
  memset( pcDataBufTmpRight, 0x00, sizeof(pcDataBufTmpRight) );\
  memcpy( pcDataBufTmpRight, pcDataBufRight, keyRight.llChildSize * sizeof(keyData) );\
  memcpy( pcDataBufRight + sizeof(keyData), pcDataBufTmpRight, keyRight.llChildSize * sizeof(keyData) );\
  \
  /*设置兄弟下级区的结点*/\
  memcpy( pcDataBufRight, &keyData, sizeof(keyData) );\
  \
  /*设置下级区的数据*/\
  setKeyBlockData( pcDataBufRight, keyRight.llChildAddr, independentFileId );\
  \
  keyRight.llKeyLen = keyData.llKeyLen;\
	keyRight.llKeyAddr = keyData.llKeyAddr;\
  keyRight.llChildSize++;\
}

