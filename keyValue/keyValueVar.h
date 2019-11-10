#define __KEY_VALUE_VAR_H

#ifndef __OS_H
  #include "os.h"
#endif

#define MAX_KEY_LEN       512                    /* KEY 的最大长度*/
#define MAX_VALUE_LEN     4096                   /* VALUE 的最大长度*/
                                                 
#define BLOCK_SIZE        1024                   /*块的大小*/
#define BLOCK_LEFT_MOVE   10                     /*1024 = 2^10*/
#define MAX_AERA_SIZE     32                     /*1024=32*32*/

#define DOUBLE_INT_SIZE   (sizeof(__int64) + sizeof(__int64))
//#define BLOCK_SIZE        512                  /*块的大小*/
//#define BLOCK_LEFT_MOVE   9                    /*4096 = 2^12*/
//#define MAX_AERA_SIZE     32                   /*4096=32*128*/
                                                 
#define MAX_LINE_BUF      4096                   /*配置文件中每一行的最大数量*/
#define PORT_LEN          10                     /*侦听端口的最大长度*/
#define PATH_LEN          512                    /*路径最大长度*/
#define FILE_LEN          256                    /*文件名字最大长度*/
#define NAME_LEN          64                     /*名字的最大长度*/
#define INT_LEN           10                     /*整型数据的字符串长度*/
#define INT64_LEN         20                     /*64位整型数据的字符串长度*/
#define MAX_LINKS         1000                   /*侦听时的最大连接数*/
#define DATE_LEN          8                      /*日期的长度*/
#define TIME_LEN          6                      /*时间的长度*/
#define DATE_TIME         16                     /*日期时间的长度*/
#define MIN_THREADS          10                  /*线程最小的量*/
#ifdef SYSTEM_WINDOWS_64
  #define MAX_THREADS        1000                /*线程最大的量*/
#else
  #define MAX_THREADS        100000              /*线程最大的量*/
#endif
#define NULL_SELECT_ID    "00000000000000000000" /*空的SELECT ID（数量为INT64_LEN）*/

/************下面两行针对VALUE的分配*************/
//#define MIN_RAM_UNIT      56                   /*每次分配内存的最小单位*//*原来定的是64，现在56+8=64*/
//#define VALUE_LEFT_MOVE   6                    /*64 = 2^6*/
#define MIN_RAM_UNIT      24                     /*每次分配内存的最小单位*//*原来定的是32，现在24+8=32*/
#define VALUE_LEFT_MOVE   5                      /*32 = 2^5*/
#define ADDRESS_SIZE      sizeof(void *)         /*内存地址大小*/
#define MIN_UNIT          (MIN_RAM_UNIT + ADDRESS_SIZE)/*每一个块结点的大小*/

#define INDEPENDENT_SIZE       1031                /*独立资源数量*//*建议用质数，计算结果比较平均*/
#define INDEPENDENT_FILE_SIZE  7                 /*数据文件数量*//*建议用质数，计算结果比较平均*//*暂用31*/
#define INDEPENDENT_FILE_LEFT_MOVE   3           /*2^INDEPENDENT_FILE_LEFT_MOVE > INDEPENDENT_FILE_SIZE，这个值取最接近的*/

#define DEFAULT_USER             "root"          /*如果配置信息中没有 USER ，这个就是默认值*/
#define DEFAULT_PASS             "root"          /*如果配置信息中没有 PASS ，这个就是默认值*/
#define DEFAULT_MANAGER_LISTEN   "10002"         /*如果配置信息中没有 MANAGER_LISTEN ，这个就是默认值*/
#define DEFAULT_SHORT_LISTEN     "10000"         /*如果配置信息中没有 SHORT_LISTEN ，这个就是默认值*/
#define DEFAULT_LONG_LISTEN      "10001"         /*如果配置信息中没有 LONG_LISTEN ，这个就是默认值*/
#define DEFAULT_LOGPATH          ""              /*如果配置信息中没有 LOGPATH ，这个就是默认值*/
#define DEFAULT_LOGNAME          "keyValue"      /*如果配置信息中没有 LOGNAME ，这个就是默认值*/
#define DEFAULT_KEY_RAM_SIZE     "1024"            /*如果配置信息中没有 KEY_RAM_SIZE ，这个就是默认值*/
#define DEFAULT_VALUE_RAM_SIZE   "1024"            /*如果配置信息中没有 VALUE_RAM_SIZE ，这个就是默认值*/
#define DEFAULT_KEY_FILE_PATH    ""              /*如果配置信息中没有 KEY_FILE_PATH ，这个就是默认值*/
#define DEFAULT_KEY_FILE_NAME    "keyValue.key"  /*如果配置信息中没有 KEY_FILE_NAME ，这个就是默认值*/
#define DEFAULT_VALUE_FILE_PATH  ""              /*如果配置信息中没有 VALUE_FILE_PATH ，这个就是默认值*/
#define DEFAULT_VALUE_FILE_NAME  "keyValue.val"  /*如果配置信息中没有 VALUE_FILE_NAME ，这个就是默认值*/
#define DEFAULT_KEY_MAPPING_PATH    ""                 /*如果配置信息中没有 KEY_MAPPING_PATH ，这个就是默认值*/
#define DEFAULT_KEY_MAPPING_NAME    "keyValue.mapKey"  /*如果配置信息中没有 KEY_MAPPING_NAME ，这个就是默认值*/
#define DEFAULT_VALUE_MAPPING_PATH  ""                 /*如果配置信息中没有 VALUE_MAPPING_PATH ，这个就是默认值*/
#define DEFAULT_VALUE_MAPPING_NAME  "keyValue.mapVal"  /*如果配置信息中没有 VALUE_MAPPING_NAME ，这个就是默认值*/
#define DEFAULT_FILE_UPPER_SIZE  "16"            /*如果配置信息中没有 FILE_UPPER_SIZE ，这个就是默认值*/
#define DEFAULT_FILE_MAX_SIZE    "16"            /*如果配置信息中没有 FILE_MAX_SIZE ，这个就是默认值*/
#define DEFAULT_SELECT_TEMP_PATH ""              /*如果配置信息中没有 SELECT_TEMP_PATH ，这个就是默认值*/

#define BUSY_THREAD_PERCENT      80              /*当正在使用的线程达到这个百分比，就把比例降低正常水平*/
#define NORMAL_THREAD_PERCENT    50              /*正常的线程使用比例*/
#define FREE_THREAD_PERCENT      20              /*当正在使用的线程达到这个百分比，就把比例增加正常水平*/


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

#define  SUCCESS              0                  /*成功*/
#define  NOT_FOUND            1                  /*不存在*/
#define  ERROR                2                  /*出错*/
#define  NO_BLOCK            -1                  /*不存在*/
#define  INTERRUPT            3                  /*被中断*/
                              
#define  REPLACE_0            0                  /*关键值变、数量变*/
#define  REPLACE_1            1                  /*关键值变、数量不变*/
#define  REPLACE_2            2                  /*关键值不变、数量变*/
#define  REPLACE_3            3                  /*关键值不变、数量不变*/
                                                 
#define  DELETE_1             1                  /*最后一级，被删除数据的位置，如果不是最后的数据*/
#define  DELETE_2             2                  /*最后一级，被删除数据的位置，如果是第一个数据*/
#define  DELETE_3             3                  /*最后一级，被删除数据的位置，如果是第一个数据和最后的数据*/
#define  DELETE_4             4                  /*非最后一级返回，无需操作*/
                                                 
#define  UPDATE_SINGLE        1                  /*客户端过来的单条更新*/
#define  DELETE_SINGLE        2                  /*客户端过来的单条删除*/
#define  SELECT_SINGLE        3                  /*客户端过来的单条查询*/
#define  UPDATE_BAT           4                  /*客户端过来的批量更新*/
#define  DELETE_BAT           5                  /*客户端过来的批量删除*/
#define  SELECT_BAT           6                  /*客户端过来的批量查询*/
#define  UP_DE_SE_BAT         7                  /*客户端过来的批量更新或删除或查询*/
#define  UPDATE_OPERATE       1001               /*更新操作*/
#define  DELETE_OPERATE       1002               /*删除操作*/
#define  SELECT_OPERATE       1003               /*查询操作*/
                                                 
#define  SHORT_SOCKET         1                  /*短连接*/
#define  LONG_SOCKET          2                  /*长连接*/
#define  OVER_TIME            3600                 /*连接超时，单位：时分秒。如果在60秒内，没有进行保存链路的链接，就结束连接*/
                                                 
#define  DATA_BUF_LEN         10                 /*客户端过来的长度域*/
#define  BAT_RETURN           524288             /*响应批量请求时每次返回的数量*/
#define  OPERATE_LEN          4                  /*客户端过来的操作长度*/
#define  KEY_VALUE_LENGTH     4                  /*客户端过来的数据长度*/
#define  UPDATE_SINGLE_CNT    "0001"             /*客户端过来的单条更新*/
#define  DELETE_SINGLE_CNT    "0002"             /*客户端过来的单条删除*/
#define  SELECT_SINGLE_CNT    "0003"             /*客户端过来的单条查询*/
#define  UPDATE_BAT_CNT       "0004"             /*客户端过来的批量更新*/
#define  DELETE_BAT_CNT       "0005"             /*客户端过来的批量删除*/
#define  SELECT_BAT_CNT       "0006"             /*客户端过来的批量查询*/
#define  UP_DE_SE_BAT_CNT     "0007"             /*客户端过来的批量更新或删除或查询*/
#define  UPDATE_OPERATE_CNT   "1001"             /*客户端过来的更新操作*/
#define  DELETE_OPERATE_CNT   "1002"             /*客户端过来的删除操作*/
#define  SELECT_OPERATE_CNT   "1003"             /*客户端过来的查询操作*/

#define  MANAGER_SELECT_KEY_CNT       "2101"         /*管理客户端过来的查询KEY操作*/
#define  MANAGER_SELECT_VALUE_CNT     "2102"         /*管理客户端过来的查询VALUE操作*/
#define  MANAGER_SELECT_KEY_VALUE_CNT "2103"         /*管理客户端过来的查询KEY VALUE操作*/
#define  MANAGER_SELECT_MIN_CNT       "2104"         /*管理客户端过来的查询最小KEY操作*/
#define  MANAGER_SELECT_MAX_CNT       "2105"         /*管理客户端过来的查询最大KEY操作*/
#define  MANAGER_SELECT_COUNT_CNT     "2106"         /*管理客户端过来的查询KEY数量操作*/
#define  MANAGER_DELETE_CNT           "2201"         /*管理客户端过来的删除操作*/
#define  MANAGER_UNLOAD_CNT           "2301"         /*管理客户端过来的导出数据到文件操作*/
#define  MANAGER_LOAD_CNT             "2401"         /*管理客户端过来的从文件导入数据操作*/
#define  MANAGER_EXIT_CNT             "2501"         /*管理客户端过来的正常结束进程操作*/
#define  MANAGER_FLUSH_CNT            "2601"         /*管理客户端过来的将内存数据写入到硬盘操作*/
#define  MANAGER_RAM_TO_DISC_CNT      "2602"         /*管理客户端过来的将内存数据写入到硬盘操作*/
#define  MANAGER_ALTER_DISCS_CNT      "2702"         /*管理客户端过来的增加文件最大值操作*/
#define  MANAGER_ALTER_UPPERS_CNT     "2703"         /*管理客户端过来的修改文件递增值操作*/
#define  MANAGER_SHOW_RAMS_CNT        "2801"         /*管理客户端过来的显示内存使用情况操作*/
#define  MANAGER_SHOW_DISCS_CNT       "2802"         /*管理客户端过来的显示硬盘使用情况操作*/
#define  MANAGER_SHOW_THREADS_CNT     "2803"         /*管理客户端过来的显示长连接和短连接线程数量操作*/
#define  MANAGER_SHOW_KEYS_CNT        "2804"         /*管理客户端过来的显示key数量操作*/


#define  NO_WHERE_KEY_VALUE           "0000"         /*没有WHERE条件*/
#define  KEY_OPEN_OPEN                "0001"         /*KEY值区间操作，左开右开()*/
#define  KEY_OPEN_CLOSE               "0002"         /*KEY值区间操作，左开右闭(]*/
#define  KEY_CLOSE_OPEN               "0003"         /*KEY值区间操作，左闭右开[)*/
#define  KEY_CLOSE_CLOSE              "0004"         /*KEY值区间操作，左闭右闭[]*/
#define  VALUE_OPEN_OPEN              "0005"         /*VALUE值区间操作，左开右开()*/
#define  VALUE_OPEN_CLOSE             "0006"         /*VALUE值区间操作，左开右闭(]*/
#define  VALUE_CLOSE_OPEN             "0007"         /*VALUE值区间操作，左闭右开[)*/
#define  VALUE_CLOSE_CLOSE            "0008"         /*VALUE值区间操作，左闭右闭[]*/

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

#define SOCKET_SLEEP_TIME      1000   /*遇阻塞时的睡眠时间，UNIX单位：微秒*/
#define SOCKET_THREAD_SLEEP() usleep( SOCKET_SLEEP_TIME );

#define THREAD_SLEEP_TIME      2000   /*遇阻塞时的睡眠时间，UNIX单位：微秒*/
#define THREAD_THREAD_SLEEP() usleep( THREAD_SLEEP_TIME );


#define LOCK_SLEEP_TIME      1   /*遇阻塞时的睡眠时间，UNIX单位：微秒*/
#define LOCK_SLEEP() usleep( LOCK_SLEEP_TIME );


typedef struct keyPoint      /*1到n级的结点*/
{
	__int64 llKeyLen;          /*KEY长度*//*64b = 8b标识+56bKEY长度。如果标识为0，表示取KEY地址的数据；如果标识为-1，表示后面的7B+下面的8B是KEY的值*/
	__int64 llKeyAddr;         /*KEY地址*/
  __int64 llChildAddr;       /*该KEY的下一级地址*/
  __int64 llChildSize;       /*该KEY的下一级数量*/
}KEY_POINT;


typedef struct keyValuePoint /*最后级的结点*/
{
  __int64 llKeyLen;          /*KEY长度*//*64b = 8b标识+56b KEY长度。如果标识为0，表示取KEY地址的数据；如果标识为-1，表示后面的7B+下面的8B是KEY的值*/
  __int64 llKeyAddr;         /*KEY地址*/
  __int64 llValueLen;        /*VALUE长度*//*64b = 8b标识+56b VALUE长度。如果标识为0，表示取VALUE地址的数据；如果标识为-1，表示后面的7B+下面的8B是VALUE的值*/
  __int64 llValueAddr;       /*VALUE位置*/
}KEY_VALUE_POINT;


typedef struct keyValueBaseInfo        /*KEY VALUE的基本信息*/
{
  char    pcCreateDateTime[DATE_TIME]; /*创建日期*/
  char    pcStartDateTime[DATE_TIME];  /*启动日期*/
  __int64 llKeyFileOffset;             /*当前 KEY 文件使用偏移量，单位：BLOCK_SIZE*/
  __int64 llValueFileOffset;           /*当前 VALUE 文件使用偏移量，单位：B*/
  __int64 llKeyFreeOffset;             /*当前文件中的 KEY 文件空闲区使用偏移量，单位：B*/
  __int64 llValueFreeOffset;           /*当前文件中的 VALUE 文件空闲区使用偏移量，单位：B*/
  __int64 llKeyValueSize;              /* KEY 的总数量*/
  __int64 llFirstDisc;                 /* KEY 文件中的 KEY 的一级数据块地址，单位：BLOCK_SIZE*/
  __int64 llFirstSize;                 /* KEY 文件中的 KEY 的一级数据数量*/
  __int64 llLoops;                     /* KEY 文件中的 KEY 的当前级数*/
}KEY_VALUE_BASE_INFO;


#define THREAD_RUN_STATE_STOP  1       /*线程运行状态：停止*/
#define THREAD_RUN_STATE_RUN   2       /*线程运行状态：继续*/
#define ACCEPT_STATE_NO        1       /*线程是否正处于ACCEPT状态：NOT*/
#define ACCEPT_STATE_YES       2       /*线程是否正处于ACCEPT状态：YES*/

#ifdef SYSTEM_WINDOWS_64
  typedef struct threadResource        /*线程建立时的资源*/
  {
  	int  iSocket;                      /*SOCKET*/
  	int  iNewSocket;                   /**/
  	int  iRunState;                    /*运行状态*/
  	int  iAcceptState;                 /*accept是否执行*/
    void *threadHandle;                /*windows下的线程句柄*/
  }THREAD_RESOURCE;
#else/*#ifdef SYSTEM_UBUNTU*/
  typedef struct threadResource        /*线程建立时的资源*/
  {
  	int  iSocket;                      /*SOCKET*/
  	int  iRunState;                    /*运行状态*/
  	int  iAcceptState;                 /*accept是否执行*/
    pthread_t threadId;                /*其他OS下的线程ID*/
  }THREAD_RESOURCE;
#endif


#ifdef RUN_STATE_UNFINISHED
  #undef RUN_STATE_UNFINISHED
#endif
#define RUN_STATE_UNFINISHED  0        /*未完成*/

#ifdef RUN_STATE_FINISHED
  #undef RUN_STATE_FINISHED
#endif
#define RUN_STATE_FINISHED    1        /*已完成*/


#ifdef RUN_CMD_RUN
  #undef RUN_CMD_RUN
#endif
#define RUN_CMD_RUN      0             /*运行*/

#ifdef RUN_CMD_EXIT
  #undef RUN_CMD_EXIT
#endif
#define RUN_CMD_EXIT     1             /*终止*/

#define TIME_WAIT_STEP   1             /*循环检查时间*/

#define SELECT_KEY       0             /*获取 KEY*/
#define SELECT_VALUE     1             /*获取 VALUE*/
#define SELECT_KEY_VALUE 2             /*获取 KEY VALUE*/
#define EACH_SELECT_SIZE 500           /*每次查询时搜索的数量*//*这个值不能小于MAX_AERA_SIZE*/
#define EACH_JOB_TIME    9*3600        /*服务端每次执行任务的最长时间，这个值必须小于客户端等待的时间*/

typedef struct loadShare                     /*LOAD 处理时的结构体*/
{                                            
	FILE    *fp;                               /*已打开的文件指针*/
	int     iState;                            /*子线程当前运行状态：未完成、已完成*/
	int     iCmd;                              /*母线程传给子线程的命令：执行、终止*/
	__int64 llTotalRows;                       /*导入总行数*/
	__int64 llTotalFail;                       /*导入失败行数*/
}LOAD_SHARE;


typedef struct unloadShare                   /* UNLOAD 处理时的结构体*/
{
	FILE    *fp;                               /*已打开的文件指针*/
	char pcValueType[KEY_VALUE_LENGTH + 1];    /*搜索条件的值区间（KEY或VALUE 左开右开、左开右闭、左闭右开、左闭右闭*/
	char    pcKeyStart[MAX_KEY_LEN + 1];       /* KEY 的开始值*/
	int     iKeyStartLen;                      /* KEY 的开始长度*/
	char    pcKeyEnd[MAX_KEY_LEN + 1];         /* KEY 的结束值*/
	int     iKeyEndLen;                        /* KEY 的结束长度*/
	__int64 llTotalRows;                       /*导出总行数*/
	int     iState;                            /*子线程当前运行状态：未完成、已完成*/
	int     iCmd;                              /*母线程传给子线程的命令：执行、终止*/
}UNLOAD_SHARE;

typedef struct deleteShare                   /* DELETE 处理时的结构体*/
{
	char pcValueType[KEY_VALUE_LENGTH + 1];    /*搜索条件的值区间（KEY或VALUE 左开右开、左开右闭、左闭右开、左闭右闭*/
	char    pcKeyStart[MAX_VALUE_LEN + 1];     /* KEY 的开始值*/
	int     iKeyStartLen;                      /* KEY 的开始长度*/
	char    pcKeyEnd[MAX_VALUE_LEN + 1];       /* KEY 的结束值*/
	int     iKeyEndLen;                        /* KEY 的结束长度*/
	__int64 llTotalRows;                       /*删除总行数*/
	char    pcConnerKey[MAX_AERA_SIZE + 1][MAX_KEY_LEN]; /*边角数据*/
	int     iConnerKeyLen[MAX_AERA_SIZE + 1]; /*边角数据长度*/
	int     iCornerSize;                       /*边角数据数量*/
}DELETE_SHARE;

typedef struct selectCountShare              /*SELECT COUNT 处理时的结构体*/
{
	char pcValueType[KEY_VALUE_LENGTH + 1];    /*搜索条件的值区间（KEY或VALUE 左开右开、左开右闭、左闭右开、左闭右闭*/
	char    pcKeyStart[MAX_KEY_LEN + 1];       /* KEY 的开始值*/
	int     iKeyStartLen;                      /* KEY 的开始长度*/
	char    pcKeyEnd[MAX_KEY_LEN + 1];         /* KEY 的结束值*/
	int     iKeyEndLen;                        /* KEY 的结束长度*/
	__int64 llTotalRows;                       /*导出总行数*/
	int     iState;                            /*子线程当前运行状态：未完成、已完成*/
	int     iCmd;                              /*母线程传给子线程的命令：执行、终止*/
}SELECT_COUNT_SHARE;

typedef struct selectShare                   /*SELECT 处理时的结构体*/
{
	int     iState;                            /*子线程当前运行状态：未完成、已完成*/
	int     iCmd;                              /*母线程传给子线程的命令：执行、终止*/
	__int64 llSelectId;                        /*查询ID*/
}SELECT_SHARE;

#define MAX_SELECT_BUF     16*1024*1024      /*查询缓冲区的最大值*/

typedef struct selectKeyValueResult          /*SELECT 处理时的处理结果*/
{
	int iKeyLen;                               /*KEY长度*/
	char *pcKey;                               /*KEY值*/
	int iValueLen;                             /*VALUE长度*/
	char *pcValue;                             /*VALUE值*/
	struct selectKeyValueResult *next;
}SELECT_KEY_VALUE_RESULT;

#define SELECT_ID_OVER_TIME   010000         /*查询ID存在的生命周期，这里表示HHMMSS,一个小时*/

typedef struct selectResultBuf               /*查询结果缓冲区*/
{
	char    pcDateTime[DATE_TIME];             /*上一次使用查询ID的时间*/
	int     iKeyValueFlag;                     /*获取KEY或者VALUE或者KEY VALUE*/
	char    pcValueType[KEY_VALUE_LENGTH + 1]; /*搜索条件的值区间（KEY或VALUE 左开右开、左开右闭、左闭右开、左闭右闭*/
	char    pcKeyValueStart[MAX_VALUE_LEN + 1];/* KEY 或 VALUE 的开始值*/
	int     iKeyValueStartLen;                 /* KEY 或 VALUE 的开始长度*/
	char    pcKeyValueEnd[MAX_VALUE_LEN + 1];  /* KEY 或 VALUE 的结束值*/
	int     iKeyValueEndLen;                   /* KEY 或 VALUE 的结束长度*/
	__int64 llSelectId;                        /*查询ID*/
	
	__int64 llStartIndex;                      /*当前结点中的查询结果的位置偏移量*/
	__int64 llRows;                            /*当前结点中的行数*/
	SELECT_KEY_VALUE_RESULT *skvr;             /*查询结果*/
	SELECT_KEY_VALUE_RESULT *skvrLast;         /*查询结果中最后结点的地址*/
	
	struct selectResultBuf *next;
}SELECT_RESULT_BUF;
