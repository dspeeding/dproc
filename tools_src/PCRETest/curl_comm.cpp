#include "stdafx.h"
#include "basetype.h"
#include "curl/curl.h"
#define PCRE_STATIC
#include "pcre.h"
#include "curl_comm.h"

char*	g_user_agent[] = {
	"Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.116 Safari/537.36",
	"Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; AcooBrowser; .NET CLR 1.1.4322; .NET CLR 2.0.50727)",
	"Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.0; Acoo Browser; SLCC1; .NET CLR 2.0.50727; Media Center PC 5.0; .NET CLR 3.0.04506)",
	"Mozilla/4.0 (compatible; MSIE 7.0; AOL 9.5; AOLBuild 4337.35; Windows NT 5.1; .NET CLR 1.1.4322; .NET CLR 2.0.50727)",
	"Mozilla/5.0 (Windows; U; MSIE 9.0; Windows NT 9.0; en-US)",
	"Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Win64; x64; Trident/5.0; .NET CLR 3.5.30729; .NET CLR 3.0.30729; .NET CLR 2.0.50727; Media Center PC 6.0)",
	"Mozilla/5.0 (compatible; MSIE 8.0; Windows NT 6.0; Trident/4.0; WOW64; Trident/4.0; SLCC2; .NET CLR 2.0.50727; .NET CLR 3.5.30729; .NET CLR 3.0.30729; .NET CLR 1.0.3705; .NET CLR 1.1.4322)",
	"Mozilla/4.0 (compatible; MSIE 7.0b; Windows NT 5.2; .NET CLR 1.1.4322; .NET CLR 2.0.50727; InfoPath.2; .NET CLR 3.0.04506.30)",
	"Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN) AppleWebKit/523.15 (KHTML, like Gecko, Safari/419.3) Arora/0.3 (Change: 287 c9dfb30)",
	"Mozilla/5.0 (X11; U; Linux; en-US) AppleWebKit/527+ (KHTML, like Gecko, Safari/419.3) Arora/0.6",
	"Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.1.2pre) Gecko/20070215 K-Ninja/2.1.1",
	"Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.9) Gecko/20080705 Firefox/3.0 Kapiko/3.0",
	"Mozilla/5.0 (X11; Linux i686; U;) Gecko/20070322 Kazehakase/0.4.5",
	"Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9.0.8) Gecko Fedora/1.9.0.8-1.fc10 Kazehakase/0.5.6",
	"Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/535.11 (KHTML, like Gecko) Chrome/17.0.963.56 Safari/535.11",
	"Mozilla/5.0 (Macintosh; Intel Mac OS X 10_7_3) AppleWebKit/535.20 (KHTML, like Gecko) Chrome/19.0.1036.7 Safari/535.20",
	"Opera/9.80 (Macintosh; Intel Mac OS X 10.6.8; U; fr) Presto/2.9.168 Version/11.52",
	"Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/536.11 (KHTML, like Gecko) Chrome/20.0.1132.11 TaoBrowser/2.0 Safari/536.11",
	"Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.1 (KHTML, like Gecko) Chrome/21.0.1180.71 Safari/537.1 LBBROWSER",
	"Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0; SLCC2; .NET CLR 2.0.50727; .NET CLR 3.5.30729; .NET CLR 3.0.30729; Media Center PC 6.0; .NET4.0C; .NET4.0E; LBBROWSER)",
	"Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; QQDownload 732; .NET4.0C; .NET4.0E; LBBROWSER)",
	"Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/535.11 (KHTML, like Gecko) Chrome/17.0.963.84 Safari/535.11 LBBROWSER",
	"Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.1; WOW64; Trident/5.0; SLCC2; .NET CLR 2.0.50727; .NET CLR 3.5.30729; .NET CLR 3.0.30729; Media Center PC 6.0; .NET4.0C; .NET4.0E)",
	"Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0; SLCC2; .NET CLR 2.0.50727; .NET CLR 3.5.30729; .NET CLR 3.0.30729; Media Center PC 6.0; .NET4.0C; .NET4.0E; QQBrowser/7.0.3698.400)",
	"Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; QQDownload 732; .NET4.0C; .NET4.0E)",
	"Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; Trident/4.0; SV1; QQDownload 732; .NET4.0C; .NET4.0E; 360SE)",
	"Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; QQDownload 732; .NET4.0C; .NET4.0E)",
	"Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.1; WOW64; Trident/5.0; SLCC2; .NET CLR 2.0.50727; .NET CLR 3.5.30729; .NET CLR 3.0.30729; Media Center PC 6.0; .NET4.0C; .NET4.0E)",
	"Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.1 (KHTML, like Gecko) Chrome/21.0.1180.89 Safari/537.1",
	"Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.1 (KHTML, like Gecko) Chrome/21.0.1180.89 Safari/537.1",
	"Mozilla/5.0 (iPad; U; CPU OS 4_2_1 like Mac OS X; zh-cn) AppleWebKit/533.17.9 (KHTML, like Gecko) Version/5.0.2 Mobile/8C148 Safari/6533.18.5",
	"Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:2.0b13pre) Gecko/20110307 Firefox/4.0b13pre",
	"Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:16.0) Gecko/20100101 Firefox/16.0",
	"Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.11 (KHTML, like Gecko) Chrome/23.0.1271.64 Safari/537.11",
	"Mozilla/5.0 (X11; U; Linux x86_64; zh-CN; rv:1.9.2.10) Gecko/20100922 Ubuntu/10.10 (maverick) Firefox/3.6.10"
};

#define MAX_USER_AGENT_NUM	35

int curl_init(void)
{
	CURLcode	res;

	res = curl_global_init(CURL_GLOBAL_ALL);
	if (res != CURLE_OK)
	{
		LOG_E("curl_global_init error[%d]", res);
		return 1;
	}

	return 0;
}

void curl_uninit(void)
{
	curl_global_cleanup();
}

int UTF8ToGBK(char* utf8str, char* gbkstr, int gbkstr_maxlen)
{
	WCHAR*	uStr = NULL;
	int		retlen = 0;

	if (utf8str == NULL)
	{
		return 0;
	}

	retlen = MultiByteToWideChar(CP_UTF8, 0, (char*)utf8str, -1, NULL, 0);		//先获取所需长度
	uStr = (WCHAR*)Mem_Malloc((retlen + 1) * sizeof(WCHAR));
	if (uStr == NULL)
	{
		LOG_E("malloc uStr error,[%#x]", CF_GetLastError());
		return 0;
	}

	retlen = MultiByteToWideChar(CP_UTF8, 0, utf8str, -1, uStr, retlen);
	if (retlen == 0)
	{
		LOG_E("convert utf8 to unicode error,[%#x]", CF_GetLastError());
		return 0;
	}

	retlen = WideCharToMultiByte(CP_ACP, 0, uStr, -1, NULL, 0, NULL, NULL);
	if (gbkstr == NULL)
	{
		//只返回大小
		Mem_Free(uStr);
		return retlen;
	}
	if (gbkstr_maxlen < retlen)
	{
		LOG_E("buffer len too small, act=[%d] excp[%d]", retlen, gbkstr_maxlen);
		Mem_Free(uStr);
		return 0;
	}

	retlen = WideCharToMultiByte(CP_ACP, 0, uStr, -1, gbkstr, retlen, NULL, NULL);
	Mem_Free(uStr);

	return retlen;
}

size_t write_data(void* buffer, size_t size, size_t nmemb, void* userp)
{
	char	*p = NULL;
	char	*pBuffer;
	size_t	left_len = 0;
	int		temp_len = 0;

	pBuffer = (char *)userp;

	p = pBuffer + strlen(pBuffer);
	memcpy(p, buffer, size*nmemb);

	return size*nmemb;
}

int reg_proc_comm(char* content, char* pattern, int region_num, char* buf, int buf_maxlen)
{
	int			ct;
	int			i = 0;
	pcre		*re;
	const char	*error;
	int			erroffset;
	int			ret;
	int			ovector[1000];
	char*		substr_start;
	int			substr_len;
	char*		p = NULL;
	char*		q = NULL;

	ct = 0;
	re = pcre_compile(pattern, 0, &error, &erroffset, NULL);
	if (re == NULL)
	{
		LOG_E("PCRE compile error,at offset [%d:%s]", erroffset, error);
		return 2;
	}

	p = content;
	q = buf;

	memset(buf, 0, buf_maxlen);
	while (1)
	{
		ret = pcre_exec(re, NULL, p, strlen(p), 0, 0, ovector, 1000);
		if (ret < 0)
		{
			if (ret == PCRE_ERROR_NOMATCH)
			{
				//LOG_E("src not match...");
				break;
			}
			pcre_free(re);
			LOG_E("match error [%d]", ret);
			return 2;
		}

		if (ret != region_num + 1)
		{
			pcre_free(re);
			LOG_E("parse error, ret=[%d]", ret);
			return 2;
		}
		ct++;
		//分别取出捕获分组 $0整个正则公式 $1第一个()    
		//标题
		i = 1;
		while (i <= region_num)
		{
			substr_start = p + ovector[2 * i];
			substr_len = ovector[2 * i + 1] - ovector[2 * i];
			memcpy(q, substr_start, substr_len);
			q += substr_len;
			*q++ = '\0';
			i++;
		}
		*q++ = '\0';

		p = substr_start + substr_len;
	}

	pcre_free(re);
	if (ct == 0)
	{
		LOG_E("some thing was wrong with the html text");
		return 3;
	}

	return 0;
}

int get_html_data(char* url, char* buffer, int timeout, char* cookie, char* ipport)
{
	CURL		*curl;
	CURLcode	res;
	long		code = 0;
	int			user_agent_index = 0;

	curl = curl_easy_init();
	if (curl == NULL)
	{
		LOG_E("err curl_easy_init");
		return 1;
	}

	if (strncmp(url, "https", 5) == 0)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, FALSE);//设定为不验证证书和HOST
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, FALSE);
	}
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout); // 设置超时时间为3秒
	curl_easy_setopt(curl, CURLOPT_POST, 0); // 非post请求
	if (ipport != NULL)
	{
		curl_easy_setopt(curl, CURLOPT_PROXY, ipport);
	}
	if (cookie != NULL)
	{
		curl_easy_setopt(curl, CURLOPT_COOKIE, cookie);
	}
	curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "cookie.txt");
	user_agent_index = rand() % MAX_USER_AGENT_NUM;
	curl_easy_setopt(curl, CURLOPT_USERAGENT, g_user_agent[user_agent_index]); //设置user-agent防止服务器对user-agent验证
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);
	/* Perform the request, res will get the return code */
	res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		//LOG_E("curl_easy_perform() failed: %s", curl_easy_strerror(res));
		curl_easy_cleanup(curl);
		return 1;
	}

	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code); // 获取http status，如果为200则输出网页内容
	if (code != 200) {
		//		LOG_E("get code is not 200[%d]", code);
		curl_easy_cleanup(curl);
		return 1;
	}

	/* always cleanup */
	curl_easy_cleanup(curl);

	//开始解析网页内容
	// 	if (UTF8ToGBK(g_buff, g_buff_gbk, MAX_BUFFER_SIZE) == 0)
	// 	{
	// 		LOG_E("convert encode error");
	// 		return 1;
	// 	}
	// 
	// 	printf("%s", g_buff_gbk);
	FILE* fp = fopen("html_data.txt", "w");
	fprintf(fp, "%s", buffer);
	fclose(fp);

	return 0;
}

static long getDownLoadFileLen(char* url)
{
	CURL		*curl;
	CURLcode	res;
	double		len = 0;


	curl = curl_easy_init();
	if (curl == NULL)
	{
		LOG_E("err curl_easy_init");
		return -1;
	}

	if (strncmp(url, "https", 5) == 0)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, FALSE);//设定为不验证证书和HOST
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, FALSE);
	}

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_HEADER, 1);
	curl_easy_setopt(curl, CURLOPT_NOBODY, 1);

	res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		LOG_E("curl_easy_perform() failed: %s", curl_easy_strerror(res));
		curl_easy_cleanup(curl);
		return -1;
	}

	res = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &len);
	if (res != CURLE_OK)
	{
		LOG_E("curl_easy_getinfo() failed: %s", curl_easy_strerror(res));
		curl_easy_cleanup(curl);
		return -1;
	}

	curl_easy_cleanup(curl);
	return (long)len;
}

static int down_prog_func(void* ptr, double totalToDownload, double nowDownloaded, double totalToUpLoad, double nowUpLoad)
{
	int					per = 0;
	CC_THR_SEG_INFO*	pctsi;

	pctsi = (CC_THR_SEG_INFO*)ptr;

	if (totalToDownload > 0)
	{
		per = (int)(nowDownloaded * 100 / totalToDownload);
	}

	if ((per > 0) && (per % 20) == 0 && per != pctsi->pre_per)
	{
		pctsi->pre_per = per;
		LOG_D("thrd[%d] download [%d%%]", pctsi->id, per);
	}

	return 0;
}

static size_t down_write_func(void* ptr, size_t size, size_t nmemb, void* userdata)
{
	CC_THR_SEG_INFO*	pctsi;
	int					written = 0;

	pctsi = (CC_THR_SEG_INFO*)userdata;

	if (pctsi->flag == 0)
	{
		written = fwrite(ptr, size, nmemb, pctsi->fp);
		return written;
	}

	WaitForSingleObject(pctsi->hMutex, INFINITE);
	if ((pctsi->start_pos + (long)(size*nmemb)) <= pctsi->end_pos)
	{
		fseek(pctsi->fp, pctsi->start_pos, SEEK_SET);
		written = fwrite(ptr, size, nmemb, pctsi->fp);
		pctsi->start_pos += size*nmemb;
	}
	else
	{
		fseek(pctsi->fp, pctsi->start_pos, SEEK_SET);
		written = fwrite(ptr, 1, pctsi->end_pos - pctsi->start_pos + 1, pctsi->fp);
		pctsi->start_pos = pctsi->end_pos;
	}
	ReleaseMutex(pctsi->hMutex);

	return written;
}

static int WorkThreadProc(void* param)
{
	CC_THR_SEG_INFO*	pctsi;
	CURL				*curl;
	CURLcode			res;
	char				range[128] = { 0 };

	pctsi = (CC_THR_SEG_INFO*)param;

	curl = curl_easy_init();
	if (curl == NULL)
	{
		LOG_E("err curl_easy_init");
		return -1;
	}

	if (strncmp(pctsi->url, "https", 5) == 0)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, FALSE);//设定为不验证证书和HOST
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, FALSE);
	}
	if (pctsi->flag)
	{
		sprintf(range, "%ld-%ld", pctsi->start_pos, pctsi->end_pos);
		//LOG_D("thrd[%d] range [%s]", pctsi->id, range);
	}
	curl_easy_setopt(curl, CURLOPT_URL, pctsi->url);
	//curl_easy_setopt(curl, CURLOPT_TIMEOUT, pctsi->timeout); // 设置超时时间为3秒
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)pctsi);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, down_write_func);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
	curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, (void*)pctsi);
	curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, down_prog_func);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 1);
	curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, pctsi->timeout);
	if (pctsi->flag)
	{
		curl_easy_setopt(curl, CURLOPT_RANGE, range);
	}

	res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		LOG_E("thrd[%d] curl_easy_perform() failed: %s", pctsi->id, curl_easy_strerror(res));
		curl_easy_cleanup(curl);
		pctsi->ret = 1;
		return -1;
	}

	curl_easy_cleanup(curl);
	pctsi->ret = 0;
	return 1;
}

int download_file(int thread_num, char* url, char* path, char* filename, int timeout)
{
	long				file_len;
	char				temp_file[MAX_PATH] = { 0 };
	FILE*				fp;
	long				part_len = 0;
	int					i;
	CC_THR_SEG_INFO		ctsi[MAX_DOWNLOAD_THREAD_NUM];
	HANDLE				hWorkHandle[MAX_DOWNLOAD_THREAD_NUM];
	int					ct = 0;
	HANDLE				hMutex;

	//LOG_D("%s start...", __FUNCTION__);

	memset(ctsi, 0, sizeof(CC_THR_SEG_INFO)*MAX_DOWNLOAD_THREAD_NUM);

	sprintf(temp_file, "%s%s", path, filename);
	fp = fopen(temp_file, "wb");
	if (fp == NULL)
	{
		LOG_E("open file[%s] error,[%#x]", temp_file, CF_GetLastError());
		return -1;
	}

	hMutex = CreateMutexA(NULL, FALSE, "CC_DOWNLOAD_FILE_MUTEX");
	if (hMutex == NULL)
	{
		LOG_E("CreateMutexA m_hMutex error");
		return -1;
	}

	file_len = getDownLoadFileLen(url);
	if (file_len < 0)
	{
		LOG_W("getDownLoadFileLen error, try signal thread download...");
		ct = 0;
		ctsi[ct].flag = 0;
		ctsi[ct].fp = fp;
		ctsi[ct].id = ct;
		ctsi[ct].hMutex = hMutex;
		ctsi[ct].timeout = timeout;
		strcpy(ctsi[ct].url, url);
		hWorkHandle[ct] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkThreadProc, &ctsi[ct], 0, 0);
		ct++;
	}
	else
	{
		LOG_D("get file len=[%.2fKb]", file_len*1.0 / 1024);

		part_len = file_len / thread_num;

		for (i = 0; i <= thread_num; i++)
		{
			if (i < thread_num)
			{
				ctsi[ct].start_pos = i*part_len;
				ctsi[ct].end_pos = (i + 1)*part_len - 1;
			}
			else
			{
				if ((file_len % thread_num) != 0)
				{
					ctsi[ct].start_pos = i*part_len;
					ctsi[ct].end_pos = file_len - 1;
				}
				else
				{
					break;
				}
			}
			ctsi[ct].flag = 1;
			ctsi[ct].fp = fp;
			ctsi[ct].id = ct;
			ctsi[ct].hMutex = hMutex;
			ctsi[ct].timeout = timeout;
			strcpy(ctsi[ct].url, url);
			hWorkHandle[ct] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkThreadProc, &ctsi[ct], 0, 0);
			ct++;

		}
	}

	WaitForMultipleObjects(ct, hWorkHandle, TRUE, INFINITE);
	//LOG_D("all thread work finish ...");
	//LOG_D("%s end...", __FUNCTION__);
	for (i = 0; i < ct; i++)
	{
		if (ctsi[i].ret == 1)
		{
			LOG_E("some thread down fail [%d]", i);
			fclose(fp);
			return 1;
		}
	}

	fclose(fp);
	return 0;
}


