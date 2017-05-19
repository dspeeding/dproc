#include "basetype.h"
#include "log.h"

static int			g_nLogLevel = 1;			//
char				g_szPrefix[MAX_FILE_PATH];	//
char				g_szLogFile[MAX_FILE_PATH];	//

#ifdef _WINDOWS
CRITICAL_SECTION	g_logCS;
#else
pthread_mutex_t		g_mutex;
#endif


/*************************************************************************
@Purpose :	log init
@Param   :	nLogLevel	--	log level
@Param   :	szPrefix	--	prifix
@Param   :	szLogFile	--	log file
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void LOG_Init(IN int nLogLevel, IN const char* szPrefix, IN const char* szLogFile)
{
	if (szLogFile == NULL || szPrefix == NULL)
	{
		return;
	}

	g_nLogLevel = nLogLevel;

	BUFCLR(g_szPrefix);
	strcpy(g_szPrefix, szPrefix);
	strcpy(g_szLogFile, szLogFile);
#ifdef _WINDOWS
	InitializeCriticalSection(&g_logCS);
#else
	pthread_mutex_init(&g_mutex, NULL);
#endif
}

void LOG_UnInit(void)
{
#ifdef _WINDOWS
	DeleteCriticalSection(&g_logCS);
#else
	pthread_mutex_destroy(&g_mutex);
#endif
}

/*************************************************************************
@Purpose :	write log
@Param   :	iLevel		-- log level
@Param   :	szModName	-- module name
@Param   :	format, ...	-- same as sprintf
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void WriteLog(IN int iLevel, IN const char* szModName, IN const char* format, ...)
{
	char		szLogData[8192];
	char		szFileName[MAX_FILE_PATH] = { 0 };
	char*		pStr;
	FILE*		logFp = NULL;
	va_list		args;
	int			nLen = 0;
#ifdef _WINDOWS
	SYSTEMTIME	st;
#else
	time_t		t;
	struct tm	*tp;
#endif

	if (iLevel > g_nLogLevel)
	{
		return;
	}

	BUFCLR(szLogData);
	pStr = szLogData;

	switch (iLevel)
	{
	case LOG_LEV_ERROR:
		pStr += sprintf(pStr, ERROLOG);
		break;
	case LOG_LEV_WARN:
		pStr += sprintf(pStr, WARNLOG);
		break;
	case LOG_LEV_INFO:
		pStr += sprintf(pStr, INFORLOG);
		break;
	case LOG_LEV_DEBUG:
		pStr += sprintf(pStr, DEBUGLOG);
		break;
	default:
		return;
	}

#ifdef _WINDOWS
	GetLocalTime(&st);
	pStr += wsprintfA(pStr, "[%s] [%s] [%04d-%02d-%02d %02d:%02d:%02d.%03d] [%#x] ",
		g_szPrefix, szModName == NULL ? "" : szModName,
		st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, GetCurrentThreadId());
#else
	time(&t);
	tp = localtime(&t);
	pStr += sprintf(pStr, "[%s] [%s] [%04d-%02d-%02d %02d:%02d:%02d] [%u] ", 
		g_szPrefix, szModName==NULL?"":szModName,
		tp->tm_year+1900, tp->tm_mon+1, tp->tm_mday,
		tp->tm_hour, tp->tm_min, tp->tm_sec, (unsigned int)getpid());
#endif
	nLen = strlen(szLogData);

	va_start(args, format);
	vsnprintf(pStr, sizeof(szLogData) - nLen - 1, format, args);
	pStr = szLogData;
	pStr += strlen(szLogData);
	va_end(args);

	*pStr = '\n';

#ifdef _WINDOWS
	sprintf(szFileName, "%s_%04d%02d%02d.log", g_szLogFile, st.wYear, st.wMonth, st.wDay);
	OutputDebugStringA(szLogData);
	EnterCriticalSection(&g_logCS);
#else
	sprintf(szFileName, "%s_%04d%02d%02d.log", g_szLogFile, tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday);
	printf("%s", szLogData);
	pthread_mutex_lock(&g_mutex);
#endif
	logFp = fopen(szFileName, "a");
	if (logFp != NULL)
	{
		fprintf(logFp, "%s", szLogData);
		fflush(logFp);
		fclose(logFp);
	}
#ifdef _WINDOWS
	LeaveCriticalSection(&g_logCS);
#else
	pthread_mutex_unlock(&g_mutex);
#endif
}

/*************************************************************************
@Purpose :	write bin data log
@Param   :	iLevel		-- log level
@Param   :	bData		-- bin data
@Param   :	nDataLen	-- bin data len
@Return  :	None
@Remark  :
*************************************************************************/
void WriteBinLog(IN int iLevel, IN const char* szModName, IN unsigned char* bData, IN int nDataLen)
{
	char		szBCD[8192];
	char		szFileName[MAX_FILE_PATH] = { 0 };
	FILE*		logFp = NULL;
	char*		pStr;
	int			nLen = 0;
#ifdef _WINDOWS
	SYSTEMTIME	st;
#else
	time_t		t;
	struct tm	*tp;
#endif

	if (iLevel > g_nLogLevel)
	{
		return;
	}

	if (bData == NULL || nDataLen <= 0)
	{
		return;
	}

	BUFCLR(szBCD);

	pStr = szBCD;

	switch (iLevel)
	{
	case LOG_LEV_ERROR:
		pStr += sprintf(pStr, ERROLOG);
		break;
	case LOG_LEV_WARN:
		pStr += sprintf(pStr, WARNLOG);
		break;
	case LOG_LEV_INFO:
		pStr += sprintf(pStr, INFORLOG);
		break;
	case LOG_LEV_DEBUG:
		pStr += sprintf(pStr, DEBUGLOG);
		break;
	default:
		return;
	}

#ifdef _WINDOWS
	GetLocalTime(&st);
	pStr += wsprintfA(pStr, "[%s] [%s] [%04d-%02d-%02d %02d:%02d:%02d.%03d] [%#x] ",
		g_szPrefix, szModName == NULL ? "" : szModName,
		st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, GetCurrentThreadId());
#else
	time(&t);
	tp = localtime(&t);
	pStr += sprintf(pStr, "[%s] [%s] [%04d-%02d-%02d %02d:%02d:%02d] [%u] ",
		g_szPrefix, szModName == NULL ? "" : szModName,
		tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday,
		tp->tm_hour, tp->tm_min, tp->tm_sec, (unsigned int)getpid());
#endif
	nLen = strlen(szBCD);

	if (STR_BIN2HEX(bData, nDataLen, sizeof(szBCD)-nLen-1, pStr))
	{
		return;
	}

	szBCD[strlen(szBCD)] = '\n';

#ifdef _WINDOWS
	sprintf(szFileName, "%s_%04d%02d%02d.log", g_szLogFile, st.wYear, st.wMonth, st.wDay);
	OutputDebugStringA(szBCD);
	EnterCriticalSection(&g_logCS);
#else
	sprintf(szFileName, "%s_%04d%02d%02d.log", g_szLogFile, tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday);
	printf("%s", szBCD);
	pthread_mutex_lock(&g_mutex);
#endif
	logFp = fopen(szFileName, "a");
	if (logFp != NULL)
	{
		fprintf(logFp, "%s", szBCD);
		fflush(logFp);
		fclose(logFp);
	}
#ifdef _WINDOWS
	LeaveCriticalSection(&g_logCS);
#else
	pthread_mutex_unlock(&g_mutex);
#endif
}


