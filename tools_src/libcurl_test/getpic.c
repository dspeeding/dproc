#include "basetype.h"

#include "curl_comm.h"

extern char*		g_buff;
extern char*		g_buff_gbk;


int test_pic_sub(char* url, char* next_url)
{
	int			ret;
	char		urltext[256] = { 0 };
	char*		pattern = "<div class=\"cp-pagenavi\">[\\w\\W]*?<a href=\"([\\w\\W]*?)\">";
	char*		pattern_2 = "<span class=\"righttext\">\\s*?<a href[\\w\\W]*?>([\\w\\W]*?)</a>[\\w\\W]*?<a href=\"([\\w\\W]*?)\" target";
	char*		buf = NULL;
	char*		p = NULL;
	char		id[32] = { 0 };
	char		jpgaddr[512] = { 0 };	
	char		pic_name[128] = { 0 };
	char*		q = NULL;

	memset(g_buff, 0, MAX_BUFFER_SIZE);
	memset(g_buff_gbk, 0, MAX_BUFFER_SIZE);

	ret = get_html_data(url, NULL, g_buff, 5, NULL, NULL);
	if (ret != 0)
	{
		LOG_E("get_html_data error");
		return 1;
	}

	if (UTF8ToGBK(g_buff, g_buff_gbk, MAX_BUFFER_SIZE) == 0)
	{
		LOG_E("convert encode error");
		return 1;
	}

	buf = (char*)Mem_Malloc(MAX_STR_SIZE);
	if (buf == NULL)
	{
		LOG_E("malloc buf error [%#x]", CF_GetLastError());
		return 1;
	}
	memset(buf, 0, MAX_STR_SIZE);

	ret = reg_proc_comm(g_buff_gbk, pattern, 1, buf, MAX_STR_SIZE);
	if (ret != 0)
	{
		Mem_Free(buf);
		LOG_E("parse regex error");
		return ret;
	}

	p = buf;
	strcpy(next_url, p);
	if (next_url[0] == 0)
	{
		LOG_E("not get next url");
		Mem_Free(buf);
		return 1;
	}
	LOG_D("next url[%s]", next_url);

	memset(buf, 0, MAX_STR_SIZE);
	ret = reg_proc_comm(g_buff_gbk, pattern_2, 2, buf, MAX_STR_SIZE);
	if (ret != 0)
	{
		Mem_Free(buf);
		LOG_E("parse regex error");
		return ret;
	}

	p = buf;

	while (*p != '\0')
	{
		strcpy(id, p);
		p += strlen(p) + 1;
		strcpy(jpgaddr, p);
		p += strlen(p) + 2;		//最后一个+2

		LOG_D("[%s] [%s]", id, jpgaddr);
		
		q = strrchr(jpgaddr, '.');
		if (q == NULL)
		{
			LOG_E("file[%s] [%s] url error", id, jpgaddr);
			continue;
		}
		memset(pic_name, 0, sizeof(pic_name));
		sprintf(pic_name, "%s%s", id, q);
		ret = download_file(1, jpgaddr + 2, "./pic/", pic_name, 5, NULL);
		if (ret != 0)
		{
			LOG_E("save file[%s] [%s] error", id, jpgaddr);
			continue;
		}
	}

	Mem_Free(buf);

	return 0;
}

void test_pic(void)
{
	int		ret;
	char	url[1024] = { 0 };
	char	next_url[1024] = { 0 };
	int		ct = 0;

	strcpy(url, "http://jandan.net/ooxx");

	while (ct < 5)
	{
		ret = test_pic_sub(url, next_url);
		if (ret)
		{
			LOG_E("get pic err, url[%s]", url);
			return;
		}
		strcpy(url, next_url);
		memset(next_url, 0, sizeof(next_url));
		ct++;
	}
	//

}