#define __KEY_VALUE_DEFINE_MAIN_H

#ifdef REPLACE_VALUE_BUF_TO_VALUE_MAIN
#undef REPLACE_VALUE_BUF_TO_VALUE_MAIN
#endif
#define REPLACE_VALUE_BUF_TO_VALUE_MAIN( keyData, value, valueLen, indepentId ) \
{\
  freeValueBlockMain( keyData.llValueAddr, indepentId );\
  keyData.llValueLen = llaValueLen;\
  keyData.llValueAddr = mallocValueBlockMain( valueLen, indepentId );\
  if (keyData.llValueAddr == NO_BLOCK)\
  {\
  	return ERROR;\
  }\
  bufToValueMain( keyData.llValueAddr, value, valueLen, indepentId );\
}

#ifdef SET_VALUE_DATA_MAIN
#undef SET_VALUE_DATA_MAIN
#endif
/*针对一个单元的 buf to value*/
#define SET_VALUE_DATA_MAIN( llaBlock, llaOffset, pcaValue, iaSize, indepentId ) memcpy( sgGlobalVariableMain[indepentId].pcgValueBufMain + (llaBlock << VALUE_LEFT_MOVE) + llaOffset, (void *)(pcaValue), iaSize );

#ifdef GET_VALUE_DATA_MAIN
#undef GET_VALUE_DATA_MAIN
#endif
/*针对一个单元的 value to buf*/
#define GET_VALUE_DATA_MAIN( llaBlock, llaOffset, pcaValue, iaSize, indepentId ) memcpy( (void *)(pcaValue), sgGlobalVariableMain[indepentId].pcgValueBufMain + (llaBlock << VALUE_LEFT_MOVE) + llaOffset, iaSize );

#ifdef SET_KEY_DATA_MAIN
#undef SET_KEY_DATA_MAIN
#endif
#define SET_KEY_DATA_MAIN( llaBlock, llaOffset, pcaKey, iaSize, indepentId ) memcpy( sgGlobalVariableMain[indepentId].pcgKeyBufMain + (llaBlock << BLOCK_LEFT_MOVE) + llaOffset, (void *)(pcaKey), iaSize );

#ifdef INCREASE_FILE_MAIN
#undef INCREASE_FILE_MAIN
#endif

#ifdef SET_KEY_VALUE_DATA_MAIN
#undef SET_KEY_VALUE_DATA_MAIN
#endif
#define SET_KEY_VALUE_DATA_MAIN( keyData, pcaKey, llaKeyLen, pcaValue, llaValueLen, indepentId ) \
{\
  memset( &keyData, 0x00, sizeof(keyData) );\
	/*设置结构体中的关键字*/\
	keyData.llKeyLen = llaKeyLen;\
  keyData.llKeyAddr = mallocValueBlockMain( llaKeyLen, indepentId );\
  if (keyData.llKeyAddr == NO_BLOCK)\
  {\
  	return ERROR;\
  }\
  \
  /*设置结构体中的值*/\
  keyData.llValueLen = llaValueLen;\
  keyData.llValueAddr = mallocValueBlockMain( llaValueLen, indepentId );\
  if (keyData.llValueAddr == NO_BLOCK)\
  {\
  	return ERROR;\
  }\
  \
  /*将值写入到 VALUE 文件中*/\
  bufToValueMain( keyData.llKeyAddr, pcaKey, llaKeyLen, indepentId );\
  bufToValueMain( keyData.llValueAddr, pcaValue, llaValueLen, indepentId );\
}

#ifdef GET_POINT_DATA_MAIN
#undef GET_POINT_DATA_MAIN
#endif
#define GET_POINT_DATA_MAIN( key, pcDataBuf, iStructOffset ) \
{\
  memset( &key, 0x00, sizeof(key) );\
  memcpy( &key, pcDataBuf + (iStructOffset) * sizeof(key), sizeof(key) );\
}

#ifdef GET_BLOCK_DATA_MAIN
#undef GET_BLOCK_DATA_MAIN
#endif
#define GET_BLOCK_DATA_MAIN( pcDataBuf, llAddr, indepentId ) \
{\
	memset( pcDataBuf, 0x00, sizeof(pcDataBuf) );\
  getKeyBlockDataMain( pcDataBuf, llAddr, indepentId );\
}

#ifdef CREATE_CHILD_POINT_MAIN
#undef CREATE_CHILD_POINT_MAIN
#endif
#define CREATE_CHILD_POINT_MAIN( pcDataBufTmp, keyRight, key, indepentId ) \
{\
	keyRight.llChildAddr = mallocKeyBlockMain(indepentId);\
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
  setKeyBlockDataMain( pcDataBufTmp, keyRight.llChildAddr, indepentId );\
}

#ifdef CREATE_NEW_FIRST_BLOCK_MAIN
#undef CREATE_NEW_FIRST_BLOCK_MAIN
#endif
#define CREATE_NEW_FIRST_BLOCK_MAIN( llFirstDiscNew, pcDataBuf, key, keyMin, indepentId ) \
{\
  /*申请索引块空间*/\
  llFirstDiscNew = mallocKeyBlockMain(indepentId);\
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
  key.llChildAddr = sgGlobalVariableMain[indepentId].llgFirstDiscMain;\
  key.llChildSize = sgGlobalVariableMain[indepentId].llgFirstSizeMain;\
  \
  /*重新设置第一级*/\
	sgGlobalVariableMain[indepentId].llgFirstDiscMain = llFirstDiscNew;\
	sgGlobalVariableMain[indepentId].llgFirstSizeMain = 2;\
  \
	/*将总级数加1*/\
	sgGlobalVariableMain[indepentId].llgLoopsMain++;\
}

#ifdef WRITE_NEW_POINT_MAIN
#undef WRITE_NEW_POINT_MAIN
#endif
#define WRITE_NEW_POINT_MAIN( pcDataBufTmp, key, keyRight, llFirstDiscNew, indepentId ) \
{\
  memset( pcDataBufTmp, 0x00, sizeof(pcDataBufTmp) );\
  memcpy( pcDataBufTmp, &key, sizeof(key) );\
  memcpy( pcDataBufTmp + sizeof(key), &keyRight, sizeof(keyRight) );\
  setKeyBlockDataMain( pcDataBufTmp, llFirstDiscNew, indepentId );\
}

#ifdef GET_MAX_KEY_MAIN
#undef GET_MAX_KEY_MAIN
#endif
#define GET_MAX_KEY_MAIN( keyMax, pcDataBuf ) \
{\
	memset( &keyMax, 0x00, sizeof(keyMax) );\
  memcpy( &keyMax, pcDataBuf + (MAX_AERA_SIZE - 1) * sizeof(keyMax), sizeof(keyMax) );\
  memset( pcDataBuf + (MAX_AERA_SIZE - 1) * sizeof(keyMax), 0x00, sizeof(keyMax) );\
}

#ifdef SET_BROTHER_POINT_MAIN
#undef SET_BROTHER_POINT_MAIN
#endif
#define SET_BROTHER_POINT_MAIN( pcDataBufTmpRight, pcDataBufRight, keyRight, keyData, indepentId ) \
{\
  memset( pcDataBufTmpRight, 0x00, sizeof(pcDataBufTmpRight) );\
  memcpy( pcDataBufTmpRight, pcDataBufRight, keyRight.llChildSize * sizeof(keyData) );\
  memcpy( pcDataBufRight + sizeof(keyData), pcDataBufTmpRight, keyRight.llChildSize * sizeof(keyData) );\
  \
  /*设置兄弟下级区的结点*/\
  memcpy( pcDataBufRight, &keyData, sizeof(keyData) );\
  \
  /*设置下级区的数据*/\
  setKeyBlockDataMain( pcDataBufRight, keyRight.llChildAddr, indepentId );\
  \
  keyRight.llKeyLen = keyData.llKeyLen;\
	keyRight.llKeyAddr = keyData.llKeyAddr;\
  keyRight.llChildSize++;\
}

