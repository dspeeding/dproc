#include <stdio.h>
#include <Windows.h>
#include "basetype.h"
#include "curl_comm.h"
#include "test_opt.h"
#include "xodbc/xodbc.h"

#pragma comment(lib, "libcurl.lib")
#pragma comment(lib, "dproclib.lib")
#pragma comment(lib, "pcre.lib")			//https://www.psyon.org/projects/pcre-win32/

char*		g_buff;
char*		g_buff_gbk;

int Test_qiushibaike(int page)
{
	int			ret;
	char		urltext[256] = { 0 };
	char*		pattern = "<div.*?class=\"content\">\\s*<span>([\\w\\W]*?)</span>\\s*</div>\\s*</a>[\\w\\W]*?<i.*? class=\"number\">(\\d*?)</i>";
	char*		buf = NULL;
	char*		p = NULL;
	char		text[8192] = { 0 };
	int			num = 0;
	char		temp[128] = { 0 };

	sprintf(urltext, "http://www.qiushibaike.com/text/page/%d", page);

	memset(g_buff, 0, MAX_BUFFER_SIZE);
	memset(g_buff_gbk, 0, MAX_BUFFER_SIZE);

	ret = get_html_data(urltext, NULL, g_buff, 5, NULL, NULL);
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

	ret = reg_proc_comm(g_buff_gbk, pattern, 2, buf, MAX_STR_SIZE);
	if (ret != 0)
	{
		Mem_Free(buf);
		LOG_E("parse regex error");
		return ret;
	}

	p = buf;

	while (*p != '\0')
	{
		strcpy(text, p);
		p += strlen(p) + 1;
		strcpy(temp, p);
		p += strlen(p) + 2;		//最后一个+2
		num = atoi(temp);

		LOG_D("[%d] [%s]", num, text);

		if (num > 500)
		{
			FILE* fp = fopen("qiushibaike.txt", "a");
			fprintf(fp, "%6d %s\n", num, text);
			fclose(fp);
		}
	}

	Mem_Free(buf);

	return 0;
}


/*create table lianjia(
	id varchar(32) not null,
	title varchar(128),
	region varchar(128),
	xiaoqu varchar(128),
	info varchar(128),
	totalprice varchar(32),
	avgprice varchar(32),
	position varchar(128),
	url	varchar(128),
	primary key(id))*/
int Test_Lianjia(int page, ODBC_CTX* ctx)
{
	int			ret;
	char		urltext[256] = { 0 };
	char*		pattern = "<div class=\"info clear\">[\\w\\W]*?<a class=\"\" href=\"([\\w\\W]*?)\" target=[\\w\\W]*?data-sl=\"\">([\\w\\W]*?)</a>[\\w\\W]*?<div class=\"houseInfo\">[\\w\\W]*?data-el=\"region\">([\\w\\W]*?)</a>\\s.*?([\\w\\W]*?)</div>[\\w\\W]*?<span class=\"positionIcon\"></span>([\\w\\W]*?)<a href[\\w\\W]*?target=\"_blank\">([\\w\\W]*?)</a>[\\w\\W]*?<div class=\"totalPrice\"><span>(\\d+?)</span>[\\w\\W]*?<span>([\\w\\W]*?)</span>";
	char*		buf = NULL;
	char*		p = NULL;
	char		title[512] = { 0 };
	char		url[1024] = { 0 };
	char		xiaoqu_name[128] = { 0 };		//小区名称
	char		tingshi_num[128] = { 0 };		//几室几厅
	char		mianji[128] = { 0 };			//面积
	char		chaoxiang[128] = { 0 };			//朝向
	char		zhuangxiu[128] = { 0 };			//装修
	char		dianti[128] = { 0 };			//电梯
	char		info[1024] = { 0 };
	char		addr[1024] = { 0 };
	char		region[1024] = { 0 };
	char		totalPrice[128] = { 0 };
	char		avgPrice[128] = { 0 };

	ODBC_STMT	ostmt;
	ODBC_STMT*	stmt = &ostmt;
	char		sql[1024] = { 0 };
	int			ind_a = SQL_NTS;
	char		id[128] = { 0 };
	char		*q1, *q2;

	memset(stmt, 0, sizeof(ostmt));
	//sprintf(urltext, "http://bj.lianjia.com/ershoufang/pg%d/", page);
	sprintf(urltext, "http://bj.lianjia.com/ershoufang/shahe2/pg%d/", page);

	memset(g_buff, 0, MAX_BUFFER_SIZE);
	memset(g_buff_gbk, 0, MAX_BUFFER_SIZE);

	ret = get_html_data(urltext, NULL, g_buff, 5, NULL, NULL);
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

	ret = reg_proc_comm(g_buff, pattern, 8, buf, MAX_STR_SIZE);
	if (ret != 0)
	{
		Mem_Free(buf);
		LOG_E("parse regex error");
		return ret;
	}

	sprintf(sql, "insert into lianjia(id,title,region,xiaoqu,info,totalprice,avgprice,position,url) values(?,?,?,?,?,?,?,?,?)");
	ret = odbc_prepare_stmt(ctx, stmt, sql);
	if (ret != ERR_OK)
	{
		LOG_E("odbc_prepare_stmt error,[%#x]", ret);
		Mem_Free(buf);
		return ret;
	}

	p = buf;

	while (*p != '\0')
	{
		strcpy(url, p);
		p += strlen(p) + 1;
		strcpy(title, p);
		p += strlen(p) + 1;
		strcpy(xiaoqu_name, p);
		p += strlen(p) + 1;
		strcpy(info, p);
		p += strlen(p) + 1;
		strcpy(addr, p);
		p += strlen(p) + 1;
		strcpy(region, p);
		p += strlen(p) + 1;
		strcpy(totalPrice, p);
		p += strlen(p) + 1;
		strcpy(avgPrice, p);
		p += strlen(p) + 2;		//最后一个+2
		

		FILE* fp = fopen("lianjia.txt", "a");
		fprintf(fp, "%-20s %6s万 %20s %-20s %-60s %-20s %-30s %s\n", 
			region, totalPrice, avgPrice, xiaoqu_name, info, addr, title, url);
		fclose(fp);

		q1 = strrchr(url, '/');
		if (q1 == NULL)
		{
			LOG_E("not find / in url,[%s]", url);
			continue;
		}
		q1++;
		q2 = strrchr(q1, '.');
		memset(id, 0, sizeof(id));
		memcpy(id, q1, q2 - q1);


		odbc_bind_col(stmt, 1, SQL_C_CHAR, SQL_CHAR, 32, 0, id, strlen(id) + 1, &ind_a);
		odbc_bind_col(stmt, 2, SQL_C_CHAR, SQL_CHAR, 128, 0, title, strlen(title) + 1, &ind_a);
		odbc_bind_col(stmt, 3, SQL_C_CHAR, SQL_CHAR, 128, 0, region, strlen(region) + 1, &ind_a);
		odbc_bind_col(stmt, 4, SQL_C_CHAR, SQL_CHAR, 128, 0, xiaoqu_name, strlen(xiaoqu_name) + 1, &ind_a);
		odbc_bind_col(stmt, 5, SQL_C_CHAR, SQL_CHAR, 128, 0, info+1, strlen(info+1) + 1, &ind_a);
		odbc_bind_col(stmt, 6, SQL_C_CHAR, SQL_CHAR, 32, 0, totalPrice, strlen(totalPrice) + 1, &ind_a);
		odbc_bind_col(stmt, 7, SQL_C_CHAR, SQL_CHAR, 32, 0, avgPrice, strlen(avgPrice) + 1, &ind_a);
		odbc_bind_col(stmt, 8, SQL_C_CHAR, SQL_CHAR, 128, 0, addr, strlen(addr) + 1, &ind_a);
		odbc_bind_col(stmt, 9, SQL_C_CHAR, SQL_CHAR, 128, 0, url, strlen(url) + 1, &ind_a);

		ret = odbc_execute(stmt);
		if (ret != ERR_OK)
		{
			continue;
		}
	}

	odbc_close_stmt(stmt);
	Mem_Free(buf);

	return 0;
}

int Test_DouBanMovie(int page)
{
	int			ret;
	char		urltext[256] = { 0 };
	char*		pattern = "<div.*?class=\"hd\">\\s*?<a.*?>\\s*?<span class=\"title\">([\\w\\W]*?)</span>[\\w\\W]*?<span class=\"rating_num\".*?average\">([\\w\\W]*?)</span>[\\w\\W]*?<span>([\\w\\W]*?)</span>";
	char*		buf = NULL;
	char*		p = NULL;
	char		title[512] = { 0 };
	char		score[128] = { 0 };				//得分
	char		pingjia_num[128] = { 0 };		//评价

	sprintf(urltext, "https://movie.douban.com/top250?start=%d", page*25);
	
	memset(g_buff, 0, MAX_BUFFER_SIZE);
	memset(g_buff_gbk, 0, MAX_BUFFER_SIZE);

	ret = get_html_data(urltext, NULL, g_buff, 5, NULL, NULL);
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

	ret = reg_proc_comm(g_buff, pattern, 3, buf, MAX_STR_SIZE);
	if (ret != 0)
	{
		Mem_Free(buf);
		LOG_E("parse regex error");
		return ret;
	}

	p = buf;

	while (*p != '\0')
	{
		strcpy(title, p);
		p += strlen(p) + 1;
		strcpy(score, p);
		p+= strlen(p) + 1;
		strcpy(pingjia_num, p);
		p += strlen(p) + 2;
		LOG_D("[%-30s] [%-6s] [%-20s]", title, score, pingjia_num);
	}

	Mem_Free(buf);

	return 0;
}

/*
void TestPCRE()
{
	pcre  *re;
	const char *error;
	int  erroffset;
	int  ovector[30];
	int  rc, i;

	char  src[] = "111 <title>Hello World</title> 222";   // 要被用来匹配的字符串    
	char  pattern[] = "<title>(.*)</(tit)le>";              // 将要被编译的字符串形式的正则表达式    

	re = pcre_compile(pattern,       // pattern, 输入参数，将要被编译的字符串形式的正则表达式    
		0,            // options, 输入参数，用来指定编译时的一些选项    
		&error,       // errptr, 输出参数，用来输出错误信息    
		&erroffset,   // erroffset, 输出参数，pattern中出错位置的偏移量    
		NULL);        // tableptr, 输入参数，用来指定字符表，一般情况用NULL    
					  // 返回值：被编译好的正则表达式的pcre内部表示结构    
	if (re == NULL) {                 //如果编译失败，返回错误信息    
		printf("PCRE compilation failed at offset %d: %s\n", erroffset, error);
		return ;
	}
	rc = pcre_exec(re,            // code, 输入参数，用pcre_compile编译好的正则表达结构的指针    
		NULL,          // extra, 输入参数，用来向pcre_exec传一些额外的数据信息的结构的指针    
		src,           // subject, 输入参数，要被用来匹配的字符串    
		strlen(src),  // length, 输入参数， 要被用来匹配的字符串的指针    
		0,             // startoffset, 输入参数，用来指定subject从什么位置开始被匹配的偏移量    
		0,             // options, 输入参数， 用来指定匹配过程中的一些选项    
		ovector,       // ovector, 输出参数，用来返回匹配位置偏移量的数组    
		30);    // ovecsize, 输入参数， 用来返回匹配位置偏移量的数组的最大大小    
					   // 返回值：匹配成功返回非负数，没有匹配返回负数    
	if (rc < 0) {                     //如果没有匹配，返回错误信息    
		if (rc == PCRE_ERROR_NOMATCH) printf("Sorry, no match ...\n");
		else printf("Matching error %d\n", rc);
		pcre_free(re);
		return ;
	}
	printf("\nOK, has matched ...\n\n");   //没有出错，已经匹配    
	for (i = 0; i < rc; i++) {             //分别取出捕获分组 $0整个正则公式 $1第一个()    
		char *substring_start = src + ovector[2 * i];
		int substring_length = ovector[2 * i + 1] - ovector[2 * i];
		printf("$%2d: %.*s\n", i, substring_length, substring_start);
	}
	pcre_free(re);                     // 编译正则表达式re 释放内存 
}
*/


void show_download_proc(long downed_len, long total_len)
{
	LOG_D("download process [%ld]--[%ld] per[%.2lf%%]", downed_len, total_len, downed_len/(total_len*1.0/100));
}

int main()
{
	int			ret;
	int			page;
	ODBC_CTX	octx;
	ODBC_CTX*	ctx = &octx;
	char		conn_str[1024] = { 0 };

	system("color 0A");

	LOG_Init(LOG_LEV_DEBUG, "testurl", "./testurl");

	g_buff = (char*)Mem_Malloc(MAX_BUFFER_SIZE);
	if (g_buff == NULL)
	{
		LOG_E("malloc g_buff error,[%#x]", CF_GetLastError());
		return 1;
	}

	memset(ctx, 0, sizeof(octx));

	memset(g_buff, 0, MAX_BUFFER_SIZE);
	g_buff_gbk = (char*)Mem_Malloc(MAX_BUFFER_SIZE);
	if (g_buff_gbk == NULL)
	{
		LOG_E("malloc g_buff_gbk error,[%#x]", CF_GetLastError());
		goto Err;
	}
	memset(g_buff_gbk, 0, MAX_BUFFER_SIZE);

	ret = curl_init();
	if (ret)
	{
		LOG_E("curl_init error");
		goto Err;
	}

	sprintf(conn_str, "Driver={MySQL ODBC 5.3 ANSI Driver};Server=172.16.1.198;Database=test;Port=3306;Uid=r7;Pwd=123456;Charset=utf8");
	ret = odbc_connect(ctx, conn_str, 5, 1);
	if (ret != ERR_OK)
	{
		LOG_E("connect odbc error,[%#x]", ret);
		goto Err;
	}

	page = 0;
	while (page <= 6)
	{
		LOG_D("get page %d ...", page);
		memset(g_buff, 0, MAX_BUFFER_SIZE);
		memset(g_buff_gbk, 0, MAX_BUFFER_SIZE);
		Test_qiushibaike(page);
// 		ret = Test_Lianjia(page, ctx);
// 		if (ret != 0)
// 		{
// 			LOG_E("Test_Lianjia error");
// 			break;
// 		}
// 		ret = Test_DouBanMovie(page);
// 		if (ret != 0)
// 		{
// 			LOG_E("Test_DouBanMovie error [%#x]", ret);
// 			break;
// 		}
// 		Test_ZhiHu();
		Sleep((rand()%20+5)*1000);
		page++;
	}
//	ret = download_file(5, "http://3w.gd.yhlg.com:8080/uploadFile/2016/BitComet.rar", "./", "BitComet1.rar", 30, NULL);
// 	ret = download_file(5, "http://releases.ubuntu.com/16.04/ubuntu-16.04.2-desktop-amd64.iso", "./", "ubuntu.iso", 60, show_download_proc);
// 	if (ret != 0)
// 	{
// 		LOG_E("down load file error");
// 		goto Err;
// 	}
	

//	test_pic();

Err:
	odbc_disconnect(ctx);
	if (g_buff) Mem_Free(g_buff);
	if (g_buff_gbk) Mem_Free(g_buff_gbk);
	curl_uninit();
	system("pause");
	return 0;
}