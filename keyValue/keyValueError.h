#define __KEY_VALUE_ERROR_H

#define CHINESE

#ifdef CHINESE 
	#define CREATE_SOCKET_ERRMSG          "建立SOCKET失败，请检查端口是否已被使用，或者程序是否有该端口的权限，errno[%d]\n"
  #define BIND_SOCKET_ERRMSG            "绑定SOCKET失败，请检查端口是否已被使用，或者程序是否有该端口的权限，errno[%d]\n"
  #define LISTEN_SOCKET_ERRMSG          "侦听SOCKET失败，请检查端口是否已被使用，或者程序是否有该端口的权限，errno[%d]\n"
  #define SET_SOCKET_ERRMSG             "设置SOCKET失败，请检查端口是否已被使用，或者程序是否有该端口的权限，errno[%d]\n"  
  #define CREATE_SOCKET_ERRMSG_UNIX     "建立SOCKET失败，请检查端口是否已被使用，或者程序是否有该端口的权限，errno[%d]msg[%s]\n"
  #define BIND_SOCKET_ERRMSG_UNIX       "绑定SOCKET失败，请检查端口是否已被使用，或者程序是否有该端口的权限，errno[%d]msg[%s]\n"
  #define LISTEN_SOCKET_ERRMSG_UNIX     "侦听SOCKET失败，请检查端口是否已被使用，或者程序是否有该端口的权限，errno[%d]msg[%s]\n"
  #define SET_SOCKET_ERRMSG_UNIX        "设置SOCKET失败，请检查端口是否已被使用，或者程序是否有该端口的权限，errno[%d]msg[%s]\n"
  
#endif


