#include "basetype.h"
#include "mem/MemDbg.h"


//------------ȫ�ֱ�����-------
static LINK_HASH		*g_mh = NULL;				//�ڴ��ϣ���ڴ��ַ��Ϊ��ֵ��������Ҫ����
unsigned long			g_ulOrder = 0;				//�����¼������

//�ڴ�ԱȺ���
static int mem_cmp(const MEM *a, const MEM *b)
{
#ifdef _WIN64
    const char *ap = (const char *)a->addr, *bp = (const char *)b->addr;
    if (ap == bp)
        return 0;
    else if (ap > bp)
        return 1;
    else
        return -1;
#else
    return (const char *)a->addr - (const char *)b->addr;
#endif
}

//�ڴ��ϣ����
static unsigned long mem_hash(const MEM *a)
{
    unsigned long ret;
	
    ret = (unsigned long)a->addr;
	
    ret = ret * 17851 + (ret >> 14) * 7 + (ret >> 4) * 251;
    return (ret);
}


/*************************************************************************
 @Purpose :	�����ڴ����
 @Param   :	addr	--	���뵽�ĵ�ַ
 @Param   : ulSize	--	�����ڴ��С����λByte
 @Param   : szFile	--	�����ڴ��Դ�ļ���ַ
 @Param   : nLine	--	�����ڴ��Դ�ļ��к�
 @Return  :	None
 @Modify  :
 @Remark  :
 *************************************************************************/
void MyMemDbg_Malloc(IN void* addr, IN unsigned long ulSize, IN const char* szFile, IN int nLine)
{
	int		nRet;
	PMEM	pMem = NULL;
	PMEM	pMM = NULL;

	if (addr == NULL)
	{
		return ;
	}
	
	//�״γ�ʼ��
	if (g_mh == NULL) 
	{
		nRet = LH_Init(&g_mh, mem_hash, mem_cmp);
		if (nRet != ERR_OK) 
		{
			LOG_E("lh_MEM_new error,err=[%#x]", nRet);
			return ;
		}
	}

	pMem = (PMEM)malloc(sizeof(MEM));
	if (pMem == NULL)
	{
		LOG_E("malloc pMem err[%#x]", CF_GetLastError());
		return ;
	}
	memset(pMem, 0, sizeof(MEM));
	pMem->addr = addr;
	strcpy(pMem->file, szFile);
	pMem->line = nLine;
	pMem->num = ulSize;
	pMem->order = (g_ulOrder++ % 0xFFFFFFFF);				//�������̫������
	pMem->threadid = CF_GetCurThreadId();
	pMem->time = time(NULL);

	//�����ϣ����
	nRet = LH_Insert(g_mh, pMem, &pMM);
	if (nRet != ERR_OK)
	{
		LOG_E("LH_Insert error,err=[%#x]", nRet);
		return;
	}

	if (pMM)
	{
		//�����ֵ����ͬ�ģ���pMMΪ֮ǰ�Ǹ�����ϣ�������滻Ϊ�µ�
		free(pMM);
	}
}

/*************************************************************************
@Purpose :	�ͷ��ڴ����
@Param   :	addr	-- �ڴ��ַ
@Return  :	None
@Modify  :
@Remark  :
 *************************************************************************/
void MyMemDbg_Free(IN void* addr)
{
	int		nRet;
	MEM		mem;
	PMEM	pMem = NULL;

	memset(&mem, 0, sizeof(MEM));
	mem.addr = addr;

	//ɾ����¼
	nRet = LH_Delete(g_mh, &mem, &pMem);
	if (nRet != ERR_OK)
	{
		LOG_E("LH_Delete error,err=[%#x]", nRet);
		return;
	}
	if (pMem)
	{
		free(pMem);
	}
}

/*************************************************************************
@Purpose :	���������ڴ�
@Param   :	addrOld	--	[IN] ԭ�ڴ��ַ
@Param   :	addrNew	--	[IN] ���ڴ��ַ
@Param   :	ulSize	--	[IN] �����ڴ��С����λByte
@Param   :	szFile	--	[IN] �����ڴ��Դ�ļ���ַ
@Param   :	nLine	--	[IN] �����ڴ��Դ�ļ��к�
@Return  :	None
@Modify  :
@Remark  :
 *************************************************************************/
void MyMemDbg_Realloc(IN void* addrOld, IN void* addrNew, IN unsigned long ulSize, IN const char* szFile, IN int nLine)
{
	int		nRet;
	MEM		mem;
	PMEM	pMem = NULL;

	if (addrNew == NULL)
	{
		return ;
	}

	if (addrOld != NULL)
	{
		memset(&mem, 0, sizeof(MEM));
		mem.addr = addrOld;

		nRet = LH_Delete(g_mh, &mem, &pMem);
		if (nRet != ERR_OK)
		{
			LOG_E("LH_Delete error,err=[%#x]", nRet);
			return;
		}
		if (pMem)
		{
			free(pMem);
		}
	}

	//�����¼�¼
	MyMemDbg_Malloc(addrNew, ulSize, szFile, nLine);	
}

/*************************************************************************
@Purpose :	������ϣ�������ִ�д�ӡ��Ϣ
@Param   :	m		-- ��ϣ�������
@Param   :	l		-- ������Ϣ���ڴ�й¶��Ϣ��
@Return  :	None
@Modify  :
@Remark  :	������Ϊ�ص����������ڹ�ϣ����ִ�н��ִ����Ӧ����
 *************************************************************************/
static void print_leak_doall_arg(IN const MEM *m, IN MEM_LEAK *l)
{
	char		buf[1024] = {0};
	struct tm	*lcl = NULL;

	lcl = localtime(&m->time);
	sprintf(buf, "MEM_LEAK<--->addr[%p] size[%lu] count[%5lu] pid[%lu] [%02d:%02d:%02d][%s][%d]", 
					m->addr, m->num, m->order, m->threadid, 
					lcl->tm_hour, lcl->tm_min, lcl->tm_sec, m->file, m->line);

	LOG_E("%s", buf);

	l->chunks++;
    l->bytes += m->num;
	
}

/*************************************************************************
@Purpose :	��ӡ�ڴ�й¶��Ϣ
@Param   :	None
@Return  :	None
@Modify  :
@Remark  :
 *************************************************************************/
void MyMemDbg_Leak(void)
{
	MEM_LEAK	ml;

	if (g_mh == NULL)
	{
		return ;
	}

	ml.bytes = 0;
    ml.chunks = 0;

	LH_DoAll_Arg(g_mh, (LINKHASH_DOALL_ARG_FN_TYPE)print_leak_doall_arg, &ml);
	if (ml.chunks != 0) 
	{
        LOG_E("%ld bytes leaked in %d chunks", ml.bytes, ml.chunks);
    } 

	//���û�з����ڴ�й¶�����ͷŹ�ϣ��
	LH_Destory(g_mh);
	g_mh = NULL;
}

