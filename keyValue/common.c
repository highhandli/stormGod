/*防止重复包含*/
#define __COMMON_C

#ifndef _COMMON_HEAD_H
  #include "commonHead.h"
#endif

char *AllTrim ( char *pcaStr );
int getDateTime( char *pcaDate, char *pcaTime );
int strcmpIgnore( char *p, char *p1 );
int isDigitals( char *pcaBufStart, char *pcaBufEnd );
int getDateTime( char *pcaDate, char *pcaTime );

/********************************************************
代码名称：common.c
代码功能：常用的公共函数 
作    者：李高文 
创建时间：20141114
修改时间：
*********************************************************/


/********************************************************
函数名：AllTrim
函数功能：去掉头尾空格 
入口参数：
第    一：字符串     I
返    回：1表示该字符为空格，0表示不是
创建时间：20130226
*********************************************************/
char *AllTrim ( char *pcaStr )
{
	char *pcStart;
	char *pcEnd;
	char pcBuf[8192];
	
	pcStart = pcaStr;
	pcEnd = pcStart + strlen(pcStart);
	
	/*忽略开头部分的空格*/
	while (1)
	{
		/*如果全都是空格*/
		if (pcStart == pcEnd)
		{
			return pcaStr;
		}
		
		/*如果是空格*/
		if (*pcStart == ' ' || *pcStart == '\t' || *pcStart == '\r' || *pcStart == '\n' /*|| *pcStart == '\0'*/)
		{
			*pcStart = 0x00;
			pcStart++;
		}
		else
		{
			break;
		}
	}
	

	/*忽略末尾部分的空格*/
	while (1)
	{
		/*如果全都是空格*/
		if (pcEnd == pcStart)
		{
			return pcaStr;
		}
		/*如果是空格*/
		if (*pcEnd == ' ' || *pcEnd == '\t' || *pcEnd == '\r' || *pcEnd == '\n' || *pcEnd == '\0')
		{
			*pcEnd = 0x00;
			pcEnd--;
		}
		else
		{
			break;
		}
	}
	memset( pcBuf, 0x00, sizeof(pcBuf) );
	memcpy( pcBuf, pcStart, pcEnd - pcStart + 1 );
	strcpy( pcaStr, pcBuf );
	return pcaStr;
}


/********************************************************
函数名：isBlank
函数功能：检查字符串是否为空格 
入口参数：
第    一：字符     I
返    回：1表示该字符为空格，0表示不是
创建时间：20130226
*********************************************************/
int isBlank( char c )
{
  if (c == '\t' || c == ' ' || c == '\n' || c == '\0' || c == '\r')
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/********************************************************
函数名：isBlank
函数功能：检查字符串是否为空格 
入口参数：
第    一：字符     I
返    回：1表示该字符串为空格，0表示不是
创建时间：20130226
*********************************************************/
int isBlanks( char *pcaStart, char *pcaEnd )
{
  char *pcStart;
  char *pcEnd;
  
  pcStart = pcaStart;
  pcEnd = pcaEnd;
  
  while (pcStart < pcEnd)
  {
  	if (*pcStart == '\t' || *pcStart == ' ' || *pcStart == '\n' || *pcStart == '\0' || *pcStart == '\r')
  	{
	  	pcStart++;
	  }
	  else
	  {
  		return 0;
  	}
  }
  return 1;
}

/********************************************************
函数名：isLetter
函数功能：检查字符串是否为字母 
入口参数：
第    一：字符     I
返    回：1表示该字符为字母，0表示不是
创建时间：20130226
*********************************************************/
int isLetter( char c )
{
  if ((c >= 'A' && c <='Z') || (c >= 'a' && c <='z'))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/********************************************************
函数名：isDigital
函数功能：检查字符串是否为数字 
入口参数：
第    一：字符     I
返    回：1表示该字符为数字，0表示不是
创建时间：20130226
*********************************************************/
int isDigital( char c )
{
  if ((c >='0' && c <='9') || c == '.')
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/********************************************************
函数名：isDouble
函数功能：检查字符串是否为浮点 
入口参数：
第    一：缓冲区的开始     I
第    二：缓冲区的结束     I 
返    回：1表示该字符串为浮点，0表示不是
创建时间：20130226
*********************************************************/
int isDouble( char *pcaBufStart, char *pcaBufEnd )
{
  char *pcBuf;
  pcBuf = pcaBufStart;
  while(1)
  {
    if (pcBuf == pcaBufEnd)
    {
      return 0;
    }
    if (*pcBuf == '.')
    {
      return 1;
    }
    else
    {
      pcBuf++;
    }
  }
}

/********************************************************
函数名：isDigitals
函数功能：检查字符串是否都为数字 
入口参数：
第    一：缓冲区的开始     I
第    二：缓冲区的结束     I 
返    回：1表示该字符串都为数字，0表示不是
创建时间：20130226
*********************************************************/
int isDigitals( char *pcaBufStart, char *pcaBufEnd )
{
  char *pcBuf;
  
  pcBuf = pcaBufStart;
  while(1)
  {
    if (pcBuf == pcaBufEnd)
    {
      return 1;
    }
    if ((*pcBuf < '0' || *pcBuf >'9') && *pcBuf != '.')
    {
      return 0;
    }
    else
    {
      pcBuf++;
    }
  }
}

/********************************************************
函数名：isInt
函数功能：检查字符串是为整型 
入口参数：
第    一：缓冲区的开始     I
第    二：缓冲区的结束     I 
返    回：1表示该字符串都为数字，0表示不是
创建时间：20130226
*********************************************************/
int isInt( char *pcaBufStart, char *pcaBufEnd )
{
  char *pcBuf;
  
  pcBuf = pcaBufStart;
  while(1)
  {
    if (pcBuf == pcaBufEnd)
    {
      return 1;
    }
    if (*pcBuf < '0' || *pcBuf >'9')
    {
      return 0;
    }
    else
    {
      pcBuf++;
    }
  }
}

/********************************************************
函数名：isDate
函数功能：检查字符串是否为日期 
入口参数：
第    一：缓冲区的开始     I
第    二：缓冲区的结束     I 
返    回：1表示该字符串满足日期格式，0表示不是
创建时间：20130226
*********************************************************/
int isDate( char *pcaBufStart, char *pcaBufEnd )
{
  /*
  日期格式：yyyymmdd、yyyy-mm-dd
  */
  char pcYear[5];
  char pcMon[3];
  char pcDay[3];
  char *pcBufStart;
  char *pcTmp;
  int  iYearValue;
  int  iMonValue;
  int  iDayValue;
  
  pcBufStart = pcaBufStart;
  
  /*搜索'-'*/
  pcTmp = strchr( pcBufStart, '-' );
  /*如果没有*/
  if (pcTmp == NULL)
  {
    /*取日期*/
    memcpy( pcYear, pcBufStart, 4 );
    pcYear[4] = 0x00;
    /*判断年份是否符合格式*/
    /*是否为整型*/
    if (!isInt( pcYear, pcYear+4 ))
    {
      return 0;
    }
    iYearValue = atoi(pcYear);
    if (iYearValue < 1900)
    {
      return 0;
    }
    
    /*取月份*/
    memcpy( pcMon, pcBufStart+4, 2 );
    pcMon[2] = 0x00;
    /*判断月份是否符合格式*/
    /*是否为整型*/
    if (!isInt( pcMon, pcMon+2 ))
    {
      return 0;
    }
    iMonValue = atoi(pcMon);
    if (iMonValue < 1 || iMonValue > 12)
    {
      return 0;
    }
    
    /*取天数*/
    memcpy( pcDay, pcBufStart+6, 2 );
    pcDay[2] = 0x00;
    /*判断月份是否符合格式*/
    /*是否为整型*/
    if (!isInt( pcDay, pcDay+2 ))
    {
      return 0;
    }
    iDayValue = atoi(pcDay);
    if (iDayValue < 1 || iDayValue > 31)
    {
      return 0;
    }
    
    /*判断月份和天数*/
    if (iMonValue == 4 || iMonValue == 6 || iMonValue == 9 || iMonValue == 11 )
    {
      if (iDayValue > 30)
      {
        return 0;
      }
    }
    else
    if (iMonValue == 2)
    {
      if (iDayValue > 29)
      {
        return 0;
      }
    }
  }
  else
  {
    /*取日期*/
    memcpy( pcYear, pcBufStart, 4 );
    pcYear[4] = 0x00;
    /*判断年份是否符合格式*/
    /*是否为整型*/
    if (!isInt( pcYear, pcYear+4 ))
    {
      return 0;
    }
    iYearValue = atoi(pcYear);
    if (iYearValue < 1900)
    {
      return 0;
    }
    
    /*取月份*/
    memcpy( pcMon, pcBufStart+5, 2 );
    pcMon[2] = 0x00;
    /*判断月份是否符合格式*/
    if (!isInt( pcMon, pcMon+2 ))
    {
      return 0;
    }
    iMonValue = atoi(pcMon);
    if (iMonValue < 1 || iMonValue > 12)
    {
      return 0;
    }
    
    /*取天数*/
    memcpy( pcDay, pcBufStart+8, 2 );
    pcDay[2] = 0x00;
    /*判断月份是否符合格式*/
    if (!isInt( pcDay, pcDay+2 ))
    {
      return 0;
    }
    iDayValue = atoi(pcDay);
    if (iDayValue < 1 || iDayValue > 31)
    {
      return 0;
    }
    
    /*判断月份和天数*/
    if (iMonValue == 4 || iMonValue == 6 || iMonValue == 9 || iMonValue == 11 )
    {
      if (iDayValue > 30)
      {
        return 0;
      }
    }
    else
    if (iMonValue == 2)
    {
      if (iDayValue > 29)
      {
        return 0;
      }
    }
  }
  return 1;
}

/********************************************************
函数名：isTime
函数功能：检查字符串是否为时间 
入口参数：
第    一：缓冲区的开始     I
第    二：缓冲区的结束     I 
返    回：1表示该字符串满足时间格式，0表示不是
创建时间：20130226
*********************************************************/
int isTime( char *pcaBufStart, char *pcaBufEnd )
{
  /*
  时间格式：hhmmss、hh-mm-ss、hh:mm:ss 
  */
  char *pcBufStart;
  char *pcTmp;
  char pcHour[3];
  char pcMin[3];
  char pcSec[3];
  int  iHourValue;
  int  iMinValue;
  int  iSecValue;
  
  pcBufStart = pcaBufStart;
  
  /*搜索'-'*/
  pcTmp = strchr( pcBufStart, '-' );
  /*如果有*/
  if (pcTmp != NULL)
  {
    /*取小时*/
    memcpy( pcHour, pcBufStart, 2 );
    pcHour[2] = 0x00;
    /*判断时间是否符合格式*/
    /*是否为整型*/
    if (!isInt( pcHour, pcHour+2 ))
    {
      return 0;
    }
    iHourValue = atoi(pcHour);
    if (iHourValue < 0 || iHourValue > 23)
    {
      return 0;
    }
    
    /*取分钟*/
    memcpy( pcMin, pcBufStart+3, 2 );
    pcMin[2] = 0x00;
    /*判断时间是否符合格式*/
    /*是否为整型*/
    if (!isInt( pcMin, pcMin+2 ))
    {
      return 0;
    }
    iMinValue = atoi(pcMin);
    if (iMinValue < 0 || iMinValue > 59)
    {
      return 0;
    }
    
    /*取秒*/
    memcpy( pcSec, pcBufStart+6, 2 );
    pcSec[2] = 0x00;
    /*判断时间是否符合格式*/
    /*是否为整型*/
    if (!isInt( pcSec, pcSec+2 ))
    {
      return 0;
    }
    iSecValue = atoi(pcSec);
    if (iSecValue < 0 || iSecValue > 59)
    {
      return 0;
    }
    return 1;
  }
  
  /*搜索':'*/
  pcTmp = strchr( pcBufStart, ':' );
  /*如果有*/
  if (pcTmp != NULL)
  {
    /*取小时*/
    memcpy( pcHour, pcBufStart, 2 );
    pcHour[2] = 0x00;
//printf( "[%s]\n", pcHour );
    /*判断时间是否符合格式*/
    /*是否为整型*/
    if (!isInt( pcHour, pcHour+2 ))
    {
      return 0;
    }
    iHourValue = atoi(pcHour);
    if (iHourValue < 0 || iHourValue > 23)
    {
      return 0;
    }
    
    /*取分钟*/
    memcpy( pcMin, pcBufStart+3, 2 );
    pcMin[2] = 0x00;
//printf( "[%s]\n", pcMin );
    /*判断时间是否符合格式*/
    /*是否为整型*/
    if (!isInt( pcMin, pcMin+2 ))
    {
      return 0;
    }
    iMinValue = atoi(pcMin);
    if (iMinValue < 0 || iMinValue > 59)
    {
      return 0;
    }
    
    /*取秒*/
    memcpy( pcSec, pcBufStart+6, 2 );
    pcSec[2] = 0x00;
//printf( "[%s]\n", pcSec );
    /*判断时间是否符合格式*/
    /*是否为整型*/
    if (!isInt( pcSec, pcSec+2 ))
    {
      return 0;
    }
    iSecValue = atoi(pcSec);
    if (iSecValue < 0 || iSecValue > 59)
    {
      return 0;
    }
    return 1;
  }
  
  /*hhmmss格式*/
  /*取小时*/
  memcpy( pcHour, pcBufStart, 2 );
  pcHour[2] = 0x00;
  /*判断时间是否符合格式*/
  /*是否为整型*/
  if (!isInt( pcHour, pcHour+2 ))
  {
    return 0;
  }
  iHourValue = atoi(pcHour);
  if (iHourValue < 0 || iHourValue > 23)
  {
    return 0;
  }
    
  /*取分钟*/
  memcpy( pcMin, pcBufStart+2, 2 );
  pcMin[2] = 0x00;
  /*判断时间是否符合格式*/
  /*是否为整型*/
  if (!isInt( pcMin, pcMin+2 ))
  {
    return 0;
  }
  iMinValue = atoi(pcMin);
  if (iMinValue < 0 || iMinValue > 59)
  {
    return 0;
  }
    
  /*取秒*/
  memcpy( pcSec, pcBufStart+4, 2 );
  pcSec[2] = 0x00;
  /*判断时间是否符合格式*/
  /*是否为整型*/
  if (!isInt( pcSec, pcSec+2 ))
  {
    return 0;
  }
  iSecValue = atoi(pcSec);
  if (iSecValue < 0 || iSecValue > 59)
  {
    return 0;
  }
  return 1; 
}

/********************************************************
函数名：isDatatime
函数功能：检查字符串是否为日期时间 
入口参数：
第    一：缓冲区的开始     I
第    二：缓冲区的结束     I 
返    回：1表示该字符串满足日期时间格式，0表示不是
创建时间：20130226
*********************************************************/
int isDatetime( char *pcaBufStart, char *pcaBufEnd )
{
  /*
  日期时间格式：yyyy-mm-dd hh:mm:ss
  */
  char *pcBufStart;
  char *pcTmp;
  
  pcBufStart = pcaBufStart;
  
  pcTmp = strchr( pcBufStart, ' ' );
  /*如果没有空格分开*/
  if (pcTmp == NULL)
  {
    return 0;
  }
  if (pcTmp -1 - pcBufStart < 8)
  {
    return 0;
  }
  if (pcaBufEnd -1 - pcTmp < 6)
  {
    return 0;
  }
  /*如果不满足日期格式*/
  if (!isDate( pcBufStart, pcTmp-1 ))
  {
    return 0;
  }
  /*如果不满足时间格式*/
  if (!isDate( pcTmp+1, pcaBufEnd ))
  {
    return 0;
  }
  return 1;
}

/**********************************************************************
函数名称: memcmpIgnore
函数功能: 比较，忽略大小写 
作    者: 李高文 
建立时间: 
参    数：
第    一：第一个缓冲区   I 
第    二：第二个缓冲区   I
第    三：数量 
返    回：0表示相等，其它表示失败 
**********************************************************************/
int memcmpIgnore( char *p, char *p1, int size )
{
  char c;
  char c1;
  int i;
  for (i = 0; i < size; i++)
  {
    c = *(p + i);
    c1 = *(p1 + i);
    if (c >= 'a' && c <= 'z')
     {
       c -= 32;
    }
    if (c1 >= 'a' && c1 <= 'z')
     {
       c1 -= 32;
    }
    if (c != c1)
    {
      return -1;
    }
  }
  return 0;
}

/**********************************************************************
函数名称: strcmpIgnore
函数功能: 比较，忽略大小写 
作    者: 李高文 
建立时间: 
参    数：
第    一：第一个缓冲区   I 
第    二：第二个缓冲区   I
返    回：0表示相等，其它表示失败 
**********************************************************************/
int strcmpIgnore( char *p, char *p1 )
{
  char c;
  char c1;
  int i;
  int size;
  int size1;
  
  size = strlen(p);
  size1 = strlen(p1);
  
  if (size != size1)
  {
    return -1;
  }
  for (i = 0; i < size; i++)
  {
    c = *(p + i);
    c1 = *(p1 + i);
    if (c >= 'a' && c <= 'z')
     {
       c -= 32;
    }
    if (c1 >= 'a' && c1 <= 'z')
     {
       c1 -= 32;
    }
    if (c != c1)
    {
      return -1;
    }
  }
  return 0;
}

/**********************************************************************
函数名称: strcmpIgnore
函数功能: 比较，忽略大小写 
作    者: 李高文 
建立时间: 
参    数：
第    一：第一个缓冲区   I 
第    二：第二个缓冲区   I
返    回：0表示相等，其它表示失败 
**********************************************************************/
int strncmpIgnore( char *p, char *p1, int iaSize )
{
  char c;
  char c1;
  int i;
  int size;
  
  size = iaSize;
  
  for (i = 0; i < size; i++)
  {
    c = *(p + i);
    c1 = *(p1 + i);
    if (c >= 'a' && c <= 'z')
     {
       c -= 32;
    }
    if (c1 >= 'a' && c1 <= 'z')
     {
       c1 -= 32;
    }
    if (c != c1)
    {
      return -1;
    }
  }
  return 0;
}

/**********************************************************************
函数名称: toLowers
函数功能: 将字符串中的字母变为小写
作    者: 李高文 
建立时间: 
参    数：
第    一：字符串       I/O
第    二：字符串长度   I
返    回：0
**********************************************************************/
int toLowers( char *pcaBuf, int iaSize )
{
	char *pcBuf;
	
	pcBuf = pcaBuf;
	
	while (pcBuf <= pcaBuf + iaSize)
	{
		*pcBuf = (char)(tolower(*pcBuf));
		pcBuf++;
	}
	return 0;
}

/**********************************************************************
函数名称: toLowers
函数功能: 将字符串中的字母变为小写
作    者: 李高文 
建立时间: 
参    数：
第    一：输出日期       O
第    二：输出时间       O
返    回：0
**********************************************************************/
int getDateTime( char *pcaDate, char *pcaTime )
{
  time_t tp=time(NULL);
  struct tm   *T_Now;
  
  T_Now = ( localtime(&(tp)) );
  sprintf( pcaDate, "%04d%02d%02d", T_Now->tm_year+1900, T_Now->tm_mon+1, T_Now->tm_mday );
  sprintf( pcaTime, "%02d%02d%02d", T_Now->tm_hour, T_Now->tm_min, T_Now->tm_sec );
  return 0;
}

/********************************************************
函数名  ：checkPort
函数功能：检查端口是否正确
入口参数：
第    一：端口                I
返    回：1表示符合，0表示不符合
创建时间：20151102
*********************************************************/
int checkPort( char *pcaPort )
{
	if (isDigitals( pcaPort, pcaPort + strlen(pcaPort)) == 0 || (atoi(pcaPort) < 0 || atoi(pcaPort) > 65535))
  {
  	return 0;
  }
  else
  {
  	return 1;
  }
}

/********************************************************
函数名  ：checkIP
函数功能：检查IP是否正确
入口参数：
第    一：IP                I
返    回：1表示符合，0表示不符合
创建时间：20131024
*********************************************************/
int checkIP( char *pcaIP )
{
  char pcValue[20];
  char *pcBuf;
  char *pcTmp;
  
  pcBuf = pcaIP;
  
  /*取前8b*/
  pcTmp = strchr( pcBuf, '.' );
  if (pcTmp == NULL)
  {
    return 0;
  }
  memset( pcValue, 0x00, sizeof(pcValue) );
  memcpy( pcValue, pcBuf, pcTmp - pcBuf );
  
  /*如果有非法字符*/
  if (!isInt(pcValue, pcValue + strlen(pcValue)))
  {
  	return 0;
  }
  
  /*如果值不对*/
  if (atoi(pcValue) < 0 || atoi(pcValue) > 255)
  {
    return 0;
  }
  pcBuf = pcTmp + 1;
  
  /*取8b到15b*/
  pcTmp = strchr( pcBuf, '.' );
  if (pcTmp == NULL)
  {
    return 0;
  }
  memset( pcValue, 0x00, sizeof(pcValue) );
  memcpy( pcValue, pcBuf, pcTmp - pcBuf );
  /*如果有非法字符*/
  if (!isInt(pcValue, pcValue + strlen(pcValue)))
  {
  	return 0;
  }
  
  /*如果值不对*/
  if (atoi(pcValue) < 0 || atoi(pcValue) > 255)
  {
    return 0;
  }
  pcBuf = pcTmp + 1;
  
  /*取16b到23b*/
  pcTmp = strchr( pcBuf, '.' );
  if (pcTmp == NULL)
  {
    return 0;
  }
  memset( pcValue, 0x00, sizeof(pcValue) );
  memcpy( pcValue, pcBuf, pcTmp - pcBuf );
  /*如果有非法字符*/
  if (!isInt(pcValue, pcValue + strlen(pcValue)))
  {
  	return 0;
  }
  
  /*如果值不对*/
  if (atoi(pcValue) < 0 || atoi(pcValue) > 255)
  {
    return 0;
  }
  pcBuf = pcTmp + 1;
  
  /*取前24b到31b*/
  memset( pcValue, 0x00, sizeof(pcValue) );
  strcpy( pcValue, pcBuf );
  /*如果有非法字符*/
  if (!isInt(pcValue, pcValue + strlen(pcValue)))
  {
  	return 0;
  }
  
  /*如果值不对*/
  if (atoi(pcValue) < 0 || atoi(pcValue) > 255)
  {
    return 0;
  }
  return 1;
}

/********************************************************
函数名：indexof
函数功能： 确定字符在字符串的位置（从前面开始搜索） 
入口参数：
第    一：字符串 
第    二：搜索的字符 
返    回：如果有该字符，返回对应的位置；没有，返回NULL 
*********************************************************/
char *indexof(char *string,char c)
{
  char *p;
  for (p = string; *p != '\0'; p++)
  {
    if (*p == c)
      return p; 
  }
  return NULL;
}

/********************************************************
函数名：lastindexof
函数功能： 确定字符在字符串的位置 （从后面开始搜索） 
入口参数：
第    一：字符串 
第    二：搜索的字符 
返    回：如果有该字符，返回对应的位置；没有，返回NULL 
*********************************************************/
char *lastindexof(char *string,char c)
{
  char *p;
  for (p = string + strlen(string) - 1; p >= 0;p--)
  {
    if(*p==c)
      return p;
    break;
  }
  return NULL;
}

/********************************************************
函数名：indexofstring
函数功能： 确定字符串在字符串的位置 （从前面开始搜索） 
入口参数：
第    一：字符串 
第    二：搜索的字符串 
返    回：如果有该字符串，返回对应的位置；没有，返回NULL 
*********************************************************/
char *indexofstring(char *string,char *found)
{
  char *p;//指向string 
  char *p1;//指向found 
  char *p2;
  int  iStringLen;
  int  iFoundLen;
  iStringLen = strlen(string);
  iFoundLen = strlen(found);
  
  if (iStringLen < iFoundLen)
  {
    return NULL;
  }
  else
  if (iStringLen == iFoundLen)
  {
    if(strcmp(string,found)==0)
    {
      return string;
    }
    else
    {
      return NULL;
    }
  }
  else
  {
    for (p = string; p < string + iStringLen; p++)
    {
      for (p1 = found, p2 = p; p1 < found + iFoundLen; p1++)
      {
        if (*p1 == *p2)
          p2++;
        else
          break;
      }
      /*如果满足条件*/
      if (p1== found + iFoundLen)
        return p;
    }
    return NULL;
  }
}

/********************************************************
函数名：indexofstringIgnore
函数功能： 确定字符串在字符串的位置（从前面开始搜索）（忽略大小写） 
入口参数：
第    一：字符串 
第    二：搜索的字符串 
返    回：如果有该字符串，返回对应的位置；没有，返回NULL 
*********************************************************/
char *indexofstringIgnore(char *string,char *found)
{
  char *p;//指向string 
  char *p1;//指向found 
  char *p2;
  char c1;
  char c2;
  int  iStringLen;
  int  iFoundLen;
  iStringLen = strlen(string);
  iFoundLen = strlen(found);
  
  if (iStringLen < iFoundLen)
  {
    return NULL;
  }
  else
  if (iStringLen == iFoundLen)
  {
    if(strcmp(string,found)==0)
    {
      return string;
    }
    else
    {
      return NULL;
    }
  }
  else
  {
    for (p = string; p < string + iStringLen; p++)
    {
      for (p1 = found, p2 = p; p1 < found + iFoundLen; p1++)
      {
        /*如果是字母类的*/
        c1 = *p1;
        c2 = *p2;
        if (c1 >= 'a' && c1 <= 'z')
        {
          c1 -= 32;
        }
        if (c2 >= 'a' && c2 <= 'z')
        {
          c2 -= 32;
        }
        if (c1 == c2)
          p2++;
        else
          break;
      }
      /*如果满足条件*/
      if (p1== found + iFoundLen)
        return p;
    }
    return NULL;
  }
}

/********************************************************
函数名：lastindexofstring
函数功能： 确定字符串在字符串的位置 （从后面开始搜索） 
入口参数：
第    一：字符串 
第    二：搜索的字符串 
返    回：如果有该字符串，返回对应的位置；没有，返回NULL 
*********************************************************/
char *lastindexofstring(char *string,char *found)
{
  char *p;//指向string 
  char *p1;//指向found 
  char *p2;
  int  iStringLen;
  int  iFoundLen;
  
  iStringLen = strlen(string);
  iFoundLen = strlen(found);
  if (iStringLen < iFoundLen)
  {
    return NULL;
  }
  else
  if (iStringLen == iFoundLen)
  {
    if(strcmp(string,found)==0)
    {
      return string;
    }
    else
    {
      return NULL;
    }
  }
  else
  {
    for (p = string + iStringLen - 1; p >= string; p--)
    {
      for (p1 = found + iFoundLen - 1, p2 = p; p1 >= found; p1--)
      {
        if (*p1 == *p2)
          p2--;
        else
          break;
      }
      /*如果满足条件*/
      if (p1== found - 1)
        return p;
    }
    return NULL;
  }
}

/********************************************************
函数名：indexofstringIgnorePro
函数功能： 确定字符串在字符串的位置（从前面开始搜索）（忽略大小写） 
入口参数：
第    一：字符串
第    二：字符串的长度 
第    三：搜索的字符串 
返    回：如果有该字符串，返回对应的位置；没有，返回NULL 
*********************************************************/
char *indexofstringIgnorePro(char *string, int iaLen, char *found)
{
  char *p;//指向string 
  char *p1;//指向found 
  char *p2;
  char c1;
  char c2;
  int  iStringLen;
  int  iFoundLen;
  iStringLen = iaLen;
  iFoundLen = strlen(found);
  
  if (iStringLen < iFoundLen)
  {
    return NULL;
  }
  else
  if (iStringLen == iFoundLen)
  {
    if(strcmp(string,found)==0)
    {
      return string;
    }
    else
    {
      return NULL;
    }
  }
  else
  {
    for (p = string; p < string + iStringLen; p++)
    {
      for (p1 = found, p2 = p; p1 < found + iFoundLen; p1++)
      {
        /*如果是字母类的*/
        c1 = *p1;
        c2 = *p2;
        if (c1 >= 'a' && c1 <= 'z')
        {
          c1 -= 32;
        }
        if (c2 >= 'a' && c2 <= 'z')
        {
          c2 -= 32;
        }
        if (c1 == c2)
          p2++;
        else
          break;
      }
      /*如果满足条件*/
      if (p1== found + iFoundLen)
        return p;
    }
    return NULL;
  }
}

/********************************************************
函数名：indexofstringPro
函数功能： 确定字符串在字符串的位置 （从前面开始搜索） 
入口参数：
第    一：字符串 
第    二：搜索的字符串 
返    回：如果有该字符串，返回对应的位置；没有，返回NULL 
*********************************************************/
char *indexofstringPro(char *string, int iaLen, char *found)
{
  char *p;//指向string 
  char *p1;//指向found 
  char *p2;
  int  iStringLen;
  int  iFoundLen;
  iStringLen = iaLen;
  iFoundLen = strlen(found);
  
  if (iStringLen < iFoundLen)
  {
    return NULL;
  }
  else
  if (iStringLen == iFoundLen)
  {
    if(strcmp(string,found)==0)
    {
      return string;
    }
    else
    {
      return NULL;
    }
  }
  else
  {
    for (p = string; p < string + iStringLen; p++)
    {
      for (p1 = found, p2 = p; p1 < found + iFoundLen; p1++)
      {
        if (*p1 == *p2)
          p2++;
        else
          break;
      }
      /*如果满足条件*/
      if (p1== found + iFoundLen)
        return p;
    }
    return NULL;
  }
}
