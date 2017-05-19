#include "basetype.h"
#include "ds/link_list.h"

/*************************************************************************
@Purpose :	list init
@Param   :	ppList		--	pList 
@Param   :	dataSize	--	data size
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int	InitLinkList(IN PLinkList* ppList, IN int dataSize)
{
	PLinkList	p;

	CHECK_POINTER(ppList, ERR_COMM_NULL_POINTER);

	if (dataSize < sizeof(int))
	{
		LOG_E("the dataSize must be bigger than [%d]", (int)sizeof(int));
		return ERR_COMM_PARAM;
	}

	*ppList = (PLinkList)Mem_Malloc(sizeof(LinkList) + dataSize);
	if ((*ppList) == NULL)
	{
		LOG_E("malloc node error");
		return ERR_COMM_MEM_MALLOC;
	}
	p = *ppList;

	p->dataSize = dataSize;
	p->data = (unsigned char*)p + sizeof(LinkList);
	memset(p->data, 0, dataSize);

	p->next = NULL;
	return ERR_OK;
}

/*************************************************************************
@Purpose :	clear list
@Param   :	pList		--	pList
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void ClearLinkList(IN PLinkList pList)
{
	PLinkList	p;

	if (pList == NULL)
	{
		return;
	}

	p = pList->next;
	while (p)
	{
		pList->next = p->next;
		Mem_Free(p);
		p = pList->next;
	}

	memset(pList->data, 0, sizeof(int));
}

/*************************************************************************
@Purpose :	destroy list, clean memory
@Param   :	pList		--	pList
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void DestroyLinkList(IN PLinkList pList)
{
	if (pList == NULL)
	{
		return;
	}

	ClearLinkList(pList);

	Mem_Free(pList);
	pList = NULL;
}

/*************************************************************************
@Purpose :	get the length of list
@Param   :	pList		--	pList
@Return  :	len
@Modify  :
@Remark  :
*************************************************************************/
int GetLinkListLength(IN PLinkList pList)
{
	int		nLen = 0;

	if (pList == NULL)
	{
		return 0;
	}

	memcpy(&nLen, pList->data, sizeof(int));
	return nLen;
}

/*************************************************************************
@Purpose :	get the value of list by position
@Param   :	pList		--	pList
@Param   :	nPos		--	the position
@Param   :	val			--	the out val
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int GetLinkListElem(IN PLinkList pList, IN int nPos, OUT void* val)
{
	int			i = 0;
	PLinkList	p = NULL;
	
	CHECK_POINTER(pList, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(val, ERR_COMM_NULL_POINTER);

	if ((nPos < 0) || (nPos > GetLinkListLength(pList)-1))
	{
		LOG_E("pos out of index, pos=[%d]", nPos);
		return ERR_COMM_OUT_OF_INDEX;
	}

	p = pList->next;

	i = 0;
	while ((i < nPos) && p)
	{
		p = p->next;
		i++;
	}

	memcpy(val, p->data, pList->dataSize);

	return ERR_OK;
}

/*************************************************************************
@Purpose :	insert val to list by position
@Param   :	pList		--	pList
@Param   :	nPos		--	the position [0 ~ len]
@Param   :	val			--	the out val
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int InsertLinkList(IN PLinkList pList, IN int nPos, IN void* val)
{
	int			i = 0;
	int			nLen = 0;
	PLinkList	p = NULL;
	PLinkList	q = NULL;

	CHECK_POINTER(pList, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(val, ERR_COMM_NULL_POINTER);

	nLen = GetLinkListLength(pList);

	if ((nPos < 0) || (nPos > nLen))
	{
		LOG_E("pos out of index, pos=[%d]", nPos);
		return ERR_COMM_OUT_OF_INDEX;
	}

	p = pList;
	i = 0;
	while ((i < nPos) && p)
	{
		p = p->next;
		i++;
	}

	q = (PLinkList)Mem_Malloc(sizeof(LinkList) + pList->dataSize);
	if (q == NULL)
	{
		LOG_E("malloc node error");
		return ERR_COMM_MEM_MALLOC;
	}
	memset(q, 0, sizeof(LinkList) + pList->dataSize);
	q->data = (unsigned char*)q + sizeof(LinkList);
	memcpy(q->data, val, pList->dataSize);
	q->next = p->next;
	p->next = q;

	nLen += 1;
	memcpy(pList->data, &nLen, sizeof(int));

	return ERR_OK;
}

/*************************************************************************
@Purpose :	delete element from list by position
@Param   :	pList		--	pList
@Param   :	nPos		--	the position [0 ~ len-1]
@Param   :	val			--	the out val
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int DeleteLinkList(IN PLinkList pList, IN int nPos, OUT void* val)
{
	int			i = 0;
	int			nLen = 0;
	PLinkList	p = NULL;
	PLinkList	q = NULL;

	CHECK_POINTER(pList, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(val, ERR_COMM_NULL_POINTER);

	nLen = GetLinkListLength(pList);

	if ((nPos < 0) || (nPos > (nLen - 1)))
	{
		LOG_E("pos out of index, pos=[%d]", nPos);
		return ERR_COMM_OUT_OF_INDEX;
	}

	i = 0;
	p = pList;
	while ((i < nPos) && p)
	{
		p = p->next;
		i++;
	}

	q = p->next;
	p->next = q->next;
	memcpy(val, q->data, pList->dataSize);
	Mem_Free(q);
	q = NULL;

	nLen -= 1;
	memcpy(pList->data, &nLen, sizeof(nLen));

	return ERR_OK;
}

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
int LocateLinkListElem(IN PLinkList pList, IN void* val, IN ComparePTR compare, OUT int* pnPos)
{
	int			i = 0;
	PLinkList	p = NULL;

	CHECK_POINTER(pList, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(val, ERR_COMM_NULL_POINTER);

	p = pList->next;
	i = 0;

	while (p)
	{
		if (compare(val, p->data) == 0)
		{
			break;
		}
		p = p->next;
		i++;
	}

	if (p == NULL)
	{
		LOG_E("not found ");
		return ERR_COMM_OUT_OF_INDEX;
	}

	*pnPos = i;
	return ERR_OK;
}

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
int TraverseLinkList(IN PLinkList pList, IN VisitPTR visit, IN int flag)
{
	int			nRet = 0;
	PLinkList	p = NULL;

	CHECK_POINTER(pList, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(visit, ERR_COMM_NULL_POINTER);

	p = pList->next;

	while (p)
	{
		nRet = visit(p->data);
		if (flag == 1 && nRet != 0)
		{
			return nRet;
		}

		p = p->next;
	}

	return ERR_OK;
}

/*************************************************************************
@Purpose :	update val to list by position
@Param   :	pList		--	pList
@Param   :	nPos		--	the position
@Param   :	val			--	the out val
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int UpdateLinkList(IN PLinkList pList, IN int nPos, IN void* val)
{
	int			i = 0;
	int			nLen = 0;
	PLinkList	p = NULL;

	CHECK_POINTER(pList, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(val, ERR_COMM_NULL_POINTER);

	nLen = GetLinkListLength(pList);

	if ((nPos < 0) || (nPos > (nLen - 1)))
	{
		LOG_E("pos out of index, pos=[%d]", nPos);
		return ERR_COMM_OUT_OF_INDEX;
	}

	p = pList->next;
	i = 0;
	while ((i < nPos) && p)
	{
		p = p->next;
		i++;
	}

	memcpy(p->data, val, pList->dataSize);

	return ERR_OK;
}

/*************************************************************************
@Purpose :	sorter list
@Param   :	pList		--	pList
@Param   :	compare		--	the compare function
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int SortLinkList(IN PLinkList pList, IN ComparePTR compare)
{
	PLinkList		p = NULL;
	PLinkList		q = NULL;
	unsigned char*	temp = NULL;

	CHECK_POINTER(pList, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(compare, ERR_COMM_NULL_POINTER);

	temp = Mem_Malloc(pList->dataSize);
	if (temp == NULL)
	{
		LOG_E("malloc node error");
		return ERR_COMM_MEM_MALLOC;
	}

	for (p = pList->next; p; p = p->next)
	{
		for (q = p->next; q; q = q->next)
		{
			if (compare(p->data, q->data) > 0)
			{
				memcpy(temp, p->data, pList->dataSize);
				memcpy(p->data, q->data, pList->dataSize);
				memcpy(q->data, temp, pList->dataSize);
			}
		}
	}

	Mem_Free(temp);

	return ERR_OK;
}

/*************************************************************************
@Purpose :	reverse list
@Param   :	pList		--	pList
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int ReverseLinkList(IN PLinkList pList)
{
	PLinkList p = NULL;
	PLinkList q = NULL;
	PLinkList s = NULL;

	CHECK_POINTER(pList, ERR_COMM_NULL_POINTER);

	p = pList->next;
	if (p == NULL)
	{
		return ERR_OK;
	}

	q = p->next;
	while (q->next)
	{
		s = q->next;
		q->next = p;
		p = q;
		q = s;
	}

	q->next = p;
	pList->next->next = NULL;
	pList->next = q;

	return ERR_OK;
}