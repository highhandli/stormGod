#define __KEY_VALUE_VAR_H

#ifndef __OS_H
  #include "os.h"
#endif

#define MAX_KEY_LEN       512                    /* KEY ����󳤶�*/
#define MAX_VALUE_LEN     4096                   /* VALUE ����󳤶�*/
                                                 
#define BLOCK_SIZE        1024                   /*��Ĵ�С*/
#define BLOCK_LEFT_MOVE   10                     /*1024 = 2^10*/
#define MAX_AERA_SIZE     32                     /*1024=32*32*/

#define DOUBLE_INT_SIZE   (sizeof(__int64) + sizeof(__int64))
//#define BLOCK_SIZE        512                  /*��Ĵ�С*/
//#define BLOCK_LEFT_MOVE   9                    /*4096 = 2^12*/
//#define MAX_AERA_SIZE     32                   /*4096=32*128*/
                                                 
#define MAX_LINE_BUF      4096                   /*�����ļ���ÿһ�е��������*/
#define PORT_LEN          10                     /*�����˿ڵ���󳤶�*/
#define PATH_LEN          512                    /*·����󳤶�*/
#define FILE_LEN          256                    /*�ļ�������󳤶�*/
#define NAME_LEN          64                     /*���ֵ���󳤶�*/
#define INT_LEN           10                     /*�������ݵ��ַ�������*/
#define INT64_LEN         20                     /*64λ�������ݵ��ַ�������*/
#define MAX_LINKS         1000                   /*����ʱ�����������*/
#define DATE_LEN          8                      /*���ڵĳ���*/
#define TIME_LEN          6                      /*ʱ��ĳ���*/
#define DATE_TIME         16                     /*����ʱ��ĳ���*/
#define MIN_THREADS          10                  /*�߳���С����*/
#ifdef SYSTEM_WINDOWS_64
  #define MAX_THREADS        1000                /*�߳�������*/
#else
  #define MAX_THREADS        100000              /*�߳�������*/
#endif
#define NULL_SELECT_ID    "00000000000000000000" /*�յ�SELECT ID������ΪINT64_LEN��*/

/************�����������VALUE�ķ���*************/
//#define MIN_RAM_UNIT      56                   /*ÿ�η����ڴ����С��λ*//*ԭ��������64������56+8=64*/
//#define VALUE_LEFT_MOVE   6                    /*64 = 2^6*/
#define MIN_RAM_UNIT      24                     /*ÿ�η����ڴ����С��λ*//*ԭ��������32������24+8=32*/
#define VALUE_LEFT_MOVE   5                      /*32 = 2^5*/
#define ADDRESS_SIZE      sizeof(void *)         /*�ڴ��ַ��С*/
#define MIN_UNIT          (MIN_RAM_UNIT + ADDRESS_SIZE)/*ÿһ������Ĵ�С*/

#define INDEPENDENT_SIZE       1031                /*������Դ����*//*�������������������Ƚ�ƽ��*/
#define INDEPENDENT_FILE_SIZE  7                 /*�����ļ�����*//*�������������������Ƚ�ƽ��*//*����31*/
#define INDEPENDENT_FILE_LEFT_MOVE   3           /*2^INDEPENDENT_FILE_LEFT_MOVE > INDEPENDENT_FILE_SIZE�����ֵȡ��ӽ���*/

#define DEFAULT_USER             "root"          /*���������Ϣ��û�� USER ���������Ĭ��ֵ*/
#define DEFAULT_PASS             "root"          /*���������Ϣ��û�� PASS ���������Ĭ��ֵ*/
#define DEFAULT_MANAGER_LISTEN   "10002"         /*���������Ϣ��û�� MANAGER_LISTEN ���������Ĭ��ֵ*/
#define DEFAULT_SHORT_LISTEN     "10000"         /*���������Ϣ��û�� SHORT_LISTEN ���������Ĭ��ֵ*/
#define DEFAULT_LONG_LISTEN      "10001"         /*���������Ϣ��û�� LONG_LISTEN ���������Ĭ��ֵ*/
#define DEFAULT_LOGPATH          ""              /*���������Ϣ��û�� LOGPATH ���������Ĭ��ֵ*/
#define DEFAULT_LOGNAME          "keyValue"      /*���������Ϣ��û�� LOGNAME ���������Ĭ��ֵ*/
#define DEFAULT_KEY_RAM_SIZE     "1024"            /*���������Ϣ��û�� KEY_RAM_SIZE ���������Ĭ��ֵ*/
#define DEFAULT_VALUE_RAM_SIZE   "1024"            /*���������Ϣ��û�� VALUE_RAM_SIZE ���������Ĭ��ֵ*/
#define DEFAULT_KEY_FILE_PATH    ""              /*���������Ϣ��û�� KEY_FILE_PATH ���������Ĭ��ֵ*/
#define DEFAULT_KEY_FILE_NAME    "keyValue.key"  /*���������Ϣ��û�� KEY_FILE_NAME ���������Ĭ��ֵ*/
#define DEFAULT_VALUE_FILE_PATH  ""              /*���������Ϣ��û�� VALUE_FILE_PATH ���������Ĭ��ֵ*/
#define DEFAULT_VALUE_FILE_NAME  "keyValue.val"  /*���������Ϣ��û�� VALUE_FILE_NAME ���������Ĭ��ֵ*/
#define DEFAULT_KEY_MAPPING_PATH    ""                 /*���������Ϣ��û�� KEY_MAPPING_PATH ���������Ĭ��ֵ*/
#define DEFAULT_KEY_MAPPING_NAME    "keyValue.mapKey"  /*���������Ϣ��û�� KEY_MAPPING_NAME ���������Ĭ��ֵ*/
#define DEFAULT_VALUE_MAPPING_PATH  ""                 /*���������Ϣ��û�� VALUE_MAPPING_PATH ���������Ĭ��ֵ*/
#define DEFAULT_VALUE_MAPPING_NAME  "keyValue.mapVal"  /*���������Ϣ��û�� VALUE_MAPPING_NAME ���������Ĭ��ֵ*/
#define DEFAULT_FILE_UPPER_SIZE  "16"            /*���������Ϣ��û�� FILE_UPPER_SIZE ���������Ĭ��ֵ*/
#define DEFAULT_FILE_MAX_SIZE    "16"            /*���������Ϣ��û�� FILE_MAX_SIZE ���������Ĭ��ֵ*/
#define DEFAULT_SELECT_TEMP_PATH ""              /*���������Ϣ��û�� SELECT_TEMP_PATH ���������Ĭ��ֵ*/

#define BUSY_THREAD_PERCENT      80              /*������ʹ�õ��̴߳ﵽ����ٷֱȣ��Ͱѱ�����������ˮƽ*/
#define NORMAL_THREAD_PERCENT    50              /*�������߳�ʹ�ñ���*/
#define FREE_THREAD_PERCENT      20              /*������ʹ�õ��̴߳ﵽ����ٷֱȣ��Ͱѱ�����������ˮƽ*/


#ifdef SUCCESS
  #undef SUCCESS
#endif

#ifdef NOT_FOUND
  #undef NOT_FOUND
#endif

#ifdef ERROR
  #undef ERROR
#endif

#ifdef NO_BLOCK
  #undef NO_BLOCK
#endif

#ifdef INTERRUPT
  #undef INTERRUPT
#endif

#define  SUCCESS              0                  /*�ɹ�*/
#define  NOT_FOUND            1                  /*������*/
#define  ERROR                2                  /*����*/
#define  NO_BLOCK            -1                  /*������*/
#define  INTERRUPT            3                  /*���ж�*/
                              
#define  REPLACE_0            0                  /*�ؼ�ֵ�䡢������*/
#define  REPLACE_1            1                  /*�ؼ�ֵ�䡢��������*/
#define  REPLACE_2            2                  /*�ؼ�ֵ���䡢������*/
#define  REPLACE_3            3                  /*�ؼ�ֵ���䡢��������*/
                                                 
#define  DELETE_1             1                  /*���һ������ɾ�����ݵ�λ�ã����������������*/
#define  DELETE_2             2                  /*���һ������ɾ�����ݵ�λ�ã�����ǵ�һ������*/
#define  DELETE_3             3                  /*���һ������ɾ�����ݵ�λ�ã�����ǵ�һ�����ݺ���������*/
#define  DELETE_4             4                  /*�����һ�����أ��������*/
                                                 
#define  UPDATE_SINGLE        1                  /*�ͻ��˹����ĵ�������*/
#define  DELETE_SINGLE        2                  /*�ͻ��˹����ĵ���ɾ��*/
#define  SELECT_SINGLE        3                  /*�ͻ��˹����ĵ�����ѯ*/
#define  UPDATE_BAT           4                  /*�ͻ��˹�������������*/
#define  DELETE_BAT           5                  /*�ͻ��˹���������ɾ��*/
#define  SELECT_BAT           6                  /*�ͻ��˹�����������ѯ*/
#define  UP_DE_SE_BAT         7                  /*�ͻ��˹������������»�ɾ�����ѯ*/
#define  UPDATE_OPERATE       1001               /*���²���*/
#define  DELETE_OPERATE       1002               /*ɾ������*/
#define  SELECT_OPERATE       1003               /*��ѯ����*/
                                                 
#define  SHORT_SOCKET         1                  /*������*/
#define  LONG_SOCKET          2                  /*������*/
#define  OVER_TIME            3600                 /*���ӳ�ʱ����λ��ʱ���롣�����60���ڣ�û�н��б�����·�����ӣ��ͽ�������*/
                                                 
#define  DATA_BUF_LEN         10                 /*�ͻ��˹����ĳ�����*/
#define  BAT_RETURN           524288             /*��Ӧ��������ʱÿ�η��ص�����*/
#define  OPERATE_LEN          4                  /*�ͻ��˹����Ĳ�������*/
#define  KEY_VALUE_LENGTH     4                  /*�ͻ��˹��������ݳ���*/
#define  UPDATE_SINGLE_CNT    "0001"             /*�ͻ��˹����ĵ�������*/
#define  DELETE_SINGLE_CNT    "0002"             /*�ͻ��˹����ĵ���ɾ��*/
#define  SELECT_SINGLE_CNT    "0003"             /*�ͻ��˹����ĵ�����ѯ*/
#define  UPDATE_BAT_CNT       "0004"             /*�ͻ��˹�������������*/
#define  DELETE_BAT_CNT       "0005"             /*�ͻ��˹���������ɾ��*/
#define  SELECT_BAT_CNT       "0006"             /*�ͻ��˹�����������ѯ*/
#define  UP_DE_SE_BAT_CNT     "0007"             /*�ͻ��˹������������»�ɾ�����ѯ*/
#define  UPDATE_OPERATE_CNT   "1001"             /*�ͻ��˹����ĸ��²���*/
#define  DELETE_OPERATE_CNT   "1002"             /*�ͻ��˹�����ɾ������*/
#define  SELECT_OPERATE_CNT   "1003"             /*�ͻ��˹����Ĳ�ѯ����*/

#define  MANAGER_SELECT_KEY_CNT       "2101"         /*����ͻ��˹����Ĳ�ѯKEY����*/
#define  MANAGER_SELECT_VALUE_CNT     "2102"         /*����ͻ��˹����Ĳ�ѯVALUE����*/
#define  MANAGER_SELECT_KEY_VALUE_CNT "2103"         /*����ͻ��˹����Ĳ�ѯKEY VALUE����*/
#define  MANAGER_SELECT_MIN_CNT       "2104"         /*����ͻ��˹����Ĳ�ѯ��СKEY����*/
#define  MANAGER_SELECT_MAX_CNT       "2105"         /*����ͻ��˹����Ĳ�ѯ���KEY����*/
#define  MANAGER_SELECT_COUNT_CNT     "2106"         /*����ͻ��˹����Ĳ�ѯKEY��������*/
#define  MANAGER_DELETE_CNT           "2201"         /*����ͻ��˹�����ɾ������*/
#define  MANAGER_UNLOAD_CNT           "2301"         /*����ͻ��˹����ĵ������ݵ��ļ�����*/
#define  MANAGER_LOAD_CNT             "2401"         /*����ͻ��˹����Ĵ��ļ��������ݲ���*/
#define  MANAGER_EXIT_CNT             "2501"         /*����ͻ��˹����������������̲���*/
#define  MANAGER_FLUSH_CNT            "2601"         /*����ͻ��˹����Ľ��ڴ�����д�뵽Ӳ�̲���*/
#define  MANAGER_RAM_TO_DISC_CNT      "2602"         /*����ͻ��˹����Ľ��ڴ�����д�뵽Ӳ�̲���*/
#define  MANAGER_ALTER_DISCS_CNT      "2702"         /*����ͻ��˹����������ļ����ֵ����*/
#define  MANAGER_ALTER_UPPERS_CNT     "2703"         /*����ͻ��˹������޸��ļ�����ֵ����*/
#define  MANAGER_SHOW_RAMS_CNT        "2801"         /*����ͻ��˹�������ʾ�ڴ�ʹ���������*/
#define  MANAGER_SHOW_DISCS_CNT       "2802"         /*����ͻ��˹�������ʾӲ��ʹ���������*/
#define  MANAGER_SHOW_THREADS_CNT     "2803"         /*����ͻ��˹�������ʾ�����ӺͶ������߳���������*/
#define  MANAGER_SHOW_KEYS_CNT        "2804"         /*����ͻ��˹�������ʾkey��������*/


#define  NO_WHERE_KEY_VALUE           "0000"         /*û��WHERE����*/
#define  KEY_OPEN_OPEN                "0001"         /*KEYֵ������������ҿ�()*/
#define  KEY_OPEN_CLOSE               "0002"         /*KEYֵ������������ұ�(]*/
#define  KEY_CLOSE_OPEN               "0003"         /*KEYֵ�������������ҿ�[)*/
#define  KEY_CLOSE_CLOSE              "0004"         /*KEYֵ�������������ұ�[]*/
#define  VALUE_OPEN_OPEN              "0005"         /*VALUEֵ������������ҿ�()*/
#define  VALUE_OPEN_CLOSE             "0006"         /*VALUEֵ������������ұ�(]*/
#define  VALUE_CLOSE_OPEN             "0007"         /*VALUEֵ�������������ҿ�[)*/
#define  VALUE_CLOSE_CLOSE            "0008"         /*VALUEֵ�������������ұ�[]*/

#ifdef  WAIT_OUT
  #undef  WAIT_OUT
#endif
#define WAIT_OUT 1

#ifdef  WAIT_IN
  #undef  WAIT_IN
#endif
#define WAIT_IN  0

#ifdef OUT
  #undef   OUT
#endif
#define  OUT           1

#ifdef NOT_OUT
  #undef   NOT_OUT
#endif
#define  NOT_OUT       0

#ifdef COMPLETED
  #undef   COMPLETED
#endif
#define  COMPLETED     1

#ifdef NOT_COMPLETED
  #undef   NOT_COMPLETED
#endif
#define  NOT_COMPLETED 0

#ifdef DATA_MODEL
  #undef DATA_MODEL
#endif
#ifdef BIG_DATA_MODEL
  #define DATA_MODEL 0
#endif
#ifdef SMALL_DATA_MODEL
  #define DATA_MODEL 1
#endif

#define SOCKET_SLEEP_TIME      1000   /*������ʱ��˯��ʱ�䣬UNIX��λ��΢��*/
#define SOCKET_THREAD_SLEEP() usleep( SOCKET_SLEEP_TIME );

#define THREAD_SLEEP_TIME      2000   /*������ʱ��˯��ʱ�䣬UNIX��λ��΢��*/
#define THREAD_THREAD_SLEEP() usleep( THREAD_SLEEP_TIME );


#define LOCK_SLEEP_TIME      1   /*������ʱ��˯��ʱ�䣬UNIX��λ��΢��*/
#define LOCK_SLEEP() usleep( LOCK_SLEEP_TIME );


typedef struct keyPoint      /*1��n���Ľ��*/
{
	__int64 llKeyLen;          /*KEY����*//*64b = 8b��ʶ+56bKEY���ȡ������ʶΪ0����ʾȡKEY��ַ�����ݣ������ʶΪ-1����ʾ�����7B+�����8B��KEY��ֵ*/
	__int64 llKeyAddr;         /*KEY��ַ*/
  __int64 llChildAddr;       /*��KEY����һ����ַ*/
  __int64 llChildSize;       /*��KEY����һ������*/
}KEY_POINT;


typedef struct keyValuePoint /*��󼶵Ľ��*/
{
  __int64 llKeyLen;          /*KEY����*//*64b = 8b��ʶ+56b KEY���ȡ������ʶΪ0����ʾȡKEY��ַ�����ݣ������ʶΪ-1����ʾ�����7B+�����8B��KEY��ֵ*/
  __int64 llKeyAddr;         /*KEY��ַ*/
  __int64 llValueLen;        /*VALUE����*//*64b = 8b��ʶ+56b VALUE���ȡ������ʶΪ0����ʾȡVALUE��ַ�����ݣ������ʶΪ-1����ʾ�����7B+�����8B��VALUE��ֵ*/
  __int64 llValueAddr;       /*VALUEλ��*/
}KEY_VALUE_POINT;


typedef struct keyValueBaseInfo        /*KEY VALUE�Ļ�����Ϣ*/
{
  char    pcCreateDateTime[DATE_TIME]; /*��������*/
  char    pcStartDateTime[DATE_TIME];  /*��������*/
  __int64 llKeyFileOffset;             /*��ǰ KEY �ļ�ʹ��ƫ��������λ��BLOCK_SIZE*/
  __int64 llValueFileOffset;           /*��ǰ VALUE �ļ�ʹ��ƫ��������λ��B*/
  __int64 llKeyFreeOffset;             /*��ǰ�ļ��е� KEY �ļ�������ʹ��ƫ��������λ��B*/
  __int64 llValueFreeOffset;           /*��ǰ�ļ��е� VALUE �ļ�������ʹ��ƫ��������λ��B*/
  __int64 llKeyValueSize;              /* KEY ��������*/
  __int64 llFirstDisc;                 /* KEY �ļ��е� KEY ��һ�����ݿ��ַ����λ��BLOCK_SIZE*/
  __int64 llFirstSize;                 /* KEY �ļ��е� KEY ��һ����������*/
  __int64 llLoops;                     /* KEY �ļ��е� KEY �ĵ�ǰ����*/
}KEY_VALUE_BASE_INFO;


#define THREAD_RUN_STATE_STOP  1       /*�߳�����״̬��ֹͣ*/
#define THREAD_RUN_STATE_RUN   2       /*�߳�����״̬������*/
#define ACCEPT_STATE_NO        1       /*�߳��Ƿ�������ACCEPT״̬��NOT*/
#define ACCEPT_STATE_YES       2       /*�߳��Ƿ�������ACCEPT״̬��YES*/

#ifdef SYSTEM_WINDOWS_64
  typedef struct threadResource        /*�߳̽���ʱ����Դ*/
  {
  	int  iSocket;                      /*SOCKET*/
  	int  iNewSocket;                   /**/
  	int  iRunState;                    /*����״̬*/
  	int  iAcceptState;                 /*accept�Ƿ�ִ��*/
    void *threadHandle;                /*windows�µ��߳̾��*/
  }THREAD_RESOURCE;
#else/*#ifdef SYSTEM_UBUNTU*/
  typedef struct threadResource        /*�߳̽���ʱ����Դ*/
  {
  	int  iSocket;                      /*SOCKET*/
  	int  iRunState;                    /*����״̬*/
  	int  iAcceptState;                 /*accept�Ƿ�ִ��*/
    pthread_t threadId;                /*����OS�µ��߳�ID*/
  }THREAD_RESOURCE;
#endif


#ifdef RUN_STATE_UNFINISHED
  #undef RUN_STATE_UNFINISHED
#endif
#define RUN_STATE_UNFINISHED  0        /*δ���*/

#ifdef RUN_STATE_FINISHED
  #undef RUN_STATE_FINISHED
#endif
#define RUN_STATE_FINISHED    1        /*�����*/


#ifdef RUN_CMD_RUN
  #undef RUN_CMD_RUN
#endif
#define RUN_CMD_RUN      0             /*����*/

#ifdef RUN_CMD_EXIT
  #undef RUN_CMD_EXIT
#endif
#define RUN_CMD_EXIT     1             /*��ֹ*/

#define TIME_WAIT_STEP   1             /*ѭ�����ʱ��*/

#define SELECT_KEY       0             /*��ȡ KEY*/
#define SELECT_VALUE     1             /*��ȡ VALUE*/
#define SELECT_KEY_VALUE 2             /*��ȡ KEY VALUE*/
#define EACH_SELECT_SIZE 500           /*ÿ�β�ѯʱ����������*//*���ֵ����С��MAX_AERA_SIZE*/
#define EACH_JOB_TIME    9*3600        /*�����ÿ��ִ��������ʱ�䣬���ֵ����С�ڿͻ��˵ȴ���ʱ��*/

typedef struct loadShare                     /*LOAD ����ʱ�Ľṹ��*/
{                                            
	FILE    *fp;                               /*�Ѵ򿪵��ļ�ָ��*/
	int     iState;                            /*���̵߳�ǰ����״̬��δ��ɡ������*/
	int     iCmd;                              /*ĸ�̴߳������̵߳����ִ�С���ֹ*/
	__int64 llTotalRows;                       /*����������*/
	__int64 llTotalFail;                       /*����ʧ������*/
}LOAD_SHARE;


typedef struct unloadShare                   /* UNLOAD ����ʱ�Ľṹ��*/
{
	FILE    *fp;                               /*�Ѵ򿪵��ļ�ָ��*/
	char pcValueType[KEY_VALUE_LENGTH + 1];    /*����������ֵ���䣨KEY��VALUE ���ҿ������ұա�����ҿ�������ұ�*/
	char    pcKeyStart[MAX_KEY_LEN + 1];       /* KEY �Ŀ�ʼֵ*/
	int     iKeyStartLen;                      /* KEY �Ŀ�ʼ����*/
	char    pcKeyEnd[MAX_KEY_LEN + 1];         /* KEY �Ľ���ֵ*/
	int     iKeyEndLen;                        /* KEY �Ľ�������*/
	__int64 llTotalRows;                       /*����������*/
	int     iState;                            /*���̵߳�ǰ����״̬��δ��ɡ������*/
	int     iCmd;                              /*ĸ�̴߳������̵߳����ִ�С���ֹ*/
}UNLOAD_SHARE;

typedef struct deleteShare                   /* DELETE ����ʱ�Ľṹ��*/
{
	char pcValueType[KEY_VALUE_LENGTH + 1];    /*����������ֵ���䣨KEY��VALUE ���ҿ������ұա�����ҿ�������ұ�*/
	char    pcKeyStart[MAX_VALUE_LEN + 1];     /* KEY �Ŀ�ʼֵ*/
	int     iKeyStartLen;                      /* KEY �Ŀ�ʼ����*/
	char    pcKeyEnd[MAX_VALUE_LEN + 1];       /* KEY �Ľ���ֵ*/
	int     iKeyEndLen;                        /* KEY �Ľ�������*/
	__int64 llTotalRows;                       /*ɾ��������*/
	char    pcConnerKey[MAX_AERA_SIZE + 1][MAX_KEY_LEN]; /*�߽�����*/
	int     iConnerKeyLen[MAX_AERA_SIZE + 1]; /*�߽����ݳ���*/
	int     iCornerSize;                       /*�߽���������*/
}DELETE_SHARE;

typedef struct selectCountShare              /*SELECT COUNT ����ʱ�Ľṹ��*/
{
	char pcValueType[KEY_VALUE_LENGTH + 1];    /*����������ֵ���䣨KEY��VALUE ���ҿ������ұա�����ҿ�������ұ�*/
	char    pcKeyStart[MAX_KEY_LEN + 1];       /* KEY �Ŀ�ʼֵ*/
	int     iKeyStartLen;                      /* KEY �Ŀ�ʼ����*/
	char    pcKeyEnd[MAX_KEY_LEN + 1];         /* KEY �Ľ���ֵ*/
	int     iKeyEndLen;                        /* KEY �Ľ�������*/
	__int64 llTotalRows;                       /*����������*/
	int     iState;                            /*���̵߳�ǰ����״̬��δ��ɡ������*/
	int     iCmd;                              /*ĸ�̴߳������̵߳����ִ�С���ֹ*/
}SELECT_COUNT_SHARE;

typedef struct selectShare                   /*SELECT ����ʱ�Ľṹ��*/
{
	int     iState;                            /*���̵߳�ǰ����״̬��δ��ɡ������*/
	int     iCmd;                              /*ĸ�̴߳������̵߳����ִ�С���ֹ*/
	__int64 llSelectId;                        /*��ѯID*/
}SELECT_SHARE;

#define MAX_SELECT_BUF     16*1024*1024      /*��ѯ�����������ֵ*/

typedef struct selectKeyValueResult          /*SELECT ����ʱ�Ĵ�����*/
{
	int iKeyLen;                               /*KEY����*/
	char *pcKey;                               /*KEYֵ*/
	int iValueLen;                             /*VALUE����*/
	char *pcValue;                             /*VALUEֵ*/
	struct selectKeyValueResult *next;
}SELECT_KEY_VALUE_RESULT;

#define SELECT_ID_OVER_TIME   010000         /*��ѯID���ڵ��������ڣ������ʾHHMMSS,һ��Сʱ*/

typedef struct selectResultBuf               /*��ѯ���������*/
{
	char    pcDateTime[DATE_TIME];             /*��һ��ʹ�ò�ѯID��ʱ��*/
	int     iKeyValueFlag;                     /*��ȡKEY����VALUE����KEY VALUE*/
	char    pcValueType[KEY_VALUE_LENGTH + 1]; /*����������ֵ���䣨KEY��VALUE ���ҿ������ұա�����ҿ�������ұ�*/
	char    pcKeyValueStart[MAX_VALUE_LEN + 1];/* KEY �� VALUE �Ŀ�ʼֵ*/
	int     iKeyValueStartLen;                 /* KEY �� VALUE �Ŀ�ʼ����*/
	char    pcKeyValueEnd[MAX_VALUE_LEN + 1];  /* KEY �� VALUE �Ľ���ֵ*/
	int     iKeyValueEndLen;                   /* KEY �� VALUE �Ľ�������*/
	__int64 llSelectId;                        /*��ѯID*/
	
	__int64 llStartIndex;                      /*��ǰ����еĲ�ѯ�����λ��ƫ����*/
	__int64 llRows;                            /*��ǰ����е�����*/
	SELECT_KEY_VALUE_RESULT *skvr;             /*��ѯ���*/
	SELECT_KEY_VALUE_RESULT *skvrLast;         /*��ѯ����������ĵ�ַ*/
	
	struct selectResultBuf *next;
}SELECT_RESULT_BUF;
