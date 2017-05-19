#include "stdafx.h"
#include "curl_comm.h"
#include "IPProxyPool.h"
#include "IPProxyPoolDlg.h"


//参考了 https://github.com/qiyeboy/IPProxyPool 上的ip代理网址

//http://www.66ip.cn
//http://www.mimiip.com/gngao
//http://www.kuaidaili.com/proxylist
//http://www.ip181.com/daili
//http://www.xicidaili.com
//http://www.ip3366.net

int GetIPProc_1(char* buffer, PROXY_INFO* piList, int *pMaxCt, int page)
{
	int			ret;
	char		url[1024] = { 0 };
	char*		pattern = "</td>\\s*?</tr>\\s*?<tr>\\s*?<td>(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})</td>\\s*?<td>([\\w\\W]*?)</td>\\s*?<td>([\\w\\W]*?)</td>\\s*?<td>([\\w\\W]*?)</td>\\s*?<td>([\\w\\W]*?)</td>";
	char*		buf = NULL;
	char*		p = NULL;
	PROXY_INFO	pi;
	int			ct = 0;

	sprintf(url, "http://www.66ip.cn/%d.html", page);

	ret = get_html_data(url, buffer, GET_HTML_DATA_TIME_OUT, NULL, NULL);
	if (ret)
	{
		LOG_E("get_html_data error");
		return 1;
	}

	buf = (char*)Mem_Malloc(MAX_STR_SIZE);
	if (buf == NULL)
	{
		LOG_E("malloc buf error [%#x]", CF_GetLastError());
		return 2;
	}
	memset(buf, 0, MAX_STR_SIZE);

	ret = reg_proc_comm(buffer, pattern, 5, buf, MAX_STR_SIZE);
	if (ret != 0)
	{
		Mem_Free(buf);
		LOG_E("parse regex error");
		return 3;
	}

	p = buf;
	memset(&pi, 0, sizeof(pi));

	while (*p != '\0')
	{
		strcpy(pi.ip, p);
		p += strlen(p) + 1;
		strcpy(pi.port, p);
		p += strlen(p) + 1;
		strcpy(pi.area, p);
		p += strlen(p) + 1;
		strcpy(pi.type, p);
		p += strlen(p) + 1;
		strcpy(pi.time, p);
		p += strlen(p) + 2;		//最后一个+2

		if (ct > *pMaxCt)
		{
			LOG_E("reach max array ct");
			return 4;
		}
		memcpy(&piList[ct++], &pi, sizeof(PROXY_INFO));
	}

	*pMaxCt = ct;
	Mem_Free(buf);
	return 0;
}

//area 1-32
int GetIPProc_2(char* buffer, PROXY_INFO* piList, int *pMaxCt, int area, int page)
{
	int			ret;
	char		url[1024] = { 0 };
	char*		pattern = "</td>\\s*?</tr>\\s*?<tr>\\s*?<td>(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})</td>\\s*?<td>([\\w\\W]*?)</td>\\s*?<td>([\\w\\W]*?)</td>\\s*?<td>([\\w\\W]*?)</td>\\s*?<td>([\\w\\W]*?)</td>";
	char*		buf = NULL;
	char*		p = NULL;
	PROXY_INFO	pi;
	int			ct = 0;

	sprintf(url, "http://www.66ip.cn/areaindex_%d/%d.html", area, page);

	ret = get_html_data(url, buffer, GET_HTML_DATA_TIME_OUT, NULL, NULL);
	if (ret)
	{
		LOG_E("get_html_data error");
		return 1;
	}

	buf = (char*)Mem_Malloc(MAX_STR_SIZE);
	if (buf == NULL)
	{
		LOG_E("malloc buf error [%#x]", CF_GetLastError());
		return 2;
	}
	memset(buf, 0, MAX_STR_SIZE);

	ret = reg_proc_comm(buffer, pattern, 5, buf, MAX_STR_SIZE);
	if (ret != 0)
	{
		Mem_Free(buf);
		LOG_E("parse regex error");
		return 3;
	}

	p = buf;
	memset(&pi, 0, sizeof(pi));

	while (*p != '\0')
	{
		strcpy(pi.ip, p);
		p += strlen(p) + 1;
		strcpy(pi.port, p);
		p += strlen(p) + 1;
		strcpy(pi.area, p);
		p += strlen(p) + 1;
		strcpy(pi.type, p);
		p += strlen(p) + 1;
		strcpy(pi.time, p);
		p += strlen(p) + 2;		//最后一个+2

		if (ct > *pMaxCt)
		{
			LOG_E("reach max array ct");
			return 4;
		}
		memcpy(&piList[ct++], &pi, sizeof(PROXY_INFO));
	}

	*pMaxCt = ct;
	Mem_Free(buf);
	return 0;
}

int GetIPProc_3(char* buffer, PROXY_INFO* piList, int *pMaxCt, int page)
{
	int			ret;
	char		url[1024] = { 0 };
	char*		pattern = "<tr>\\s*?<td>(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})</td>\\s*?<td>([\\w\\W]*?)</td>\\s*?<td>([\\w\\W]*?)</td>\\s*?<td>([\\w\\W]*?)</td>\\s*?<td>([\\w\\W]*?)</td>\\s*?<td>[\\w\\W]*?</td>\\s*?<td>([\\w\\W]*?)</td>";
	char*		pattern_sub = "<a href=[\\w\\W]*?>([\\w\\W]*?)</a>";
	char*		buf = NULL;
	char*		buffer_gbk = NULL;
	char*		p = NULL;
	PROXY_INFO	pi;
	int			ct = 0;
	char		area_temp[1024] = { 0 };
	char		buf_temp[1024] = { 0 };
	char*		q = NULL;

	sprintf(url, "http://www.mimiip.com/gngao/%d", page);

	ret = get_html_data(url, buffer, GET_HTML_DATA_TIME_OUT, NULL, NULL);
	if (ret)
	{
		LOG_E("get_html_data error");
		return 1;
	}

	buf = (char*)Mem_Malloc(MAX_STR_SIZE);
	if (buf == NULL)
	{
		LOG_E("malloc buf error [%#x]", CF_GetLastError());
		return 2;
	}
	memset(buf, 0, MAX_STR_SIZE);

	buffer_gbk = (char*)Mem_Malloc(MAX_BUFFER_SIZE);
	if (buf == NULL)
	{
		LOG_E("malloc buffer_gbk error [%#x]", CF_GetLastError());
		return 2;
	}
	memset(buffer_gbk, 0, MAX_BUFFER_SIZE);
	if (UTF8ToGBK(buffer, buffer_gbk, MAX_BUFFER_SIZE) == 0)
	{
		LOG_E("convert encode error");
		Mem_Free(buf);
		Mem_Free(buffer_gbk);
		return 1;
	}


	ret = reg_proc_comm(buffer_gbk, pattern, 6, buf, MAX_STR_SIZE);
	if (ret != 0)
	{
		Mem_Free(buf);
		Mem_Free(buffer_gbk);
		LOG_E("parse regex error");
		return 3;
	}

	Mem_Free(buffer_gbk);
	p = buf;
	memset(&pi, 0, sizeof(pi));

	while (*p != '\0')
	{
		strcpy(pi.ip, p);
		p += strlen(p) + 1;
		strcpy(pi.port, p);
		p += strlen(p) + 1;
		strcpy(area_temp, p);
		p += strlen(p) + 1;
		strcpy(pi.type, p);
		p += strlen(p) + 1;
		strcpy(pi.protocol, p);
		p += strlen(p) + 1;
		strcpy(pi.time, p);
		p += strlen(p) + 2;		//最后一个+2

		memset(pi.area, 0, sizeof(pi.area));
		memset(buf_temp, 0, sizeof(buf_temp));
		ret = reg_proc_comm(area_temp, pattern_sub, 1, buf_temp, sizeof(buf_temp));
		if (ret != 0)
		{
			Mem_Free(buf);
			LOG_E("parse regex sub error");
			return 3;
		}
		q = buf_temp;
		while (*q != '\0')
		{
			strcat(pi.area, q);
			q += strlen(q) + 2;
		}

		if (ct > *pMaxCt)
		{
			LOG_E("reach max array ct");
			return 4;
		}
		memcpy(&piList[ct++], &pi, sizeof(PROXY_INFO));
	}

	*pMaxCt = ct;
	Mem_Free(buf);
	return 0;
}

int GetIPProc_4(char* buffer, PROXY_INFO* piList, int *pMaxCt, int page)
{
	int			ret;
	char		url[1024] = { 0 };
	char*		pattern = "<tr>\\s*?<td data-title=\"IP\">(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})</td>\\s*?<td data-title=\"PORT\">([\\w\\W]*?)</td>\\s*?<td [\\w\\W]*?>([\\w\\W]*?)</td>\\s*?<td [\\w\\W]*?>([\\w\\W]*?)</td>\\s*?<td [\\w\\W]*?>[\\w\\W]*?</td>\\s*?<td [\\w\\W]*?>([\\w\\W]*?)</td>\\s*?<td [\\w\\W]*?>([\\w\\W]*?)</td>\\s*?<td [\\w\\W]*?>([\\w\\W]*?)</td>";
	char*		buf = NULL;
	char*		buffer_gbk = NULL;
	char*		p = NULL;
	PROXY_INFO	pi;
	int			ct = 0;

	sprintf(url, "http://www.kuaidaili.com/proxylist/%d", page);

	ret = get_html_data(url, buffer, GET_HTML_DATA_TIME_OUT, NULL, NULL);
	if (ret)
	{
		LOG_E("get_html_data error");
		return 1;
	}

	buf = (char*)Mem_Malloc(MAX_STR_SIZE);
	if (buf == NULL)
	{
		LOG_E("malloc buf error [%#x]", CF_GetLastError());
		return 2;
	}
	memset(buf, 0, MAX_STR_SIZE);
	buffer_gbk = (char*)Mem_Malloc(MAX_BUFFER_SIZE);
	if (buf == NULL)
	{
		LOG_E("malloc buffer_gbk error [%#x]", CF_GetLastError());
		return 2;
	}
	memset(buffer_gbk, 0, MAX_BUFFER_SIZE);
	if (UTF8ToGBK(buffer, buffer_gbk, MAX_BUFFER_SIZE) == 0)
	{
		LOG_E("convert encode error");
		Mem_Free(buf);
		Mem_Free(buffer_gbk);
		return 1;
	}

	ret = reg_proc_comm(buffer_gbk, pattern, 7, buf, MAX_STR_SIZE);
	if (ret != 0)
	{
		Mem_Free(buf);
		Mem_Free(buffer_gbk);
		LOG_E("parse regex error");
		return 3;
	}
	Mem_Free(buffer_gbk);

	p = buf;
	memset(&pi, 0, sizeof(pi));

	while (*p != '\0')
	{
		strcpy(pi.ip, p);
		p += strlen(p) + 1;
		strcpy(pi.port, p);
		p += strlen(p) + 1;
		strcpy(pi.type, p);
		p += strlen(p) + 1;
		strcpy(pi.protocol, p);
		p += strlen(p) + 1;
		strcpy(pi.area, p);
		p += strlen(p) + 1;
		strcpy(pi.delay, p);
		p += strlen(p) + 1;
		strcpy(pi.time, p);
		p += strlen(p) + 2;		//最后一个+2

		if (ct > *pMaxCt)
		{
			LOG_E("reach max array ct");
			return 4;
		}
		memcpy(&piList[ct++], &pi, sizeof(PROXY_INFO));
	}

	*pMaxCt = ct;
	Mem_Free(buf);
	return 0;
}

int GetIPProc_5(char* buffer, PROXY_INFO* piList, int *pMaxCt, int page)
{
	int			ret;
	char		url[1024] = { 0 };
	char*		pattern = "<tr>\\s*?<td>(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})</td>\\s*?<td>([\\w\\W]*?)</td>\\s*?<td>([\\w\\W]*?)</td>\\s*?<td>([\\w\\W]*?)</td>\\s*?<td>([\\w\\W]*?)</td>\\s*?<td>([\\w\\W]*?)</td>\\s*?<td>([\\w\\W]*?)</td>";
	char*		buf = NULL;
	char*		p = NULL;
	PROXY_INFO	pi;
	int			ct = 0;

	sprintf(url, "http://www.ip181.com/daili/%d.html", page);

	ret = get_html_data(url, buffer, GET_HTML_DATA_TIME_OUT, NULL, NULL);
	if (ret)
	{
		LOG_E("get_html_data error");
		return 1;
	}

	buf = (char*)Mem_Malloc(MAX_STR_SIZE);
	if (buf == NULL)
	{
		LOG_E("malloc buf error [%#x]", CF_GetLastError());
		return 2;
	}
	memset(buf, 0, MAX_STR_SIZE);

	ret = reg_proc_comm(buffer, pattern, 7, buf, MAX_STR_SIZE);
	if (ret != 0)
	{
		Mem_Free(buf);
		LOG_E("parse regex error");
		return 3;
	}

	p = buf;
	memset(&pi, 0, sizeof(pi));

	while (*p != '\0')
	{
		strcpy(pi.ip, p);
		p += strlen(p) + 1;
		strcpy(pi.port, p);
		p += strlen(p) + 1;
		strcpy(pi.type, p);
		p += strlen(p) + 1;
		strcpy(pi.protocol, p);
		p += strlen(p) + 1;
		strcpy(pi.delay, p);
		p += strlen(p) + 1;
		strcpy(pi.area, p);
		p += strlen(p) + 1;
		strcpy(pi.time, p);
		p += strlen(p) + 2;		//最后一个+2

		if (ct > *pMaxCt)
		{
			LOG_E("reach max array ct");
			return 4;
		}
		memcpy(&piList[ct++], &pi, sizeof(PROXY_INFO));
	}

	*pMaxCt = ct;
	Mem_Free(buf);
	return 0;
}

int GetIPProc_6_sub(char* buffer, PROXY_INFO* piList, int *pMaxCt, int page, const char* type)
{
	int			ret;
	char		url[1024] = { 0 };
	char*		pattern = "<td>(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})</td>\\s*?<td>([\\w\\W]*?)</td>\\s*?<td>([\\w\\W]*?)</td>\\s*?<td [\\w\\W]*?>([\\w\\W]*?)</td>\\s*?<td>([\\w\\W]*?)</td>\\s*?<td [\\w\\W]*?>\\s*?<div title=\"([\\w\\W]*?)\"[\\w\\W]*?</td>\\s*?<td[\\w\\W]*?>[\\w\\W]*?</td>\\s*?<td>[\\w\\W]*?</td>\\s*?<td>([\\w\\W]*?)</td>";
	char*		pattern_sub = "<a href=[\\w\\W]*?>([\\w\\W]*?)</a>";
	char*		buf = NULL;
	char*		buffer_gbk = NULL;
	char*		p = NULL;
	PROXY_INFO	pi;
	int			ct = 0;
	char		area_temp[1024] = { 0 };
	char		buf_temp[1024] = { 0 };
	char*		q = NULL;

	sprintf(url, "http://www.xicidaili.com/%s/%d", type, page);

	ret = get_html_data(url, buffer, GET_HTML_DATA_TIME_OUT, NULL, NULL);
	if (ret)
	{
		LOG_E("get_html_data error");
		return 1;
	}

	buf = (char*)Mem_Malloc(MAX_STR_SIZE);
	if (buf == NULL)
	{
		LOG_E("malloc buf error [%#x]", CF_GetLastError());
		return 2;
	}
	memset(buf, 0, MAX_STR_SIZE);
	buffer_gbk = (char*)Mem_Malloc(MAX_BUFFER_SIZE);
	if (buf == NULL)
	{
		LOG_E("malloc buffer_gbk error [%#x]", CF_GetLastError());
		return 2;
	}
	memset(buffer_gbk, 0, MAX_BUFFER_SIZE);
	if (UTF8ToGBK(buffer, buffer_gbk, MAX_BUFFER_SIZE) == 0)
	{
		LOG_E("convert encode error");
		Mem_Free(buf);
		Mem_Free(buffer_gbk);
		return 1;
	}

	ret = reg_proc_comm(buffer_gbk, pattern, 7, buf, MAX_STR_SIZE);
	if (ret != 0)
	{
		Mem_Free(buf);
		Mem_Free(buffer_gbk);
		LOG_E("parse regex error");
		return 3;
	}
	Mem_Free(buffer_gbk);

	p = buf;
	memset(&pi, 0, sizeof(pi));

	while (*p != '\0')
	{
		strcpy(pi.ip, p);
		p += strlen(p) + 1;
		strcpy(pi.port, p);
		p += strlen(p) + 1;
		strcpy(area_temp, p);
		p += strlen(p) + 1;
		strcpy(pi.type, p);
		p += strlen(p) + 1;
		strcpy(pi.protocol, p);
		p += strlen(p) + 1;
		strcpy(pi.delay, p);
		p += strlen(p) + 1;
		strcpy(pi.time, p);
		p += strlen(p) + 2;		//最后一个+2

		if (strstr(area_temp, "<a") == 0)
		{
			STR_Trim(area_temp, TRIM_LEFT | TRIM_RIGHT);
			strcpy(pi.area, area_temp);
		}
		else
		{
			memset(pi.area, 0, sizeof(pi.area));
			memset(buf_temp, 0, sizeof(buf_temp));
			ret = reg_proc_comm(area_temp, pattern_sub, 1, buf_temp, sizeof(buf_temp));
			if (ret != 0)
			{
				Mem_Free(buf);
				LOG_E("parse regex sub error");
				return 3;
			}
			q = buf_temp;
			while (*q != '\0')
			{
				strcat(pi.area, q);
				q += strlen(q) + 2;
			}
		}

		if (ct > *pMaxCt)
		{
			LOG_E("reach max array ct");
			return 4;
		}
		memcpy(&piList[ct++], &pi, sizeof(PROXY_INFO));
	}

	*pMaxCt = ct;
	Mem_Free(buf);
	return 0;
}

int GetIPProc_6(char* buffer, PROXY_INFO* piList, int *pMaxCt, int page)
{
	return GetIPProc_6_sub(buffer, piList, pMaxCt, page, "nn");
}

int GetIPProc_7(char* buffer, PROXY_INFO* piList, int *pMaxCt, int page)
{
	return GetIPProc_6_sub(buffer, piList, pMaxCt, page, "nt");
}

int GetIPProc_8(char* buffer, PROXY_INFO* piList, int *pMaxCt, int page)
{
	return GetIPProc_6_sub(buffer, piList, pMaxCt, page, "wn");
}

int GetIPProc_9(char* buffer, PROXY_INFO* piList, int *pMaxCt, int page)
{
	return GetIPProc_6_sub(buffer, piList, pMaxCt, page, "wt");
}

int GetIPProc_10(char* buffer, PROXY_INFO* piList, int *pMaxCt, int page)
{
	int			ret;
	char		url[1024] = { 0 };
	char*		pattern = "<td>(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})</td>\\s*?<td>([\\w\\W]*?)</td>\\s*?<td>([\\w\\W]*?)</td>\\s*?<td>([\\w\\W]*?)</td>\\s*?<td>[\\w\\W]*?</td>\\s*?<td>([\\w\\W]*?)</td>\\s*?<td>([\\w\\W]*?)</td>\\s*?<td>([\\w\\W]*?)</td>";
	char*		buf = NULL;
	char*		p = NULL;
	PROXY_INFO	pi;
	int			ct = 0;

	sprintf(url, "http://www.ip3366.net/?stype=1&page=%d", page);

	ret = get_html_data(url, buffer, GET_HTML_DATA_TIME_OUT, NULL, NULL);
	if (ret)
	{
		LOG_E("get_html_data error");
		return 1;
	}

	buf = (char*)Mem_Malloc(MAX_STR_SIZE);
	if (buf == NULL)
	{
		LOG_E("malloc buf error [%#x]", CF_GetLastError());
		return 2;
	}
	memset(buf, 0, MAX_STR_SIZE);

	ret = reg_proc_comm(buffer, pattern, 7, buf, MAX_STR_SIZE);
	if (ret != 0)
	{
		Mem_Free(buf);
		LOG_E("parse regex error");
		return 3;
	}

	p = buf;
	memset(&pi, 0, sizeof(pi));

	while (*p != '\0')
	{
		strcpy(pi.ip, p);
		p += strlen(p) + 1;
		strcpy(pi.port, p);
		p += strlen(p) + 1;
		strcpy(pi.type, p);
		p += strlen(p) + 1;
		strcpy(pi.protocol, p);
		p += strlen(p) + 1;
		strcpy(pi.area, p);
		p += strlen(p) + 1;
		strcpy(pi.delay, p);
		p += strlen(p) + 1;
		strcpy(pi.time, p);
		p += strlen(p) + 2;		//最后一个+2

		if (ct > *pMaxCt)
		{
			LOG_E("reach max array ct");
			return 4;
		}
		memcpy(&piList[ct++], &pi, sizeof(PROXY_INFO));
	}

	*pMaxCt = ct;
	Mem_Free(buf);
	return 0;
}

int GetIPProc_11(char* buffer, PROXY_INFO* piList, int *pMaxCt, int page)
{
	int			ret;
	char		url[1024] = { 0 };
	char*		pattern = "<td[\\w\\W]*?>(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})</td>\\s*?<td[\\w\\W]*?>([\\w\\W]*?)</td>\\s*?<td[\\w\\W]*?>([\\w\\W]*?)</td>\\s*?<td[\\w\\W]*?>([\\w\\W]*?)</td>\\s*?<td[\\w\\W]*?>([\\w\\W]*?)</td>\\s*?<td[\\w\\W]*?>[\\w\\W]*?</td>\\s*?<td[\\w\\W]*?>[\\w\\W]*?</td>\\s*?<td[\\w\\W]*?>([\\w\\W]*?)</td>";
	char*		buf = NULL;
	char*		p = NULL;
	PROXY_INFO	pi;
	int			ct = 0;

	sprintf(url, "http://www.nianshao.me/?page=%d", page);

	ret = get_html_data(url, buffer, GET_HTML_DATA_TIME_OUT, NULL, NULL);
	if (ret)
	{
		LOG_E("get_html_data error");
		return 1;
	}

	buf = (char*)Mem_Malloc(MAX_STR_SIZE);
	if (buf == NULL)
	{
		LOG_E("malloc buf error [%#x]", CF_GetLastError());
		return 2;
	}
	memset(buf, 0, MAX_STR_SIZE);

	ret = reg_proc_comm(buffer, pattern, 6, buf, MAX_STR_SIZE);
	if (ret != 0)
	{
		Mem_Free(buf);
		LOG_E("parse regex error");
		return 3;
	}

	p = buf;
	memset(&pi, 0, sizeof(pi));

	while (*p != '\0')
	{
		strcpy(pi.ip, p);
		p += strlen(p) + 1;
		strcpy(pi.port, p);
		p += strlen(p) + 1;
		strcpy(pi.area, p);
		p += strlen(p) + 1;
		strcpy(pi.type, p);
		p += strlen(p) + 1;
		strcpy(pi.protocol, p);
		p += strlen(p) + 1;
		strcpy(pi.time, p);
		p += strlen(p) + 2;		//最后一个+2

		if (ct > *pMaxCt)
		{
			LOG_E("reach max array ct");
			return 4;
		}
		memcpy(&piList[ct++], &pi, sizeof(PROXY_INFO));
	}

	*pMaxCt = ct;
	Mem_Free(buf);
	return 0;
}

void WorkProc(WORKINFO* pWorkInfo)
{
	int			nRet;
	PROXY_INFO	piList[1024];
	int			ct = 1024;


	LOG_D("get taskinfo taskid[%d] page[%d] sleeptime[%d]...", pWorkInfo->taskInfo.nTaskID,
		pWorkInfo->taskInfo.nPage, pWorkInfo->taskInfo.nSleepTime);

	memset(piList, 0, sizeof(PROXY_INFO) * 1024);
	ct = 1024;
	memset(pWorkInfo->pBuffer, 0, MAX_BUFFER_SIZE);

	switch (pWorkInfo->taskInfo.nTaskID)
	{
	case 0:
		nRet = GetIPProc_1(pWorkInfo->pBuffer, piList, &ct, pWorkInfo->taskInfo.nPage);
		break;
	case 1:
		nRet = GetIPProc_2(pWorkInfo->pBuffer, piList, &ct, 0, pWorkInfo->taskInfo.nPage);
		break;
	case 2:
		nRet = GetIPProc_3(pWorkInfo->pBuffer, piList, &ct, pWorkInfo->taskInfo.nPage);
		break;
	case 3:
		nRet = GetIPProc_4(pWorkInfo->pBuffer, piList, &ct, pWorkInfo->taskInfo.nPage);
		break;
	case 4:
		nRet = GetIPProc_5(pWorkInfo->pBuffer, piList, &ct, pWorkInfo->taskInfo.nPage);
		break;
	case 5:
		nRet = GetIPProc_6(pWorkInfo->pBuffer, piList, &ct, pWorkInfo->taskInfo.nPage);
		break;
	case 6:
		nRet = GetIPProc_7(pWorkInfo->pBuffer, piList, &ct, pWorkInfo->taskInfo.nPage);
		break;
	case 7:
		nRet = GetIPProc_8(pWorkInfo->pBuffer, piList, &ct, pWorkInfo->taskInfo.nPage);
		break;
	case 8:
		nRet = GetIPProc_9(pWorkInfo->pBuffer, piList, &ct, pWorkInfo->taskInfo.nPage);
		break;
	case 9:
		nRet = GetIPProc_10(pWorkInfo->pBuffer, piList, &ct, pWorkInfo->taskInfo.nPage);
		break;
	default:
		LOG_E("not support taskid[%d]", pWorkInfo->taskInfo.nTaskID);
		return;
	}

	WaitForSingleObject(pWorkInfo->pDlg->m_hMutex_UI, INFINITE);
	if (nRet == 0)
	{
		pWorkInfo->pDlg->m_task_statis[pWorkInfo->taskInfo.nTaskID].nSuccCall++;
	}
	else
	{
		pWorkInfo->pDlg->m_task_statis[pWorkInfo->taskInfo.nTaskID].nFailCall++;
	}
	ReleaseMutex(pWorkInfo->pDlg->m_hMutex_UI);

	if (nRet)
	{
		LOG_E("GetIPProc error");
		return;
	}

	for (int j = 0; j < ct; j++)
	{
// 		LOG_D("ip[%s] port[%s] area[%s] type[%s] protocol[%s] delay[%s] time[%s]",
// 			piList[j].ip, piList[j].port, piList[j].area, piList[j].type, piList[j].protocol, piList[j].delay, piList[j].time);

		piList[j].nTaskID = pWorkInfo->taskInfo.nTaskID;
		WaitForSingleObject(pWorkInfo->pDlg->m_hFullSemp, INFINITE);
		pWorkInfo->pDlg->LockData();
		nRet = AddArrayQueue(pWorkInfo->pDlg->m_Queue, (void *)&piList[j]);
		if (nRet != ERR_OK)
		{
			LOG_W("AddArrayQueue error[%#x]", nRet);
			pWorkInfo->pDlg->UnLockData();
			continue;
		}
		pWorkInfo->pDlg->m_max_get_num++;
		pWorkInfo->pDlg->UnLockData();
		ReleaseSemaphore(pWorkInfo->pDlg->m_hSemp, 1, NULL);
	}

	Sleep(pWorkInfo->taskInfo.nSleepTime);
	
}


/*************************************************************************
 Purpose ：	工作线程处理
 Input   :  parameter	--	工作线程结构指针
 Return  :	None
 Remark  :	
 *************************************************************************/
void WorkThreadProc(void* parameter)
{
	DWORD		dwRet = 0;
	WORKINFO*	pWorkInfo = (WORKINFO*)parameter;

	LOG_D("工作线程[%02d]进入主循环...", pWorkInfo->nWorkID);
	while(pWorkInfo->nWorkCircle)
	{
		//首先更新线程时间
		pWorkInfo->lTime = GetTickCount();
		//等待work事件
		dwRet = WaitForMultipleObjects(3, pWorkInfo->hWorkEvent, FALSE, 500);
		switch (dwRet)
		{
		case WAIT_TIMEOUT:
			break;
		case WAIT_OBJECT_0:
			LOG_D("接收到工作消息，开始工作");
			pWorkInfo->nIsWorking = 1;
			WorkProc(pWorkInfo);
			pWorkInfo->nIsWorking = 0;
			break;
		case WAIT_OBJECT_0+1:
			LOG_D("接收到设备插入消息，开始工作");
			break;
		case WAIT_OBJECT_0+2:
			LOG_D("接收到下电消息");
			break;
		}
	}
	LOG_D("工作线程[%02d]退出主循环...", pWorkInfo->nWorkID);
}