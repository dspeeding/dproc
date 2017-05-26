/*********************************************************************
*:
*:            Author: dspeeding
*:          Copyright (c) 2016
*:
*:		Created at: 2016.01.05
*:		Last modified: 2016.01.05
*:
*:      Introduction: Binary Tree
*:
*:		PS: data no not support deep copy
*:		
*:
*:*********************************************************************/
#pragma once

typedef int(*BITREE_COMP_FN_TYPE)(const void *, const void *);

typedef struct BiTreeNode_ST
{
	void*					data;		//data
	struct BiTreeNode_ST*	left;		//left child
	struct BiTreeNode_ST*	right;		//right child
}BiTreeNode;

typedef struct BinaryTree_ST
{
	int						nDataSize;
	BITREE_COMP_FN_TYPE		comp;
	BiTreeNode*				root;
}BinaryTree;

