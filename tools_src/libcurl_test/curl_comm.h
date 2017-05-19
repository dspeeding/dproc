#pragma once

#ifdef __cplusplus
extern "C"
{
#endif


#define MAX_BUFFER_SIZE		10*1024*1024			//获取的最大缓冲区大小
#define MAX_STR_SIZE		5*1024*1024

#define MAX_DOWNLOAD_THREAD_NUM	100		//最大下载线程
#define MAX_ERR_RETRY_NUM		3		//最大下载重试次数

typedef void(*SHOW_DOWNLOAD_PROCESS_PROC)(long downed_len, long total_len);

typedef struct _TDEF_CC_THR_SEG_INFO
{
	int		id;				//当前线程ID
	int		flag;			//是否分片下载	某些情况下无法获取filelen
	FILE*	fp;				//打开的文件描述符
	long	start_pos;		//当前线程的文件起始位置
	long	cur_pos;		//处理到的位置
	long	end_pos;		//当前线程的文件终止位置
	int		ret;			//当前线程的执行结果 0--成功 1--失败
	int		timeout;		//执行curl的超时时间
	HANDLE	hMutex;			//写文件的互斥量
	char	url[1024];		//下载文件的url
	int		pre_per;		//线程中，上次下载的进度
}CC_THR_SEG_INFO;

int curl_init(void);

void curl_uninit(void);

int UTF8ToGBK(char* utf8str, char* gbkstr, int gbkstr_maxlen);

/*************************************************************************
@Purpose :	正则解析过程
@Param   :	content		--	待解析的文本内容
@Param   :	pattern		--	正则表达式串
@Param   :	region_num	--	正则表达式中匹配区域个数
@Param   :	buf			--	匹配内容返回缓冲区
@Param   :	buf_maxlen	--	匹配内容返回缓冲区长度
@Return  :	0--success other--fail
@Modify  :
@Remark  :	返回的内容是 str1 \0 str2 \0\0 str1 \0 str2 \0\0
*************************************************************************/
int reg_proc_comm(char* content, char* pattern, int region_num, char* buf, int buf_maxlen);

int get_html_data(char* url, char* buffer, int timeout, char* cookie, char* ipport);

int download_file(int thread_num, char* url, char* path, char* filename, int timeout, SHOW_DOWNLOAD_PROCESS_PROC proc);

#ifdef __cplusplus
}
#endif

