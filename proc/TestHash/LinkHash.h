#pragma once


#define LINK_HASH_LOAD_MULT		256

#define LINK_HASH_MIN_NODES		16
#define LINK_HASH_UP_LOAD		(2*LINK_HASH_LOAD_MULT)
#define LINK_HASH_DOWN_LOAD		(LINK_HASH_LOAD_MULT)

typedef struct LINK_HASH_NODE_ST
{
	void*						data;	//数据内容
	struct LINK_HASH_NODE_ST*	next;	//下一个节点
	unsigned long				hash;	//数据哈希计算值
}LINK_HASH_NODE;

typedef int (*LINKHASH_COMP_FN_TYPE)(const void *, const void *);
typedef unsigned long (*LINKHASH_HASH_FN_TYPE)(const void *);
typedef void (*LINKHASH_DOALL_FN_TYPE)(void *);
typedef void(*LINKHASH_DOALL_ARG_FN_TYPE)(void*, void*);


/************************************************************************
这里p和pmax用途，当哈希表已经很满的时候，发生冲突的概率更大，访问数据时
要把整个链表遍历一遍，这样性能会很低，openssl中采用的方法是将表的数据增长一些
把那些链表的元素往新增的部分移动一些，这样就能产生很好的效果，通过装填因子来
判断哈希表是否很满，装填因子 a = num_items/num_nodes
************************************************************************/

typedef struct LINK_HASH_ST
{
	LINK_HASH_NODE **b;								//哈希指针数组，数组中每个值为哈希链表中的首地址
	LINKHASH_COMP_FN_TYPE	comp;					//数据比较函数
	LINKHASH_HASH_FN_TYPE	hash;					//计算哈希值函数的地址
	unsigned int			num_nodes;				//表示链表个数
	unsigned int			num_alloc_nodes;		//表示分配空间个数，大于等于num_nodes
	unsigned int			p;						//
	unsigned int			pmax;					//
	unsigned int			up_load;				//
	unsigned int			down_load;				//
	unsigned long			num_items;				//所有的数据个数
	unsigned long			num_expands;			//扩充调用计数器
	unsigned long			num_expand_reallocs;	//
	unsigned long			num_contracts;
	unsigned long			num_contract_reallocs;
	unsigned long			num_hash_calls;			//计算哈希函数调用计数器
	unsigned long			num_comp_calls;			//比较函数调用计数器
	unsigned long			num_insert;				//插入数据计数器
	unsigned long			num_replace;			//相同数据替换计数器
	unsigned long			num_delete;				//删除数据计数器
	unsigned long			num_no_delete;			//删除失败计数器
	unsigned long			num_retrieve;			//检索数据计数器
	unsigned long			num_retrieve_miss;		//检索失败计数器
	unsigned long			num_hash_comps;			//对比计数器
	int						error;					//错误次数
}LINK_HASH;

#ifdef __cplusplus
extern "C"
{
#endif

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
int LH_Init(OUT LINK_HASH** ppHash, IN LINKHASH_HASH_FN_TYPE h, IN LINKHASH_COMP_FN_TYPE c);

/*************************************************************************
@Purpose :	释放哈希链表
@Param   :	pHash	    -- hash pointer
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void LH_Destory(IN LINK_HASH* pHash);

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
int LH_Insert(IN LINK_HASH* pHash, IN void* data, OUT void** old_data);

/*************************************************************************
@Purpose :	删除数据
@Param   :	pHash	    -- hash pointer
@Param   :	data		-- 待删除的数据
@Param   :	old_data	-- 返回的值
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :	返回成功后，需要对old_data做进一步处理，比如释放old_data的内存之类
*************************************************************************/
int LH_Delete(IN LINK_HASH* pHash, IN const void* data, OUT void* old_data);

/*************************************************************************
@Purpose :	检索数据
@Param   :	pHash	    -- hash pointer
@Param   :	data		-- 待查询的数据
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int LH_Retrieve(IN LINK_HASH* pHash, IN const void* data);

/*************************************************************************
@Purpose :  对所有数据执行操作
@Param   :  pHash	--	哈希表指针
@Param   :	func	--	执行操作的函数地址
@Return  :  None
@Modify  :
@Remark  :
*************************************************************************/
void LH_DoAll(IN LINK_HASH *pHash, IN LINKHASH_DOALL_FN_TYPE func);

/*************************************************************************
@Purpose :  对所有数据执行操作（需要参数）
@Param   :  pHash	--	哈希表指针
@Param   :	func	--	执行操作的函数地址
@Param   :	arg		--	执行函数时需要的参数
@Return  :  None
@Modify  :
@Remark  :
*************************************************************************/
void LH_DoAll_Arg(IN LINK_HASH *pHash, IN LINKHASH_DOALL_ARG_FN_TYPE func, IN void *arg);

unsigned long LH_GetNum(IN const LINK_HASH* pHash);

void LH_Stats(IN LINK_HASH *pHash, IN FILE *out);

void LH_Node_Stats(IN LINK_HASH *pHash, IN FILE *out);

void LH_Node_Usage_Stats(IN LINK_HASH *pHash, IN FILE *out);

#ifdef __cplusplus
}
#endif
