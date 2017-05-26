#include "basetype.h"
#include "ds/BinaryTree.h"

/*************************************************************************
@Purpose :	binary tree init
@Param   :	ppTree		--	binary tree pointer
@Param   :	comp		--	comp function
@Param	 :	nDataSize	--	data size
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int	BiTree_Init(OUT BinaryTree** ppTree, IN BITREE_COMP_FN_TYPE comp, IN int nDataSize)
{
	BinaryTree* pTree;

	CHECK_POINTER(ppTree, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(comp, ERR_COMM_NULL_POINTER);
	if (nDataSize <= 0)
	{
		LOG_E("param nDataSize error, nDataSize=[%d]", nDataSize);
		return ERR_COMM_PARAM;
	}

	*ppTree = (BinaryTree*)Mem_Malloc(sizeof(BinaryTree));
	if (*ppTree == NULL)
	{
		LOG_E("malloc *ppTree error,err=[%#x]", CF_GetLastError());
		return ERR_COMM_MEM_MALLOC;
	}

	memset(*ppTree, 0, sizeof(BinaryTree));

	pTree = *ppTree;

	pTree->nDataSize = nDataSize;
	pTree->comp = comp;
	pTree->root = NULL;

	return ERR_OK;
}

static void BiTreeNode_Free(BiTreeNode* node)
{
	if (node)
	{
		BiTreeNode_Free(node->left);
		BiTreeNode_Free(node->right);
		if (node->data)
		{
			Mem_Free(node->data);
		}
		Mem_Free(node);
	}
}

/*************************************************************************
@Purpose :	binary tree destory and free
@Param   :	pTree		--	binary tree pointer
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void BiTree_Destroy(IN BinaryTree* pTree)
{
	if (pTree == NULL)
	{
		return;
	}

	BiTreeNode_Free(pTree->root);
	Mem_Free(pTree);
}

static int BiTreeNode_Insert(IN BiTreeNode** ppNode, IN BITREE_COMP_FN_TYPE comp, IN void* data, IN int nDataSize)
{
	BiTreeNode*		pNode;

	if (*ppNode == NULL)
	{
		*ppNode = (BiTreeNode *)Mem_Malloc(sizeof(BiTreeNode));
		if (*ppNode == NULL)
		{
			LOG_E("malloc *node error, err=[%#x]", CF_GetLastError());
			return ERR_COMM_MEM_MALLOC;
		}

		pNode = *ppNode;
		memset(pNode, 0, sizeof(BiTreeNode));
		pNode->data = (void*)Mem_Malloc(nDataSize);
		if (pNode->data == NULL)
		{
			LOG_E("malloc data error, err=[%#x]", CF_GetLastError());
			return ERR_COMM_MEM_MALLOC;
		}
		memset(pNode->data, 0, nDataSize);
		memcpy(pNode->data, data, nDataSize);
		pNode->left = NULL;
		pNode->right = NULL;
		return ERR_OK;
	}

	if (comp(data, (*ppNode)->data) <= 0)
	{
		return BiTreeNode_Insert(&((*ppNode)->left), comp, data);
	}
	else
	{
		return BiTreeNode_Insert(&((*ppNode)->right), comp, data);
	}
}

/*************************************************************************
@Purpose :	binary tree insert data
@Param   :	pTree		--	binary tree pointer
@Param   :	data		--	data to insert to binary tree
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int	BiTree_Insert(IN BinaryTree* pTree, IN void* data)
{
	int	nRet;

	CHECK_POINTER(pTree, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(data, ERR_COMM_NULL_POINTER);

	nRet = BiTreeNode_Insert(&(pTree->root), pTree->comp, data);
	if (nRet != ERR_OK)
	{
		LOG_E("BiTreeNode_Insert error, err=[%#x]", nRet);
		return nRet;
	}

	return ERR_OK;
}
