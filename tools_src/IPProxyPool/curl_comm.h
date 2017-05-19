#pragma once

#ifdef __cplusplus
extern "C"
{
#endif


#define MAX_BUFFER_SIZE		10*1024*1024			//��ȡ����󻺳�����С
#define MAX_STR_SIZE		5*1024*1024

#define MAX_DOWNLOAD_THREAD_NUM	100		//��������߳�
#define MAX_ERR_RETRY_NUM		3		//����������Դ���

typedef void(*SHOW_DOWNLOAD_PROCESS_PROC)(long downed_len, long total_len);

typedef struct _TDEF_CC_THR_SEG_INFO
{
	int		id;				//��ǰ�߳�ID
	int		flag;			//�Ƿ��Ƭ����	ĳЩ������޷���ȡfilelen
	FILE*	fp;				//�򿪵��ļ�������
	long	start_pos;		//��ǰ�̵߳��ļ���ʼλ��
	long	cur_pos;		//������λ��
	long	end_pos;		//��ǰ�̵߳��ļ���ֹλ��
	int		ret;			//��ǰ�̵߳�ִ�н�� 0--�ɹ� 1--ʧ��
	int		timeout;		//ִ��curl�ĳ�ʱʱ��
	HANDLE	hMutex;			//д�ļ��Ļ�����
	char	url[1024];		//�����ļ���url
	int		pre_per;		//�߳��У��ϴ����صĽ���
}CC_THR_SEG_INFO;

int curl_init(void);

void curl_uninit(void);

int UTF8ToGBK(char* utf8str, char* gbkstr, int gbkstr_maxlen);

/*************************************************************************
@Purpose :	�����������
@Param   :	content		--	���������ı�����
@Param   :	pattern		--	������ʽ��
@Param   :	region_num	--	������ʽ��ƥ���������
@Param   :	buf			--	ƥ�����ݷ��ػ�����
@Param   :	buf_maxlen	--	ƥ�����ݷ��ػ���������
@Return  :	0--success other--fail
@Modify  :
@Remark  :	���ص������� str1 \0 str2 \0\0 str1 \0 str2 \0\0
*************************************************************************/
int reg_proc_comm(char* content, char* pattern, int region_num, char* buf, int buf_maxlen);

int get_html_data(char* url, char* buffer, int timeout, char* cookie, char* ipport);

int download_file(int thread_num, char* url, char* path, char* filename, int timeout, SHOW_DOWNLOAD_PROCESS_PROC proc);

#ifdef __cplusplus
}
#endif

