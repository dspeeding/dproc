#include "basetype.h"
#include "mem/MemDbg.h"


//------------全局变量区-------
static LINK_HASH		*g_mh = NULL;				//内存哈希表，内存地址作为键值，访问需要互斥
unsigned long			g_ulOrder = 0;				//申请记录计数器

//内存对比函数
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

//内存哈希函数
static unsigned long mem_hash(const MEM *a)
{
    unsigned long ret;
	
    ret = (unsigned long)a->addr;
	
    ret = ret * 17851 + (ret >> 14) * 7 + (ret >> 4) * 251;
    return (ret);
}


/*************************************************************************
 @Purpose :	申请内存调试
 @Param   :	addr	--	申请到的地址
 @Param   : ulSize	--	申请内存大小，单位Byte
 @Param   : szFile	--	申请内存的源文件地址
 @Param   : nLine	--	申请内存的源文件行号
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
	
	//首次初始化
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
	pMem->order = (g_ulOrder++ % 0xFFFFFFFF);				//如果次数太大会溢出
	pMem->threadid = CF_GetCurThreadId();
	pMem->time = time(NULL);

	//加入哈希表中
	nRet = LH_Insert(g_mh, pMem, &pMM);
	if (nRet != ERR_OK)
	{
		LOG_E("LH_Insert error,err=[%#x]", nRet);
		return;
	}

	if (pMM)
	{
		//如果键值有相同的，则pMM为之前那个，哈希表中则替换为新的
		free(pMM);
	}
}

/*************************************************************************
@Purpose :	释放内存调试
@Param   :	addr	-- 内存地址
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

	//删除记录
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
@Purpose :	重新申请内存
@Param   :	addrOld	--	[IN] 原内存地址
@Param   :	addrNew	--	[IN] 新内存地址
@Param   :	ulSize	--	[IN] 申请内存大小，单位Byte
@Param   :	szFile	--	[IN] 申请内存的源文件地址
@Param   :	nLine	--	[IN] 申请内存的源文件行号
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

	//加入新记录
	MyMemDbg_Malloc(addrNew, ulSize, szFile, nLine);	
}

/*************************************************************************
@Purpose :	遍历哈希表带参数执行打印信息
@Param   :	m		-- 哈希结点数据
@Param   :	l		-- 参数信息（内存泄露信息）
@Return  :	None
@Modify  :
@Remark  :	本函数为回调函数，用于哈希遍历执行结点执行相应操作
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
@Purpose :	打印内存泄露信息
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

	//如果没有发现内存泄露，则释放哈希表
	LH_Destory(g_mh);
	g_mh = NULL;
}

