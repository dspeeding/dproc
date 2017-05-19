#include "basetype.h"
#include "str_utils.h"

/*************************************************************************
@Purpose :	bin data to hex string
@Param   :	szOriData	--	bin data
@Param   :	nOriLen		--	bin data len
@Param   :	nDesMaxLen	--	target buffer max len
@Param   :	szDest		--	target buffer
@Return  :	0--success  -1--fail
@Modify  :
@Remark  :
*************************************************************************/
int STR_BIN2HEX(IN unsigned char* bOriData, IN int nOriLen, IN int nDesMaxLen, OUT char* szDest)
{
	int i;

	if (bOriData == NULL || szDest == NULL)
	{
		return -1;
	}
	if (nDesMaxLen < 2 * nOriLen)
	{
		return -1;
	}
	const char szTab[] = "0123456789ABCDEF";
	memset(szDest, 0x00, nDesMaxLen);
	for (i = 0; i < nOriLen; i++)
	{
		*szDest++ = szTab[*bOriData >> 4];
		*szDest++ = szTab[*bOriData & 0x0f];
		bOriData++;
	}

	return 0;
}

/*************************************************************************
@Purpose :	hex string to bin data
@Param   :	bufBCD		--	hex string
@Param   :	bufBIN		--	bin data buffer
@Param   :	nBinMaxLen	--	bin data buffer max len
@Return  :	bin data len--success  -1--fail
@Modify  :
@Remark  :
*************************************************************************/
int STR_HEX2BIN(IN const char* bufBCD, IN unsigned char* bufBIN, IN int nBinMaxLen)
{
	int		i;
	int		nHexLen = 0;

	nHexLen = strlen(bufBCD);
	if (nBinMaxLen < nHexLen / 2)
	{
		return -1;
	}

	memset(bufBIN, 0x00, nBinMaxLen);

	for (i = 0; i<nHexLen; i += 2)
	{
		if (bufBCD[i] >= '0' && bufBCD[i] <= '9')
		{
			*bufBIN = ((bufBCD[i] - '0') << 4);
		}
		if (bufBCD[i] >= 'a' && bufBCD[i] <= 'f')
		{
			*bufBIN = ((bufBCD[i] - 'a' + 0x0a) << 4);
		}
		if (bufBCD[i] >= 'A' && bufBCD[i] <= 'F')
		{
			*bufBIN = ((bufBCD[i] - 'A' + 0x0a) << 4);
		}

		if (bufBCD[i + 1] >= '0' && bufBCD[i + 1] <= '9')
		{
			*bufBIN += (bufBCD[i + 1] - '0');
		}
		if (bufBCD[i + 1] >= 'a' && bufBCD[i + 1] <= 'f')
		{
			*bufBIN += (bufBCD[i + 1] - 'a' + 0x0a);
		}
		if (bufBCD[i + 1] >= 'A' && bufBCD[i + 1] <= 'F')
		{
			*bufBIN += (bufBCD[i + 1] - 'A' + 0x0a);
		}

		bufBIN++;

	}

	return (nHexLen / 2);
}

/*************************************************************************
@Purpose :	reverse string
@Param   :	data			--	string
@Param   :	len				--	string len
@Return  :	void
@Modify  :
@Remark  :	abcd   ->   dcba
*************************************************************************/
void STR_Reverse(INANDOUT unsigned char *data, IN int len)
{
	unsigned char	a;
	int				l, c, b;

	l = len / 2;
	for (b = 0, c = len - 1; b < l; b++, c--)
	{
		a = data[b];
		data[b] = data[c];
		data[c] = a;
	}
}

/*************************************************************************
@Purpose :	trim string
@Param   :	szSrc				--	string
@Param   :	nFlag				-- trim format
@Return  :	szSrc
@Modify  :
@Remark  :	nFlag
			1--TRIM_LEFT	Trim left
			2--TRIM_RIGHT	Trim right
			4--TRIM_ALL		Trim all
*************************************************************************/
char* STR_Trim(INANDOUT char* szSrc, IN int nFlag)
{
	char*	p;
	char*	q;
	char*	r;
	char*	pBuf = NULL;

	pBuf = (char*)Mem_Malloc(strlen(szSrc) + 1);
	if (pBuf == NULL)
	{
		return NULL;
	}
	memset(pBuf, 0, strlen(szSrc) + 1);

	p = szSrc;
	q = pBuf;

	if ((nFlag & TRIM_LEFT) == TRIM_LEFT)
	{
		//Trim left
		while (*p != '\0')
		{
			if (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')
			{
				p++;
			}
			else
			{
				break;
			}
		}
	}

	if (nFlag == TRIM_ALL)
	{
		while (*p != '\0')
		{
			if (*p != ' ' && *p != '\n' && *p != '\r')
			{
				*q++ = *p++;
			}
			else
			{
				p++;
			}
		}
	}
	else
	{
		if ((nFlag & TRIM_RIGHT) == TRIM_RIGHT)
		{
			//trim right
			r = szSrc + strlen(szSrc) - 1;
			while (r >= p)
			{
				if (*r == ' ' || *r == '\n' || *r == '\r' || *r == '\t')
				{
					*r = '\0';
					r--;
				}
				else
				{
					break;
				}
			}
		}
		while (*p != '\0')
		{
			*q++ = *p++;
		}
	}

	memset(szSrc, 0, strlen(szSrc));
	strcpy(szSrc, pBuf);
	Mem_Free(pBuf);
	return szSrc;
}

/*************************************************************************
@Purpose :	get current time to str
@Param   :	szTime		-- return time str
@Param   :	nMaxLen		-- max data buffer len
@Param   :	nFlag		-- format
@Return  :	szTime
@Modify  :
@Remark  :	nFlag
			1-- YYYY-MM-DD HH:mm:SS
			2-- YYYY/MM/DD HH:mm:SS
			3-- YYYYMMDDHHmmSS
*************************************************************************/
char* STR_GetCurTimeStr(OUT char* szTime, IN int nMaxLen, IN int nFlag)
{
#ifdef _WINDOWS
	SYSTEMTIME	st;
#else
	time_t		t;
	struct tm	*tp;
#endif

#ifdef _WINDOWS
	GetLocalTime(&st);
	switch (nFlag)
	{
	case 1:
		snprintf(szTime, nMaxLen, "%04d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		break;
	case 2:
		snprintf(szTime, nMaxLen, "%04d/%02d/%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		break;
	case 3:
		snprintf(szTime, nMaxLen, "%04d%02d%02d%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		break;
	}

#else
	time(&t);
	tp = localtime(&t);
	switch (nFlag)
	{
	case 1:
		strftime(szTime, nMaxLen, "%Y-%m-%d %H:%M:%S", tp);
		//snprintf(szTime, nMaxLen, "%04d-%02d-%02d %02d:%02d:%02d", tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday, tp->tm_hour, tp->tm_min, tp->tm_sec);
		break;
	case 2:
		strftime(szTime, nMaxLen, "%Y/%m/%d %H:%M:%S", tp);
		//snprintf(szTime, nMaxLen, "%04d/%02d/%02d %02d:%02d:%02d", tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday, tp->tm_hour, tp->tm_min, tp->tm_sec);
		break;
	case 3:
		strftime(szTime, nMaxLen, "%Y%m%d%H%M%S", tp);
		//snprintf(szTime, nMaxLen, "%04d%02d%02d%02d%02d%02d", tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday, tp->tm_hour, tp->tm_min, tp->tm_sec);
		break;
	}
#endif

	return szTime;
}


/*************************************************************************
@Purpose :	get current str to time
@Param   :	szTime		-- return time str
@Param   :	nFlag		-- format
@Return  :	time_t
@Modify  :
@Remark  :	nFlag
			1-- YYYY-MM-DD HH:mm:SS
			2-- YYYY/MM/DD HH:mm:SS
			3-- YYYYMMDDHHmmSS
*************************************************************************/
time_t STR_Str2Time(IN char* szTime, IN int nFlag)
{
	struct tm	tm_;
	time_t		t;

#ifdef _WINDOWS
	memset(&tm_, 0, sizeof(tm_));
	switch (nFlag)
	{
	case 1:
		sscanf(szTime, "%04d-%02d-%02d %02d:%02d:%02d", &tm_.tm_year, &tm_.tm_mon, &tm_.tm_mday,
			&tm_.tm_hour, &tm_.tm_min, &tm_.tm_sec);
		break;
	case 2:
		sscanf(szTime, "%04d/%02d/%02d %02d:%02d:%02d", &tm_.tm_year, &tm_.tm_mon, &tm_.tm_mday,
			&tm_.tm_hour, &tm_.tm_min, &tm_.tm_sec);
		break;
	case 3:
		sscanf(szTime, "%04d%02d%02d%02d%02d%02d", &tm_.tm_year, &tm_.tm_mon, &tm_.tm_mday,
			&tm_.tm_hour, &tm_.tm_min, &tm_.tm_sec);
		break;
	default:
		return 0;
	}
	tm_.tm_year -= 1900;
	tm_.tm_mon -= 1;
	tm_.tm_isdst = -1;

	t = mktime(&tm_);
	return t;
#else
	switch (nFlag)
	{
	case 1:
		strptime(szTime, "%Y-%m-%d %H:%M:%S", &tm_);
		break;
	case 2:
		strptime(szTime, "%Y/%m/%d %H:%M:%S", &tm_);
		break;
	case 3:
		strptime(szTime, "%Y%m%d%H%M%S", &tm_);
		break;
	default:
		return 0;
	}
	tm_.tm_isdst = -1;
	t = mktime(&tm_);

	return t;
#endif
}
