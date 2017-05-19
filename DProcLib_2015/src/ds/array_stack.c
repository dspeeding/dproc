#include "basetype.h"


#define GetTopData(p, i) ((unsigned char*)((p)->pArray) + ((p)->nDataSize*(i)))

/*************************************************************************
@Purpose :	init stack
@Param   :	ppStack     -- stack pointer
@Param   :	nElemSize	-- element data size
@Param   :	nMaxNum		-- max num of stack
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int InitArrayStack(IN PArrayStack* ppStack, IN int nElemSize, IN int nMaxNum)
{
	PArrayStack		pStack;

	CHECK_POINTER(ppStack, ERR_COMM_NULL_POINTER);

	if (nMaxNum <= 0)
	{
		LOG_E("param [maxNum] error,maxNum=[%#x]", nMaxNum);
		return ERR_COMM_PARAM;
	}

	*ppStack = (PArrayStack)Mem_Malloc(sizeof(ArrayStack));
	if (*ppStack == NULL)
	{
		LOG_E("malloc *ppStack error,[%#x]", CF_GetLastError());
		return ERR_COMM_MEM_MALLOC;
	}
	pStack = *ppStack;

	memset(pStack, 0, sizeof(ArrayStack));

	pStack->pArray = (void *)Mem_Malloc(nMaxNum * nElemSize);
	if (pStack->pArray == NULL)
	{
		Mem_Free(*ppStack);
		LOG_E("malloc pArray error,[%#x]", CF_GetLastError());
		return ERR_COMM_MEM_MALLOC;
	}
	memset(pStack->pArray, 0, nMaxNum * nElemSize);

	pStack->nMaxNum = nMaxNum;
	pStack->nDataSize = nElemSize;
	pStack->nTop = 0;

	return ERR_OK;
}

/*************************************************************************
@Purpose :	judge is array stack is empty
@Param   :	pStack      -- stack pointer
@Return  :	0 -- is empty ,-1 -- not empty
@Modify  :
@Remark  :
*************************************************************************/
int IsEmptyArrayStack(IN PArrayStack pStack)
{
	CHECK_POINTER(pStack, 0);

	if (pStack->nTop == 0)
	{
		return 0;
	}
	return -1;
}

/*************************************************************************
@Purpose :	judge is array stack is full
@Param   :	pStack      -- stack pointer
@Return  :	0 -- is full ,-1 -- not full
@Modify  :
@Remark  :
*************************************************************************/
int IsFullArrayStack(IN PArrayStack pStack)
{
	CHECK_POINTER(pStack, 0);

	if (pStack->nTop == pStack->nMaxNum)
	{
		return 0;
	}
	return -1;
}

/*************************************************************************
@Purpose :	remalloc array stack 
@Param   :	pStack      -- stack pointer
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :	expend array to double size
*************************************************************************/
static int AgainMallocArrayStack(IN PArrayStack pStack)
{
	void*		pTempData;

	CHECK_POINTER(pStack, ERR_COMM_NULL_POINTER);

	pTempData = Mem_Realloc(pStack->pArray, 2 * pStack->nMaxNum * pStack->nDataSize);
	if (pTempData == NULL)
	{
		LOG_E("remalloc pArray error,[%#x]", CF_GetLastError());
		return ERR_COMM_MEM_MALLOC;
	}

	pStack->pArray = pTempData;
	memset(GetTopData(pStack, pStack->nMaxNum), 0, pStack->nMaxNum * pStack->nDataSize);

	pStack->nMaxNum = 2 * pStack->nMaxNum;

	return ERR_OK;
}

/*************************************************************************
@Purpose :	push data to stack
@Param   :	pStack      -- stack pointer
@Param   :	pVal		-- element which to push
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int PushArrayStack(IN PArrayStack pStack, IN void* pVal)
{
	int		nRet;

	CHECK_POINTER(pStack, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(pVal, ERR_COMM_NULL_POINTER);

	if (IsFullArrayStack(pStack) == 0)
	{
		//stack is full
		nRet = AgainMallocArrayStack(pStack);
		if (nRet != ERR_OK)
		{
			LOG_E("AgainMallocArrayStack err,[%#x]", nRet);
			return nRet;
		}
	}

	memcpy(GetTopData(pStack, pStack->nTop), pVal, pStack->nDataSize);
	pStack->nTop += 1;

	return ERR_OK;
}

/*************************************************************************
@Purpose :	pop data from stack
@Param   :	pStack      -- stack pointer
@Param   :	pVal		-- element buffer 
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int PopArrayStack(IN PArrayStack pStack, OUT void* pVal)
{
	CHECK_POINTER(pStack, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(pVal, ERR_COMM_NULL_POINTER);

	if (IsEmptyArrayStack(pStack) == 0)
	{
		LOG_E("stack is empty");
		return ERR_COMM_OUT_OF_INDEX;
	}

	pStack->nTop -= 1;

	memcpy(pVal, GetTopData(pStack, pStack->nTop), pStack->nDataSize);
	memset(GetTopData(pStack, pStack->nTop), 0, pStack->nDataSize);
	
	return ERR_OK;
}

/*************************************************************************
@Purpose :	get the top data from stack
@Param   :	pStack      -- stack pointer
@Param   :	pVal		-- element buffer
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int GetTopArrayStack(IN PArrayStack pStack, OUT void* pVal)
{
	CHECK_POINTER(pStack, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(pVal, ERR_COMM_NULL_POINTER);

	if (IsEmptyArrayStack(pStack) == 0)
	{
		LOG_E("stack is empty");
		return ERR_COMM_OUT_OF_INDEX;
	}

	memcpy(pVal, GetTopData(pStack, (pStack->nTop-1)), pStack->nDataSize);

	return ERR_OK;
}

/*************************************************************************
@Purpose :	destroy stack,free memory
@Param   :	pStack      -- stack pointer
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void DestroyArrayStack(IN PArrayStack pStack)
{
	if (pStack == NULL)
	{
		return;
	}

	if (pStack->pArray != NULL)
	{
		Mem_Free(pStack->pArray);
		pStack->pArray = NULL;
		pStack->nTop = 0;
	}

	Mem_Free(pStack);
	pStack = NULL;
}
