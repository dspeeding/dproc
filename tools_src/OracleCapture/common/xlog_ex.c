#include "basetype.h"

pthread_mutex_t	g_mutex;
static int		g_nLogLevel = 1;
char			g_szPrefix[MAX_FILE_PATH];
char			g_szLogFile[MAX_FILE_PATH];

int get_log_lev(char* base_dir, int* plog_lev)
{
	FILE*	fp;
	char	filename[MAX_FILE_PATH] = { 0 };
	char	line[1024] = { 0 };
	char*	p = NULL;

	sprintf(filename, "%s/etc/xlog.ini", base_dir);
	fp = fopen(filename, "r");
	if (fp == NULL)
	{
		fprintf(stderr, "open file [%s] err[%s]", filename, strerror(errno));
		return ERR_COMM_OPEN_FILE;
	}

	while (!feof(fp))
	{
		fgets(line, sizeof(line), fp);
		p = strstr(line, "rootLogger=");
		if (p == NULL)
		{
			continue;
		}
		if (strstr(line, "DEBUG1") != NULL)
		{
			*plog_lev = LOG_LEV_DEBUG;
		}
		else if (strstr(line, "DEBUG2") != NULL)
		{
			*plog_lev = LOG_LEV_MORE;
		}
		else if (strstr(line, "INFO") != NULL)
		{
			*plog_lev = LOG_LEV_INFO;
		}
		else if (strstr(line, "WARN") != NULL)
		{
			*plog_lev = LOG_LEV_ERROR;
		}
		else if (strstr(line, "ERROR") != NULL)
		{
			*plog_lev = LOG_LEV_ERROR;
		}
		else
		{
			*plog_lev = LOG_LEV_DEBUG;
		}
		break;
	}
	fclose(fp);
	return ERR_OK;
}

void LOG_Init(int nLogLevel, const char* szPrefix, const char* szLogFile)
{
	if (szLogFile == NULL || szPrefix == NULL)
	{
		return;
	}
	g_nLogLevel = nLogLevel;

	BUFCLR(g_szPrefix);
	strcpy(g_szPrefix, szPrefix);
	strcpy(g_szLogFile, szLogFile);

	pthread_mutex_init(&g_mutex, NULL);
}

void LOG_UnInit()
{
	pthread_mutex_destroy(&g_mutex);
}

void xlog_error(const char* file, int line, const char* format, ...)
{
	char		szLogData[1024 * 1024];
	char		szFileName[MAX_FILE_PATH] = { 0 };
	char*		pStr;
	FILE*		logFp = NULL;
	va_list		args;
	int			nLen = 0;
	time_t		t;
	struct tm	*tp;
	int			iLevel = 0;

	iLevel = LOG_LEV_ERROR;
	if (iLevel > g_nLogLevel)
	{
		return;
	}

	BUFCLR(szLogData);
	pStr = szLogData;

	switch (iLevel)
	{
	case LOG_LEV_ERROR:
		pStr += sprintf(pStr, ERRLOG);
		break;
	case LOG_LEV_WARN:
		pStr += sprintf(pStr, WARNLOG);
		break;
	case LOG_LEV_INFO:
		pStr += sprintf(pStr, INFOLOG);
		break;
	case LOG_LEV_DEBUG:
		pStr += sprintf(pStr, DEBUGLOG);
		break;
	case LOG_LEV_MORE:
		pStr += sprintf(pStr, MORELOG);
		break;
	default:
		return;
	}

	time(&t);
	tp = localtime(&t);
	pStr += sprintf(pStr, "[%s] [%04d-%02d-%02d %02d:%02d:%02d] [%u] ",
		g_szPrefix, tp->tm_year+1900, tp->tm_mon+1, tp->tm_mday,
		tp->tm_hour, tp->tm_min, tp->tm_sec, (unsigned int)getpid());
	nLen = strlen(szLogData);

	va_start(args, format);
	vsnprintf(pStr, sizeof(szLogData) - nLen, format, args);
	va_end(args);

	nLen = strlen(szLogData);
	pStr = szLogData;
	pStr += nLen;

	snprintf(pStr, sizeof(szLogData)-nLen, "\t[%s][%d]\n", file, line);

	sprintf(szFileName, "%s_%04d%02d%02d.log", g_szLogFile, tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday);
	printf("%s", szLogData);
	pthread_mutex_lock(&g_mutex);

	logFp = fopen(szFileName, "a");
	if (logFp != NULL)
	{
		fprintf(logFp, "%s", szLogData);
		fflush(logFp);
		fclose(logFp);
	}

	pthread_mutex_unlock(&g_mutex);
	 
	if (iLevel == LOG_LEV_ERROR)
	{
		sprintf(szFileName, "%04d%02d%02d.log", tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday);
		logFp = fopen(szFileName, "a");
		if (logFp != NULL)
		{
			fprintf(logFp, "%s", szLogData);
			fflush(logFp);
			fclose(logFp);
		}
	}
}

void xlog_debug1(const char* format, ...)
{
	char		szLogData[1024 * 1024];
	char		szFileName[MAX_FILE_PATH] = { 0 };
	char*		pStr;
	FILE*		logFp = NULL;
	va_list		args;
	int			nLen = 0;
	time_t		t;
	struct tm	*tp;
	int			iLevel = 0;

	iLevel = LOG_LEV_DEBUG;
	if (iLevel > g_nLogLevel)
	{
		return;
	}

	BUFCLR(szLogData);
	pStr = szLogData;

	switch (iLevel)
	{
	case LOG_LEV_ERROR:
		pStr += sprintf(pStr, ERRLOG);
		break;
	case LOG_LEV_WARN:
		pStr += sprintf(pStr, WARNLOG);
		break;
	case LOG_LEV_INFO:
		pStr += sprintf(pStr, INFOLOG);
		break;
	case LOG_LEV_DEBUG:
		pStr += sprintf(pStr, DEBUGLOG);
		break;
	case LOG_LEV_MORE:
		pStr += sprintf(pStr, MORELOG);
		break;
	default:
		return;
	}

	time(&t);
	tp = localtime(&t);
	pStr += sprintf(pStr, "[%s] [%04d-%02d-%02d %02d:%02d:%02d] [%u] ",
		g_szPrefix, tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday,
		tp->tm_hour, tp->tm_min, tp->tm_sec, (unsigned int)getpid());
	nLen = strlen(szLogData);

	va_start(args, format);
	vsnprintf(pStr, sizeof(szLogData) - nLen, format, args);
	va_end(args);

	nLen = strlen(szLogData);
	pStr = szLogData;
	pStr += nLen;
	*pStr = '\n';

	sprintf(szFileName, "%s_%04d%02d%02d.log", g_szLogFile, tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday);
	printf("%s", szLogData);
	pthread_mutex_lock(&g_mutex);

	logFp = fopen(szFileName, "a");
	if (logFp != NULL)
	{
		fprintf(logFp, "%s", szLogData);
		fflush(logFp);
		fclose(logFp);
	}

	pthread_mutex_unlock(&g_mutex);

	if (iLevel == LOG_LEV_ERROR)
	{
		sprintf(szFileName, "%04d%02d%02d.log", tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday);
		logFp = fopen(szFileName, "a");
		if (logFp != NULL)
		{
			fprintf(logFp, "%s", szLogData);
			fflush(logFp);
			fclose(logFp);
		}
	}
}

void xlog_debug2(const char* format, ...)
{
	char		szLogData[1024 * 1024];
	char		szFileName[MAX_FILE_PATH] = { 0 };
	char*		pStr;
	FILE*		logFp = NULL;
	va_list		args;
	int			nLen = 0;
	time_t		t;
	struct tm	*tp;
	int			iLevel = 0;

	iLevel = LOG_LEV_MORE;
	if (iLevel > g_nLogLevel)
	{
		return;
	}

	BUFCLR(szLogData);
	pStr = szLogData;

	switch (iLevel)
	{
	case LOG_LEV_ERROR:
		pStr += sprintf(pStr, ERRLOG);
		break;
	case LOG_LEV_WARN:
		pStr += sprintf(pStr, WARNLOG);
		break;
	case LOG_LEV_INFO:
		pStr += sprintf(pStr, INFOLOG);
		break;
	case LOG_LEV_DEBUG:
		pStr += sprintf(pStr, DEBUGLOG);
		break;
	case LOG_LEV_MORE:
		pStr += sprintf(pStr, MORELOG);
		break;
	default:
		return;
	}

	time(&t);
	tp = localtime(&t);
	pStr += sprintf(pStr, "[%s] [%04d-%02d-%02d %02d:%02d:%02d] [%u] ",
		g_szPrefix, tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday,
		tp->tm_hour, tp->tm_min, tp->tm_sec, (unsigned int)getpid());
	nLen = strlen(szLogData);

	va_start(args, format);
	vsnprintf(pStr, sizeof(szLogData) - nLen, format, args);
	va_end(args);

	nLen = strlen(szLogData);
	pStr = szLogData;
	pStr += nLen;
	*pStr = '\n';

	sprintf(szFileName, "%s_%04d%02d%02d.log", g_szLogFile, tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday);
	printf("%s", szLogData);
	pthread_mutex_lock(&g_mutex);

	logFp = fopen(szFileName, "a");
	if (logFp != NULL)
	{
		fprintf(logFp, "%s", szLogData);
		fflush(logFp);
		fclose(logFp);
	}

	pthread_mutex_unlock(&g_mutex);

	if (iLevel == LOG_LEV_ERROR)
	{
		sprintf(szFileName, "%04d%02d%02d.log", tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday);
		logFp = fopen(szFileName, "a");
		if (logFp != NULL)
		{
			fprintf(logFp, "%s", szLogData);
			fflush(logFp);
			fclose(logFp);
		}
	}
}

void xlog_info(const char* format, ...)
{
	char		szLogData[1024 * 1024];
	char		szFileName[MAX_FILE_PATH] = { 0 };
	char*		pStr;
	FILE*		logFp = NULL;
	va_list		args;
	int			nLen = 0;
	time_t		t;
	struct tm	*tp;
	int			iLevel = 0;

	iLevel = LOG_LEV_INFO;
	if (iLevel > g_nLogLevel)
	{
		return;
	}

	BUFCLR(szLogData);
	pStr = szLogData;

	switch (iLevel)
	{
	case LOG_LEV_ERROR:
		pStr += sprintf(pStr, ERRLOG);
		break;
	case LOG_LEV_WARN:
		pStr += sprintf(pStr, WARNLOG);
		break;
	case LOG_LEV_INFO:
		pStr += sprintf(pStr, INFOLOG);
		break;
	case LOG_LEV_DEBUG:
		pStr += sprintf(pStr, DEBUGLOG);
		break;
	case LOG_LEV_MORE:
		pStr += sprintf(pStr, MORELOG);
		break;
	default:
		return;
	}

	time(&t);
	tp = localtime(&t);
	pStr += sprintf(pStr, "[%s] [%04d-%02d-%02d %02d:%02d:%02d] [%u] ",
		g_szPrefix, tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday,
		tp->tm_hour, tp->tm_min, tp->tm_sec, (unsigned int)getpid());
	nLen = strlen(szLogData);

	va_start(args, format);
	vsnprintf(pStr, sizeof(szLogData) - nLen, format, args);
	va_end(args);

	nLen = strlen(szLogData);
	pStr = szLogData;
	pStr += nLen;
	*pStr = '\n';

	sprintf(szFileName, "%s_%04d%02d%02d.log", g_szLogFile, tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday);
	printf("%s", szLogData);
	pthread_mutex_lock(&g_mutex);

	logFp = fopen(szFileName, "a");
	if (logFp != NULL)
	{
		fprintf(logFp, "%s", szLogData);
		fflush(logFp);
		fclose(logFp);
	}

	pthread_mutex_unlock(&g_mutex);

	if (iLevel == LOG_LEV_ERROR)
	{
		sprintf(szFileName, "%04d%02d%02d.log", tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday);
		logFp = fopen(szFileName, "a");
		if (logFp != NULL)
		{
			fprintf(logFp, "%s", szLogData);
			fflush(logFp);
			fclose(logFp);
		}
	}
}