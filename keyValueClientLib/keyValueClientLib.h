void *keyValueMalloc( int iaSize );
void keyValueFree( void *pcaAddress );
int keyValueClientShort( char *pcaIp, char *pcaPort, void **pcaData, int *iaDataLen );
pthread_t keyValueClientLongInti( char *pcaIp, char *pcaPort );
void keyValueLongSocket( int *iaSocket );
void keyValueClientLongRls(pthread_t threadID);
int keyValueClientLong( pthread_t pthreadID, void **pcaData, int *iaDataLen );
int keyValueUpdateSingle( char *pcaBufStart, char *pcaBufEnd, int iaSocket, char **pcaOutBuf, int *iaOutLen );
int keyValueDeleteSingle( char *pcaBufStart, char *pcaBufEnd, int iaSocket, char **pcaOutBuf, int *iaOutLen );
int keyValueSelectSingle( char *pcaBufStart, char *pcaBufEnd, int iaSocket, char **pcaOutBuf, int *iaOutLen );
int keyValueUpdateBat( char *pcaBufStart, char *pcaBufEnd, int iaSocket, char **pcaOutBuf, int *iaOutLen );
int keyValueDeleteBat( char *pcaBufStart, char *pcaBufEnd, int iaSocket, char **pcaOutBuf, int *iaOutLen );
int keyValueSelectBat( char *pcaBufStart, char *pcaBufEnd, int iaSocket, char **pcaOutBuf, int *iaOutLen );
int keyValueMixBat( char *pcaBufStart, char *pcaBufEnd, int iaSocket, char **pcaOutBuf, int *iaOutLen );
int keyValueConnectToServer( char *pcaIp, char *pcaPort );
int keyValueWaitFd( int iFd, int iFlag, int iTimeOut );
char *keyValueAllTrim ( char *pcaStr );
int keyValueCheckPort( char *pcaPort );
int keyValueCheckIP( char *pcaIP );
int keyValueIsBlank( char c );
int keyValueIsInt( char *pcaBufStart, char *pcaBufEnd );
int keyValueIsDigitals( char *pcaBufStart, char *pcaBufEnd );
int keyValueStrcmpIgnore( char *p, char *p1 );
int keyValueStrcnmpIgnore( char *p, char *p1, int iaSize );
