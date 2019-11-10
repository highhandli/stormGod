#define __OS_H

#define SYSTEM_WINDOWS_64
//#define SYSTEM_UBUNTU

#define BIG_DATA_MODEL     /*大数模式*/
//#define SMALL_DATA_MODEL   /*小数模式*/

#ifdef SYSTEM_WINDOWS_64

#else//#ifdef SYSTEM_UBUNTU
  #define closesocket(socket) close(socket)
  
  typedef long long int __int64;

  #define fopen64( v1, v2 ) fopen( v1, v2 )
  #define fseeko64( v1, v2, v3 ) fseek( v1, v2, v3 )
  #define ftello64( v1 ) ftell( v1 )
  #define atoi( v1 ) atoll( v1 )
#endif