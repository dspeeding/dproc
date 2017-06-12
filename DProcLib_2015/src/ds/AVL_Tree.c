#include "basetype.h"
#include "ds/AVL_Tree.h"

static void AVLTreeNode_Free(AVLTreeNode* node)
{
	if (node)
	{
		AVLTreeNode_Free(node->left);
		AVLTreeNode_Free(node->right);
		if (node->data)
		{
			Mem_Free(node->data);
		}
		Mem_Free(node);
	}
}

//LL 与左子节点单旋转（以根为基准）
static AVLTreeNode* AVLTree_RotateWithLeftChild(IN AVLTreeNode* k1)
{
	AVLTreeNode*	k2;

	k2 = k1->left;
	k1->left = k2->right;
	k2->right = k1;

	k1->height = AVL_MAX(AVL_HEIGHT(k1->left), AVL_HEIGHT(k1->right)) + 1;
	k2->height = AVL_MAX(AVL_HEIGHT(k2->left), k1->height) + 1;
	return k2;
}

//RR 与右子节点单旋转（以根为基准）
static AVLTreeNode* AVLTree_RotateWithRightChild(IN AVLTreeNode* k1)
{
	AVLTreeNode*	k2;

	k2 = k1->right;
	k1->right = k2->left;
	k2->left = k1;

	k1->height = AVL_MAX(AVL_HEIGHT(k1->left), AVL_HEIGHT(k1->right)) + 1;
	k2->height = AVL_MAX(AVL_HEIGHT(k2->right), k1->height) + 1;
	return k2;
}

//LR 双旋转 先根的左子节点和其右子节点旋转，然后根节点与左子节点单旋转
static AVLTreeNode* AVLTree_DoubleWithLeftChild(AVLTreeNode* k1)
{
	AVLTreeNode*	k2;

	//将双旋转转换为之前习惯的单旋转情况
	k1->left = AVLTree_RotateWithRightChild(k1->left);
	//然后通过调用一次与左子节点单旋转
	k2 = AVLTree_RotateWithLeftChild(k1);

	return k2;
}

//RL 双旋转 
static AVLTreeNode* AVLTree_DoubleWithRightChild(AVLTreeNode* k1)
{
	AVLTreeNode*	k2;

	//将双旋转转换为之前习惯的单旋转的情况
	k1->right = AVLTree_RotateWithLeftChild(k1->right);

	k2 = AVLTree_RotateWithRightChild(k1);

	return k2;
}

static int AVLTree_Balance(IN AVLTreeNode** ppNode)
{
	AVLTreeNode*	pNode;

	if (*ppNode == NULL)
	{
		return ERR_OK;
	}

	pNode = *ppNode;

	if (AVL_HEIGHT(pNode->left) - AVL_HEIGHT(pNode->right) > 1)
	{
		if (AVL_HEIGHT(pNode->left->left) > AVL_HEIGHT(pNode->left->right))
		{
			*ppNode = AVLTree_RotateWithLeftChild(pNode);
		}
		else
		{
			*ppNode = AVLTree_DoubleWithLeftChild(pNode);
		}
	}
	else if (AVL_HEIGHT(pNode->right) - AVL_HEIGHT(pNode->left) > 1)
	{
		if (AVL_HEIGHT(pNode->right->left) < AVL_HEIGHT(pNode->right->right))
		{
			*ppNode = AVLTree_RotateWithRightChild(pNode);
		}
		else
		{
			*ppNode = AVLTree_DoubleWithRightChild(pNode);
		}
	}

	pNode = *ppNode;
	pNode->height = AVL_MAX(AVL_HEIGHT(pNode->left), AVL_HEIGHT(pNode->right)) + 1;

	return ERR_OK;
}

static int AVLTreeNode_Insert(OUT AVLTreeNode** ppNode, IN AVLTREE_COMP_FN_TYPE comp, IN void* data)
{
	AVLTreeNode*		pNode;
	int					ret;
	int					comp_ret;

	if (*ppNode == NULL)
	{
		*ppNode = (AVLTreeNode *)Mem_Malloc(sizeof(AVLTreeNode));
		if (*ppNode == NULL)
		{
			LOG_E("malloc *node error, err=[%#x]", CF_GetLastError());
			return ERR_COMM_MEM_MALLOC;
		}

		pNode = *ppNode;
		memset(pNode, 0, sizeof(AVLTreeNode));
		pNode->height = 0;
		pNode->data = data;
		pNode->left = NULL;
		pNode->right = NULL;
		return ERR_OK;
	}

	comp_ret = comp(data, (*ppNode)->data);
	if (comp_ret < 0)
	{
		//插入到左子树
		ret = AVLTreeNode_Insert(&((*ppNode)->left), comp, data);
		if (ret != ERR_OK)
		{
			LOG_E("AVLTreeNode_Insert error,err=[%#x]", ret);
			return ret;
		}
	}
	else if (comp_ret > 0)
	{
		//插入到右子树
		ret = AVLTreeNode_Insert(&((*ppNode)->right), comp, data);
		if (ret != ERR_OK)
		{
			LOG_E("AVLTreeNode_Insert error,err=[%#x]", ret);
			return ret;
		}
	}
	else
	{
		LOG_E("do not support the same value to tree");
		return ERR_COMM_PARAM;
	}

	AVLTree_Balance(ppNode);

	return ERR_OK;
}

static AVLTreeNode* AVLTree_FindMin(IN AVLTreeNode* pNode)
{
	if (pNode == NULL)
	{
		return NULL;
	}

	while (pNode->left != NULL)
	{
		pNode = pNode->left;
	}
	return pNode;
}

static AVLTreeNode* AVLTree_FindMax(IN AVLTreeNode* pNode)
{
	if (pNode == NULL)
	{
		return NULL;
	}

	while (pNode->right != NULL)
	{
		pNode = pNode->right;
	}
	return pNode;
}

static int AVLTreeNode_Delete(OUT AVLTreeNode** ppNode, IN AVLTREE_COMP_FN_TYPE comp, IN void* data, OUT void** pOldData)
{
	AVLTreeNode*		pMinNode;
	AVLTreeNode*		pNode;
	AVLTreeNode*		tmp;
	int					ret;
	int					comp_ret;

	pNode = *ppNode;

	if (pNode == NULL)
	{
		LOG_E("malloc *node error, err=[%#x]", CF_GetLastError());
		return ERR_COMM_MEM_MALLOC;
	}

	comp_ret = comp(data, pNode->data);
	if (comp_ret < 0)
	{
		//到左子树
		ret = AVLTreeNode_Delete(&(pNode->left), comp, data, pOldData);
		if (ret != ERR_OK)
		{
			LOG_E("AVLTreeNode_Insert error,err=[%#x]", ret);
			return ret;
		}
	}
	else if (comp_ret > 0)
	{
		//到右子树
		ret = AVLTreeNode_Delete(&(pNode->right), comp, data, pOldData);
		if (ret != ERR_OK)
		{
			LOG_E("AVLTreeNode_Insert error,err=[%#x]", ret);
			return ret;
		}
	}
	else
	{
		if (*pOldData == NULL)
		{
			*pOldData = pNode->data;
		}

		if (pNode->left != NULL && pNode->right != NULL)
		{
			//如果左右子树都不空
			//找到左子树的最大元素或者右子树的最小元素来填充这个位置，找到右子树最小的
			pMinNode = AVLTree_FindMin(pNode->right);
			pNode->data = pMinNode->data;
			AVLTreeNode_Delete(&pNode->right, comp, pMinNode->data, pOldData);
		}
		else
		{
			//如果左子树为空，直接将右子树的节点代替根
			tmp = pNode;
			*ppNode = (pNode->left != NULL) ? pNode->left : pNode->right;
			Mem_Free(tmp);
		}
	}

	AVLTree_Balance(ppNode);

	return ERR_OK;
}

static AVLTreeNode_TravelPre(AVLTreeNode* pNode, IN VisitPTR visit)
{
	if (pNode != NULL)
	{
		visit(pNode);
		AVLTreeNode_TravelPre(pNode->left, visit);
		AVLTreeNode_TravelPre(pNode->right, visit);
	}
}

static void AVLTreeNode_TravelIn(AVLTreeNode* pNode, IN VisitPTR visit)
{
	if (pNode != NULL)
	{
		AVLTreeNode_TravelIn(pNode->left, visit);
		visit(pNode);
		AVLTreeNode_TravelIn(pNode->right, visit);
	}
}

static void AVLTreeNode_TravelAfter(AVLTreeNode* pNode, IN VisitPTR visit)
{
	if (pNode != NULL)
	{
		AVLTreeNode_TravelAfter(pNode->left, visit);
		AVLTreeNode_TravelAfter(pNode->right, visit);
		visit(pNode);
	}
}

static void AVLTreeNode_Dump(IN AVLTreeNode* pNode, void* data, int direction, IN TOSTRING_PTR tostring)
{
	if (pNode == NULL)
	{
		return;
	}

	if (direction == 0)
	{
		LOG_D("node [%s] is root", tostring(pNode->data));
	}
	else
	{
		LOG_D("node [%s] is [%s]'s %s child", tostring(pNode->data), tostring(data), direction == 1 ? "right" : "left");
	}

	AVLTreeNode_Dump(pNode->left, pNode->data, -1, tostring);
	AVLTreeNode_Dump(pNode->right, pNode->data, 1, tostring);
}

static int AVLTreeNode_Retrieve(IN AVLTreeNode* pNode, IN AVLTREE_COMP_FN_TYPE comp, IN void* data, OUT void** pData)
{
	int			ret;

	if (pNode == NULL)
	{
		return ERR_COMM_NOT_FIND_DATA;
	}

	ret = comp(pNode->data, data);
	if (ret == 0)
	{
		*pData = pNode->data;
		return ERR_OK;
	}
	else if (ret > 0)
	{
		return AVLTreeNode_Retrieve(pNode->left, comp, data, pData);
	}
	else
	{
		return AVLTreeNode_Retrieve(pNode->right, comp, data, pData);
	}
}

/*************************************************************************
@Purpose :	AVL tree init
@Param   :	ppTree		--	AVL tree pointer
@Param   :	comp		--	comp function
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int	AVLTree_Init(OUT AVLTree** ppTree, IN AVLTREE_COMP_FN_TYPE comp)
{
	AVLTree* pTree;

	CHECK_POINTER(ppTree, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(comp, ERR_COMM_NULL_POINTER);

	*ppTree = (AVLTree*)Mem_Malloc(sizeof(AVLTree));
	if (*ppTree == NULL)
	{
		LOG_E("malloc *ppTree error,err=[%#x]", CF_GetLastError());
		return ERR_COMM_MEM_MALLOC;
	}

	memset(*ppTree, 0, sizeof(AVLTree));

	pTree = *ppTree;
	pTree->comp = comp;
	pTree->root = NULL;

	return ERR_OK;
}

/*************************************************************************
@Purpose :	AVL tree destory and free
@Param   :	pTree		--	AVL tree pointer
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void AVLTree_Destroy(IN AVLTree* pTree)
{
	if (pTree == NULL)
	{
		return;
	}

	AVLTreeNode_Free(pTree->root);
	Mem_Free(pTree);
}

/*************************************************************************
@Purpose :	AVL tree insert data
@Param   :	pTree		--	AVL tree pointer
@Param   :	data		--	data to insert to AVL tree
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int	AVLTree_Insert(IN AVLTree* pTree, IN void* data)
{
	int	nRet;

	CHECK_POINTER(pTree, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(data, ERR_COMM_NULL_POINTER);

	nRet = AVLTreeNode_Insert(&(pTree->root), pTree->comp, data);
	if (nRet != ERR_OK)
	{
		LOG_E("AVLTreeNode_Insert error, err=[%#x]", nRet);
		return nRet;
	}

	return ERR_OK;
}

/*************************************************************************
@Purpose :	AVL tree delete data
@Param   :	pTree		--	AVL tree pointer
@Param   :	data		--	data to delete from AVL tree
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int	AVLTree_Delete(IN AVLTree* pTree, IN void* data, OUT void** pOldData)
{
	int	nRet;

	CHECK_POINTER(pTree, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(data, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(pOldData, ERR_COMM_NULL_POINTER);

	nRet = AVLTreeNode_Delete(&pTree->root, pTree->comp, data, pOldData);
	if (nRet != ERR_OK)
	{
		LOG_E("AVLTreeNode_Delete error, err=[%#x]", nRet);
		return nRet;
	}

	return ERR_OK;
}

/*************************************************************************
@Purpose :	先序遍历AVL树
@Param   :	pTree		--	AVL tree pointer
@Param   :	data		--	data to delete from AVL tree
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
void AVLTree_TravelPre(IN AVLTree* pTree, IN VisitPTR visit)
{
	AVLTreeNode_TravelPre(pTree->root, visit);
}

/*************************************************************************
@Purpose :	中序遍历AVL树
@Param   :	pTree		--	AVL tree pointer
@Param   :	data		--	data to delete from AVL tree
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
void AVLTree_TravelIn(IN AVLTree* pTree, IN VisitPTR visit)
{
	AVLTreeNode_TravelIn(pTree->root, visit);
}

/*************************************************************************
@Purpose :	后序遍历AVL树
@Param   :	pTree		--	AVL tree pointer
@Param   :	data		--	data to delete from AVL tree
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
void AVLTree_TravelAfter(IN AVLTree* pTree, IN VisitPTR visit)
{
	AVLTreeNode_TravelAfter(pTree->root, visit);
}

/*************************************************************************
@Purpose :	dump AVL树
@Param   :	pTree		--	AVL tree pointer
@Param   :	tostring	--	data to string function
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
void AVLTree_Dump(IN AVLTree* pTree, IN TOSTRING_PTR tostring)
{
	AVLTreeNode_Dump(pTree->root, NULL, 0, tostring);
}

/*************************************************************************
@Purpose :	检索数据
@Param   :	pTree		--	AVL tree pointer
@Param   :	data		--	data which content the key to find data
@Param	 :	pData		--	the point to save data which will find
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int AVLTree_Retrieve(IN AVLTree* pTree, IN void* data, OUT void** pData)
{
	CHECK_POINTER(pTree, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(data, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(pData, ERR_COMM_NULL_POINTER);

	return AVLTreeNode_Retrieve(pTree->root, pTree->comp, data, pData);
}