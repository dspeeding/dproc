#pragma once


#define LINK_HASH_LOAD_MULT		256

#define LINK_HASH_MIN_NODES		16
#define LINK_HASH_UP_LOAD		(2*LINK_HASH_LOAD_MULT)
#define LINK_HASH_DOWN_LOAD		(LINK_HASH_LOAD_MULT)

typedef struct LINK_HASH_NODE_ST
{
	void*						data;	//��������
	struct LINK_HASH_NODE_ST*	next;	//��һ���ڵ�
	unsigned long				hash;	//���ݹ�ϣ����ֵ
}LINK_HASH_NODE;

typedef int (*LINKHASH_COMP_FN_TYPE)(const void *, const void *);
typedef unsigned long (*LINKHASH_HASH_FN_TYPE)(const void *);
typedef void (*LINKHASH_DOALL_FN_TYPE)(void *);
typedef void(*LINKHASH_DOALL_ARG_FN_TYPE)(void*, void*);


/************************************************************************
����p��pmax��;������ϣ���Ѿ�������ʱ�򣬷�����ͻ�ĸ��ʸ��󣬷�������ʱ
Ҫ�������������һ�飬�������ܻ�ܵͣ�openssl�в��õķ����ǽ������������һЩ
����Щ�����Ԫ���������Ĳ����ƶ�һЩ���������ܲ����ܺõ�Ч����ͨ��װ��������
�жϹ�ϣ���Ƿ������װ������ a = num_items/num_nodes
************************************************************************/

typedef struct LINK_HASH_ST
{
	LINK_HASH_NODE **b;								//��ϣָ�����飬������ÿ��ֵΪ��ϣ�����е��׵�ַ
	LINKHASH_COMP_FN_TYPE	comp;					//���ݱȽϺ���
	LINKHASH_HASH_FN_TYPE	hash;					//�����ϣֵ�����ĵ�ַ
	unsigned int			num_nodes;				//��ʾ�������
	unsigned int			num_alloc_nodes;		//��ʾ����ռ���������ڵ���num_nodes
	unsigned int			p;						//
	unsigned int			pmax;					//
	unsigned int			up_load;				//
	unsigned int			down_load;				//
	unsigned long			num_items;				//���е����ݸ���
	unsigned long			num_expands;			//������ü�����
	unsigned long			num_expand_reallocs;	//
	unsigned long			num_contracts;
	unsigned long			num_contract_reallocs;
	unsigned long			num_hash_calls;			//�����ϣ�������ü�����
	unsigned long			num_comp_calls;			//�ȽϺ������ü�����
	unsigned long			num_insert;				//�������ݼ�����
	unsigned long			num_replace;			//��ͬ�����滻������
	unsigned long			num_delete;				//ɾ�����ݼ�����
	unsigned long			num_no_delete;			//ɾ��ʧ�ܼ�����
	unsigned long			num_retrieve;			//�������ݼ�����
	unsigned long			num_retrieve_miss;		//����ʧ�ܼ�����
	unsigned long			num_hash_comps;			//�Աȼ�����
	int						error;					//�������
}LINK_HASH;

#ifdef __cplusplus
extern "C"
{
#endif

/*************************************************************************
@Purpose :	init Link Hash
@Param   :	ppHash	    -- hash pointer
@Param   :	h			-- ��ϣ����ָ��
@Param   :	c			-- �ȽϺ���ָ��
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :	��� c Ϊ�գ���Ĭ��ʹ��strcmp ����
			��� h Ϊ�գ���Ĭ��ʹ�� LinkHash_Str ����
*************************************************************************/
int LH_Init(OUT LINK_HASH** ppHash, IN LINKHASH_HASH_FN_TYPE h, IN LINKHASH_COMP_FN_TYPE c);

/*************************************************************************
@Purpose :	�ͷŹ�ϣ����
@Param   :	pHash	    -- hash pointer
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void LH_Destory(IN LINK_HASH* pHash);

/*************************************************************************
@Purpose :	��������
@Param   :	pHash	    -- hash pointer
@Param   :	data		-- �����������
@Param   :	old_data	-- �������ͬ��ֵ���򷵻�ԭ����ֵ������Ϊ��
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :	���سɹ��󣬻���Ҫ�ж�һ��old_data �Ƿ�ΪNULL�������ΪNULL��Ҫ����һ������
			�����ͷ�old_data���ڴ�֮��
*************************************************************************/
int LH_Insert(IN LINK_HASH* pHash, IN void* data, OUT void** old_data);

/*************************************************************************
@Purpose :	ɾ������
@Param   :	pHash	    -- hash pointer
@Param   :	data		-- ��ɾ��������
@Param   :	old_data	-- ���ص�ֵ
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :	���سɹ�����Ҫ��old_data����һ�����������ͷ�old_data���ڴ�֮��
*************************************************************************/
int LH_Delete(IN LINK_HASH* pHash, IN const void* data, OUT void* old_data);

/*************************************************************************
@Purpose :	��������
@Param   :	pHash	    -- hash pointer
@Param   :	data		-- ����ѯ������
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int LH_Retrieve(IN LINK_HASH* pHash, IN const void* data);

/*************************************************************************
@Purpose :  ����������ִ�в���
@Param   :  pHash	--	��ϣ��ָ��
@Param   :	func	--	ִ�в����ĺ�����ַ
@Return  :  None
@Modify  :
@Remark  :
*************************************************************************/
void LH_DoAll(IN LINK_HASH *pHash, IN LINKHASH_DOALL_FN_TYPE func);

/*************************************************************************
@Purpose :  ����������ִ�в�������Ҫ������
@Param   :  pHash	--	��ϣ��ָ��
@Param   :	func	--	ִ�в����ĺ�����ַ
@Param   :	arg		--	ִ�к���ʱ��Ҫ�Ĳ���
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
