#include "basetype.h"
#include "LinkHash.h"

/*************************************************************************
@Purpose :	default hash function
@Param   :	c	    -- data to be hash
@Return  :	hash data
@Modify  :
@Remark  :	The following hash seems to work very well on normal text strings no
			collisions on /usr/dict/words and it distributes on %2^n quite well, not
			as good as MD5, but still good.
*************************************************************************/
static unsigned long LinkHash_Str(IN const char* c)
{
	unsigned long	ret = 0;
	long			n;
	unsigned long	v;
	int				r;

	if ((c == NULL) || (*c == '\0'))
		return (ret);
	/*-
	unsigned char b[16];
	MD5(c,strlen(c),b);
	return(b[0]|(b[1]<<8)|(b[2]<<16)|(b[3]<<24));
	*/

	n = 0x100;
	while (*c)
	{
		v = n | (*c);
		n += 0x100;
		r = (int)((v >> 2) ^ v) & 0x0f;
		ret = (ret << r) | (ret >> (32 - r));
		ret &= 0xFFFFFFFFL;
		ret ^= v * v;
		c++;
	}
	return ((ret >> 16) ^ ret);
}

/*************************************************************************
@Purpose :	init Link Hash
@Param   :	ppHash	    -- hash pointer
@Param   :	h			-- 哈希函数指针
@Param   :	c			-- 比较函数指针
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :	如果 c 为空，则默认使用strcmp 函数
			如果 h 为空，则默认使用 LinkHash_Str 函数
*************************************************************************/
int LH_Init(OUT LINK_HASH** ppHash, IN LINKHASH_HASH_FN_TYPE h, IN LINKHASH_COMP_FN_TYPE c)
{
	int			i;
	LINK_HASH*	pHash;

	*ppHash = (LINK_HASH*)Mem_Malloc(sizeof(LINK_HASH));
	if (*ppHash == NULL)
	{
		LOG_E("malloc *ppHash error,err=[%#x]", CF_GetLastError());
		return ERR_COMM_MEM_MALLOC;
	}

	pHash = *ppHash;

	pHash->b = (LINK_HASH_NODE **)Mem_Malloc(sizeof(LINK_HASH_NODE *)* LINK_HASH_MIN_NODES);
	if (pHash->b == NULL)
	{
		free(pHash);
		LOG_E("malloc pHash->b error,err=[%#x]", CF_GetLastError());
		return ERR_COMM_MEM_MALLOC;
	}

	for (i = 0; i < LINK_HASH_MIN_NODES; i++)
	{
		pHash->b[i] = NULL;
	}

	pHash->comp = ((c == NULL) ? (LINKHASH_COMP_FN_TYPE)strcmp : c);
	pHash->hash = ((h == NULL)? (LINKHASH_HASH_FN_TYPE)LinkHash_Str : h);

	pHash->num_nodes = LINK_HASH_MIN_NODES / 2;
	pHash->num_alloc_nodes = LINK_HASH_MIN_NODES;
	pHash->p = 0;
	pHash->pmax = LINK_HASH_MIN_NODES / 2;
	pHash->up_load = LINK_HASH_UP_LOAD;
	pHash->down_load = LINK_HASH_DOWN_LOAD;
	pHash->num_items = 0;
	pHash->num_expands = 0;
	pHash->num_expand_reallocs = 0;
	pHash->num_contracts = 0;
	pHash->num_contract_reallocs = 0;
	pHash->num_hash_calls = 0;
	pHash->num_comp_calls = 0;
	pHash->num_insert = 0;
	pHash->num_replace = 0;
	pHash->num_delete = 0;
	pHash->num_no_delete = 0;
	pHash->num_retrieve = 0;
	pHash->num_retrieve_miss = 0;
	pHash->num_hash_comps = 0;

	pHash->error = 0;

	return ERR_OK;
}

/*************************************************************************
@Purpose :	释放哈希链表
@Param   :	pHash	    -- hash pointer
@Return  :	None
@Modify  :
@Remark  :	
*************************************************************************/
void LH_Destory(IN LINK_HASH* pHash)
{
	unsigned int	i;
	LINK_HASH_NODE* n;
	LINK_HASH_NODE* nn;

	if (pHash == NULL)
	{
		return;
	}

	for (i = 0; i < pHash->num_nodes; i++)
	{
		n = pHash->b[i];
		while (n != NULL)
		{
			nn = n->next;
			Mem_Free(n);
			n = nn;
		}
	}
	Mem_Free(pHash->b);
	Mem_Free(pHash);
}

/*************************************************************************
@Purpose :  对哈希表进行扩展
@Param   :  pHash		--	[IN] 哈希表指针
@Return  :  ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :	之前只使用了b[0]-b[7] 往后扩展，每次一个
*************************************************************************/
static int LH_Expand(IN LINK_HASH* pHash)
{
	unsigned int		i;
	unsigned int		p;
	unsigned int		j;
	LINK_HASH_NODE**	n1;
	LINK_HASH_NODE**	n2;
	LINK_HASH_NODE*		np;
	LINK_HASH_NODE**	n;
	unsigned long		hash;
	unsigned long		nni;

	pHash->num_nodes++;
	pHash->num_expands++;
	p = pHash->p++;

	n1 = &pHash->b[p];
	n2 = &pHash->b[p + pHash->pmax];
	*n2 = NULL;

	nni = pHash->num_alloc_nodes;

	//对于b[p]的链表进行调整
	for (np = *n1; np != NULL;)
	{
		hash = np->hash;

		//将哈希值扩展到整个空间
		if ((hash % nni) != p)
		{
			//如果本次求出来的哈希值跟原来的不一样，则放置到b[pmax+p]链表下
			*n1 = (*n1)->next;
			np->next = *n2; 
			*n2 = np;
		}
		else
		{
			n1 = &((*n1)->next);
		}
		np = *n1;
	}

	//如果空间太小，则进行扩容
	if ((pHash->p) >= pHash->pmax)
	{
		j = pHash->num_alloc_nodes * 2;
		n = (LINK_HASH_NODE **)Mem_Realloc(pHash->b, sizeof(LINK_HASH_NODE*) * j);
		if (n == NULL)
		{
			LOG_E("realloc pHash->b error,err=[%#x]", CF_GetLastError());
			return ERR_COMM_MEM_MALLOC;
		}

		for (i = pHash->num_alloc_nodes; i < j; i++)
		{
			n[i] = NULL;
		}

		pHash->pmax = pHash->num_alloc_nodes;
		pHash->num_alloc_nodes = j;
		pHash->num_expand_reallocs++;
		pHash->p = 0;
		pHash->b = n;
	}

	return ERR_OK;
}

/*************************************************************************
@Purpose :  对哈希表进行收缩
@Param   :  pHash		--	[IN] 哈希表指针
@Return  :  ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :	
*************************************************************************/
static int LH_Contract(IN LINK_HASH* pHash)
{
	LINK_HASH_NODE**	n;
	LINK_HASH_NODE*		np;
	LINK_HASH_NODE*		n1;

	np = pHash->b[pHash->p + pHash->pmax - 1];
	pHash->b[pHash->p + pHash->pmax - 1] = NULL;

	if (pHash->p == 0)
	{
		n = (LINK_HASH_NODE **)Mem_Realloc(pHash->b, sizeof(LINK_HASH_NODE*) * pHash->pmax);
		if (n == NULL)
		{
			LOG_E("realloc n error,err=[%#]", CF_GetLastError());
			return ERR_COMM_MEM_MALLOC;
		}
		pHash->num_contract_reallocs++;
		pHash->num_alloc_nodes /= 2;
		pHash->pmax /= 2;
		pHash->p = pHash->pmax - 1;
		pHash->b = n;
	}
	else
	{
		pHash->p--;
	}

	pHash->num_nodes--;
	pHash->num_contracts++;

	n1 = pHash->b[pHash->p];
	if (n1 == NULL)
	{
		pHash->b[pHash->p] = np;
	}
	else
	{
		//定位到b[p]链表的末尾
		while (n1->next != NULL)
		{
			n1 = n1->next;
		}
		//指向b[p+pmax-1]
		n1->next = np;
	}

	return ERR_OK;
}

/*************************************************************************
@Purpose :  获取数据的位置指针及哈希结果
@Param   :  pHash		--	哈希表指针
@Param   :	data		--	数据指针
@Param   :	rhash		--	计算出的哈希值
@Return  :  哈希节点的指针
@Modify  :
@Remark  :
*************************************************************************/
static LINK_HASH_NODE** LH_GetRN(IN LINK_HASH* pHash, IN const void* data, OUT unsigned long* rhash)
{
	unsigned long		hash;
	unsigned long		nn;
	LINK_HASH_NODE**	n;
	LINK_HASH_NODE*		n1;

	hash = pHash->hash(data);
	pHash->num_hash_calls++;
	*rhash = hash;

	//只使用到哈希的前半部分
	nn = hash % pHash->pmax;
	if (nn < pHash->p)
	{
		//如果小于p表明进行过扩展，则需要用到哈希的全部，即num_alloc_nodes
		nn = hash % pHash->num_alloc_nodes;
	}

	n = &(pHash->b[nn]);
	for (n1 = *n; n1 != NULL; n1 = n1->next)
	{
		pHash->num_hash_comps++;
		if (n1->hash != hash)
		{
			n = &(n1->next);
			continue;
		}

		pHash->num_comp_calls++;
		if (pHash->comp(n1->data, data) == 0)
		{
			//如果值一样，则返回
			break;
		}
		n = &(n1->next);
	}
	
	return n;
}

/*************************************************************************
@Purpose :	插入数据
@Param   :	pHash	    -- hash pointer
@Param   :	data		-- 待插入的数据
@Param   :	old_data	-- 如果有相同的值，则返回原来的值，否则为空
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :	返回成功后，还需要判断一下old_data 是否为NULL，如果不为NULL需要做进一步处理，
			比如释放old_data的内存之类
*************************************************************************/
int LH_Insert(IN LINK_HASH* pHash, IN void* data, OUT void** old_data)
{
	int					nRet;
	unsigned long		hash;
	LINK_HASH_NODE**	rn = NULL;
	LINK_HASH_NODE*		nn = NULL;

	pHash->error = 0;

	//如果装填因子超过上限，则扩展哈希表
	if (pHash->up_load <= (pHash->num_items * LINK_HASH_LOAD_MULT / pHash->num_nodes))
	{
		nRet = LH_Expand(pHash);
		if (nRet != ERR_OK)
		{
			LOG_E("LH_Expand error");
			return nRet;
		}
	}

	rn = LH_GetRN(pHash, data, &hash);

	if (*rn == NULL)
	{
		//在链表尾部追加数据
		nn = (LINK_HASH_NODE *)Mem_Malloc(sizeof(LINK_HASH_NODE));
		if (nn == NULL)
		{
			LOG_E("malloc node error,err=[%#x]", CF_GetLastError());
			return ERR_COMM_MEM_MALLOC;
		}

		nn->data = data;
		nn->next = NULL;
		nn->hash = hash;
		(*rn) = nn;
		*old_data = NULL;
		pHash->num_insert++;
		pHash->num_items++;
	}
	else
	{
		//如果是相同的值，则替换为新的数据
		*old_data = (*rn)->data;
		(*rn)->data = data;
		pHash->num_replace++;
	}

	return ERR_OK;
}

/*************************************************************************
@Purpose :	删除数据
@Param   :	pHash	    -- hash pointer
@Param   :	data		-- 待删除的数据
@Param   :	old_data	-- 返回的值
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :	返回成功后，需要对old_data做进一步处理，比如释放old_data的内存之类
*************************************************************************/
int LH_Delete(IN LINK_HASH* pHash, IN const void* data, OUT void** old_data)
{
	int					nRet;
	unsigned long		hash;
	LINK_HASH_NODE**	rn;
	LINK_HASH_NODE*		nn;

	rn = LH_GetRN(pHash, data, &hash);

	if (*rn == NULL)
	{
		pHash->num_no_delete++;
		return ERR_COMM_NOT_FIND_DATA;
	}

	nn = *rn;
	*rn = nn->next;
	*old_data = nn->data;
	Mem_Free(nn);
	pHash->num_delete++;

	pHash->num_items--;
	if ((pHash->num_nodes > LINK_HASH_MIN_NODES) &&
		(pHash->down_load >= (pHash->num_items * LINK_HASH_LOAD_MULT / pHash->num_nodes)))
	{
		nRet = LH_Contract(pHash);
		if (nRet != ERR_OK)
		{
			LOG_E("LH_Contract error,[%#x]", nRet);
			return nRet;
		}
	}

	return ERR_OK;
}

/*************************************************************************
@Purpose :	检索数据
@Param   :	pHash	    -- hash pointer
@Param   :	data		-- 待查询的数据
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :	
*************************************************************************/
int LH_Retrieve(IN LINK_HASH* pHash, IN const void* data)
{
	unsigned long		hash;
	LINK_HASH_NODE**	rn;

	rn = LH_GetRN(pHash, data, &hash);

	if (*rn == NULL)
	{
		pHash->num_retrieve_miss++;
		return ERR_COMM_NOT_FIND_DATA;
	}

	pHash->num_retrieve++;
	return ERR_OK;
}

/*************************************************************************
@Purpose :  对所有数据执行操作
@Param   :  lh		--	哈希表指针
@Param   :	use_arg	--	0--表示不需要参数 1--表示需要参数
@Param   :	func	--	执行操作的函数地址
@Param   :	arg		--	执行函数时需要的参数
@Return  :  None
@Modify  :
@Remark  :
*************************************************************************/
static void doall_util_fn(IN LINK_HASH *pHash, IN int use_arg, IN LINKHASH_DOALL_FN_TYPE func,
	IN LINKHASH_DOALL_ARG_FN_TYPE func_arg, IN void *arg)
{
	int					i;
	LINK_HASH_NODE*		a;
	LINK_HASH_NODE*		n;

	if (pHash == NULL)
	{
		return;
	}

	for (i = pHash->num_nodes - 1; i >= 0; i--)
	{
		a = pHash->b[i];
		while (a != NULL)
		{
			n = a->next;
			if (use_arg)
			{
				func_arg(a->data, arg);
			}
			else
			{
				func(a->data);
			}
			a = n;
		}
	}
}

/*************************************************************************
@Purpose :  对所有数据执行操作
@Param   :  pHash	--	哈希表指针
@Param   :	func	--	执行操作的函数地址
@Return  :  None
@Modify  :
@Remark  :
*************************************************************************/
void LH_DoAll(IN LINK_HASH *pHash, IN LINKHASH_DOALL_FN_TYPE func)
{
	doall_util_fn(pHash, 0, func, (LINKHASH_DOALL_ARG_FN_TYPE)0, NULL);
}

/*************************************************************************
@Purpose :  对所有数据执行操作（需要参数）
@Param   :  pHash	--	哈希表指针
@Param   :	func	--	执行操作的函数地址
@Param   :	arg		--	执行函数时需要的参数
@Return  :  None
@Modify  :
@Remark  :
*************************************************************************/
void LH_DoAll_Arg(IN LINK_HASH *pHash, IN LINKHASH_DOALL_ARG_FN_TYPE func, IN void *arg)
{
	doall_util_fn(pHash, 1, (LINKHASH_DOALL_FN_TYPE)0, func, arg);
}

unsigned long LH_GetNum(IN const LINK_HASH* pHash)
{
	return pHash ? pHash->num_items : 0;
}

void LH_Stats(IN LINK_HASH *pHash, IN FILE *out)
{
	fprintf(out, "num_items             = %lu\n", pHash->num_items);
	fprintf(out, "num_nodes             = %u\n",  pHash->num_nodes);
	fprintf(out, "num_alloc_nodes       = %u\n",  pHash->num_alloc_nodes);
	fprintf(out, "num_expands           = %lu\n", pHash->num_expands);
	fprintf(out, "num_expand_reallocs   = %lu\n", pHash->num_expand_reallocs);
	fprintf(out, "num_contracts         = %lu\n", pHash->num_contracts);
	fprintf(out, "num_contract_reallocs = %lu\n", pHash->num_contract_reallocs);
	fprintf(out, "num_hash_calls        = %lu\n", pHash->num_hash_calls);
	fprintf(out, "num_comp_calls        = %lu\n", pHash->num_comp_calls);
	fprintf(out, "num_insert            = %lu\n", pHash->num_insert);
	fprintf(out, "num_replace           = %lu\n", pHash->num_replace);
	fprintf(out, "num_delete            = %lu\n", pHash->num_delete);
	fprintf(out, "num_no_delete         = %lu\n", pHash->num_no_delete);
	fprintf(out, "num_retrieve          = %lu\n", pHash->num_retrieve);
	fprintf(out, "num_retrieve_miss     = %lu\n", pHash->num_retrieve_miss);
	fprintf(out, "num_hash_comps        = %lu\n", pHash->num_hash_comps);
	fprintf(out, "p                     = %u\n",  pHash->p);
	fprintf(out, "pmax                  = %u\n",  pHash->pmax);
	fprintf(out, "up_load               = %lu\n", pHash->up_load);
	fprintf(out, "down_load             = %lu\n", pHash->down_load);
}

void LH_Node_Stats(IN LINK_HASH *pHash, IN FILE *out)
{
	LINK_HASH_NODE	*n;
	unsigned int	i, num;

	for (i = 0; i < pHash->num_nodes; i++) {
		for (n = pHash->b[i], num = 0; n != NULL; n = n->next)
			num++;
		fprintf(out, "node %6u -> %3u\n", i, num);
	}
}

void LH_Node_Usage_Stats(IN LINK_HASH *pHash, IN FILE *out)
{
	LINK_HASH_NODE *n;
	unsigned long	num;
	unsigned int	i;
	unsigned long	total = 0, n_used = 0;

	for (i = 0; i < pHash->num_nodes; i++) {
		for (n = pHash->b[i], num = 0; n != NULL; n = n->next)
			num++;
		if (num != 0) {
			n_used++;
			total += num;
		}
	}
	fprintf(out, "%lu nodes used out of %u\n", n_used, pHash->num_nodes);
	fprintf(out, "%lu items\n", total);
	if (n_used == 0)
		return;
	fprintf(out, "load %d.%02d  actual load %d.%02d\n",
		(int)(total / pHash->num_nodes),
		(int)((total % pHash->num_nodes) * 100 / pHash->num_nodes),
		(int)(total / n_used), (int)((total % n_used) * 100 / n_used));
}



