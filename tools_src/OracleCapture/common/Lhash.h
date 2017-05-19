#ifndef HEADER_LHASH_H
#define HEADER_LHASH_H

#include <stdio.h>

#ifdef  __cplusplus
extern "C" {
#endif



#  define CHECKED_PTR_OF(type, p) \
    ((void*) (1 ? p : (type*)0))

	
//哈希表数据结构
typedef struct lhash_node_st 
{
    void *data;						//数据地址
    struct lhash_node_st *next;		//下一个数据地址
# ifndef OPENSSL_NO_HASH_COMP		
    unsigned long hash;				//数据哈希计算值
# endif
} LHASH_NODE;

typedef int (*LHASH_COMP_FN_TYPE) (const void *, const void *);
typedef unsigned long (*LHASH_HASH_FN_TYPE) (const void *);
typedef void (*LHASH_DOALL_FN_TYPE) (void *);
typedef void (*LHASH_DOALL_ARG_FN_TYPE) (void *, void *);

/*
 * Macros for declaring and implementing type-safe wrappers for LHASH
 * callbacks. This way, callbacks can be provided to LHASH structures without
 * function pointer casting and the macro-defined callbacks provide
 * per-variable casting before deferring to the underlying type-specific
 * callbacks. NB: It is possible to place a "static" in front of both the
 * DECLARE and IMPLEMENT macros if the functions are strictly internal.
 */

/* First: "hash" functions */
# define DECLARE_LHASH_HASH_FN(name, o_type) \
        unsigned long name##_LHASH_HASH(const void *);
# define IMPLEMENT_LHASH_HASH_FN(name, o_type) \
        unsigned long name##_LHASH_HASH(const void *arg) { \
                const o_type *a = (const o_type *)arg; \
                return name##_hash(a); }
# define LHASH_HASH_FN(name) name##_LHASH_HASH

/* Second: "compare" functions */
# define DECLARE_LHASH_COMP_FN(name, o_type) \
        int name##_LHASH_COMP(const void *, const void *);
# define IMPLEMENT_LHASH_COMP_FN(name, o_type) \
        int name##_LHASH_COMP(const void *arg1, const void *arg2) { \
                const o_type *a = (const o_type*)arg1;             \
                const o_type *b = (const o_type*)arg2; \
                return name##_cmp(a,b); }
# define LHASH_COMP_FN(name) name##_LHASH_COMP

/* Third: "doall" functions */
# define DECLARE_LHASH_DOALL_FN(name, o_type) \
        void name##_LHASH_DOALL(void *);
# define IMPLEMENT_LHASH_DOALL_FN(name, o_type) \
        void name##_LHASH_DOALL(void *arg) { \
                o_type *a = (o_type *)arg; \
                name##_doall(a); }
# define LHASH_DOALL_FN(name) name##_LHASH_DOALL

/* Fourth: "doall_arg" functions */
# define DECLARE_LHASH_DOALL_ARG_FN(name, o_type, a_type) \
        void name##_LHASH_DOALL_ARG(void *, void *);
# define IMPLEMENT_LHASH_DOALL_ARG_FN(name, o_type, a_type) \
        void name##_LHASH_DOALL_ARG(void *arg1, void *arg2) { \
                o_type *a = (o_type *)arg1; \
                a_type *b = (a_type *)arg2; \
                name##_doall_arg(a, b); }
# define LHASH_DOALL_ARG_FN(name) name##_LHASH_DOALL_ARG


/************************************************************************
这里p和pmax用途，当哈希表已经很满的时候，发生冲突的概率更大，访问数据时
要把整个链表遍历一遍，这样性能会很低，openssl中采用的方法是将表的数据增长一些
把那些链表的元素往新增的部分移动一些，这样就能产生很好的效果，通过装填因子来
判断哈希表是否很满，装填因子 a = num_items/num_nodes
************************************************************************/


typedef struct lhash_st {
    LHASH_NODE **b;										//哈希指针数组，数组中每个值为哈希链表中的首地址
    LHASH_COMP_FN_TYPE comp;							//数据比较函数地址
    LHASH_HASH_FN_TYPE hash;							//计算哈希值函数的地址
    unsigned int num_nodes;								//表示链表个数
    unsigned int num_alloc_nodes;						//表示分配空间个数，大于等于num_nodes
    unsigned int p;										//
    unsigned int pmax;
    unsigned long up_load;								/* load times 256 */
    unsigned long down_load;							/* load times 256 */
    unsigned long num_items;							//所有的数据个数
    unsigned long num_expands;							//扩充调用计数器
    unsigned long num_expand_reallocs;					//
    unsigned long num_contracts;
    unsigned long num_contract_reallocs;
    unsigned long num_hash_calls;						//计算哈希函数调用计数器
    unsigned long num_comp_calls;						//比较函数调用计数器
    unsigned long num_insert;							//插入数据计数器
    unsigned long num_replace;							//相同数据替换计数器
    unsigned long num_delete;							//删除数据计数器
    unsigned long num_no_delete;						//删除失败计数器
    unsigned long num_retrieve;							//检索数据计数器
    unsigned long num_retrieve_miss;					//检索失败计数器
    unsigned long num_hash_comps;						//对比计数器
    int error;											//错误次数
} _LHASH;                       /* Do not use _LHASH directly, use LHASH_OF
                                 * and friends */

# define LH_LOAD_MULT    256

/*
 * Indicates a malloc() error in the last call, this is only bad in
 * lh_insert().
 */
# define lh_error(lh)    ((lh)->error)

_LHASH *lh_new(IN LHASH_HASH_FN_TYPE h, IN LHASH_COMP_FN_TYPE c);
void lh_free(IN _LHASH *lh);
void *lh_insert(IN _LHASH *lh, IN void *data);
void *lh_delete(IN _LHASH *lh, IN const void *data);
void *lh_retrieve(IN _LHASH *lh, IN const void *data);
void lh_doall(IN _LHASH *lh, IN LHASH_DOALL_FN_TYPE func);
void lh_doall_arg(IN _LHASH *lh, IN LHASH_DOALL_ARG_FN_TYPE func, IN void *arg);
unsigned long lh_strhash(const char *c);
unsigned long lh_num_items(const _LHASH *lh);

# ifndef OPENSSL_NO_FP_API
void lh_stats(IN _LHASH *lh, IN FILE *out);
void lh_node_stats(IN _LHASH *lh, IN FILE *out);
void lh_node_usage_stats(IN _LHASH *lh, IN FILE *out);
# endif

/* Type checking... */

# define LHASH_OF(type) struct lhash_st_##type

# define DECLARE_LHASH_OF(type) LHASH_OF(type) { int dummy; }

# define CHECKED_LHASH_OF(type,lh) ((_LHASH *)CHECKED_PTR_OF(LHASH_OF(type),lh))

/* Define wrapper functions. */
# define LHM_lh_new(type, name)							((LHASH_OF(type) *)lh_new(LHASH_HASH_FN(name), LHASH_COMP_FN(name)))
# define LHM_lh_error(type, lh)							lh_error(CHECKED_LHASH_OF(type,lh))
# define LHM_lh_insert(type, lh, inst)					((type *)lh_insert(CHECKED_LHASH_OF(type, lh), CHECKED_PTR_OF(type, inst)))
# define LHM_lh_retrieve(type, lh, inst)				((type *)lh_retrieve(CHECKED_LHASH_OF(type, lh), CHECKED_PTR_OF(type, inst)))
# define LHM_lh_delete(type, lh, inst)					((type *)lh_delete(CHECKED_LHASH_OF(type, lh), CHECKED_PTR_OF(type, inst)))
# define LHM_lh_doall(type, lh,fn)						lh_doall(CHECKED_LHASH_OF(type, lh), fn)
# define LHM_lh_doall_arg(type, lh, fn, arg_type, arg)	lh_doall_arg(CHECKED_LHASH_OF(type, lh), fn, CHECKED_PTR_OF(arg_type, arg))
# define LHM_lh_num_items(type, lh)						lh_num_items(CHECKED_LHASH_OF(type, lh))
# define LHM_lh_down_load(type, lh)						(CHECKED_LHASH_OF(type, lh)->down_load)
# define LHM_lh_node_stats(type, lh, out)				lh_node_stats(CHECKED_LHASH_OF(type, lh), out)
# define LHM_lh_node_usage_stats(type, lh, out)			lh_node_usage_stats(CHECKED_LHASH_OF(type, lh), out)
# define LHM_lh_stats(type, lh, out)					lh_stats(CHECKED_LHASH_OF(type, lh), out)
# define LHM_lh_free(type, lh)							lh_free(CHECKED_LHASH_OF(type, lh))


#ifdef  __cplusplus
}
#endif

#endif
