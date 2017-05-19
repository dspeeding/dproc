/*********************************************************************
*:
*:            Author: dspeeding
*:          Copyright (c) 2016
*:
*:		Created at: 2016.01.05
*:		Last modified: 2016.01.05
*:
*:      Introduction: array list
*:
*:		PS: data no not support deep copy
*:		need to realize the function of Visit is necessary
*:		need to realize the function of Compare is necessary
*:
*:*********************************************************************/
#pragma once

typedef struct _TDEF_SQ_LIST
{
	void*	list;			//data list header pointer
	int		dataSize;		//data size
	int		curNum;			//cur data num
	int		maxNum;			//max data num
}SqList;

typedef SqList* PSqList;
typedef const SqList CSqList;
typedef const SqList* PCSqlist;


#ifdef __cplusplus
extern "C"
{
#endif

/*************************************************************************
@Purpose :	list init
@Param   :	ppList		--	sqlist
@Param   :	dataSize	--	data size
@Param   :	maxNum		--	max num
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int	InitList(IN PSqList* ppList, IN int dataSize, IN int maxNum);

/*************************************************************************
@Purpose :	clear list
@Param   :	pList		--	sqlist
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void ClearList(IN PSqList pList);

/*************************************************************************
@Purpose :	destroy list, free mem
@Param   :	pList		--	sqlist
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void DestroyList(IN PSqList pList);

/*************************************************************************
@Purpose :	get list length
@Param   :	pList		--	sqlist
@Return  :	the length of the list
@Modify  :
@Remark  :
*************************************************************************/
int GetListLength(IN PSqList pList);

/*************************************************************************
@Purpose :	judge list is empty
@Param   :	pList		--	sqlist
@Return  :	1--empty  0--has data
@Modify  :
@Remark  :
*************************************************************************/
int IsListEmpty(IN PSqList pList);

/*************************************************************************
@Purpose :	get the nPos's data value
@Param   :	pList		--	sqlist
@Param   :	nPos		--	the data position [0-GetListLength(pList)-1]
@Param   :	pVal		--	the data
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int GetElem(IN PSqList pList, IN int nPos, OUT void* pVal);

/*************************************************************************
@Purpose :	traverse every element of list use proc visit
@Param   :	pList		--	sqlist
@Param   :	visit		--	the traverse funciton,
@Param   :	flag		--	flag
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :	0--visit fail continue, 1--visit fail break
visit: the function return 0 mean success
*************************************************************************/
int TraverseList(IN PSqList pList, IN VisitPTR visit, IN int flag);

/*************************************************************************
@Purpose :	insert element to list specify by position
@Param   :	pList		--	sqlist
@Param   :	nPos		--	insert position [0 ~ GetListLength(pList)]
@Param   :	data		--	insert data
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int InsertList(IN PSqList pList, IN int nPos, IN void* data);

/*************************************************************************
@Purpose :	delete element from list specify by position
@Param   :	pList		--	sqlist
@Param   :	nPos		--	delete position [0 ~ GetListLength(pList)-1]
@Param   :	data		--	delete data
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int DeleteList(IN PSqList pList, IN int nPos, OUT void* data);

/*************************************************************************
@Purpose :	update element from list specify by position
@Param   :	pList		--	sqlist
@Param   :	nPos		--	delete position [0 ~ GetListLength(pList)-1]
@Param   :	data		--	delete data
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int UpdateList(IN PSqList pList, IN int nPos, IN void* data);

/*************************************************************************
@Purpose :	sorter list
@Param   :	pList		--	sqlist
@Param   :	compare		--	the compare function
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int SortList(IN PSqList pList, IN ComparePTR compare);

#ifdef __cplusplus
}
#endif

