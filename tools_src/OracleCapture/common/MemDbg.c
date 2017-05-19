#include "basetype.h"
#include "MemDbg.h"
#include "Lhash.h"

//�����ڴ��ϣ��
DECLARE_LHASH_OF(MEM);

//���������ڴ�����Ĺ�ϣ����
# define lh_MEM_new()							LHM_lh_new(MEM,mem)
# define lh_MEM_insert(lh,inst)					LHM_lh_insert(MEM,lh,inst)
# define lh_MEM_retrieve(lh,inst)				LHM_lh_retrieve(MEM,lh,inst)
# define lh_MEM_delete(lh,inst)					LHM_lh_delete(MEM,lh,inst)
# define lh_MEM_doall(lh,fn)					LHM_lh_doall(MEM,lh,fn)
# define lh_MEM_doall_arg(lh,fn,arg_type,arg)	LHM_lh_doall_arg(MEM,lh,fn,arg_type,arg)
# define lh_MEM_error(lh)						LHM_lh_error(MEM,lh)
# define lh_MEM_num_items(lh)					LHM_lh_num_items(MEM,lh)
# define lh_MEM_down_load(lh)					LHM_lh_down_load(MEM,lh)
# define lh_MEM_node_stats_bio(lh,out)			LHM_lh_node_stats(MEM,lh,out)
# define lh_MEM_node_usage_stats_bio(lh,out)	LHM_lh_node_usage_stats(MEM,lh,out)
# define lh_MEM_stats_bio(lh,out)				LHM_lh_stats(MEM,lh,out)
# define lh_MEM_free(lh)						LHM_lh_free(MEM,lh)


//------------ȫ�ֱ�����-------
static LHASH_OF(MEM)	*g_mh = NULL;				//�ڴ��ϣ���ڴ��ַ��Ϊ��ֵ��������Ҫ����
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

static IMPLEMENT_LHASH_COMP_FN(mem, MEM)

//�ڴ��ϣ����
static unsigned long mem_hash(const MEM *a)
{
    unsigned long ret;
	
    ret = (unsigned long)a->addr;
	
    ret = ret * 17851 + (ret >> 14) * 7 + (ret >> 4) * 251;
    return (ret);
}

static IMPLEMENT_LHASH_HASH_FN(mem, MEM)


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
	PMEM	pMem = NULL;
	PMEM	pMM = NULL;

	if (addr == NULL)
	{
		return ;
	}
	
	//�״γ�ʼ��
	if (g_mh == NULL) 
	{
		if ((g_mh = lh_MEM_new()) == NULL) 
		{
			LOG_E("lh_MEM_new err");
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
	if ((pMM = lh_MEM_insert(g_mh, pMem)) != NULL) 
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
	MEM		mem;
	PMEM	pMem = NULL;

	memset(&mem, 0, sizeof(MEM));
	mem.addr = addr;

	//ɾ����¼
	pMem = lh_MEM_delete(g_mh, &mem);
	if (pMem != NULL)
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
		pMem = lh_MEM_delete(g_mh, &mem);
		if (pMem != NULL)
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

//������ӡ��ϣ�ڴ�й¶��������--������
static IMPLEMENT_LHASH_DOALL_ARG_FN(print_leak, const MEM, MEM_LEAK)

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

	lh_MEM_doall_arg(g_mh, LHASH_DOALL_ARG_FN(print_leak), MEM_LEAK, &ml);
	if (ml.chunks != 0) 
	{
        LOG_E("%ld bytes leaked in %d chunks", ml.bytes, ml.chunks);
    } 
	else
	{
		//���û�з����ڴ�й¶�����ͷŹ�ϣ��
		lh_MEM_free(g_mh);
		g_mh = NULL;
	}
}

