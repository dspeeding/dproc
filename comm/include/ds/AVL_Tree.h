/*********************************************************************
*:
*:            Author: dspeeding
*:          Copyright (c) 2016
*:
*:		Created at: 2017.06.05
*:		Last modified: 2017.06.05
*:
*:      Introduction: AVL Tree
*:
*:		PS: data no not support deep copy
*:
*:
*:*********************************************************************/
#pragma once

typedef int(*AVLTREE_COMP_FN_TYPE)(const void *, const void *);

typedef char* (*TOSTRING_PTR)(const void*);

#define AVL_HEIGHT(p)	(((p) == NULL) ? -1 : ((AVLTreeNode*)(p))->height)
#define AVL_MAX(a,b)	((a) > (b) ? (a) : (b))

typedef struct AVLTreeNode_ST
{
	int						height;		//height
	void*					data;		//data
	struct AVLTreeNode_ST*	left;		//left child
	struct AVLTreeNode_ST*	right;		//right child
}AVLTreeNode;

typedef struct AVLTree_ST
{
	AVLTREE_COMP_FN_TYPE	comp;
	AVLTreeNode*			root;
}AVLTree;

#ifdef __cplusplus
extern "C"
{
#endif

/*
AVL�����ľ��ǲ���ɾ�����ƽ�������ʧ�����4����ʽ
1 > LL ����	 ���ڵ�������������������зǿ��ӽڵ㣬���¸����������ĸ߶ȱȸ����������߶ȴ�2
2 >	LR ����	 ���ڵ�������������������зǿ��ӽڵ㣬���¸����������ĸ߶ȱȸ����������߶ȴ�2
3 >	RR ����	 ���ڵ�������������������зǿ��ӽڵ㣬���¸����������ĸ߶ȱȸ����������߶ȴ�2
4 > RL ����	 ���ڵ�������������������зǿ��ӽڵ㣬���¸����������ĸ߶ȱȸ����������ĸ߶ȴ�2

�ָ�ʧ��Ĳ������£�
1 > LL ������  K2�Ǹ��ڵ㣬K1��K2�����ӽڵ�  --  ��k1��ɸ��ڵ㣬k2���k1����������k1�����������k2��������
2 > RR ������  K1�Ǹ��ڵ㣬K2��K1�����ӽڵ�  --	  ��K2��ɸ��ڵ㣬K1���K2����������K2�����������K1��������
3 > LR ˫��ת  �Ƚ���RR��������Ȼ���ٽ���LL������
4 > RL ˫��ת  �Ƚ���LL��������Ȼ���ٽ���RR������

�ο����� http://www.cnblogs.com/skywang12345/p/3576969.html
�ο����� http://www.cnblogs.com/xiaotiaosi/p/6934724.html
*/


/*************************************************************************
@Purpose :	AVL tree init
@Param   :	ppTree		--	AVL tree pointer
@Param   :	comp		--	comp function
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int	AVLTree_Init(OUT AVLTree** ppTree, IN AVLTREE_COMP_FN_TYPE comp);

/*************************************************************************
@Purpose :	AVL tree destory and free
@Param   :	pTree		--	AVL tree pointer
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void AVLTree_Destroy(IN AVLTree* pTree);

/*************************************************************************
@Purpose :	AVL tree insert data
@Param   :	pTree		--	AVL tree pointer
@Param   :	data		--	data to insert to AVL tree
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int	AVLTree_Insert(IN AVLTree* pTree, IN void* data);

/*************************************************************************
@Purpose :	AVL tree delete data
@Param   :	pTree		--	AVL tree pointer
@Param   :	data		--	data to delete from AVL tree
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int	AVLTree_Delete(IN AVLTree* pTree, IN void* data, OUT void** pOldData);

/*************************************************************************
@Purpose :	�������AVL��
@Param   :	pTree		--	AVL tree pointer
@Param   :	data		--	data to delete from AVL tree
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
void AVLTree_TravelPre(IN AVLTree* pTree, IN VisitPTR visit);

/*************************************************************************
@Purpose :	�������AVL��
@Param   :	pTree		--	AVL tree pointer
@Param   :	data		--	data to delete from AVL tree
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
void AVLTree_TravelIn(IN AVLTree* pTree, IN VisitPTR visit);

/*************************************************************************
@Purpose :	�������AVL��
@Param   :	pTree		--	AVL tree pointer
@Param   :	data		--	data to delete from AVL tree
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
void AVLTree_TravelAfter(IN AVLTree* pTree, IN VisitPTR visit);

/*************************************************************************
@Purpose :	dump AVL��
@Param   :	pTree		--	AVL tree pointer
@Param   :	tostring	--	data to string function
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
void AVLTree_Dump(IN AVLTree* pTree, IN TOSTRING_PTR tostring);

/*************************************************************************
@Purpose :	��������
@Param   :	pTree		--	AVL tree pointer
@Param   :	data		--	data which content the key to find data
@Param	 :	pData		--	the point to save data which will find
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int AVLTree_Retrieve(IN AVLTree* pTree, IN void* data, OUT void** pData);


#ifdef __cplusplus
}
#endif


