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
AVL树核心就是插入删除后的平衡操作，失衡包括4种形式
1 > LL 左左	 根节点的左子树的左子树还有非空子节点，导致根的左子树的高度比根的右子树高度大2
2 >	LR 左右	 根节点的左子树的右子树还有非空子节点，导致根的左子树的高度比根的右子树高度大2
3 >	RR 右右	 根节点的右子树的右子树还有非空子节点，导致根的右子树的高度比根的左子树高度大2
4 > RL 右左	 根节点的右子树的左子树还有非空子节点，导致根的右子树的高度比根的左子树的高度大2

恢复失衡的操作如下：
1 > LL 单左旋  K2是根节点，K1是K2的左子节点  --  将k1变成根节点，k2变成k1的右子树，k1的右子树变成k2的左子树
2 > RR 单右旋  K1是根节点，K2是K1的右子节点  --	  将K2变成根节点，K1变成K2的左子树，K2的左子树变成K1的右子树
3 > LR 双旋转  先进行RR单右旋，然后再进行LL单左旋
4 > RL 双旋转  先进行LL单左旋，然后再进行RR单右旋

参考文章 http://www.cnblogs.com/skywang12345/p/3576969.html
参考文章 http://www.cnblogs.com/xiaotiaosi/p/6934724.html
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
@Purpose :	先序遍历AVL树
@Param   :	pTree		--	AVL tree pointer
@Param   :	data		--	data to delete from AVL tree
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
void AVLTree_TravelPre(IN AVLTree* pTree, IN VisitPTR visit);

/*************************************************************************
@Purpose :	中序遍历AVL树
@Param   :	pTree		--	AVL tree pointer
@Param   :	data		--	data to delete from AVL tree
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
void AVLTree_TravelIn(IN AVLTree* pTree, IN VisitPTR visit);

/*************************************************************************
@Purpose :	后序遍历AVL树
@Param   :	pTree		--	AVL tree pointer
@Param   :	data		--	data to delete from AVL tree
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
void AVLTree_TravelAfter(IN AVLTree* pTree, IN VisitPTR visit);

/*************************************************************************
@Purpose :	dump AVL树
@Param   :	pTree		--	AVL tree pointer
@Param   :	tostring	--	data to string function
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
void AVLTree_Dump(IN AVLTree* pTree, IN TOSTRING_PTR tostring);

/*************************************************************************
@Purpose :	检索数据
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


