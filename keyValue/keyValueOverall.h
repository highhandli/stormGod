#define __KEY_VALUE_OVERALL_H

#ifndef __KEY_VALUE_VAR_H
  #include "keyValueVar.h"
#endif

#ifndef __OS_H
  #include "os.h"
#endif

/**************************初始化时的配置文件数据*******************************/
char pcgUser[NAME_LEN];                 /*配置信息中的 USER */
char pcgPass[NAME_LEN];                 /*配置信息中的 PASS */
char pcgManagerListen[PORT_LEN];        /*配置信息中的 MANAGER_LISTEN */
char pcgShortListen[PORT_LEN];          /*配置信息中的 SHORT_LISTEN */
char pcgLongListen[PORT_LEN];           /*配置信息中的 LONG_LISTEN */
char pcgLogPath[PATH_LEN];              /*配置信息中的 LOGPATH */
char pcgLogName[FILE_LEN];              /*配置信息中的 LOGNAME */
char pcgKeyRamSize[INT_LEN];            /*配置信息中的 KEY_RAM_SIZE */
char pcgValueRamSize[INT_LEN];          /*配置信息中的 VALUE_RAM_SIZE */
char pcgKeyFilePath[PATH_LEN];          /*配置信息中的 KEY_FILE_PATH */
char pcgKeyFileName[PATH_LEN];          /*配置信息中的 KEY_FILE_NAME */
char pcgValueFilePath[PATH_LEN];        /*配置信息中的 VALUE_FILE_PATH */
char pcgValueFileName[PATH_LEN];        /*配置信息中的 VALUE_FILE_NAME */
char pcgKeyMappingPath[PATH_LEN];       /*配置信息中的 KEY_MAPPING_PATH */
char pcgKeyMappingName[PATH_LEN];       /*配置信息中的 KEY_MAPPING_NAME */
char pcgValueMappingPath[PATH_LEN];     /*配置信息中的 VALUE_MAPPING_PATH */
char pcgValueMappingName[PATH_LEN];     /*配置信息中的 VALUE_MAPPING_NAME */
char pcgFileUpperSize[INT_LEN];         /*配置信息中的 FILE_UPPER_SIZE */
char pcgFileMaxSize[INT_LEN];           /*配置信息中的 FILE_MAX_SIZE */
char pcgSelectTempPath[PATH_LEN];       /*配置信息中的 SELECT_TEMP_PATH */

int  igManagerListen = 0;               /*配置信息中的 MANAGER_LISTEN 的整数值*/
int  igShortListen = 0;                 /*配置信息中的 SHORT_LISTEN 的整数值*/
int  igLongListen = 0;                  /*配置信息中的 LONG_LISTEN 的整数值*/
__int64 llgKeyRamSizeMain = 0;          /*配置信息中的 KEY_RAM_SIZE 的整数值*/
__int64 llgValueRamSizeMain = 0;        /*配置信息中的 VALUE_RAM_SIZE 的整数值*/
__int64 llgFileUpperSize = 0;           /*配置信息中的 FILE_UPPER_SIZE 的整数值*/
__int64 llgFileMaxSize = 0;             /*配置信息中的 FILE_MAX_SIZE 的整数值*/

int  igShortSocket;                     /*短连接侦听SOCKET*/
int  igLongSocket;                      /*长连接侦听SOCKET*/

FILE *fpgKey[INDEPENDENT_FILE_SIZE];                    /* KEY 文件指针 */
FILE *fpgValue[INDEPENDENT_FILE_SIZE];                  /* VALUE 文件指针 */
__int64 llgKeyFileSize[INDEPENDENT_FILE_SIZE];          /* KEY 文件的大小，单位：B*/
__int64 llgKeyFileDisc[INDEPENDENT_FILE_SIZE];          /* KEY 文件的大小，单位：BLOCK_SIZE*/
__int64 llgValueFileSize[INDEPENDENT_FILE_SIZE];        /* VALUE 文件的大小，单位：B*/

__int64 llgKeyFileOffset[INDEPENDENT_FILE_SIZE];        /*当前 KEY 文件使用偏移量，单位：BLOCK_SIZE*/
__int64 llgValueFileOffset[INDEPENDENT_FILE_SIZE];      /*当前 VALUE 文件使用偏移量，单位：B*/
char pcgKeyFree[INDEPENDENT_FILE_SIZE][BLOCK_SIZE];     /* KEY 文件空闲记录块*/
char pcgValueFree[INDEPENDENT_FILE_SIZE][BLOCK_SIZE];   /* VALUE 文件空闲记录块*/

KEY_VALUE_BASE_INFO sgBase[INDEPENDENT_FILE_SIZE];      /* KEY 文件中的基础信息*/

typedef struct keyValueMainGlobalVariable
{
  char *pcgKeyBufMain;                /* KEY 文件缓冲区*/
  char *pcgValueBufMain;              /* VALUE 文件缓冲区*/
  
  __int64 llgKeyRamSizeMain;          /* KEY 文件缓冲区大小*/
  __int64 llgValueRamSizeMain;        /* VALUE 文件缓冲区大小*/

  __int64 llgKeyRamOffsetMain;        /* KEY 文件缓冲区的使用偏移量*/
  __int64 llgValueRamOffsetMain;      /* VALUE 文件缓冲区的使用偏移量*/
  
  __int64 llgKeyRamUsedMain;          /* KEY 文件缓冲区的使用数量，这里还包括释放过的*/
  __int64 llgValueRamUsedMain;        /* VALUE 文件缓冲区的使用数量，这里还包括释放过的*/
  
  __int64 llgKeyFreeOffsetMain;       /*当前文件中的 KEY 文件空闲区使用偏移量，单位：B*/
  __int64 llgValueFreeOffsetMain;     /*当前文件中的 VALUE 文件空闲区使用偏移量，单位：B*/
  __int64 llgKeyValueSizeMain;        /* KEY 的总数量*/
  __int64 llgFirstDiscMain;           /* KEY 文件中的 KEY 的一级数据块地址，单位：BLOCK_SIZE*/
  __int64 llgFirstSizeMain;           /* KEY 文件中的 KEY 的一级数据数量*/
  __int64 llgLoopsMain;               /* KEY 文件中的 KEY 的当前级数*/
  
  char pcgKeyFreeMain[BLOCK_SIZE];    /* KEY 文件空闲记录块*/
  char pcgValueFreeMain[BLOCK_SIZE];  /* VALUE 文件空闲记录块*/
  
  //char *pcgKeyFreeMain;    /* KEY 文件空闲记录块*/
  //char *pcgValueFreeMain;  /* VALUE 文件空闲记录块*/
}GLOBAL_VARIABLE;

GLOBAL_VARIABLE sgGlobalVariableMain[INDEPENDENT_SIZE];

FILE *fpgMappingKey;                    /* KEY 缓冲区映射文件指针 */
FILE *fpgMappingValue;                  /* KEY 缓冲区映射文件指针 */

int igConnectionErrFlag = SUCCESS;      /*短连接和长连接线程的运行状态*/

THREAD_RESOURCE sgThreadControl;          /*控制线程数量*/
THREAD_RESOURCE sgManagerThread;          /*管理线程资源*/
THREAD_RESOURCE sgShortThreadArray[MAX_THREADS]; /*短连接线程资源*/
THREAD_RESOURCE sgLongThreadArray[MAX_THREADS];  /*长连接线程资源*/

__int64 llgSelectId;            /*查询ID*/
int igShortThreads;             /*当前的短连接线程数量*/
int igLongThreads;              /*当前的长连接线程数量*/
int igExitFlag;                 /*程序的退出标识*/ 

#ifdef NOT_USED
  #undef NOT_USED
#endif
#define NOT_USED      0         /*未有使用*/

#ifdef USED
  #undef USED
#endif
#define USED          1         /*正在使用*/

#ifdef DEL_USED
  #undef DEL_USED
#endif
#define DEL_USED      2         /*正在使用*/

#define SERVER_EXIT   1
#define SERVER_RUN    2 

pthread_mutex_t mutexMain[INDEPENDENT_SIZE];
pthread_mutex_t mutexFile[INDEPENDENT_FILE_SIZE];

#define LOCK_STATE   1
#define UNLOCK_STATE 2
int igMainMutexState[INDEPENDENT_SIZE];
int igMutexState[INDEPENDENT_FILE_SIZE];

/*多线程读写硬盘，只能单线程使用*/
#define UPDATE_DELETE_LOCK(id)      pthread_mutex_lock(&(mutexFile[id]));igMutexState[id]=LOCK_STATE;
#define UPDATE_DELETE_UNLOCK(id)    igMutexState[id]=LOCK_STATE;pthread_mutex_unlock(&(mutexFile[id]));

#define SELECT_LOCK(id)      pthread_mutex_lock(&(mutexFile[id]));igMutexState[id]=LOCK_STATE;
#define SELECT_UNLOCK(id)    igMutexState[id]=LOCK_STATE;pthread_mutex_unlock(&(mutexFile[id]));

int igManagerTmp;
#define MANAGER_WRLOCK()   for(igManagerTmp=0;igManagerTmp<INDEPENDENT_SIZE;igManagerTmp++){pthread_mutex_lock(&(mutexMain[igManagerTmp]));}for(igManagerTmp=0;igManagerTmp<INDEPENDENT_FILE_SIZE;igManagerTmp++){pthread_mutex_lock(&(mutexFile[igManagerTmp]));}
#define MANAGER_WRUNLOCK() for(igManagerTmp=0;igManagerTmp<INDEPENDENT_SIZE;igManagerTmp++){pthread_mutex_unlock(&(mutexMain[igManagerTmp]));}for(igManagerTmp=0;igManagerTmp<INDEPENDENT_FILE_SIZE;igManagerTmp++){pthread_mutex_unlock(&(mutexFile[igManagerTmp]));}

#define MANAGER_RDLOCK()   for(igManagerTmp=0;igManagerTmp<INDEPENDENT_SIZE;igManagerTmp++){pthread_mutex_lock(&(mutexMain[igManagerTmp]));}for(igManagerTmp=0;igManagerTmp<INDEPENDENT_FILE_SIZE;igManagerTmp++){pthread_mutex_lock(&(mutexFile[igManagerTmp]));}
#define MANAGER_RDUNLOCK() for(igManagerTmp=0;igManagerTmp<INDEPENDENT_SIZE;igManagerTmp++){pthread_mutex_unlock(&(mutexMain[igManagerTmp]));}for(igManagerTmp=0;igManagerTmp<INDEPENDENT_FILE_SIZE;igManagerTmp++){pthread_mutex_unlock(&(mutexFile[igManagerTmp]));}

/*多线程读写内存，可以多线程执行*/
#define UPDATE_DELETE_MAIN_LOCK(id)    pthread_mutex_lock(&(mutexMain[id]));igMainMutexState[id]=LOCK_STATE;
#define UPDATE_DELETE_MAIN_UNLOCK(id)  igMainMutexState[id]=UNLOCK_STATE;pthread_mutex_unlock(&(mutexMain[id]));

#define SELECT_MAIN_LOCK(id)           pthread_mutex_lock(&(mutexMain[id]));igMainMutexState[id]=LOCK_STATE;
#define SELECT_MAIN_UNLOCK(id)         igMainMutexState[id]=UNLOCK_STATE;pthread_mutex_unlock(&(mutexMain[id]));


pthread_mutex_t srMutex;          /*互斥锁*/
SELECT_RESULT_BUF *sgResultBuf = NULL;
__int64 llgResultBufOffset = 0;

#define MANAGER_SELECT_LOCK()   pthread_mutex_lock(&srMutex)
#define MANAGER_SELECT_UNLOCK() pthread_mutex_unlock(&srMutex);
