/*********************************************************************
*:
*:            Author: dspeeding
*:          Copyright (c) 2016
*:
*:		Created at: 2016.01.05
*:		Last modified: 2016.01.05
*:
*:      Introduction: link list
*:
*:		PS: data no not support deep copy
*:		need to realize the function of Visit if necessary
*:		need to realize the function of Compare if necessary
*:
*:*********************************************************************/
#pragma once

typedef struct _TDEF_LINK_LIST
{
	void*	data;			//data list header pointer
	int		dataSize;		//data size
	struct _TDEF_LINK_LIST* next;	//the next node's pointer
}LinkList;

typedef LinkList*	PLinkList;

#ifdef __cplusplus
extern "C"
{
#endif

/*************************************************************************
@Purpose :	list init
@Param   :	ppList		--	pList
@Param   :	dataSize	--	data size
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int	InitLinkList(IN PLinkList* ppList, IN int dataSize);

/*************************************************************************
@Purpose :	clear init
@Param   :	pList		--	pList
@Param   :	dataSize	--	data size
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
void ClearLinkList(IN PLinkList pList);

/*************************************************************************
@Purpose :	destroy list, clean memory
@Param   :	pList		--	pList
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void DestroyLinkList(IN PLinkList pList);

/*************************************************************************
@Purpose :	get the length of list
@Param   :	pList		--	pList
@Return  :	len
@Modify  :
@Remark  :
*************************************************************************/
int GetLinkListLength(IN PLinkList pList);

/*************************************************************************
@Purpose :	get the value of list by position
@Param   :	pList		--	pList
@Param   :	nPos		--	the position
@Param   :	val			--	the out val
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int GetLinkListElem(IN PLinkList pList, IN int nPos, OUT void* val);

/*************************************************************************
@Purpose :	insert val to list by position
@Param   :	pList		--	pList
@Param   :	nPos		--	the position
@Param   :	val			--	the out val
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int InsertLinkList(IN PLinkList pList, IN int nPos, IN void* val);

/*************************************************************************
@Purpose :	delete element from list by position
@Param   :	pList		--	pList
@Param   :	nPos		--	the position
@Param   :	val			--	the out val
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int DeleteLinkList(IN PLinkList pList, IN int nPos, OUT void* val);

/*************************************************************************
@Purpose :	get the first of same element's position
@Param   :	pList		--	pList
@Param   :	val			--	the out val
@Param   :	compare		--	the function of compare
@Param   :	pnPos		--	the position
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int LocateLinkListElem(IN PLinkList pList, IN void* val, IN ComparePTR compare, OUT int* pnPos);

/*************************************************************************
@Purpose :	traverse every element of list use proc visit
@Param   :	pList		--	pList
@Param   :	visit		--	the traverse funciton,
@Param   :	flag		--	flag
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :	0--visit fail continue, 1--visit fail break
			visit: the function return 0 mean success
*************************************************************************/
int TraverseLinkList(IN PLinkList pList, IN VisitPTR visit, IN int flag);

/*************************************************************************
@Purpose :	update val to list by position
@Param   :	pList		--	pList
@Param   :	nPos		--	the position
@Param   :	val			--	the out val
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int UpdateLinkList(IN PLinkList pList, IN int nPos, IN void* val);

/*************************************************************************
@Purpose :	sorter list
@Param   :	pList		--	pList
@Param   :	compare		--	the compare function
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int SortLinkList(IN PLinkList pList, IN ComparePTR compare);

/*************************************************************************
@Purpose :	reverse list
@Param   :	pList		--	pList
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int ReverseLinkList(IN PLinkList pList);

#ifdef __cplusplus
}
#endif

