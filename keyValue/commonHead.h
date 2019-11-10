#define __COMMON_HEAD_H

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <memory.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <sys/timeb.h>
#include <time.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pthread.h>
#include <signal.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <signal.h>

#ifndef __OS_H
  #include "os.h"
#endif

#ifdef SYSTEM_WINDOWS_64
  #include <winsock2.h>
  #include <windows.h>
  #include <process.h>
  #include <synchapi.h>
  #include <processthreadsapi.h>
#endif

#ifdef SYSTEM_UBUNTU
  #include <sys/socket.h>
  #include <arpa/inet.h>
#endif

#ifndef __KEY_VALUE_VAR_H
  #include "keyValueVar.h"
#endif

#ifndef __KEY_VALUE_OVERALL_H
  #include "keyValueOverall.h"
#endif

#ifndef __KEY_VALUE_DEFINE_H
  #include "keyValueDefine.h"
#endif

#ifndef __COMMON_C
  #include "common.c"
#endif

#ifndef __LOG_C
  #include "log.c"
#endif

#ifndef __LOG_H
  #include "log.h"
#endif

#ifndef __BLOCK_C
  #include "block.c"
#endif

#ifndef __KEY_VALUE_ERROR_H
  #include "keyValueError.h"
#endif

#ifndef __KEY_VALUE_SELECT_C
  #include "keyValueSelect.c"
#endif

#ifndef __KEY_VALUE_INSERT_C
  #include "keyValueInsert.c"
#endif

#ifndef __KEY_VALUE_UPDATE_C
  #include "keyValueUpdate.c"
#endif

#ifndef __KEY_VALUE_DELETE_C
  #include "keyValueDelete.c"
#endif

#ifndef __KEY_VALUE_INIT_C
  #include "keyValueInit.c"
#endif

#ifndef __KEY_VALUE_DEFINE_MAIN_H
  #include "keyValueDefineMain.h"
#endif

#ifndef __BLOCK_MAIN_C
  #include "blockMain.c"
#endif

#ifndef __KEY_VALUE_SELECT_MAIN_C
  #include "keyValueSelectMain.c"
#endif

#ifndef __KEY_VALUE_INSERT_MAIN_C
  #include "keyValueInsertMain.c"
#endif

#ifndef __KEY_VALUE_UPDATE_MAIN_C
  #include "keyValueUpdateMain.c"
#endif

#ifndef __KEY_VALUE_DELETE_MAIN_C
  #include "keyValueDeleteMain.c"
#endif

#ifndef __KEY_VALUE_THREAD_C
  #include "keyValueThread.c"
#endif

#ifndef __KEY_VALUE_THREAD_MANAGER_C
  #include "keyValueThreadManager.c"
#endif

#ifndef __KEY_VALUE_THREAD_CONTROL_C
  #include "keyValueThreadControl.c"
#endif 