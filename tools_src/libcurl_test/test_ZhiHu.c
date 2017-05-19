#include "basetype.h"
#include "curl_comm.h"

extern char*		g_buff;
extern char*		g_buff_gbk;

int Test_Zhihu(char* name)
{
	int			ret;
	char		urltext[256] = { 0 };
	char*		pattern = "<div class=\"UserItem-title\">[\\w\\W]*?href=\"/people/([\\w\\W]*?)\">([\\w\\W]*?)</a>[\\w\\W]*?<div class=\"ContentItem-status\">\\s*?<span class=\"ContentItem-statusItem\">([\\w\\W]*?)</span>\\s*?<span class=\"ContentItem-statusItem\">([\\w\\W]*?)</span>\\s*?<span class=\"ContentItem-statusItem\">([\\w\\W]*?)</span>";
	char		cookie[8192] = { 0 };
	char*		buf = NULL;
	char*		p = NULL;
	char		name_pinyin[512] = { 0 };			//名称
	char		name_a[128] = { 0 };				//别名
	char		ans[128] = { 0 };					//回答
	char		wenzhang[128] = { 0 };				//文章
	char		guanzhuzhe[128] = { 0 };			//关注者	


	sprintf(urltext, "https://www.zhihu.com/people/%s/following", name);

	p = cookie;
	p += sprintf(p, "%s; ", "__utma=51854390.1927729736.1491478359.1491478359.1491478359.1");
	p += sprintf(p, "%s; ", "__utmb=51854390.0.10.1491478359");
	p += sprintf(p, "%s; ", "__utmc=51854390");
	p += sprintf(p, "%s; ", "__utmv=51854390.100-1|2=registration_date=20140901=1^3=entry_date=20140901=1");
	p += sprintf(p, "%s; ", "__utmz=51854390.1491478359.1.1.utmcsr=(direct)|utmccn=(direct)|utmcmd=(none)");
	p += sprintf(p, "%s; ", "_xsrf=b43af5639a5c2864f0fe4bb3d6498259");
	p += sprintf(p, "%s; ", "_zap=818c71e8-1ce7-496c-85e2-9c7fb548f9c8");
	p += sprintf(p, "%s; ", "aliyungf_tc=AQAAALCMgXbdaA0ACLLZ3XoOY3BVdTCX");
	p += sprintf(p, "%s; ", "cap_id=\"MGVlYWE3ZmFkNTBjNGE3YWJmMDlhMDljOWE2MjgwNmM=|1491478378|f5e3bfcafe785def6005b656a837b4d450e7b685\"");
	p += sprintf(p, "%s; ", "capsion_ticket=\"2|1:0|10:1491478371|14:capsion_ticket|44:YjgyMTAxZDM3MjViNDQ4YzkyZDcxNDc4MzljMzcwNjY=|d18ae975a6da65fb4c4368d9cf50073a3bf5ed7871d3a685f1de904898a5b51a\"");
	p += sprintf(p, "%s; ", "d_c0=\"AIBCJSnWkAuPTmhrE_zY6xAw9LxUn4BvNxs=|1491478378\"");
	p += sprintf(p, "%s; ", "l_cap_id=\"ZDRlZTliOGEyMGE2NDAyZmIwNzczMTM2NDkwYjY2OWI=|1491478378|9f9cacdf431e951ed47fa61721ea18cd56eb485d\"");
	p += sprintf(p, "%s; ", "l_n_c=1");
	p += sprintf(p, "%s; ", "n_c_l=1");
	p += sprintf(p, "%s; ", "q_c1=4acfd8ecbaeb4d02b7fe53f868603fd1|1491478378000|1491478378000");
	p += sprintf(p, "%s; ", "r_cap_id=\"ODc1ZGFiZmViYzA5NGI5ZmExNjkxMGM3ZjU5OGVhNTI=|1491478378|2a9e8fee052d4321b59c732688fd89290369ec89\"");
	p += sprintf(p, "%s; ", "z_c0=Mi4wQUFCQUxUMDJBQUFBZ0VJbEtkYVFDeGNBQUFCaEFsVk5kclFOV1FBNUxyYzM0a1QwUk5iYTdJSURZN2dmcVJIZXpB|1491478394|5bb27554f160f0efd2975322638cd80af8244fa6");

	LOG_D("cookie=[%s]", cookie);

	memset(g_buff, 0, MAX_BUFFER_SIZE);
	memset(g_buff_gbk, 0, MAX_BUFFER_SIZE);

	ret = get_html_data(urltext, g_buff, 5, cookie, NULL);
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

	ret = reg_proc_comm(g_buff_gbk, pattern, 5, buf, MAX_STR_SIZE);
	if (ret != 0)
	{
		Mem_Free(buf);
		LOG_E("parse regex error");
		return ret;
	}

	p = buf;

	while (*p != '\0')
	{
		strcpy(name_pinyin, p);
		p += strlen(p) + 1;
		strcpy(name_a, p);
		p += strlen(p) + 1;
		strcpy(ans, p);
		p += strlen(p) + 1;
		strcpy(wenzhang, p);
		p += strlen(p) + 1;
		strcpy(guanzhuzhe, p);
		p += strlen(p) + 2;
		LOG_D("[%-20s] [%-20s] [%-10s] [%-10s] [%-10s]", name_pinyin, name_a, ans, wenzhang, guanzhuzhe);
	}

	Mem_Free(buf);

	return 0;
}

int Test_ZhiHu(void)
{

	Test_Zhihu("wang-ni-ma-94");

	return 0;
}