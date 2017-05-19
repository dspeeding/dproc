#include "basetype.h"
#include "ds/sq_list.h"

#define GetData(pList,i) ((unsigned char*)(pList)->list+(pList)->dataSize*(i))


/*************************************************************************
@Purpose :	re malloc list
@Param   :	pList		--	sqlist
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
static int ReMallocList(IN PSqList pList)
{
	void* p = NULL;
	//extend memory to double size
	p = Mem_Realloc(pList->list, 2 * pList->maxNum*pList->dataSize);
	if (p == NULL)
	{
		LOG_E("realloc list error,[%#x]", CF_GetLastError());
		return ERR_COMM_MEM_MALLOC;
	}

	pList->list = p;
	memset((unsigned char*)p + pList->maxNum*pList->dataSize, 0, pList->maxNum*pList->dataSize);
	pList->maxNum = 2 * pList->maxNum;
	
	return ERR_OK;
}

/*************************************************************************
@Purpose :	list init
@Param   :	ppList		--	sqlist 
@Param   :	dataSize	--	data size
@Param   :	maxNum		--	max num 
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int	InitList(IN PSqList* ppList, IN int dataSize, IN int maxNum)
{
	PSqList		pList;

	CHECK_POINTER(ppList, ERR_COMM_NULL_POINTER);
	if (maxNum <= 0)
	{
		LOG_E("param [maxNum] error,maxNum=[%#x]", maxNum);
		return ERR_COMM_PARAM;
	}

	*ppList = (PSqList)Mem_Malloc(sizeof(SqList));
	if (*ppList == NULL)
	{
		LOG_E("malloc *ppList error,[%#x]", CF_GetLastError());
		return ERR_COMM_MEM_MALLOC;
	}

	pList = *ppList;

	memset(pList, 0, sizeof(SqList));

	pList->maxNum = maxNum;
	pList->dataSize = dataSize;
	pList->curNum = 0;
	pList->list = Mem_Malloc(maxNum*dataSize);
	if (pList->list == NULL)
	{
		LOG_E("malloc list error,[%#x]", CF_GetLastError());
		return ERR_COMM_MEM_MALLOC;
	}

	memset(pList->list, 0, maxNum*dataSize);

	return ERR_OK;
}

/*************************************************************************
@Purpose :	clear list
@Param   :	pList		--	sqlist
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void ClearList(IN PSqList pList)
{
	if (pList == NULL)
	{
		return;
	}
	
	pList->curNum = 0;
	if (pList->list != NULL)
	{
		memset(pList->list, 0, pList->dataSize*pList->maxNum);
	}
}

/*************************************************************************
@Purpose :	destroy list, free mem
@Param   :	pList		--	sqlist
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void DestroyList(IN PSqList pList)
{
	if (pList == NULL)
	{
		return;
	}
	if (pList->list != NULL)
	{
		Mem_Free(pList->list);
		pList->list = NULL;
	}

	Mem_Free(pList);
	pList = NULL;
}

/*************************************************************************
@Purpose :	get list length
@Param   :	pList		--	sqlist
@Return  :	the length of the list
@Modify  :
@Remark  :
*************************************************************************/
int GetListLength(IN PSqList pList)
{
	return pList->curNum;
}

/*************************************************************************
@Purpose :	judge list is empty
@Param   :	pList		--	sqlist
@Return  :	1--empty  0--has data
@Modify  :
@Remark  :
*************************************************************************/
int IsListEmpty(IN PSqList pList)
{
	if (pList->curNum == 0)
	{
		return 1;
	}
	return 0;
}

/*************************************************************************
@Purpose :	get the nPos's data value
@Param   :	pList		--	sqlist
@Param   :	nPos		--	the data position [0-GetListLength(pList)-1]
@Param   :	pVal		--	the data
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int GetElem(IN PSqList pList, IN int nPos, OUT void* pVal)
{
	CHECK_POINTER(pList, ERR_COMM_NULL_POINTER);
	if (pList->list == NULL)
	{
		LOG_E("list is null");
		return ERR_COMM_NULL_POINTER;
	}

	if (nPos < 0 || nPos > (pList->curNum - 1))
	{
		LOG_E("pos out of index, pos=[%d]", nPos);
		return ERR_COMM_OUT_OF_INDEX;
	}

	memcpy(pVal, GetData(pList, nPos), pList->dataSize);

	return ERR_OK;
}

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
int TraverseList(IN PSqList pList, IN VisitPTR visit, IN int flag)
{
	int		i;
	int		nRet;

	CHECK_POINTER(pList, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(pList->list, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(visit, ERR_COMM_NULL_POINTER);

	for (i = 0; i < pList->curNum; i++)
	{
		nRet = visit(GetData(pList, i));
		if (flag == 1 && nRet != 0)
		{
			return nRet;
		}
	}

	return ERR_OK;
}

/*************************************************************************
@Purpose :	insert element to list specify by position
@Param   :	pList		--	sqlist
@Param   :	nPos		--	insert position [0 ~ GetListLength(pList)]
@Param   :	data		--	insert data
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :	
*************************************************************************/
int InsertList(IN PSqList pList, IN int nPos, IN void* data)
{
	int		i;
	int		nRet;

	CHECK_POINTER(pList, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(data, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(pList->list, ERR_COMM_NULL_POINTER);

	if (nPos < 0 || nPos > pList->curNum)
	{
		LOG_E("pos out of index, pos=[%d]", nPos);
		return ERR_COMM_OUT_OF_INDEX;
	}

	if (pList->curNum == pList->maxNum)
	{
		//remalloc bigger mem
		nRet = ReMallocList(pList);
		if (nRet != ERR_OK)
		{
			LOG_E("ReMallocList error,[%#x]", nRet);
			return nRet;
		}
	}

	for (i = pList->curNum - 1; i >= nPos; i--)
	{
		memcpy(GetData(pList, i + 1), GetData(pList, i), pList->dataSize);
	}

	memcpy(GetData(pList, nPos), data, pList->dataSize);
	pList->curNum++;

	return ERR_OK;
}

/*************************************************************************
@Purpose :	delete element from list specify by position
@Param   :	pList		--	sqlist
@Param   :	nPos		--	delete position [0 ~ GetListLength(pList)-1]
@Param   :	data		--	delete data
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int DeleteList(IN PSqList pList, IN int nPos, OUT void* data)
{
	int		i;

	CHECK_POINTER(pList, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(data, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(pList->list, ERR_COMM_NULL_POINTER);

	if (nPos < 0 || nPos > (pList->curNum - 1))
	{
		LOG_E("pos out of index, pos=[%d]", nPos);
		return ERR_COMM_OUT_OF_INDEX;
	}

	memcpy(data, GetData(pList, nPos), pList->dataSize);
	for (i = nPos + 1; i < pList->curNum; i++)
	{
		memcpy(GetData(pList, i - 1), GetData(pList, i), pList->dataSize);
	}
	memset(GetData(pList, pList->curNum - 1), 0, pList->dataSize);
	pList->curNum--;

	return ERR_OK;
}

/*************************************************************************
@Purpose :	update element from list specify by position
@Param   :	pList		--	sqlist
@Param   :	nPos		--	delete position [0 ~ GetListLength(pList)-1]
@Param   :	data		--	delete data
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int UpdateList(IN PSqList pList, IN int nPos, IN void* data)
{
	CHECK_POINTER(pList, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(data, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(pList->list, ERR_COMM_NULL_POINTER);

	if (nPos < 0 || nPos > pList->curNum - 1)
	{
		LOG_E("pos out of index, pos=[%d]", nPos);
		return ERR_COMM_OUT_OF_INDEX;
	}

	memcpy(GetData(pList, nPos), data, pList->dataSize);
	return ERR_OK;
}

/*************************************************************************
@Purpose :	sorter list
@Param   :	pList		--	sqlist
@Param   :	compare		--	the compare function
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int SortList(IN PSqList pList, IN ComparePTR compare)
{
	int				i, j;
	unsigned char*	pTemp = NULL;

	CHECK_POINTER(pList, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(compare, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(pList->list, ERR_COMM_NULL_POINTER);

	pTemp = (unsigned char*)Mem_Malloc(pList->dataSize);
	if (pTemp == NULL)
	{
		LOG_E("malloc pTemp error,[%#x]", CF_GetLastError());
		return ERR_COMM_MEM_MALLOC;
	}

	for (i = 0; i < pList->curNum; i++)
	{
		for (j = i + 1; j < pList->curNum; j++)
		{
			if (compare(GetData(pList, i), GetData(pList, j)) > 0)
			{
				memcpy(pTemp, GetData(pList, i), pList->dataSize);
				memcpy(GetData(pList, i), GetData(pList, j), pList->dataSize);
				memcpy(GetData(pList, j), pTemp, pList->dataSize);
			}
		}
	}

	Mem_Free(pTemp);
	return ERR_OK;
}



