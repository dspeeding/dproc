#ifndef HEADER_LHASH_H
#define HEADER_LHASH_H

#include <stdio.h>

#ifdef  __cplusplus
extern "C" {
#endif



#  define CHECKED_PTR_OF(type, p) \
    ((void*) (1 ? p : (type*)0))

	
//��ϣ�����ݽṹ
typedef struct lhash_node_st 
{
    void *data;						//���ݵ�ַ
    struct lhash_node_st *next;		//��һ�����ݵ�ַ
# ifndef OPENSSL_NO_HASH_COMP		
    unsigned long hash;				//���ݹ�ϣ����ֵ
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
����p��pmax��;������ϣ���Ѿ�������ʱ�򣬷�����ͻ�ĸ��ʸ��󣬷�������ʱ
Ҫ�������������һ�飬�������ܻ�ܵͣ�openssl�в��õķ����ǽ������������һЩ
����Щ�����Ԫ���������Ĳ����ƶ�һЩ���������ܲ����ܺõ�Ч����ͨ��װ��������
�жϹ�ϣ���Ƿ������װ������ a = num_items/num_nodes
************************************************************************/


typedef struct lhash_st {
    LHASH_NODE **b;										//��ϣָ�����飬������ÿ��ֵΪ��ϣ�����е��׵�ַ
    LHASH_COMP_FN_TYPE comp;							//���ݱȽϺ�����ַ
    LHASH_HASH_FN_TYPE hash;							//�����ϣֵ�����ĵ�ַ
    unsigned int num_nodes;								//��ʾ�������
    unsigned int num_alloc_nodes;						//��ʾ����ռ���������ڵ���num_nodes
    unsigned int p;										//
    unsigned int pmax;
    unsigned long up_load;								/* load times 256 */
    unsigned long down_load;							/* load times 256 */
    unsigned long num_items;							//���е����ݸ���
    unsigned long num_expands;							//������ü�����
    unsigned long num_expand_reallocs;					//
    unsigned long num_contracts;
    unsigned long num_contract_reallocs;
    unsigned long num_hash_calls;						//�����ϣ�������ü�����
    unsigned long num_comp_calls;						//�ȽϺ������ü�����
    unsigned long num_insert;							//�������ݼ�����
    unsigned long num_replace;							//��ͬ�����滻������
    unsigned long num_delete;							//ɾ�����ݼ�����
    unsigned long num_no_delete;						//ɾ��ʧ�ܼ�����
    unsigned long num_retrieve;							//�������ݼ�����
    unsigned long num_retrieve_miss;					//����ʧ�ܼ�����
    unsigned long num_hash_comps;						//�Աȼ�����
    int error;											//�������
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
